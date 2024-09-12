cmake_minimum_required(VERSION 3.28)

find_program(CCACHE_EXECUTABLE ccache)
if (CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
endif()

set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

project(WorkHorz VERSION 0.0.1 DESCRIPTION "WorkHorz" LANGUAGES CXX)

find_package(OpenSSL CONFIG REQUIRED)
find_package(boost_asio REQUIRED CONFIG)
find_package(fmt CONFIG REQUIRED)
find_package(simdjson CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_package(lua REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(Taskflow CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(ftxui CONFIG REQUIRED)

add_executable(whz-core src/whz-core.cpp
               src/whz_common.cpp
               src/whz_connection.cpp
               src/whz_io_context_pool.cpp
               src/whz_ssl_connection.cpp
               src/whz_request.cpp
               src/whz_request_handler.cpp
               src/whz_request_parser.cpp
               src/whz_server.cpp
               src/whz_utils.cpp
               src/whz_resources.cpp
               src/whz_config.cpp
               src/whz_http_routing.cpp
               src/whz_LUA_core.cpp
               src/whz_LUA_api.cpp
               src/whz_renderer.cpp
)

# set_property(TARGET whz-core PROPERTY CXX_STANDARD 23)

find_path(QUILL_INCLUDE_DIRS "quill/Backend.h")
target_include_directories(whz-core PRIVATE ${QUILL_INCLUDE_DIRS})

find_path(RANG_INCLUDE_DIRS "rang.hpp")
target_include_directories(whz-core PRIVATE ${RANG_INCLUDE_DIRS})

target_include_directories(whz-core PRIVATE ${LUA_INCLUDE_DIRS})

target_link_libraries(whz-core PRIVATE
  Boost::asio
  fmt::fmt
  OpenSSL::SSL
  OpenSSL::Crypto
  simdjson::simdjson
  Catch2::Catch2 Catch2::Catch2WithMain
  Taskflow::Taskflow
  sol2
  ${LUA_LIBRARIES}
  CLI11::CLI11
  #fmt::fmt-header-only
  fmt::fmt
  ftxui::dom ftxui::screen ftxui::component
)

if (BUILD_TESTING)
  message("Building tests")
  include(CTest)
  include(Catch)

  add_executable(sample_tests tests/factorial.cpp)
  target_link_libraries(sample_tests PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)

  catch_discover_tests(sample_tests)
endif ()
