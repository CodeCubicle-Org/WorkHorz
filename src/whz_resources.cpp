//
// Created by patle on 25/07/2024.
//

#include "whz_resources.hpp"
#include <fstream>
#include <iostream>

namespace whz {

resources::~resources() {
  delete[] this->_resourceContentByte; // Because created with new char[]
}

/**
 * @brief Load a resource file and return false if the file could not be loaded.
 * All the file's data is stored in the class
 * @param sfilepath The full path to the resource file
 * @return True if the file was loaded
 */
bool resources::loadResource(const std::string& sfilepath) {
  bool bRet = false;
  bool bFileExists = false;

  this->_resourcePath = std::filesystem::path(sfilepath);
  bFileExists = this->checkResourceFileExists(this->_resourcePath);
  if (bFileExists) {
    std::ifstream ifs(sfilepath, std::ios::in); // Open the file for reading
    if (!ifs.is_open())
      std::cout << "ERROR: Failed to open the file" << sfilepath << '\n';
    else {
      if (_resourceType == resource_type::IMAGE_PNG ||
          _resourceType == resource_type::IMAGE_JPG) {
        // Read the file into a byte buffer
        ifs.seekg(0, std::ios::end);
        auto fsize = ifs.tellg();
        char* tBytes = new char[fsize];
        ifs.seekg(0, std::ios::beg);
        ifs.read(tBytes, fsize);

        this->_resourceContentByte = tBytes;
      } else // it's a text file
      {
        // Read the file into a string
        ifs.seekg(0, std::ios::end);
        std::string tStr = "";
        tStr.resize(ifs.tellg());
        ifs.seekg(0, std::ios::beg);
        ifs.read(&tStr[0], tStr.size());

        this->_resourceContentStr = std::move(tStr);
      }

      auto fsize = ifs.tellg();
      std::string tStr = "";

      // Read the file into a string
      ifs.seekg(0, std::ios::end);
      tStr.resize(ifs.tellg());
      ifs.seekg(0, std::ios::beg);
      ifs.read(&tStr[0], tStr.size());

      this->_resourceContentStr = std::move(tStr);
      // Check the io state of the stream
      if (ifs.good()) {
        bRet = true;
      } else {
        // No good!
      }
      ifs.close();
    }
  }
  return (bRet && bFileExists);
}

/**
 * @brief Free the memory resources for this file by resetting all the member
 * variables
 * @return True if the resources were freed
 */
bool resources::unloadResource() {
  this->_resourcePath.clear();
  delete[] this->_resourceContentByte; // Because created with new char[]
  this->_resourceContentByte = nullptr;
  this->_resourceContentStr.clear();
  this->_resourceFileExists = false;
  this->_resourceType = resource_type::UNKNOWN;
  return true;
}

bool resources::resourceExists() {
  return this->_resourceFileExists;
}

bool resources::resourceisDir() {
  return is_directory(this->_resourcePath);
}

std::optional<std::filesystem::path> resources::getResourcePathFilename() {
  return this->_resourcePath.filename();
}

std::optional<std::filesystem::path> resources::getResourcePathDirname() {
  return this->_resourcePath.root_directory();
}

std::optional<std::filesystem::path> resources::getResourcePathFullpath() {
  return this->_resourcePath;
}

/**
 * @brief Get the size of the loaded resource file in bytes
 * @return The size of the loaded resource file in bytes
 */
std::uintmax_t resources::getResourceSize() {
  return std::filesystem::file_size(this->_resourcePath);
}

/**
 * @brief Get the remaining available space on the current filesystem in bytes
 * @return The remaining available space on the system in bytes
 */
std::uintmax_t resources::getAvailableSystemSpace() {
  std::filesystem::space_info fsi = std::filesystem::space(this->_resourcePath);
  std::uintmax_t availableBytes = fsi.available;
  return availableBytes;
}

std::optional<std::string> resources::getResourceContentStr() {
  return this->_resourceContentStr;
}

std::optional<char*> resources::getResourceContentBytes() {
  return this->_resourceContentByte;
}

/**
 * @brief Check if the resource file exists and set the internal variables
 * accordingly
 * @param fpath The path to the resource file
 * @return True if the file exists and is a regular file
 */
bool resources::checkResourceFileExists(std::filesystem::path fpath) {
  bool bRet = false;

  if (std::filesystem::exists(fpath)) {
    bRet = std::filesystem::is_regular_file(fpath);
    if (bRet) {
      _resourceFileExists = true;
      _resourcePath = fpath;
    } else {
      _resourceFileExists = false;
      _resourcePath = "";
      _resourceType = resource_type::UNKNOWN;
    }
    // Check for the known file extensions
    if (fpath.extension().string() == ".css") {
      _resourceType = resource_type::CSS;
    } else if (
        fpath.extension().string() == ".html" ||
        fpath.extension().string() == ".htm") {
      _resourceType = resource_type::HTML;
    } else if (fpath.extension().string() == ".whzt") {
      _resourceType = resource_type::WHZT;
    } else if (fpath.extension().string() == ".js") {
      _resourceType = resource_type::JS;
    } else if (fpath.extension().string() == ".png") {
      _resourceType = resource_type::IMAGE_PNG;
    } else if (
        fpath.extension().string() == ".jpg" ||
        fpath.extension().string() == ".jpeg") {
      _resourceType = resource_type::IMAGE_JPG;
    } else {
      _resourceType = resource_type::UNKNOWN;
      std::cout << "ERROR: Failed to open the file" << fpath
                << ", file extension is unknown/unsupported.\n";
    }
  }
  return bRet;
}

std::optional<resource_type> resources::getResourceType() {
  return this->_resourceType;
}
} // namespace WHZ
