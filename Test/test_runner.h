//
// Created by Aleksei Lukianchuk on 15-Nov-22.
//

// In the spirit of the test runner from the Yandex Cpp course on Coursera:
//

#ifndef EFTPROFILER_TEST_RUNNER_H
#define EFTPROFILER_TEST_RUNNER_H
#pragma once

#include <sstream>
#include <stdexcept>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "Profiler.h"

template <class T>
std::ostream& operator << (std::ostream& os, const std::vector<T>& s) {
    os << "{";
    bool first = true;
    for (const auto& x : s) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << "}";
}

template <class T>
std::ostream& operator << (std::ostream& os, const std::set<T>& s) {
    os << "{";
    bool first = true;
    for (const auto& x : s) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << "}";
}

template <class K, class V>
std::ostream& operator << (std::ostream& os, const std::map<K, V>& m) {
    os << "{";
    bool first = true;
    for (const auto& kv : m) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << kv.first << ": " << kv.second;
    }
    return os << "}";
}

template<class T, class U>
void AssertEqual(const T& t, const U& u, const std::string& hint = {}) {
    if (!(t == u)) {
        std::ostringstream os;
        os << "Assertion failed: " << t << " != " << u;
        if (!hint.empty()) {
            os << " hint: " << hint;
        }
        throw std::runtime_error(os.str());
    }
}

template<class T, class U>
void AssertNotEqual(const T& t, const U& u, const std::string& hint = {}) {
    if ((t == u)) {
        std::ostringstream os;
        os << "Assertion failed: " << t << " == " << u;
        if (!hint.empty()) {
            os << " hint: " << hint;
        }
        throw std::runtime_error(os.str());
    }
}

inline void Assert(bool b, const std::string& hint) {
    AssertEqual(b, true, hint);
}

class TestRunner;
struct TestRes;

struct TestRes {
    std::chrono::microseconds duration;
    std::string name;
    std::string res;

    inline std::string PrettyDuration() const noexcept {return {""}};
};

class TestRunner {
public:
    template <class TestFunc>
    TestRes RunTest(TestFunc func, const std::string& test_name) {
        TestRes res;
        try {
            {
                EFT_LOG_DURATION(test_name)
                func();
            }
            res.res = "OK";
            res.duration = eft::utils::Profiler::GetLastDuration();
            res.name = test_name;
            return res;
            //std::cerr << test_name << " OK" << std::endl;
        }
        catch (std::exception& e) {
            ++fail_count;
            res.res = std::string("Fail: ") + std::string(e.what());
            return res;
            //return std::string("Fail: ") + std::string(e.what());
            //std::cerr << test_name << " fail: " << e.what() << std::endl;
        }
        catch (...) {
            ++fail_count;
            res.res = "Unknown exception caught";
            return res;
            //std::cerr << "Unknown exception caught" << std::endl;
        }
    }

    inline size_t GetFailCount() const noexcept { return fail_count; }

    ~TestRunner() {
        if (fail_count > 0) {
            std::cerr << fail_count << " unit tests failed. Terminate" << std::endl;
            exit(1);
        }
    }

private:
    int fail_count = 0;
};



#define ASSERT_EQUAL(x, y) {                \
  std::ostringstream __os;                  \
  __os << #x << " != " << #y << ", "        \
    << __FILE__ << ":" << __LINE__;         \
  AssertEqual(x, y, __os.str());            \
}

#define ASSERT_NOT_EQUAL(x, y) {                \
    std::ostringstream __os;                    \
  __os << #x << " == " << #y << ", "            \
    << __FILE__ << ":" << __LINE__;             \
  AssertNotEqual(x, y, __os.str());             \
}

#define ASSERT(x) {                         \
  ostringstream __os;                       \
  __os << #x << " is false, "               \
    << __FILE__ << ":" << __LINE__;         \
  Assert(x, __os.str());                    \
}

#define ASSERT_NOT(x) {                    \
  ostringstream __os;                      \
  __os << #x << " is true, "               \
    << __FILE__ << ":" << __LINE__;        \
  Assert(!(x), __os.str());                \
}

#define ASSERT_THROW(expression, exceptionType) { \
    bool __eft__is_passes = false;              \
    std::ostringstream __os;                    \
    try {                                       \
        expression;                             \
        __os << #expression << " didn't throw " \
            << #exceptionType << ", at: "       \
            << __FILE__ << ':' << __LINE__;     \
        __eft__is_passes = false;               \
    } catch (const exceptionType& e) {          \
        __eft__is_passes = true;                \
    } catch (std::exception& e) {               \
        __os << #expression                     \
            << " throw std::exception: "        \
            << e.what() << " instead of: "      \
            << #exceptionType << ", at: "       \
            << __FILE__ << ':' << __LINE__;     \
    } catch(...) {                              \
        __os << #expression                     \
            << " throw an unknown exception: "  \
            << " instead of: "                  \
            << #exceptionType << ", at: "       \
            << __FILE__ << ':' << __LINE__;     \
    }                                           \
   if ( !__eft__is_passes ) {                   \
      throw std::runtime_error(__os.str());     \
   }                                            \
}


#define ASSERT_NO_THROW(expression) {               \
    try {                                           \
        expression;                                 \
    } catch(...) {                                  \
        std::ostringstream __os;                    \
        __os << #expression                         \
            << " which should not throw, "          \
            << " throws an exception at:"           \
            << __FILE__ << ':' << __LINE__;         \
       throw std::runtime_error(__os.str());        \
    }                                               \
}

#define RUN_TEST(tr, func) \
  tr.RunTest(func, #func)
#endif //EFTPROFILER_TEST_RUNNER_H
