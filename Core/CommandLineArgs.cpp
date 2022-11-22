//
// Created by Aleksei Lukianchuk on 15-Sep-22.
//

#include "CommandLineArgs.h"
#include <iostream>
#include <iomanip>
#include <spdlog/fmt/bundled/format.h>
#include <string_view>
//#include "../Utils/StringUtils.h"

using namespace std;

CommandLineArgs::CommandLineArgs(int argc, char **argv) {

    if (argc < 2) {
        cout << fmt::format("[CommandLineArgs] no input args") << endl;
        return;
    }

    EFT_PROF_TRACE("[CommandLineArgs] parse {} command line arguments:", (argc - 1));
    ParseInput(argc, argv);
}

bool CommandLineArgs::ParseInput(int argc, char* argv[])
{
    vector<string> tokens;
    for (size_t idx {1}; idx != argc; ++idx) {
        EFT_PROF_DEBUG(" CommandLineArgs::ParseInput token: {}", string(argv[idx]));
        tokens.emplace_back(argv[idx]);
    }
    string key = tokens.front();
    key = TrimKey(key);
    vector<string> vals;


    for (auto& token : tokens) {
        if (token[0] == '-' && token[1] == '-') {
                ops[key] = vals;
                keys.insert(key);
                EFT_PROF_DEBUG("register: {:20} => {:20} vals", key, vals.size());
                vals.clear();
            key = TrimKey(token);
            AddKey(key);
        }
        else {
            vals.push_back(token);
        }
    }

    AddKey(key);
    ops[std::move(key)] = std::move(vals);

    EFT_PROF_INFO("+={:=^20}=+=====+={:=^20}=+", "=", "=");
    EFT_PROF_INFO("| {:^20} | ==> | {:^20} |", "Option", "Values");
    EFT_PROF_INFO("+={:=^20}=+=====+={:=^20}=+", "=", "=");
    for (const auto& key_ : keys) {
        if (ops.at(key_).empty())
        {
            EFT_PROF_INFO("| {:>20} |     | {:<20} |", key_, ' ');
        }
        else {
            const auto& vals_ = ops.at(key_);
            bool is_first = true;
            for (const auto& val_: vals_) {
                if (is_first) {
                    EFT_PROF_INFO("| {:>20} | ==> | {:<20} |", key_, val_);
                    is_first = false;
                }
                else
                    EFT_PROF_INFO("| {:>20} | ==> | {:<20} |", ' ', val_);
            }
        }
    }
    EFT_PROF_INFO("+={:=^20}=+=====+={:=^20}=+", "=", "=");
    return true;
}

void CommandLineArgs::AddKey(CommandLineArgs::Key key)
{
    EFT_PROF_DEBUG("Add key from output: {}", key);
    //_requested_keys[key] = true;
    _parsed_keys.insert(key);
    keys.insert(std::move(key));
}

//std::pair<CommandLineArgs::Key, CommandLineArgs::Vals>
//CommandLineArgs::ExtractVals(std::string_view raw) noexcept
//{
//    cout << "extact vals from: " << raw << endl;
//
//    //if (raw.find('-') == string::npos)
//    //    return {}
//
//    size_t pos_key_begin = raw.find_first_not_of('-');
//    size_t pos_key_end   = raw.find_first_of(' ', pos_key_begin);
//    auto key = raw.substr(pos_key_begin, pos_key_end - pos_key_begin);
//    cout << fmt::format("raw key: [{}]", key)<< endl;
//    //keys.push_back(key);
//
//    raw = raw.substr(pos_key_end, raw.size());
//
//    size_t pos_vals_first_not_empty = raw.find_first_not_of(' ');
//    raw = raw.substr(pos_vals_first_not_empty, raw.size());
//
//    cout << fmt::format("vals: [{}]", raw) << endl;
//
//    Vals vals;
//    while (!raw.empty()) {
//        size_t pos {raw.find_first_of(' ')};
//        vals.push_back(raw.substr(raw.find_first_not_of(' '), pos));
//        raw.remove_prefix(pos != string::npos ? pos + 1 : raw.size());
//        cout << fmt::format("\t add: [{}]", vals.back()) << endl;
//    }
//    return {key, std::move(vals)};
//    //return make_pair(key, std::move(vals));
//}

optional<CommandLineArgs::Vals> CommandLineArgs::GetVals(const Key& option) const
{
    //cout << fmt::format("[CmdLine] GetVals for {} key", option);
    _requested_keys.insert(option);
    if (keys.find(option) == keys.end()) {
    //if (find( keys.begin(), keys.end(), option ) != keys.end()) {
        return nullopt;
    }
    return ops.at(option);
}

optional<CommandLineArgs::Val> CommandLineArgs::GetVal(const CommandLineArgs::Key& option) const
{
    //cout << fmt::format("[CmdLine] GetVals for {} key", option);
    _requested_keys.insert(option);
    if (keys.find(option) == keys.end()) {
        return nullopt;
    }
    return ops.at(option)[0];
}

void CommandLineArgs::ReportStatus() const noexcept
{
    std::set<Key> all_keys;
    for (const auto& key: _requested_keys) {
        all_keys.insert(key);
    }
    for (const auto& key: _parsed_keys) {
        all_keys.insert(key);
    }

    std::set<Key> unknown_keys;
    size_t unknown_keys_count {0};

    // check which keys are not requested, but are parsed (== not correct keys in the request)
    for (const auto& key : all_keys)
    {
        if (_requested_keys.find(key) == _requested_keys.end()) {
            EFT_PROF_CRITICAL("Used an unknown key: {:10} in the command line", key);
            unknown_keys_count++;
            unknown_keys.insert(key);
        }

        //if (_parsed_keys.find(key) == _parsed_keys.end()) {
        //    EFT_PROF_CRITICAL("IMPLEMENTATION: forgot to  {} in the command line", key);
        //    unknown_keys++;
        //}
    } // all keys

    EFT_PROF_DEBUG("CommandLine: used {} unknown keys:", unknown_keys_count);

    if (unknown_keys_count != 0) {
        for (const auto& key : unknown_keys) {
            EFT_PROF_WARN("{:10} unknown command line option", key);
        }
        EFT_PROF_CRITICAL("Use on of the following keys:");
        for (const auto& key : _requested_keys) {
            EFT_PROF_WARN("{:10}", key);
        }
        throw std::runtime_error("Unknown command line options. Use the ones above");
    }

    EFT_PROF_DEBUG("CommandLine: code checked {} keys", _requested_keys.size());

}

//CommandLineArgs::CommandLineArgs(const vector<string>& args) {
//    string joined = eft::StringUtils::Join(' ', args);
//    istringstream is{joined};
//    CommandLineArgs(is);
//}