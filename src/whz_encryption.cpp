//
// Created by Pat Le Cat on 04/10/2024.
//

#include "whz_encryption.hpp"

namespace whz {

    void whz_encryption::generateKeyPair(std::vector<unsigned char>&publicKey, std::vector<unsigned char>&secretKey) {
        publicKey.resize(crypto_box_PUBLICKEYBYTES);
        secretKey.resize(crypto_box_SECRETKEYBYTES);
        if (crypto_box_keypair(publicKey.data(), secretKey.data()) != 0) {
            this->_qlogger.error("Error: Failed to generate key pair.");
            std::cerr << "Error: Failed to generate key pair." << std::endl;
            return;
        }

        // Save the public key to a file
        std::ofstream publicKeyFile("public.key", std::ios::binary);
        if (!publicKeyFile) {
            this->_qlogger.error("Error: Failed to open public key file for writing.");
            std::cerr << "Error: Failed to open public key file for writing." << std::endl;
            return;
        }
        publicKeyFile.write(reinterpret_cast<char *>(publicKey.data()), publicKey.size());
        publicKeyFile.close();

        // Save the secret key to a file
        std::ofstream secretKeyFile("secret.key", std::ios::binary);
        if (!secretKeyFile) {
            this->_qlogger.error("Error: Failed to open secret key file for writing.");
            std::cerr << "Error: Failed to open secret key file for writing." << std::endl;
            return;
        }
        secretKeyFile.write(reinterpret_cast<char *>(secretKey.data()), secretKey.size());
        secretKeyFile.close();

        publicKey.resize(crypto_box_PUBLICKEYBYTES);
        secretKey.resize(crypto_box_SECRETKEYBYTES);
        if (crypto_box_keypair(publicKey.data(), secretKey.data()) != 0) {
            this->_qlogger.error("Error: Failed to generate key pair.");
            std::cerr << "Error: Failed to generate key pair." << std::endl;
        }
    }

    bool whz_encryption::validateKeyPair(const std::vector<unsigned char>&publicKey, const std::vector<unsigned char>&secretKey) {
        std::vector<unsigned char> test_message(crypto_box_SEALBYTES);
        randombytes_buf(test_message.data(), test_message.size());

        std::vector<unsigned char> ciphertext(test_message.size() + crypto_box_SEALBYTES);
        if (crypto_box_seal(ciphertext.data(), test_message.data(), test_message.size(), publicKey.data()) != 0) {
            this->_qlogger.error("Error: Failed during key validation encryption.");
            std::cerr << "Error: Failed during key validation encryption." << std::endl;
            return false;
        }

        std::vector<unsigned char> decrypted(test_message.size());
        if (crypto_box_seal_open(decrypted.data(), ciphertext.data(), ciphertext.size(), publicKey.data(), secretKey.data()) != 0) {
            this->_qlogger.error("Error: Failed during key validation decryption.");
            std::cerr << "Error: Failed during key validation decryption." << std::endl;
            return false;
        }

        return true;
    }

    void whz_encryption::decryptFile(const std::string&encryptedFilePath, const std::vector<unsigned char>&secretKey) {
        unsigned char  publicKey[crypto_box_PUBLICKEYBYTES]; // TODO: has to be removed and fixed, but how?
        std::ifstream encryptedFile(encryptedFilePath, std::ios::binary);
        if (!encryptedFile) {
            this->_qlogger.error("Error: Failed to open encrypted file.");
            //LOG_ERROR(whz_logger, "Error: Failed to open encrypted file.");
            std::cerr << "Error: Failed to open encrypted file." << std::endl;
            return;
        }

        std::vector<unsigned char> ciphertext((std::istreambuf_iterator<char>(encryptedFile)),
                                              std::istreambuf_iterator<char>());
        encryptedFile.close();

        if (ciphertext.size() < crypto_box_SEALBYTES) {
            this->_qlogger.error("Error: Ciphertext is too short.");
            //LOG_ERROR(whz_logger, "Error: Ciphertext is too short.");
            std::cerr << "Error: Ciphertext is too short." << std::endl;
            return;
        }

        std::vector<unsigned char> plaintext(ciphertext.size() - crypto_box_SEALBYTES);
        if (crypto_box_seal_open(plaintext.data(), ciphertext.data(), ciphertext.size(), publicKey, secretKey.data()) != 0) {
            this->_qlogger.error("Error: Failed to decrypt file.");
            //LOG_ERROR(whz_logger, "Error: Failed to decrypt file.");
            std::cerr << "Error: Failed to decrypt file." << std::endl;
            return;
        }

        std::ofstream decryptedFile(encryptedFilePath.substr(0, encryptedFilePath.find_last_of('.')),
                                    std::ios::binary);
        decryptedFile.write(reinterpret_cast<char *>(plaintext.data()), plaintext.size());
    }

