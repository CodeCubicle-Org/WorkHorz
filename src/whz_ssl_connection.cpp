#include "whz_ssl_connection.hpp"
#include "whz_request_handler.hpp"

#include <boost/system/detail/error_code.hpp>

namespace whz {

ssl_connection::ssl_connection(
    boost::asio::ssl::stream<tcp::socket> socket,
    whz::request_handler& request_handler)
    : socket_(std::move(socket)), request_handler_(request_handler) {}

auto ssl_connection::start() -> void {
  do_handshake();
}

auto ssl_connection::do_handshake() -> void {
  auto self(shared_from_this());
  socket_.async_handshake(
      boost::asio::ssl::stream_base::server,
      [this, self](const boost::system::error_code& error) {
        if (!error) {
          do_read();
        }
      });
}

auto ssl_connection::do_read() -> void {
  auto self(shared_from_this());
  socket_.async_read_some(
      boost::asio::buffer(buffer_),
      [this, self](const boost::system::error_code& ec, std::size_t length) {
        if (!ec) {
          do_write(length);
        }
      });
}

auto ssl_connection::do_write(std::size_t length) -> void {
  auto self(shared_from_this());
  boost::asio::async_write(
      socket_,
      boost::asio::buffer(buffer_, length),
      [this, self](const boost::system::error_code& ec, std::size_t) {
        if (!ec) {
          do_read();
        }
      });
}

}; // namespace whz
