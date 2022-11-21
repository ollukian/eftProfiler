//
// Created by Aleksei Lukianchuk on 21-Nov-22.
//

#include "Tester.h"
#include "../Core/CommandLineArgs.h"
#include "../Utils/StringUtils.h"

using namespace std;

// helpers to emulate argc & argv passed to the command line
void ConvertToArgcAgv(istringstream& s, int& argc, char** argv)
{
    string as_string{s.str()};
    as_string = "Test_to_emulate_executable_name " + as_string;
    EFT_PROF_INFO("convert {} to argc, argv", as_string);
    auto components = eft::StringUtils::Split(as_string, ' ');
    EFT_PROF_DEBUG("received {} components", components);
    argc = components.size();
    EFT_PROF_DEBUG("argc: {}", argc);
    argv = new char* [argc];
    for (size_t idx {}; static_cast<int>(idx) < argc; ++idx) {
        EFT_PROF_DEBUG(" try initiate argv[{}] = {}", idx, components[idx]);
        argv[idx] = new char [components[idx].size() - 1];
        for (size_t idx_inner {0}; idx_inner < components[idx].size() - 1; ++idx_inner) {
            argv[idx][idx_inner] = components[idx][idx_inner];
        }
        //memcpy(argv[idx], components[idx].data(), components[idx].size());

        //argv[idx] = components[idx].data();
        //EFT_PROF_DEBUG(" try initiate argv[{}] = {} ==> done", idx, components[idx]);
        EFT_PROF_DEBUG("argv[{}] = [{}]", idx, argv[idx]);
    }
    EFT_PROF_DEBUG("ConvertToArgcAgv is done, we're still in the funcrtion, let's see argv:");
    EFT_PROF_INFO("argv:");
    for (size_t idx {0}; idx != argc; ++idx) {
        EFT_PROF_INFO("token: [{}]", argv[idx]);
    }
}

void ConvertToArgcAgv(const vector<string>& s, int& argc, char** argv)
{
    string joined = eft::StringUtils::Join(' ', s);
    istringstream is{joined};
    return ConvertToArgcAgv(is, argc, argv);
}

void TestBasicArgParsing() {
    eft::stats::Logger::SetActive();
    {
        istringstream arguments {"--one_key one_value"};
        int argc {0};
        char** argv = nullptr;
        ConvertToArgcAgv(arguments, argc, argv);

        EFT_PROF_INFO("after function: argv:");
        for (size_t idx {0}; idx != argc; ++idx) {
            EFT_PROF_INFO("token: {}", argv[idx]);
        }

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
        ConvertToArgcAgv(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        ASSERT(cmd.HasKey("one_key"));
        ASSERT(cmd.GetVal("one_key").has_value());
        ASSERT_EQUAL(cmd.GetVals("one_key").value().size(), 2);
    }
    {
        istringstream arguments {"--only_key"};
        int argc {0};
        char** argv = nullptr;
        ConvertToArgcAgv(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        ASSERT(cmd.HasKey("only_key"));
        ASSERT_NOT(cmd.GetVal("one_key").has_value());
    }
    {
        istringstream arguments {"--first --second --third --fourth 1 2 3 4 5 --fifth 6"};
        int argc {0};
        char** argv = nullptr;
        ConvertToArgcAgv(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        ASSERT(cmd.HasKey("first"));
        ASSERT(cmd.HasKey("second"));
        ASSERT(cmd.HasKey("third"));
        ASSERT(cmd.HasKey("fourth"));
        ASSERT(cmd.HasKey("fifth"));
        ASSERT_EQUAL(cmd.GetKeys().size(), 5);
        ASSERT_EQUAL(cmd.GetVals("first")->size(), 0);
        ASSERT_EQUAL(cmd.GetVals("second")->size(), 0);
        ASSERT_EQUAL(cmd.GetVals("third")->size(), 0);
        ASSERT_EQUAL(cmd.GetVals("fourth")->size(), 5);
        ASSERT_EQUAL(cmd.GetVals("fifth")->size(), 6);
    }
}

void TestNegativeParsing() {
    {
        istringstream arguments {"--key_to_neg -1"};
        int argc {0};
        char** argv = nullptr;
        ConvertToArgcAgv(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        ASSERT(cmd.HasKey("key_to_neg"));
        ASSERT(cmd.GetVal("key_to_neg").has_value());
        ASSERT_EQUAL(cmd.GetVal("key_to_neg").value(), "-1");
    }
    {
        istringstream arguments {"--key_to_neg -1 -2 -3 --another_key"};
        int argc {0};
        char** argv = nullptr;
        ConvertToArgcAgv(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        ASSERT(cmd.HasKey("key_to_neg"));
        ASSERT(cmd.GetVal("key_to_neg").has_value());
        ASSERT_EQUAL(cmd.GetVals("key_to_neg").value()[0], "-1");
        ASSERT_EQUAL(cmd.GetVals("key_to_neg").value()[1], "-2");
        ASSERT_EQUAL(cmd.GetVals("key_to_neg").value()[2], "-3");
    }
}

void TestSetValIfArgExistsBOOL()
{
    {
        istringstream arguments {"--no_gamma"};
        int argc {0};
        char** argv = nullptr;
        ConvertToArgcAgv(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        bool bool_val_1 = false;
        ASSERT( cmd.SetValIfArgExists("no_gamma", bool_val_1) );
        ASSERT(bool_val_1);
        ASSERT_NOT( cmd.SetValIfArgExists("gamma", bool_val_1) );
        ASSERT(bool_val_1);
    }
}

void TestSetValIfArgExistsFloat()
{
    {
        istringstream arguments {"--key1 1.2 --key2 2.3 --key3 3.4"};
        int argc {0};
        char** argv = nullptr;
        ConvertToArgcAgv(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        float my_float_1 = 0.1f;
        float my_float_2 = 0.2f;
        float my_float_3 = 0.3f;
        ASSERT( cmd.SetValIfArgExists("key1", my_float_1));
        ASSERT( cmd.SetValIfArgExists("key2", my_float_2));
        ASSERT( cmd.SetValIfArgExists("key3", my_float_3));

        ASSERT_EQUAL(my_float_1, 1.2f);
        ASSERT_EQUAL(my_float_2, 2.3f);
        ASSERT_EQUAL(my_float_3, 3.4f);
    }
}

void TestSetValIfArgExistsString()
{
    {
        istringstream arguments {"--input my_cool_input --filename my_fyle.root"};
        int argc {0};
        char** argv = nullptr;
        ConvertToArgcAgv(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        string my_string1;
        string my_string2;

        ASSERT( cmd.SetValIfArgExists("input", my_string1));
        ASSERT( cmd.SetValIfArgExists("filename", my_string2));
        ASSERT_EQUAL(my_string1, "my_cool_input");
        ASSERT_EQUAL(my_string2, "my_fyle.root");
    }
}

void TestSetValIfArgExistsVectorStrings()
{
    {
        istringstream arguments {"--input my_cool_input --filename file1.root file2.root"};
        int argc {0};
        char** argv = nullptr;
        ConvertToArgcAgv(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        vector<string> my_vec_1;
        vector<string> my_vec_2;

        ASSERT( cmd.SetValIfArgExists("input", my_vec_1));
        ASSERT( cmd.SetValIfArgExists("filename", my_vec_2));
        ASSERT_EQUAL(my_vec_1, vector<string>({"my_cool_input"}) );
        ASSERT_EQUAL(my_vec_2, vector<string>({"file1.root", "file2.root"}));
    }
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
