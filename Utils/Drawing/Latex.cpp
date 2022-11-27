//
// Created by Aleksei Lukianchuk on 27-Nov-22.
//

#include "Latex.h"

#include "../../Application/FitManagerConfig.h"

#include "../../Core/Logger.h"
#include "../../Core/Profiler.h"

namespace eft::utils::draw {

void LatexSettings::ReadFromConfig(const eft::stats::FitManagerConfig& config) {
    this->size = config.text_size;
    this->font = config.text_font;
//#ifndef EFT_GET_FROM_CONFIG
//#define EFT_GET_FROM_CONFIG(param) \
//    this->param = config.param;
//
//    EFT_GET_FROM_CONFIG(text_size);
//#undef EFT_GET_FROM_CONFIG
//#endif
}

void Latex::Draw(float x, float y, const std::string& text) noexcept {
    EFT_PROFILE_FN();

    if (! is_settings_valid_ ) {
        EFT_PROF_TRACE("Current settings are not valid, need to update");
        const auto& settings = GetActiveSettings();
        latexDrawerInstance_->SetNDC(settings->is_ndc);
        latexDrawerInstance_->SetTextColor(settings->colour);
        latexDrawerInstance_->SetTextFont(settings->font);
        latexDrawerInstance_->SetTextSize(settings->size);
        latexDrawerInstance_->SetTextAngle(settings->angle);
        EFT_PROF_TRACE("Changed settings according to: {}:", settings->name);

        EFT_PROF_TRACE("Set font to:    [{}]", settings->font);
        EFT_PROF_TRACE("Set colour to:  [{}]", settings->colour);
        EFT_PROF_TRACE("Set size to:    [{}]", settings->size);
        EFT_PROF_TRACE("Set angle to:   [{}]", settings->angle);
        EFT_PROF_TRACE("Set is_ndc to:  [{}]", settings->is_ndc);
    }

    if (GetActiveSettings()->is_ndc) {
        EFT_PROF_DEBUG("Draw text NDC: {:20} at: [{.3}][{.3}]", text, x, y);
        latexDrawerInstance_->DrawLatexNDC(x, y, text.c_str());
    } else {
        EFT_PROF_DEBUG("Draw text    : {:20} at: [{.3}][{.3}]", text, x, y);
        latexDrawerInstance_->DrawLatex(x, y, text.c_str());
    }
}

Latex::Settings& Latex::ChangeSettings(const std::string& name) {
    EFT_PROFILE_FN();
    if (settings_.find(name) == settings_.end()) {
        EFT_PROF_CRITICAL("[Latex::ChangeSettings] settings with name: {} are not present. Available: {} ones",
                          name,
                          settings_.size());
        throw std::runtime_error("");
    }
    return settings_.at(name);
}

void Latex::ActivateRegime(const std::string& name) {
    EFT_PROFILE_FN();
    if (settings_.find(name) == settings_.end()) {
        EFT_PROF_CRITICAL("[Latex::ActivateRegime] settings with name: {} are not present. Available: {} ones",
                          name,
                          settings_.size());
        throw std::runtime_error("");
    }
    is_settings_valid_ = false;
    active_ = settings_.at(name);
    EFT_PROF_INFO("Latex: activate regime: [{}]", name);
}

void Latex::AddSettings(Settings settings, std::string name) {
    EFT_PROFILE_FN();
    if (settings_.find(name) != settings_.end()) {
        EFT_PROF_CRITICAL("[Latex::AddSettings] settings with name: {} are already present",
                          settings->name);
        throw std::runtime_error("");
    }
    EFT_PROF_DEBUG("Latex register settings: {}", name);
    settings_[std::move(name)] = std::move(settings);
}

} // draw