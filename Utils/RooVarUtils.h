//
// Created by Aleksei Lukianchuk on 13-Apr-23.
//

#ifndef EFTPROFILER_ROOVARUTILS_H
#define EFTPROFILER_ROOVARUTILS_H

class RooAbsArg;
class RooArgSet;

#include <iostream>


namespace eft::utils {

class RooVarUtils {
public:
    [[nodiscard]]
    static std::string PrintVar(const RooRealVar& var);
    static void inline PrintVar(const RooRealVar& var, std::ostream& os);

    static std::string PrintVars(const RooArgSet& set);
    static void        PrintVars(const RooArgSet& set, std::ostream& os);

};

void inline RooVarUtils::PrintVar(const RooRealVar& var, std::ostream& os) {
    os << RooVarUtils::PrintVar(var) << std::endl;
}

} // eft::utils

#endif //EFTPROFILER_ROOVARUTILS_H
