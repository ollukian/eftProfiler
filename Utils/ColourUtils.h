//
// Created by Aleksei Lukianchuk on 18-Nov-22.
//

#ifndef EFTPROFILER_COLOURUTILS_H
#define EFTPROFILER_COLOURUTILS_H

#include <string>
#include <vector>
#include <string_view>
#include <map>
#include <set>

#include <memory>

#include "TColor.h"

namespace eft::utils {


class ColourUtils;
class Colour;

class ColourUtils {
public:
    // TODO: finish as a singleton

    ColourUtils(const ColourUtils&) = delete;
    ColourUtils(ColourUtils&&) = delete;
    ColourUtils& operator = (const ColourUtils&) = delete;
    ColourUtils& operator = (ColourUtils&&) = delete;

    size_t RegisterColour(const Colour& c, const std::string& name = "");
    size_t GetColourIdx(const Colour& c) const noexcept;
    size_t GetColourIdx(const std::string& name) const noexcept;

private:
    std::set<std::unique_ptr<TColor>> registry_colours_;
    std::set<size_t> registered_colours_idx_;

    std::map<std::string, size_t> named_colours_;
private:
    ColourUtils() = default;
};


class Colour {
public:
    [[nodiscard]] inline size_t r() const noexcept { return r_;}
    [[nodiscard]] inline size_t g() const noexcept { return g_;}
    [[nodiscard]] inline size_t b() const noexcept { return b_;}
    [[nodiscard]] inline size_t a() const noexcept { return a_;}

    static Colour CreateFromString(std::string_view s);
private:
    Colour(size_t r, size_t g, size_t b, size_t a = 1.0)
        : r_(r)
        , g_(g)
        , b_(b)
        , a_(a)
    {
        if ( ! AssertRange() )
            throw std::out_of_range("");
    }

    [[nodiscard]] bool AssertRange() const noexcept;

    static Colour CreateFromStringRGB(std::string_view s);
    static Colour CreateFromStringRGBA(std::string_view s);

    //static Colour _ParseRgbAsOneWordNoSpace(std::string_view s);
private:
    size_t r_ = 255;
    size_t g_ = 255;
    size_t b_ = 255;
    size_t a_ = 255;
};


}

#endif //EFTPROFILER_COLOURUTILS_H