    std::string whz_encryption::hashPassword(const std::string&password) {
        std::vector<unsigned char> hashed_password(crypto_pwhash_STRBYTES);
        if (crypto_pwhash_str(reinterpret_cast<char *>(hashed_password.data()), password.c_str(), password.length(),
                              crypto_pwhash_OPSLIMIT_MODERATE, crypto_pwhash_MEMLIMIT_MODERATE) != 0) {
            this->_qlogger.error("Error: Failed to hash password.");
            //LOG_ERROR(whz_logger, "Error: Failed to hash password.");
            std::cerr << "Error: Failed to hash password." << std::endl;
        }
        return std::string(hashed_password.begin(), hashed_password.end());
    }

    bool whz_encryption::verifyPassword(const std::string&hashed_password, const std::string&password) {
        return crypto_pwhash_str_verify(hashed_password.c_str(), password.c_str(), password.length()) == 0;
    }

    std::string whz_encryption::createCSRFToken() {
        std::vector<unsigned char> token(crypto_generichash_BYTES);
        randombytes_buf(token.data(), token.size());
        return std::string(token.begin(), token.end());
    }


    void whz_encryption::encryptFile(const std::string&filePath, const std::vector<unsigned char>&publicKey) {
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            this->_qlogger.error("Error: Failed to open file.");
            //LOG_ERROR(whz_logger, "Error: Failed to open file.");
            std::cerr << "Error: Failed to open file." << std::endl;
        }

        std::vector<unsigned char> plaintext((std::istreambuf_iterator<char>(file)),
                                             std::istreambuf_iterator<char>());
        file.close();

        std::vector<unsigned char> ciphertext(plaintext.size() + crypto_box_SEALBYTES);
        if (crypto_box_seal(ciphertext.data(), plaintext.data(), plaintext.size(), publicKey.data()) != 0) {
            this->_qlogger.error("Error: Failed to encrypt file.");
            //LOG_ERROR(whz_logger, "Error: Failed to encrypt file.");
            std::cerr << "Error: Failed to encrypt file." << std::endl;
        }

        std::ofstream encryptedFile(filePath + ".enc", std::ios::binary);
        encryptedFile.write(reinterpret_cast<char *>(ciphertext.data()), ciphertext.size());
    }

    std::vector<unsigned char>
    whz_encryption::createSignedMessage(const std::string&message, const std::vector<unsigned char>&secretKey) {
        std::vector<unsigned char> signed_message(message.size() + crypto_sign_BYTES);
        crypto_sign(signed_message.data(), nullptr,
                    reinterpret_cast<const unsigned char *>(message.data()), message.size(),
                    secretKey.data());
        return signed_message;
    }

    std::string whz_encryption::verifySignedMessage(const std::vector<unsigned char>&signed_message,
                                    const std::vector<unsigned char>&publicKey) {
        std::vector<unsigned char> message(signed_message.size());
        unsigned long long message_len;
        if (crypto_sign_open(message.data(), &message_len, signed_message.data(), signed_message.size(),
                             publicKey.data()) != 0) {
            this->_qlogger.error("Error: Failed to verify signed message.");
            //LOG_ERROR(whz_logger, "Error: Failed to verify signed message.");
            std::cerr << "Error: Failed to verify signed message." << std::endl;
        }
        return std::string(message.begin(), message.begin() + message_len);
    }

} // whz