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
        EFT_PROF_WARN("Colour: {} alreagdy registered", c);
        throw std::logic_error("");
    }

    size_t new_idx = TColor::GetFreeColorIndex();
    unique_ptr<TColor> colour = make_unique<TColor>(new_idx,
                                                    c.r_as_fraction(),
                                                    c.g_as_fraction(),
                                                    c.b_as_fraction(),
                                                    name.c_str(),
                                                    c.a_as_fraction());

    EFT_PROF_INFO("Registry new colour: ({:3}{:3}{:3}{:3}) with idx: {}",
                  c.r(),
                  c.g(),
                  c.b(),
                  c.a(),
                  new_idx);
    registered_colours_rgba_.insert(c);
    registry_colours_.insert(std::move(colour));
    registered_colours_idx_.insert(new_idx);
    idx_of_colour_[c] = new_idx;
    colour_of_idx_[new_idx] = c;
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
    if (idx_of_colour_.find(c) != idx_of_colour_.end())
        return idx_of_colour_.at(c);
    EFT_PROF_WARN("Colour: {}{}{}{} is not registered, register it first",
                  c.r(),
                  c.g(),
                  c.b(),
                  c.a());
    return RegisterColour(c);
}

Colour& ColourUtils::GetColourByIdx(size_t idx)  {
    if (registered_colours_idx_.find(idx) != registered_colours_idx_.end())
        return colour_of_idx_.at(idx);
    throw std::logic_error(fmt::format("colour idx: {} is not registered", idx));
}

ostream& operator << (ostream& os, const Colour& c) noexcept {
    return os << fmt::format("RGBA({:3}, {:3}, {:3}, {:3})", c.r(), c.g(), c.b(), c.a()) << endl;
}


}
