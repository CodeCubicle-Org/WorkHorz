#pragma once

#include <cstdint>
#include <memory>
#include <boost/asio.hpp>

#include "whz_common.hpp"
#include "whz_request_handler.hpp"
#include "whz_request_parser.hpp"

namespace whz {

// NOTE(bc): These should have a well defined place
class connection : public std::enable_shared_from_this<connection> {
 public:
  connection(const connection&) = delete;
  connection& operator=(const connection&) = delete;
  connection(const connection&&) = delete;
  connection& operator=(const connection&&) = delete;
  ~connection() = default;

  explicit connection(
      boost::asio::ip::tcp::socket socket, request_handler& handler);

  auto start() -> void;

 private:
  auto do_read() -> void;
  auto do_write() -> void;

  boost::asio::ip::tcp::socket socket_;
  whz::request_handler& request_handler_;
  std::array<std::uint8_t, 8192>
      buffer_{}; // NOTE(bc): Check this. Can we use span?
  request request_;
  whz::request_parser request_parser_;
  whz::reply reply_;
};

using http_connection_ptr = std::shared_ptr<connection>;

}; // namespace whz
