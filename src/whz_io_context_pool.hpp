#pragma once

#include <list>
#include <memory>
#include <vector>

#include <boost/asio.hpp>

namespace whz {
class io_context_pool {
 public:
  explicit io_context_pool(std::size_t pool_size);

  io_context_pool(const io_context_pool&) = delete;
  io_context_pool(const io_context_pool&&) = delete;
  io_context_pool& operator=(const io_context_pool&) = delete;
  io_context_pool& operator=(const io_context_pool&&) = delete;
  ~io_context_pool() = default;

  auto run() -> void;
  auto stop() -> void;
  auto get_io_context() -> boost::asio::io_context&;

 private:
  using io_context_ptr = std::shared_ptr<boost::asio::io_context>;
  using io_context_work =
      boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

  std::vector<io_context_ptr> io_contexts_;

  // NOTE: This looks like it can be held in a different container
  std::list<io_context_work> work_;

  std::size_t next_io_context;
};
}; // namespace whz
