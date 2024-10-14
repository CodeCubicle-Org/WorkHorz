#include <filesystem>
#include <print>
#include <optional>
#include <iostream>

#include "CLI/CLI.hpp"
#include "whz_quill_wrapper.hpp"
#include "whz_server.hpp"
#include "LocalizationManager.hpp"
#include "whz_encryption.hpp"
#include "whz_datacompression.hpp"
#include "whz_config.hpp"
//#include "whz_templating.hpp"
#include "whz_vcard.hpp"
#include "whz_utils.hpp"
#include "whz_qrcode_generator.hpp"

#define WHZ_VERSION "0.0.1"

using namespace whz;

auto main(int argc, char **argv) -> int {

    std::cout << "*** Start of server" << std::endl;
    CLI::App app{fmt::format("WorkHorz Web-App Server - Version: {}", WHZ_VERSION)};
    // argv = app.ensure_utf8(argv); // Only useful on Windows
    std::cout << "*** Start of CLI11" << std::endl;

    std::string config_path;
    [[maybe_unused]] CLI::Option *opt = app.add_option("-c, --config", config_path,
                                                       "Path to WorkHorz config file, default: ./whz_config.json");

    // Read the commandline arguments
    try {
        app.parse(argc, argv);
    } catch(const CLI::ParseError &e) {
        std::cerr << "ERROR: Commandline parsing failed, aborting." << e.what() << std::endl;
        exit(app.exit(e)); // Exit command from CLI11
    }

    // Create the config object
    whz::Config::get_instance();

    // Config must first be read successfully, else abort! Logging can only be used after a successful config read
    std::cout << "Before Config reading..." << std::endl;
    if (config_path.empty()) {
        std::cerr << "No config file provided at commandline, using default config of ./whz_config.json" << std::endl;
        if (!whz::Config::get_instance().read_config("whz_config.json")) {
            std::cerr << "Error reading configuration file" << std::endl;
            return 1;
        }
        std::cout << "After successful Config reading..." << std::endl;
    }
    else {
        std::cout << "Using the provided config file-path: " << config_path << std::endl;
        if (!whz::Config::get_instance().read_config(config_path)) {
            std::cerr << "Error reading configuration file" << std::endl;
            return 1;
        }
        std::cout << "After successful Config reading..." << std::endl;
    }

    whz::whz_qlogger qlogger;
    qlogger.info("My first logging message ma! :D");
    std::cout << "After 1st logging call..." << std::endl;

    std::filesystem::path path{"/tmp/whz"};

    // --------------------------------------------------------------------------------
    /// Testing the localization manager & translations
    qlogger.info("Testing Localization Manager");
    std::cout << "Testing Localization Manager" << "\n";
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
    std::cout << "Test Encryption" << "\n";
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
    std::cout << "Encrypting file with public key..." << "\n";
    std::vector<unsigned char> publicKey2(crypto_box_PUBLICKEYBYTES);
    randombytes_buf(publicKey2.data(), publicKey2.size());
    secureUtils.encryptFile("whz_config.json", publicKey2);

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
    // --------------------------------------------------------------------------------
    /// Testing the file compression utilities
    qlogger.info("Test File Compression");
    std::cout << "Test File Compression" << "\n";
    whz::whz_datacompression dchandler;
    std::vector<fs::path> files = {"Baudelaire_pg36287.txt", "Homer-Iliad_pg6130.txt"};
    dchandler.compress(files, "zipped_output.zip", ".zip");
    dchandler.compress(files, "7zipped_output.7z", ".7z");
    dchandler.decompress("zipped_output.zip", "zip_output_dir");
    dchandler.decompress("7zipped_output.zip", "7z_output_dir");

    // Compress and decompress a directory recursively
    dchandler.compressDirectory("external", "zipped_output_dir.zip", ".zip");
    dchandler.compressDirectory("external", "7zipped_output_dir.7z", ".7z");
    dchandler.decompressToDirectory("zipped_output_dir.zip", "zip_output_dir_unzipped");
    dchandler.decompressToDirectory("7zipped_output_dir.zip", "7zip_output_dir_unzipped");
    // --------------------------------------------------------------------------------
    std::cout << std::endl;
    // --------------------------------------------------------------------------------
    /// Writing out the latest configuration parameters to a JSON file
    /*
    if (whz::Config::get_instance().is_config_loaded()) {
        bool bRet = whz::Config::get_instance().createJSON_config("whz_config2.json");  // Create in same folder as executable
        if (bRet) {
            qlogger.info("Configuration written to JSON file.");
            std::cout << "Configuration written to JSON file.\n";
        } else {
            qlogger.error("Error writing configuration to JSON file.");
            std::cerr << "Error writing configuration to JSON file.\n";
        }
    }*/
    // --------------------------------------------------------------------------------
    std::cout << std::endl;
    // --------------------------------------------------------------------------------
    /// Testing the TemplateProcessor
    /*auto processor_exp = TemplateProcessor::Create("database.db");
    if (!processor_exp) {
        std::cerr << "Error: " << processor_exp.error() << std::endl;
        return 1;
    }
    auto processor = std::move(*processor_exp);

    // Set the TemplateDefiner instance
    auto template_definer = std::make_shared<TemplateDefiner>();
    processor.SetTemplateDefiner(template_definer);

    // Process the template
    auto result = processor.ProcessTemplate("template.whzt");
    if (result) {
        std::cout << "Rendered Template:\n" << *result << std::endl;
    } else {
        std::cerr << "Error: " << result.error() << std::endl;
    }*/
    // --------------------------------------------------------------------------------
    std::cout << std::endl;
    // --------------------------------------------------------------------------------
    /// Testing the VCard
    std::cout << "Testing VCard" << "\n";
    qlogger.info("Testing VCard");
    whz_vcard vcard;
    vcard.addProperty("FN", "Johann Döttinger");
    vcard.addProperty("N", "Döttinger;Johann;;;");
    vcard.addProperty("EMAIL", { { "TYPE", "Work" } }, "johann.doettinger@example.com");
    vcard.addProperty("TEL", { { "TYPE", "Mobile" } }, "+4179456987");
    vcard.addAddress({ { "TYPE", "Home" } }, { "", "", "Strassenweg 11", "Zürich", "ZH", "8000", "Switzerland" });
    vcard.addRFC6350Field("URL", { { "TYPE", "Work" } }, "https://codecubicle.ch");

    std::string vcardString = vcard.toString();
    std::cout << "VCard version 4 (RFC6350):\n-------------------------\n" << vcardString << "-------------------------";
    // --------------------------------------------------------------------------------
    std::cout << std::endl;
    // --------------------------------------------------------------------------------
    /// Testing the Utils
    std::cout << "Testing Utils" << "\n";
    qlogger.info("Testing Utils");
    std::string test_str = "Hello, \x01\x02\x03 world! \u3053\u3093\u306b\u3061\u306f";
    std::cout << "Testing Utils\nUnsanitized string: " << test_str << "\n";
    std::string sanitized_str = whz::sanitize_utf8_string(test_str);
    std::cout << "Sanitized same string: " << sanitized_str << "\n";
    // --------------------------------------------------------------------------------
    std::cout << std::endl;
    // --------------------------------------------------------------------------------
    /// Testing the QR Code Generator
    std::cout << "Testing QR Code Generator" << "\n";
    qlogger.info("Testing QR Code Generator");
    whz_qrcode_generator generator;
    whz_qrcode_generator::QRCodeParams params;
    params.scale = 8;
    params.border = 2;
    params.foregroundColor = "#0000FF";
    params.backgroundColor = "#FFFFFF";
    params.errorCorrectionLevel = qrcodegen::QrCode::Ecc::MEDIUM;

    std::u8string vCard = u8"BEGIN:VCARD\nVERSION:4.0\nFN:John Doe\nORG:Example Corp\nTEL:+123456789\nEMAIL:john.doe@example.com\nEND:VCARD";

    // Generate SVG file
    if (!generator.generateQRCode(vCard, "qrcode.svg", "svg", params)) {
        std::cerr << "Failed to generate SVG QR Code." << std::endl;
        qlogger.error("Failed to generate SVG QR Code.");
    }

    // Generate PNG file
//    if (!generator.generateQRCode(vCard, "qrcode.png", "png", params)) {
//        std::cerr << "Failed to generate PNG QR Code." << std::endl;
//    }

    // Generate Base64 encoded PNG
    std::string base64Png = generator.generateBase64Bitmap(vCard, params);
    if (base64Png.empty()) {
        std::cerr << "Failed to generate Base64 PNG QR Code." << std::endl;
        qlogger.error("Failed to generate Base64 PNG QR Code.");
    } else {
        //std::cout << "Base64 PNG: " << base64Png << std::endl;
    }
    // --------------------------------------------------------------------------------
    std::cout << std::endl;


    qlogger.info("*** Starting WHZ Listening Server ***");
    std::cout << "*** Starting WHZ Listening Server ***" << std::endl;
    whz::server s{"0.0.0.0", 8080, std::move(path), 1};
    std::cout << "-   press Ctrl-C to terminate the server   -" << std::endl;

    s.listen_and_serve();
    std::cout << std::endl;
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