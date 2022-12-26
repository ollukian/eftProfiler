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

//static vector<vector<char>> vstrings;
//static vector<const char*>  cstrings;

// helpers to emulate argc & argv passed to the command line
//vector<vector<char>> ConvertToArgcAgv(istringstream& s)
//{
//    string as_string{s.str()};
//    as_string = "Test_to_emulate_executable_name " + as_string;
//    EFT_PROF_INFO("convert {} to argc, argv", as_string);
//    auto components = eft::StringUtils::Split(as_string, ' ');
//
//    vector<vector<char>> vstrings;
//
//    size_t nb_components = components.size();
//    vstrings.reserve(nb_components);
//
//    for (size_t idx {}; idx < nb_components; ++idx) {
//        vstrings.emplace_back(components[idx].begin(), components[idx].end());
//        vstrings.back().push_back('\0');
//    }
//    return vstrings;
//}
//
//vector<vector<char>> ConvertToArgcAgv(const vector<string>& s)
//{
//    string joined = eft::StringUtils::Join(' ', s);
//    istringstream is{joined};
//    return ConvertToArgcAgv(is);
//}

//void GetArgcArgvFromVecCharStars(istringstream& arguments, int& argc, char**& argv) {
//    //static vector<vector<char>> vstrings;
//    //static vector<const char*>  cstrings;
//
//    vstrings.clear();
//    cstrings.clear();
//
//    vstrings = ConvertToArgcAgv(arguments);
//    cstrings.reserve(vstrings.size());
//
//    for(auto& s: vstrings)
//        cstrings.push_back(s.data());
//
//    argv = const_cast<char **>(cstrings.data());
//    argc = cstrings.size();
//}

