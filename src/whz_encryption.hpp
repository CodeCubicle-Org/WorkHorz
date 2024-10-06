//
// Created by Pat Le Cat on 04/10/2024.
//

#pragma once

#include <sodium.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

namespace whz {

    class whz_encryption {

    public:
        /**
         * @brief Constructs the SecureUtils object and initializes libsodium.
         *
         * This method must be called before using any of the other methods provided by SecureUtils.
         */
        whz_encryption() {
            if (sodium_init() < 0) {
                std::cerr << "Error: Failed to initialize libsodium." << std::endl;
            }
        }

        ~whz_encryption() = default;

        /**
         * @brief Generates a public and private key pair for encryption and decryption.
         *
         * The generated keys are saved to files named "public.key" and "secret.key".
         * @param publicKey A vector to store the generated public key.
         * @param secretKey A vector to store the generated secret key.
         */
        void generateKeyPair(std::vector<unsigned char>& publicKey, std::vector<unsigned char>& secretKey);


        /**
         * @brief Validates a public and private key pair by encrypting and decrypting a test message.
         *
         * @param publicKey The public key to validate.
         * @param secretKey The secret key to validate.
         * @return True if the keys are valid, false otherwise.
         */
        bool validateKeyPair(const std::vector<unsigned char>& publicKey, const std::vector<unsigned char>& secretKey);

        /**
         * @brief Decrypts an encrypted file using the provided secret key.
         *
         * The decrypted content is saved to a new file with the same name but without the ".enc" extension.
         * @param encryptedFilePath The path to the encrypted file.
         * @param secretKey The secret key used for decryption.
         */
        void decryptFile(const std::string& encryptedFilePath, const std::vector<unsigned char>& secretKey);


        /**
         * @brief Hashes a password securely using the Argon2 algorithm.
         *
         * @param password The password to hash.
         * @return A hashed password string.
         */
        std::string hashPassword(const std::string& password);


        /**
         * @brief Verifies if a given password matches the hashed password.
         *
         * @param hashed_password The hashed password.
         * @param password The password to verify.
         * @return True if the password matches, false otherwise.
         */
        bool verifyPassword(const std::string& hashed_password, const std::string& password);

        /**
         * @brief Creates a CSRF token.
         *
         * @return A CSRF token string.
         */
        std::string createCSRFToken();

        /**
         * @brief Encrypts a file using the provided public key.
         *
         * The encrypted content is saved to a new file with the ".enc" extension.
         * @param filePath The path to the file to encrypt.
         * @param publicKey The public key used for encryption.
         */
        void encryptFile(const std::string& filePath, const std::vector<unsigned char>& publicKey);

        /**
         * @brief Creates a signed message using the provided secret key.
         *
         * @param message The message to sign.
         * @param secretKey The secret key used for signing.
         * @return A vector containing the signed message.
         */
        std::vector<unsigned char>
        createSignedMessage(const std::string& message, const std::vector<unsigned char>& secretKey);

        /**
         * @brief Verifies and extracts a signed message using the provided public key.
         *
         * @param signed_message The signed message to verify.
         * @param publicKey The public key used for verification.
         * @return The original message if verification is successful.
         */
        std::string verifySignedMessage(const std::vector<unsigned char>& signed_message,
                                        const std::vector<unsigned char>& publicKey);
    };
} // whz namespace

/*
int main() {
    // Generate key pair
    std::vector<unsigned char> publicKey, secretKey;
    secureUtils.generateKeyPair(publicKey, secretKey);
    std::cout << "Generated key pair." << std::endl;

    // Validate key pair
    bool key_valid = secureUtils.validateKeyPair(publicKey, secretKey);
    std::cout << "Key validation: " << (key_valid ? "Valid" : "Invalid") << std::endl;
    whz_encryption secureUtils;

        // Password hashing and verification
        std::string password = "secure_password123";
        std::string hashed_password = secureUtils.hashPassword(password);
        std::cout << "Hashed Password: " << hashed_password << std::endl;
        bool is_valid = secureUtils.verifyPassword(hashed_password, password);
        std::cout << "Password verification: " << (is_valid ? "Valid" : "Invalid") << std::endl;

        // CSRF Token creation
        std::string csrf_token = secureUtils.createCSRFToken();
        std::cout << "CSRF Token: " << csrf_token << std::endl;

        // File encryption
        // Note: Replace `publicKey` with an actual generated public key in production use
        std::vector<unsigned char> publicKey(crypto_box_PUBLICKEYBYTES);
        randombytes_buf(publicKey.data(), publicKey.size());
        secureUtils.encryptFile("example.txt", publicKey);

        // Signed message creation and verification
        std::string message = "This is a secure message.";
        std::vector<unsigned char> secretKey(crypto_sign_SECRETKEYBYTES);
        std::vector<unsigned char> pubKey(crypto_sign_PUBLICKEYBYTES);
        crypto_sign_keypair(pubKey.data(), secretKey.data());

        std::vector<unsigned char> signed_message = secureUtils.createSignedMessage(message, secretKey);
        std::string verified_message = secureUtils.verifySignedMessage(signed_message, pubKey);
        std::cout << "Verified Message: " << verified_message << std::endl;

    return 0;
}
*/


