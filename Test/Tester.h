//
// Created by Aleksei Lukianchuk on 17-Nov-22.
//

#ifndef EFTPROFILER_TESTER_H
#define EFTPROFILER_TESTER_H

// void TestStringUtils();
// void TestFileSystemUtils();

#include "test_runner.h"
#include "../Core/Logger.h"

#include <string>
#include <vector>
#include <functional>
#include <memory>


class Tester {

    using Test = std::function<void()>;
    using Group = std::vector<Test>;

public:
    void AddTest(const Test& test, std::string name = {""}, std::string groupname = {"common"});
    void RunTests(const std::string& groupname = {""});
    //void Report();

    static inline std::unique_ptr<Tester>& Get(); // may throw via new()

    Tester(const Tester&) = delete;

    static inline Tester* Create() { return new Tester(); }

    ~Tester() = default;

private:
    static inline TestRunner tr_;
    static inline std::unique_ptr<Tester> tester_ {};
    std::map<std::string, Group> tests_;
private:
    Tester() = default;
};

inline std::unique_ptr<Tester>& Tester::Get()
{
   if (tester_)
       return tester_;
   tester_.reset( new Tester() );
   return tester_;
}

#ifndef EFT_ADD_TEST
#define EFT_ADD_TEST(func, group) Tester::Get()->AddTest(func, #func, group);
#endif

#ifndef EFT_ADD_TESTFILE
#define EFT_ADD_TESTFILE(namethis) \
    void Test##namethis();
#endif

EFT_ADD_TESTFILE(FileSystem);

#endif //EFTPROFILER_TESTER_H
