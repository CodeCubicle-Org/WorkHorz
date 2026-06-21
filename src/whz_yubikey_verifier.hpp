#pragma once

#include <string>
#include <string_view>
#include <future>
#include <expected>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <ykclient.h>
#include "whz_quill_wrapper.hpp"
#include "whz_request.hpp"
#include "whz_reply.hpp"
#include <array>
#include <atomic>

namespace whz {

class yubikey_verifier {
public:
    // Error types that can occur during verification
    enum class error {
        invalid_otp,
        network_error,
        bad_signature,
        api_error,
        replayed_otp
    };

    explicit yubikey_verifier(
        std::string_view client_id,
        std::string_view api_key,
        boost::asio::io_context& io_context,
        boost::asio::ssl::context& ssl_context);
    
    ~yubikey_verifier();

    // Async verification of YubiKey OTP
    auto verify_otp(std::string_view otp) 
        -> std::future<std::expected<bool, error>>;

    // Extract the unique identity from a YubiKey OTP
    static auto extract_identity(std::string_view otp) -> std::expected<std::string, error>;

private:
    static constexpr size_t YUBIKEY_OTP_LENGTH = 44;
    static constexpr size_t YUBIKEY_ID_LENGTH = 12;

    auto create_request(std::string_view otp, std::string_view host) -> whz::request;
    auto parse_response(const whz::reply& response) -> std::expected<bool, error>;
    
    ykclient_t* client_;
    std::string client_id_;
    std::string api_key_;
    boost::asio::io_context& io_context_;
    boost::asio::ssl::context& ssl_context_;
    whz::whz_qlogger logger_;
    
    static constexpr std::string_view API_PATH = "/wsapi/2.0/verify";
    static constexpr int API_PORT = 443;

    static constexpr std::array<std::string_view, 5> API_HOSTS = {
        "api.yubico.com",
        "api2.yubico.com",
        "api3.yubico.com",
        "api4.yubico.com",
        "api5.yubico.com"
    };
    
    auto verify_single_server(
        std::string_view otp, 
        std::string_view host,
        std::atomic<bool>& completed) 
        -> std::expected<bool, error>;
};

} // namespace whz
