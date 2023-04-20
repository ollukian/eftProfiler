//
// Created by Aleksei Lukianchuk on 13-Apr-23.
//

#include "RooRealVar.h"
#include "RooArgSet.h"

#include "RooVarUtils.h"
#include <string>
#include <spdlog/fmt/bundled/format.h>

namespace eft::utils {

std::string RooVarUtils::PrintVar(const RooRealVar& var) {
    char is_const_str = 'F';
    if (var.isConstant())
        is_const_str = 'C';

    std::string errors_str = fmt::format("+- {:10}", var.getError());
    if (var.getErrorLo() != var.getErrorHi())
        errors_str = fmt::format("+ {:10} - {:10}", var.getErrorHi(), var.getErrorLo());

    return fmt::format("{:60} [{:10} {}] {}",
                       var.GetName(),
                       var.getVal(),
                       std::move(errors_str),
                       is_const_str);
}


std::string RooVarUtils::PrintVars(const RooArgSet& set) {
    std::string res;
    for (auto poi : set) {
        auto ptr = dynamic_cast<RooRealVar*>(poi);
        res += PrintVar(*ptr) + '\n';
    }
    return res;
}

} // utils