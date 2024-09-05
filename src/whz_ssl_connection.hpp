#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "whz_common.hpp"
#include "whz_request_handler.hpp"
#include "whz_request_parser.hpp"

#include <array>
#include <memory>

namespace whz {

using boost::asio::ip::tcp;

class ssl_connection : public std::enable_shared_from_this<ssl_connection> {
 public:
  ssl_connection(
      boost::asio::ssl::stream<tcp::socket> socket,
      whz::request_handler& handler);

  auto start() -> void;

 private:
  auto do_handshake() -> void;
  auto do_read() -> void;
  auto do_write(std::size_t length) -> void;

  boost::asio::ssl::stream<tcp::socket> socket_;
  whz::request_handler& request_handler_;
  whz::request_parser request_parser_;
  std::array<char, 8192> buffer_;
  whz::request request_;
  whz::reply reply;
};

using connection_ptr = std::shared_ptr<ssl_connection>;
} // namespace whz
