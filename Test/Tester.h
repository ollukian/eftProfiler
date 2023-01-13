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

class Tester;
//std::vector<std::vector<char>> ConvertToArgcAgv(const std::vector<std::string>& s);
//std::vector<std::vector<char>> ConvertToArgcAgv(std::istringstream& s);
//void GetArgcArgvFromVecCharStars(std::istringstream& arguments, int& argc, char**& argv);

//static inline std::vector<std::vector<char>> vstrings;
//static inline std::vector<const char*>       cstrings;

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

    static inline void SetGroupName(const std::string& name) { group_name_ = name; }

    Tester(const Tester&) = delete;

    static inline Tester* Create() { return new Tester(); }

    ~Tester() = default;

private:
    static inline TestRunner tr_;
    static inline std::unique_ptr<Tester> tester_ {};
    static inline std::string group_name_ {};
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

#define STRINGIFY(x) #x

#ifndef EFT_RUNTEST
#define EFT_RUNTEST(name, group) Tester::Get()->AddTest(Test##group##name, \
    STRINGIFY(Test##group##name),                                          \
    STRINGIFY(group));
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

#ifndef EFT_START_TEST_GROUP
#define EFT_START_TEST_GROUP(namethis) \
    void Test##namethis()              \
    {                                  \
       const char* __eft__this_group_name = #namethis;
#endif
// Tester::SetGroupName(#namethis);     \

#ifndef EFT_END_IMPLEMENT_TESTFILE
#define EFT_END_IMPLEMENT_TESTFILE(namethis)

#endif

#ifndef EFT_IMPLEMENT_TEST
#define EFT_IMPLEMENT_TEST(group, name) \
    void Test##group##name()
#endif

//#define EFT_GET_STR_VAL(name) name

//#ifndef EFT_IMPLEMENT_TEST_IN_GROUP
//#define EFT_IMPLEMENT_TEST_IN_GROUP(test) \
//    void Test
//#endif


#define EFT_RUN_TESTS()             \
    do  {                           \
    Tester::Get()->InitSetTests();  \
    Tester::Get()->RunTests();      \
} while (false)

//#ifndef EFT_BEGIN_TESTFILE
//#define EFT_BEGIN_TESTFILE(groupname) \
//        EFT_                              \
//    //static const char* __eft_this_groupname = #groupname;
//#endif

EFT_ADD_TESTFILE(FileSystem);
EFT_ADD_TESTFILE(StringUtils);
EFT_ADD_TESTFILE(ColourUtils);
EFT_ADD_TESTFILE(CommandLineArguments);
EFT_ADD_TESTFILE(PlotterUtils);
EFT_ADD_TESTFILE(WorkSpaceWrapper);
EFT_ADD_TESTFILE(Scene);
EFT_ADD_TESTFILE(FitManager);

#endif //EFTPROFILER_TESTER_H
