#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <system_error>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/context.hpp>

#include "whz_io_context_pool.hpp"
#include "whz_request_handler.hpp"
#include "whz_quill_wrapper.hpp"

namespace whz {

using boost::asio::ip::tcp;

using ssl_socket = boost::asio::ssl::stream<tcp::socket>;

// main server takes the number of threads it should run in
// In the future it can also support the tls handshake and read timeouts
class server {
 public:
  explicit server(
      std::string_view address,
      std::uint32_t port,
      std::filesystem::path documents_root,
      std::size_t io_pool_size);
  server(const server&) = delete;
  server(const server&&) = delete;
  server& operator=(const server&) = delete;
  server& operator=(const server&&) = delete;

  auto listen_and_serve() -> std::optional<std::error_code>;

  auto listen_and_serve(boost::asio::ssl::context& tls_context)
      -> std::optional<std::error_code>;

  auto listen_and_serve(
      boost::asio::ssl::context& tls_context,
      std::string_view address,
      std::string_view port) -> std::error_code;

  auto run() -> void;

 private:
  [[nodiscard]] auto bind_and_listen() -> std::optional<std::error_code>;
  [[nodiscard]] auto bind_and_listen_http() -> std::optional<std::error_code>;
  auto start_accept(boost::asio::ssl::context& tls_context) -> void;
  auto start_accept_http() -> void;

  auto do_accept() -> void;
  auto do_accept_http() -> void;
  auto do_await_stop() -> void;

  std::string_view address_;
  std::uint32_t port_;
  // std::filesystem::path documents_root_;
  std::size_t io_pool_size_;
  io_context_pool io_context_pool_;

  boost::asio::signal_set signals_;

  // boost::asio::signal_set signals;
  boost::asio::ip::tcp::acceptor acceptor_;

  // NOTE(bc): Need to be checked, should be a duration
  // std::chrono::milliseconds tls_handshake_timeout_;
  // std::chrono::milliseconds read_timeout_;
  whz::request_handler request_handler_;
  whz::whz_qlogger _qlogger;
};

}; // namespace whz
