//
// Created by Aleksei Lukianchuk on 27-Nov-22.
//

#ifndef EFTPROFILER_DRAWABLE_H
#define EFTPROFILER_DRAWABLE_H

#include <string>
#include <memory>
#include <utility>

#include "TObject.h"

#include "Logger.h"
#include "Profiler.h"

namespace eft::utils::draw {

struct Drawable {
    TObject* obj;
    //std::weak_ptr<TObject> obj {};
    std::string draw_options;
    std::string name;
    bool should_be_drawn {true};

    Drawable(Drawable&& other) noexcept {
        obj = std::move(other.obj);
        std::swap(draw_options, other.draw_options);
        std::swap(name, other.name);
        std::swap(should_be_drawn, other.should_be_drawn);
    }

    Drawable(Drawable& other) = delete;

    //Drawable* Clone()

    template<typename T>
    T* As() {
       return dynamic_cast<T*>(obj);
    }

//    explicit Drawable(TObject* ptr) noexcept : obj(ptr) {};
//    explicit Drawable(TObject* ptr, std::string opt = "", std::string name_ext = "") noexcept
//        : obj(ptr)
//        , draw_options(std::move(opt))
//        , name(std::move(name_ext))
//        {};
    explicit Drawable(std::unique_ptr<TObject> ptr, std::string opt = "", std::string name_ext = "") noexcept
            : obj(ptr.get())
            , draw_options(std::move(opt))
            , name(std::move(name_ext))
    {};
    explicit Drawable(TObject* ptr, std::string opt = "", std::string name_ext = "") noexcept
            : obj(ptr)
            , draw_options(std::move(opt))
            , name(std::move(name_ext))
    {};
    explicit Drawable(std::shared_ptr<TObject>& ptr, std::string opt = "", std::string name_ext = "") noexcept
            : obj(ptr.get())
            , draw_options(std::move(opt))
            , name(std::move(name_ext))
    {};
    Drawable() = default;
    ~Drawable() = default;

//    template<class T>
//    static inline std::unique_ptr<Drawable> Create(T* ptr, std::string opt = "", std::string name_external = "");

//    template<class T>
//    static inline std::unique_ptr<Drawable> Create(std::unique_ptr<T> ptr, std::string opt = "", std::string name_external = "");
    static inline std::unique_ptr<Drawable> Create(TObject* ptr, std::string opt = "", std::string name_external = "");
    static inline std::unique_ptr<Drawable> Create(std::unique_ptr<TObject> obj, std::string opt = "", std::string name_external = "");


    //template<typename T>
    //static inline T* Create(T* ptr) { Create(ptr);  }
   // template <class C, typename T>
   // static inline std::unique_ptr<Drawable> Create(C init_list);
};

//template<class T>
//std::unique_ptr<Drawable> Drawable::Create(T* ptr, std::string opt, std::string name_external)
//{
//    return std::make_unique<Drawable>(ptr, std::move(opt), std::move(name_external));
//}

//template<class T>
//std::unique_ptr<Drawable> Drawable::Create(std::unique_ptr<T> obj, std::string opt, std::string name_external)
//{
//    return std::make_unique<Drawable>(std::move(obj), std::move(opt), std::move(name_external));
//}

std::unique_ptr<Drawable> Drawable::Create(TObject* ptr, std::string opt, std::string name_external)
{
    EFT_PROFILE_FN();
    EFT_PROF_INFO("Create a Drawable: {} to the scene from a pointer to Drawable", name_external);
    return std::make_unique<Drawable>(ptr, std::move(opt), std::move(name_external));
}
std::unique_ptr<Drawable> Drawable::Create(std::unique_ptr<TObject> obj, std::string opt, std::string name_external)
{
    EFT_PROFILE_FN();
    EFT_PROF_INFO("Create a Drawable: {} to the scene from a unique ptr to Drawable", name_external);
    return std::make_unique<Drawable>(std::move(obj), std::move(opt), std::move(name_external));
}

//template <class C, typename T>
//std::unique_ptr<Drawable> Drawable::Create(C init_list)
//{
//    C* obj = new C{init_list};
//    return Create(obj);
//}


} // eft::utils::draw

#endif //EFTPROFILER_DRAWABLE_H
