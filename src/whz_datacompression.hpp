//
// Created by Pat Le Cat on 10/10/2024.
//

#pragma once

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <zip.h>
#include <lzma.h>
#include <archive.h>
#include <archive_entry.h>
#include "whz_quill_wrapper.hpp"

namespace whz {

    namespace fs = std::filesystem;

    class whz_datacompression {
    public:
        whz_datacompression() = default;
        ~whz_datacompression() = default;

        /**
         * @brief Compresses a list of files into an archive
         * @param files List of files to compress
         * @param output Path to the output archive
         * @param format Archive format ("zip" or "7z")
         * @param compressionLevel Compression level (0-9), default is 6
         * @return True if the compression was successful, false otherwise
         */
        bool compress(const std::vector<fs::path>& files, const fs::path& output, const std::string& format, int compressionLevel = 6) {
            bool bRet = false;

            // check if the path exists
            if (!fs::exists(output.parent_path())) {
                this->qlogger.error(fmt::format("Compression Error: Output directory does not exist: {}", output.parent_path().string()));
                //throw std::runtime_error("Output directory does not exist");
                //return false;
            }

            if (format == "zip") {
                bRet = compressZip(files, output, compressionLevel);
            } else if (format == "7z") {
                bRet = compress7z(files, output, compressionLevel);
            } else {
                this->qlogger.error("Compression Error: Unsupported file format. Use only .zip or .7z");
                return false;
            }
            return bRet;
        }

        bool decompress(const fs::path& archive, const fs::path& outputDir) {
            bool bRet = false;

            // check if the archive file exists
            if (!fs::exists(archive)) {
                this->qlogger.error(fmt::format("Compression Error: Output directory does not exist: {}", archive.string()));
                return false;
            }

            if (archive.extension() == ".zip") {
                bRet = decompressZip(archive, outputDir);
            } else if (archive.extension() == ".7z") {
                bRet = decompress7z(archive, outputDir);
            } else {
                this->qlogger.error("Compression Error: Unsupported file format. Use only .zip or .7z");
                return false;
            }
            return bRet;
        }

        bool compressDirectory(const fs::path& directory, const fs::path& output, const std::string& format) {
            // Check if the directory and output paths exist
            if (!fs::exists(directory)) {
                this->qlogger.error(fmt::format("Compression Error: Directory does not exist: {}", directory.string()));
                return false;
            }
            if (!fs::exists(output.parent_path())) {
                this->qlogger.error(fmt::format("Compression Error: Output directory does not exist: {}", output.parent_path().string()));
                return false;
            }

            std::vector<fs::path> files;
            for (const auto& entry : fs::recursive_directory_iterator(directory)) {
                if (fs::is_regular_file(entry)) {
                    files.push_back(entry.path());
                }
            }
            return compress(files, output, format);
        }

        bool decompressToDirectory(const fs::path& archive, const fs::path& outputDir) {
            return decompress(archive, outputDir);
        }

    private:
        /**
         * @brief Compresses files into a ZIP archive
         * @param files List of files to compress
         * @param output Path to the output ZIP archive
         * @param compressionLevel Compression level (0-9)
         * @return True if the compression was successful, false otherwise
         */
        bool compressZip(const std::vector<fs::path>& files, const fs::path& output, int compressionLevel = 6) {
            int error;
            zip_t* archive = zip_open(output.string().c_str(), ZIP_CREATE | ZIP_TRUNCATE, &error);
            if (!archive) {
                this->qlogger.error(fmt::format("Compression Error({}): Failed to create ZIP archive", error));
                return false;
            }

            for (const auto& file : files) {
                if (fs::is_regular_file(file)) {
                    zip_source_t* source = zip_source_file(archive, file.string().c_str(), 0, 0);
                    if (source == nullptr || zip_file_add(archive, file.filename().string().c_str(), source, ZIP_FL_OVERWRITE) < 0) {
                        zip_source_free(source);
                        zip_close(archive);
                        this->qlogger.error("Compression Error: Failed to add file to ZIP archive");
                        return false;
                    }
                    // Set the compression level for the file
                    if (zip_set_file_compression(archive, zip_name_locate(archive, file.filename().string().c_str(), 0), ZIP_CM_DEFLATE, compressionLevel) < 0) {
                        this->qlogger.error("Compression Error: Failed to set compression level");
                        return false;
                    }
                }
            }
            zip_close(archive);
            return true;
        }

