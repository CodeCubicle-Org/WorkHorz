#pragma once

//#include "quill/Backend.h"
//#include "quill/Frontend.h"
//#include "quill/LogMacros.h"
#include "fmt/format.h"
#include "quill/logger.h"

//#include "quill/sinks/ConsoleSink.h"
//#include "quill/sinks/RotatingFileSink.h"

namespace whz {

    /**
     * @brief Logger class that offers a rotating file as the target for the log output. Use the trace_L3, trace_L2,
     * trace_L1, debug, info, warning, error, critical, and backtrace methods to log messages. Prepare the fmtstr with
     * fmt::format() before calling the log methods so you can easily add variables to the log message. Configure the
     * details of the rotating file in the configuration file.
     *
     */
    class whz_qlogger {
    public:
        whz_qlogger();      /// Initializes the logger, quill backend is a singleton
        ~whz_qlogger();     /// Flushes the log
        void stopLogger();

        /// Log APIs to use, output is defined in the configuration file
        void trace_L3(const std::string& fmtstr);  /// Enter a fmt-formatted string
        void trace_L2(const std::string& fmtstr);  /// Enter a fmt-formatted string
        void trace_L1(const std::string& fmtstr);  /// Enter a fmt-formatted string
        void debug(const std::string& fmtstr);      /// Enter a fmt-formatted string
        void info(const std::string& fmtstr);       /// Enter a fmt-formatted string
        void warning(const std::string& fmtstr);    /// Enter a fmt-formatted string
        void error(const std::string& fmtstr);      /// Enter a fmt-formatted string
        void critical(const std::string& fmtstr);   /// Enter a fmt-formatted string
        void backtrace(const std::string& fmtstr);  /// Enter a fmt-formatted string

    private:
        bool is_backend_closed = false;
        quill::Logger* qlogger;
        std::string qlogger_name;
    };
}

/*
LOG_TRACE_L3(logger, fmt, ...)
LOG_TRACE_L2(logger, fmt, ...)
LOG_TRACE_L1(logger, fmt, ...)
LOG_DEBUG(logger, fmt, ...) QUI
LOG_INFO(logger, fmt, ...) QUIL
LOG_WARNING(logger, fmt, ...) Q
LOG_ERROR(logger, fmt, ...) QUI
LOG_CRITICAL(logger, fmt, ...)
LOG_BACKTRACE(logger, fmt, ...)
 */
