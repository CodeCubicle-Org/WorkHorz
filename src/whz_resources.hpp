#pragma once
//
// Created by patle on 25/07/2024.
//

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include "whz_quill_wrapper.hpp"

namespace whz {

    enum class resource_type : std::uint8_t {
        CSS, /// Files with .css extension
        HTML, /// Files with .html extension
        WHZT, /// Files with .whzt extension, HTML templates for WHZ
        JS, /// Files with .js extension
        IMAGE_PNG, /// Files with .png extension
        IMAGE_JPG, /// Files with .jpg extension
        UNKNOWN /// Files with an extension unknown to WHZ
    };

/**
 * @brief The Resources class is meant for reading resources as files from the
 *filesystem for now. Later it can also write. Every request object will have to
 *manage its own resources.
 **/
    class resources {
    public:
        resources();

        ~resources();

        // std::optional<Resources&>
        bool loadResource(
                const std::string&sfilepath); /// Load a resource file and return false if the
        /// file could not be loaded
        bool unloadResource(); /// Free the memory resources for this file
        bool resourceExists(); /// Check if the resource file exists
        bool resourceisFile(); /// Check if the resource is a file
        bool resourceisDir(); /// Check if the resource is a directory
        std::optional<resource_type>
        getResourceType(); /// Get the resource type of the loaded resource, returns
        /// empty if no resource is loaded
        std::optional<std::filesystem::path>
        getResourcePathFilename(); /// Get the path to the loaded resource, returns
        /// empty if no resource is loaded
        std::optional<std::filesystem::path>
        getResourcePathDirname(); /// Get the path to the loaded resource, returns
        /// empty if no resource is loaded
        std::optional<std::filesystem::path>
        getResourcePathFullpath(); /// Get the path to the loaded resource, returns
        /// empty if no resource is loaded
        std::uintmax_t
        getResourceSize(); /// Get the size of the loaded resource file in bytes,
        /// returns 0 if no resource is loaded or file has 0 bytes
        std::uintmax_t
        getAvailableSystemSpace(); /// Get the remaining available space on the
        /// current filesystem in bytes
        std::optional<std::string>
        getResourceContentStr(); /// Get the content as a string, returns empty if no
        /// resource is loaded for html, js and css files
        std::optional<char *>
        getResourceContentBytes(); /// Get the content as a byte buffer, returns empty
        /// if no resource is loaded

        // Later see how we have to process images to be sent. Maybe bas64 encoded?

    private:
        bool checkResourceFileExists(
                std::filesystem::path
                fpath); /// Check several things about the resource file and save the
        /// results as variables to be ready when called

        bool _resourceFileExists; /// True if the resource file exists
        std::string
                _resourceContentStr; /// The content of the resource file as a string
        std::filesystem::path _resourcePath; /// Path to the resource file
        whz::resource_type _resourceType; /// Enum Class on lower architectural level
        char *_resourceContentByte; /// The content of the resource file as a byte
        /// buffer for images
        whz::whz_qlogger _qlogger; /// The logger for this class
    };
} // namespace WHZ
