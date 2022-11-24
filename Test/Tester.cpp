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
    fmt::print(cout, "=={:=<45}==={:=<10}==={:=<10}==\n", "", "", "");
    fmt::print(cout, "| {:^45} | {:^10} | {:10} |\n", "Test Name", "Status", "Duration");
    fmt::print(cout, "=={:=<45}==={:=<10}==={:=<10}==\n", "", "", "");
    //EFT_PROF_INFO("=={:=<45}==={:=<10}==", "", "");
    //EFT_PROF_INFO("| {:^45} | {:^10} |", "Test Name", "Status");
    //EFT_PROF_INFO("=={:=<45}==={:=<10}==", "", "");

    bool first = true;

    for (const auto& groupname : group_nb_) {
        const auto& tests = tests_.at(groupname);
        //if (!first) {
        //    EFT_PROF_INFO("+{:-<45} | {:-<10} +", "", "");
       // }
        //else {
        //    first = false;
        //}
        bool group_ok = true;
        fmt::print(cout, "| {:^58} |\n", groupname);
        fmt::print(cout, "+{:-<45}--+-{:-<10}--+-{:-<10}-+\n", "", "", "");
        //EFT_PROF_INFO("| {:^58} |", groupname);
        //EFT_PROF_INFO("+{:-<45}--+-{:-<10}-+", "", "");
        for (const auto& [name, function] : tests)
        {
            //cerr << fmt::format("| {:45} | ==> ", name);
            Logger::SetSilent(); // to ignore text from the tests
            auto test_res = tr_.RunTest(function, "");
            Logger::SetLevel(spdlog::level::level_enum::info);
            if (test_res.res == "OK") {
                //EFT_PROF_INFO("| {:45} | {:10} |", name, test_res);
                //fmt::print(cout, "| {:45} | {:10} |\n", name, "Wine", "OK");
                fmt::print(fmt::fg(fmt::color::green), "| {:45} | {:10} | {:10} |\n", name, "OK", test_res.duration.count());
            }
            else {
                group_ok = false;
                ////fmt::print(fmt::fg(fmt::color::red), "| {:45} | {:10} | {:10} |\n", name, test_res.res, test_res.duration.count());
                fmt::print(fmt::fg(fmt::color::red), "| {:45} | fail | {:10} |\n", name,  test_res.duration.count());
                fmt::print(fmt::fg(fmt::color::red), "| {}", test_res.res);
                //EFT_PROF_ERROR("| {:45} | {:10} |", name, test_res);
                //fmt::print(cerr, "| {:45} | {:10} |\n", name, std::move(test_res));
            }
            //EFT_PROF_INFO("| {:45} | {:10} |", name, std::move(test_res));
            Logger::SetLevel(spdlog::level::level_enum::info);
        } // tests in this group
        if (tr_.GetFailCount() == 0) {
            fmt::print(cout, "+{:-<45}--+-{:-<10}}--+-{:-<10}-+\n", "-", "-", "");
            //EFT_PROF_INFO("+{:-<45}--+-{:-<10}-+", "-", "-");
            //EFT_PROF_INFO("Tests Group: {:30} all test have been successfully passed", groupname);
        }
        else {
            fmt::print(fmt::fg(fmt::color::red), "| Tests Group: {:30}, {} tests have failed\n", groupname, tr_.GetFailCount());
            //fmt::print(cerr, "Tests Group: {:30}, {} tests have failed\n", groupname, tr_.GetFailCount());
            //EFT_PROF_CRITICAL("Tests Group: {:30}, {} tests have failed", groupname, tr_.GetFailCount());
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