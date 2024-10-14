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
find_package(boost_locale REQUIRED CONFIG)
find_package(simdutf REQUIRED CONFIG)
find_package(fmt CONFIG REQUIRED)
find_package(simdjson CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_package(lua REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(Taskflow CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(ftxui CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED) # for libsodium
find_package(libzip CONFIG REQUIRED)
find_package(liblzma CONFIG REQUIRED)
find_package(SQLiteCpp REQUIRED)
find_package(unofficial-nayuki-qr-code-generator CONFIG REQUIRED)
find_package(PNG REQUIRED)
find_package(LibArchive REQUIRED)

find_path(QUILL_INCLUDE_DIRS "quill/Backend.h" "quill/Frontend.h" "quill/LogMacros.h" "quill/whz_logger.h" "quill/sinks/ConsoleSink.h")

add_subdirectory(external/bustache)

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
               src/whz_templateCache.cpp
               src/whz_quill_wrapper.cpp
               src/LocalizationManager.cpp
               src/whz_encryption.cpp
               src/whz_template_parser.cpp
               src/whz_datacompression.cpp
               src/whz_vcard.cpp
               src/whz_qrcode_generator.cpp
               #src/whz_templating.cpp
)

# set_property(TARGET whz-core PROPERTY CXX_STANDARD 23)


find_path(QUILL_INCLUDE_DIRS "quill/Backend.h")
find_path(RANG_INCLUDE_DIRS "rang.hpp")
find_path(RAPIDHASH_INCLUDE_DIRS "rapidhash.h")

target_include_directories(whz-core PRIVATE ${QUILL_INCLUDE_DIRS})
target_include_directories(whz-core PRIVATE ${RANG_INCLUDE_DIRS})
target_include_directories(whz-core PRIVATE ${LUA_INCLUDE_DIRS})
target_include_directories(whz-core PRIVATE ${RAPIDHASH_INCLUDE_DIRS})


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
  fmt::fmt
  ftxui::dom ftxui::screen ftxui::component
  simdutf::simdutf
  Boost::locale
  unofficial-sodium::sodium
  bustache::bustache
  libzip::zip
  liblzma::liblzma
  SQLiteCpp
  unofficial::nayuki-qr-code-generator::nayuki-qr-code-generator
  PNG::PNG
  LibArchive::LibArchive
)

if (BUILD_TESTING)
  message("Building tests")
  include(CTest)
  include(Catch)

  add_executable(sample_tests tests/factorial.cpp)
  target_link_libraries(sample_tests PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)

  catch_discover_tests(sample_tests)
endif ()

if (BUILD_DOC)
  find_package(Doxygen)
  if (DOXYGEN_FOUND)
    set(abs_top_srcdir ${CMAKE_CURRENT_SOURCE_DIR})
    set(abs_top_builddir ${CMAKE_CURRENT_BINARY_DIR})
    message(${CMAKE_CURRENT_SOURCE_DIR})
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    add_custom_target(doc_doxygen ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM)
  else (DOXYGEN_FOUND)
    message("Doxygen needs to be installed to generate doxygen documentation")
  endif (DOXYGEN_FOUND)
endif()
