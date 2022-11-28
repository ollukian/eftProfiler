//
// Created by Aleksei Lukianchuk on 16-Sep-22.
//

#ifndef EFTPROFILER_LOGGER_H
#define EFTPROFILER_LOGGER_H

// Same as in the eftModules project

#ifdef _MSVC
#pragma warning(push, 0)
#endif
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#ifdef _MSVC
#pragma warning(pop)
#endif

#include <memory>

#include <set>
#include <map>
#include <vector>
#include <string>
#include <deque>
#include <list>


namespace eft::stats {

    class Logger {
    public:
        static void Init(size_t worker_id = 0);

        static inline std::shared_ptr<spdlog::logger>& GetLogger() noexcept { return logger_; }
        static inline void SetSilent()       noexcept { logger_->set_level(spdlog::level::off); }
        static inline void SetFullPrinting() noexcept { logger_->set_level(spdlog::level::trace); }
        static inline void SetRelease()      noexcept { logger_->set_level(spdlog::level::info); }
        static inline void SetDebug()        noexcept { logger_->set_level(spdlog::level::debug); }
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


template<typename OStream, class T>
OStream& operator << (OStream& os, const std::vector<T>& vec) {
    os << '{';
    bool first = true;
    for (const auto& x : vec) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << '}';
}

template<typename OStream, class T>
OStream& operator << (OStream& os, const std::deque<T>& vec) {
    os << '{';
    bool first = true;
    for (const auto& x : vec) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << '}';
}

template<typename OStream, class T>
OStream& operator << (OStream& os, const std::list<T>& vec) {
    os << '{';
    bool first = true;
    for (const auto& x : vec) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << '}';
}

template<typename OStream, class T>
OStream& operator << (OStream& os, const std::set<T>& s) {
    os << '{';
    bool first = true;
    for (const auto& x : s) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << '}';
}

template<typename OStream, class K, class V>
OStream& operator << (OStream& os, const std::map<K, V>& m) {
    os << '{';
    bool first = true;
    for (const auto& kv : m) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << kv.first << ": " << kv.second;
    }
    return os << '}';
}


#endif //EFTPROFILER_LOGGER_H
