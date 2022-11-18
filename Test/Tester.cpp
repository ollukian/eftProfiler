//
// Created by Aleksei Lukianchuk on 18-Nov-22.
//

#include "Tester.h"

void Tester::AddTest(const Tester::Test& test, std::string name, std::string groupname)
{
    EFT_PROF_DEBUG("Add test: {:15} to the group: {:15}");
    tests_[std::move(groupname)].emplace_back(std::move(name)) = test;
}

void Tester::RunTests(const std::string& groupname)
{
    EFT_PROF_INFO("Run all tests");
    for (const auto& [groupname_, tests] : tests_)
    {
        EFT_PROF_INFO("Running test group: {:10}", groupname_);
        for (const auto& test : tests)
        {
            RUN_TEST(tr_, test);
        }
    }
}
