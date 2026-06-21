// jira_api.cpp
#include "whz_jira_wrapper.hpp"
#include <boost/beast/core/base64.hpp>

namespace whz {

JiraAPI::JiraAPI(Config config) : config_(std::move(config)) {
    // Create base64 auth header
    std::string auth_string = config_.username + ":" + config_.api_token;
    auth_header_ = "Basic " + boost::beast::base64_encode(auth_string);
    
    init_session();
}

void JiraAPI::init_session() {
    boost::system::error_code ec;
    
    session_ = std::make_unique<nghttp2::asio_http2::client::session>(
        io_context_,
        config_.base_url,
        config_.port,
        ec
    );

    if (ec) {
        throw std::runtime_error("Failed to create HTTP/2 session: " + ec.message());
    }
}

std::future<simdjson::dom::element> JiraAPI::make_request(
    const std::string& endpoint,
    const std::string& method,
    const std::optional<std::string>& data) {

    return std::async(std::launch::async, [this, endpoint, method, data]() {
        std::promise<simdjson::dom::element> promise;

        boost::system::error_code ec;
        auto req = session_->submit(ec, method, endpoint);

        if (ec) {
            throw std::runtime_error("Failed to submit request: " + ec.message());
        }

        req->header("Authorization", auth_header_);
        req->header("Content-Type", "application/json");

        if (data) {
            req->write_payload(data.value(), ec);
        }

        std::string response_body;
        req->on_response([&response_body](const nghttp2::asio_http2::client::response& res) {
            res.on_data([&response_body](const uint8_t* data, size_t len) {
                response_body.append(reinterpret_cast<const char*>(data), len);
            });
        });

        io_context_.run();

        simdjson::dom::element json_result;
        auto error = json_parser_.parse(response_body).get(json_result);
        
        if (error) {
            throw std::runtime_error("Failed to parse JSON response");
        }

        return json_result;
    });
}

template<JSONSerializable T>
std::future<T> JiraAPI::get_resource(const std::string& endpoint) {
    return std::async(std::launch::async, [this,