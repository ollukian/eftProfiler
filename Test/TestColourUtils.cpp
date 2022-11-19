//
// Created by Aleksei Lukianchuk on 19-Nov-22.
//

#include "Tester.h"
#include "../Utils/ColourUtils.h"
#include "../Core/Logger.h"

#include <sstream>

using namespace eft::utils;
using namespace std;

void TestColourCreationRGBNormalConstructor()
{
    {
        istringstream is {"RGB(0, 0, 0)"};
        const auto color = Colour::CreateFromString(is.str());
        ASSERT_EQUAL(color.r(), 0);
        ASSERT_EQUAL(color.g(), 0);
        ASSERT_EQUAL(color.b(), 0);
        ASSERT_EQUAL(color.a(), 255);
    }
    {
        istringstream is {"RGB(104, 104, 101)"};
        const auto color = Colour::CreateFromString(is.str());
        ASSERT_EQUAL(color.r(), 104);
        ASSERT_EQUAL(color.g(), 104);
        ASSERT_EQUAL(color.b(), 101);
        ASSERT_EQUAL(color.a(), 255);
    }
    {
        istringstream is {"RGB(104,104,101)"};
        const auto color = Colour::CreateFromString(is.str());
        ASSERT_EQUAL(color.r(), 104);
        ASSERT_EQUAL(color.g(), 104);
        ASSERT_EQUAL(color.b(), 101);
        ASSERT_EQUAL(color.a(), 255);
    }
}

void TestColourCreationRGBThrowConstructor() {

    {
        istringstream is{"RGB(300, 200, 200)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
    }
    {
        eft::stats::Logger::GetLogger()->set_level(spdlog::level::trace);
        istringstream is{"RGB(0, 0)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
        eft::stats::Logger::GetLogger()->set_level(spdlog::level::off);
    }
}

void TestColourCreationRGBANormalConstructor()
{
    {
        istringstream is {"RGB(0, 0, 0)"};
        const auto color = Colour::CreateFromString(is.str());
        ASSERT_EQUAL(color.r(), 0);
        ASSERT_EQUAL(color.g(), 0);
        ASSERT_EQUAL(color.b(), 0);
        ASSERT_EQUAL(color.a(), 255);
    }
    {
        istringstream is {"RGB(104, 104, 104)"};
        const auto color = Colour::CreateFromString(is.str());
        ASSERT_EQUAL(color.r(), 104);
        ASSERT_EQUAL(color.g(), 104);
        ASSERT_EQUAL(color.b(), 104);
        ASSERT_EQUAL(color.a(), 255);
    }
    {
        istringstream is {"RGB(104,104,104)"};
        const auto color = Colour::CreateFromString(is.str());
        ASSERT_EQUAL(color.r(), 104);
        ASSERT_EQUAL(color.g(), 104);
        ASSERT_EQUAL(color.b(), 104);
        ASSERT_EQUAL(color.a(), 255);
    }
}

void TestColourCreationRGBAThrowConstructor() {

    {
        istringstream is{"RGBA(300, 200, 200, 300)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
    }
    {
        istringstream is{"RGBA(0, 0)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
    }
}

void TestNoRGB()
{
    {
        istringstream is{"color(300, 200, 200, 300)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
    }
    {
        istringstream is{"RG(200, 200, 200)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
    }
}

void TestNotEnoughVals()
{
    {
        istringstream is{"RGB(200, 200)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
    }
}

EFT_IMPLEMENT_TESTFILE(ColourUtils) {
        eft::stats::Logger::GetLogger()->set_level(spdlog::level::off);
        EFT_ADD_TEST(TestColourCreationRGBNormalConstructor,    "ColourUtils");
        EFT_ADD_TEST(TestColourCreationRGBThrowConstructor,     "ColourUtils");
        EFT_ADD_TEST(TestColourCreationRGBANormalConstructor,   "ColourUtils");
        EFT_ADD_TEST(TestColourCreationRGBAThrowConstructor,    "ColourUtils");
        EFT_ADD_TEST(TestNoRGB,                                 "ColourUtils");
        EFT_ADD_TEST(TestNotEnoughVals,                         "ColourUtils");
        eft::stats::Logger::GetLogger()->set_level(spdlog::level::info);
}
EFT_END_IMPLEMENT_TESTFILE(ColourUtils);
