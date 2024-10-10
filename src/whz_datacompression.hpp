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
#include "whz_quill_wrapper.hpp"

namespace whz {

    namespace fs = std::filesystem;

    class whz_datacompression {
    public:
        whz_datacompression() = default;
        ~whz_datacompression() = default;

        bool compress(const std::vector<fs::path>& files, const fs::path& output, const std::string& format) {
            bool bRet = false;

            // check if the path exists
            if (!fs::exists(output.parent_path())) {
                this->qlogger.error(fmt::format("Compression Error: Output directory does not exist: {}", output.parent_path().string()));
                //throw std::runtime_error("Output directory does not exist");
                return false;
            }

            if (format == ".zip") {
                bRet = compressZip(files, output);
            } else if (format == ".7z") {
                bRet = compress7z(files, output);
            } else {
                this->qlogger.error("Compression Error: Unsupported file format. Use only .zip or .7z");
                return false;
            }
            return bRet;
        }

        bool decompress(const fs::path& archive, const fs::path& outputDir) {
            bool bRet = false;

            // check if the path exists
            if (!fs::exists(outputDir.parent_path())) {
                this->qlogger.error(fmt::format("Compression Error: Output directory does not exist: {}", outputDir.parent_path().string()));
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
        bool compressZip(const std::vector<fs::path>& files, const fs::path& output) {
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
                }
            }
            zip_close(archive);
            return true;
        }

        bool decompressZip(const fs::path& archive, const fs::path& outputDir) {
            int error;
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

        bool compress7z(const std::vector<fs::path>& files, const fs::path& output) {
            lzma_stream strm = LZMA_STREAM_INIT;
            if (lzma_easy_encoder(&strm, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64) != LZMA_OK) {
                this->qlogger.error("Compression Error: Failed to initialize LZMA encoder");
                return false;
            }

            std::ofstream ofs(output, std::ios::binary);
            if (!ofs) {
                lzma_end(&strm);
                this->qlogger.error(fmt::format("Compression Error: Failed to open output file: {}", output.string()));
                return false;
            }

            for (const auto& file : files) {
                if (fs::is_regular_file(file)) {
                    std::ifstream ifs(file, std::ios::binary);
                    if (!ifs) {
                        lzma_end(&strm);
                        this->qlogger.error("Compression Error: Failed to open input file");
                        return false;
                    }

                    char buffer[8192];
                    while (ifs.read(buffer, sizeof(buffer)) || ifs.gcount() > 0) {
                        strm.next_in = reinterpret_cast<const uint8_t*>(buffer);
                        strm.avail_in = ifs.gcount();

                        while (strm.avail_in > 0) {
                            char outbuf[8192];
                            strm.next_out = reinterpret_cast<uint8_t*>(outbuf);
                            strm.avail_out = sizeof(outbuf);

                            lzma_ret ret = lzma_code(&strm, LZMA_RUN);
                            if (ret != LZMA_OK && ret != LZMA_STREAM_END) {
                                lzma_end(&strm);
                                this->qlogger.error("Compression Error: LZMA compression error");
                                return false;
                            }

                            ofs.write(outbuf, sizeof(outbuf) - strm.avail_out);
                        }
                    }
                }
            }
            lzma_end(&strm);
            return true;
        }

        bool decompress7z(const fs::path& archive, const fs::path& outputDir) {
            lzma_stream strm = LZMA_STREAM_INIT;
            if (lzma_auto_decoder(&strm, UINT64_MAX, 0) != LZMA_OK) {
                this->qlogger.error("Decompression Error: Failed to initialize LZMA decoder");
                return false;
            }

            std::ifstream ifs(archive, std::ios::binary);
            if (!ifs) {
                lzma_end(&strm);
                this->qlogger.error("Decompression Error: Failed to open input archive");
                return false;
            }

            fs::create_directories(outputDir);
            std::ofstream ofs(outputDir / archive.stem(), std::ios::binary);

            char buffer[8192];
            while (ifs.read(buffer, sizeof(buffer)) || ifs.gcount() > 0) {
                strm.next_in = reinterpret_cast<const uint8_t*>(buffer);
                strm.avail_in = ifs.gcount();

                while (strm.avail_in > 0) {
                    char outbuf[8192];
                    strm.next_out = reinterpret_cast<uint8_t*>(outbuf);
                    strm.avail_out = sizeof(outbuf);

                    lzma_ret ret = lzma_code(&strm, LZMA_RUN);
                    if (ret != LZMA_OK && ret != LZMA_STREAM_END) {
                        lzma_end(&strm);
                        this->qlogger.error("Decompression Error: LZMA decompression error");
                        return false;
                    }
                    ofs.write(outbuf, sizeof(outbuf) - strm.avail_out);
                }
            }
            lzma_end(&strm);
            return true;
        }

    private:
        whz_qlogger qlogger;
    };
} // whz
