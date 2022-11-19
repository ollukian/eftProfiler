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
        istringstream is{"RGB(0, 0)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
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

void TestNotCorrectSeparators()
{
    {
        istringstream is{"RGB(300* 200, 200)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
    }
    {
        istringstream is{"RGB(300* 200^ 200__)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
    }
    {
        istringstream is{"RGB(*300* 200 200)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
    }
    {
        istringstream is{"RGB(300 200 200 200)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
    }
    {
        istringstream is{"RGB(300, 200, 200, 200)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
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
        eft::stats::Logger::SetLevel(spdlog::level::level_enum::trace);
        istringstream is{"RGB(200, 200)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
    }
    {
        istringstream is{"RGB(200)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
    }
    {
        istringstream is{"RGB()"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
    }
    {
        istringstream is{"RGBA(200, 200, 250)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
    }
    {
        istringstream is{"RGBA(200, 200, 250,)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
    }
    {
        istringstream is{"RGBA(200, 200)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
    }
}

void TestGetters()
{
    {
        istringstream  is {"RGBA(1, 2, 3, 4)"};
        ASSERT_NO_THROW(Colour::CreateFromString(is.str()));
        const auto colour = Colour::CreateFromString(is.str());
        ASSERT_EQUAL(colour.r(), 1);
        ASSERT_EQUAL(colour.g(), 2);
        ASSERT_EQUAL(colour.b(), 3);
        ASSERT_EQUAL(colour.a(), 4);
        ASSERT_EQUAL(colour.r_as_fraction(), 1.f / 256.f);
        ASSERT_EQUAL(colour.g_as_fraction(), 2.f / 256.f);
        ASSERT_EQUAL(colour.b_as_fraction(), 3.f / 256.f);
        ASSERT_EQUAL(colour.a_as_fraction(), 4.f / 256.f);
    }
}

EFT_IMPLEMENT_TESTFILE(ColourUtils) {
        EFT_ADD_TEST(TestColourCreationRGBNormalConstructor,    "ColourUtils");
        EFT_ADD_TEST(TestColourCreationRGBThrowConstructor,     "ColourUtils");
        EFT_ADD_TEST(TestColourCreationRGBANormalConstructor,   "ColourUtils");
        EFT_ADD_TEST(TestColourCreationRGBAThrowConstructor,    "ColourUtils");
        EFT_ADD_TEST(TestNoRGB,                                 "ColourUtils");
        EFT_ADD_TEST(TestNotEnoughVals,                         "ColourUtils");
        EFT_ADD_TEST(TestNotCorrectSeparators,                  "ColourUtils");
        EFT_ADD_TEST(TestGetters,                               "ColourUtils");
}
EFT_END_IMPLEMENT_TESTFILE(ColourUtils);
