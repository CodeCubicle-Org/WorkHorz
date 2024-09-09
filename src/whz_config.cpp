//
// Created by Pat Le Cat on 05/09/2024.
//

#include "whz_config.hpp"

namespace whz {

    /**
     * @brief Loads the configuration file from the given path and reads the configuration parameters into the class
     * members according to the enum ConfigParameter.
     *
     * @param sfilepath The path to the configuration file as string
     * @return bool True if the configuration file was loaded and read successfully, false otherwise
     */
    bool Config::read_config(std::string sfilepath) {
        bool bRet = false;

        // Check that the file sfilepath exists
        if (std::filesystem::exists(sfilepath)) {
            // Read the file with simdjson check for errors
            simdjson::dom::parser parser;
            simdjson::dom::element doc;
            simdjson::error_code error = parser.load(sfilepath).get(doc);
            if (error) {
                std::cerr << "Error: " << error << std::endl;
            } else {
                std::string key = "";

                for (auto &param: doc.get_object()) {
                    key = std::any_cast<std::string const &>(param.key);

                    ConfigParameter paramEnum = ConfigParameter::UNKNOWN;
                    if (key == "SERVER_HTTP_PORT") paramEnum = ConfigParameter::SERVER_HTTP_PORT;
                    else if (key == "SERVER_HTTPS_PORT") paramEnum = ConfigParameter::SERVER_HTTPS_PORT;
                    else if (key == "SERVER_ROOTPATH") paramEnum = ConfigParameter::SERVER_ROOTPATH;
                    else if (key == "SERVER_LOGPATH") paramEnum = ConfigParameter::SERVER_LOGPATH;
                    else if (key == "SERVER_LOG_LEVEL_MIN") paramEnum = ConfigParameter::SERVER_LOG_LEVEL_MIN;
                    else if (key == "SERVER_LOG_FILENAME") paramEnum = ConfigParameter::SERVER_LOG_FILENAME;
                    else if (key == "SERVER_LOG_POSTFIX") paramEnum = ConfigParameter::SERVER_LOG_POSTFIX;
                    else if (key == "SERVER_LOG_ROTATION") paramEnum = ConfigParameter::SERVER_LOG_ROTATION;
                    else if (key == "CONNECTION_TIMEOUT_MS") paramEnum = ConfigParameter::CONNECTION_TIMEOUT_MS;
                    else if (key == "SERVER_DOMAINNAME") paramEnum = ConfigParameter::SERVER_DOMAINNAME;
                    else if (key == "SERVER_SSL_CERTPATH") paramEnum = ConfigParameter::SERVER_SSL_CERTPATH;
                    else if (key == "CONNECTION_MAX_IO_CONTEXTS") paramEnum = ConfigParameter::CONNECTION_MAX_IO_CONTEXTS;
                    else if (key == "CONNECTION_USE_IOURING") paramEnum = ConfigParameter::CONNECTION_USE_IOURING;
                    else if (key == "THREADPOOL_SIZE") paramEnum = ConfigParameter::THREADPOOL_SIZE;
                    else if (key == "CPU_CORES") paramEnum = ConfigParameter::CPU_CORES;
                    else if (key == "REQUESTS_ACTIVE_MAX") paramEnum = ConfigParameter::REQUESTS_ACTIVE_MAX;
                    else if (key == "REQUESTS_QUEUED_MAX") paramEnum = ConfigParameter::REQUESTS_QUEUED_MAX;
                    else if (key == "AVAILABLE_NODENAMES") paramEnum = ConfigParameter::AVAILABLE_NODENAMES;
                    else if (key == "WHZ_CLI_PATH") paramEnum = ConfigParameter::WHZ_CLI_PATH;
                    else if (key == "DATABASE_PATH") paramEnum = ConfigParameter::DATABASE_PATH;
                    else if (key == "DATABASE_NAME") paramEnum = ConfigParameter::DATABASE_NAME;
                    else if (key == "DATABASE_USER") paramEnum = ConfigParameter::DATABASE_USER;
                    else if (key == "DATABASE_PASSWORD") paramEnum = ConfigParameter::DATABASE_PASSWORD;
                    else if (key == "DATABASE_PORT") paramEnum = ConfigParameter::DATABASE_PORT;
                    else if (key == "DATABASE_HOST") paramEnum = ConfigParameter::DATABASE_HOST;
                    else if (key == "DATABASE_ENGINE") paramEnum = ConfigParameter::DATABASE_ENGINE;
                    else if (key == "LUA_SCRIPT_PATH") paramEnum = ConfigParameter::LUA_SCRIPT_PATH;
                    else if (key == "LUA_START_SCRIPT_FILENAME") paramEnum = ConfigParameter::LUA_START_SCRIPT_FILENAME;

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
                        case ConfigParameter::SERVER_LOG_LEVEL_MIN:
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
                            break;
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
                        default:
                            break;
                    }
                }
                bRet = true;
                this->m_bConfigLoaded = true;
            }
        } else {
            // File does not exist
            std::cerr << "Error: File " << sfilepath << " does not exist." << std::endl;
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
            case ConfigParameter::SERVER_LOG_LEVEL_MIN:
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
                break;
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
            default:
                value = "Unknown";
                break;
        }
        return value;
    }
} // whz