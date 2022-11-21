//
// Created by Aleksei Lukianchuk on 19-Nov-22.
//

#include "Tester.h"
#include "../Utils/ColourUtils.h"
#include "../Core/Logger.h"
#include <random>

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

void TestColourOperators()
{
    {
        ColourUtils::ClearRegistry();
        Colour c1(1, 2, 3, 4);
        Colour c2(1, 2, 3, 4);
        ASSERT_EQUAL( c1, c2 );
    }
    {
        ColourUtils::ClearRegistry();
        Colour c1(1, 2, 3, 4);
        Colour c2(2, 2, 3, 4);
        ASSERT_NOT_EQUAL(c1, c2);
        ASSERT(c1 < c2);
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

void TestROOTcoloursRecognition()
{
    {
        // presence of basic colours from RTypes
        ColourUtils::ClearRegistry();
        ASSERT_NO_THROW(ColourUtils::CheckIfROOTcolour("kBlue"));
        ASSERT(ColourUtils::CheckIfROOTcolour("kBlue").has_value());
        size_t idx = ColourUtils::CheckIfROOTcolour("kBlue").value();
        ASSERT_EQUAL(idx, EColor::kBlue);
    }
    {
        // presence of other colours
        ColourUtils::ClearRegistry();
        ASSERT(ColourUtils::CheckIfROOTcolour("kBlue").has_value());
        ASSERT(ColourUtils::CheckIfROOTcolour("kRed").has_value());
        ASSERT(ColourUtils::CheckIfROOTcolour("kDeepSea").has_value());


        size_t idx_blue     = ColourUtils::CheckIfROOTcolour("kBlue").value();
        size_t idx_red      = ColourUtils::CheckIfROOTcolour("kRed").value();
        size_t idx_deep_sea = ColourUtils::CheckIfROOTcolour("kDeepSea").value();


        ASSERT_EQUAL(idx_blue, EColor::kBlue);
        ASSERT_EQUAL(idx_red, EColor::kRed);
        ASSERT_EQUAL(idx_deep_sea, EColorPalette::kDeepSea);
    }
}

void TestROOTcoloursParseSpaces()
{
    {
        ColourUtils::ClearRegistry();
        string_view s_red {"  kRed"};
        ASSERT_NO_THROW(ColourUtils::CheckIfROOTcolour(s_red));
        ASSERT(ColourUtils::CheckIfROOTcolour(s_red).has_value());
        size_t idx = ColourUtils::CheckIfROOTcolour(s_red).value();
        ASSERT_EQUAL(idx, EColor::kRed);
    }
    {
        ColourUtils::ClearRegistry();
        string_view s_red {"  kRed "};
        ASSERT_NO_THROW(ColourUtils::CheckIfROOTcolour(s_red));
        ASSERT(ColourUtils::CheckIfROOTcolour(s_red).has_value());
        size_t idx = ColourUtils::CheckIfROOTcolour(s_red).value();
        ASSERT_EQUAL(idx, EColor::kRed);
    }
}

void TestROOTcoloursTryNotExistent()
{

    {
        ColourUtils::ClearRegistry();
        string_view s_not_existent {"kFakeColourNotInROOTforSure"};
        ASSERT_NO_THROW( ColourUtils::CheckIfROOTcolour(s_not_existent) );
        ASSERT_NOT(ColourUtils::CheckIfROOTcolour(s_not_existent).has_value());
    }
    {
        ColourUtils::ClearRegistry();
        string_view s_not_existent {"nameNotStartingFromK"};
        ASSERT_NO_THROW( ColourUtils::CheckIfROOTcolour(s_not_existent) );
        ASSERT_NOT(ColourUtils::CheckIfROOTcolour(s_not_existent).has_value());
    }
}

void TestROOTcolourFromRegisterFromString()
{
    {
        ColourUtils::ClearRegistry();
        string_view s_red {"  kRed"};
        ASSERT_NO_THROW(ColourUtils::GetColourFromString(s_red));
        size_t idx = ColourUtils::GetColourFromString(s_red);
        ASSERT_EQUAL(idx, EColor::kRed);
    }
    {
        ColourUtils::ClearRegistry();
        string_view s_red {"  kDeepSea"};
        ASSERT_NO_THROW(ColourUtils::GetColourFromString(s_red));
        size_t idx = ColourUtils::GetColourFromString(s_red);
        ASSERT_EQUAL(idx, EColorPalette::kDeepSea);
    }
}

void TestColourRandomRegistering() {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<std::mt19937::result_type> dist256(0, 255);

    constexpr static size_t NB_TESTS = 1E4;

    ColourUtils::ClearRegistry();
    for (size_t idx {0}; idx < NB_TESTS; ++idx) {
        ColourUtils::RegisterColourFromString(fmt::format("RGB({}, {}, {})", dist256(rng), dist256(rng), dist256(rng)));
    }
    for (size_t idx {0}; idx < NB_TESTS; ++idx) {
        ColourUtils::RegisterColourFromString(fmt::format("RGBA({}, {}, {})", dist256(rng), dist256(rng), dist256(rng)));
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
        EFT_ADD_TEST(TestColourOperators,                       "ColourUtils");
        EFT_ADD_TEST(TestRegisterColourBasic,                   "ColourUtils");
        EFT_ADD_TEST(TestRegisterColourAlreadyPresent,          "ColourUtils");
        EFT_ADD_TEST(TestROOTcoloursRecognition,                "ColourUtils");
        EFT_ADD_TEST(TestROOTcoloursParseSpaces,                "ColourUtils");
        EFT_ADD_TEST(TestROOTcoloursTryNotExistent,             "ColourUtils");
        EFT_ADD_TEST(TestROOTcolourFromRegisterFromString,      "ColourUtils");
        EFT_ADD_TEST(TestColourRandomRegistering,               "ColourUtils");
}
EFT_END_IMPLEMENT_TESTFILE(ColourUtils);
