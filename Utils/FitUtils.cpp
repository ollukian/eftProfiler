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

    EFT_PROF_DEBUG("FitUtils::GetPairConstraints get contsrain pdfs");
    auto* constraint_pdfs = GetConstraintPdfs(pdf, obs, nps);
    EFT_PROF_DEBUG("FitUtils::GetPairConstraints get contsrain pdfs => {} elements received",
                   constraint_pdfs->size());
    for (const auto& constraint_pdf : *constraint_pdfs) {
        RooRealVar* target_np = nullptr;
        RooRealVar* target_glob = nullptr;
        RooArgSet* nuis_components = UnfoldComponents(constraint_pdf, nps);
        if (nuis_components->getSize() == 0){
            for (const auto& np : *nps) {
                //EFT_PROF_DEBUG("FitUtils::GetPairConstraints check if pdf depends on np: {}", np->GetName());
                if (constraint_pdf->dependsOn(*np)){
                    EFT_PROF_DEBUG("FitUtils::GetPairConstraints pdf depends on np: {}", np->GetName());
                    target_np = dynamic_cast<RooRealVar*>(np);
                    break;
                }
            }
        }
        else if (nuis_components->getSize() > 1) {
            EFT_PROF_CRITICAL("FitUtils::GetPairConstraints nuis_components->getSize() > 1");
            throw std::runtime_error("");
        }
        else {
            EFT_PROF_CRITICAL("FitUtils::GetPairConstraints nuis_components->getSize() == 1, pick it up: {}",
                              nuis_components->first()->GetName());
            target_np = (RooRealVar *) nuis_components->first();
        }
        EFT_PROF_DEBUG("FitUtils::GetPairConstraint loop over globs");
        for (const auto& glob : *globs) {
           // EFT_PROF_DEBUG("FitUtils::GetPairConstraint check glob: {}", glob->GetName());
            if (constraint_pdf->dependsOn(*glob)){
                EFT_PROF_DEBUG("FitUtils::GetPairConstraint pdf depends on glob: {}", glob->GetName());
                target_glob = dynamic_cast<RooRealVar*>(glob);
                break;
            }
        }
        if ((target_glob == nullptr) || (target_np == nullptr)) {
            EFT_PROF_CRITICAL("FitUtils::GetPairConstraints (target_glob == nullptr) || (target_np == nullptr)");
            continue;
            //throw std::runtime_error("");
        }
        res.paired_nps->add(*target_np);
        res.paired_globs->add(*target_glob);
        res.paired_constr_pdf->add(*constraint_pdf);
    }
    return res;
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

    if (! all_constraints)
    {
        EFT_PROF_WARN("FitUtils::GetConstraintPdfs empty all_constrains");
        return nullptr;
    }

    auto constraint_pdfs = new RooArgSet(all_constraints->GetName());

    for (const auto constrain : *all_constraints) {
        //EFT_PROF_DEBUG("FitUtils::GetConstraintPdfs deal with {} constrain", constrain->GetName());
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
    EFT_PROF_DEBUG("FitUtils::GetConstraintPdfs leave function with {} size", constraint_pdfs->size());
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
    if (reference_components->contains(*target)) {
        EFT_PROF_DEBUG("FitUtils::UnfoldComponents ref_comp contains target, add the target to the found comps");
        found_components->add(*target);
    }
    auto inner_components = new RooArgSet();
    if (target->IsA() == RooAbsPdf::Class()){
        EFT_PROF_DEBUG("FitUtils::UnfoldComponents target is RooAbsPdf, add targer->components, substruct target to the inner_comp");
        inner_components = ((RooAbsPdf*) target)->getComponents();
        inner_components->remove(*target);
    }
    else if (target->IsA() == RooProduct::Class()) {
        inner_components->add(((RooProduct *) target)->components());
        EFT_PROF_DEBUG("FitUtils::UnfoldComponents target is RooProduct. add target->components to the inner_components");
    }
    if (inner_components->getSize() != 0){
        for (const auto& component : *inner_components) {
            found_components->add(*UnfoldComponents(component, reference_components));
        }
    }
    EFT_PROF_DEBUG("UnfoldComponents leave with {} elems", found_components->size());
    return found_components;
}

PairConstraintsResults::PairConstraintsResults()
{
    EFT_PROF_TRACE("PairConstraintsResults::ctor");
    paired_globs = new RooArgList();
    paired_nps   = new RooArgList();
    paired_constr_pdf = new RooArgList();
}


} // stats