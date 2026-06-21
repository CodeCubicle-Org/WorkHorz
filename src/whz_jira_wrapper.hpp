// jira_api.hpp
#pragma once

#include <string>
#include <memory>
#include <future>
#include <concepts>
#include <nghttp2/asio_http2_client.h>
#include <simdjson.h>

namespace whz {

// Concept for JSON serializable types
template<typename T>
concept JSONSerializable = requires(T t, simdjson::dom::element el) {
    { t.to_json() } -> std::convertible_to<std::string>;
    { T::from_json(el) } -> std::convertible_to<T>;
};

class JiraAPI {
public:
    struct Config {
        std::string base_url;
        std::string username;
        std::string api_token;
        uint16_t port{443};
    };

    explicit JiraAPI(Config config);
    ~JiraAPI() = default;

    // Generic CRUD operations
    template<JSONSerializable T>
    std::future<T> get_resource(const std::string& endpoint);

    template<JSONSerializable T>
    std::future<T> create_resource(const std::string& endpoint, const T& data);

    template<JSONSerializable T>
    std::future<T> update_resource(const std::string& endpoint, const T& data);

    std::future<bool> delete_resource(const std::string& endpoint);

    // Specific Jira operations
    std::future<simdjson::dom::element> get_issue(const std::string& issue_key);
    std::future<simdjson::dom::element> create_issue(const std::string& issue_data);
    std::future<simdjson::dom::element> search_issues(const std::string& jql);

private:
    Config config_;
    simdjson::dom::parser json_parser_;
    boost::asio::io_context io_context_;
    std::unique_ptr<nghttp2::asio_http2::client::session> session_;
    std::string auth_header_;

    void init_session();
    std::future<simdjson::dom::element> make_request(
        const std::string& endpoint,
        const std::string& method,
        const std::optional<std::string>& data = std::nullopt
    );
};

} // namespace whz