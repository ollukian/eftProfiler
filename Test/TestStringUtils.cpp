//
// Created by Aleksei Lukianchuk on 15-Nov-22.
//

#include "../Utils/StringUtils.h"
#include "test_runner.h"
#include "Tester.h"
#include "EftTests.h"

using SU = eft::StringUtils;
using std::string;
using std::vector;

using Sentence = string;
using Words = vector<string>;

void TestJoin()
{
    {
        Words words{"one", "two", "three"};
        const auto obtained = SU::Join(' ', words);
        const Sentence expected{"one two three"};
        ASSERT_EQUAL(expected, obtained);
    }
    {
        Words words{"one", "two"};
        const auto obtained = SU::Join('_', words);
        const Sentence expected{"one_two"};
        ASSERT_EQUAL(expected, obtained);
    }
    {
        Words words {"one"};
        const auto obtained = SU::Join('*', words);
        const Sentence expected{"one"};
        ASSERT_EQUAL(expected, obtained);
    }
    {
        Words words {};
        const auto obtained = SU::Join(' ', words);
        const Sentence expected {};
        ASSERT(obtained.empty());
    }
    {
        Words words {"one"};
        const auto obtained = SU::Join("__", words);
        const Sentence expected{"one"};
        ASSERT_EQUAL(expected, obtained);
    }
    {
        Words words {"one", "two"};
        const auto obtained = SU::Join("__", words);
        const Sentence expected{"one__two"};
        ASSERT_EQUAL(expected, obtained);
    }
}
void TestStrip()
{
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
        string s3 {"  "};
        const string s3_res {""};
        auto s3_stripped = SU::Strip(s3);
        ASSERT_EQUAL(s3_stripped, s3_res);
    }
}

void TestStripCopy()
{
    {
        std::string_view s {" no_copy"};
        auto s_copied_stripped = SU::StripCopy(s);
        ASSERT_EQUAL(s_copied_stripped, "no_copy");
        ASSERT_EQUAL(s,                 " no_copy");
    }
    {
        std::string_view s {" no_copy"};
        SU::Strip(s);
        ASSERT_EQUAL(s, "no_copy");
    }
}

void TestRemovePrefix()
{

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
    {
        string s {"bcda"};
        const string prefix {"a"};
        const string s_res{"bcda"};
        SU::RemovePrefix(s, prefix);
        ASSERT_EQUAL(s, s_res);
    }
    {
        string s {"[text"};
        const string prefix {"["};
        const string s_res{"text"};
        SU::RemovePrefix(s, prefix);
        ASSERT_EQUAL(s, s_res);
    }
}
void TestRemoveSuffix()
{

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
    {
        string s {"abcd"};
        const string suffix {"a"};
        const string s_res{"abcd"};
        SU::RemoveSuffix(s, suffix);
        ASSERT_EQUAL(s, s_res);
    }
    {
        string s {"text]"};
        const string suffix {"]"};
        const string s_res{"text"};
        SU::RemoveSuffix(s, suffix);
        ASSERT_EQUAL(s, s_res);
    }
}
void TestReplace()
{

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
        const string replace_what_1 {"experiment"};
        const string replace_with_1 {"ATLAS"};
        const string replace_what_2 {"some_combination_channel"};
        const string replace_with_2 {"HComb"};
        const string replace_what_3 {"thisChannel"};
        const string replace_with_3 {"Hgg"};
        SU::Replace(s, replace_what_1, replace_with_1);
        SU::Replace(s, replace_what_2, replace_with_2);
        SU::Replace(s, replace_what_3, replace_with_3);
        ASSERT_EQUAL(s, "ATLAS_cool_Hgg_systematics_HComb");
    }
}
void TestSplit()
{

    {
        const Sentence s {"no_split"};
        const auto obtained = SU::Split(s, ' ');
        const Words expected {"no_split"};
        ASSERT_EQUAL(expected, obtained);
    }
    {
        const Sentence s {"one two"};
        const auto obtained = SU::Split(s, ' ');
        const Words expected {"one", "two"};
        ASSERT_EQUAL(expected, obtained);
    }
    {
        const Sentence s {"hey_there_i_am_here"};
        const auto obtained = SU::Split(s, '_');
        const Words expected {"hey", "there", "i", "am", "here"};
        ASSERT_EQUAL(expected, obtained);
    }
}

void TestToLowCase() {
    {
        Sentence s {"Simple"};
        SU::ToLowCase(s);
        ASSERT_EQUAL(s, "simple");
    }
    {
        Sentence s {"SimPleWithLEtTERS"};
        SU::ToLowCase(s);
        ASSERT_EQUAL(s, "simplewithletters");
    }
    {
        Sentence s {"With Spaces HERE"};
        SU::ToLowCase(s);
        ASSERT_EQUAL(s, "with spaces here");
    }
    {
        Sentence s {"   raNDom    spaces  "};
        SU::ToLowCase(s);
        ASSERT_EQUAL(s, "   random    spaces  ");
    }
    {
        Sentence s {"all low"};
        SU::ToLowCase(s);
        ASSERT_EQUAL(s, "all low");
    }
    {
        Sentence s {""};
        SU::ToLowCase(s);
        ASSERT_EQUAL(s, "");
    }
}

void TestToUpperCase() {
    {
        Sentence s {"Simple"};
        SU::ToUpperCase(s);
        ASSERT_EQUAL(s, "SIMPLE");
    }
    {
        Sentence s {"SimPleWithLEtTERS"};
        SU::ToUpperCase(s);
        ASSERT_EQUAL(s, "SIMPLEWITHLETTERS");
    }
    {
        Sentence s {"with spaces here"};
        SU::ToUpperCase(s);
        ASSERT_EQUAL(s, "WITH SPACES HERE");
    }
    {
        Sentence s {"WITH SPACES HERE"};
        SU::ToUpperCase(s);
        ASSERT_EQUAL(s, "WITH SPACES HERE");
    }
    {
        Sentence s {"   RANDOM    SPACES  "};
        SU::ToUpperCase(s);
        ASSERT_EQUAL(s, "   RANDOM    SPACES  ");
    }
    {
        Sentence s {"all low"};
        SU::ToUpperCase(s);
        ASSERT_EQUAL(s, "ALL LOW");
    }
    {
        Sentence s {""};
        SU::ToUpperCase(s);
        ASSERT_EQUAL(s, "");
    }
    {
        Sentence s {"ALL UP"};
        SU::ToUpperCase(s);
        ASSERT_EQUAL(s, "ALL UP");
    }
}

EFT_IMPLEMENT_TESTFILE(StringUtils) {
    EFT_ADD_TEST(TestJoin,          "StringUtils");
    EFT_ADD_TEST(TestStrip,         "StringUtils");
    EFT_ADD_TEST(TestStripCopy,     "StringUtils");
    EFT_ADD_TEST(TestRemovePrefix,  "StringUtils");
    EFT_ADD_TEST(TestRemoveSuffix,  "StringUtils");
    EFT_ADD_TEST(TestReplace,       "StringUtils");
    EFT_ADD_TEST(TestSplit,         "StringUtils");
    EFT_ADD_TEST(TestToLowCase,     "StringUtils");
    EFT_ADD_TEST(TestToUpperCase,   "StringUtils");
    // TestRunner tr;
    // RUN_TEST(tr, TestJoin);
    // RUN_TEST(tr, TestStrip);
    // RUN_TEST(tr, TestRemovePrefix);
    // RUN_TEST(tr, TestRemoveSuffix);
    // RUN_TEST(tr, TestReplace);
    // RUN_TEST(tr, TestSplit);
}
EFT_END_IMPLEMENT_TESTFILE();