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

    [[nodiscard]]
    std::optional<Vals> GetVals(Key&& option) const;
    [[nodiscard]]
    std::optional<Val> GetVal(const Key& option) const;

    template<typename T>
    bool SetValIfArgExists(const std::string& key, T& val);

    [[nodiscard]]
    const auto& GetOps() const noexcept { return ops; }
    [[nodiscard]]
    const auto& GetKeys() const noexcept { return keys;}
    //const std::map<std::string, std::string>& GetArgs
private:
   std::map<Key, Vals> ops;
   Keys keys;
private:
    bool ParseInput(int argc, char* argv[]);
    //static std::pair<Key, Vals> ExtractVals(std::string_view raw) noexcept;
};

template<typename T>
bool CommandLineArgs::SetValIfArgExists(const std::string& key, T& val)
{
    EFT_PROF_INFO("[CommandLineArgs] try to get value for key: {}", key);
    auto val_opt = GetVal(key);
    if (val_opt.has_value()) {
        if constexpr(std::is_same_v<std::string, T>) {
            val = val_opt.value();
            EFT_PROF_INFO("[CommandLineArgs] value for key: {:10} ==> {:10} as string", key, val_opt.value());
            return true;
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
        else {
            EFT_PROF_CRITICAL("[CommandLineArgs] value for key: {:10} ==> {:10} UNKNOWN TYPE", key, val_opt.value());
            return false;
        }
    } // if has value
    return false;
}


#endif //EFTPROFILER_COMMANDLINEARGS_H
