#pragma once

#include <cstdint>
#include <tuple>

#include "whz_common.hpp"

namespace whz {

enum result_type : std::uint8_t { good, bad, indeterminate };

class request_parser {
 public:
  request_parser();

  auto reset() -> void;

  template <typename InputIterator>
  std::tuple<result_type, InputIterator> parse(
      whz::request& request, InputIterator begin, InputIterator end) {
    while (begin != end) {
      result_type result = consume(request, *begin++);
      if (result == good || result == bad) {
        return std::make_tuple(result, begin);
      }
    }
    return std::make_tuple(indeterminate, begin);
  }

 private:
  auto consume(request& req, char input) -> result_type;
  static auto is_char(int c) -> bool;
  static auto is_ctl(int c) -> bool;
  static auto is_tspecial(int c) -> bool;
  static auto is_digit(int c) -> bool;

  /// The current state of the parser.
  enum state : std::uint8_t {
    method_start,
    method,
    uri,
    http_version_h,
    http_version_t_1,
    http_version_t_2,
    http_version_p,
    http_version_slash,
    http_version_major_start,
    http_version_major,
    http_version_minor_start,
    http_version_minor,
    expecting_newline_1,
    header_line_start,
    header_lws,
    header_name,
    space_before_header_value,
    header_value,
    expecting_newline_2,
    expecting_newline_3
  } state_;
};

}; // namespace whz
