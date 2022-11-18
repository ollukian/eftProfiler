//
// Created by Aleksei Lukianchuk on 17-Nov-22.
//

#ifndef EFTPROFILER_TESTER_H
#define EFTPROFILER_TESTER_H

void TestStringUtils();
void TestFileSystemUtils();

#include "test_runner.h"
#include "../Core/Logger.h"

#include <string>
#include <vector>
#include <functional>


//class Tester {
//
//    using Test = std::function<void()>;
//    using Group = std::vector<Test>;
//
//public:
//    static void AddTest(const Test& test, std::string name = {""}, std::string groupname = {"common"});
//    static void RunTests(const std::string& groupname = {""});
//    static void Report() const;
//private:
//    static inline TestRunner tr_;
//    static inline std::map<std::string, Group> tests_;
//};
//
//#ifndef EFT_ADD_TEST
//#define EFT_ADD_TEST(func, group) \
//  Tester::AddTest(func, #func, group);
//
//#endif


#endif //EFTPROFILER_TESTER_H
