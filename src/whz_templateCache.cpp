//
// Created by patle on 26/09/2024.
//

#include "whz_templateCache.hpp"
#include "whz_quill_wrapper.hpp"
#include <cstring>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

namespace whz {

    void whz_templateCache::memoryMapTemplates(const std::string &target_filePath) {
        int fd = open(target_filePath.c_str(), O_RDWR | O_CREAT, 0666);
        if (fd == -1) {
            this->_qlogger.error(fmt::format("Failed to open file for memory mapping: {}", target_filePath));
            //LOG_ERROR(whz_qlogger::getInstance().getLogger(), "Failed to open file for memory mapping: {}", target_filePath);
            std::cerr << "Failed to open file for memory mapping: " << target_filePath << std::endl;
            return;
        }

        size_t size = 0;
        for (const auto &pair : whz_templates) {
            size += pair.first.size() + pair.second.size() + 2; // +2 for null terminators
        }

        if (ftruncate(fd, size) == -1) {
            this->_qlogger.error(fmt::format("Failed to set the size of the file: {}", target_filePath));
            //LOG_ERROR(whz_qlogger::getInstance().getLogger(), "Failed to set the size of the file: {}", target_filePath);
            std::cerr << "Failed to set the size of the file: " << target_filePath << std::endl;
            close(fd);
            return;
        }

        void *map = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (map == MAP_FAILED) {
            this->_qlogger.error(fmt::format("Failed to memory-map the file: {}", target_filePath));
            //LOG_ERROR(whz_qlogger::getInstance().getLogger(), "Failed to memory-map the file: {}", target_filePath);
            std::cerr << "Failed to memory-map the file: " << target_filePath << std::endl;
            close(fd);
            return;
        }

        char *ptr = static_cast<char *>(map);
        for (const auto &pair : whz_templates) {
            memcpy(ptr, pair.first.c_str(), pair.first.size() + 1);
            ptr += pair.first.size() + 1;
            memcpy(ptr, pair.second.c_str(), pair.second.size() + 1);
            ptr += pair.second.size() + 1;
        }

        if (msync(map, size, MS_SYNC) == -1) {
            this->_qlogger.error(fmt::format("Failed to sync the memory-mapped file: {}", target_filePath));
            //LOG_ERROR(whz_qlogger::getInstance().getLogger(), "Failed to sync the memory-mapped file: {}", target_filePath);
            std::cerr << "Failed to sync the memory-mapped file: " << target_filePath << std::endl;
        }

        if (munmap(map, size) == -1) {
            this->_qlogger.error(fmt::format("Failed to unmap the memory-mapped file: {}", target_filePath));
            //LOG_ERROR(whz_qlogger::getInstance().getLogger(), "Failed to unmap the memory-mapped file: {}", target_filePath);
            std::cerr << "Failed to unmap the memory-mapped file: " << target_filePath << std::endl;
        }

        close(fd);
    }
} // whz