//
// Created by Aleksei Lukianchuk on 18-Nov-22.
//

#include <sstream>

#include "ColourUtils.h"
#include "../Core/Logger.h"
#include "StringUtils.h"

using namespace std;

namespace eft::utils {

Colour Colour::CreateFromString(std::string_view s) {
    EFT_PROF_INFO("Create colour from: {}", s);
    if (s.find("RGBA") != std::string_view::npos)
        return CreateFromStringRGBA(s);
    else if (s.find("RGB") != std::string_view::npos)
        return CreateFromStringRGB(s);
    else {
        EFT_PROF_ERROR("Cannot create colour from: {} - not in the formats: {} | {} | {} | {}",
                       s,
                       "RGBA(r, g, b, a)",
                       "RGB(r, g, b)",
                       "RGBA(r,g,b,a)",
                       "RGB(r,g,b)");
        throw std::logic_error(fmt::format("not correct format of colour. See Error above"));
    }
}

Colour Colour::CreateFromStringRGB(std::string_view s)
{
    EFT_PROF_TRACE("Redirect to create colour from RGB string");
    const auto components = ::eft::StringUtils::StripCopy(s);
    // RGB(r, g, b)
    // or
    // RGB(r g b)
    // RGB(r,g,b)

    size_t pos_rgb = s.find("RGB(");
    s.remove_prefix(pos_rgb + 4);

    istringstream ss {string(s)};
    //ss.exceptions(std::stringstream::badbit);

    size_t r, g, b;

    try {
        bool is_stream_ok = true;
        is_stream_ok = is_stream_ok && (ss >> r);
        if ( ! is_stream_ok ) {
            throw std::logic_error(fmt::format("Cannot parse r-component of RGB in: {}", s));
        }
        is_stream_ok = is_stream_ok && (ss.peek() == ',' || ss.peek() == ' ');
        ss.ignore(1);

        is_stream_ok = is_stream_ok && (ss >> g);
        if ( ! is_stream_ok ) {
            throw std::logic_error(fmt::format("Cannot parse g-component of RGB in: {}", s));
        }
        is_stream_ok = is_stream_ok && (ss.peek() == ',' || ss.peek() == ' ');
        ss.ignore(1);

        is_stream_ok = is_stream_ok && (ss >> b);
        if ( ! is_stream_ok ) {
            throw std::logic_error(fmt::format("Cannot parse b-component of RGB in: {}", s));
        }
    }
    catch (std::exception& e) {
        EFT_PROF_CRITICAL("Problem in colour parsing: {}", e.what());
        throw std::logic_error(fmt::format("colour parsing problem: {}", e.what()));
    }
    catch (...) {
        EFT_PROF_CRITICAL("caught unknown exception in CreateFromStringRGB for: {}", s);
        throw std::logic_error(fmt::format("colour parsing problem of: {}", s));
    }
    return {r, g, b};
}

Colour Colour::CreateFromStringRGBA(std::string_view s)
{
    EFT_PROF_TRACE("Redirect to create colour from RGBA string");
    const auto components = ::eft::StringUtils::StripCopy(s);
    // RGB(r, g, b, a)
    // or
    // RGB(r g b a) <= not supported now
    // RGB(r,g,b,a)

    size_t pos_rgba = s.find("RGBA(");
    s.remove_prefix(pos_rgba + 5);

    istringstream ss {string(s)};

    size_t r, g, b, a;
    char del1, del2, del3;

    try {
        bool is_stream_ok = true;

        is_stream_ok = is_stream_ok && (ss >> r);
        if ( ! is_stream_ok ) {
            throw std::logic_error(fmt::format("Cannot parse r-component of RGBA in: {}", s));
        }
        is_stream_ok = is_stream_ok && (ss.peek() == ',' || ss.peek() == ' ');
        ss.ignore(1);

        is_stream_ok = is_stream_ok && (ss >> g);
        if ( ! is_stream_ok ) {
            throw std::logic_error(fmt::format("Cannot parse g-component of RGBA in: {}", s));
        }
        is_stream_ok = is_stream_ok && (ss.peek() == ',' || ss.peek() == ' ');
        ss.ignore(1);

        is_stream_ok = is_stream_ok && (ss >> b);
        if ( ! is_stream_ok ) {
            throw std::logic_error(fmt::format("Cannot parse b-component of RGBA in: {}", s));
        }
        is_stream_ok = is_stream_ok && (ss.peek() == ',' || ss.peek() == ' ');
        ss.ignore(1);

        is_stream_ok = is_stream_ok && (ss >> a);
        if ( ! is_stream_ok ) {
            throw std::logic_error(fmt::format("Cannot parse a-component of RGBA in: {}", s));
        }
    }
    catch (std::logic_error& e) {
        throw std::logic_error(e.what());
    }
    catch (...) {
        EFT_PROF_CRITICAL("Unknown exception caught");
        throw std::runtime_error("Unknown exception caught");
    }
    //catch (std::exception& e) {
    //EFT_PROF_CRITICAL("Problem in colour parsing: {}", e.what());
    //    throw std::logic_error(fmt::format("colour parsing problem: {}", e.what()));
    //}
    return {r, g, b, a};
}

bool Colour::AssertRange() const noexcept
{
    if (r_ > 255)
    {
        EFT_PROF_CRITICAL("Colour::AssertRange r must be <= 255");
        return false;
    }
    if (g_ > 255)
    {
        EFT_PROF_CRITICAL("Colour::AssertRange g must be <= 255");
        return false;
    }
    if (b_ > 255)
    {
        EFT_PROF_CRITICAL("Colour::AssertRange b must be <= 255");
        return false;
    }
    if (a_ > 255)
    {
        EFT_PROF_CRITICAL("Colour::AssertRange a must be <= 255");
        return false;
    }
    return true;
}

//Colour Colour::_ParseRgbAsOneWordNoSpace(std::string_view s)
//{
//    EFT_PROF_TRACE("Parse {} as one word with no spaces", s);
//
//    size_t pos_rgb = s.find("RGB(");
//    s.remove_prefix(pos_rgb + 4);
//    EFT_PROF_DEBUG("pos_rbg = {}", pos_rgb);
//    EFT_PROF_DEBUG("s after removing prefix = [{}]", s);
//
//    istringstream ss {string(s)};
//
//    float r, g, b;
//    char del1, del2;
//
//    try {
//        ss >> r >> del1;
//        ss >> g >> del2;
//        ss >> b;
//    }
//    catch (std::exception& e) {
//        EFT_PROF_CRITICAL("Problem in colour parsing: {}", e.what());
//    }
//    return {r, g, b};
//}



size_t ColourUtils::RegisterColour(const Colour& c, const string& name) {
    if (registered_colours_rgba_.find(c) != registered_colours_rgba_.end()) {
        EFT_PROF_WARN("Colour: {} already registered", c);
        throw std::logic_error("");
    }

    //EFT_PROF_DEBUG("ColourUtils: registered colours:");
    //for (const auto& colour : registered_colours_rgba_) {
    //    EFT_PROF_DEBUG(colour);
    //}

    size_t new_idx = TColor::GetFreeColorIndex();
    unique_ptr<TColor> colour = make_unique<TColor>(new_idx,
                                                    c.r_as_fraction(),
                                                    c.g_as_fraction(),
                                                    c.b_as_fraction(),
                                                    name.c_str(),
                                                    c.a_as_fraction());

    EFT_PROF_INFO("Registered new colour: ({}) with idx: {}",
                  c,
                  new_idx);
    registered_colours_rgba_.insert(c);
    registry_colours_.insert(std::move(colour));
    registered_colours_idx_.insert(new_idx);
    idx_of_colour_[c] = new_idx;
    colour_of_idx_[new_idx] = c;

    //EFT_PROF_DEBUG("ColourUtils: after registering {}, registered colours:", c);
    //for (const auto& reg_colour : registered_colours_rgba_) {
    //    EFT_PROF_DEBUG(reg_colour);
    //}
    return new_idx;
}

size_t ColourUtils::RegisterColourFromString(std::string_view s) {
    try {
        auto colour = Colour::CreateFromString(s);
        return RegisterColour(colour);
    } catch (std::logic_error& e) {
        EFT_PROF_ERROR("Cannot register colour from string: {} due to {}",
                       s,
                       e.what());
        throw std::logic_error(e);
    }
}

size_t ColourUtils::GetColourIdx(const Colour& c) noexcept {
    EFT_PROF_TRACE("GetColourIdx: {}", c);
    if (idx_of_colour_.find(c) != idx_of_colour_.end())
        return idx_of_colour_.at(c);
    EFT_PROF_WARN("Colour: {} is not registered, register it first",
                  c);
    return RegisterColour(c);
}

Colour& ColourUtils::GetColourByIdx(size_t idx)  {
    if (registered_colours_idx_.find(idx) != registered_colours_idx_.end())
        return colour_of_idx_.at(idx);
    throw std::logic_error(fmt::format("colour idx: {} is not registered", idx));
}

ostream& operator << (ostream& os, const Colour& c) noexcept {
    return os << fmt::format("RGBA({:3}, {:3}, {:3}, {:3})", c.r(), c.g(), c.b(), c.a());
}


std::optional<size_t> ColourUtils::CheckIfROOTcolour(std::string_view s) noexcept {

    EFT_PROF_TRACE("Check if [{}] is a ROOT colour", s);

    StringUtils::Strip(s);
    if (s.empty())
        return {};
    if (s[0] != 'k') {
        EFT_PROF_TRACE("[{}] doesn't start with {}, but with: {}", s, "k", s[0]);
        return {};
    }

    // taken from ROOT 6.24.01 on 21Nov2022
    // TColor.h
    // RTypes.h
    static const map<string_view, size_t> eft_ROOT_ColourNames{
            {"kWhite",                  0},
            {"kBlack",                  1},
            {"kGray",                   920},
            {"kRed",                    632},
            {"kGreen",                  416},
            {"kBlue",                   600},
            {"kYellow",                 400},
            {"kMagenta",                616},
            {"kCyan",                   432},
            {"kOrange",                 800},
            {"kSpring",                 820},
            {"kTeal",                   840},
            {"kAzure",                  860},
            {"kViolet",                 880},
            {"kPink",                   900},
            {"kDeepSea",                  51},
            {"kGreyScale",                52},
            {"kDarkBodyRadiator",         53},
            {"kBlueYellow",               54},
            {"kRainBow",                  55},
            {"kInvertedDarkBodyRadiator", 56},
            {"kBird",                     57},
            {"kCubehelix",                58},
            {"kGreenRedViolet",           59},
            {"kBlueRedYellow",            60},
            {"kOcean",                    61},
            {"kColorPrintableOnGrey",     62},
            {"kAlpine",                   63},
            {"kAquamarine",               64},
            {"kArmy",                     65},
            {"kAtlantic",                 66},
            {"kAurora",                   67},
            {"kAvocado",                  68},
            {"kBeach",                    69},
            {"kBlackBody",                70},
            {"kBlueGreenYellow",          71},
            {"kBrownCyan",                72},
            {"kCMYK",                     73},
            {"kCandy",                    74},
            {"kCherry",                   75},
            {"kCoffee",                   76},
            {"kDarkRainBow",              77},
            {"kDarkTerrain",              78},
            {"kFall",                     79},
            {"kFruitPunch",               80},
            {"kFuchsia",                  81},
            {"kGreyYellow",               82},
            {"kGreenBrownTerrain",        83},
            {"kGreenPink",                84},
            {"kIsland",                   85},
            {"kLake",                     86},
            {"kLightTemperature",         87},
            {"kLightTerrain",             88},
            {"kMint",                     89},
            {"kNeon",                     90},
            {"kPastel",                   91},
            {"kPearl",                    92},
            {"kPigeon",                   93},
            {"kPlum",                     94},
            {"kRedBlue",                  95},
            {"kRose",                     96},
            {"kRust",                     97},
            {"kSandyTerrain",             98},
            {"kSienna",                   99},
            {"kSolar",                    100},
            {"kSouthWest",                101},
            {"kStarryNight",              102},
            {"kSunset",                   103},
            {"kTemperatureMap",           104},
            {"kThermometer",              105},
            {"kValentine",                106},
            {"kVisibleSpectrum",          107},
            {"kWaterMelon",               108},
            {"kCool",                     109},
            {"kCopper",                   110},
            {"kGistEarth",                111},
            {"kViridis",                  112},
            {"kCividis",                  113}
    };

    if (eft_ROOT_ColourNames.find(s) != eft_ROOT_ColourNames.end()) {
        size_t res = eft_ROOT_ColourNames.at(s);
        EFT_PROF_TRACE("{} is present in the ROOT pallette with idx: {}", s, res);
        return res;
    }
    EFT_PROF_CRITICAL("{} starts with [k] but is not present in the palette");
    EFT_PROF_CRITICAL("If you are sure, it should be, check eft_ROOT_ColourNames in Utils/ColourUtils.cpp at line ~{}", __LINE__);
    return {};
}

size_t ColourUtils::GetColourFromString(std::string_view s) noexcept {
    EFT_PROF_TRACE("Get colour from [{}]", s);

    if (auto res = CheckIfROOTcolour(s); res.has_value()) {
        EFT_PROF_DEBUG("[{}] is a ROOT colour: [{}]", s, res.value());
        return res.value();
    }

    auto colour = Colour::CreateFromString(s);
    if (idx_of_colour_.find(colour) != idx_of_colour_.end()) {
        auto idx = idx_of_colour_.at(colour);
        EFT_PROF_TRACE("Colour {} is already present, return its idx: {}", colour, idx);
        return idx;
    }
    return RegisterColour(colour);
}


}
