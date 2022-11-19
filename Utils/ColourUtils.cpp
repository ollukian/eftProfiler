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
    EFT_PROF_TRACE("Create colour from: {}", s);
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
    const auto components = ::eft::StringUtils::Strip(s);
    // RGB(r, g, b)
    // or
    // RGB(r g b)
    // RGB(r,g,b)

    size_t pos_rgb = s.find("RGB(");
    s.remove_prefix(pos_rgb + 4);
    EFT_PROF_DEBUG("pos_rbg = {}", pos_rgb);
    EFT_PROF_DEBUG("s after removing prefix = [{}]", s);

    istringstream ss {string(s)};
    ss.exceptions(std::stringstream::badbit);

    size_t r, g, b;
    char del1, del2;

    try {
        ss >> r >> del1;
        if (ss.bad())
            throw std::logic_error("");
        ss >> g >> del2;
        if (ss.bad())
            throw std::logic_error("");
        ss >> b;
    }
    //catch (std::logic_error& e) {
    //    EFT_PROF_CRITICAL("Problem: {} in colour parsing of: {}", e.what(), s);
    //    throw std::logic_error("colour parsing problem");
    //}
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
    EFT_PROF_TRACE("Redirect to create colour from RGB string");
    const auto components = ::eft::StringUtils::Strip(s);
    // RGB(r, g, b, a)
    // or
    // RGB(r g b a) <= not supported now
    // RGB(r,g,b,a)

    size_t pos_rgba = s.find("RGBA(");
    s.remove_prefix(pos_rgba + 5);
    EFT_PROF_DEBUG("pos_rbga = {}", pos_rgba);
    EFT_PROF_DEBUG("s after removing prefix = [{}]", s);

    istringstream ss {string(s)};

    size_t r, g, b, a;
    char del1, del2, del3;

    try {
        ss >> r >> del1;
        ss >> g >> del2;
        ss >> b >> del3;
        ss >> a;
    }
    catch (std::exception& e) {
        EFT_PROF_CRITICAL("Problem in colour parsing: {}", e.what());
        throw std::logic_error(fmt::format("colour parsing problem: {}", e.what()));
    }
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



}
