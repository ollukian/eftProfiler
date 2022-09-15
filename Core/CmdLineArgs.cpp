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

    if (argc < 2) {
        cout << fmt::format("[CmdLineArgs] no input args") << endl;
        return;
    }

    cout << fmt::format("[CmdLineArgs] parse {} command line arguments:", (argc - 1)) << endl;
    ParseInput(argc, argv);
}

bool CmdLineArgs::ParseInput(int argc, char* argv[])
{
    vector<string> tokens;
    for (size_t idx {1}; idx != argc; ++idx) {
        tokens.emplace_back(argv[idx]);
        cout << fmt::format("add: [{}] token", tokens.back()) << endl;
    }
    string key = tokens.front();
    vector<string> vals;

    for (auto& token : tokens) {
        cout << fmt::format("token: [{}]", token) << endl;
        if (token.find('-') != string::npos) {
            cout << fmt::format("\t[{}] is a new key, clean it", token) << endl;
            if ( ! key.empty() ) { // get rid of the prev key
                ops[key] = vals;
                vals.clear();
            }

            key = token.substr(token.find_first_not_of('-'), token.size());
            keys.insert(token);
        }
        else {
            cout << fmt::format("\t[{}] is a val", token) << endl;
            vals.push_back(token);
        }
    }

    ops[std::move(key)] = std::move(vals);

    cout << "* res" << endl;
    for (const auto& [key_, vals_] : ops) {
        for (const auto& val_ : vals_) {
            cout << "* " << key_ << " \t " << val_ << endl;
        }
    }


//    while ( ! args.empty() ) {
//        size_t idx_first_arg_begin = args.find_first_of('-');
//        size_t idx_last_arg_begin  = args.find_first_not_of('-');
//        string arg_line = args.substr(idx_first_arg_begin, idx_last_arg_begin - idx_first_arg_begin);
//        //args = args.substr(idx_last_arg_begin + 1, args.size());
//        //args = args.substr(args.find_first_not_of(' '))
//        cout << "to parse: {" << arg_line << "}" << endl;
//        auto [key, vals] = ExtractVals(std::move(arg_line));
//        size_t pos_last_val = args.find(vals.back());
//
//        args = args.substr(pos_last_val + vals.back().size() + 1, args.size());
//        cout << "left for the next: {" << args << "}" << endl;
//        keys.insert(key);
//        ops[key] = std::move(vals);
//
//
//
//    }

    //for (size_t idx {1}; idx != argc; ++idx) {
        //cout << "parse input. Idx: " << idx << endl;
        //string arg = {argv[idx]};
        //cout << "arg: " << arg << endl;

//        if (arg.find('-') != string::npos) {
//            cout << "arg contains - symbol" << endl;
//            if (idx < argc - 1) {
//                cout << "* can increase idx" << endl;
//                string new_args = argv[++idx];
//                cout << "* new args: " << new_args << endl;
//                while (arg.find('-') == string::npos) {
//                    cout << " ** keep adding";
//                    arg += ' ' + new_args;
//                    new_args = argv[++idx];
//                    cout << "arg: {" << arg << "}, new_args: {" << new_args << "}" << endl;
//                } // extracted all relevant args
//            }
//        }

       // cout << fmt::format("\t{}", arg) << endl;
//
//        auto [key, vals] = ExtractVals(std::move(arg));
//        keys.insert(key);
//        //keys.push_back(key);
//        ops[key] = std::move(vals);
//    }
    return true;
}

//std::pair<CmdLineArgs::Key, CmdLineArgs::Vals>
//CmdLineArgs::ExtractVals(std::string_view raw) noexcept
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