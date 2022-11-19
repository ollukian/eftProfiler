//
// Created by Aleksei Lukianchuk on 18-Nov-22.
//

#include "Tester.h"

void Tester::AddTest(Tester::Test test, std::string name, std::string groupname)
{
    EFT_PROF_DEBUG("Add test: {:35} to the group: {:15}", name, groupname);
    tests_[std::move(groupname)].push_back({name, test});
}

void Tester::RunTests(const std::string& groupname)
{

    eft::stats::Logger::GetLogger()->set_level(spdlog::level::info);

    EFT_PROF_INFO("Run all tests");
    for (const auto& [groupname_, tests] : tests_)
    {
        // TODO: redirect output to a stream, which can be "silent"
        EFT_PROF_INFO("Running test group: {:10}", groupname_);
        for (const auto& [name, function] : tests)
        {
            std::cerr << fmt::format("{:25} ==> ", name);
            eft::stats::Logger::SetSilent();
            tr_.RunTest(function, name);
            eft::stats::Logger::SetLevel(spdlog::level::level_enum::info);
        }
    }
    size_t fail_count = tr_.GetFailCount();
    if (fail_count == 0) {
        EFT_PROF_INFO("All tests ran successfully");
    }
}

void Tester::InitSetTests() {
    EFT_PROF_DEBUG("Init set tests");
    EFT_RUN_TESTFILE(FileSystem);
    EFT_RUN_TESTFILE(StringUtils);
    EFT_RUN_TESTFILE(ColourUtils);
}