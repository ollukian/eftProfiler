//
// Created by Aleksei Lukianchuk on 15-Nov-22.
//

#ifndef EFTPROFILER_STRINGUTILS_H
#define EFTPROFILER_STRINGUTILS_H

#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <string_view>

#include "Profiler.h"

namespace eft {

template <typename Iterator>
class IteratorRange;

// TODO: adapt for c++17 => string_view
    // c++14 only, so no std::string_view
class StringUtils {
public:
    //static inline std::vector<std::string> ParseText(const std::string& s) noexcept;
    static inline std::string              Strip(const std::string& s);
    static inline std::string_view         StripCopy(std::string_view s);
    static inline void                     Strip(std::string_view& s);

    static inline std::vector<std::string> Split(const std::string& s, char sep = ' ');
    static inline std::vector<std::string_view> SplitBy(std::string_view s, char sep = ' ');

    static inline void          Ltrim(std::string &s);
    static inline void          Rtrim(std::string &s);
    static inline void          Trim(std::string &s);
    static inline std::string   LtrimCopy(std::string s);
    static inline std::string   RtrimCopy(std::string s);
    static inline std::string   TrimCopy(std::string s);

    static inline void          RemovePrefix(std::string& s, const std::string& prefix);
    static inline std::string   RemovePrefixCopy(std::string s,  const std::string& prefix);
    static inline void          RemoveSuffix(std::string& s, const std::string& suffix);
    static inline std::string   RemoveSuffixCopy(std::string s,  const std::string& suffix);

    static inline void          Replace(std::string& s, const std::string& what, const std::string& with);
    static inline std::string   ReplaceCopy(std::string s, const std::string& what, const std::string& with);

    template <typename Container>
    static inline std::string    Join(char c, const Container& cont);

    template <typename Container>
    static inline std::string    Join(std::string line, const Container& cont);

    static inline void          ToLowCase(std::string& s) noexcept;
    static inline void          ToUpperCase(std::string& s) noexcept;
    static inline std::string          ToLowCaseCopy(std::string s) noexcept;
    static inline std::string          ToUpperCaseCopy(std::string s) noexcept;
};

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end)
            : first(begin)
            , last(end)
    {
    }

    Iterator begin() const {
        return first;
    }

    Iterator end() const {
        return last;
    }

private:
    Iterator first, last;
};

template <typename Collection>
auto Head(Collection& v, size_t top) {
    return IteratorRange{v.begin(), next(v.begin(), std::min(top, v.size()))};
}


inline void StringUtils::Replace(std::string& s, const std::string& what, const std::string& with) {
    EFT_PROFILE_FN();
    if (what.empty())
        return;
    auto pos_begin = s.find(what);
    if (pos_begin == std::string::npos)
        return;
    if (with.empty())
        s.erase(pos_begin, what.length());
    else
        s.replace(pos_begin, what.length(), with);
}

 inline void StringUtils::RemovePrefix(std::string& s, const std::string& prefix)
 {
     EFT_PROFILE_FN();
     Trim(s);
    auto pos_prefix = s.find(prefix);
    if ( pos_prefix != 0) {
        return;
    }
    s = s.substr(pos_prefix +prefix.length(), s.length());
 }
