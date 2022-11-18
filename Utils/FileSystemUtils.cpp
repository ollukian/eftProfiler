//
// Created by Aleksei Lukianchuk on 18-Nov-22.
//

#include "FileSystemUtils.h"
#include "../Core/Logger.h"

using namespace std;

namespace eft::utils {
optional<vector<string>> FileSystemUtils::ReadLines(const filesystem::path& path) {
    std::error_code errorCode_path;
    if ( !exists(path, errorCode_path) )
    {
        EFT_PROF_CRITICAL("Cannot open: {} - path doesn't exist", path.string());
        return {};
    }
    std::fstream fs(path, fstream::in);
    if ( ! fs.is_open() )
    {
        EFT_PROF_CRITICAL("Cannot open file: {} ", path.string());
        return {};
    }

    vector<string> res;
    string line;
    while (getline(fs, line)) {
        res.push_back(std::move(line));
    }
    EFT_PROF_DEBUG("Read {} lines from {}", res.size(), path.string());
    return res;
}
} // eft::utils