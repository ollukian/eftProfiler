//
// Created by Aleksei Lukianchuk on 18-Nov-22.
//

#include "FileSystemUtils.h"
#include "../Core/Logger.h"

using namespace std;

namespace eft::utils {



optional<vector<string>> FileSystemUtils::ReadLines(const filesystem::path& path) {
    EFT_PROF_TRACE("FileSystemUtils ReadLines from {}", path.string());
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

[[nodiscard]]
std::vector<std::string> FileSystemUtils::GetListOfFiles(const std::filesystem::path& path)
{
    EFT_PROF_TRACE("FileSystemUtils ReadLines from {}", path.string());
    if ( ! is_directory(path) ) {
        EFT_PROF_CRITICAL("Path: {} is not a directory", path.string());
        return {};
    }

    vector<string> res;
    for (const auto& file : filesystem::directory_iterator(path)) {
        if (file.is_regular_file())
            res.emplace_back(file.path());
        else
            EFT_PROF_DEBUG("Skeeping: {:15} - is not a file", file.path());
    }
    return res;
}





} // eft::utils