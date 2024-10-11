#include "whz_quill_wrapper.hpp"
#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"
//#include "quill/sinks/ConsoleSink.h"
#include "quill/sinks/RotatingFileSink.h"
#include "quill/backend/PatternFormatter.h"
#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/std.h"
#include "whz_config.hpp"


namespace whz {
    whz_qlogger::whz_qlogger() {
        // TODO - add configurations for the logger

        quill::BackendOptions backend_options;
        quill::Backend::start(backend_options);


        std::shared_ptr<quill::Sink> rotating_file_sink_a =
                quill::Frontend::create_or_get_sink<quill::RotatingFileSink>("whz_logfile.log",
                                                     []() {
                                                         quill::RotatingFileSinkConfig rfh_cfg;
                                                         rfh_cfg.set_rotation_time_daily("00:00");
                                                         rfh_cfg.set_open_mode('w');
                                                         rfh_cfg.set_rotation_max_file_size(1024 * 1024 * 5);
                                                         rfh_cfg.set_remove_old_files(false);
                                                         rfh_cfg.set_timezone(quill::Timezone::LocalTime);
                                                         rfh_cfg.set_rotation_naming_scheme(quill::RotatingFileSinkConfig::RotationNamingScheme::DateAndTime);
                                                         rfh_cfg.set_filename_append_option( quill::FilenameAppendOption::StartDateTime);
                                                         return rfh_cfg;
                                                     }());

        // Create a logger with the rotating file sink and format the output properly
        this->qlogger = quill::Frontend::create_or_get_logger("logger_user", std::move(rotating_file_sink_a),
                                                              "%(time) [%(thread_id)] %(short_source_location:<28) "
                                                              "%(log_level:<9) %(logger:<12) %(message)",
                                                              "%H:%M:%S.%Qus");
    }

    whz_qlogger::~whz_qlogger() {
        if (!this->is_backend_closed) {
            this->qlogger->flush_log();
        }
    }

    void whz_qlogger::trace_L3(const std::string& fmtstr) {
        // Check if the value from Config is true then log, else do nothing
        if (std::any_cast<bool>(whz::Config::get_instance().get_config_value(Config::ConfigParameter::LOG_TRACE_L3))) {
            LOG_TRACE_L3(this->qlogger, "{}", fmtstr);
        }
    }

    void whz_qlogger::trace_L2(const std::string& fmtstr) {
        // Check if the value from Config is true then log, else do nothing
        if (std::any_cast<bool>(whz::Config::get_instance().get_config_value(Config::ConfigParameter::LOG_TRACE_L2))) {
            LOG_TRACE_L2(this->qlogger, "{}", fmtstr);
        }
    }

    void whz_qlogger::trace_L1(const std::string& fmtstr) {
        // Check if the value from Config is true then log, else do nothing
        if (std::any_cast<bool>(whz::Config::get_instance().get_config_value(Config::ConfigParameter::LOG_TRACE_L1))) {
            LOG_TRACE_L1(this->qlogger, "{}", fmtstr);
        }
    }

    void whz_qlogger::debug(const std::string& fmtstr) {
        // Check if the value from Config is true then log, else do nothing
        if (std::any_cast<bool>(whz::Config::get_instance().get_config_value(Config::ConfigParameter::LOG_DEBUG))) {
            LOG_DEBUG(this->qlogger, "{}", fmtstr);
        }
    }

    void whz_qlogger::info(const std::string& fmtstr) {
        // Check if the value from Config is true then log, else do nothing
        bool bLogInfo = std::any_cast<bool>(whz::Config::get_instance().get_config_value(Config::ConfigParameter::LOG_INFO));
        if (bLogInfo) {
            LOG_INFO(this->qlogger, "{}", fmtstr);
        }
    }

    void whz_qlogger::warning(const std::string& fmtstr) {
        // Check if the value from Config is true then log, else do nothing
        if (std::any_cast<bool>(whz::Config::get_instance().get_config_value(Config::ConfigParameter::LOG_WARNING))) {
            LOG_WARNING(this->qlogger, "{}", fmtstr);
        }
    }

    void whz_qlogger::error(const std::string& fmtstr) {
        // Check if the value from Config is true then log, else do nothing
        bool bLogVal = false;
        //std::string log_string = std::any_cast<std::string>(whz::Config::get_instance().get_config_value(Config::ConfigParameter::LOG_ERROR));
        //std::cout << whz::Config::get_instance().get_config_value(Config::ConfigParameter::LOG_ERROR).type() << std::endl;
        if (whz::Config::get_instance().get_config_value(Config::ConfigParameter::LOG_ERROR).type() == typeid(bool)) {
            std::cout << "Type of LOG_ERROR is bool" << std::endl;
            bLogVal = std::any_cast<bool>(whz::Config::get_instance().get_config_value(Config::ConfigParameter::LOG_ERROR));
        }
        //bLogVal = std::any_cast<bool>(whz::Config::get_instance().get_config_value(Config::ConfigParameter::LOG_ERROR));
        if (bLogVal) {
            LOG_ERROR(this->qlogger, "{}", fmtstr);
        }
    }

    void whz_qlogger::critical(const std::string& fmtstr) {
        // Check if the value from Config is true then log, else do nothing
        if (std::any_cast<bool>(whz::Config::get_instance().get_config_value(Config::ConfigParameter::LOG_CRITICAL))) {
            LOG_CRITICAL(this->qlogger, "{}", fmtstr);
        }
    }

    void whz_qlogger::backtrace(const std::string& fmtstr) {
        // Check if the value from Config is true then log, else do nothing
        if (std::any_cast<bool>(whz::Config::get_instance().get_config_value(Config::ConfigParameter::LOG_BACKTRACE))) {
            LOG_BACKTRACE(this->qlogger, "{}", fmtstr);
        }
    }

    /** Flushes the log and stops the backend process explicitly if this is the last object to close. Has to be called
     * explicitly to ensure that the log is flushed before the program exits and the backend is closed properly.
     *
     */
    void whz_qlogger::stopLogger() {
        this->qlogger->flush_log();
        quill::Frontend::remove_logger(this->qlogger);
        quill::Backend::stop();
        this->is_backend_closed = true;
    }
} // namespace whz
