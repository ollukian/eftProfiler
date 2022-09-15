//
// Created by Aleksei Lukianchuk on 15-Sep-22.
//

#include "CmdLineArgs.h"
#include <iostream>
#include <iomanip>
#include <spdlog/fmt/bundled/format.h>
#include <string_view>

using namespace std;

CmdLineArgs::CmdLineArgs(int argc, char **argv) {
    //keys.reserve(argc - 1);

    cout << fmt::format("[CmdLineArgs] parse {} command line arguments:", (argc - 1)) << endl;

    for (size_t idx {0}; idx != argc; ++idx) {
        cout << fmt::format("\t{}", argv[idx]) << endl;
    }

    ParseInput(argc, argv);
}

bool CmdLineArgs::ParseInput(int argc, char* argv[])
{
    for (size_t idx {0}; idx != argc; ++idx) {
        string arg = {argv[idx]};
        cout << fmt::format("\t{}", arg) << endl;

        auto [key, vals] = ExtractVals(std::move(arg));
        keys.insert(key);
        //keys.push_back(key);
        ops[key] = std::move(vals);
    }
    return true;
}

std::pair<CmdLineArgs::Key, CmdLineArgs::Vals>
CmdLineArgs::ExtractVals(std::string_view raw) noexcept
{
    cout << "extact vals from: " << raw << endl;
    size_t pos_key_begin = raw.find_first_not_of('-');
    size_t pos_key_end   = raw.find_first_of(' ', pos_key_begin);
    auto key = raw.substr(pos_key_begin, pos_key_end - pos_key_begin);
    cout << fmt::format("raw key: {}", raw)<< endl;
    //keys.push_back(key);

    raw = raw.substr(pos_key_end);
    cout << fmt::format("vals: {}", raw) << endl;

    Vals vals;
    while (!raw.empty()) {
        size_t pos {raw.find_first_of(' ')};
        vals.push_back(raw.substr(0, pos));
        raw.remove_prefix(pos != string::npos ? pos + 1 : raw.size());
        cout << fmt::format("\t add: {}", vals.back()) << endl;
    }
    return {key, std::move(vals)};
    //return make_pair(key, std::move(vals));
}

optional<CmdLineArgs::Vals> CmdLineArgs::GetVals(CmdLineArgs::Key&& option) const
{
    cout << fmt::format("[CmdLine] GetVals for {} key", option);

    if (keys.find(option) != keys.end()) {
    //if (find( keys.begin(), keys.end(), option ) != keys.end()) {
        return nullopt;
    }
    return ops.at(option);
}

optional<CmdLineArgs::Val> CmdLineArgs::GetVal(CmdLineArgs::Key&& option) const
{
    cout << fmt::format("[CmdLine] GetVals for {} key", option);

    if (keys.find(option) != keys.end()) {
    //if (find( keys.begin(), keys.end(), option ) != keys.end()) {
        return nullopt;
    }
    return ops.at(option)[0];
}
