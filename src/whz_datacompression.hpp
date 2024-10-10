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

namespace whz {

    namespace fs = std::filesystem;

    class whz_datacompression {
    public:
        whz_datacompression() = default;
        ~whz_datacompression() = default;

        void compress(const std::vector<fs::path>& files, const fs::path& output, const std::string& format) {
            if (format == ".zip") {
                compressZip(files, output);
            } else if (format == ".7z") {
                compress7z(files, output);
            } else {
                throw std::runtime_error("Unsupported format");
            }
        }

        void decompress(const fs::path& archive, const fs::path& outputDir) {
            if (archive.extension() == ".zip") {
                decompressZip(archive, outputDir);
            } else if (archive.extension() == ".7z") {
                decompress7z(archive, outputDir);
            } else {
                throw std::runtime_error("Unsupported format");
            }
        }

        void compressDirectory(const fs::path& directory, const fs::path& output, const std::string& format) {
            std::vector<fs::path> files;
            for (const auto& entry : fs::recursive_directory_iterator(directory)) {
                if (fs::is_regular_file(entry)) {
                    files.push_back(entry.path());
                }
            }
            compress(files, output, format);
        }

        void decompressToDirectory(const fs::path& archive, const fs::path& outputDir) {
            decompress(archive, outputDir);
        }

    private:
        void compressZip(const std::vector<fs::path>& files, const fs::path& output) {
            int error;
            zip_t* archive = zip_open(output.string().c_str(), ZIP_CREATE | ZIP_TRUNCATE, &error);
            if (!archive) {
                throw std::runtime_error("Failed to create ZIP archive");
            }

            for (const auto& file : files) {
                if (fs::is_regular_file(file)) {
                    zip_source_t* source = zip_source_file(archive, file.string().c_str(), 0, 0);
                    if (source == nullptr || zip_file_add(archive, file.filename().string().c_str(), source, ZIP_FL_OVERWRITE) < 0) {
                        zip_source_free(source);
                        zip_close(archive);
                        throw std::runtime_error("Failed to add file to ZIP archive");
                    }
                }
            }
            zip_close(archive);
        }

        void decompressZip(const fs::path& archive, const fs::path& outputDir) {
            int error;
            zip_t* za = zip_open(archive.string().c_str(), 0, &error);
            if (!za) {
                throw std::runtime_error("Failed to open ZIP archive");
            }

            zip_int64_t numEntries = zip_get_num_entries(za, 0);
            for (zip_int64_t i = 0; i < numEntries; ++i) {
                struct zip_stat st;
                zip_stat_index(za, i, 0, &st);

                zip_file_t* zf = zip_fopen_index(za, i, 0);
                if (!zf) {
                    zip_close(za);
                    throw std::runtime_error("Failed to open file inside ZIP archive");
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
        }

        void compress7z(const std::vector<fs::path>& files, const fs::path& output) {
            lzma_stream strm = LZMA_STREAM_INIT;
            if (lzma_easy_encoder(&strm, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64) != LZMA_OK) {
                throw std::runtime_error("Failed to initialize LZMA encoder");
            }

            std::ofstream ofs(output, std::ios::binary);
            if (!ofs) {
                lzma_end(&strm);
                throw std::runtime_error("Failed to open output file");
            }

            for (const auto& file : files) {
                if (fs::is_regular_file(file)) {
                    std::ifstream ifs(file, std::ios::binary);
                    if (!ifs) {
                        lzma_end(&strm);
                        throw std::runtime_error("Failed to open input file");
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
                                throw std::runtime_error("LZMA compression error");
                            }

                            ofs.write(outbuf, sizeof(outbuf) - strm.avail_out);
                        }
                    }
                }
            }
            lzma_end(&strm);
        }

        void decompress7z(const fs::path& archive, const fs::path& outputDir) {
            lzma_stream strm = LZMA_STREAM_INIT;
            if (lzma_auto_decoder(&strm, UINT64_MAX, 0) != LZMA_OK) {
                throw std::runtime_error("Failed to initialize LZMA decoder");
            }

            std::ifstream ifs(archive, std::ios::binary);
            if (!ifs) {
                lzma_end(&strm);
                throw std::runtime_error("Failed to open input archive");
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
                        throw std::runtime_error("LZMA decompression error");
                    }
                    ofs.write(outbuf, sizeof(outbuf) - strm.avail_out);
                }
            }
            lzma_end(&strm);
        }
    };
} // whz

/* Example code
int main() {
    whz::whz_datacompression handler;
    std::vector<fs::path> files = {"example1.txt", "example2.txt"};
    handler.compress(files, "zipped_output.zip", ".zip");
    handler.compress(files, "7zipped_output.7z", ".7z");
    handler.decompress("zipped_output.zip", "zip_output_dir");
    handler.decompress("7zipped_output.zip", "7z_output_dir");

    // Compress and decompress a directory recursively
    handler.compressDirectory("compress_example_dir", "zipped_output_dir.zip", ".zip");
    handler.compressDirectory("compress_example_dir", "7zipped_output_dir.7z", ".7z");
    handler.decompressToDirectory("zipped_output_dir.zip", "zip_output_dir_unzipped");
    handler.decompressToDirectory("7zipped_output_dir.zip", "7zip_output_dir_unzipped");

    return 0;
}
 */