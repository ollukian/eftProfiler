//
// Created by Aleksei Lukianchuk on 18-Nov-22.
//

#include "Tester.h"

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

    eft::stats::Logger::GetLogger()->set_level(spdlog::level::info);

    EFT_PROF_INFO("Run all tests");
    //for (const auto& [groupname_, idx] : nb_of_group_) {
    //    const auto& groupname = group_nb_[idx];
        //const auto& tests = tests_.at(groupname_);
    //for (const auto& [groupname_, tests] : tests_)
    //{
    for (const auto& groupname : group_nb_) {
        const auto& tests = tests_.at(groupname);
        EFT_PROF_INFO("Running test group: {:10}", groupname);
        for (const auto& [name, function] : tests)
        {
            std::cerr << fmt::format("| {:45} | ==> ", name);
            eft::stats::Logger::SetSilent();
            tr_.RunTest(function, "");
            eft::stats::Logger::SetLevel(spdlog::level::level_enum::info);
        } // tests in this group
        if (tr_.GetFailCount() == 0) {
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