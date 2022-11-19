//
// Created by Aleksei Lukianchuk on 16-Sep-22.
//

#ifndef EFTPROFILER_LOGGER_H
#define EFTPROFILER_LOGGER_H

// Same as in the eftModules project

#include <memory>

#ifdef _MSVC
#pragma warning(push, 0)
#endif
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#ifdef _MSVC
#pragma warning(pop)
#endif

namespace eft::stats {

    class Logger {
    public:
        static void Init(size_t worker_id = 0);

        static inline std::shared_ptr<spdlog::logger>& GetLogger() noexcept { return logger_; }
        static inline void SetSilent() noexcept { logger_->set_level(spdlog::level::off); }
        static inline void SetActive() noexcept { logger_->set_level(spdlog::level::trace); }
        static inline void SetLevel(spdlog::level::level_enum level) noexcept { logger_->set_level(level); }

    private:
        static std::shared_ptr<spdlog::logger> logger_;
    };
} // namespace eft

#define EFT_PROF_TRACE(...)      ::eft::stats::Logger::GetLogger()->trace(__VA_ARGS__)
#define EFT_PROF_INFO(...)       ::eft::stats::Logger::GetLogger()->info(__VA_ARGS__)
#define EFT_PROF_WARN(...)       ::eft::stats::Logger::GetLogger()->warn(__VA_ARGS__)
#define EFT_PROF_ERROR(...)      ::eft::stats::Logger::GetLogger()->error(__VA_ARGS__)
#define EFT_PROF_CRITICAL(...)   ::eft::stats::Logger::GetLogger()->critical(__VA_ARGS__)
#define EFT_PROF_DEBUG(...)      ::eft::stats::Logger::GetLogger()->debug(__VA_ARGS__)


#endif //EFTPROFILER_LOGGER_H
