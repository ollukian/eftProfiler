//
// Created by Aleksei Lukianchuk on 27-Nov-22.
//

#include "Scene.h"
#include "Drawing/Latex.h"

#include "TLatex.h"
#include "TH1D.h"
#include "TBox.h"
#include "TMarker.h"
#include "TLegend.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TGaxis.h"

#include "Logger.h"
#include "Profiler.h"

using namespace std;

namespace eft::utils::draw {

void Scene::Init(const std::shared_ptr<eft::stats::FitManagerConfig>& config) noexcept {
    config_ = config;
}

TCanvas* Scene::Create(size_t width, size_t height) {
    EFT_PROFILE_FN();
    EFT_PROF_INFO("Create scene with width: {}, height: {}", width, height);
    if (canvas_ != nullptr) {
        EFT_PROF_WARN("A scene already exists. New one will replace the previous one");
        canvas_.reset();
    }
    canvas_ = std::make_unique<TCanvas>("c", "c", width, height);
    return canvas_.get();
}

Drawable* Scene::AddBox(float xl, float yl, float xh, float yh) {
    EFT_PROFILE_FN();
    EFT_PROF_INFO("Create box with coord: [{}], [{}], [{}], [{}]", xl, yl, xh, yh);
    //std::unique_ptr<TObject> box = make_unique<TBox>(xl, yl, xh, yh);
    std::shared_ptr<TObject> box = std::make_shared<TBox>(xl, yl, xh, yh);
    //owned_.insert(box);
    //auto box_drawable =
    //auto box_drawable = make_unique<Drawable>(std::move(box));
    //Register(box_drawable);
    return Register(box);
    //return dynamic_cast<TBox *>(Register(box_drawable));
}

void Scene::Draw() noexcept {
    EFT_PROFILE_FN();
    EFT_PROF_INFO("Draw scene");
    for (const auto& ptr : objects_) {
        if (ptr->should_be_drawn) {
            EFT_PROF_DEBUG("Obj: {:10} draw with options: {}", ptr->name, ptr->draw_options);
            ptr->obj->Draw(ptr->draw_options.c_str());
        }
        else {
            EFT_PROF_DEBUG("Obj: {:10} should not be drawn");
        }
    }
}

void Scene::SaveAs(const std::string& name) noexcept {
    EFT_PROFILE_FN();
    EFT_PROF_INFO("Save canvas as: {}", name);
    canvas_->SaveAs(name.c_str());
}

void Scene::CreateLatexHorizontalOrientation() noexcept {
    EFT_PROFILE_FN();

    LatexSettings settings_horizontal;
    settings_horizontal.ReadFromConfig(*config_);

    settings_horizontal.angle = 0;
    settings_horizontal.size = 0.040f;
    CreateLatex(std::move(settings_horizontal), "horizontal");
}
void Scene::CreateLatexVerticalOrientation() noexcept {
    EFT_PROFILE_FN();

    LatexSettings settings_horizontal;
    settings_horizontal.ReadFromConfig(*config_);
    settings_horizontal.angle = 90;
    settings_horizontal.size = 0.025f;
    CreateLatex(std::move(settings_horizontal), "vertical");
}

TLatex*  Scene::DrawLatex(float x, float y, const std::string& text) {
    latexDrawer->Draw(x, y, text);
    return latexDrawer->GetRaw();
}

void Scene::CreateLatex(LatexSettings settings, std::string name) {
    //latexDrawer->AddSettings(std::move(settings), std::move(name));
}

} // eft::utils::draw