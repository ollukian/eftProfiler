//
// Created by Aleksei Lukianchuk on 16-Dec-22.
//

#ifndef EFTPROFILER_MISSINGNPSPROCESSOR_H
#define EFTPROFILER_MISSINGNPSPROCESSOR_H

#include <filesystem>
#include <iosfwd>
#include <memory>
#include <set>
#include "MissingNpsProcessorSettings.h"

class CommandLineArgs;

namespace eft::stats::ranking {

class MissingNpsProcessor {
public:
    MissingNpsProcessor() = default;

    void ReadSettingsFromCommandLine(CommandLineArgs* cmdLineArgs);
    //void ReadNpFiles();
    void PrintMissingNps(std::string& path, const std::string& separator = " \n");
    void PrintMissingNps(std::ostream& os, const std::string& separator = " \n");
    void ComputeMissingNPs() noexcept;
private:
    std::shared_ptr<MissingNpsProcessorSettings> settings_ {std::make_shared<MissingNpsProcessorSettings>()};
    std::set<std::string> all_nps_;
    std::set<std::string> present_nps_;
    std::set<std::string> missing_nps_;
};

} // eft::stats::ranking

#endif //EFTPROFILER_MISSINGNPSPROCESSOR_H
