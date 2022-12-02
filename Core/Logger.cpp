//
// Created by Aleksei Lukianchuk on 16-Sep-22.
//

#include "Logger.h"
#include "CommandLineArgs.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <vector>

using namespace std;

namespace eft::stats {

void Logger::Init(string name, string path, string logger_name)
{

    if (logger_default_) {
        cout << "drop default logger" << endl;
        spdlog::drop("eft_profiler_default");
    }

    const std::string log_filename = std::move(path) + "eft_log_" + std::move(name) + ".log";

    vector<spdlog::sink_ptr> logSinks;
    logSinks.emplace_back(make_shared<spdlog::sinks::stdout_color_sink_mt>());
    logSinks.emplace_back(make_shared<spdlog::sinks::basic_file_sink_mt>(log_filename, true));

    logSinks[0]->set_pattern("%^[%T] %n: %v%$"); // in console
    logSinks[1]->set_pattern("[%T] [%l] %n: %v"); // out

    //logSinks[0]->set_level(spdlog::level::debug);
    //logSinks[1]->set_level(spdlog::level::trace);

    if (logger_name.empty())
        logger_ = std::make_shared<spdlog::logger>("eft_profiler", begin(logSinks), end(logSinks));
    else
        logger_ = std::make_shared<spdlog::logger>(std::move(logger_name), begin(logSinks), end(logSinks));

    spdlog::register_logger(logger_);
    logger_->set_level(spdlog::level::trace);
    logger_->flush_on(spdlog::level::trace);

    cout << "set is_init to true" << endl;
    is_init_ = true;
    cout << "is set_init == true ==> " << is_init_ << endl;
}

void Logger::Init() {
    // default logger
    const std::string log_filename = "defaul_eft_profiler_logger.log";
    vector<spdlog::sink_ptr> logSinks;
    logSinks.emplace_back(make_shared<spdlog::sinks::stdout_color_sink_mt>());
    logSinks.emplace_back(make_shared<spdlog::sinks::basic_file_sink_mt>(log_filename, true));

    logSinks[0]->set_pattern("%^[%T] %n: %v%$"); // in console
    logSinks[1]->set_pattern("[%T] [%l] %n: %v"); // out

    //logSinks[0]->set_level(spdlog::level::debug);
    //logSinks[1]->set_level(spdlog::level::trace);

    logger_default_ = std::make_shared<spdlog::logger>("eft_profiler_default", begin(logSinks), end(logSinks));
    spdlog::register_logger(logger_default_);
    logger_default_->set_level(spdlog::level::trace);
    logger_default_->flush_on(spdlog::level::trace);
}


void Logger::Init(const std::shared_ptr<CommandLineArgs>& commandLineArgs)
{
    cout << "init logger from a ptr to cmdline args" << endl;
    string log_path;
    string logger_name;
    commandLineArgs->SetValIfArgExists("log_path", log_path);
    commandLineArgs->SetValIfArgExists("logger_name", logger_name);

    if (commandLineArgs->HasKey("task")) {
        string task;
        commandLineArgs->SetValIfArgExists("task", task);
        if (task == "compute_ranking") {
            string poi;
            size_t worker_id;
            commandLineArgs->SetValIfArgExists("poi", poi);
            commandLineArgs->SetValIfArgExists("worker_id", worker_id);
            string name = fmt::format("ranking_{}_worker_{}", poi, worker_id);
            cout << "name of the new looger: [" << name << "]" << endl;
            Logger::Init(std::move(name), std::move(log_path), std::move(logger_name));
            cout << "new logger is created" << endl;
        }
        else if (task == "plot_ranking") {
            string poi;
            string name = fmt::format("plot_ranking_{}", std::move(poi));
            Logger::Init(std::move(name), std::move(log_path), std::move(logger_name));
        }
        else {
            string name = fmt::format("eft_{}", task);
            cout << "name of the new looger: [" << logger_name << "]" << endl;
            Logger::Init(std::move(name), std::move(log_path), std::move(logger_name));
        }
    } // task

    cout << "in init from cmd line: is_init ==> " << is_init_ << endl;

    if (commandLineArgs->HasKey("release")) {
        eft::stats::Logger::SetRelease();
    }

    if (commandLineArgs->HasKey("debug")) {
        eft::stats::Logger::SetFullPrinting();
    }

    if (commandLineArgs->HasKey("silent")) {
        eft::stats::Logger::SetSilent();
    }

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