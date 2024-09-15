//
// Created by Pat Le Cat on 05/09/2024.
//

#pragma once

#include <simdjson.h>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <any>

namespace whz {

    class Config {
    public:
        Config() = default;
        ~Config() = default;

        bool read_config(std::string sfilepath = "");
        [[nodiscard]] bool is_config_loaded() const { return m_bConfigLoaded; };
        bool relaod_config() { return read_config(); };

        enum class ConfigParameter: uint8_t {
            UNKNOWN = 0,            // Reserved, do not use
            SERVER_HTTP_PORT,       // HTTP port to listen on
            SERVER_HTTPS_PORT,      // HTTPS port to listen on
            SERVER_ROOTPATH,        // Base path of the server in which all server files are located (in subdirs)
            SERVER_LOGPATH,         // Path to the log files
            SERVER_LOG_LEVEL_MIN,   // Minimum log level at which to log
            SERVER_LOG_FILENAME,    // Filename of the log file (prefix)
            SERVER_LOG_POSTFIX,     // Postfix of the log file (POSTFIX)
            SERVER_LOG_ROTATION,    // Log rotation in days (e.g. "3d") or in Megabytes (e.g. "50MB")
            CONNECTION_TIMEOUT_MS,  // Connection timeout in milliseconds
            SERVER_DOMAINNAME,      // Base-domain name of the server e.g. myserver.ch
            SERVER_SSL_CERTPATH,    // Path to the SSL certificate files (PEM)
            CONNECTION_MAX_IO_CONTEXTS, // Maximum number of I/O contexts to use in the pool
            CONNECTION_USE_IOURING, // Use io_uring for async I/O, else use epoll
            THREADPOOL_SIZE,        // Number of threads in the thread pool for handling requests
            CPU_CORES,              // Number of CPU cores found at startup of whz_core
            REQUESTS_ACTIVE_MAX,    // Maximum number of active requests to prepare to use immediately
            REQUESTS_QUEUED_MAX,    // Maximum number of queued requests to be pulled from the io_uring buffer
            AVAILABLE_NODENAMES,    // List of node binaries found in path
            WHZ_CLI_PATH,           // Path to the whz-cli binary
            DATABASE_PATH,          // Path to the SQLite database file
            DATABASE_NAME,          // Name of the SQLite database to use
            DATABASE_USER,          // Username to use for the database
            DATABASE_PASSWORD,      // Password to use for the database
            DATABASE_PORT,          // Port to use for the database
            DATABASE_HOST,          // Hostname to use for the database
            DATABASE_ENGINE,        // Currently only SQLite
            LUA_SCRIPT_PATH,        // Path to the user Lua scripts
            LUA_START_SCRIPT_FILENAME,  // Filename of the Lua script to run at startup
        };

        std::any get_config_value(ConfigParameter eParam);

    private:
        bool m_bConfigLoaded = false;
        std::any server_http_port;
        std::any server_https_port;
        std::any server_rootpath;
        std::any server_logpath;
        std::any server_log_level_min;
        std::any server_log_filename;
        std::any server_log_postfix;
        std::any server_log_rotation;
        std::any connection_timeout_ms;
        std::any server_domainname;
        std::any server_ssl_certpath;
        std::any connection_max_io_context;
        std::any connection_use_iouring;
        std::any threadpool_size;
        std::any cpu_cores;
        std::any requests_active_max;
        std::any requests_queued_max;
        std::any available_nodenames;
        std::any whz_cli_path;
        std::any database_path;
        std::any database_name;
        std::any database_user;
        std::any database_password;
        std::any database_port;
        std::any database_host;
        std::any database_engine;
        std::any lua_script_path;
        std::any lua_start_script_filename;
    };

} // whz
