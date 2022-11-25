//
// Created by Aleksei Lukianchuk on 21-Nov-22.
//

#include "Tester.h"
#include "../Core/CommandLineArgs.h"
#include "../Utils/StringUtils.h"
#include "spdlog/fmt/bundled/ostream.h"
#include "spdlog/fmt/bundled/color.h"
#include "spdlog/fmt/bundled/core.h"

using namespace std;

// helpers to emulate argc & argv passed to the command line
vector<vector<char>> ConvertToArgcAgv(istringstream& s)
{
    string as_string{s.str()};
    as_string = "Test_to_emulate_executable_name " + as_string;
    EFT_PROF_INFO("convert {} to argc, argv", as_string);
    auto components = eft::StringUtils::Split(as_string, ' ');

    vector<vector<char>> vstrings;

    size_t nb_components = components.size();
    vstrings.reserve(nb_components);

    for (size_t idx {}; idx < nb_components; ++idx) {
        vstrings.emplace_back(components[idx].begin(), components[idx].end());
        vstrings.back().push_back('\0');
    }
    EFT_PROF_INFO("argv:");
    for (size_t idx {0}; idx != nb_components; ++idx) {
        size_t nb_chars_line {vstrings[idx].size()};
        fmt::print(fmt::fg(fmt::color::aqua), "Arg#{} ==> [", idx);
        for (size_t idx_char {0}; idx_char < nb_chars_line; ++idx_char) {
            fmt::print(fmt::fg(fmt::color::aqua), "{}", vstrings[idx][idx_char]);
        }
        fmt::print(fmt::fg(fmt::color::aqua), "]\n");
    }
    return vstrings;
}

vector<vector<char>> ConvertToArgcAgv(const vector<string>& s)
{
    string joined = eft::StringUtils::Join(' ', s);
    istringstream is{joined};
    return ConvertToArgcAgv(is);
}

void GetArgcArgvFromVecCharStars(istringstream& arguments, int& argc, char** argv) {
    static vector<vector<char>> vstrings;
    static vector<const char*>  cstrings;

    vstrings.clear();
    cstrings.clear();

    vstrings = ConvertToArgcAgv(arguments);
    cstrings.reserve(vstrings.size());

    for(auto& s: vstrings)
        cstrings.push_back(s.data());

    argv = const_cast<char **>(cstrings.data());
    argc = cstrings.size();
}

void TestBasicArgParsing() {
    eft::stats::Logger::SetFullPrinting();
    {
        istringstream arguments {"--one_key one_value"};
        int argc {0};
        char** argv = nullptr;
//        const auto vstring = ConvertToArgcAgv(arguments);
//
//        std::vector<const char*> cstrings;
//        cstrings.reserve(vstring.size());
//
//        for(auto& s: vstring)
//            cstrings.push_back(s.data());
//
//        argv = const_cast<char **>(cstrings.data());
//        argc = cstrings.size();
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        ASSERT(cmd.HasKey("one_key"));
        ASSERT(cmd.GetVal("one_key").has_value());
        ASSERT_EQUAL(cmd.GetVal("one_key").value(), "one_value");
        ASSERT_NOT(cmd.HasKey("random_not_present_key"));
        ASSERT_NOT(cmd.GetVal("random_not_present_key").has_value());
    }
    {
        istringstream arguments {"--one_key first_value second_value"};
        int argc {0};
        char** argv = nullptr;
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        ASSERT(cmd.HasKey("one_key"));
        ASSERT(cmd.GetVal("one_key").has_value());
        ASSERT_EQUAL(cmd.GetVals("one_key").value().size(), 2);
    }
//    {
//        istringstream arguments {"--only_key"};
//        int argc {0};
//        char** argv = nullptr;
//        ConvertToArgcAgv(arguments, argc, argv);
//        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
//        CommandLineArgs cmd(argc, argv);
//        ASSERT(cmd.HasKey("only_key"));
//        ASSERT_NOT(cmd.GetVal("one_key").has_value());
//    }
//    {
//        istringstream arguments {"--first --second --third --fourth 1 2 3 4 5 --fifth 6"};
//        int argc {0};
//        char** argv = nullptr;
//        ConvertToArgcAgv(arguments, argc, argv);
//        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
//        CommandLineArgs cmd(argc, argv);
//        ASSERT(cmd.HasKey("first"));
//        ASSERT(cmd.HasKey("second"));
//        ASSERT(cmd.HasKey("third"));
//        ASSERT(cmd.HasKey("fourth"));
//        ASSERT(cmd.HasKey("fifth"));
//        ASSERT_EQUAL(cmd.GetKeys().size(), 5);
//        ASSERT_EQUAL(cmd.GetVals("first")->size(), 0);
//        ASSERT_EQUAL(cmd.GetVals("second")->size(), 0);
//        ASSERT_EQUAL(cmd.GetVals("third")->size(), 0);
//        ASSERT_EQUAL(cmd.GetVals("fourth")->size(), 5);
//        ASSERT_EQUAL(cmd.GetVals("fifth")->size(), 6);
//    }
}

