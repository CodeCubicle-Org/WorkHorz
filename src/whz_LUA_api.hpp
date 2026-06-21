//
// Created by Pat Le Cat on 07/09/2024.
//
#pragma once

#include "whz_config.hpp"
#include "whz_quill_wrapper.hpp"
#include <sol/sol.hpp>

namespace whz {

    /**
     * @brief API class that offers a user API to the LUA scripting engine it's a simplified interface allowing users to
     * interact with their own LUA scripts. This is a Facade pattern class.
     *
     */
    class whz_LUA_api {
    public:
        whz_LUA_api();
        ~whz_LUA_api() = default;

        // Get the Lua state
        [[nodiscard]] sol::state& get_lua_state();

    private:
        sol::state lua;

        void register_config_api();
        static Config::ConfigParameter string_to_config_param(const std::string& param_name);
        static sol::object cpp_value_to_lua(sol::state& lua, const std::any& value);
    };

} // whz
