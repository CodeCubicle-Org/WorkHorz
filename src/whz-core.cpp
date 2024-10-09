#include <filesystem>
#include <print>
#include <optional>

#include "quill/LogMacros.h"
//#include "quill/logger.h"
#include "CLI/CLI.hpp"

#include "whz_quill_wrapper.hpp"
#include "whz_server.hpp"
#include "LocalizationManager.hpp"
#include "whz_encryption.hpp"

//extern quill::Logger *whz_qlogger::getInstance().getLogger();
using namespace whz;

auto main(int argc, char **argv) -> int {
    //setup_quill();

    CLI::App app{"WorkHorz server"};
    argv = app.ensure_utf8(argv);
    whz::whz_qlogger qlogger;

    std::string config_path;
    [[maybe_unused]] CLI::Option *opt = app.add_option("-c, --config,config", config_path,
                                                       "Path to WorkHorz config file");

    CLI11_PARSE(app, argc, argv);

    if (config_path.empty()) {
        qlogger.info("Using default config");
        //LOG_INFO(whz_qlogger::getInstance().getLogger(), "Using default config");
    }

    std::filesystem::path path{"/tmp/whz"};

    // --------------------------------------------------------------------------------
    /// Testing the localization manager & translations
    qlogger.info("Testing Localization Manager");
    //LOG_INFO(whz_qlogger::getInstance().getLogger(), "Testing Localization Manager");
    // Initialize the localization manager
    auto& locManager = LocalizationManager::getInstance();
    locManager.addLocale("path/to/locale", "messages", "en_US");
    locManager.addLocale("path/to/locale", "messages", "de_DE");

    // Switch to English and translate a string
    locManager.switchLocale("en_US");
    std::string translated_en = locManager.translate("Hello, my World!");
    std::cout << "UTF-8: " << translated_en << std::endl;
    std::cout << "Latin1: " << locManager.toLatin1(translated_en) << "\n";

    // Switch to French and translate a string
    locManager.switchLocale("de_DE");
    std::string translated_de = locManager.translate("Hallo, meine Welt!");
    std::cout << "UTF-8: " << translated_de << std::endl;
    std::cout << "Latin1: " << locManager.toLatin1(translated_de) << "\n";

    // Convert back from Latin1 to UTF-8
    std::string utf8_from_latin1 = locManager.toUtf8(locManager.toLatin1(translated_de));
    std::cout << "UTF-8 from Latin1: " << utf8_from_latin1 << "\n";

    // Validate UTF-8 string
    bool isValid = locManager.isValidUtf8(translated_de);
    std::cout << "Is valid UTF-8: " << std::boolalpha << isValid << "\n";
    // --------------------------------------------------------------------------------
    std::cout << std::endl;

    // --------------------------------------------------------------------------------
    /// Testing the encryption utilities
    qlogger.info("Test Encryption");
    //LOG_INFO(whz_qlogger::getInstance().getLogger(), "Test Encryption");
    // Generate key pair
    whz::whz_encryption secureUtils;
    std::vector<unsigned char> publicKey, secretKey;
    secureUtils.generateKeyPair(publicKey, secretKey);
    std::cout << "Generated key pair." << "\n";

    // Validate key pair
    bool key_valid = secureUtils.validateKeyPair(publicKey, secretKey);
    std::cout << "Key validation: " << (key_valid ? "Valid" : "Invalid") << "\n";

    // Password hashing and verification
    std::string password = "secure_password123";
    std::string hashed_password = secureUtils.hashPassword(password);
    std::cout << "Hashed Password: " << hashed_password << "\n";
    bool is_valid = secureUtils.verifyPassword(hashed_password, password);
    std::cout << "Password verification: " << (is_valid ? "Valid" : "Invalid") << "\n";

    // CSRF Token creation
    std::string csrf_token = secureUtils.createCSRFToken();
    std::cout << "CSRF Token: " << std::hex << csrf_token << "\n";

    // File encryption
    // Note: Replace `publicKey` with an actual generated public key in production use
    std::vector<unsigned char> publicKey2(crypto_box_PUBLICKEYBYTES);
    randombytes_buf(publicKey2.data(), publicKey2.size());
    secureUtils.encryptFile("example.txt", publicKey2);

    // Signed message creation and verification
    std::string message = "This is a secure message.";
    std::vector<unsigned char> secretKey2(crypto_sign_SECRETKEYBYTES);
    std::vector<unsigned char> pubKey(crypto_sign_PUBLICKEYBYTES);
    crypto_sign_keypair(pubKey.data(), secretKey2.data());

    std::vector<unsigned char> signed_message = secureUtils.createSignedMessage(message, secretKey2);
    std::string verified_message = secureUtils.verifySignedMessage(signed_message, pubKey);
    std::cout << "Verified Message: " << verified_message << "\n";
    // --------------------------------------------------------------------------------
    std::cout << std::endl;

    qlogger.info("Starting WHZ");
    //LOG_INFO(whz_qlogger::getInstance().getLogger(), "Starting WHZ");
    whz::server s{"0.0.0.0", 8080, std::move(path), 1};

    s.listen_and_serve();
    return 0;
}

/**
 * \mainpage whz-core.cpp
 * \author WorkHorz developers
 * \version 0.0.1
 * \see http://github.com/code-cubicle.org/workhorz
 *
 * Main entrypoint for WorkHorz
 */
/*
auto main() -> int {

 std::filesystem::path path{"/tmp/whz"};

 // SETUP Stage (low level C++)
 // Initialise the base objects that are part of the core (not in the nodes)
 // Load the configured system nodes dynamically and let them initialise themselves
 // Load the configuration file and process it (e.g. validate paths and settings)
 // Check the available resources (CPU, memory, disk space, etc.), adjust config if necessary
 // Create the LUA context and check the existence of the LUA start-script, execute startup operations if needed
 // Create the database object(s) and connect to the database(s), execute startup operations if needed
 // Connect to the other whz-cores if configured
 // Load the configured user nodes dynamically and let them initialise themselves
 // Create the server object and start listening in the server
 // Start the LUA start-script

 // SETUP Stage (high level C++ and LUA)
 // Initialise user objects, attach dynamic nodes (user + system) and load the config
 // Create the server object and start listening in the server

 // --------------------------------------------------------------------------------

 // REQUEST Processing Stage (low level C++)
 // Pull the next requests from the io_uring buffer and assign reused contexts and threads/tasks to them, they are queued
 // Execute as many requests as we have CPU cores configured and available, they are executed in parallel
 // REQ: Parse the request (HTTP header) and validate it, check the caller permissions and the rate limits
 // REQ: Validate any user input and request parameters and escape/sanitize them before further processing
 // REQ: Create a user-session object and cache it if necessary.
 // REQ: Find the page from the path and the query parameters, check the cache.
 // REQ: Assemble the page and its resources.
 // REQ: Parse the page template and process them + fill the data in it, render the page
 // REQ: Construct HTTP header and serve the page data and resources requested
 // Free the context and the thread/task

 // REQUEST Processing Stage (high level C++ and LUA)
 // REQ: Process request and receive the data of it
 // REQ: Apply the business logic and assemble the page data
 // REQ: Render the page and serve it
 // REQ: Close request.


 whz::server s{"0.0.0.0", 8080, std::move(path), 1};

 s.listen_and_serve();
 return 0;
}
*/