void TestBasicArgParsing() {
    {
        istringstream arguments {"--one_key one_value"};
        int argc {0};
        char** argv = nullptr;
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
    {
        istringstream arguments {"--only_key"};
        int argc {0};
        char** argv = nullptr;
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        ASSERT(cmd.HasKey("only_key"));
        ASSERT_NOT(cmd.GetVal("one_key").has_value());
    }
    {
        istringstream arguments {"--first --second --third --fourth 1 2 3 4 5 --fifth 6"};
        int argc {0};
        char** argv = nullptr;
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        ASSERT(cmd.HasKey("first"));
        ASSERT(cmd.HasKey("second"));
        ASSERT(cmd.HasKey("third"));
        ASSERT(cmd.HasKey("fourth"));
        ASSERT(cmd.HasKey("fifth"));

        ASSERT_NOT(cmd.HasKey("no"));
        ASSERT_NOT(cmd.HasKey("empty"));
        ASSERT_NOT(cmd.HasKey(""));
        string val1;
        ASSERT_NOT(cmd.SetValIfArgExists(val1, "empty"));
        ASSERT_NOT(cmd.SetValIfArgExists(val1, ""));

        ASSERT_EQUAL(cmd.GetKeys().size(), 5);
        ASSERT_EQUAL(cmd.GetVals("first")->size(), 0);
        ASSERT_EQUAL(cmd.GetVals("second")->size(), 0);
        ASSERT_EQUAL(cmd.GetVals("third")->size(), 0);
        ASSERT_EQUAL(cmd.GetVals("fourth")->size(), 5);
        ASSERT_EQUAL(cmd.GetVals("fifth")->size(), 1);
    }
}

void TestNegativeParsing() {
    {
        istringstream arguments {"--key_to_neg -1"};
        int argc {0};
        char** argv = nullptr;
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
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
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        ASSERT(cmd.HasKey("key_to_neg"));
        ASSERT(cmd.HasKey("another_key"));
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
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        bool bool_val_1 = false;
        bool bool_val_2 = false;
        ASSERT( cmd.SetValIfArgExists("no_gamma", bool_val_1) );
        ASSERT(bool_val_1);
        ASSERT_NOT( cmd.SetValIfArgExists("gamma", bool_val_1) ); // val should not change
        ASSERT_NOT( cmd.SetValIfArgExists("gamma", bool_val_2) );
        ASSERT(bool_val_1);
        ASSERT_NOT(bool_val_2);
    }
    {
        istringstream arguments {"--b --first arg1 arg2 a --no_gamma --no_arg --n"};
        int argc {0};
        char** argv = nullptr;
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        bool bool_val_1 = false;

        ASSERT(cmd.HasKey("b"));
        ASSERT(cmd.HasKey("first"));
        ASSERT(cmd.HasKey("no_gamma"));
        ASSERT(cmd.HasKey("no_arg"));

        ASSERT( cmd.SetValIfArgExists("no_gamma", bool_val_1) );
        ASSERT(bool_val_1);
        ASSERT_NOT( cmd.SetValIfArgExists("gamma", bool_val_1) );
        ASSERT(bool_val_1);
    }
}

void TestSetValIfArgExistsFloat()
{
    {
        istringstream arguments {"--key1 1.2 --key2 2.3 --key3 3.4 --key4 .1"};
        int argc {0};
        char** argv = nullptr;
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        float my_float_1 = 0.1f;
        float my_float_2 = 0.2f;
        float my_float_3 = 0.3f;

        ASSERT(cmd.HasKey("key1"));
        ASSERT(cmd.HasKey("key2"));
        ASSERT(cmd.HasKey("key3"));
        ASSERT(cmd.HasKey("key4"));

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
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        string my_string1;
        string my_string2;

        ASSERT( cmd.SetValIfArgExists("input", my_string1));
        ASSERT( cmd.SetValIfArgExists("filename", my_string2));
        ASSERT_EQUAL(my_string1, "my_cool_input");
        ASSERT_EQUAL(my_string2, "my_fyle.root");
    }
    {
        istringstream arguments {"--input my_cool_input --filename my_fyle.root"};
        int argc {0};
        char** argv = nullptr;
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
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

void TestSetValIfArgExistsChar() {
    {
        istringstream arguments {"--key_for_char c --key2 r"};
        int argc {0};
        char** argv = nullptr;
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        char c1;
        char c2;

        ASSERT_NO_THROW(cmd.SetValIfArgExists("key_for_char", c1));
        ASSERT_NO_THROW(cmd.SetValIfArgExists("key2", c1));
        ASSERT_NO_THROW(cmd.SetValIfArgExists("random", c1));

        ASSERT( cmd.SetValIfArgExists("key_for_char", c1));
        ASSERT( cmd.SetValIfArgExists("key2", c2));
        ASSERT_EQUAL(c1, 'c');
        ASSERT_EQUAL(c2, 'r');
        ASSERT_NOT( cmd.SetValIfArgExists("non_existent", c2));
        ASSERT_EQUAL(c2, 'r');
    }
}

void TestSetValIfArgExistsVector()
{
    {
        istringstream arguments {"--input my_cool_input --filename file1.root file2.root"};
        int argc {0};
        char** argv = nullptr;
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        vector<string> my_vec_1;
        vector<string> my_vec_2;

        ASSERT( cmd.SetValIfArgExists("input", my_vec_1));
        ASSERT( cmd.SetValIfArgExists("filename", my_vec_2));
        ASSERT_EQUAL(my_vec_1, vector<string>({"my_cool_input"}) );
        ASSERT_EQUAL(my_vec_2, vector<string>({"file1.root", "file2.root"}));
    }
    {
        istringstream arguments {"--sequence_chars a b c d e f g h i j k l"};
        int argc {0};
        char** argv = nullptr;
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        vector<char> chars;
        vector<char> expected {'a', 'b','c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l'};

        ASSERT_NO_THROW( cmd.SetValIfArgExists("sequence_chars", chars));
        ASSERT_EQUAL(chars, expected);
    }
}

void TestSetValIfArgExistsPointer() {
    {
        istringstream arguments {"--key1 1 --key2"};
        int argc {0};
        char** argv = nullptr;
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);

        int* val1 = new int(0);

        ASSERT(val1);
        ASSERT_EQUAL(*val1, 0);

        ASSERT_NO_THROW(std::ignore = cmd.HasKey("key1"));
        ASSERT_NO_THROW(cmd.SetValIfArgExists("key1", val1));
        ASSERT_EQUAL(*val1, 1);
        delete val1; // try with raw pointer
    }
    {
        istringstream arguments {"--key1 1 --key2"};
        int argc {0};
        char** argv = nullptr;
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);

        auto val1 = make_shared<int>(0);
        ASSERT(val1.get());
        ASSERT_EQUAL(*val1, 0);

        ASSERT_NO_THROW(std::ignore = cmd.HasKey("key1"));
        ASSERT_NO_THROW(cmd.SetValIfArgExists("key1", val1));
        ASSERT_EQUAL(*val1, 1);
    }
    {
        istringstream arguments {"--input my_cool_input --filename file1.root file2.root"};
        int argc {0};
        char** argv = nullptr;
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);
        auto my_vec_1 = new std::vector<std::string>();
        auto my_vec_2 = new std::vector<std::string>();

        ASSERT( cmd.SetValIfArgExists("input", my_vec_1));
        ASSERT( cmd.SetValIfArgExists("filename", my_vec_2));
        ASSERT_EQUAL(*my_vec_1, vector<string>({"my_cool_input"}) );
        ASSERT_EQUAL(*my_vec_2, vector<string>({"file1.root", "file2.root"}));

        delete my_vec_1;
        delete my_vec_2;
    }
}

