//
// Created by patle on 26/09/2024.
//

#pragma once

#include <unordered_map>
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>

namespace whz {
    /** Singleton class to cache the content of the templates files with the extension ".whzt". Those files can contain
     *  HTML code with placeholders that will be replaced by the data of the page. But can actually contain any kind of
     *  text content with placeholders. Mustache must be used to use the built-in template engine.
     */
    class whz_templateCache {
    public:
        // Delete copy constructor and assignment operator to prevent copying
        whz_templateCache(const whz_templateCache &) = delete;

        whz_templateCache &operator=(const whz_templateCache &) = delete;

        // Static method to get the single instance of the class
        static whz_templateCache &getInstance() {
            static whz_templateCache instance;
            return instance;
        }

        /** Load all .whzt files from the given directory into memory or a memory-mapped file if too big. Call this once
         * at the startup.
         *
         *  @param directoryPath The path to the directory containing the .whzt files, including subfolders
         *  @param domemorymap If true, the content of the templates will be memory-mapped into /whz_mmtemplates
         */
        void loadTemplates(const std::string &directoryPath, bool domemorymap = false) {
            for (const auto &entry: std::filesystem::recursive_directory_iterator(directoryPath)) {
                if (entry.is_regular_file() && entry.path().extension() == ".whzt") {
                    std::ifstream file(entry.path());
                    if (file) {
                        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                        whz_templates.emplace(entry.path().string(), content);
                    } else {
                        std::cerr << "Failed to open template file: " << entry.path() << std::endl;
                    }
                }
            }
            if (!this->whz_templates.empty() && domemorymap) {
                memoryMapTemplates(directoryPath + "/whz_mmtemplates/whz_mmtemplates_001.mmf"); // default memory map location
            }
        }

        // Get the content of a template by its path
        std::string getTemplate(const std::string &path) const {
            auto range = whz_templates.equal_range(path);
            if (range.first != range.second) {
                return range.first->second;
            }
            return "";
        }

        /** Method to reload all .whzt files from the given directory. Call this when the templates have been updated,
         * during runtime at your own risk!
         *
         */
        void reloadTemplates(const std::string &directoryPath) {
            whz_templates.clear();
            loadTemplates(directoryPath);
        }



    private:
        // Private constructor to prevent instantiation
        whz_templateCache() = default;

        // Method to memory-map the whz_templates variable
        void memoryMapTemplates(const std::string &target_filePath);

        std::unordered_multimap<std::string, std::string> whz_templates;
    };
} // namespace whz
