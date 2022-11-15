//
// Created by Aleksei Lukianchuk on 15-Nov-22.
//

#include "../Utils/StringUtils.h"
#include "test_runner.h"

void TestJoin()
{
    using eft::StringUtils;
    //StringUtils::Join();
}

void TestStrip()
{
    using SU = eft::StringUtils;
    using std::string;

    const string s1 {"no_strip"};
    auto res1 = SU::Strip(s1);
    ASSERT_EQUAL(s1, res1);

    const string s2 {"_one_strip"};
    auto res2 = SU::Strip(s1);
    ASSERT_EQUAL(s1, res1);
}

void TestStringUtils()
{
    TestRunner tr;
    RUN_TEST(tr, TestJoin);
    RUN_TEST(tr, TestStrip);
}