void TestSharedPtrReading() {
    {
        istringstream arguments {"--key1 1 --key2 --key_vec one two threeeee"};
        int argc {0};
        char** argv = nullptr;
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);

        auto i_ptr = make_shared<int>(0);
        auto i_ptr_2 = make_shared<int>(0);
        auto vec_ptr_1 = make_shared<vector<string>>();

        ASSERT(i_ptr.get());
        ASSERT(i_ptr_2.get());
        ASSERT(vec_ptr_1.get());
        ASSERT_EQUAL(*i_ptr, 0);
        ASSERT_EQUAL(*i_ptr_2, 0);

        ASSERT_NO_THROW(std::ignore = cmd.HasKey("key1"));
        ASSERT_NO_THROW(std::ignore = cmd.HasKey("key_not_present"));
        ASSERT_NO_THROW(std::ignore = cmd.HasKey("key_vec"));

        ASSERT_NO_THROW(cmd.SetValIfArgExists("key1", i_ptr));
        ASSERT_NO_THROW(cmd.SetValIfArgExists("key_not_present", i_ptr_2));
        ASSERT_NO_THROW(cmd.SetValIfArgExists("key_vec", vec_ptr_1));
        ASSERT_EQUAL(*i_ptr, 1);
        ASSERT_EQUAL(*i_ptr_2, 0);
        const vector<string> expected {"one", "two", "threeeee"};
        ASSERT_EQUAL(*vec_ptr_1, expected);
    }
}

void TestIncorrectArgs() {
    {
        istringstream arguments {"---key1 1 --key2"};
        int argc {0};
        char** argv = nullptr;
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);

        ASSERT_NO_THROW(std::ignore = cmd.HasKey("-key1"));
        ASSERT(cmd.HasKey("-key1"));
        ASSERT(cmd.HasKey("key2"));
        ASSERT_EQUAL(cmd.GetKeys().size(), 2);
    }
}



EFT_IMPLEMENT_TESTFILE(CommandLineArguments) {
        EFT_ADD_TEST(TestBasicArgParsing,                   "CommandLineArguments");
        EFT_ADD_TEST(TestNegativeParsing,                   "CommandLineArguments");
        EFT_ADD_TEST(TestSetValIfArgExistsBOOL,             "CommandLineArguments");
        EFT_ADD_TEST(TestSetValIfArgExistsChar,             "CommandLineArguments");
        EFT_ADD_TEST(TestSetValIfArgExistsFloat,            "CommandLineArguments");
        EFT_ADD_TEST(TestSetValIfArgExistsString,           "CommandLineArguments");
        EFT_ADD_TEST(TestSetValIfArgExistsVector,           "CommandLineArguments");
        EFT_ADD_TEST(TestIncorrectArgs,                     "CommandLineArguments");
        EFT_ADD_TEST(TestSetValIfArgExistsPointer,          "CommandLineArguments");
        EFT_ADD_TEST(TestSharedPtrReading,                  "CommandLineArguments");
}
EFT_END_IMPLEMENT_TESTFILE(ColourUtils);
