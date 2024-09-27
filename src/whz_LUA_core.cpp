//
// Created by Pat Le Cat on 07/09/2024.
//

#include <fstream>
#include "whz_LUA_core.hpp"

namespace whz {
    bool whz_LUA_core::init_LUA(const std::string& startup_script_path) {
        this->_startup_script_path = startup_script_path;
        return this->init_LUA(this->_startup_script_path);
    }

    /**
     * @brief Initialize the LUA scripting engine with a startup script defined in config, do this once before running
     * the LUA startup script.
     *
     * @param startup_script_path As filesystem path
     * @return true All good
     * @return false Didn't work abort all
     */
    bool whz_LUA_core::init_LUA(const std::filesystem::path& startup_script_path) {
        bool bRet = false;
        // check if file exists in the filesystem startup_script_path
        if (!std::filesystem::exists(startup_script_path)) {
            std::cerr << "Error initializing LUA: Startup script not found: " << startup_script_path << std::endl;
            return bRet;
        }
        // check if the file has the extension .LUA
        if (startup_script_path.extension() != ".lua") {
            std::cerr << "Error initializing LUA: Startup script is not a LUA script: " << startup_script_path << std::endl;
            return bRet;
        }
        this->_startup_script_path = startup_script_path;

        // Read the file content of startup_script_path into a string
        try {
            std::ifstream ifs(startup_script_path);
            if (!ifs.is_open()) {
                std::cerr << "Error initializing LUA: Could not open startup script: " << startup_script_path
                          << std::endl;
                return bRet;
            }
            this->_startup_script_content_str = std::string((std::istreambuf_iterator<char>(ifs)),
                                                            std::istreambuf_iterator<char>());
            ifs.close();
        } catch (const std::exception& e) {
            std::cerr << "Error initializing LUA: Could not read startup script: " << e.what() << std::endl;
            return bRet;
        }

        bRet = true;

        return bRet;
    }

    /**
     * @brief Run the LUA startup script, this is the main entry point to the user's LUA based application. Run the
     * init_LUA() once before running this. This function will potentially run for a very long time we don't know what
     * the user's LUA script does.
     *
     * @return true All good
     * @return false Didn't work abort all
     */
    bool whz_LUA_core::run_LUA_startup_script() {
        bool bRet = false;

        if (this->_startup_script_path.empty()) {
            std::cerr << "Error running LUA startup script: No startup script defined." << std::endl;
            return bRet;
        }

        try {
            // Set the LUA GC mode
            this->_lua01.supports_gc_mode(sol::gc_mode::incremental);

            std::cout << "Test if LUA GC is turned on: " << this->_lua01.is_gc_on() << std::endl; // Check if GC is on

            //this->_lua01.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::string, sol::lib::os, sol::lib::math, sol::lib::table, sol::lib::debug, sol::lib::bit32, sol::lib::io, sol::lib::ffi, sol::lib::jit);
            this->_lua01.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::string, sol::lib::os, sol::lib::table, sol::lib::debug, sol::lib::bit32, sol::lib::io, sol::lib::ffi, sol::lib::jit);
            //this->_lua01.script_file(this->_startup_script_path);
            this->_lua01.script(this->_startup_script_content_str);
            bRet = true;
        } catch (const std::exception& e) {
            std::cerr << "Error running the LUA startup script (" << this->_startup_script_path << "): " << e.what() << std::endl;
        }
        // Run the GC right now to clean up the LUA memory
        this->_lua01.collect_garbage();
        return bRet;
    }

    /**
     * @brief Initialize the LUA user facing API by calling all the public function and data definitions. Facade!
     *
     */
    void whz_LUA_core::init_LUA_user_api() {
        // Call all the public functions and data definitions in the LUA API
    }

    /**
     * @brief Initialize the LUA scripting engine with a startup script defined in config, do this once before running
     * the LUA startup script.
     *
     * @return true All good
     * @return false Didn't work abort all
     */
    bool whz_LUA_core::init_LUA() {
        this->get_LUA_startup_script_path();
        return init_LUA(this->_startup_script_path);
    }

    bool whz_LUA_core::get_LUA_startup_script_path() {
        bool bRet = false;

        // Check if the LUA startup script is defined in the config class
        if (this->_whz_config.is_config_loaded()) {
            std::any lua_script_path = this->_whz_config.get_config_value(whz::Config::ConfigParameter::LUA_START_SCRIPT_FILENAME);
            if (lua_script_path.has_value()) {
                this->_startup_script_path = std::any_cast<std::string>(lua_script_path);
                bRet = true;
            }
        }
        return bRet;
    }

    /**
     * @brief Do 1 step in the LUA garbage collector with the preconfigured step size. Or 100KB if not defined or invalid.
     *
     * @return true The collectgarbage will return true if the triggered step was the last step of a garbage-collection cycle.
     * @return false False if the step wasn't the last one of the cycle.
     */
    bool whz_LUA_core::step_LUA_gc() {
        bool bRet = false;
        if (whz::Config::get_instance().get_config_value(whz::Config::ConfigParameter::LUA_GC_STEPSIZE).has_value()) {
            bRet = this->_lua01.step_gc(std::any_cast<int>(whz::Config::get_instance().get_config_value(whz::Config::ConfigParameter::LUA_GC_STEPSIZE))); // cast any to int
        }
        else {
            bRet = this->_lua01.step_gc(100); // Default to 100 KB
        }
        return bRet;
    }

} // whz