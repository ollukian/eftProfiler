//
// Created by Aleksei Lukianchuk on 12-Sep-22.
//

#include "WorkspaceWrapper.h"

namespace eft::stats {
    void WorkspaceWrapper::SaveNPsSnapshot(const std::string& name) noexcept {
//        EFT_PROF_TRACE("WorkspaceWrapper::SaveNPsSnapshot as {}", name);
//        auto* globObs = GetListAsArgSet("paired_globs");
//        auto* nps     = GetListAsArgSet("paired_nps");
//        auto args = new RooArgSet();
//        args->add(*globObs);
//        args->add(*nps);
//        EFT_PROF_TRACE("[WorkspaceWrapper] save snapshot tmp_nps with globs and obs");
//        ws_->saveSnapshot("tmp_nps", *args, true);
    }
    void WorkspaceWrapper::SaveGlobsSnapshot(const std::string& name) noexcept {}
    void WorkspaceWrapper::SaveNpsAndGlobsSnapshot(const std::string& name) noexcept {}
    void WorkspaceWrapper::LoadNPsSnapshot(const std::string& name) noexcept {}
    void WorkspaceWrapper::LoadGlobsSnapshot(const std::string& name) noexcept {}
    void WorkspaceWrapper::LoadNpsAndGlobsSnapshot(const std::string& name) noexcept {}

} // stats