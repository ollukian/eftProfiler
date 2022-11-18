//
// Created by Aleksei Lukianchuk on 18-Nov-22.
//

#include "../Utils/FileSystemUtils.h"
#include "test_runner.h"
#include "Tester.h"
#include "EftTests.h"

using FS = eft::utils::FileSystemUtils;
using namespace std;
using namespace filesystem;

void TestReadLines()
{

    {
        const filesystem::path test_path {"__tmp_test_file.txt"};
        fstream fs(test_path, fstream::in | fstream::out);
        ASSERT(fs.is_open());

        fs << "first line" << endl;
        fs << "second line" << endl;
        const auto res = FS::ReadLines(test_path);
        ASSERT(res.has_value());
        ASSERT_EQUAL(res->size(), 2);
        ASSERT_EQUAL(res.value()[0], "first line");
        ASSERT_EQUAL(res.value()[1], "second line");
        filesystem::remove(test_path);
        ASSERT( ! exists(test_path) );
    }
    {
        const filesystem::path test_path {"__tmp_test_file.txt"};
        fstream fs(test_path, fstream::in | fstream::out);
        ASSERT(fs.is_open());

        const std::string line {"one line with a lot of things here even if they take a looooot of space such that you cannot even imagine it for yourself"};
        fs << line << endl;
        const auto res = FS::ReadLines(test_path);
        ASSERT(res.has_value());
        ASSERT_EQUAL(res->size(), 1);
        ASSERT_EQUAL(res.value()[0], line);
        filesystem::remove(test_path);
        ASSERT( ! exists(test_path) );
    }
    {
        const std::string test_path ("__tmp_test_file.txt");
        fstream fs(test_path, fstream::in | fstream::out);
        ASSERT(fs.is_open());

        const std::string line {"one line with a lot of things here even if they take a looooot of space such that you cannot even imagine it for yourself"};
        fs << line << endl;
        const auto res = FS::ReadLines(test_path);
        ASSERT(res.has_value());
        ASSERT_EQUAL(res->size(), 1);
        ASSERT_EQUAL(res.value()[0], line);
        filesystem::remove(test_path);
        ASSERT( ! exists(test_path) );
    }
}


EFT_IMPLEMENT_TESTFILE(FileSystem)
{
    EFT_ADD_TEST(TestReadLines, "filesystem");
    // TestRunner tr;
    // RUN_TEST(tr, TestReadLines);
}
EFT_END_IMPLEMENT_TESTFILE(FileSystem);


