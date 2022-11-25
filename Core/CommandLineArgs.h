//
// Created by Aleksei Lukianchuk on 15-Sep-22.
//

#ifndef EFTPROFILER_COMMANDLINEARGS_H
#define EFTPROFILER_COMMANDLINEARGS_H

#include <string>
#include <map>
#include <vector>
#include <optional>
#include <set>

#include "../Core/Logger.h"

// Wrapper around command line arguments

class CommandLineArgs {
public:
    using Key = std::string;        // -k filename.root
    using Val = std::string;        //  ^     ^----- val
    using Vals = std::vector<Val>;  //  |----------- key


    using Keys = std::set<Key>;
    //using option = std::string;

    CommandLineArgs() = delete;
    CommandLineArgs(const CommandLineArgs&) = delete;
    CommandLineArgs(CommandLineArgs&&)      = delete;
    CommandLineArgs& operator = (const CommandLineArgs&) = delete;
    CommandLineArgs& operator = (CommandLineArgs&&)      = delete;

    CommandLineArgs(int argc, char* argv[]);
    //explicit CommandLineArgs(std::istringstream& is);
    //explicit CommandLineArgs(const std::vector<std::string>& args);

    [[nodiscard]]
    std::optional<Vals> GetVals(const Key& option) const;
    [[nodiscard]]
    std::optional<Val> GetVal(const Key& option) const;

    [[nodiscard]]
    inline bool HasKey(const Key& key) const noexcept;

    template<typename T>
    bool SetValIfArgExists(const std::string& key, T& val);

    [[nodiscard]]
    const auto& GetOps() const noexcept { return ops; }
    [[nodiscard]]
    const auto& GetKeys() const noexcept { return keys;}

    void ReportStatus() const noexcept;
private:
    std::map<Key, Vals> ops;
    mutable std::set<Key> _requested_keys; // to track down that all keys have been asked for
    mutable std::set<Key> _parsed_keys;   // to track down that all keys have been asked for
    Keys keys;
private:
    bool ParseInput(int argc, char* argv[]);
    [[nodiscard]]
    static inline Key TrimKey(const Key& key) noexcept;
    void AddKey(Key key);
    //static std::pair<Key, Vals> ExtractVals(std::string_view raw) noexcept;
};

// https://stackoverflow.com/questions/41853159/how-to-detect-if-a-type-is-shared-ptr-at-compile-time
template<typename T, typename Enable = void>
struct is_smart_pointer
{
    enum { value = false };
};

template<typename T>
struct is_smart_pointer<T, typename std::enable_if<std::is_same<typename std::remove_cv<T>::type, std::shared_ptr<typename T::element_type>>::value>::type>
{
    enum { value = true };
};

template<typename T>
struct is_smart_pointer<T, typename std::enable_if<std::is_same<typename std::remove_cv<T>::type, std::unique_ptr<typename T::element_type>>::value>::type>
{
    enum { value = true };
};

template<typename T>
struct is_smart_pointer<T, typename std::enable_if<std::is_same<typename std::remove_cv<T>::type, std::weak_ptr<typename T::element_type>>::value>::type>
{
    enum { value = true };
};

//#template <class>
//inline constexpr bool is_smart_pointer_v = false;

//template <class _Ty, class... _Types>
//inline constexpr bool _Is_any_of_v = // true if and only if _Ty is in _Types
//        disjunction_v<std::is_same<_Ty, _Types>...>;

//template <class T>
//inline constexpr bool is_smart_pointer_v = _Is_any_of_v<std::shared_ptr<T>, std::unique_ptr<T>, std::weak_ptr<T>>;

//template <class T>
//inline constexpr bool is_smart_pointer_v = false;
//
//template <class T>
//inline constexpr bool is_smart_pointer_v<is_smart_pointer<T>::value> = true;

