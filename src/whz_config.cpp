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

                for (auto&param: doc.get_object()) {
                    key = std::any_cast<std::string const&>(param.key);

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
                            server_http_port = param.value.get_uint64();
                            break;
                        case ConfigParameter::SERVER_HTTPS_PORT:
                            server_https_port = param.value.get_uint64();
                            break;
                        case ConfigParameter::SERVER_ROOTPATH:
                            server_rootpath = std::string(param.value.get_c_str());
                            break;
                        case ConfigParameter::SERVER_LOGPATH:
                            server_logpath = std::string(param.value.get_c_str());
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
                            connection_timeout_ms = param.value.get_uint64();
                            break;
                        case ConfigParameter::SERVER_DOMAINNAME:
                            server_domainname = std::string(param.value.get_c_str());
                            break;
                        case ConfigParameter::SERVER_SSL_CERTPATH:
                            server_ssl_certpath = std::string(param.value.get_c_str());
                            break;
                        case ConfigParameter::CONNECTION_MAX_IO_CONTEXTS:
                            connection_max_io_context = param.value.get_uint64();
                            break;
                        case ConfigParameter::CONNECTION_USE_IOURING:
                            connection_use_iouring = param.value.get_bool();
                            break;
                        case ConfigParameter::THREADPOOL_SIZE:
                            threadpool_size = param.value.get_uint64();
                            break;
                        case ConfigParameter::CPU_CORES:
                            cpu_cores = param.value.get_uint64();
                            break;
                        case ConfigParameter::REQUESTS_ACTIVE_MAX:
                            requests_active_max = param.value.get_uint64();
                            break;
                        case ConfigParameter::REQUESTS_QUEUED_MAX:
                            requests_queued_max = param.value.get_uint64();
                            break;
                        case ConfigParameter::AVAILABLE_NODENAMES:
                            available_nodenames = std::string(param.value.get_c_str());
                            break;
                        case ConfigParameter::WHZ_CLI_PATH:
                            whz_cli_path = std::string(param.value.get_c_str());
                            break;
                        case ConfigParameter::DATABASE_PATH:
                            database_path = std::string(param.value.get_c_str());
                            break;
                        case ConfigParameter::DATABASE_NAME:
                            database_name = std::string(param.value.get_c_str());
                            break;
                        case ConfigParameter::DATABASE_USER:
                            database_user = std::string(param.value.get_c_str());
                            break;
                        case ConfigParameter::DATABASE_PASSWORD:
                            database_password = std::string(param.value.get_c_str());
                            break;
                        case ConfigParameter::DATABASE_PORT:
                            database_port = param.value.get_uint64();
                            break;
                        case ConfigParameter::DATABASE_HOST:
                            database_host = std::string(param.value.get_c_str());
                            break;
                        case ConfigParameter::DATABASE_ENGINE:
                            database_engine = std::string(param.value.get_c_str());
                            break;
                        case ConfigParameter::LUA_SCRIPT_PATH:
                            lua_script_path = std::string(param.value.get_c_str());
                            break;
                        case ConfigParameter::LUA_START_SCRIPT_FILENAME:
                            lua_start_script_filename = std::string(param.value.get_c_str());
                            break;
                        case ConfigParameter::LUA_GC_STEPSIZE:
                            lua_gc_stepsize = param.value.get_uint64();
                            break;
                        case ConfigParameter::LOG_TRACE_L3:
                            log_trace_L3 = param.value.get_bool();
                            break;
                        case ConfigParameter::LOG_TRACE_L2:
                            log_trace_L2 = param.value.get_bool();
                            break;
                        case ConfigParameter::LOG_TRACE_L1:
                            log_trace_L1 = param.value.get_bool();
                            break;
                        case ConfigParameter::LOG_DEBUG:
                            log_debug = param.value.get_bool();
                            break;
                        case ConfigParameter::LOG_INFO:
                            log_info = param.value.get_bool();
                            break;
                        case ConfigParameter::LOG_WARNING:
                            log_warning = param.value.get_bool();
                            break;
                        case ConfigParameter::LOG_ERROR:
                            log_error = param.value.get_bool();
                            break;
                        case ConfigParameter::LOG_CRITICAL:
                            log_critical = param.value.get_bool();
                            break;
                        case ConfigParameter::LOG_BACKTRACE:
                            log_backtrace = param.value.get_bool();
                            break;
                        case ConfigParameter::LOG_FILENAME:
                            log_filename = std::string(param.value.get_c_str());
                            break;
                        case ConfigParameter::LOG_ROTATION_DAYS:
                            log_rotation_days = param.value.get_uint64();
                            break;
                        case ConfigParameter::LOG_ROTATION_MB:
                            log_rotation_mb = param.value.get_uint64();
                            break;
                        case ConfigParameter::LOG_PATH:
                            log_path = std::string(param.value.get_c_str());
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
