//
// Created by Pat Le Cat on 05/09/2024.
//

#include "whz_config.hpp"
#include "whz_quill_wrapper.hpp"
#include "simdjson.h"

namespace whz {

    /**
     * @brief Loads the configuration file from the given path and reads the configuration parameters into the class
     * members according to the enum ConfigParameter.
     *
     * @param sfilepath The path to the configuration file as string
     * @return bool True if the configuration file was loaded and read successfully, false otherwise
     */
    bool Config::read_config(const std::string& sfilepath) {
        bool bRet = false;

        // Check that the file sfilepath exists
        if (sfilepath.empty() != true || std::filesystem::exists(sfilepath) == true) {
            // Read the file with simdjson check for errors
            simdjson::dom::parser parser;
            simdjson::dom::element doc;
            std::cout << "Before parser.load() of config file: " << sfilepath << std::endl;
            simdjson::error_code error = parser.load(sfilepath).get(doc);
            if (error) {
                std::cerr << "Config Read Error (simdjson): " << error << std::endl;
                //this->_qlogger.error(fmt::format("Config Error: {}", static_cast<int>(error)));
            } else {
                std::cout << "After successful parser.load() of config file: " << sfilepath << std::endl;
                std::string key = "";

                for (auto& param: doc.get_object()) {
                    key = param.key;
                    std::cout << "Key value: " << key << std::endl;
                    ConfigParameter paramEnum = ConfigParameter::UNKNOWN;
                    // ----- SERVER -----
                    if (key == "SERVER_HTTP_PORT") paramEnum = ConfigParameter::SERVER_HTTP_PORT;
                    else if (key == "SERVER_HTTPS_PORT") paramEnum = ConfigParameter::SERVER_HTTPS_PORT;
                    else if (key == "SERVER_ROOTPATH") paramEnum = ConfigParameter::SERVER_ROOTPATH;
                    else if (key == "SERVER_LOGPATH") paramEnum = ConfigParameter::SERVER_LOGPATH;
                    //else if (key == "SERVER_LOG_LEVEL_MIN") paramEnum = ConfigParameter::SERVER_LOG_LEVEL_MIN;
                    //else if (key == "SERVER_LOG_FILENAME") paramEnum = ConfigParameter::SERVER_LOG_FILENAME;
                    //else if (key == "SERVER_LOG_POSTFIX") paramEnum = ConfigParameter::SERVER_LOG_POSTFIX;
                    //else if (key == "SERVER_LOG_ROTATION") paramEnum = ConfigParameter::SERVER_LOG_ROTATION;
                    else if (key == "CONNECTION_TIMEOUT_MS") paramEnum = ConfigParameter::CONNECTION_TIMEOUT_MS;
                    else if (key == "SERVER_DOMAINNAME") paramEnum = ConfigParameter::SERVER_DOMAINNAME;
                    else if (key == "SERVER_SSL_CERTPATH") paramEnum = ConfigParameter::SERVER_SSL_CERTPATH;
                    else if (key == "CONNECTION_MAX_IO_CONTEXTS")
                        paramEnum = ConfigParameter::CONNECTION_MAX_IO_CONTEXTS;
                    else if (key == "CONNECTION_USE_IOURING") paramEnum = ConfigParameter::CONNECTION_USE_IOURING;
                    else if (key == "THREADPOOL_SIZE") paramEnum = ConfigParameter::THREADPOOL_SIZE;
                    else if (key == "CPU_CORES") paramEnum = ConfigParameter::CPU_CORES;
                    else if (key == "REQUESTS_ACTIVE_MAX") paramEnum = ConfigParameter::REQUESTS_ACTIVE_MAX;
                    else if (key == "REQUESTS_QUEUED_MAX") paramEnum = ConfigParameter::REQUESTS_QUEUED_MAX;
                    else if (key == "AVAILABLE_NODENAMES") paramEnum = ConfigParameter::AVAILABLE_NODENAMES;
                    // ----- WHZ-CLI -----
                    else if (key == "WHZ_CLI_PATH") paramEnum = ConfigParameter::WHZ_CLI_PATH;
                    // ----- DATABASE -----
                    else if (key == "DATABASE_PATH") paramEnum = ConfigParameter::DATABASE_PATH;
                    else if (key == "DATABASE_NAME") paramEnum = ConfigParameter::DATABASE_NAME;
                    else if (key == "DATABASE_USER") paramEnum = ConfigParameter::DATABASE_USER;
                    else if (key == "DATABASE_PASSWORD") paramEnum = ConfigParameter::DATABASE_PASSWORD;
                    else if (key == "DATABASE_PORT") paramEnum = ConfigParameter::DATABASE_PORT;
                    else if (key == "DATABASE_HOST") paramEnum = ConfigParameter::DATABASE_HOST;
                    else if (key == "DATABASE_ENGINE") paramEnum = ConfigParameter::DATABASE_ENGINE;
                    // ----- LUA -----
                    else if (key == "LUA_SCRIPT_PATH") paramEnum = ConfigParameter::LUA_SCRIPT_PATH;
                    else if (key == "LUA_START_SCRIPT_FILENAME") paramEnum = ConfigParameter::LUA_START_SCRIPT_FILENAME;
                    else if (key == "LUA_GC_STEPSIZE") paramEnum = ConfigParameter::LUA_GC_STEPSIZE;
                    // ----- LOGGING -----
                    else if (key == "LOG_TRACE_L3") paramEnum = ConfigParameter::LOG_TRACE_L3;
                    else if (key == "LOG_TRACE_L2") paramEnum = ConfigParameter::LOG_TRACE_L2;
                    else if (key == "LOG_TRACE_L1") paramEnum = ConfigParameter::LOG_TRACE_L1;
                    else if (key == "LOG_DEBUG") paramEnum = ConfigParameter::LOG_DEBUG;
                    else if (key == "LOG_INFO") paramEnum = ConfigParameter::LOG_INFO;
                    else if (key == "LOG_WARNING") paramEnum = ConfigParameter::LOG_WARNING;
                    else if (key == "LOG_ERROR") paramEnum = ConfigParameter::LOG_ERROR;
                    else if (key == "LOG_CRITICAL") paramEnum = ConfigParameter::LOG_CRITICAL;
                    else if (key == "LOG_BACKTRACE") paramEnum = ConfigParameter::LOG_BACKTRACE;
                    else if (key == "LOG_FILENAME") paramEnum = ConfigParameter::LOG_FILENAME;
                    else if (key == "LOG_ROTATION_DAYS") paramEnum = ConfigParameter::LOG_ROTATION_DAYS;
                    else if (key == "LOG_ROTATION_MB") paramEnum = ConfigParameter::LOG_ROTATION_MB;
                    else if (key == "LOG_PATH") paramEnum = ConfigParameter::LOG_PATH;
                    //else if (key == "LOG_CONSOLE") paramEnum = ConfigParameter::LOG_CONSOLE;


                    switch (paramEnum) {
                        case ConfigParameter::SERVER_HTTP_PORT:
                            if (!param.value.is_null()) {
                                server_http_port = param.value.get_uint64();
                            }
                            else {
                                server_http_port = 0;
                            }
                            break;
                        case ConfigParameter::SERVER_HTTPS_PORT:
                            if (!param.value.is_null()) {
                                server_https_port = param.value.get_uint64();
                            }
                            else {
                                server_https_port = 0;
                            }
                            break;
                        case ConfigParameter::SERVER_ROOTPATH:
                            if (!param.value.is_null()) {
                                server_rootpath = param.value.get_c_str();
                            }
                            else {
                                server_rootpath = "";
                            }
                            break;
                        case ConfigParameter::SERVER_LOGPATH:
                            if (!param.value.is_null()) {
                                server_logpath = param.value.get_c_str();
                            }
                            else {
                                server_logpath = "";
                            }
                            break;
                        /*case ConfigParameter::SERVER_LOG_LEVEL_MIN:
                            server_log_level_min = param.value.get_uint64();
                            break;
                        case ConfigParameter::SERVER_LOG_FILENAME:
                            server_log_filename = std::string(param.value.get_c_str());
                            break;
                        case ConfigParameter::SERVER_LOG_POSTFIX:
                            server_log_postfix = std::string(param.value.get_c_str());
                            break;
                        case ConfigParameter::SERVER_LOG_ROTATION:
                            server_log_rotation = std::string(param.value.get_c_str());
                            break;*/
                        case ConfigParameter::CONNECTION_TIMEOUT_MS:
                            if (!param.value.is_null()) {
                                connection_timeout_ms = param.value.get_uint64();
                            }
                            else {
                                connection_timeout_ms = 5000;
                            }
                            break;
                        case ConfigParameter::SERVER_DOMAINNAME:
                            if (!param.value.is_null()) {
                                server_domainname = param.value.get_c_str();
                            }
                            else {
                                server_domainname = "";
                            }
                            break;
                        case ConfigParameter::SERVER_SSL_CERTPATH:
                            if (!param.value.is_null()) {
                                server_ssl_certpath = param.value.get_c_str();
                            }
                            else {
                                server_ssl_certpath = "";
                            }
                            break;
                        case ConfigParameter::CONNECTION_MAX_IO_CONTEXTS:
                            if (!param.value.is_null()) {
                                connection_max_io_context = param.value.get_uint64();
                            }
                            else {
                                connection_max_io_context = 100;
                            }
                            break;
                        case ConfigParameter::CONNECTION_USE_IOURING:
                            if (!param.value.is_null()) {
                                connection_use_iouring = param.value.get_bool();
                            }
                            else {
                                connection_use_iouring = false;
                            }
                            break;
                        case ConfigParameter::THREADPOOL_SIZE:
                            if (!param.value.is_null()) {
                                threadpool_size = param.value.get_uint64();
                            }
                            else {
                                threadpool_size = 100;
                            }
                            break;
                        case ConfigParameter::CPU_CORES:
                            if (!param.value.is_null()) {
                                cpu_cores = param.value.get_uint64();
                            }
                            else {
                                cpu_cores = 8;
                            }
                            break;
                        case ConfigParameter::REQUESTS_ACTIVE_MAX:
                            if (!param.value.is_null()) {
                                requests_active_max = param.value.get_uint64();
                            }
                            else {
                                requests_active_max = 8;
                            }
                            break;
                        case ConfigParameter::REQUESTS_QUEUED_MAX:
                            if (!param.value.is_null()) {
                                requests_queued_max = param.value.get_uint64();
                            }
                            else {
                                requests_queued_max = 100;
                            }
                            break;
                        case ConfigParameter::AVAILABLE_NODENAMES:
                            if (!param.value.is_null()) {
                                available_nodenames = param.value.get_c_str();
                            }
                            else {
                                available_nodenames = "";
                            }
                            break;
                        case ConfigParameter::WHZ_CLI_PATH:
                            if (!param.value.is_null()) {
                                whz_cli_path = param.value.get_c_str();
                            }
                            else {
                                whz_cli_path = "";
                            }
                            break;
                        case ConfigParameter::DATABASE_PATH:
                            if (!param.value.is_null()) {
                                database_path = param.value.get_c_str();
                            }
                            else {
                                database_path = "";
                            }
                            break;
                        case ConfigParameter::DATABASE_NAME:
                            if (!param.value.is_null()) {
                                database_name = param.value.get_c_str();
                            }
                            else {
                                database_name = "";
                            }
                            break;
                        case ConfigParameter::DATABASE_USER:
                            if (!param.value.is_null()) {
                                database_user = param.value.get_c_str();
                            }
                            else {
                                database_user = "";
                            }
                            break;
                        case ConfigParameter::DATABASE_PASSWORD:
                            if (!param.value.is_null()) {
                                database_password = param.value.get_c_str();
                            }
                            else {
                                database_password = "";
                            }
                            break;
                        case ConfigParameter::DATABASE_PORT:
                            if (!param.value.is_null()) {
                                database_port = param.value.get_uint64();
                            }
                            else {
                                database_port = 0;
                            }
                            break;
                        case ConfigParameter::DATABASE_HOST:
                            if (!param.value.is_null()) {
                                database_host = param.value.get_c_str();
                            }
                            else {
                                database_host = "";
                            }
                            break;
                        case ConfigParameter::DATABASE_ENGINE:
                            if (!param.value.is_null()) {
                                database_engine = param.value.get_c_str();
                            }
                            else {
                                database_engine = "";
                            }
                            break;
                        case ConfigParameter::LUA_SCRIPT_PATH:
                            if (!param.value.is_null()) {
                                lua_script_path = param.value.get_c_str();
                            }
                            else {
                                lua_script_path = "";
                            }
                            break;
                        case ConfigParameter::LUA_START_SCRIPT_FILENAME:
                            if (!param.value.is_null()) {
                                lua_start_script_filename = param.value.get_c_str();
                            }
                            else {
                                lua_start_script_filename = "";
                            }
                            break;
                        case ConfigParameter::LUA_GC_STEPSIZE:
                            if (!param.value.is_null()) {
                                lua_gc_stepsize = param.value.get_uint64();
                            }
                            else {
                                lua_gc_stepsize = 1000000; // 1MB
                            }
                            break;
                        case ConfigParameter::LOG_TRACE_L3:
                            if (!param.value.is_null()) {
                                log_trace_L3 = param.value.get_bool();
                            }
                            else {
                                log_trace_L3 = false;
                            }
                            break;
                        case ConfigParameter::LOG_TRACE_L2:
                            if (!param.value.is_null()) {
                                log_trace_L2 = param.value.get_bool();
                            }
                            else {
                                log_trace_L2 = false;
                            }
                            break;
                        case ConfigParameter::LOG_TRACE_L1:
                            if (!param.value.is_null()) {
                                log_trace_L1 = param.value.get_bool();
                            }
                            else {
                                log_trace_L1 = false;
                            }
                            break;
                        case ConfigParameter::LOG_DEBUG:
                            if (!param.value.is_null()) {
                                log_debug = param.value.get_bool();
                            }
                            else {
                                log_debug = false;
                            }
                            break;
                        case ConfigParameter::LOG_INFO:
                            if (!param.value.is_null()) {
                                log_info = param.value.get_bool();
                            }
                            else {
                                log_info = false;
                            }
                            break;
                        case ConfigParameter::LOG_WARNING:
                            if (!param.value.is_null()) {
                                log_warning = param.value.get_bool();
                            }
                            else {
                                log_warning = false;
                            }
                            break;
                        case ConfigParameter::LOG_ERROR:
                            if (!param.value.is_null()) {
                                log_error = param.value.get_bool();
                                std::cout << "LOG_ERROR value from file: " << param.value.get_bool() << std::endl;
                            }
                            else {
                                log_error = false;
                            }
                            break;
                        case ConfigParameter::LOG_CRITICAL:
                            if (!param.value.is_null()) {
                                log_critical = param.value.get_bool();
                            }
                            else {
                                log_critical = false;
                            }
                            break;
                        case ConfigParameter::LOG_BACKTRACE:
                            if (!param.value.is_null()) {
                                log_backtrace = param.value.get_bool();
                            }
                            else {
                                log_backtrace = false;
                            }
                            break;
                        case ConfigParameter::LOG_FILENAME:
                            if (!param.value.is_null()) {
                                log_filename = param.value.get_c_str();
                            }
                            else {
                                log_filename = "";
                            }
                            break;
                        case ConfigParameter::LOG_ROTATION_DAYS:
                            if (!param.value.is_null()) {
                                log_rotation_days = param.value.get_uint64();
                            }
                            else {
                                log_rotation_days = 1;
                            }
                            break;
                        case ConfigParameter::LOG_ROTATION_MB:
                            if (!param.value.is_null()) {
                                log_rotation_mb = param.value.get_uint64();
                            }
                            else {
                                log_rotation_mb = 50;
                            }
                            break;
                        case ConfigParameter::LOG_PATH:
                            if (!param.value.is_null()) {
                                log_path = param.value.get_c_str();
                            }
                            else {
                                log_path = "";
                            }
                            break;
                        default:
                            break;
                    }
                }
                bRet = true;
                this->m_bConfigLoaded = true;
            }
        } else {
            // File does not exist
            std::cerr << "Config Error: File " << sfilepath << " does not exist." << std::endl;
            //this->_qlogger.error(fmt::format("Config Error: File {} does not exist.", sfilepath));
        }
        return bRet;
    }

