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
#include <sstream>
#include <random>


class Tester {

    using Test      = std::function<void()>;
    using TestName  = std::string;
    using GroupName = std::string;

    using Group = std::vector<std::pair<TestName, Test>>;

public:
    void InitSetTests();
    void AddTest(Test test, std::string name = {""}, std::string groupname = {"common"});
    void RunTests(const std::string& groupname = {""});
    //void Report();

    static inline std::unique_ptr<Tester>& Get(); // may throw via new()

    Tester(const Tester&) = delete;

    static inline Tester* Create() { return new Tester(); }

    ~Tester() = default;

private:
    static inline TestRunner tr_;
    static inline std::unique_ptr<Tester> tester_ {};
    //std::vector<std::pair<GroupName, Group>> tests_;
    std::map<GroupName, Group> tests_;
    std::vector<GroupName> group_nb_;
    std::map<GroupName, size_t> nb_of_group_;
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

#ifndef EFT_RUN_TESTFILE
#define EFT_RUN_TESTFILE(namethis) \
    Test##namethis();
#endif


#ifndef EFT_IMPLEMENT_TESTFILE
#define EFT_IMPLEMENT_TESTFILE(namethis) \
    void Test##namethis()
#endif

#ifndef EFT_END_IMPLEMENT_TESTFILE
#define EFT_END_IMPLEMENT_TESTFILE(namethis)

#endif

#define EFT_RUN_TESTS()             \
    do  {                           \
    Tester::Get()->InitSetTests();  \
    Tester::Get()->RunTests();      \
} while (false)



EFT_ADD_TESTFILE(FileSystem);
EFT_ADD_TESTFILE(StringUtils);
EFT_ADD_TESTFILE(ColourUtils);
EFT_ADD_TESTFILE(CommandLineArguments);
EFT_ADD_TESTFILE(PlotterUtils);
EFT_ADD_TESTFILE(WorkSpaceWrapper);

#endif //EFTPROFILER_TESTER_H
