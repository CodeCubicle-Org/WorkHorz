#include "whz_yubikey_verifier.hpp"
#include "whz_ssl_connection.hpp"
#include <format>
#include <vector>

namespace whz {

yubikey_verifier::yubikey_verifier(
    std::string_view client_id,
    std::string_view api_key,
    boost::asio::io_context& io_context,
    boost::asio::ssl::context& ssl_context)
    : client_id_(client_id)
    , api_key_(api_key)
    , io_context_(io_context)
    , ssl_context_(ssl_context)
    , logger_("yubikey_verifier") {
    
    if (ykclient_init(&client_) != YKCLIENT_OK) {
        throw std::runtime_error("Failed to initialize YubiKey client");
    }
    
    ykclient_set_client(client_, 
        std::stoi(std::string(client_id)).client_b64_key(std::string(api_key).c_str()));
}

yubikey_verifier::~yubikey_verifier() {
    if (client_) {
        ykclient_done(&client_);
    }
}

auto yubikey_verifier::verify_otp(std::string_view otp) 
    -> std::future<std::expected<bool, error>> {
    
    return std::async(std::launch::async, [this, otp = std::string(otp)]() {
        if (otp.length() != YUBIKEY_OTP_LENGTH) {
            return std::expected<bool, error>(
                std::unexpected(error::invalid_otp));
        }

        std::atomic<bool> completed = false;
        std::vector<std::future<std::expected<bool, error>>> futures;
        futures.reserve(API_HOSTS.size());

        // Launch parallel requests to all API servers
        for (const auto& host : API_HOSTS) {
            futures.push_back(
                std::async(std::launch::async,
                    [this, &completed, &otp, host]() {
                        return verify_single_server(otp, host, completed);
                    }
                )
            );
        }

        // Wait for first successful response or all failures
        std::expected<bool, error> final_result{std::unexpected(error::network_error)};
        bool got_success = false;

        for (auto& future : futures) {
            if (auto result = future.get()) {
                completed = true;
                final_result = result;
                got_success = true;
                break;
            }
        }

        return got_success ? final_result : 
            std::expected<bool, error>(std::unexpected(error::network_error));
    });
}

auto yubikey_verifier::verify_single_server(
    std::string_view otp,
    std::string_view host,
    std::atomic<bool>& completed) 
    -> std::expected<bool, error> {
    
    try {
        // Early exit if another request already succeeded
        if (completed) {
            return std::unexpected(error::network_error);
        }

        ssl_socket socket(io_context_, ssl_context_);
        auto request = create_request(otp, host);  // Pass host to create_request
        
        // Connect to specific Yubico API server
        boost::asio::ip::tcp::resolver resolver(io_context_);
        auto endpoints = resolver.resolve(host, std::to_string(API_PORT));
        
        // Set short timeout for connect and operations
        socket.lowest_layer().set_option(
            boost::asio::socket_base::timeout(
                boost::posix_time::seconds(2)));

        boost::asio::connect(socket.lowest_layer(), endpoints);
        socket.handshake(boost::asio::ssl::stream_base::client);

        boost::asio::write(socket, boost::asio::buffer(request.to_string()));
        
        whz::reply reply;
        std::array<char, 1024> buffer;
        size_t bytes_transferred = socket.read_some(
            boost::asio::buffer(buffer));
        
        reply.parse(std::string_view(buffer.data(), bytes_transferred));
        return parse_response(reply);
    }
    catch (const std::exception& e) {
        logger_.error("Verification failed for {}: {}", host, e.what());
        return std::unexpected(error::network_error);
    }
}

auto yubikey_verifier::create_request(std::string_view otp, std::string_view host) -> whz::request {
    whz::request req;
    req.method = "GET";
    req.uri = std::format("{}?id={}&otp={}&nonce={}", 
        API_PATH, client_id_, otp, 
        ykclient_get_last_nonce(client_));
    req.headers["Host"] = std::string(host);
    req.headers["User-Agent"] = "WorkHorz/1.0";
    return req;
}

auto yubikey_verifier::parse_response(const whz::reply& response) 
    -> std::expected<bool, error> {
    const auto& body = response.content;
    
    if (body.find("status=OK") != std::string::npos) {
        return true;
    }
    else if (body.find("status=REPLAYED_OTP") != std::string::npos) {
        return std::unexpected(error::replayed_otp);
    }
    else if (body.find("status=BAD_SIGNATURE") != std::string::npos) {
        return std::unexpected(error::bad_signature);
    }
    
    return std::unexpected(error::api_error);
}

auto yubikey_verifier::extract_identity(std::string_view otp) 
    -> std::expected<std::string, error> {
    if (otp.length() != YUBIKEY_OTP_LENGTH) {
        return std::unexpected(error::invalid_otp);
    }
    
    // The first 12 characters of a YubiKey OTP contain the identity
    // This is in ModHex encoding, but we'll return it as-is since
    // that's the canonical form of the identity
    return std::string(otp.substr(0, YUBIKEY_ID_LENGTH));
}

} // namespace whz
