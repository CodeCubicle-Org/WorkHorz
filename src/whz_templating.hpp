//
// Created by Pat Le Cat on 12/10/2024.
//

#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <memory>
#include <vector>
#include <expected>
#include <unordered_map>
#include <bustache/model.hpp>
#include <bustache/format.hpp>
#include <bustache/render/string.hpp>
#include <SQLiteCpp/SQLiteCpp.h>

namespace whz {

    class whz_templating {

    };

// Placeholder for the TemplateDefiner class
    class TemplateDefiner {
    public:
        // Returns the list of variables required by the template
        std::vector<std::string> GetTemplateVariables(const std::string& template_file);

        // Returns the SQL query for a given variable
        std::string GetSQLForVariable(const std::string& variable_name);
    };

// Custom Data Model
    struct DataContext {
        std::unordered_map<std::string, std::string> data;
    };

// Implementing the Bustache Model Traits for DataContext
    namespace bustache {
        template<>
        struct impl_object<DataContext> {
            static void get(DataContext const& self, std::string const& key, value_handler visit) {
                auto it = self.data.find(key);
                if (it != self.data.end()) {
                    visit(it->second);
                } else {
                    visit(nullptr); // Indicate that the key was not found
                }
            }
        };
    }

// TemplateProcessor class
    class TemplateProcessor {
    public:
        // Factory method to create a TemplateProcessor instance
        static std::expected<TemplateProcessor, std::string> Create(const std::string& db_path);

        TemplateProcessor(TemplateProcessor&&) = default;
        TemplateProcessor& operator=(TemplateProcessor&&) = default;
        ~TemplateProcessor() = default;

        // Processes the template file and returns the rendered string or an error message
        [[nodiscard]] std::expected<std::string, std::string> ProcessTemplate(const std::string& template_file);

        // Sets the TemplateDefiner instance
        void SetTemplateDefiner(std::shared_ptr<TemplateDefiner> definer);

    private:
        std::unique_ptr<SQLite::Database> db_;
        std::shared_ptr<TemplateDefiner> template_definer_;

        // Private constructor
        TemplateProcessor(std::unique_ptr<SQLite::Database> db);

        // Helper functions
        [[nodiscard]] std::expected<std::string, std::string> ReadTemplateFile(const std::string& template_file);
        [[nodiscard]] std::expected<bustache::format, std::string> CompileTemplate(const std::string& template_content);
        [[nodiscard]] std::expected<DataContext, std::string> FetchData(const std::string& template_file);
    };

} // whz
