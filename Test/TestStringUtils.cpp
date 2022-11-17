//
// Created by Aleksei Lukianchuk on 15-Nov-22.
//

#include "../Utils/StringUtils.h"
#include "test_runner.h"
#include "Tester.h"

void TestJoin()
{
    using eft::StringUtils;
    //StringUtils::Join();
}

void TestStrip()
{
    using SU = eft::StringUtils;
    using std::string;

    {
        string s1{"no_strip"};
        auto res1 = SU::Strip(s1);
        ASSERT_EQUAL(s1, res1);
    }
    {
        string s2{" one_strip_front"};
        const string s2_res{"one_strip_front"};
        auto s2_stripped = SU::Strip(s2);
        ASSERT_EQUAL(s2_stripped, s2_res);
    }
    {
        string s3 {"                many_strips_front"};
        const string s3_res {"many_strips_front"};
        auto s3_stripped = SU::Strip(s3);
        ASSERT_EQUAL(s3_stripped, s3_res);
    }
    {
        string s2{"one_strip_back "};
        const string s2_res{"one_strip_back"};
        auto s2_stripped = SU::Strip(s2);
        ASSERT_EQUAL(s2_stripped, s2_res);
    }
    {
        string s3 {"many_strips_back            "};
        const string s3_res {"many_strips_back"};
        auto s3_stripped = SU::Strip(s3);
        ASSERT_EQUAL(s3_stripped, s3_res);
    }
    {
        string s3 {"      many_strips            "};
        const string s3_res {"many_strips"};
        auto s3_stripped = SU::Strip(s3);
        ASSERT_EQUAL(s3_stripped, s3_res);
    }
    {
        string s3 {" "};
        const string s3_res {""};
        auto s3_stripped = SU::Strip(s3);
        ASSERT_EQUAL(s3_stripped, s3_res);
    }
}

void TestRemovePrefix()
{
    using SU = eft::StringUtils;
    using std::string;
    {
        string s {"abcd"};
        const string prefix {"a"};
        const string s_res{"bcd"};
        SU::RemovePrefix(s, prefix);
        ASSERT_EQUAL(s, s_res);
    }
    {
        string s {"aaabcd"};
        const string prefix {"aaa"};
        const string s_res{"bcd"};
        SU::RemovePrefix(s, prefix);
        ASSERT_EQUAL(s, s_res);
    }
    {
        string s {"abcd"};
        const string prefix {"e"};
        const string s_res{"abcd"};
        SU::RemovePrefix(s, prefix);
        ASSERT_EQUAL(s, s_res);
    }
    {
        string s {"abcd"};
        const string prefix {"abcdefghfkfls fdfdfd"};
        const string s_res{"abcd"};
        SU::RemovePrefix(s, prefix);
        ASSERT_EQUAL(s, s_res);
    }
}
void TestRemoveSuffix()
{
    using SU = eft::StringUtils;
    using std::string;
    {
        string s {"abcd"};
        const string suffix {"d"};
        const string s_res{"abc"};
        SU::RemoveSuffix(s, suffix);
        ASSERT_EQUAL(s, s_res);
    }
    {
        string s {"abcddd"};
        const string suffix {"ddd"};
        const string s_res{"abc"};
        SU::RemoveSuffix(s, suffix);
        ASSERT_EQUAL(s, s_res);
    }
    {
        string s {"abcd"};
        const string suffix {"e"};
        const string s_res{"abcd"};
        SU::RemoveSuffix(s, suffix);
        ASSERT_EQUAL(s, s_res);
    }
    {
        string s {"abcd"};
        const string suffix {"abcdefghfkfls fdfdfd"};
        const string s_res{"abcd"};
        SU::RemoveSuffix(s, suffix);
        ASSERT_EQUAL(s, s_res);
    }
    {
        string s {"abcd"};
        const string suffix {""};
        const string s_res{"abcd"};
        SU::RemoveSuffix(s, suffix);
        ASSERT_EQUAL(s, s_res);
    }
}

void TestReplace()
{
    using SU = eft::StringUtils;
    using std::string;

    {
        string s{"bad_string"};
        const string replace_what{"bad"};
        const string replace_with{"good"};
        SU::Replace(s, replace_what, replace_with);
        ASSERT_EQUAL(s, "good_string");
    }
    {
        string s{"nothing_replace"};
        const string replace_what{""};
        const string replace_with{""};
        SU::Replace(s, replace_what, replace_with);
        ASSERT_EQUAL(s, "nothing_replace");
    }
    {
        string s{"replace_this"};
        const string replace_what{"this"};
        const string replace_with{""};
        SU::Replace(s, replace_what, replace_with);
        ASSERT_EQUAL(s, "replace_");
    }
    {
        string s{"replace_nothing_with_something"};
        const string replace_what{""};
        const string replace_with{"something"};
        SU::Replace(s, replace_what, replace_with);
        ASSERT_EQUAL(s, "replace_nothing_with_something");
    }
    {
        string s{"experiment_cool_thisChannel_systematics_some_combination_channel"};
        const string replace_what_1{"experiment"};
        const string replace_with_1{"ATLAS"};
        const string replace_what_2{"some_combination_channel"};
        const string replace_with_2{"HComb"};
        SU::Replace(s, replace_what_1, replace_with_1);
        SU::Replace(s, replace_what_2, replace_with_2);
        ASSERT_EQUAL(s, "ATLAS_cool_Hgg_systematics_HComb");
    }
}

void TestStringUtils()
{
    TestRunner tr;
    RUN_TEST(tr, TestJoin);
    RUN_TEST(tr, TestStrip);
    RUN_TEST(tr, TestRemovePrefix);
    RUN_TEST(tr, TestRemoveSuffix);
    RUN_TEST(tr, TestReplace);
}
