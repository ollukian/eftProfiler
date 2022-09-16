//
// Created by Aleksei Lukianchuk on 15-Sep-22.
//

#include <nlohmann/json.hpp>
#include <spdlog/fmt/fmt.h>

#include "NpRankingPlotter.h"
#include <iostream>
#include <fstream>

using namespace std;

namespace eft::plot {

void NpRankingPlotter::ReadValues(const std::filesystem::path& path)
{
    namespace fs = std::filesystem;

    cout << "[NpRankingPlotter] read values from: " << path.string() << endl;
    cout << "[NpRankingPlotter] start looping over the directory" << endl;

    for (const auto& entry : fs::directory_iterator{path}) {
        const auto filenameStr = entry.path().filename().string();
        if (entry.is_directory()) {
            cout << setw(15) << filenameStr << " is a directory" << endl;
        }
        else if (entry.is_regular_file()) {
            ReadValuesOneFile(entry);
        }
    }

}

void NpRankingPlotter::ReadValuesOneFile(const std::filesystem::path& path)
{
    const string filename = path.string();
    cout << fmt::format("[ReavValueOneFile] read from {}", filename);
    const string extension = path.extension().string();
    cout << fmt::format("[ReavValueOneFile] extension: {}", extension);
    if (extension != "json") {
        cout << fmt::format("[ReavValueOneFile] extension: {} of {} is not .json", filename, extension);
        return;
    }

    cout << "[ReavValueOneFile] try to open: " << filename << endl;
    ifstream ifs(filename);
    if ( ! ifs.is_open() ) {
        cout << "[ReavValueOneFile] cannot open: " << filename << endl;
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