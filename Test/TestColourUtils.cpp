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
    {
        istringstream is{"RGBA(*, 0)"};
        ASSERT_THROW(Colour::CreateFromString(is.str()), std::logic_error);
    }
    {
        istringstream is{"RGBA(* * * * *)"};
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

void TestColourCtorFromInts()
{
    {
        Colour c(1, 2, 3, 4);
        ASSERT_EQUAL(c.r(), 1);
        ASSERT_EQUAL(c.g(), 2);
        ASSERT_EQUAL(c.b(), 3);
        ASSERT_EQUAL(c.a(), 4);
    }
    {
        Colour c(1, 2, 3);
        ASSERT_EQUAL(c.r(), 1);
        ASSERT_EQUAL(c.g(), 2);
        ASSERT_EQUAL(c.b(), 3);
        ASSERT_EQUAL(c.a(), 255);
    }
    {
        ASSERT_NO_THROW(Colour(255, 255, 255, 255));

        ASSERT_THROW(Colour(256, 0, 0, 0), std::logic_error);
        ASSERT_THROW(Colour(0, 256, 0, 0), std::logic_error);
        ASSERT_THROW(Colour(0, 0, 256, 0), std::logic_error);
        ASSERT_THROW(Colour(0, 0, 0, 256), std::logic_error);
        ASSERT_THROW(Colour(256, 256, 256, 256), std::logic_error);
        ASSERT_THROW(Colour(-2, 0, 0, 0), std::logic_error);
        ASSERT_THROW(Colour(0, -2, 0, 0), std::logic_error);
        ASSERT_THROW(Colour(0, 0, -2, 0), std::logic_error);
        ASSERT_THROW(Colour(0, 0, 0, -2), std::logic_error);
    }
}

void TestRegisterColourBasic()
{
    {
        ColourUtils::ClearRegistry();
        Colour c(1, 2, 3, 4);
        ASSERT_NO_THROW( ColourUtils::RegisterColour(c) );
    }
    {
        ColourUtils::ClearRegistry();
        Colour c(1, 2, 3, 4);
        auto idx_color = ColourUtils::RegisterColour(c);
        const auto reg = ColourUtils::GetRegistryColourIdx();
        ASSERT(reg.find(idx_color) != reg.end() );
    }
    {
        ColourUtils::ClearRegistry();
        Colour c(1, 2, 3, 4);
        auto idx_color = ColourUtils::RegisterColour(c);
        ASSERT_EQUAL(ColourUtils::GetColourByIdx(idx_color), c);
    }
    {
        ColourUtils::ClearRegistry();
        Colour c1(1, 2, 3, 4);
        Colour c2(10, 20, 30, 40);
        auto idx_color_1 = ColourUtils::RegisterColour(c1);
        auto idx_color_2 = ColourUtils::RegisterColour(c2);
        ASSERT_NOT_EQUAL(idx_color_1, idx_color_2);
        ASSERT_NO_THROW(ColourUtils::GetColourByIdx(idx_color_1));
        ASSERT_NO_THROW(ColourUtils::GetColourByIdx(idx_color_2));
    }
    {
        ColourUtils::ClearRegistry();
        eft::stats::Logger::SetLevel(spdlog::level::level_enum::trace);
        Colour c1(1, 2, 3, 4);
        Colour c2(10, 20, 30, 40);
        auto idx_color_1 = ColourUtils::RegisterColour(c1);
        auto idx_color_2 = ColourUtils::RegisterColour(c2);
        ASSERT_NOT_EQUAL(idx_color_1, idx_color_2);
        ASSERT_EQUAL(idx_color_1, ColourUtils::GetColourIdx(c1));
        ASSERT_EQUAL(idx_color_2, ColourUtils::GetColourIdx(c2));
    }
}

void TestRegisterColourAlreadyPresent(){
    {
        eft::stats::Logger::SetLevel(spdlog::level::level_enum::trace);
        Colour c1(1, 2, 3, 4);
        Colour c2(1, 2, 3, 4);
        ColourUtils::ClearRegistry();
        ColourUtils::RegisterColour(c1);
        ASSERT_THROW(ColourUtils::RegisterColour(c2), std::logic_error);
    }
    {
        Colour c1(1, 2, 3 );
        Colour c2(1, 2, 3, 255);
        ColourUtils::ClearRegistry();
        ColourUtils::RegisterColour(c1);
        ASSERT_THROW(ColourUtils::RegisterColour(c2), std::logic_error);
    }
    {
        ColourUtils::ClearRegistry();
        Colour c1;
        Colour c2;
        ColourUtils::RegisterColour(c1);
        ASSERT_THROW(ColourUtils::RegisterColour(c2), std::logic_error);
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
        EFT_ADD_TEST(TestColourCtorFromInts,                    "ColourUtils");
        EFT_ADD_TEST(TestRegisterColourBasic,                   "ColourUtils");
        EFT_ADD_TEST(TestRegisterColourAlreadyPresent,          "ColourUtils");
}
EFT_END_IMPLEMENT_TESTFILE(ColourUtils);