inline std::string StringUtils::RemovePrefixCopy(std::string s, const std::string& prefix)
{
    EFT_PROFILE_FN();
    RemovePrefix(s, prefix);
    return s;
}
inline void StringUtils::RemoveSuffix(std::string& s, const std::string& suffix) {
    // might be simple done just with inverse() and ::RemovePrefix,
    // but I wouldn't like to inverse it to prevent memory allocation
    EFT_PROFILE_FN();
    if (s.empty())
        return;
    auto pos_suffix = s.rfind(suffix);
    if (pos_suffix != s.size() - suffix.length())
        return;
    s = s.substr(0, pos_suffix);
}
inline std::string StringUtils::RemoveSuffixCopy(std::string s, const std::string& suffix) {
    EFT_PROFILE_FN();
    RemoveSuffix(s, suffix);
    return s;
}
inline std::string StringUtils::ReplaceCopy(std::string s, const std::string& what, const std::string& with) {
    EFT_PROFILE_FN();
    Replace(s, what, with);
    return s;
}
// trim from start (in place)
inline void StringUtils::Ltrim(std::string &s) {
        EFT_PROFILE_FN();
        if (s.empty())
            return;
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
inline void StringUtils::Rtrim(std::string &s) {
        EFT_PROFILE_FN();
        if (s.empty())
            return;
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
 inline void StringUtils::Trim(std::string &s) {
        EFT_PROFILE_FN();
    Rtrim(s);
    Ltrim(s);
}

// trim from start (copying)
 inline std::string StringUtils::LtrimCopy(std::string s) {
    EFT_PROFILE_FN();
    Ltrim(s);
    return s;
}

// trim from end (copying)
 inline std::string StringUtils::RtrimCopy(std::string s) {
    EFT_PROFILE_FN();
    Rtrim(s);
    return s;
}

// trim from both ends (copying)
 inline std::string StringUtils::TrimCopy(std::string s) {
    EFT_PROFILE_FN();
    Trim(s);
    return s;
}

template <typename Container>
std::string StringUtils::Join(char c, const Container& cont) {
    EFT_PROFILE_FN();
    if (cont.empty())
        return {};
    std::ostringstream os;
    for (const auto& item : Head(cont, cont.size() - 1)) {
        os << item << c;
    }
    os << *rbegin(cont);
    return os.str();
}

template <typename Container>
std::string StringUtils::Join(std::string line, const Container& cont) {
    EFT_PROFILE_FN();
    if (cont.empty())
        return {};
    std::ostringstream os;
    for (const auto& item : Head(cont, cont.size() - 1)) {
        os << item << line;
    }
    os << *rbegin(cont);
    return os.str();
}



std::string StringUtils::Strip(const std::string& s) {
    EFT_PROFILE_FN();
    if (s.empty())
        return {};
    size_t idx_first_not_space = 0;
    size_t idx_last_not_space  = s.size() - 1;
    size_t idx_max = s.length() - 1;
    while (idx_first_not_space < idx_max && isspace(s.at(idx_first_not_space))) {
       idx_first_not_space++;
    }
    while (idx_last_not_space > 0 && isspace(s.at(idx_last_not_space))) {
        idx_last_not_space--;
    }
    if (idx_last_not_space != idx_first_not_space)
        return s.substr(idx_first_not_space, idx_last_not_space - idx_first_not_space + 1);
    else
        return "";
}

std::string_view StringUtils::StripCopy(std::string_view s) {
    EFT_PROFILE_FN();
    if (s.empty())
        return {};
    Strip(s);
    return s;
}
void StringUtils::Strip(std::string_view& s) {
    EFT_PROFILE_FN();
    while (!s.empty() && isspace(s.front())) {
        s.remove_prefix(1);
    }
    while (!s.empty() && isspace(s.back())) {
        s.remove_suffix(1);
    }
}

std::vector<std::string_view> StringUtils::SplitBy(std::string_view s, char sep) {
    if (s.empty())
        return {};
    EFT_PROFILE_FN();
    std::vector<std::string_view> result;
    while (!s.empty()) {
        size_t pos = s.find(sep);
        result.push_back(s.substr(0, pos));
        s.remove_prefix(pos != std::string_view::npos
            ? pos + 1
            : s.size()
            );
    }
    return result;
}

std::vector<std::string> StringUtils::Split(const std::string& s, char sep) {
    EFT_PROFILE_FN();
    if (s.empty())
        return {};
    std::vector<std::string> result;
    std::string string_copy = s;
    while (!string_copy.empty()) {
        size_t pos = string_copy.find(sep);
        result.push_back(string_copy.substr(0, pos));
        string_copy = string_copy.substr(pos != std::string::npos ? pos + 1 : string_copy.size(), string_copy.length());
        //s.remove_prefix(pos != s.npos ? pos + 1 : s.size());
    }
    return result;
}

inline void StringUtils::ToLowCase(std::string& s) noexcept {
    EFT_PROFILE_FN();
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}
inline void StringUtils::ToUpperCase(std::string& s) noexcept {
    EFT_PROFILE_FN();
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
}
inline std::string StringUtils::ToLowCaseCopy(std::string s) noexcept {
    EFT_PROFILE_FN();
    ToUpperCase(s);
    return s;
}
inline std::string StringUtils::ToUpperCaseCopy(std::string s) noexcept {
    EFT_PROFILE_FN();
    ToLowCase(s);
    return s;
}

} // eft

#endif //EFTPROFILER_STRINGUTILS_H
