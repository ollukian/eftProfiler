//
// Created by Aleksei Lukianchuk on 27-Nov-22.
//

#ifndef EFTPROFILER_SCENE_H
#define EFTPROFILER_SCENE_H

#include "Logger.h"
#include "Profiler.h"

class TH1D;
class TBox;
class TMarker;
class TLegend;
class TLine;
class TGraphErorrs;
class TGraph;
class TGaxis;

#include "Drawing/Latex.h"
#include "Drawing/Drawable.h"

#include <memory>
#include <vector>
#include <string>

#include "TObject.h"
#include "TCanvas.h"


#include "TBox.h"
#include "TEllipse.h"
#include "TMarker.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TGaxis.h"

 //  public TObject, public TAttLine, public TAttFill, public TAttBBox2D
 //

namespace eft::utils::draw {


class Scene {
    //using Drawable = TObject;
    using Object = std::unique_ptr<Drawable>;
    //using Object   = Drawable;
    using HistoPtr = std::shared_ptr<TH1D>;
public:
    static void Init(const std::shared_ptr<eft::stats::FitManagerConfig>& config) noexcept;
    static void Draw() noexcept;
    static void SaveAs(const std::string& name) noexcept;

    static TCanvas* Create(size_t width = 800, size_t height = 1200);
    static Drawable*  AddBox(float xl, float yl, float xh, float yr);
    static Drawable*  AddLine(float xl, float yl, float xh, float yh, uint16_t colour = kBlack);
    //static TH1D*    AddHisto(size_t nb_bins, double low, double high);
    //static TBox*    AddBox(TBox& box);
    static TLatex*  DrawLatex(float x, float y, const std::string& text);
    static TLatex*  WriteLine(const std::string& text); // x, y, dx and dy are from settings
    //static TLatex*  DrawLatexNDC(float x, float y, const std::string& text);

    //static void     RegisterHisto(const HistoPtr& histo, std::string name = "histo");
    //static HistoPtr& GetHisto(const std::string& name);

    static const std::vector<Object>& GetRegistry() noexcept { return objects_; }
    static inline Drawable* Register(Object& object) noexcept;
    static inline Drawable* Register(Drawable* object) noexcept;

    static inline Drawable* Register(TObject* object) noexcept;

    static inline void Clear() noexcept;

private:
    //static inline TObject& Register(TObject* obj) noexcept;

    static void CreateLatexHorizontalOrientation() noexcept;
    static void CreateLatexVerticalOrientation() noexcept;
    static void CreateLatex(LatexSettings settings, std::string name);
private:
    static inline std::vector<Object>       objects_ {};
    static inline std::unique_ptr<TCanvas>  canvas_ {};
    static inline std::shared_ptr<Latex>    latexDrawer {};

    static inline std::map<std::string, HistoPtr> histos_;
    static inline std::shared_ptr<eft::stats::FitManagerConfig>  config_ {};
};

inline Drawable* Scene::Register(Drawable* object) noexcept {
    EFT_PROFILE_FN();
    EFT_PROF_INFO("Register an object with name: {} to the scene from a pointer to Drawable", object->name);
    objects_.push_back(std::make_unique<Drawable>(std::move(*object)));
    return objects_.back().get();
}

inline Drawable* Scene::Register(Scene::Object& object) noexcept {
    EFT_PROFILE_FN();
    EFT_PROF_INFO("Register an object with name: {} to the scene from a unique ptr", object->name);
    objects_.push_back(std::move(object));
    return objects_.back().get();
}

inline Drawable* Scene::Register(TObject* object) noexcept
{
    EFT_PROFILE_FN();
    EFT_PROF_INFO("Register an object with name: {} to the scene from a ptr", object->GetName());
    objects_.push_back(std::make_unique<Drawable>(object));
    //objects_.push_back(std::move(object));
    return objects_.back().get();
}

inline void Scene::Clear() noexcept {
    EFT_PROFILE_FN();
    EFT_PROF_DEBUG("Clear scene");
    objects_.clear();
    canvas_.reset();
    latexDrawer.reset();
    histos_.clear();
    config_.reset();
}

//TObject& Scene::Register(TObject* obj) noexcept {
//    auto ptr = std::make_unique<TObject>(*obj);
//    auto canvas_drawable = std::make_shared<Drawable>();
//    canvas_drawable->obj = std::move(ptr);
//    canvas_drawable->name = "canvas";
//    canvas_drawable->should_be_drawn = false;
//    return Register(canvas_drawable);
//}


} // eft::utils::draw

#endif //EFTPROFILER_SCENE_H
