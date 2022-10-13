//
// Created by Aleksei Lukianchuk on 26-Sep-22.
//

#ifndef EFTPROFILER_FITUTILS_H
#define EFTPROFILER_FITUTILS_H

// fwd declaration
class RooAbsPdf;
class RooAbsArg;
class RooArgSet;
class RooArgList;
class RooProdPdf;


namespace eft::stats {

struct PairConstraintsResults;

// from Statistical Tool ATLAS
// with modifications
class FitUtils {
public:
    [[nodiscard]]
    static PairConstraintsResults GetPairConstraints(RooAbsPdf* pdf,
                                                     RooArgSet* nps,
                                                     RooArgSet* globs,
                                                     RooArgSet* obs);

    [[nodiscard]]
    static RooArgSet* GetConstraintPdfs(RooAbsPdf* pdf,
                                        const RooArgSet* obs,
                                        const RooArgSet* nuis);

    static void FindUniqueProdComponents(RooProdPdf* base_pdf,
                                         RooArgSet* components,
                                         const int& recursion_count = 0);

    [[nodiscard]]
    static RooArgSet* UnfoldComponents(RooAbsArg* target,
                                       const RooArgSet* reference_components);
};

struct PairConstraintsResults
{
    RooArgList* paired_nps        = nullptr;
    RooArgList* paired_globs      = nullptr;
    RooArgList* paired_constr_pdf = nullptr;

    PairConstraintsResults();
};

} // stats

#endif //EFTPROFILER_FITUTILS_H
