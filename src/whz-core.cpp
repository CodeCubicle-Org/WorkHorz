#include <filesystem>
#include <print>
#include <optional>

#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "CLI/CLI.hpp"

#include "whz_quill_wrapper.hpp"
#include "whz_server.hpp"

extern quill::Logger* logger;

auto main(int argc, char** argv) -> int {
  setup_quill();

  CLI::App app {"WorkHorz server"};
  argv = app.ensure_utf8(argv);

  std::string config_path;
  [[maybe_unused]] CLI::Option *opt = app.add_option("-c, --config,config", config_path, "Path to WorkHorz config file");

  CLI11_PARSE(app, argc, argv);

  if (config_path.empty()) {
    LOG_INFO(logger, "Using default config");
  }

  LOG_INFO(logger, "Starting WHZ");

  std::filesystem::path path{"/tmp/whz"};

  whz::server s{"0.0.0.0", 8080, path, 1};

  s.listen_and_serve();
  return 0;
}
