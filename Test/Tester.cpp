//
// Created by Aleksei Lukianchuk on 18-Nov-22.
//

#include "Tester.h"
#include "spdlog/fmt/bundled/color.h"
#include "spdlog/fmt/bundled/ostream.h"

using namespace std;
using eft::stats::Logger;

void Tester::AddTest(Tester::Test test, std::string name, std::string groupname)
{
    EFT_PROFILE_FN();
    if (nb_of_group_.find(groupname) == nb_of_group_.end()) {
        nb_of_group_[groupname] = nb_of_group_.size();
        group_nb_.push_back(groupname);
    }
    tests_[std::move(groupname)].push_back({std::move(name), std::move(test)});
}

void Tester::RunTests(const std::string& groupname_to_run_only)
{
    EFT_PROFILE_FN();
    Logger::GetLogger()->set_level(spdlog::level::info);
    EFT_PROF_INFO("Run all tests");
    fmt::print(cout, "=={:=<45}==={:=<10}==={:=<10}==\n", "", "", "");
    fmt::print(cout, "| {:^45} | {:^10} | {:10} |\n", "Test Name", "Status", "Duration");
    fmt::print(cout, "=={:=<45}==={:=<10}==={:=<10}==\n", "", "", "");
    for (const auto& groupname : group_nb_) {
        const auto& tests = tests_.at(groupname);
        fmt::print(cout, "| {:^71} |\n", groupname);
        fmt::print(cout, "+{:-<45}--+-{:-<10}-+-{:-<10}-+\n", "", "", "");
        for (const auto& [name, function] : tests)
        {
            Logger::SetSilent(); // to ignore text from the tests
            auto test_res = tr_.RunTest(function, "");
            Logger::SetLevel(spdlog::level::level_enum::info);
            if (test_res.res == "OK") {
                fmt::print(fmt::fg(fmt::color::green), "| {:45} | {:10} | {:10} |\n", name, "OK", test_res.PrettyDuration());
            }
            else {
                fmt::print(fmt::fg(fmt::color::red), "| {:45} | {:10} | {:10} |\n", name, "Fail", test_res.PrettyDuration());
                fmt::print(fmt::fg(fmt::color::red), "| {} \n", test_res.res);
            }
            Logger::SetLevel(spdlog::level::level_enum::info);
        } // tests in this group
        if (tr_.GetFailCount() == 0) {
            fmt::print(cout, "+{:-<45}--+-{:-<10}-+-{:-<10}-+\n", "-", "-", "");
        }
        else {
            fmt::print(fmt::fg(fmt::color::red), "| Tests Group: {:30}, {} tests have failed\n", groupname, tr_.GetFailCount());
            return;
        }
    } // groups
    size_t fail_count = tr_.GetFailCount();
    if (fail_count == 0) {
        EFT_PROF_INFO("All tests ran successfully");
    }
    EFT_PROF_INFO("Statistics for function calls:");
    const auto& durations = eft::utils::Profiler::GetDurations();
    const auto& avg_durations = eft::utils::Profiler::GetAvgDurations());
    for (const auto& [name, duration] : durations) {
        fmt::print(cout, "| {:-^30} | {:-<10} | {:-<15} |\n", "Function", "Duration", "Avg duration");
        fmt::print(fmt::fg(fmt::color::light_green), "| {:^30} | {:10} | {:10} | \n",
                   name,
                   duration.count(),
                   avg_durations.at(name).count()
                   );
    }
}

void Tester::InitSetTests() {
    EFT_PROFILE_FN();
    //using namespace eft::inner::tests;
    EFT_PROF_DEBUG("Init set tests");
    EFT_RUN_TESTFILE(FileSystem);
    EFT_RUN_TESTFILE(StringUtils);
    EFT_RUN_TESTFILE(ColourUtils);
    EFT_RUN_TESTFILE(PlotterUtils);
    EFT_RUN_TESTFILE(CommandLineArguments); // problems with vec<string> ==> argv convertion
    EFT_RUN_TESTFILE(WorkSpaceWrapper);
}