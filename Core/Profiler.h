//
// Created by Aleksei Lukianchuk on 24-Nov-22.
//

#ifndef EFTPROFILER_PROFILER_H
#define EFTPROFILER_PROFILER_H

#include <string>
#include <unordered_map>
#include <chrono>
#include <ratio>
#include <iostream>
#include <sstream>

#include "Logger.h"

namespace eft::utils {


class Profiler {
public:
    using TimePoint = std::chrono::steady_clock::time_point;
    using Duration  = std::chrono::microseconds;
    using DurationPerFunction = std::unordered_map<std::string, Duration>;
    using FunctionName = std::string;

    static inline std::chrono::microseconds GetLastDuration() { return last_duration_; }
    static inline const DurationPerFunction& GetDurations()   { return durations_;}

    explicit Profiler(std::string msg = "")
            : message(std::move(msg))
            //, prefRes_()
            , start(std::chrono::steady_clock::now())
    {
    }

    ~Profiler() {
        auto finish = std::chrono::steady_clock::now();
        elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
        last_duration_ = elapsedTime;
        durations_[message] += elapsedTime;
        //auto dur = finish - start;
        //std::stringstream message_to_display;
        //message_to_display << "[" << message << "] done in " << elapsedTime.count()
        //                   << "mcs";
        //EFT_DEBUG(message_to_display.str());
    }

private:
    const std::string  message {};
    std::ostream& os_ {std::cout};
    std::chrono::steady_clock::time_point start {};
    std::chrono::microseconds             elapsedTime {};

    static inline std::unordered_map<FunctionName, Duration> durations_ {};
    static inline Duration last_duration_ {};
};

// Function name wrapper
#define EFT_PROFILE
#ifdef EFT_PROFILE
#   if defined(__GNUC__)
#       define EFT_FN_SIG __FUNCTION__  // _func_
#   elif (defined(__FUNCSIG__) || (_MSC_VER))
#       define EFT_FN_SIG  __FUNCTION__ //__FUNCSIG__
#   elif defined(__cplusplus) && (__cplusplus >= 201103)
    #      define EFT_FN_SIG __func__
#   else
#       define EFT_FN_SIG "EFT_FN_SIG is not known. Visit Core/Profiler.h"
#   endif
#endif

// UNIQ_ID
#ifndef EFT_UNIQ_ID_IMPL
#   define EFT_UNIQ_ID_IMPL(lineno) _eft_a_local_var_##lineno
#   define EFT_UNIQ_ID(lineno) EFT_UNIQ_ID_IMPL(lineno)
#endif


// Log duration
#define EFT_LOG_DURATION(message) ::eft::utils::Profiler EFT_UNIQ_ID(__LINE__){message};

#define EFT_PROFILE_FN() EFT_LOG_DURATION(EFT_FN_SIG);

} // utils

#endif //EFTPROFILER_PROFILER_H
