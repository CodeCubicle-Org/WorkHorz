#include "whz_server.hpp"
#include <optional>
#include <string>
#include <boost/asio/ip/basic_resolver_query.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/detail/error_code.hpp>
#include "whz_connection.hpp"
#include "whz_io_context_pool.hpp"
#include "whz_ssl_connection.hpp"

namespace whz {

server::server(
    std::string_view address,
    std::uint32_t port,
    std::filesystem::path documents_root,
    std::size_t io_pool_size)
    : address_(address),
      port_(port),
      io_pool_size_(io_pool_size),
      io_context_pool_(io_pool_size_),
      signals_(io_context_pool_.get_io_context()),
      acceptor_(io_context_pool_.get_io_context()),
      request_handler_(std::move(documents_root)) {
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
#if defined(SIGQUIT)
  signals_.add(SIGQUIT);
#endif

  do_await_stop();
}

auto server::listen_and_serve(boost::asio::ssl::context& tls_context)
    -> std::optional<std::error_code> {
  if (bind_and_listen()) {
    return std::make_error_code(std::errc::io_error);
  }

  start_accept(tls_context);

  io_context_pool_.run();

  return std::nullopt;
}

auto server::listen_and_serve() -> std::optional<std::error_code> {
  if (bind_and_listen_http()) {
    return std::make_error_code(std::errc::io_error);
  }

  start_accept_http();

  io_context_pool_.run();
  return std::nullopt;
}

auto server::bind_and_listen_http() -> std::optional<std::error_code> {
  boost::asio::ip::tcp::resolver resolver(acceptor_.get_executor());
  tcp::endpoint endpoint =
      *resolver.resolve(address_, std::to_string(port_)).begin();

  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  do_accept_http();
  return std::nullopt;
}

auto server::bind_and_listen() -> std::optional<std::error_code> {
  boost::asio::ip::tcp::resolver resolver(io_context_pool_.get_io_context());
  boost::asio::ip::tcp::resolver res(acceptor_.get_executor());
  tcp::endpoint endpoint =
      *res.resolve(address_, std::to_string(port_)).begin();

  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  do_accept();

  return std::nullopt; // NOTE(bc): HACK
}

auto server::do_accept_http() -> void {
  acceptor_.async_accept(
      io_context_pool_.get_io_context(),
      [this](
          boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (!acceptor_.is_open()) {
          return;
        }
        if (!ec) {
          std::make_shared<whz::connection>(std::move(socket), request_handler_)
              ->start();
        }
        do_accept_http();
      });
}

auto server::do_accept() -> void {
  // NOTE(bc): prefer not to use error_code from boost here
  acceptor_.async_accept(
      io_context_pool_.get_io_context(),
      [this](
          boost::system::error_code /*ec*/,
          boost::asio::ip::tcp::socket socket) -> void {
        if (!acceptor_.is_open())
          return;

        std::make_shared<whz::connection>(std::move(socket), request_handler_)
            ->start();
        do_accept();
      });
}

auto server::do_await_stop() -> void {
  signals_.async_wait([this](boost::system::error_code /*ec*/, int /*signo*/) {
    io_context_pool_.stop();
  });
}

auto server::start_accept_http() -> void {
  // Is the acceptor listening for new connections?
  if (!acceptor_.is_open()) {
    return;
  }

  acceptor_.async_accept(
      io_context_pool_.get_io_context(),
      [this](
          boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (!ec) {
          std::make_shared<whz::connection>(std::move(socket), request_handler_)
              ->start();
        }
        do_accept_http();
      });
}

auto server::start_accept(boost::asio::ssl::context& tls_context) -> void {
  if (!acceptor_.is_open()) {
    return;
  }

  acceptor_.async_accept(
      io_context_pool_.get_io_context(),
      [this, &tls_context](const std::error_code& error, tcp::socket socket) {
        if (!acceptor_.is_open()) {
          return;
        }

        if (!error) {
          std::make_shared<whz::ssl_connection>(
              boost::asio::ssl::stream<tcp::socket>(
                  std::move(socket), tls_context),
              request_handler_)
              ->start();
        }
        start_accept(tls_context);
      });
}

}; // namespace whz
