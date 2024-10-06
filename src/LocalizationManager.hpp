#pragma once

#include <boost/locale.hpp>
#include <string>
#include <mutex>
#include <unordered_map>
#include <simdutf.h>
#include <iostream>

class LocalizationManager {
public:
    static LocalizationManager& getInstance() {
        static LocalizationManager instance;
        return instance;
    }

    void addLocale(std::string_view localeDir, std::string_view domain, std::string_view language) {
        boost::locale::generator gen;

        std::lock_guard lock(mutex_);

        gen.add_messages_path(std::string(localeDir));
        gen.add_messages_domain(std::string(domain));
        locales_.emplace(language, gen(std::string(language)));
    }

    void switchLocale(std::string_view language) {
        std::lock_guard lock(mutex_);
        if (auto it = locales_.find(static_cast<std::string>(language)); it != locales_.end()) {
            std::locale::global(it->second);
            std::cout.imbue(std::locale());
        }
    }

    std::string translate(std::string_view msg) {
        std::lock_guard lock(mutex_);
        return boost::locale::translate(std::string(msg)).str();
    }

    std::string toLatin1(std::string_view utf8Str) {
        std::string latin1Str;
        latin1Str.resize(utf8Str.size()); // Latin1 will not be larger than UTF-8
        auto result = simdutf::convert_utf8_to_latin1(utf8Str.data(), utf8Str.size(), latin1Str.data());
        latin1Str.resize(result);
        return latin1Str;
    }

    std::string toUtf8(std::string_view latin1Str) {
        std::string utf8Str;
        utf8Str.resize(latin1Str.size() * 2); // UTF-8 can be up to twice as large as Latin1
        auto result = simdutf::convert_latin1_to_utf8(latin1Str.data(), latin1Str.size(), utf8Str.data());
        utf8Str.resize(result);
        return utf8Str;
    }

    bool isValidUtf8(std::string_view str) {
        return simdutf::validate_utf8(str.data(), str.size());
    }

private:
    LocalizationManager() = default;
    ~LocalizationManager() = default;
    LocalizationManager(const LocalizationManager&) = delete;
    LocalizationManager& operator=(const LocalizationManager&) = delete;

    std::mutex mutex_;
    std::unordered_map<std::string, std::locale> locales_;
};