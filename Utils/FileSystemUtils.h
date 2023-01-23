//
// Created by Aleksei Lukianchuk on 18-Nov-22.
//

#ifndef EFTPROFILER_FILESYSTEMUTILS_H
#define EFTPROFILER_FILESYSTEMUTILS_H

#include <vector>
#include <string>
#include <optional>
#include <filesystem>
#include <iostream>
#include <fstream>

namespace eft::utils {

class FileSystemUtils {
public:
    [[nodiscard]]
    static std::vector<std::string> GetListOfFiles(const std::filesystem::path& path);
    [[nodiscard]]
    static std::optional<std::vector<std::string>> ReadLines(const std::filesystem::path& path);
};

} // eft::utils

#endif //EFTPROFILER_FILESYSTEMUTILS_H
