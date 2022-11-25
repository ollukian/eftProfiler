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

template<typename T>
bool CommandLineArgs::SetValIfArgExists(const std::string& key, T& val)
{
    EFT_PROF_DEBUG("[CommandLineArgs] try to get value for key: {}", key);

    _requested_keys.insert(key); // to show that this key has been checked

    // to handle bool - it doesn't have a key
    if constexpr(std::is_same_v<bool, T>) {
        if (HasKey(key)) {
            val = true;
            return true;
        } else {
            return false;
        }
    }

    auto val_opt = GetVal(key);
    if (val_opt.has_value()) {
        if constexpr(std::is_same_v<std::string, T>) {
            val = val_opt.value();
            EFT_PROF_INFO("[CommandLineArgs] Set value for key: {:10} ==> {:10} as string", key, val_opt.value());
            return true;
        }
//        else if constexpr(std::is_same_v<std::vector<char>, T>) {
//            EFT_PROF_INFO("[CommandLineArgs] value for key: {:10} ==> {:10} as vector<char>", key, val_opt.value());
//            val = GetVals(key).value();
//        }
        else if constexpr(std::is_same_v<std::vector<std::string>, T>) {
            EFT_PROF_INFO("[CommandLineArgs] value for key: {:10} ==> {:10} as vector<string>", key, val_opt.value());
            val = GetVals(key).value();
            return true;
            // TODO: to do the same with arrays and other containers. decay_type ?
            // TODO: add unfolding of a vector by looping over it and extracting components
        }
        else if constexpr(std::is_floating_point_v<T>) {
            val = stod(val_opt.value());
            EFT_PROF_INFO("[CommandLineArgs] value for key: {:10} ==> {:10} as float", key, val_opt.value());
            return true;
        }
        else if constexpr(std::is_integral_v<T>) {
            val = stoi(val_opt.value());
            EFT_PROF_INFO("[CommandLineArgs] value for key: {:10} ==> {:10} as integer", key, val_opt.value());
            return true;
        }
        else if constexpr(std::is_array_v<std::decay_t<T>>) {
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
