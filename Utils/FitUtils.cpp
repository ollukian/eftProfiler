//
// Created by Aleksei Lukianchuk on 26-Sep-22.
//

#include "FitUtils.h"

#include "RooAbsPdf.h"
#include "RooAbsArg.h"
#include "RooArgSet.h"
#include "RooProdPdf.h"
#include "RooRealVar.h"
#include "RooProduct.h"

#include "../Core/Logger.h"

namespace eft::stats {


PairConstraintsResults
FitUtils::GetPairConstraints(RooAbsPdf *pdf,
                             RooArgSet *nps,
                             RooArgSet *globs,
                             RooArgSet *obs)
{
    EFT_PROF_TRACE("FitUtils::GetPairConstraints");

    PairConstraintsResults res;

    auto* constraint_pdfs = GetConstraintPdfs(pdf, obs, nps);
    for (const auto& constraint_pdf : *constraint_pdfs) {
        RooRealVar* target_np = nullptr;
        RooRealVar* target_glob = nullptr;
        RooArgSet* nuis_components = UnfoldComponents(constraint_pdf, nps);
        if (nuis_components->getSize() == 0){
            for (const auto& np : *nps) {
                if (constraint_pdf->dependsOn(*np)){
                    target_np = dynamic_cast<RooRealVar*>(np);
                    break;
                }
            }
        }
        else if (nuis_components->getSize() > 1) {
            EFT_PROF_CRITICAL("FitUtils::GetPairConstraints nuis_components->getSize() > 1");
            throw std::runtime_error("");
        }
        else
            target_np = (RooRealVar*) nuis_components->first();
        for (const auto& glob : *globs) {
            if (constraint_pdf->dependsOn(*glob)){
                target_glob = dynamic_cast<RooRealVar*>(glob);
                break;
            }
        }
        if ((target_glob == nullptr) || (target_np == nullptr)) {
            EFT_PROF_CRITICAL("FitUtils::GetPairConstraints (target_glob == nullptr) || (target_np == nullptr)");
            throw std::runtime_error("");
        }
        res.paired_nps->add(*target_np);
        res.paired_globs->add(*target_glob);
        res.paired_constr_pdf->add(*constraint_pdf);
    }
}

RooArgSet*
FitUtils::GetConstraintPdfs(RooAbsPdf* pdf,
                            const RooArgSet* obs,
                            const RooArgSet* nps)
{
    EFT_PROF_TRACE("FitUtils::GetConstraintPdfs");
    auto obs_clone  = dynamic_cast<RooArgSet*>( obs->Clone() );
    auto nuis_clone = dynamic_cast<RooArgSet*>( nps->Clone() );
    auto all_constraints = pdf->getAllConstraints(*obs_clone, *nuis_clone, false);
    auto constraint_pdfs = new RooArgSet(all_constraints->GetName());

    for (const auto constrain : *all_constraints) {
        EFT_PROF_DEBUG("FitUtils::GetConstraintPdfs deal with {} constrain", constrain->GetName());
        if (constrain->IsA() == RooProdPdf::Class()) {
            EFT_PROF_DEBUG("FitUtils::GetConstraintPdfs {} is composite, add components", constrain->GetName());
            auto components = new RooArgSet();
            FindUniqueProdComponents( dynamic_cast<RooProdPdf*>(constrain), components );
            constraint_pdfs->add(*components);
        }
        else {
            EFT_PROF_DEBUG("FitUtils::GetConstraintPdfs {} is not composite, add it directly", constrain->GetName());
            constraint_pdfs->add(*constrain);
        }
    }
    return constraint_pdfs;
}

void FitUtils::FindUniqueProdComponents(RooProdPdf *base_pdf,
                                        RooArgSet *components,
                                        const int& recursion_count)
{
    EFT_PROF_TRACE("FitUtils::FindUniqueProdComponents depth: {}", recursion_count);
    if (recursion_count > 50)
        return;
    const RooArgList& pdf_list = base_pdf->pdfList();
    if (pdf_list.getSize() == 1)
        components->add(pdf_list);
    else {
        for (const auto& pdf : pdf_list) {
            if (pdf->IsA() != RooProdPdf::Class())
                components->add(*pdf);
            continue;
            FindUniqueProdComponents((RooProdPdf*)pdf, components, recursion_count + 1);
        }
    }
}

RooArgSet* FitUtils::UnfoldComponents(RooAbsArg* target,
                                      const RooArgSet* reference_components)
{
    EFT_PROF_TRACE("FitUtils::UnfoldComponents");
    auto found_components = new RooArgSet();
    if (reference_components->contains(*target))
        found_components->add(*target);
    auto inner_components = new RooArgSet();
    if (target->IsA() == RooAbsPdf::Class()){
        inner_components = ((RooAbsPdf*) target)->getComponents();
        inner_components->remove(*target);
    }
    else if (target->IsA() == RooProduct::Class())
        inner_components->add(((RooProduct*) target)->components());
    if (inner_components->getSize() != 0){
        for (const auto& component : *inner_components) {
            found_components->add(*UnfoldComponents(component, reference_components));
        }
    }
    return found_components;
}


} // stats