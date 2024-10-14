//
// Created by Pat Le Cat on 12/10/2024.
//

#include "whz_templating.hpp"

namespace whz {

    std::expected<TemplateProcessor, std::string> TemplateProcessor::Create(const std::string& db_path) {
        try {
            auto db = std::make_unique<SQLite::Database>(db_path, SQLite::OPEN_READONLY);
            return TemplateProcessor(std::move(db));
        } catch (const SQLite::Exception& e) {
            // Convert the exception into an error message without throwing
            return std::unexpected("Failed to open database: " + std::string(e.what()));
        }
    }

    TemplateProcessor::TemplateProcessor(std::unique_ptr<SQLite::Database> db)
            : db_(std::move(db)) {}

    void TemplateProcessor::SetTemplateDefiner(std::shared_ptr<TemplateDefiner> definer) {
        template_definer_ = definer;
    }

    [[nodiscard]] std::expected<std::string, std::string> TemplateProcessor::ReadTemplateFile(const std::string& template_file) {
        std::ifstream file(template_file);
        if (!file) {
            return std::unexpected("Failed to open template file: " + template_file);
        }

        return std::string{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    }

    [[nodiscard]] std::expected<bustache::format, std::string> TemplateProcessor::CompileTemplate(const std::string& template_content) {
        try {
            bustache::format format{template_content};
            return format;
        } catch (const std::exception& e) {
            return std::unexpected("Template compilation failed: " + std::string(e.what()));
        }
    }

    [[nodiscard]] std::expected<DataContext, std::string> TemplateProcessor::FetchData(const std::string& template_file) {
        if (!template_definer_) {
            return std::unexpected("Template definer not set.");
        }

        auto variables = template_definer_->GetTemplateVariables(template_file);
        DataContext context;

        for (const auto& var : variables) {
            auto sql = template_definer_->GetSQLForVariable(var);

            try {
                SQLite::Statement query(*db_, sql);

                if (query.executeStep()) {
                    context.data[var] = query.getColumn(0).getString();
                } else {
                    context.data[var] = "";
                }
            } catch (const SQLite::Exception& e) {
                // Convert the exception into an error message without throwing
                return std::unexpected("SQL execution error for variable '" + var + "': " + e.what());
            }
        }

        return context;
    }

    [[nodiscard]] std::expected<std::string, std::string> TemplateProcessor::ProcessTemplate(const std::string& template_file) {
        auto template_content_exp = ReadTemplateFile(template_file);
        if (!template_content_exp) {
            return std::unexpected(template_content_exp.error());
        }
        auto template_content = *template_content_exp;

        auto format_exp = CompileTemplate(template_content);
        if (!format_exp) {
            return std::unexpected(format_exp.error());
        }
        auto format = *format_exp;

        auto data_exp = FetchData(template_file);
        if (!data_exp) {
            return std::unexpected(data_exp.error());
        }
        auto data = *data_exp;

        try {
            return bustache::render_string(format, data);
        } catch (const std::exception& e) {
            return std::unexpected("Template rendering failed: " + std::string(e.what()));
        }
    }

} // whz