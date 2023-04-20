//
// Created by Aleksei Lukianchuk on 20-Apr-23.
//

#ifndef EFTPROFILER_APPLICATION_H
#define EFTPROFILER_APPLICATION_H

#include <iostream>

#include "../Core/CommandLineArgs.h"
#include "../Vendors/toml/toml.hpp"

namespace eft {

    class Application;
    enum class DebugState : uint8_t;
    enum class ApplicationState : uint8_t;


    enum class DebugState : uint8_t {
        SILENT      = 0,
        RELEASE     = 1,
        DEBUG       = 2,
        FULL_DEBUG  = 3,
    };

    enum class ApplicationState : uint8_t {
        WAITING_REQUEST = 0,
        TEST            = 1,
        HELP            = 2,
        FINISHED        = 3,
        INITIALISING    = 4,
        APP_ERROR       = 5,
    };

    class Application {
    public:
        static void ProcessRequest(const std::string& request);
        static void Init(int argc, char** argv);
        static void Run();
        static void Finalize();

        static bool isReady() noexcept    { return applicationState_ == ApplicationState::WAITING_REQUEST; }
        static bool isFinished() noexcept { return applicationState_ == ApplicationState::FINISHED; }
        static bool isErrored() noexcept  { return applicationState_ == ApplicationState::APP_ERROR; }
        static void setSuccess() noexcept { applicationState_ = ApplicationState::FINISHED; }
        static void setError() noexcept   { applicationState_ = ApplicationState::APP_ERROR; }

        static inline std::shared_ptr<CommandLineArgs> GetCommandLineArgs() noexcept { return commandLineArgs_; }
        static inline const toml::table& GetConfig() noexcept { return config_settings_; }


        Application() = delete;
        Application(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator= (const Application&) = delete;
        Application& operator= (Application&&) = delete;
    private:
        void ReadConfig(const std::string& path = "config.toml");

        static void PrintHelp(std::ostream& os);
        static void InitDebugLevel();
        //void InitLogger();


        static void ProcessTest();

        static void ProcessGet();
        static void ProcessRankingComputation();
        static void ProcessRankingPlotting();
        static void ProcessNllScanComputation();
        static void ProcessNllScanPlotting();
        static void ProcessFreeFitComputation();
        static void ProcessCompareRatings();
        static void ProcessComputeHesseNps();
        static void ProcessGetMissingNps();
    private:
        static inline std::shared_ptr<CommandLineArgs> commandLineArgs_;
        static inline DebugState debugState_ {DebugState::RELEASE};
        static inline ApplicationState applicationState_ {ApplicationState::INITIALISING};
        static inline toml::table      config_settings_;
    };



} // eft

#endif //EFTPROFILER_APPLICATION_H
