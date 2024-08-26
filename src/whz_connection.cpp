#include "whz_connection.hpp"

#include <system_error>
#include <boost/asio/impl/write.hpp>
#include "whz_request_handler.hpp"
#include "whz_request_parser.hpp"

namespace whz {
connection::connection(
    boost::asio::ip::tcp::socket socket, whz::request_handler& handler)
    : socket_(std::move(socket)), request_handler_(handler) {}

auto connection::start() -> void {
  do_read();
}

auto connection::do_read() -> void {
  auto self(shared_from_this());

  socket_.async_read_some(
      boost::asio::buffer(buffer_),
      [this, self](std::error_code ec, std::size_t /*bytes_transferred*/) {
        if (!ec) {
          whz::result_type result;
          std::tie(result, std::ignore) = request_parser_.parse(
              request_,
              buffer_.begin(),
              buffer_.end()); // TODO(bc): This should be thoroughly checked

          if (result == whz::result_type::good) {
            request_handler_.handle_request(request_, reply_);
            do_write();
          } else if (result == whz::result_type::bad) {
            reply_ = reply::stock_reply(reply::bad_request);
            do_write();
          } else {
            do_read();
          }
        }
      });
}

auto connection::do_write() -> void {
  auto self(shared_from_this());
  boost::asio::async_write(
      socket_,
      reply_.to_buffers(),
      [this, self](std::error_code /*ec*/, std::size_t /*s*/) -> void {
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
      });
}
}; // namespace whz
