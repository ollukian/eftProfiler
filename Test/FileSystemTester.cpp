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
        fstream fs_out(test_path, fstream::out);
        ASSERT(fs_out.is_open());

        fs_out << "first line" << endl;
        fs_out << "second line" << endl;
        fs_out.close();
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
        fstream fs_out(test_path, fstream::out);
        ASSERT(fs_out.is_open());

        const std::string line {"one line with a lot of things here even if they take a looooot of space such that you cannot even imagine it for yourself"};
        fs_out << line << endl;
        fs_out.close();
        const auto res = FS::ReadLines(test_path);
        ASSERT(res.has_value());
        ASSERT_EQUAL(res->size(), 1);
        ASSERT_EQUAL(res.value()[0], line);
        filesystem::remove(test_path);
        ASSERT( ! exists(test_path) );
    }
    {
        const std::string test_path ("__tmp_test_file.txt");
        fstream fs_out(test_path, fstream::out);
        ASSERT(fs_out.is_open());

        const std::string line {"one line with a lot of things here even if they take a looooot of space such that you cannot even imagine it for yourself"};
        fs_out << line << endl;
        fs_out.close();
        const auto res = FS::ReadLines(test_path);
        ASSERT(res.has_value());
        ASSERT_EQUAL(res->size(), 1);
        ASSERT_EQUAL(res.value()[0], line);
        filesystem::remove(test_path);
        ASSERT( ! exists(test_path) );
    }
}

//void TestGetListOfFiles()
//{
//    filesystem::create_directory("__tmp_dir");
//}


EFT_IMPLEMENT_TESTFILE(FileSystem)
{
    EFT_ADD_TEST(TestReadLines, "filesystem");
}
EFT_END_IMPLEMENT_TESTFILE(FileSystem);


