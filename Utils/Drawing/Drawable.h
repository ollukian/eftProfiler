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
    std::string draw_options;
    std::string name;
    bool should_be_drawn {true};

    explicit Drawable(TObject* ptr) noexcept : obj(ptr) {};
    explicit Drawable(TObject* ptr, std::string opt = "", std::string name_ext = "") noexcept
        : obj(ptr)
        , draw_options(std::move(opt))
        , name(std::move(name_ext))
        {};
    explicit Drawable(std::unique_ptr<TObject> ptr, std::string opt = "", std::string name_ext = "") noexcept
            : obj(std::move(ptr))
            , draw_options(std::move(opt))
            , name(std::move(name_ext))
    {};
    Drawable() = default;
    ~Drawable() = default;

    static std::unique_ptr<Drawable> Create(TObject* ptr, std::string opt = "", std::string name_external = "");
    static std::unique_ptr<Drawable> Create(std::unique_ptr<TObject> ptr, std::string opt = "", std::string name_external = "");
};

std::unique_ptr<Drawable> Drawable::Create(TObject* ptr, std::string opt, std::string name_external)
{
    return std::make_unique<Drawable>(ptr, std::move(opt), std::move(name_external));
}
    std::unique_ptr<Drawable> Drawable::Create(std::unique_ptr<TObject> obj, std::string opt, std::string name_external)
    {
        return std::make_unique<Drawable>(std::move(obj), std::move(opt), std::move(name_external));
    }

} // eft::utils::draw

#endif //EFTPROFILER_DRAWABLE_H
