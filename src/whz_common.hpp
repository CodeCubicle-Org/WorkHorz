#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include "whz_quill_wrapper.hpp"

namespace whz {
struct header {
  std::string name;
  std::string value;
};

struct request {
  std::string method;
  std::string uri;
  std::uint8_t http_version_major;
  std::uint8_t http_version_minor;
  std::vector<header> headers;
};

struct reply {
  enum status_type : std::uint16_t {
    ok = 200,
    created = 201,
    accepted = 202,
    no_content = 204,
    multiple_choices = 300,
    moved_permanently = 301,
    moved_temporarily = 302,
    not_modified = 304,
    bad_request = 400,
    unauthorized = 401,
    forbidden = 403,
    not_found = 404,
    internal_server_error = 500,
    not_implemented = 501,
    bad_gateway = 502,
    service_unavailable = 503
  } status;

  std::vector<header> headers;
  std::string content;
  std::vector<boost::asio::const_buffer> to_buffers();
  static auto stock_reply(reply::status_type status) -> reply;
};

namespace status_strings {
auto to_buffer(whz::reply::status_type status) -> boost::asio::const_buffer;
}

auto reply_to_string(whz::reply::status_type status) -> std::string;

}; // namespace whz