    /**
     * @brief Get the value of a configuration parameter if it exists and if the config file exists and has been loaded.
     * The parameters are stored as std::any and need to be cast to the right type.
     *
     * @param eParam The configuration parameter (an enum) to get the value of the configuration parameter
     * @return std::any The value of the configuration parameter as any. Use std::any_cast to get the value in the right type.
     */
    std::any Config::get_config_value(Config::ConfigParameter eParam) {
        // Return the value of the class member with the same name as the enum in eParam, use switch case
        std::any value;
        switch (eParam) {
            case ConfigParameter::SERVER_HTTP_PORT:
                value = server_http_port;
                break;
            case ConfigParameter::SERVER_HTTPS_PORT:
                value = server_https_port;
                break;
            case ConfigParameter::SERVER_ROOTPATH:
                value = server_rootpath;
                break;
            case ConfigParameter::SERVER_LOGPATH:
                value = server_logpath;
                break;
            /*case ConfigParameter::SERVER_LOG_LEVEL_MIN:
                value = server_log_level_min;
                break;
            case ConfigParameter::SERVER_LOG_FILENAME:
                value = server_log_filename;
                break;
            case ConfigParameter::SERVER_LOG_POSTFIX:
                value = server_log_postfix;
                break;
            case ConfigParameter::SERVER_LOG_ROTATION:
                value = server_log_rotation;
                break;*/
            case ConfigParameter::CONNECTION_TIMEOUT_MS:
                value = connection_timeout_ms;
                break;
            case ConfigParameter::SERVER_DOMAINNAME:
                value = server_domainname;
                break;
            case ConfigParameter::SERVER_SSL_CERTPATH:
                value = server_ssl_certpath;
                break;
            case ConfigParameter::CONNECTION_MAX_IO_CONTEXTS:
                value = connection_max_io_context;
                break;
            case ConfigParameter::CONNECTION_USE_IOURING:
                value = connection_use_iouring;
                break;
            case ConfigParameter::THREADPOOL_SIZE:
                value = threadpool_size;
                break;
            case ConfigParameter::CPU_CORES:
                value = cpu_cores;
                break;
            case ConfigParameter::REQUESTS_ACTIVE_MAX:
                value = requests_active_max;
                break;
            case ConfigParameter::REQUESTS_QUEUED_MAX:
                value = requests_queued_max;
                break;
            case ConfigParameter::AVAILABLE_NODENAMES:
                value = available_nodenames;
                break;
            case ConfigParameter::WHZ_CLI_PATH:
                value = whz_cli_path;
                break;
            case ConfigParameter::DATABASE_PATH:
                value = database_path;
                break;
            case ConfigParameter::DATABASE_NAME:
                value = database_name;
                break;
            case ConfigParameter::DATABASE_USER:
                value = database_user;
                break;
            case ConfigParameter::DATABASE_PASSWORD:
                value = database_password;
                break;
            case ConfigParameter::DATABASE_PORT:
                value = database_port;
                break;
            case ConfigParameter::DATABASE_HOST:
                value = database_host;
                break;
            case ConfigParameter::DATABASE_ENGINE:
                value = database_engine;
                break;
            case ConfigParameter::LUA_SCRIPT_PATH:
                value = lua_script_path;
                break;
            case ConfigParameter::LUA_START_SCRIPT_FILENAME:
                value = lua_start_script_filename;
                break;
            case ConfigParameter::LUA_GC_STEPSIZE:
                value = lua_gc_stepsize;
                break;
            case ConfigParameter::LOG_TRACE_L3:
                value = log_trace_L3;
                break;
            case ConfigParameter::LOG_TRACE_L2:
                value = log_trace_L2;
                break;
            case ConfigParameter::LOG_TRACE_L1:
                value = log_trace_L1;
                break;
            case ConfigParameter::LOG_DEBUG:
                value = log_debug;
                break;
            case ConfigParameter::LOG_INFO:
                value = log_info;
                break;
            case ConfigParameter::LOG_WARNING:
                value = log_warning;
                break;
            case ConfigParameter::LOG_ERROR:
                value = log_error;
                break;
            case ConfigParameter::LOG_CRITICAL:
                value = log_critical;
                break;
            case ConfigParameter::LOG_BACKTRACE:
                value = log_backtrace;
                break;
            case ConfigParameter::LOG_FILENAME:
                value = log_filename;
                break;
            case ConfigParameter::LOG_ROTATION_DAYS:
                value = log_rotation_days;
                break;
            case ConfigParameter::LOG_ROTATION_MB:
                value = log_rotation_mb;
                break;
            case ConfigParameter::LOG_PATH:
                value = log_path;
                break;
            default:
                value = "Unknown";
                break;
        }
        return value;
    }

    bool Config::createJSON_config(const std::string& output_filepath) {
        bool bRet = false;
        simdjson::dom::object json_config;

        // Iterate over the enum values and retrieve their configuration values
        for (uint8_t i = static_cast<uint8_t>(ConfigParameter::SERVER_HTTP_PORT);
             i <= static_cast<uint8_t>(ConfigParameter::LOG_PATH); ++i) {
            ConfigParameter param = static_cast<ConfigParameter>(i);
            std::string pname = std::any_cast<std::string>(this->get_config_value(param));
            json_config[pname];
        }

        // Write the JSON object to a file
        std::ofstream output_file(output_filepath);
        if (output_file.is_open()) {
            output_file << simdjson::to_string(json_config); // Convert the JSON object to a string
            output_file.close();
        } else {
            this->_qlogger.error(fmt::format("Unable to open file for writing: {}", output_filepath));
            return bRet;
        }
        bRet = true;
        return bRet;
    }

} // whz
