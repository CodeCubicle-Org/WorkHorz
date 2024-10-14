#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <sstream>
#include <string>

#include <regex>
#include <sstream>
#include <iomanip>
#include <locale>
#include <codecvt>

#include <boost/asio/ssl.hpp>
#include <openssl/ssl.h>
#include "whz_quill_wrapper.hpp"

[[maybe_unused]] static auto url_decode(const std::string& url) -> std::optional<std::string> {
  std::string result{};

  for (std::size_t i = 0; i < url.size(); ++i) {
    if (url[i] == '%') {
      if (i + 3 <= url.size()) {
        std::uint16_t value = 0;
        std::istringstream is(url.substr(i + 1, 2));
        if (is >> std::hex >> value) {
          result += static_cast<char>(value);
          i += 2;
        } else {
          return std::nullopt;
        }
      } else {
        return std::nullopt;
      }
    } else if (url[i] == '+') {
      result += ' ';
    } else {
      result += url[i];
    }
  }
  return result;
};

namespace whz {

    inline std::string sanitize_utf8_string(const std::string& input) {
        // Lambda to check if a character is printable
        auto is_printable = [](char32_t c) -> bool {
            return (c >= 0x20 && c <= 0x7E) || (c >= 0xA0 && c <= 0x10FFFF);
        };

        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
        std::u32string utf32_string;

        try {
            utf32_string = converter.from_bytes(input);
        } catch (const std::range_error&) {
            // Handle invalid UTF-8 sequences by replacing them with replacement character
            utf32_string = U"\uFFFD";
        }

        std::ostringstream result;
        for (char32_t ch: utf32_string) {
            if (is_printable(ch)) {
                result << converter.to_bytes(ch);
            } else {
                // Convert unprintable character to \uXXXX format
                result << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(ch);
            }
        }

        return result.str();
    }

} //namespace whz