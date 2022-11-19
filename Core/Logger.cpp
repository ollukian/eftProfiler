//
// Created by Aleksei Lukianchuk on 16-Sep-22.
//

#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <vector>

using namespace std;

namespace eft::stats {

    shared_ptr<spdlog::logger> Logger::logger_;

    // TODO: add a logger for traces and durations - only for debugging;

    void Logger::Init(size_t worker_id)
    {

        const std::string log_filename = "eft_log_worker_" + to_string(worker_id) + ".log";

        vector<spdlog::sink_ptr> logSinks;
        logSinks.emplace_back(make_shared<spdlog::sinks::stdout_color_sink_mt>());
        logSinks.emplace_back(make_shared<spdlog::sinks::basic_file_sink_mt>(log_filename, true));

        logSinks[0]->set_pattern("%^[%T] %n: %v%$"); // in console
        logSinks[1]->set_pattern("[%T] [%l] %n: %v"); // out

        //logSinks[0]->set_level(spdlog::level::debug);
        //logSinks[1]->set_level(spdlog::level::trace);

        logger_ = std::make_shared<spdlog::logger>("eft_profiler", begin(logSinks), end(logSinks));
        spdlog::register_logger(logger_);
        logger_->set_level(spdlog::level::trace);
        logger_->flush_on(spdlog::level::trace);
    }


    /*
     *  You can disable logging with set_level():

        auto my_logger = spdlog::basic_logger_mt("basic_logger", "logs/basic.txt");

        #if defined(PRODUCTION)
            my_logger->set_level(spdlog::level::off);
        #else
            my_logger->set_level(spdlog::level::trace);
        #endif

spdlog::register_logger(my_logger);
     *
     *
     *
     *
     *
     *  https://github.com/gabime/spdlog/blob/v1.x/include/spdlog/spdlog.h :
     *
     * //
        // enable/disable log calls at compile time according to global level.
        //
        // define SPDLOG_ACTIVE_LEVEL to one of those (before including spdlog.h):
        // SPDLOG_LEVEL_TRACE,
        // SPDLOG_LEVEL_DEBUG,
        // SPDLOG_LEVEL_INFO,
        // SPDLOG_LEVEL_WARN,
        // SPDLOG_LEVEL_ERROR,
        // SPDLOG_LEVEL_CRITICAL,
        // SPDLOG_LEVEL_OFF
//
     *
     * */

} // namespace eft::stats