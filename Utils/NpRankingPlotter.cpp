//
// Created by Aleksei Lukianchuk on 15-Sep-22.
//

#include <nlohmann/json.hpp>
#include <spdlog/fmt/fmt.h>

#include "NpRankingPlotter.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <filesystem>

using namespace std;

namespace eft::plot {

void NpRankingPlotter::ReadValues(const std::filesystem::path& path)
{
    namespace fs = std::filesystem;

    cout << "[NpRankingPlotter] read values from: " << path.string() << endl;
    cout << "[NpRankingPlotter] start looping over the directory" << endl;

    for (const auto& entry : fs::directory_iterator{path}) {
        const auto filenameStr = entry.path().filename().string();
        cout << filenameStr;
        if (entry.is_directory()) {
            cout << " ==> is a directory" << endl;
        }
        else if (entry.is_regular_file()) {
            cout << " ==> is a regular file, try to parse it" << endl;
            ReadValuesOneFile(entry);
        }
    }

}

void NpRankingPlotter::ReadValuesOneFile(const std::filesystem::path& path)
{
    const string filename = path.string();
    cout << fmt::format("[ReadValuesOneFile] read from {}", filename) << endl;
    const string extension = path.extension().string();
    cout << fmt::format("[ReadValuesOneFile] extension: [{}]", extension);
    if (extension != ".json") {
        cout << fmt::format(" NOT [.json]") << endl;
        return;
    }

    cout << " => is [.json]" << endl;
    cout << "[ReadValuesOneFile] try to open: " << filename << endl;
    ifstream ifs(filename);
    if ( ! ifs.is_open() ) {
        cout << "[ReadValuesOneFile] cannot open: " << filename << endl;
        throw std::runtime_error("error opening: " + filename);
    }

    nlohmann::json j;
    ifs >> j;

    auto res = j.get<NpRankingStudyRes>();


    cout << fmt::format("[ReavValueOneFile] read res for poi: {}, np: {}", res.poi_name, res.np_name) << endl;
    cout << setw(4) << j << endl;
    np_study_res_[res.np_name] = std::move(res);
}


}