//
// Created by Aleksei Lukianchuk on 18-Nov-22.
//

#include "Tester.h"

using namespace std;
using eft::stats::Logger;

void Tester::AddTest(Tester::Test test, std::string name, std::string groupname)
{
    if (nb_of_group_.find(groupname) == nb_of_group_.end()) {
        nb_of_group_[groupname] = nb_of_group_.size();
        group_nb_.push_back(groupname);
    }
    tests_[std::move(groupname)].push_back({std::move(name), std::move(test)});
}

void Tester::RunTests(const std::string& groupname_to_run_only)
{

    Logger::GetLogger()->set_level(spdlog::level::info);
    EFT_PROF_INFO("Run all tests");
    EFT_PROF_INFO("|{:-=45} | {:5} |", "=", "=");
    EFT_PROF_INFO("|{:-<45} | {:5} |", "Test Name", "Status");
    EFT_PROF_INFO("|{:-=45} | {:5} |", "=", "=");

    bool first = true;

    for (const auto& groupname : group_nb_) {
        const auto& tests = tests_.at(groupname);
        if (!first) {
            EFT_PROF_INFO("|{:-<45} | {:5} |", "-", "-");
        }
        else {
            first = false;
        }
        EFT_PROF_INFO("Running test group: {:10}", groupname);
        EFT_PROF_INFO("|{:-<45} | {:5} |", "-", "-");
        for (const auto& [name, function] : tests)
        {
            //cerr << fmt::format("| {:45} | ==> ", name);
            Logger::SetSilent(); // to ignore text from the tests
            string test_res = tr_.RunTest(function, "");
            Logger::SetLevel(spdlog::level::level_enum::info);
            EFT_PROF_INFO("|{:45} | {:5} |", name, std::move(test_res));
            Logger::SetLevel(spdlog::level::level_enum::info);
        } // tests in this group
        if (tr_.GetFailCount() == 0) {
            EFT_PROF_INFO("|{:-<45} | {:5} |", "-", "-");
            EFT_PROF_INFO("Tests Group: {:30} all test have been successfully passed", groupname);
        }
        else {
            EFT_PROF_CRITICAL("Tests Group: {:30}, {} tests have failed", groupname, tr_.GetFailCount());
            return;
        }
    } // groups
    size_t fail_count = tr_.GetFailCount();
    if (fail_count == 0) {
        EFT_PROF_INFO("All tests ran successfully");
    }
}

void Tester::InitSetTests() {
    //using namespace eft::inner::tests;


    EFT_PROF_DEBUG("Init set tests");
    EFT_RUN_TESTFILE(FileSystem);
    EFT_RUN_TESTFILE(StringUtils);
    EFT_RUN_TESTFILE(ColourUtils);
    EFT_RUN_TESTFILE(PlotterUtils);
    EFT_RUN_TESTFILE(WorkSpaceWrapper);
    //EFT_RUN_TESTFILE(CommandLineArguments); // problems with vec<string> ==> argv convertion
}