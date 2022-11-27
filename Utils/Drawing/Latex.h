//
// Created by Aleksei Lukianchuk on 27-Nov-22.
//

#ifndef EFTPROFILER_LATEX_H
#define EFTPROFILER_LATEX_H

#include <memory>
#include <map>
#include <string>
#include <set>

// not fwd to have inline make_unique<>() initiation
#include "TLatex.h"

// to read settings from it
namespace eft::stats {
    class FitManagerConfig;
}


namespace eft::utils::draw {

class Latex;
struct LatexSettings;

class Latex {
    using Settings = std::shared_ptr<LatexSettings>;
public:
    static void ActivateRegime(const std::string& name);
    static void            AddSettings(Settings settings, std::string name);
    static const Settings& GetActiveSettings() noexcept { return active_;}
    static Settings&       ChangeSettings(const std::string& name);

    static TLatex*            GetRaw() noexcept { return latexDrawerInstance_.get(); }
    static void               Draw(float x, float y, const std::string& text) noexcept;
    //static void               DrawNDC(float x, float y, const std::string& text) noexcept;
private:
    static inline std::unique_ptr<TLatex>         latexDrawerInstance_ { std::make_unique<TLatex>() };
    static inline std::map<std::string, Settings> settings_;
    static inline Settings                        active_ {};
    static inline bool                            is_settings_valid_   {true};
};

struct LatexSettings {
    float size;
    short font;
    short colour {kBlack};
    short angle  {0};
    bool  is_ndc {false};
    std::string name {"default"};

    void ReadFromConfig(const eft::stats::FitManagerConfig& config);
};



} // eft::utils::draw

#endif //EFTPROFILER_LATEX_H
