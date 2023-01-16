//
// Created by Aleksei Lukianchuk on 12-Jan-23.
//
// @ lukianchuk.aleksei@gmail.com

#ifndef EFTPROFILER_NLLSCANRESULT_H
#define EFTPROFILER_NLLSCANRESULT_H

#include <string>
#include <string>
#include "nlohmann/json_fwd.hpp"

#include "PoiConfig.h"
#include "../NpRankingStudyRes.h"
#include "NllScanResult_detail.h"


namespace eft::stats::scans {

using detail::NllScanResult;

//namespace detail {
//    struct NllScanResult;
//}
inline bool operator == (const NllScanResult& l, const NllScanResult& r) noexcept;
inline bool operator != (const NllScanResult& l, const NllScanResult& r) noexcept;
void to_json(nlohmann::json& j, const NllScanResult& res);
void from_json(const nlohmann::json& j, NllScanResult& res);

void to_json_v1(nlohmann::json& j, const NllScanResult& res);
void from_json_v1(const nlohmann::json& j, NllScanResult& res);

void to_json_v2(nlohmann::json& j, const NllScanResult& res);
void from_json_v2(const nlohmann::json& j, NllScanResult& res);



//namespace detail = detail_v2;
//using detail::NllScanResult;



inline bool operator == (const NllScanResult& l, const NllScanResult& r) noexcept {
    if (l.poi_configs.size() != r.poi_configs.size())
        return false;
    if (l.poi_configs != r.poi_configs)
        return false;
    if (l.nll_val != r.nll_val)
        return false;
    return true;
}
inline bool operator != (const NllScanResult& l, const NllScanResult& r) noexcept {
    return ! (l == r);
}

} // eft::stats::scans

namespace std {
    template<>
    struct hash<eft::stats::scans::NllScanResult> {
        std::size_t operator()(const eft::stats::scans::NllScanResult& res) const {
            using std::hash;
            using std::string;

            // it should not overflow for around 5-6 multiplications
            static constexpr size_t big_number = 98765;

            long long int hash_pois = 1;
            for (const auto& poi : res.poi_configs) {
                hash_pois *= big_number;
                hash_pois ^= hash<eft::stats::scans::PoiConfig>()(poi);
            }

            auto h1 = hash<float>()(res.nll_val);
            return hash_pois * big_number + h1;
        }
    };
}

#endif //EFTPROFILER_NLLSCANRESULT_H
