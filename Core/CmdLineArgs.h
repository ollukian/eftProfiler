//
// Created by Aleksei Lukianchuk on 15-Sep-22.
//

#ifndef EFTPROFILER_CMDLINEARGS_H
#define EFTPROFILER_CMDLINEARGS_H

#include <string>
#include <map>
#include <vector>
#include <optional>
#include <set>

// Wrapper around command line arguments

class CmdLineArgs {
public:
    using Key = std::string;   // -k filename.root
    using Val = std::string;   // ^     ^----- val
    using Vals = std::vector<Val>;  // |----------- key


    using Keys = std::set<Key>;
    //using option = std::string;

    CmdLineArgs() = delete;
    CmdLineArgs(const CmdLineArgs&) = delete;
    CmdLineArgs(CmdLineArgs&&)      = delete;
    CmdLineArgs& operator = (const CmdLineArgs&) = delete;
    CmdLineArgs& operator = (CmdLineArgs&&)      = delete;

    CmdLineArgs(int argc, char* argv[]);

    [[nodiscard]]
    std::optional<Vals> GetVals(Key&& option) const;
    [[nodiscard]]
    std::optional<Val> GetVal(Key&& option) const;

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
    static std::pair<Key, Vals> ExtractVals(std::string_view raw) noexcept;
};


#endif //EFTPROFILER_CMDLINEARGS_H
