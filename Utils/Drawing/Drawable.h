//
// Created by Aleksei Lukianchuk on 27-Nov-22.
//

#ifndef EFTPROFILER_DRAWABLE_H
#define EFTPROFILER_DRAWABLE_H

#include <string>
#include <memory>
#include <utility>

#include "TObject.h"

namespace eft::utils::draw {

struct Drawable {
    std::unique_ptr<TObject> obj {};
    //TObject* obj = nullptr;
    std::string draw_options;
    std::string name;
    bool should_be_drawn {true};

    explicit Drawable(TObject* ptr) noexcept : obj(ptr) {};
    explicit Drawable(TObject* ptr, std::string opt = "") noexcept : obj(ptr), draw_options(std::move(opt)) {};
    explicit Drawable(std::unique_ptr<TObject> ptr, std::string opt = "") noexcept
            : obj(std::move(ptr))
            , draw_options(std::move(opt))
    {};
    Drawable() = default;
    ~Drawable() = default;
};

} // eft::utils::draw

#endif //EFTPROFILER_DRAWABLE_H
