//
// Created by Pat Le Cat on 14/10/2024.
//

#pragma once

#include "qrcodegen.hpp"
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <iomanip>
#include <png.h>
#include <memory>

namespace whz {

    class whz_qrcode_generator {


    public:
        struct QRCodeParams {
            int scale = 10;
            int border = 4;
            std::string foregroundColor = "black";
            std::string backgroundColor = "white";
            qrcodegen::QrCode::Ecc errorCorrectionLevel = qrcodegen::QrCode::Ecc::LOW;
            int maskPattern = -1; // -1 for automatic
        };

        whz_qrcode_generator() = default;

        bool generateQRCode(const std::u8string &input, const std::string &outputPath, const std::string &format, const QRCodeParams &params) noexcept {
            if (!std::filesystem::exists(std::filesystem::path(outputPath).parent_path())) {
                //return false;
            }

            auto qr = qrcodegen::QrCode::encodeText(reinterpret_cast<const char*>(input.c_str()), params.errorCorrectionLevel);

            if (format == "png") {
                return writePNG(outputPath, qr, params);
            } else if (format == "svg") {
                return writeSVG(outputPath, qr, params);
            }
            return false;
        }

        std::string generateBase64Bitmap(const std::u8string &input, const QRCodeParams &params) noexcept {
            auto qr = qrcodegen::QrCode::encodeText(reinterpret_cast<const char*>(input.c_str()), params.errorCorrectionLevel);
            return encodeBase64Png(qr, params);
        }

