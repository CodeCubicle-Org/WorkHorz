#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <sstream>
#include <string>

#include <boost/asio/ssl.hpp>
#include <openssl/ssl.h>

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
