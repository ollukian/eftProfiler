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
#include <iostream>

#include "TColor.h"

#include "../Core/Logger.h"

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

    static inline void ClearRegistry() noexcept;

    static size_t RegisterColourFromString(std::string_view s);
    static size_t RegisterColour(const Colour& c, const std::string& name = "");
    static size_t GetColourIdx(const Colour& c) noexcept;

    static inline std::set<size_t>& GetRegistryColourIdx() noexcept     { return registered_colours_idx_; }
    static inline std::map<Colour, size_t>& GetMapColourIdx() noexcept  { return idx_of_colour_; }
    static Colour& GetColourByIdx(size_t idx);
    //Colour& GetColourByIdx(size_t idx);
    //size_t GetColourIdx(const std::string& name)    const noexcept;

private:
    static inline std::set<std::unique_ptr<TColor>>   registry_colours_;
    static inline std::set<size_t>                    registered_colours_idx_;
    static inline std::set<Colour>                    registered_colours_rgba_;
    static inline std::map<Colour, size_t>            idx_of_colour_;
    static inline std::map<size_t, Colour>            colour_of_idx_;

    static inline std::map<std::string, size_t> named_colours_;
private:
    ColourUtils() = default;
};


class Colour {
public:
    Colour(size_t r, size_t g, size_t b, size_t a = 255)
            : r_(r)
            , g_(g)
            , b_(b)
            , a_(a)
    {
        if ( ! AssertRange() )
            throw std::out_of_range("");
    }

    Colour() = default;
    Colour(const Colour&) = default;
    Colour(Colour&&) = default;

    Colour& operator=(const Colour&) = default;
    Colour& operator=(Colour&&) = default;


    [[nodiscard]] inline size_t r() const noexcept { return r_;}
    [[nodiscard]] inline size_t g() const noexcept { return g_;}
    [[nodiscard]] inline size_t b() const noexcept { return b_;}
    [[nodiscard]] inline size_t a() const noexcept { return a_;}

    [[nodiscard]] inline float r_as_fraction() const noexcept { return static_cast<float>(r_) / 256.f; }
    [[nodiscard]] inline float g_as_fraction() const noexcept { return static_cast<float>(g_) / 256.f; }
    [[nodiscard]] inline float b_as_fraction() const noexcept { return static_cast<float>(b_) / 256.f; }
    [[nodiscard]] inline float a_as_fraction() const noexcept { return static_cast<float>(a_) / 256.f; }

    // todo: rewrite in terms of io operators
    //friend std::ostream& operator >> (std::ostream& os, Colour& c);

    static Colour CreateFromString(std::string_view s);
private:
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

inline bool operator <  (const Colour& l, const Colour& r) noexcept;
inline bool operator <= (const Colour& l, const Colour& r) noexcept;
inline bool operator == (const Colour& l, const Colour& r) noexcept;
inline bool operator != (const Colour& l, const Colour& r) noexcept;
inline bool operator >  (const Colour& l, const Colour& r) noexcept;
inline bool operator >= (const Colour& l, const Colour& r) noexcept;

std::ostream& operator << (std::ostream& os, const Colour& c) noexcept;

inline void ColourUtils::ClearRegistry() noexcept {
    registry_colours_.clear();
    registered_colours_idx_.clear();
    registered_colours_rgba_.clear();
    idx_of_colour_.clear();
    colour_of_idx_.clear();
    named_colours_.clear();
}

inline bool operator < (const Colour& l, const Colour& r) noexcept
{
    if (l.r() < r.r())
        return true;
    if (l.r() > r.r())
        return false;

    if (l.g() < r.g())
        return true;
    if (l.g() > r.g())
        return false;

    if (l.b() < r.b())
        return true;
    if (l.b() > r.b())
        return false;

    if (l.a() < r.a())
        return true;
    if (l.a() > r.a())
        return false;

    return true;
}

bool operator==(const Colour& l, const Colour& r)  noexcept {
    bool res = (l.r() == r.r()
                && l.g() == r.g()
                && l.b() == r.b()
                && l.a() == r.a());
    EFT_PROF_INFO("Colour::operator== compare {} & {} ==> {}", l, r, res);
    return (l.r() == r.r()
            && l.g() == r.g()
            && l.b() == r.b()
            && l.a() == r.a());
}

bool operator!=(const Colour& l, const Colour& r)  noexcept {
    bool res = !(l == r);
    EFT_PROF_INFO("Colour::operator!= compare {} & {} ==> {}", l, r, res);
    return ! (l == r);
}

inline bool operator > (const Colour& l, const Colour& r)  noexcept
{
    if (l.r() > r.r())
        return true;
    if (l.r() < r.r())
        return false;

    if (l.g() > r.g())
        return true;
    if (l.g() < r.g())
        return false;

    if (l.b() > r.b())
        return true;
    if (l.b() < r.b())
        return false;

    if (l.a() > r.a())
        return true;
    if (l.a() < r.a())
        return false;

    return true;
}

inline bool  operator <= (const Colour& l, const Colour& r)  noexcept
{
    bool res = !(l > r);
    EFT_PROF_INFO("Colour::operator<= compare {} & {} ==> {}", l, r, res);
    return ! (l > r);
}

inline bool  operator >= (const Colour& l, const Colour& r)  noexcept
{
    bool res = !(l < r);
    EFT_PROF_INFO("Colour::operator>= compare {} & {} ==> {}", l, r, res);
    return ! (l < r);
}



}

#endif //EFTPROFILER_COLOURUTILS_H
