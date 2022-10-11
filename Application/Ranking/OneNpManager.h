//
// Created by Aleksei Lukianchuk on 11-Oct-22.
//

#ifndef EFTPROFILER_ONENPMANAGER_H
#define EFTPROFILER_ONENPMANAGER_H

#include <string>
#include <memory>

namespace eft::stats::ranking {

class RooArgSet;
class RooAbsPdf;
class RooAbsData;

using Nps = std::shared_ptr<RooArgSet>;
using Globs = std::shared_ptr<RooArgSet>;
using Data = std::shared_ptr<RooAbsData>;
using Pdf = std::shared_ptr<RooAbsPdf>;
using Poi = std::string;
using NP  = std::string;

class OneNpManagerBuilder;
class OneNpManager;

class OneNpManager {
    friend class OneNpManagerBuilder;
public:
    static inline OneNpManagerBuilder create();

    inline void ResetNp(double val = 0, double err = 0);
    inline void ResetPoi(double val = 0, double err = 0);

    OneNpManager(const OneNpManager&) = default;
    OneNpManager(OneNpManager&&) = delete;
    OneNpManager operator=(const OneNpManager&) = delete;
    OneNpManager operator=(OneNpManager&&) = delete;
private:
    mutable Poi   poi_;
    mutable NP    np_;
    mutable Nps   nps_;
    mutable Globs globs_;
    mutable Data  data_;
    mutable Pdf   pdf_;
private:
    OneNpManager() noexcept = default;
};

class OneNpManagerBuilder {
public:
    //inline OneNpManager build() const noexcept { return result_; }

    operator OneNpManager() { return result_;}

    inline OneNpManagerBuilder& UsingNPs(const Nps& nps) noexcept;
    inline OneNpManagerBuilder& UsingGlobalObservables(const Globs& globs) noexcept;
    inline OneNpManagerBuilder& UsingData(const Data& data) noexcept;
    inline OneNpManagerBuilder& UsingPdf(const Pdf& pdf) noexcept;
    inline OneNpManagerBuilder& ForPOI(const Poi& poi) noexcept;
    inline OneNpManagerBuilder& ForNP(const NP& np) noexcept;
private:
    OneNpManager result_;
};

inline OneNpManagerBuilder OneNpManager::create() {
    return {};
}

inline OneNpManagerBuilder& OneNpManagerBuilder::UsingNPs(const Nps& nps) noexcept {
    result_.nps_ = nps;
    return *this;
}

inline OneNpManagerBuilder& OneNpManagerBuilder::UsingGlobalObservables(const Globs& globs) noexcept
{
    result_.globs_ = globs;
    return *this;
}

inline OneNpManagerBuilder& OneNpManagerBuilder::UsingData(const Data& data) noexcept
{
    result_.data_ = data;
    return *this;
}

inline OneNpManagerBuilder& OneNpManagerBuilder::UsingPdf(const Pdf& pdf) noexcept
{
    result_.pdf_ = pdf;
    return *this;
}

inline OneNpManagerBuilder& OneNpManagerBuilder::ForPOI(const Poi& poi) noexcept
{
    result_.poi_ = poi;
    return *this;
}

inline OneNpManagerBuilder& OneNpManagerBuilder::ForNP(const NP& np) noexcept
{
    result_.np_ = np;
    return *this;
}


} // eft::stats::ranking

#endif //EFTPROFILER_ONENPMANAGER_H