template<typename T>
bool CommandLineArgs::SetValIfArgExists(const std::string& key, T& val)
{
    EFT_PROF_DEBUG("[CommandLineArgs] try to get value for key: {}", key);

    _requested_keys.insert(key); // to show that this key has been checked

    // to handle bool - it doesn't have a key
    if constexpr(std::is_same_v<bool, std::remove_cv_t<T>>) {
        if (HasKey(key)) {
            val = true;
            return true;
        } else {
            return false;
        }
    }

    auto val_opt = GetVal(key);
    if (val_opt.has_value()) {
        if constexpr (std::is_pointer_v<std::remove_cv_t<T>>) {
            EFT_PROF_INFO("is a pointer, loop again, removing pointer");
            return SetValIfArgExists(key, *val);
        }
        else if constexpr(is_smart_pointer<T>::value) {
            EFT_PROF_INFO("Is a smart pointer, extract the pointed value");
            return SetValIfArgExists(key, *val);
        }
        else if constexpr(std::is_same_v<std::string, std::remove_cv_t<T>>) {
            EFT_PROF_DEBUG("is a string");
            val = val_opt.value();
            EFT_PROF_INFO("[CommandLineArgs] Set value for key: {:10} ==> {:10} as string", key, val_opt.value());
            return true;
        }
        else if constexpr(std::is_same_v<std::vector<char>, std::remove_cv_t<T>>) {
            EFT_PROF_DEBUG("is a vector[chars]");
            EFT_PROF_INFO("[CommandLineArgs] value for key: {:10} ==> as vector<char>", key);
            std::vector<std::string> tmp = GetVals(key).value();
            EFT_PROF_INFO("exctacted {} elems: [{}]", tmp.size(), tmp);
            val.clear();
            val.reserve(tmp.size());
            for (const auto& elem : tmp) {
                EFT_PROF_INFO("handle: [{}]", elem);
                if (elem.size() != 1) {
                    throw std::logic_error(fmt::format("{} [{}] as array of chars: {} [{}] {}",
                                                       "Cannot parse obj for the key",
                                                       key,
                                                       "element",
                                                       elem,
                                                       "is not a char"));
                }
                val.emplace_back(elem[0]);
            }
            EFT_PROF_INFO("written {} elems: [{}]", val.size(), val);
            return true;
        }
        else if constexpr(std::is_same_v<std::vector<std::string>, std::remove_cv_t<T>>) {
            EFT_PROF_DEBUG("is a vector[string]");
            EFT_PROF_INFO("[CommandLineArgs] value for key: {:10} ==> {:10} as vector<string>", key, val_opt.value());
            val = GetVals(key).value();
            return true;
            // TODO: to do the same with arrays and other containers. decay_type ?
            // TODO: add unfolding of a vector by looping over it and extracting components
        }
        else if constexpr(std::is_floating_point_v<std::remove_cv_t<T>>) {
            EFT_PROF_DEBUG("is a float");
            val = stod(val_opt.value());
            EFT_PROF_INFO("[CommandLineArgs] value for key: {:10} ==> {:10} as float", key, val_opt.value());
            return true;
        }
        if constexpr(std::is_same_v<char, std::remove_cv_t<T>>) {
            EFT_PROF_INFO("requested value is a char");
            std::string tmp = val_opt.value();
            EFT_PROF_INFO("this char: [{}] with size: {}", tmp, tmp.size());
            if (tmp.size() != 1) {
                throw std::logic_error(fmt::format("{}: [{}] as a char: [{}] {}",
                                                   "Cannot parse char for the key",
                                                   key,
                                                   tmp,
                                                   "is not a char"));
            }
            val = tmp[0];
            EFT_PROF_INFO("[CommandLineArgs] Set value for key: {:10} ==> {:10} as char", key, val_opt.value());
            return true;
        }
        else if constexpr(std::is_integral_v<std::remove_cv_t<T>>) {
            EFT_PROF_DEBUG("is an int");
            val = stoi(val_opt.value());
            EFT_PROF_INFO("[CommandLineArgs] value for key: {:10} ==> {:10} as integer", key, val_opt.value());
            return true;
        }
        else if constexpr(std::is_array_v<std::decay_t<T>>) {
            EFT_PROF_DEBUG("is an array");
            val = GetVals(key).value();
            EFT_PROF_INFO("[CommandLineArgs] value for key: {:10} ==> {:10} as array", key, val_opt.value());
            return true;
        }
        else {
            EFT_PROF_CRITICAL("[CommandLineArgs] value for key: {:10} ==> {:10} UNKNOWN TYPE", key, val_opt.value());
            EFT_PROF_CRITICAL("[CommandLineArgs] support of: string, int, float, string, vector<string>");
            return false;
        }
    } // if has value
    return false;
}

bool CommandLineArgs::HasKey(const Key& key) const noexcept
{
    _requested_keys.insert(key);
    if (keys.find(key) != keys.end())
        return true;
    return false;
}

CommandLineArgs::Key CommandLineArgs::TrimKey(const CommandLineArgs::Key& key) noexcept
{
    char first_symbol = key[0];
    char second_symbol = key[1];

    if (first_symbol != '-')
    {
        EFT_PROF_WARN("CommandLineArgs::TrimKey key {} is already trimmed", key);
        return key;
    }

    if (second_symbol != '-')
    {
        return key.substr(1, key.size());
    }
    else {
        return key.substr(2, key.size());
    }
}


#endif //EFTPROFILER_COMMANDLINEARGS_H
