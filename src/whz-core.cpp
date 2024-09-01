#include <filesystem>
#include <print>

#include "quill/LogMacros.h"
#include "quill/Logger.h"

#include "whz_quill_wrapper.hpp"
#include "whz_server.hpp"

extern quill::Logger* logger;

auto main() -> int {
  setup_quill();

  LOG_INFO(logger, "Starting WHZ");

  std::filesystem::path path{"/tmp/whz"};

  whz::server s{"0.0.0.0", 8080, path, 1};

  s.listen_and_serve();
  return 0;
}