    private:
        bool writePNG(const std::string &filePath, const qrcodegen::QrCode &qr, const QRCodeParams &params) noexcept {
            FILE *fp = fopen(filePath.c_str(), "wb");
            if (!fp) {
                return false;
            }

            png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
            if (!png) {
                fclose(fp);
                return false;
            }

            png_infop info = png_create_info_struct(png);
            if (!info) {
                png_destroy_write_struct(&png, nullptr);
                fclose(fp);
                return false;
            }

            if (setjmp(png_jmpbuf(png))) {
                png_destroy_write_struct(&png, &info);
                fclose(fp);
                return false;
            }

            png_init_io(png, fp);

            int size = qr.getSize();
            int imageSize = size * params.scale + 2 * params.border;

            png_set_IHDR(png, info, imageSize, imageSize, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
            png_write_info(png, info);

            std::vector<uint8_t> row(imageSize, 255);
            for (int y = -params.border; y < size * params.scale + params.border; y++) {
                std::fill(row.begin(), row.end(), 255);
                for (int x = -params.border; x < size * params.scale + params.border; x++) {
                    if (y >= 0 && y / params.scale < size && x >= 0 && x / params.scale < size) {
                        if (qr.getModule(x / params.scale, y / params.scale)) {
                            row[x + params.border] = 0;
                        }
                    }
                }
                png_write_row(png, row.data());
            }

            png_write_end(png, nullptr);
            png_destroy_write_struct(&png, &info);
            fclose(fp);
            return true;
        }

        bool writeSVG(const std::string &filePath, const qrcodegen::QrCode &qr, const QRCodeParams &params) noexcept {
            std::ofstream file(filePath);
            if (!file) {
                return false;
            }

            int size = qr.getSize();
            int border = params.border;
            file << "<svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' viewBox='0 0 " << (size + border * 2) << " " << (size + border * 2) << "' stroke='none'>\n";
            file << "<rect width='100%' height='100%' fill='" << params.backgroundColor << "'/>\n";
            file << "<path d='";
            for (int y = 0; y < size; y++) {
                for (int x = 0; x < size; x++) {
                    if (qr.getModule(x, y)) {
                        if (x != 0 || y != 0) {
                            file << " ";
                        }
                        file << "M" << (x + border) << "," << (y + border) << "h1v1h-1z";
                    }
                }
            }
            file << "' fill='" << params.foregroundColor << "'/>\n";
            file << "</svg>\n";
            return true;
        }

        std::string encodeBase64Png(const qrcodegen::QrCode &qr, const QRCodeParams &params) noexcept {
            std::ostringstream ss;
            ss << "data:image/png;base64,";

            int size = qr.getSize();
            int imageSize = size * params.scale + 2 * params.border;

            std::vector<uint8_t> image(imageSize * imageSize, 255);
            for (int y = 0; y < size; y++) {
                for (int x = 0; x < size; x++) {
                    if (qr.getModule(x, y)) {
                        for (int dy = 0; dy < params.scale; dy++) {
                            for (int dx = 0; dx < params.scale; dx++) {
                                int index = ((y * params.scale + dy + params.border) * imageSize) + (x * params.scale + dx + params.border);
                                image[index] = 0;
                            }
                        }
                    }
                }
            }

            // Use a dynamic memory buffer to write the PNG data
            std::unique_ptr<FILE, decltype(&fclose)> fp(tmpfile(), &fclose);
            if (!fp) {
                return "";
            }

            png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
            if (!png) {
                return "";
            }

            png_infop info = png_create_info_struct(png);
            if (!info) {
                png_destroy_write_struct(&png, nullptr);
                return "";
            }

            if (setjmp(png_jmpbuf(png))) {
                png_destroy_write_struct(&png, &info);
                return "";
            }

            png_init_io(png, fp.get());

            png_set_IHDR(png, info, imageSize, imageSize, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
            png_write_info(png, info);

            for (int y = 0; y < imageSize; y++) {
                png_write_row(png, &image[y * imageSize]);
            }

            png_write_end(png, nullptr);
            png_destroy_write_struct(&png, &info);

            // Read the PNG data from the temporary file
            fseek(fp.get(), 0, SEEK_END);
            long fileSize = ftell(fp.get());
            fseek(fp.get(), 0, SEEK_SET);

            std::vector<unsigned char> pngData(fileSize);
            fread(pngData.data(), 1, fileSize, fp.get());

            ss << base64Encode(pngData.data(), pngData.size());
            return ss.str();
        }

        static std::string base64Encode(const unsigned char *data, size_t len) noexcept {
            static const char base64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
            std::string ret;
            ret.reserve((len + 2) / 3 * 4);

            for (size_t i = 0; i < len; i += 3) {
                unsigned int value = data[i] << 16;
                if (i + 1 < len) value |= data[i + 1] << 8;
                if (i + 2 < len) value |= data[i + 2];

                ret.push_back(base64Chars[(value >> 18) & 0x3F]);
                ret.push_back(base64Chars[(value >> 12) & 0x3F]);
                ret.push_back((i + 1 < len) ? base64Chars[(value >> 6) & 0x3F] : '=');
                ret.push_back((i + 2 < len) ? base64Chars[value & 0x3F] : '=');
            }
            return ret;
        }
    };

// Example usage with VCard version 4
    /*int main() {
        QRCodeGenerator generator;
        QRCodeGenerator::QRCodeParams params;
        params.scale = 8;
        params.border = 2;
        params.foregroundColor = "#0000FF";
        params.backgroundColor = "#FFFFFF";
        params.errorCorrectionLevel = qrcodegen::QrCode::Ecc::MEDIUM;

        std::u8string vCard = u8"BEGIN:VCARD\nVERSION:4.0\nFN:John Doe\nORG:Example Corp\nTEL:+123456789\nEMAIL:john.doe@example.com\nEND:VCARD";

        // Generate SVG file
        if (!generator.generateQRCode(vCard, "qrcode.svg", "svg", params)) {
            std::cerr << "Failed to generate SVG QR Code." << std::endl;
        }

        // Generate PNG file
        if (!generator.generateQRCode(vCard, "qrcode.png", "png", params)) {
            std::cerr << "Failed to generate PNG QR Code." << std::endl;
        }

        // Generate Base64 encoded PNG
        std::string base64Png = generator.generateBase64Bitmap(vCard, params);
        if (base64Png.empty()) {
            std::cerr << "Failed to generate Base64 PNG QR Code." << std::endl;
        } else {
            std::cout << "Base64 PNG: " << base64Png << std::endl;
        }

        return 0;
    }*/
} // whz

