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

    void Logger::Init()
    {
        vector<spdlog::sink_ptr> logSinks;
        logSinks.emplace_back(make_shared<spdlog::sinks::stdout_color_sink_mt>());
        logSinks.emplace_back(make_shared<spdlog::sinks::basic_file_sink_mt>("eft_log.log", true));

        logSinks[0]->set_pattern("%^[%T] %n: %v%$"); // in console
        logSinks[1]->set_pattern("[%T] [%l] %n: %v"); // out

        logSinks[0]->set_level(spdlog::level::debug);
        logSinks[1]->set_level(spdlog::level::trace);

        logger_ = std::make_shared<spdlog::logger>("eft_profiler", begin(logSinks), end(logSinks));
        spdlog::register_logger(logger_);
        //logger_->set_level(spdlog::level::trace);
        logger_->flush_on(spdlog::level::trace);
    }

} // namespace eft::stats