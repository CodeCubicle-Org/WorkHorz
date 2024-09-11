//
// Created by Pat Le Cat on 07/09/2024.
//
#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <filesystem>
#include "whz_LUA_api.hpp"

// Some LUA Libs that are built-in and can be added as needed, the rest needs to be included manually in LUA.:
//    base, package, coroutine, string, os, math, table, debug, bit32, io, ffi, jit
// See: https://www.lua.org/manual/5.4/manual.html#6
// or here: https://www.tutorialspoint.com/lua/lua_standard_libraries.htm

namespace whz {

    /**
     * @brief The LUA core class serving and managing the LUA scripting engine for internal use only. Low level stuff.
     *
     */
    class whz_LUA_core {
    public:
        whz_LUA_core() = default;
        ~whz_LUA_core() = default;

        bool init_LUA(const std::string& startup_script_path); /// Initialize the LUA scripting engine with a startup script defined in config
        bool init_LUA(const std::filesystem::path& startup_script_path);
        bool run_LUA_startup_script(void);

    protected:
        void init_LUA_user_api(void); /// Initialize the LUA user facing API

    private:
        sol::state _lua01;
        std::filesystem::path _startup_script_path;
        std::string _startup_script_content_str;
        whz::whz_LUA_api _whz_LUA_user_api;

    };

} // whz