void TestNegativeParsing() {
//    {
//        istringstream arguments {"--key_to_neg -1"};
//        int argc {0};
//        char** argv = nullptr;
//        ConvertToArgcAgv(arguments, argc, argv);
//        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
//        CommandLineArgs cmd(argc, argv);
//        ASSERT(cmd.HasKey("key_to_neg"));
//        ASSERT(cmd.GetVal("key_to_neg").has_value());
//        ASSERT_EQUAL(cmd.GetVal("key_to_neg").value(), "-1");
//    }
//    {
//        istringstream arguments {"--key_to_neg -1 -2 -3 --another_key"};
//        int argc {0};
//        char** argv = nullptr;
//        ConvertToArgcAgv(arguments, argc, argv);
//        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
//        CommandLineArgs cmd(argc, argv);
//        ASSERT(cmd.HasKey("key_to_neg"));
//        ASSERT(cmd.GetVal("key_to_neg").has_value());
//        ASSERT_EQUAL(cmd.GetVals("key_to_neg").value()[0], "-1");
//        ASSERT_EQUAL(cmd.GetVals("key_to_neg").value()[1], "-2");
//        ASSERT_EQUAL(cmd.GetVals("key_to_neg").value()[2], "-3");
//    }
}

void TestSetValIfArgExistsBOOL()
{
//    {
//        istringstream arguments {"--no_gamma"};
//        int argc {0};
//        char** argv = nullptr;
//        ConvertToArgcAgv(arguments, argc, argv);
//        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
//        CommandLineArgs cmd(argc, argv);
//        bool bool_val_1 = false;
//        ASSERT( cmd.SetValIfArgExists("no_gamma", bool_val_1) );
//        ASSERT(bool_val_1);
//        ASSERT_NOT( cmd.SetValIfArgExists("gamma", bool_val_1) );
//        ASSERT(bool_val_1);
//    }
}

void TestSetValIfArgExistsFloat()
{
//    {
//        istringstream arguments {"--key1 1.2 --key2 2.3 --key3 3.4"};
//        int argc {0};
//        char** argv = nullptr;
//        ConvertToArgcAgv(arguments, argc, argv);
//        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
//        CommandLineArgs cmd(argc, argv);
//        float my_float_1 = 0.1f;
//        float my_float_2 = 0.2f;
//        float my_float_3 = 0.3f;
//        ASSERT( cmd.SetValIfArgExists("key1", my_float_1));
//        ASSERT( cmd.SetValIfArgExists("key2", my_float_2));
//        ASSERT( cmd.SetValIfArgExists("key3", my_float_3));
//
//        ASSERT_EQUAL(my_float_1, 1.2f);
//        ASSERT_EQUAL(my_float_2, 2.3f);
//        ASSERT_EQUAL(my_float_3, 3.4f);
//    }
}

void TestSetValIfArgExistsString()
{
//    {
//        istringstream arguments {"--input my_cool_input --filename my_fyle.root"};
//        int argc {0};
//        char** argv = nullptr;
//        ConvertToArgcAgv(arguments, argc, argv);
//        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
//        CommandLineArgs cmd(argc, argv);
//        string my_string1;
//        string my_string2;
//
//        ASSERT( cmd.SetValIfArgExists("input", my_string1));
//        ASSERT( cmd.SetValIfArgExists("filename", my_string2));
//        ASSERT_EQUAL(my_string1, "my_cool_input");
//        ASSERT_EQUAL(my_string2, "my_fyle.root");
//    }
}

void TestSetValIfArgExistsVectorStrings()
{
//    {
//        istringstream arguments {"--input my_cool_input --filename file1.root file2.root"};
//        int argc {0};
//        char** argv = nullptr;
//        ConvertToArgcAgv(arguments, argc, argv);
//        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
//        CommandLineArgs cmd(argc, argv);
//        vector<string> my_vec_1;
//        vector<string> my_vec_2;
//
//        ASSERT( cmd.SetValIfArgExists("input", my_vec_1));
//        ASSERT( cmd.SetValIfArgExists("filename", my_vec_2));
//        ASSERT_EQUAL(my_vec_1, vector<string>({"my_cool_input"}) );
//        ASSERT_EQUAL(my_vec_2, vector<string>({"file1.root", "file2.root"}));
//    }
}

void TestIncorrectArgs() {
    {
        istringstream arguments {"--key1 1 --key2"};
        EFT_PROF_CRITICAL("Implement");
    }
}



EFT_IMPLEMENT_TESTFILE(CommandLineArguments) {
        EFT_ADD_TEST(TestBasicArgParsing,  "CommandLineArguments");
        EFT_ADD_TEST(TestNegativeParsing,  "CommandLineArguments");
        //EFT_ADD_TEST(TestIncorrectArgs,    "CommandLineArguments");
        EFT_ADD_TEST(TestSetValIfArgExistsBOOL, "CommandLineArguments");
        EFT_ADD_TEST(TestSetValIfArgExistsFloat, "CommandLineArguments");
        EFT_ADD_TEST(TestSetValIfArgExistsString, "CommandLineArguments");
        EFT_ADD_TEST(TestSetValIfArgExistsVectorStrings, "CommandLineArguments");
}
EFT_END_IMPLEMENT_TESTFILE(ColourUtils);
