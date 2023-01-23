//
// Created by Aleksei Lukianchuk on 16-Dec-22.
//

#ifndef EFTPROFILER_MISSINGNPSPROCESSORSETTINGS_H
#define EFTPROFILER_MISSINGNPSPROCESSORSETTINGS_H


#include <filesystem>

struct MissingNpsProcessorSettings
{
    std::filesystem::path   path_files;
    std::string             separator;
};

#endif //EFTPROFILER_MISSINGNPSPROCESSORSETTINGS_H
