#include "whz_io_context_pool.hpp"
#include <cstddef>

#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <fmt/printf.h>

#include <thread>

namespace whz {
io_context_pool::io_context_pool(std::size_t pool_size) : next_io_context(0) {
  if (pool_size == 0) {
    fmt::printf("io_context_pol size is 0");
    exit(1);
  }

  for (std::size_t i = 0; i < pool_size; ++i) {
    io_context_ptr io_context(
        new boost::asio::io_context); // NOTE(bc): Can the new be removed
    io_contexts_.emplace_back(io_context);
    work_.emplace_back(boost::asio::make_work_guard(*io_context));
  }
}

auto io_context_pool::run() -> void {
  std::vector<std::jthread> threads;
  threads.reserve(io_contexts_.size());

  for (const auto& context : io_contexts_) {
    threads.emplace_back([context] { context->run(); });
  }
}

auto io_context_pool::stop() -> void {
  for (const auto& context : io_contexts_) {
    context->stop();
  }
}

auto io_context_pool::get_io_context() -> boost::asio::io_context& {
  boost::asio::io_context& io_context = *io_contexts_[next_io_context];
  ++next_io_context;
  if (next_io_context == io_contexts_.size()) {
    next_io_context = 0;
  }
  return io_context;
}
}; // namespace whz