        bool decompressZip(const fs::path& archive, const fs::path& outputDir) {
            int error = 0;
            zip_t* za = zip_open(archive.string().c_str(), 0, &error);
            if (!za) {
                this->qlogger.error(fmt::format("Decompression Error({}): Failed to open ZIP archive", error));
                return false;
            }

            zip_int64_t numEntries = zip_get_num_entries(za, 0);
            for (zip_int64_t i = 0; i < numEntries; ++i) {
                struct zip_stat st;
                zip_stat_index(za, i, 0, &st);

                zip_file_t* zf = zip_fopen_index(za, i, 0);
                if (!zf) {
                    zip_close(za);
                    this->qlogger.error("Decompression Error: Failed to open file inside ZIP archive");
                    return false;
                }

                fs::path outFilePath = outputDir / st.name;
                fs::create_directories(outFilePath.parent_path());
                std::ofstream outFile(outFilePath, std::ios::binary);

                char buffer[8192];
                zip_int64_t bytesRead;
                while ((bytesRead = zip_fread(zf, buffer, sizeof(buffer))) > 0) {
                    outFile.write(buffer, bytesRead);
                }
                zip_fclose(zf);
            }
            zip_close(za);
            return true;
        }

        /**
         * @brief Compresses files into a 7z archive
         * @param files List of files to compress
         * @param output Path to the output 7z archive
         * @param compressionLevel Compression levels: 0 - 9 or LZMA_PRESET_DEFAULT=6
         * @return True if the compression was successful, false otherwise
         */
        bool compress7z(const std::vector<fs::path>& files, const fs::path& output, uint32_t compressionLevel = 6) {
            struct archive* a;
            struct archive_entry* entry;
            char buff[8192];
            int len;
            std::ifstream ifs;

            a = archive_write_new();
            archive_write_set_format_7zip(a);
            archive_write_add_filter_lzma(a);
            archive_write_set_options(a, ("compression-level=" + std::to_string(compressionLevel)).c_str());

            if (archive_write_open_filename(a, output.string().c_str()) != ARCHIVE_OK) {
                this->qlogger.error("Compression Error: Failed to open output file");
                return false;
            }

            for (const auto& file : files) {
                if (fs::is_regular_file(file)) {
                    entry = archive_entry_new();
                    archive_entry_set_pathname(entry, file.string().c_str());
                    archive_entry_set_size(entry, fs::file_size(file));
                    archive_entry_set_filetype(entry, AE_IFREG);
                    archive_entry_set_perm(entry, 0644);
                    archive_write_header(a, entry);

                    ifs.open(file, std::ios::binary);
                    while ((len = ifs.readsome(buff, sizeof(buff))) > 0) {
                        archive_write_data(a, buff, len);
                    }
                    ifs.close();
                    archive_entry_free(entry);
                }
            }

            archive_write_close(a);
            archive_write_free(a);
            return true;
        }

        bool decompress7z(const fs::path& archive, const fs::path& outputDir) {
            struct archive* a;
            struct archive* ext;
            struct archive_entry* entry;
            int r;

            a = archive_read_new();
            archive_read_support_format_7zip(a);
            archive_read_support_filter_all(a);

            ext = archive_write_disk_new();
            archive_write_disk_set_options(ext, ARCHIVE_EXTRACT_TIME);
            archive_write_disk_set_standard_lookup(ext);

            if ((r = archive_read_open_filename(a, archive.string().c_str(), 10240))) {
                this->qlogger.error(fmt::format("Decompression Error: Failed to open 7z archive: {}", archive.string()));
                archive_read_free(a);
                archive_write_free(ext);
                return false;
            }

            while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
                const char* currentFile = archive_entry_pathname(entry);
                fs::path fullOutputPath = outputDir / currentFile;
                archive_entry_set_pathname(entry, fullOutputPath.string().c_str());

                r = archive_write_header(ext, entry);
                if (r != ARCHIVE_OK) {
                    this->qlogger.error(fmt::format("Decompression Error: Failed to write header for file: {}", currentFile));
                } else {
                    const void* buff;
                    size_t size;
                    la_int64_t offset;

                    while ((r = archive_read_data_block(a, &buff, &size, &offset)) == ARCHIVE_OK) {
                        r = archive_write_data_block(ext, buff, size, offset);
                        if (r != ARCHIVE_OK) {
                            this->qlogger.error(fmt::format("Decompression Error: Failed to write data for file: {}", currentFile));
                            break;
                        }
                    }
                    if (r != ARCHIVE_EOF) {
                        this->qlogger.error(fmt::format("Decompression Error: Failed to read data for file: {}", currentFile));
                    }
                }
                archive_write_finish_entry(ext);
            }

            archive_read_close(a);
            archive_read_free(a);
            archive_write_close(ext);
            archive_write_free(ext);
            return true;
        }

    private:
        whz_qlogger qlogger;
    };
} // whz
