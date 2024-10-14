//
// Created by Pat Le Cat on 14/10/2024.
//

#pragma once

#include <string>
#include <string_view>
#include <sstream>
#include <map>
#include <vector>
#include <format>
#include <initializer_list>

namespace whz {


class whz_vcard {
public:
    // Set default version to 4.0 for RFC6350 compliance
    whz_vcard() {
        this->addProperty("VERSION", "4.0");
    }

    // Add a property to the VCard (e.g., FN, N, TEL, EMAIL)
    void addProperty(std::string_view name, std::string_view value) {
        this->properties_.emplace_back(std::string{name}, std::string{value});
    }

    // Add a property with parameters to the VCard (e.g., TEL;TYPE=cell)
    void addProperty(std::string_view name, const std::map<std::string, std::string>& parameters, std::string_view value) {
        std::ostringstream oss;
        oss << name;
        for (const auto& [key, param_value] : parameters) {
            oss << ";" << key << "=" << param_value;
        }
        oss << ":" << value;
        this->properties_.emplace_back(oss.str(), "");
    }

    // Add an address field to the VCard
    void addAddress(const std::map<std::string, std::string>& parameters, std::initializer_list<std::string> addressFields) {
        std::ostringstream oss;
        oss << "ADR";
        for (const auto& [key, param_value] : parameters) {
            oss << ";" << key << "=" << param_value;
        }
        oss << ":";
        bool first = true;
        for (const auto& field : addressFields) {
            if (!first) {
                oss << ";";
            }
            oss << field;
            first = false;
        }
        this->properties_.emplace_back(oss.str(), "");
    }

    // Generate and assemble the VCard string in UTF-8 format
    [[nodiscard]] std::string toString() const {
        std::ostringstream oss;
        oss << "BEGIN:VCARD\n";
        for (const auto& [name, value] : this->properties_) {
            if (!value.empty()) {
                oss << name << ":" << value << "\n";
            } else {
                oss << name << "\n";
            }
        }
        oss << "END:VCARD\n";
        return oss.str();
    }

    // Add all RFC6350 fields in a dynamic way
    void addRFC6350Field(std::string_view fieldName, const std::map<std::string, std::string>& parameters, std::string_view value) {
        addProperty(fieldName, parameters, value);
    }

private:
    std::vector<std::pair<std::string, std::string>> properties_;
};

        /*
int main() {
    VCard vcard;
    vcard.addProperty("FN", "John Doe");
    vcard.addProperty("N", "Doe;John;;;");
    vcard.addProperty("EMAIL", { { "TYPE", "work" } }, "john.doe@example.com");
    vcard.addProperty("TEL", { { "TYPE", "cell" } }, "+123456789");
    vcard.addAddress({ { "TYPE", "home" } }, { "", "", "123 Main St", "Anytown", "CA", "12345", "USA" });
    vcard.addRFC6350Field("URL", { { "TYPE", "work" } }, "https://example.com");

    std::string vcardString = vcard.toString();
    std::cout << vcardString;

    return 0;
}

};*/

} // whz

