#include <filesystem>
#include <print>

#include "whz_server.hpp"

auto main() -> int {
  std::filesystem::path path{"/tmp/whz"};

  whz::server s{"0.0.0.0", 8080, path, 1};

  s.listen_and_serve();
  return 0;
}
