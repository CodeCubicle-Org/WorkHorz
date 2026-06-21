//
// Created by Pat Le Cat on 07/09/2024.
//

#include "whz_LUA_api.hpp"
#include <unordered_map>

namespace whz {

    whz_LUA_api::whz_LUA_api() {
        lua.open_libraries(sol::lib::base, sol::lib::package);
        register_config_api();
    }

    sol::state& whz_LUA_api::get_lua_state() {
        return lua;
    }

    void whz_LUA_api::register_config_api() {
        auto config_api = lua.create_table("config");
        
        config_api["get"] = [this](const std::string& param_name) -> sol::object {
            auto& cfg = Config::get_instance();
            
            auto param = string_to_config_param(param_name);
            if (param == Config::ConfigParameter::UNKNOWN) {
                return sol::make_object(lua, sol::nil);
            }

            auto value = cfg.get_config_value(param);
            return cpp_value_to_lua(lua, value);
        };
    }

    Config::ConfigParameter whz_LUA_api::string_to_config_param(const std::string& param_name) {
        static const std::unordered_map<std::string, Config::ConfigParameter> param_map = {
            {"http_port", Config::ConfigParameter::SERVER_HTTP_PORT},
            {"https_port", Config::ConfigParameter::SERVER_HTTPS_PORT},
            {"root_path", Config::ConfigParameter::SERVER_ROOTPATH},
            {"log_path", Config::ConfigParameter::SERVER_LOGPATH},
            {"connection_timeout", Config::ConfigParameter::CONNECTION_TIMEOUT_MS},
            {"domain_name", Config::ConfigParameter::SERVER_DOMAINNAME},
            {"ssl_cert_path", Config::ConfigParameter::SERVER_SSL_CERTPATH},
            {"max_io_contexts", Config::ConfigParameter::CONNECTION_MAX_IO_CONTEXTS},
            {"use_io_uring", Config::ConfigParameter::CONNECTION_USE_IOURING},
            {"threadpool_size", Config::ConfigParameter::THREADPOOL_SIZE},
            {"cpu_cores", Config::ConfigParameter::CPU_CORES},
            {"max_active_requests", Config::ConfigParameter::REQUESTS_ACTIVE_MAX},
            {"max_queued_requests", Config::ConfigParameter::REQUESTS_QUEUED_MAX},
            {"available_nodes", Config::ConfigParameter::AVAILABLE_NODENAMES},
            {"cli_path", Config::ConfigParameter::WHZ_CLI_PATH},
            {"db_path", Config::ConfigParameter::DATABASE_PATH},
            {"db_name", Config::ConfigParameter::DATABASE_NAME},
            {"db_user", Config::ConfigParameter::DATABASE_USER},
            {"db_password", Config::ConfigParameter::DATABASE_PASSWORD},
            {"db_port", Config::ConfigParameter::DATABASE_PORT},
            {"db_host", Config::ConfigParameter::DATABASE_HOST},
            {"db_engine", Config::ConfigParameter::DATABASE_ENGINE},
            {"lua_script_path", Config::ConfigParameter::LUA_SCRIPT_PATH},
            {"lua_start_script", Config::ConfigParameter::LUA_START_SCRIPT_FILENAME},
            {"lua_gc_stepsize", Config::ConfigParameter::LUA_GC_STEPSIZE},
            {"log_trace_l3", Config::ConfigParameter::LOG_TRACE_L3},
            {"log_trace_l2", Config::ConfigParameter::LOG_TRACE_L2},
            {"log_trace_l1", Config::ConfigParameter::LOG_TRACE_L1},
            {"log_debug", Config::ConfigParameter::LOG_DEBUG},
            {"log_info", Config::ConfigParameter::LOG_INFO},
            {"log_warning", Config::ConfigParameter::LOG_WARNING},
            {"log_error", Config::ConfigParameter::LOG_ERROR},
            {"log_critical", Config::ConfigParameter::LOG_CRITICAL},
            {"log_backtrace", Config::ConfigParameter::LOG_BACKTRACE},
            {"log_filename", Config::ConfigParameter::LOG_FILENAME},
            {"log_rotation_days", Config::ConfigParameter::LOG_ROTATION_DAYS},
            {"log_rotation_mb", Config::ConfigParameter::LOG_ROTATION_MB},
            {"log_path", Config::ConfigParameter::LOG_PATH}
        };

        auto it = param_map.find(param_name);
        return it != param_map.end() ? it->second : Config::ConfigParameter::UNKNOWN;
    }

    sol::object whz_LUA_api::cpp_value_to_lua(sol::state& lua, const std::any& value) {
        try {
            if (value.type() == typeid(int)) {
                return sol::make_object(lua, std::any_cast<int>(value));
            }
            if (value.type() == typeid(std::string)) {
                return sol::make_object(lua, std::any_cast<std::string>(value));
            }
            if (value.type() == typeid(bool)) {
                return sol::make_object(lua, std::any_cast<bool>(value));
            }
        } catch (const std::bad_any_cast&) {
            return sol::make_object(lua, sol::nil);
        }
        return sol::make_object(lua, sol::nil);
    }

} // whz