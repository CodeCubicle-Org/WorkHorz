cmake_minimum_required(VERSION 3.28)

find_program(CCACHE_EXECUTABLE ccache)
if (CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
endif()

set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

project(WorkHorz VERSION 0.0.1
        DESCRIPTION "WorkHorz"
        LANGUAGES CXX)

find_package(Catch2 CONFIG REQUIRED)
find_package(OpenSSL CONFIG REQUIRED)
find_package(boost_asio REQUIRED CONFIG)
find_package(fmt CONFIG REQUIRED)
find_package(simdjson CONFIG REQUIRED)

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
               src/whz_config.cpp)

target_link_libraries(whz-core PRIVATE
  Boost::asio
  fmt::fmt
  OpenSSL::SSL
  OpenSSL::Crypto
  simdjson::simdjson
  Catch2::Catch2 Catch2::Catch2WithMain
)

if (BUILD_TESTING)
  message("Building tests")
  include(CTest)
  include(Catch)

  add_executable(sample_tests tests/factorial.cpp)
  target_link_libraries(sample_tests PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)

  catch_discover_tests(sample_tests)
endif ()
