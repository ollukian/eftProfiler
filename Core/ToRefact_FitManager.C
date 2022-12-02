//
// Created by Aleksei Lukianchuk on 12-Sep-22.
//

#include "ToRefact_FitManager.h"

using namespace std;

namespace stats {

    //**************************************************************************************************
    void FitManager::PlotDatasetAndPdfGivenCat(const std::string& cat) {
        TCanvas* c = new TCanvas("c", "c", 1200, 800);

        std::string pdfSigOffName  = "pdf__commonSig_"  + cat;
        std::string pdfBkgOffName  = "pdf__background_" + cat;
        std::string pdfModelName   = "_model_" + cat;
        std::string pdfModelSBName = "_modelSB_" + cat;
        std::string varName  = "atlas_invMass_" + cat;

        RooRealVar* var = ws_->var( varName.c_str() );
        RooPlot*    frame = var->frame();
        RooAbsPdf*  pdfSig     = ws_->pdf( pdfSigOffName.c_str() );
        RooAbsPdf*  pdfBkg     = ws_->pdf( pdfBkgOffName.c_str() );
        RooAbsPdf*  pdfModel   = ws_->pdf( pdfModelName.c_str() );
        RooAbsPdf*  pdfModelSB = ws_->pdf( pdfModelSBName.c_str() );
        //auto data = ws_->data("combData");
        //RooAbsData* data = ws_->data("combDatabinned");

        RooDataSet* data = dsPerCat_[cat];

        var->setBins(55);

        data  ->plotOn( frame, RooFit::MarkerColor(kRed), RooFit::Binning(55, 105.0f, 160.0f) );
        //pdfSig->plotOn( frame, RooFit::LineColor(kBlue) );
        pdfBkg->plotOn( frame, RooFit::LineColor(kGreen) );
        pdfModel  ->plotOn( frame, RooFit::LineColor(kYellow), RooFit::LineStyle(kDashed) );
        pdfModelSB->plotOn( frame, RooFit::LineColor(kBlack), RooFit::LineStyle(kDashed) );

        /*cout << endl;
        cout << " **** print info on the signal:" << endl;
        pdfSig->Print("v");
        cout << " **** print info on the bkg:" << endl;
        pdfBkg->Print("v");
        cout << " **** print info on the data:" << endl;
        data->Print("v");
        cout << "--------------" << endl; */

        frame->Draw();

        std::string nameSave = "figures/" + cat + ".png";
        c->SaveAs(nameSave.c_str());
    }

    //**************************************************************************************************
    TList* FitManager::SplitDSonCategories() {
        RooCategory *cat = dynamic_cast<RooCategory *>( ws_->cat("channellist") );
        TList* datalist = dataSetTotal_->split(*cat,true);
        rawListDataPerCat_ = datalist;
        return datalist;
    }
    //*************************************************************************************************
    inline void FitManager::AddCatToClosure(const std::string& cat, RooDataSet* ds) {
        dsPerCat_[cat] = ds;
    }
    //**************************************************************************************************
    inline void FitManager::AddWSbyAddress(const std::string& path, const std::string& name) {
        //TFile* f = new TFile( path.c_str() );
        TFile* f_ = TFile::Open(path.c_str());
        ws_ = (RooWorkspace*) f_->Get( name.c_str() );
        //cout << " ws is extracted; Here's it" << endl;
        //ws_->Print("v");
    }
    //**************************************************************************************************
    inline void FitManager::SetUpPoisToFit(std::vector<std::string>& pois) {
        //poisToFit_ = move(pois);
        poisToFit_ = pois;
        std::cout << " * set up POI to fit:" << endl;
        for (const string& poi : poisToFit_) {
            std::cout << " - " <<  poi << endl;
        }
        cout << " * done" << endl;
    }
    //**************************************************************************************************
    inline void FitManager::SetUpPoisToFit(stats::POIs pois) {
        study_ = pois;
        SetUpPoisToFit( mapPOIsGivenScheme[pois] );
    }
    //**************************************************************************************************
    inline void FitManager::AddModelConfig(const string& name) {
        modelConfig_ = dynamic_cast<RooStats::ModelConfig*>( ws_->obj(name.c_str() ) );
        modelConfig_->
    }
    //**************************************************************************************************
    inline void FitManager::PlotDSandPdfGivenCats(const std::vector<std::string>& cats) {
        for (const string& cat : cats) {
            PlotDatasetAndPdfGivenCat(cat);
        }
    }
    //*************************************************************************************************
    inline void FitManager::ExtractSelectedVariablesToClosure() {
        /*****************************************************************
         * extracts to the working closure @dsPerCat_ only the categories
         * which are specified in the @catsForStudy
         *****************************************************************/
        cout << " * extract selected" << catsForStudy_.size() << " categories" << endl;

        SplitDSonCategories();
        cout << "[ExtractDSperCat]" << endl;
        for (const string& cat : catsForStudy_) {
            cout << cat << " -> ";
            dsPerCat_[cat] = (RooDataSet*) rawListDataPerCat_->operator()( cat.c_str() );
            cout << dsPerCat_[cat]->sumEntries() << " entries" << endl;
        }
    }
    //**************************************************************************************************
    inline void FitManager::ExtractVariablesToClosure() {
        std::vector<std::string> allCats;
        cout << " ## get list of categories" << endl;
        GetListOfCategories(allCats);
        cout << " ## done" << endl;

        //if ( rawListDataPerCat_->size() != 0) SplitDSonCategories();
        SplitDSonCategories();

        cout << " ### extract" << endl;
        for (const string& cat : allCats) {
            cout << " - " << cat << endl;
            dsPerCat_[cat] = (RooDataSet*) rawListDataPerCat_->operator()( cat.c_str() );
        }
    }
    //**************************************************************************************************
    inline void FitManager::SetUpCats(const std::vector<std::string>& cats) {
        //catsForStudy_ = move(cats);
        catsForStudy_ = cats;
    }
    //**************************************************************************************************
    inline void FitManager::SetUpCats() {
        //catsForStudy_ = move(cats);
        vector<string> cats;
        GetListOfCategories(cats);

        catsForStudy_ = move(cats);
    }
    //**************************************************************************************************
    inline void FitManager::UseBinnedData(bool b) {
        if (!b) return;

        else if(dataSetTotal_ != nullptr)
            dataHistTotal_ = dataSetTotal_->binnedClone("binned_full_ds", "Combined DS");
        else
            throw std::runtime_error("An attempt to create a binned version of Combined DS before initialising it - firstly, set it up using \"SetUpData\"");
    }
    //**************************************************************************************************
    inline RooArgSet FitManager::GetObservables(const std::vector<std::string> cats) {
        RooArgSet res;
        for (const string& cat : cats) {
            std::string varName  = "atlas_invMass_" + cat;
            res.add( *GetObservable(cat) );
        }
        return res;
    }
    //**************************************************************************************************
    inline void FitManager::GetListOfCategories(vector<string>& cats) {
            if (channelList_ == nullptr) channelList_ = dynamic_cast<RooCategory *>( ws_->cat("channellist") );

        cats.clear();
        size_t nbCats = channelList_->size();
        cats.reserve(nbCats);

        for (auto cat : *channelList_) {
            //string name = cat->GetName();
            string name = cat.first;
            cats.push_back( move(name) );
        }
    }
    //**************************************************************************************************
    inline std::vector<std::string> FitManager::GetListOfFirstNcategories(size_t nbCats) {
        cout << " # in GetListOfFirstNcategoris for " << nbCats << " categories" << endl;
        std::vector<std::string> allCats;
        GetListOfCategories(allCats);

        std::vector<std::string> firstNcats;

        for (size_t idx = 0; idx < nbCats; idx++) {
            cout << " add " << allCats[idx] << endl;
            firstNcats.push_back( allCats[idx] );
        }

        cout << " the list of cats to return contain " << firstNcats.size() << " categories" << endl;

        return move(firstNcats);
        //return firstNcats;
        //return firstNcats;
    }
    //**************************************************************************************************
    //inline void FitManager::SetAllNuisanceParamsZero() {
    inline void FitManager::SetAllNuisanceParamsConst() {
        if (nuisanceParameters_ == nullptr)
            ExtractNuisanseParameters();

        cout << " *** in set np to const" << endl;
        string name;
        //isStatOnly = true;

        for (auto& np : *nuisanceParameters_) {
            /*name = static_cast<RooRealVar*>(np)->GetTitle();
            cout << " * " << name;
            if (name.substr(0, 5) == "ATLAS")
          cout << " --- real NP" << endl;
            else
            cout << endl;*/
            if (string(static_cast<RooRealVar*>(np)->GetTitle()).substr(0, 5) == "ATLAS")
                static_cast<RooRealVar*>(np)->setConstant(1);
        }

        nuisanceParameters_->Print("v");
        cout << " * all NP are set to const \n";
    }
    //**************************************************************************************************
    inline void FitManager::SetAllNuisanceParamsFloat() {
        if (nuisanceParameters_ == nullptr)
            ExtractNuisanseParameters();

        cout << " *** in set np to float" << endl;
        string name;
        isStatOnly = false;

        for (auto& np : *nuisanceParameters_) {
            if (string(static_cast<RooRealVar*>(np)->GetTitle()).substr(0, 5) == "ATLAS")
                static_cast<RooRealVar*>(np)->setConstant(0);
        }

        cout << " * all NP are set to float \n";
    }
    //**************************************************************************************************
    inline void FitManager::SetAllNuisanceParamsToValue(float val) {
        if (nuisanceParameters_ == nullptr)
            ExtractNuisanseParameters();

        for (auto& np : *nuisanceParameters_) {
            if (string(static_cast<RooRealVar*>(np)->GetTitle()).substr(0, 5) == "ATLAS")
                static_cast<RooRealVar*>(np)->setVal(val);
        }
    }
    //**************************************************************************************************
    void FitManager::DoToyStudyGivenCategory(std::vector<std::string>& cats, size_t nbToys) {

        cout << "all obs:" << endl;
        GetObservables(cats).Print("v");

        //modelConfig_->GetNuisanceParameters()->Print("v");
        //exit(1);

        for (const string& cat : cats) {
            cout << " * Do toy study for category (for a test): " << cat << endl;
            RooDataSet* ds  = GetDataSetGivenCategory(cat);
            RooAbsPdf*  pdf = GetPdfSBGivenCategory(cat);

            pdf->fitTo(*ds, RooFit::BatchMode(true));

            double mu_data = ws_->var("mu")->getVal();
            //ws_->var("mu")->setVal(1);
            SetPOIsToValue(1);

            cout << " ============== pdf is fit to the dataset of a given category" << endl;

            RooRealVar* observable = GetObservable(cat);
            RooArgSet   argsetObs(*observable);

            RooArgSet* argsetPars   = pdf->getParameters(*observable);
            RooArgSet* snapshotPars = argsetPars->snapshot();

            size_t nbInitial = ds->sumEntries();

            TH1D* mu_obs = new TH1D("h_mu", "h_mu_obs", 25, -5, 5);
            TH1D* h_nb   = new TH1D("h", "h", 40, nbInitial - 5 * sqrt(nbInitial), nbInitial + 5 * sqrt(nbInitial) );

            RooDataSet* dsToy;
            RooFitResult* fitResult;

            for (size_t idx = 0; idx < 100; ++idx) {
                RooRandom::randomGenerator()->SetSeed(idx + 1);
                //argsetPars = snapshotPars;
                *argsetPars = *snapshotPars;

                dsToy = pdf->generate(argsetObs, RooFit::Extended());
                fitResult = pdf->fitTo(*dsToy, RooFit::Save(), RooFit::BatchMode(true));

                if (fitResult->status() != 0) continue;

                double mu = static_cast<RooRealVar*>(pdf->getVariables()->find("mu"))->getVal();
                //double mu = ( (RooRealVar*) fitResult->floatParsFinal().find("mu"))->getVal();
                mu_obs->Fill(mu);
                h_nb->Fill(dsToy->sumEntries());
            }
            //exit(1);
            //TCanvas* c = new TCanvas("c", "c", 1200, 800);
            gStyle->SetOptStat(1211);
            unique_ptr<TCanvas> c = make_unique<TCanvas>("c", "c", 1200, 800);

            h_nb->Draw("HIST");

            TLine l(nbInitial, 0, nbInitial, 100);
            l.SetLineColor(kRed);
            l.Draw();
            //TLatex tex;
            //tex.DrawLatex(0.8, 0.8, "Mean")

            h_nb->GetXaxis()->SetTitle("number of events in a toy");
            c->SaveAs( ("figures/nbEventsInToys_Without_extended" + cat + ".png").c_str() );

            //TCanvas* cmu = new TCanvas("c_mu", "c_mu", 1200, 800);
            unique_ptr<TCanvas> cmu = make_unique<TCanvas>("c_mu", "c_mu", 1200, 800);
            mu_obs->Draw("HIST");
            mu_obs->GetXaxis()->SetTitle("#mu^{obs}");
            cmu->SaveAs( ("figures/mu_obs_" + cat + ".png").c_str() );
            //dsToy = pdf->generate(*argsetObs, RooFit::Extended());
        } // categories
    }
    //**************************************************************************************************
    void FitManager::DoToyStudy(size_t nbToys) {
        /********************
         * Performs a toy study to compute sensitivity:
         *
         *
         ***************/

        //// find bkg pdf

        combinedPdf_->fitTo(*dataSetTotal_);

    }// do toy study
    //**************************************************************************************************
    void FitManager::DoAsimovStudyGivenCategoriesBKUP(std::vector<std::string>& cats) {
        cout << " *** Start do asimov for a few categories" << endl;
        RooRandom::randomGenerator()->SetSeed(1);

        RooArgSet argsetDV;
        RooArgSet argsetDVweighted;
        //combinedPdf_->fitTo(*dataSetTotal_);
        //ws_->var("mu")->setVal(1);
        SetPOIsToValue(1);

        std::map<std::string, RooDataSet*> asimovDSperCat;

        cout << " * loop over requested categories" << endl;
        // loop over cats and add their roorealvars to the argset
        // to be used later on to construct AsimovDS
        for (const string& cat : cats) {
            RooDataSet* ds  = GetDataSetGivenCategory(cat);
            RooAbsPdf*  pdf = GetPdfSBGivenCategory(cat);
            pdf->fitTo(*ds, RooFit::BatchMode(true));

            RooRealVar* var = GetObservable(cat);
            argsetDV.add(*var);
            //var->Print();
        } //cats

        //ws_->var("mu")->setVal(1);
        SetPOIsToValue(1);

        float valuePdf = 0;
        RooRealVar* weight   = new RooRealVar("weight", "weight", 0, 1000000);
        argsetDVweighted = argsetDV;
        argsetDVweighted.add(*weight);

        RooDataSet* dsAsimov;// = new RooDataSet("dsAsimov", "dsAsimov", argsetDV, RooFit::WeightVar(*weight)); // add a weight!

        cout << " ---------- add entreis for all categories to the Asimov DS" << endl;
        // add an entry to Asimov for each category
        for (const string& cat : cats) {
            cout << " ---- cat:" << cat << endl;
            RooRealVar* nBkg = GetNbkgGivenCategory(cat);
            RooRealVar* var  = GetObservable(cat);
            RooAbsPdf*  pdf  = GetPdfModelGivenCategory(cat);

            //var->setBins(55);
            RooArgSet argset(*var);
            argset.add(*weight);
            cout << " - nbkg = " << nBkg->getVal() << endl;
            cout << " - var = ";
            var->Print();
            cout << endl;
            cout << " - pdf = ";
            pdf->Print();

            const char* name = ( string("ds_") + cat ).c_str();
            asimovDSperCat[cat] = new RooDataSet(name, name, argset, RooFit::WeightVar(*weight));

            size_t nb_bins = var->getBins();
            cout << " -------- loop over bins:" << endl;
            for (size_t idx_bin = 0; idx_bin < nb_bins; ++idx_bin) {
                cout << " - " << idx_bin << endl;
                var->setBin(idx_bin);

                valuePdf = pdf->getVal(argset);
                valuePdf *= var->getBinWidth(0);

                weight->setVal(valuePdf * nBkg->getVal());
                asimovDSperCat[cat]->add(argset, weight->getVal());
            } // bins
        } //cats

        RooCategory *channelList = dynamic_cast<RooCategory *>( ws_->cat("channellist") );
        TCanvas* c;
        //make plots:
        for (const string& cat : cats) {
            RooRealVar* var = GetObservable(cat);
            c = new TCanvas("c", "c", 1200, 800);
            RooPlot* frame = var->frame(RooFit::Bins(55));

            dataSetTotal_->plotOn(frame,
                                  RooFit::Cut( (("channellist==channellist::") + cat).c_str()),
                                  RooFit::SumW2Error(1),
                                  RooFit::MarkerColor(kGreen),
                                  RooFit::LineColor(kGreen)
            );

            asimovDSperCat[cat]->plotOn(frame, RooFit::MarkerColor(kBlack));
            combinedPdf_       ->plotOn(frame, RooFit::Slice(*channelList, cat.c_str()), RooFit::ProjWData(*channelList, *dataSetTotal_), RooFit::LineColor(kBlue) );
            GetPdfModelGivenCategory(cat)->plotOn(frame, RooFit::LineColor(kRed));

            frame->Draw();
            c->SaveAs( string(("figures/Asimov_" + var->getTitle() + ".png" )).c_str() );
        }

        //argsetDV.add(*channelList);
        //argsetDV.Print("v");

        dsAsimov = new RooDataSet("dsAsimov", "dsAsimov", argsetDVweighted,
                                  RooFit::WeightVar(*weight),
                                  RooFit::Import( asimovDSperCat ),
                                  RooFit::Index(*channelList)
        );
        //dsAsimov->Print("v");

        //RooFitResult* fitResult = pdfTotal_->fitTo(*asAsimov);
        RooAbsPdf* pdf =  GetPdfModelGivenCategory(cats[0]);
        RooFitResult* fitResult = pdf->fitTo(*dsAsimov, RooFit::Save(), RooFit::BatchMode(true));
        PrintFoundPois();
        //cout << " mu = "       << static_cast<RooRealVar*>(pdf->getVariables()->find("mu"))->getVal() << endl;
        //cout << " delta mu = " << static_cast<RooRealVar*>(pdf->getVariables()->find("mu"))->getError() << endl;
    } // Asimov study
    //**************************************************************************************************
    void FitManager::DoAsimovStudy() {
        vector<string> cats;
        cout << " --- create a list of categories" << endl;
        for (auto cat : *channelList_) {
            //string name = cat->GetName();
            string name = cat.first;
            cats.push_back(name);
            cout << " - " << name << endl;
        }
        cout << " -- done " << endl;

        DoAsimovStudy(cats);
    }
    //**************************************************************************************************
    void FitManager::DoAsimovStudyBKUP() {
        /*RooRealVar* weight   = new RooRealVar("weight", "weight", 0, 1000000);
        RooArgSet argsetDVweighted = *observables_;
        cout << " ### number of observables:" << argsetDVweighted.size() << endl;
        cout << " add weight" << endl;
        argsetDVweighted.add(*weight);
        cout << " weight is added to dv" << endl;
        std::map<std::string, RooDataSet*> asimovDSperCat;

        //combinedPdf_->fitTo(*dataSetTotal_);
        ws_->var("mu")->setVal(1);

        double valuePdf = 0;
        double nBkg = 0;

        RooCategory* channelList = dynamic_cast<RooCategory*>( ws_->cat("channellist") );
        vector<string> cats;

        cout << " --- create a list of categories" << endl;
        for (auto cat : *channelList) {
          string name = cat->GetName();
          cats.push_back(name);
          cout << " - " << name << endl;
        }
        cout << " -- done " << endl;

        cout << " ---- Create sub-parts of Asimov ds" << endl;
        //for (const RooAbsArg* observable : *observables_) {
        for (const string& cat : cats) {
          RooRealVar* var  = GetObservable(cat);
          //RooRealVar* var = (RooRealVar*) observable;
          var->setBins(55);
        */
        //string cat = var->getTitle().Data();
        /*cout << " == dealing with an obs with pure name cat = " << cat << endl;
        if (cat == "channellist") { cout << " = channellist! - break the loop" << endl; break; }
        string prefix = "atlas_invMass_";
        if (cat.find(prefix) == string::npos) { cout << " = an attempt to find prefix in: " << cat << endl; break; }

        cat = cat.substr( cat.find(prefix) + prefix.size(), cat.size() );
        cout << " passes selection; Name of the cat  = " << cat << endl;*/

        /*cout << " cat = |" << cat << "| \n";

        RooArgSet argset(*var);
        argset.add(*weight);

        nBkg =  GetNbkgGivenCategory(cat)->getVal();

        const char* name = ( string("ds_") + cat ).c_str();
        asimovDSperCat[cat] = new RooDataSet(name, name, argset, RooFit::WeightVar(*weight));

        size_t nb_bins = var->getBins();
        for (size_t idx_bin = 0; idx_bin < nb_bins; ++idx_bin) {
      //cout << " - " << idx_bin << endl;
      var->setBin(idx_bin);

      valuePdf = combinedPdf_->getVal(argset);
      valuePdf *= var->getBinWidth(0);

      weight->setVal(valuePdf * nBkg);
      asimovDSperCat[cat]->add(argset, weight->getVal());
        }// bins
      } //cats

      cout << " argsetweightrd:" << endl;


      cout << " extract channel list" << endl;

      cout << " channel list = " << endl;
      channelList->Print("v");
      cout << " create an asimov ds based on the components:" << endl;
      cout << " --- argsetweighted:" << endl;
      argsetDVweighted.Print("v");
      cout << " ----------- asimovDSperCat:" << endl;
      for (const auto& cat : asimovDSperCat) {
        cout << " --- " << cat.first << "  : " << endl;
        cat.second->Print();
      }

      argsetDVweighted = *observables_;
      argsetDVweighted.add(*weight);

      cout << " ### one more time new argsetDVweighted:" << endl;
      argsetDVweighted.Print("v");

      cout << " --- activate ctor" << endl;
      RooDataSet* dsAsimov = new RooDataSet("dsAsimov", "dsAsimov",
                        argsetDVweighted,
                        RooFit::WeightVar(*weight),
                        RooFit::Import( asimovDSperCat ),
                        RooFit::Index(*channelList)
                        );
      cout << " --- dsAsimov is created" << endl;


      cout << " -- make test plots" << endl;
      RooFitResult* fitResult = combinedPdf_->fitTo(*dsAsimov, RooFit::Save());
      cout << " mu = "       << static_cast<RooRealVar*>(combinedPdf_->getVariables()->find("mu"))->getVal() << endl;
      cout << " delta mu = " << static_cast<RooRealVar*>(combinedPdf_->getVariables()->find("mu"))->getError() << endl;

      TCanvas* c;
      for (const string& cat : cats) {
        RooRealVar* var = GetObservable(cat);
        c = new TCanvas("c", "c", 1200, 800);
        RooPlot* frame = var->frame(RooFit::Bins(55));

        dataSetTotal_->plotOn(frame,
                  RooFit::Cut( (("channellist==channellist::") + cat).c_str()),
                  RooFit::SumW2Error(1),
                  RooFit::MarkerColor(kGreen),
                  RooFit::LineColor(kGreen)
                  );

        asimovDSperCat[cat]->plotOn(frame, RooFit::MarkerColor(kBlack));
        combinedPdf_       ->plotOn(frame, RooFit::Slice(*channelList, cat.c_str()), RooFit::ProjWData(*channelList, *dataSetTotal_), RooFit::LineColor(kBlue) );
        GetPdfModelGivenCategory(cat)->plotOn(frame, RooFit::LineColor(kRed));
        dsAsimov->plotOn(frame,
                 RooFit::Cut( (("channellist==channellist::") + cat).c_str()),
                 RooFit::SumW2Error(1),
                 RooFit::MarkerColor(kRed),
                 RooFit::LineColor(kRed)
                 );

        frame->Draw();
        c->SaveAs( string(("figures/Asimov_full" + cat + ".png" )).c_str() );
        }*/

        //TFile f("resources/test_4POI.root", "RECREATE");

        RooArgSet argsetDV;
        RooArgSet argsetDVweighted;
        SetPOIsToValue(1);
        FixPois();
        cout << " ### fit the initial dataset with the combined pdf" << endl;
        RooFitResult* fitres = combinedPdf_->fitTo(*dataSetTotal_, RooFit::SumW2Error(false), RooFit::Save(), RooFit::BatchMode(true), RooFit::Offset(true));
        FloatPOIs();
        //cout << " ### observed" << endl;
        //PrintFoundPois();
        //cout << " ###" << endl;
        //RooRealVar* mu = ws_->var("mu");
        //cout << "mu" << endl;
        // mu->Print("v");
        //combinedPdf_->Write("CombinedPdf_observed_fit");
        //fitres->Write("FitResultCombinedPdfOfCombinedData");
        //combinedPdf_->SaveAs("resources/4POI.root");
        //cout << " ## done" << endl;
        //cout << " its result:" << endl;
        //fitres->Print();

        RooCategory* channelList = dynamic_cast<RooCategory*>( ws_->cat("channellist") );
        vector<string> cats;

        cout << " --- create a list of categories" << endl;
        for (auto cat : *channelList) {
            //string name = cat->GetName();
            string name = cat.first;
            cats.push_back(name);
            cout << " - " << name << endl;
        }
        cout << " -- done " << endl;


        shared_ptr<TCanvas> c;
        for (const string& cat : cats) {
            RooRealVar* var = GetObservable(cat);
            c = make_shared<TCanvas>("c", "c", 1200, 800);
            RooPlot* frame = var->frame(RooFit::Bins(55));

            dataSetTotal_->plotOn(frame,
                                  RooFit::Cut( (("channellist==channellist::") + cat).c_str()),
                                  RooFit::DataError(RooAbsData::SumW2),
                                  RooFit::MarkerColor(kGreen),
                                  RooFit::LineColor(kGreen),
                                  RooFit::MarkerSize(2)
            );

            channelList->Print();
            cout << "cat=" << cat << endl;
            combinedPdf_->plotOn(frame,
                                 RooFit::Slice(*channelList, cat.c_str()),
                                 RooFit::ProjWData(*channelList, *dataSetTotal_),
                    //RooFit::ProjWData(*channelList, *dsAsimov),
                                 RooFit::LineColor(kBlue),
                                 RooFit::LineWidth(2)
            );

            /*GetPdfModelGivenCategory(cat)->plotOn(frame,
                              RooFit::LineColor(kRed),
                              RooFit::LineWidth(1)
                              );*/

            frame->Draw();
            c->SaveAs( ("figures/test_" + cat + ".png").c_str() );
        }

        //float mu_found = combWS->var("mu")->getVal();
        //SetPOIsToValue(1);
        std::map<std::string, RooDataSet*> asimovDSperCat;
        /*RooCategory* channelList = dynamic_cast<RooCategory*>( ws_->cat("channellist") );
        vector<string> cats;

        cout << " --- create a list of categories" << endl;
        for (auto cat : *channelList) {
          string name = cat->GetName();
          cats.push_back(name);
          cout << " - " << name << endl;
        }
        cout << " -- done " << endl;*/

        for (const string& cat : cats) {
            RooRealVar* var = GetObservable(cat);
            argsetDV.add(*var);
            argsetDVweighted.add(*var);
        } //cats

        cout << " ### created a list of argsetDV:" << endl;
        argsetDV.Print("v");
        cout << " ####" << endl;

        float valuePdf = 0;
        RooRealVar* weight   = new RooRealVar("weight", "weight", 0, 1000000);
        //argsetDVweighted = argsetDV;
        argsetDVweighted.add(*weight);

        cout << " ### created a list of argsetDVweighted:" << endl;
        argsetDV.Print("v");
        cout << " ####" << endl;

        RooDataSet* dsAsimov;// = new RooDataSet("dsAsimov", "dsAsimov", argsetDV, RooFit::WeightVar(*weight)); // add a weight!

        cout << " ---------- add entries for all categories to the Asimov DS" << endl;
        // add an entry to Asimov for each category

        RooRealVar* nBkg;
        RooRealVar* var;
        RooAbsPdf*  pdf;

        float n_bkg, n_sig;
        float n_generate;

        for (const string& cat : cats) {
            cout << " ---- cat:" << cat << endl;
            nBkg = GetNbkgGivenCategory(cat);
            var  = GetObservable(cat);
            pdf  = GetPdfModelGivenCategory(cat);

            n_bkg = nBkg->getVal();
            cout << " - nbkg = " << n_bkg;
            //float n_entries_tot = dsPerCat_[cat]->sumEntries();
            //n_sig = (n_entries_tot - n_bkg) / mu_found;

            n_generate = dsPerCat_[cat]->sumEntries();

            cout << " - ngenerate = " << n_generate << endl;

            //var->setBins(55);
            RooArgSet argsetVarThisCatAndWeight(*var);
            argsetVarThisCatAndWeight.add(*weight);

            //const char* name = ( string("ds_") + cat ).c_str();
            string name = "ds_" + cat;
            cout << " create an asimov ds for the category " << setw(30) << cat <<  " with the name " << name << endl;
            asimovDSperCat[cat] = new RooDataSet(name.c_str(), name.c_str(), argsetVarThisCatAndWeight, RooFit::WeightVar(*weight));

            size_t nb_bins = var->getBins();
            cout << " -------- loop over bins:" << endl;
            for (size_t idx_bin = 0; idx_bin < nb_bins; ++idx_bin) {
                cout << " - " << idx_bin << " --> ";
                var->setBin(idx_bin);

                //valuePdf = combinedPdf_->getVal(argsetVarThisCatAndWeight);
                //valuePdf = combinedPdf_->getVal(argsetVarThisCatAndWeightDVweighted);
                valuePdf = pdf->getVal(argsetVarThisCatAndWeight);
                valuePdf *= var->getBinWidth(0);
                /////cout << " valuepdf * (nBkg + nSig) = " << setw(15) << valuePdf << " * " << setw(8) << n_generate << " = " << valuePdf * n_generate << endl;
                //cout << " valuepdf * nBkg = " << setw(15) << valuePdf << " * " << setw(8) << nBkg->getVal() << " = " << valuePdf * nBkg->getVal() << endl;
                //cout << " nBkg = " << nBkg->getVal() << endl;

                weight->setVal(valuePdf * (n_generate));
                asimovDSperCat[cat]->add(argsetVarThisCatAndWeight, weight->getVal());
                //asimovDSperCat[cat]->SaveAs("resources/4POI.root");
            } // bins
            //asimovDSperCat[cat]->Write( ("asimovDSperCat_" + cat).c_str() );
        } //cats

        cout << " ### create an Asimov ds for the argsetDVweighted:" << endl;
        argsetDVweighted.Print("v");
        cout << " ### activate ctor" << endl;

        dsAsimov = new RooDataSet("dsAsimov", "dsAsimov",
                                  argsetDVweighted,
                                  RooFit::WeightVar(*weight),
                                  RooFit::Import( asimovDSperCat ),
                                  RooFit::Index(*channelList)
        );

        //dsAsimov->SaveAs("resources/4POI.root");
        //dsAsimov->Write("asimovDS");

        cout << " # Asimov ds is created; Here's it:" << endl;
        dsAsimov->Print();
        cout << " ----------- asimovDSperCat:" << endl;
        for (const auto& cat : asimovDSperCat) {
            cout << " --- " << cat.first << "  : " << endl;
            cat.second->Print();
        }

        cout << " * fit an asimov ds" << endl;
        RooFitResult* fitResult = combinedPdf_->fitTo(*dsAsimov, RooFit::Save(), RooFit::SumW2Error(false), RooFit::BatchMode(true), RooFit::Offset(true));
        //fitResult->Write("FitResultOfCombinedPdfOverAsimov");
        cout << " * fit is done" << endl;
        cout << " ### expected" << endl;
        PrintFoundPois();
        cout << " ###" << endl;

        ErrorType errorType;
        if (isStatOnly) errorType = ErrorType::STAT;
        else            errorType = ErrorType::TOTAL;

        WriteObtainedPoisToInternalStorage(ResultType::EXPECTED, errorType);

        //shared_ptr<TCanvas> c;
        //make plots:
        //for (auto obs : CombinedPdf_->getObservables(dataSetTotal_ ) ) {
        //RooRealVar* var = (RooRealVar*) obs;
        for (const string& cat : cats) {
            RooRealVar* var = GetObservable(cat);
            c = make_shared<TCanvas>("c", "c", 1200, 800);
            RooPlot* frame = var->frame(RooFit::Bins(55));

            /*dataSetTotal_->plotOn(frame,
                      RooFit::Cut( (("channellist==channellist::") + cat).c_str()),
                      RooFit::DataError(RooAbsData::SumW2),
                      RooFit::MarkerColor(kGreen),
                      RooFit::LineColor(kGreen),
                      RooFit::MarkerSize(2)
                      );*/

            dsAsimov->plotOn(frame,
                             RooFit::Cut( (("channellist==channellist::") + cat).c_str()),
                             RooFit::DataError(RooAbsData::SumW2),
                             RooFit::MarkerColor(kBlack),
                             RooFit::LineColor(kBlack),
                             RooFit::MarkerSize(1)
            );



            combinedPdf_->plotOn(frame,
                                 RooFit::Slice(*channelList, cat.c_str()),
                    //RooFit::ProjWData(*channelList, *dataSetTotal_),
                                 RooFit::ProjWData(*channelList, *dsAsimov),
                                 RooFit::LineColor(kBlue),
                                 RooFit::LineWidth(2)
            );

            /*asimovDSperCat[cat]->plotOn(frame,
                        RooFit::DataError(RooAbsData::SumW2),
                        RooFit::MarkerColor(kBlack),
                        RooFit::LineWidth(1)
                        );*/
            //TO DO channelList->setIndex"the good cateogry
            channelList->Print();
            cout << "cat=" << cat << endl;

            /*GetPdfModelGivenCategory(cat)->plotOn(frame,
                              RooFit::LineColor(kRed),
                              RooFit::LineWidth(1)
                              );*/

            frame->Draw();
            c->SaveAs( string(("figures/Asimov_full_" + var->getTitle() + ".png" )).c_str() );
        }

        PrintFoundPois();

        //f.Close();

    }
    //**************************************************************************************************
    void FitManager::DoAsimovStudy(std::vector<std::string>& cats) {
        cout << " #[INFO]: NP:" << endl;
        PrintNP();
        cout << " [STUDY]: DoAsimovStudy for " << cats.size() << " categories";
        if (isStatOnly) {
            cout << " in stat only regime " << endl;
            SetAllNuisanceParamsConst();
        }
        else {
            cout << " in stat + syst regime " << endl;
            SetAllNuisanceParamsFloat();
        }

        RooSimultaneous* combinedPdf  = CreateCombinedPdfFromGivenCategories(cats);
        RooDataSet*      dataSetTotal = CreateCombinedDataSetFromGivenCategories(cats);

        //RooArgSet argsetDV;
        //RooArgSet argsetDVweighted;

        cout << " #[INFO]: pois:" << endl;
        PrintFoundPois();
        PrintPOIs();

        cout << " #[INFO]: NP:" << endl;
        PrintNP();

        if ( !isStatOnly ) {
            cout << " # set pois to 1" << endl;
            SetPOIsToValue(1);
            cout << " # fix pois" << endl;
            FixPois();

            cout << " #[INFO]: pois:" << endl;
            PrintFoundPois();
            PrintPOIs();
            //exit(1);
            cout << " #[INFO] fit the initial dataset with the combined pdf" << endl;
            RooFitResult* fitres = combinedPdf->fitTo(*dataSetTotal, RooFit::SumW2Error(false), RooFit::Save(), RooFit::BatchMode(true), RooFit::Offset(true));
            //RooFitResult* fitres = combinedPdf->fitTo(*dataSetTotal, RooFit::SumW2Error(true), RooFit::Save());
            cout << " #[INFO] after fit:" << endl;
            PrintInnerParams();
            ////cout << " #[INFO] set global observables to value found in fit" << endl;
            ////if (!isStatOnly)
            ////  SetGlobalObservablesToValueFoundInFit();
            cout << " #[INFO] float POIS" << endl;
            FloatPOIs();
        }
        PrintInnerParams();

        RooCategory* channelList = dynamic_cast<RooCategory*>( ws_->cat("channellist") );

        /*
        shared_ptr<TCanvas> c;
        for (const string& cat : cats) {
          RooRealVar* var = GetObservable(cat);
          c = make_shared<TCanvas>("c", "c", 1200, 800);
          RooPlot* frame = var->frame(RooFit::Bins(55));

          dataSetTotal->plotOn(frame,
                    RooFit::Cut( (("channellist==channellist::") + cat).c_str()),
                    RooFit::DataError(RooAbsData::SumW2),
                    RooFit::MarkerColor(kGreen),
                    RooFit::LineColor(kGreen),
                    RooFit::MarkerSize(2)
                    );

          channelList->Print();
          cout << "cat=" << cat << endl;
          combinedPdf->plotOn(frame,
                  RooFit::Slice(*channelList, cat.c_str()),
                  RooFit::ProjWData(*channelList, *dataSetTotal),
                  //RooFit::ProjWData(*channelList, *dsAsimov),
                  RooFit::LineColor(kBlue),
                  RooFit::LineWidth(2)
                  );
        */
        /*GetPdfModelGivenCategory(cat)->plotOn(frame,
                          RooFit::LineColor(kRed),
                          RooFit::LineWidth(1)
                          );*/

        /*frame->Draw();
        frame->BuildLegend()->Draw("same");
        if (isStatOnly)
      c->SaveAs( ("figures/DStotal_STAT_and_pdf_proj_on_" + cat + ".png").c_str() );
        else
      c->SaveAs( ("figures/DStotal_FULL_and_pdf_proj_on_" + cat + ".png").c_str() );
      }*/


        /*cout << " #[INFO]: check if Asimov DS exists:";
        if (asimovDSperCat == nullptr) {
          cout << " - no" << endl;
          cout << " *** [INFO] generate a new asimov ds" << endl;
          asimovDSperCat = new std::map<std::string, RooDataSet*>;
          //std::map<std::string, RooDataSet*> asimovDSperCat;

          for (const string& cat : cats) {
        RooRealVar* var = GetObservable(cat);
        argsetDV.add(*var);
        argsetDVweighted.add(*var);
          } //cats

          cout << " ### created a list of argsetDV:" << endl;
          argsetDV.Print("v");
          cout << " ####" << endl;

          float valuePdf = 0;
          RooRealVar* weight   = new RooRealVar("weight", "weight", 0, 1000000);
          //argsetDVweighted = argsetDV;
          argsetDVweighted.add(*weight);

          cout << " ### created a list of argsetDVweighted:" << endl;
          argsetDV.Print("v");
          cout << " ####" << endl;

          //RooDataSet* dsAsimov;// = new RooDataSet("dsAsimov", "dsAsimov", argsetDV, RooFit::WeightVar(*weight)); // add a weight!

          cout << " ---------- add entries for all categories to the Asimov DS" << endl;
          // add an entry to Asimov for each category

          RooRealVar* nBkg;
          RooRealVar* var;
          RooAbsPdf*  pdf;

          float n_bkg, n_sig;
          float n_generate;

          for (const string& cat : cats) {
        ///cout << " ---- cat:" << cat << endl;
        nBkg = GetNbkgGivenCategory(cat);
        var  = GetObservable(cat);
        pdf  = GetPdfModelGivenCategory(cat);

        // TO THINK: which pdf to extract? Maybe pdfSB?

        n_bkg = nBkg->getVal();
        ///cout << " - nbkg = " << n_bkg;
        //float n_entries_tot = dsPerCat_[cat]->sumEntries();
        //n_sig = (n_entries_tot - n_bkg) / mu_found;

        n_generate = dsPerCat_[cat]->sumEntries();

        ///cout << " - ngenerate = " << n_generate << endl;

        //var->setBins(55);
        RooArgSet argsetVarThisCat(*var);
        RooArgSet argsetVarThisCatAndWeight(*var);
        argsetVarThisCatAndWeight.add(*weight);

        //const char* name = ( string("ds_") + cat ).c_str();
        string name = "ds_" + cat;
        cout << " create an asimov ds for the category " << setw(30) << cat <<  " with the name " << name << endl;
        asimovDSperCat->operator[](cat) = new RooDataSet(name.c_str(), name.c_str(), argsetVarThisCatAndWeight, RooFit::WeightVar(*weight));

        size_t nb_bins = var->getBins();
        ////cout << " -------- loop over bins:" << endl;
        for (size_t idx_bin = 0; idx_bin < nb_bins; ++idx_bin) {
          //cout << " - " << idx_bin << " --> ";
          var->setBin(idx_bin);
          //RooArgSet asMyy(*var);

          //valuePdf = combinedPdf->getVal(argsetVarThisCatAndWeight);
          ////valuePdf = combinedPdf->getVal(argsetVarThisCat);
          ////valuePdf = combinedPdf->getVal(asMyy);
          valuePdf = pdf->getVal(argsetVarThisCatAndWeight);
          valuePdf *= var->getBinWidth(0);
          ////cout << " valuepdf * (n_generate) = " << setw(15) << valuePdf << " * " << setw(8) << n_generate << " = " << valuePdf * n_generate << endl;
          //cout << " valuepdf * nBkg = " << setw(15) << valuePdf << " * " << setw(8) << nBkg->getVal() << " = " << valuePdf * nBkg->getVal() << endl;
          //cout << " nBkg = " << nBkg->getVal() << endl;

          ////weight->setVal(valuePdf * (n_generate));
          //asimovDSperCat[cat]->add(argsetVarThisCatAndWeight, weight->getVal());
          ////asimovDSperCat->operator[](cat)->add(argsetVarThisCatAndWeight, valuePdf * n_generate);
          asimovDSperCat->operator[](cat)->add(argsetVarThisCat, valuePdf * n_generate);

          //asimovDSperCat[cat]->SaveAs("resources/4POI.root");
        } // bins
        //asimovDSperCat[cat]->Write( ("asimovDSperCat_" + cat).c_str() );
          } //cats

          cout << " ### create an Asimov ds for the argsetDVweighted:" << endl;
          argsetDVweighted.Print("v");
          cout << " ### activate ctor" << endl;

          dsAsimov = new RooDataSet("dsAsimov", "dsAsimov",
                    argsetDVweighted,
                    RooFit::WeightVar(*weight),
                    RooFit::Import( *asimovDSperCat ),
                    RooFit::Index(*channelList)
                    );

        } // if Asimov was not generated yet
        else {
          cout << " yes" << endl;
          cout << "# [INFO] Asimov ds already exists" << endl;
          }*/

        cout << " #[INFO]: check if Asimov DS exists:";
        if (asimovDSperCat == nullptr) {
            cout << " - no" << endl;
            cout << " *** [INFO] generate a new asimov ds" << endl;
            CreateAsimovDataSet(cats);
        }
        else {
            cout << " yes" << endl;
            cout << "# [INFO] Asimov ds already exists" << endl;
        }


        RooRealVar* nBkg;
        RooRealVar* var;
        RooAbsPdf*  pdf;
        /*
        for (const string& cat : cats) {
          cout << " ---- cat:" << cat << endl;
          //nBkg = GetNbkgGivenCategory(cat);
          var  = GetObservable(cat);
          RooArgSet argsetVar(*var);
          c = make_shared<TCanvas>("c", "c", 1200, 800);
          RooPlot* frame = var->frame(RooFit::Bins(55));
          //RooPlot* frame = var->frame();

          shared_ptr<TLegend> leg = make_shared<TLegend>(0.7, 0.7, 1.0, 1.0);

          /*   /////asimovDSperCat[cat]->plotOn(frame,
                      RooFit::LineColor(kBlack),
                      RooFit::LineWidth(1),
                      RooFit::DataError(RooAbsData::SumW2),
                      RooFit::Name("Asimov per cat")
                      );*/
        /*
          /* ////asimovDSperCat[cat]->plotOn(frame,
        RooFit::LineColor(kBlack),
        RooFit::LineWidth(1),
        RooFit::DataError(RooAbsData::SumW2),
        RooFit::Name("Asimov per cat")
        );*/

        /*dsAsimov->plotOn(frame,
                RooFit::Cut( (("channellist==channellist::") + cat).c_str()),
                RooFit::DataError(RooAbsData::Poisson),
                //RooFit::DataError(RooAbsData::Auto),
                RooFit::MarkerColor(kBlack),
                RooFit::LineColor(kBlack),
                RooFit::MarkerSize(2),
                RooFit::Name("Asimov per cat")
                );*/

        /*asimovDSperCat->operator[](cat)->plotOn(frame,
                            RooFit::LineColor(kBlack),
                            RooFit::LineWidth(1),
                            RooFit::DataError(RooAbsData::Poisson),
                            //RooFit::DataError(RooAbsData::Auto),
                            RooFit::Name("Asimov per cat")
                            );

       combinedPdf->plotOn(frame,
               RooFit::Slice(*channelList, cat.c_str()),
               //RooFit::ProjWData(argsetVar),
               //RooFit::ProjWData(*channelList, *dataSetTotal),
               RooFit::ProjWData(*channelList, *dsAsimov),
               //RooFit::ProjWData(*channelList, *asimovDSperCat[cat]),
               RooFit::LineColor(kBlue),
               RooFit::LineWidth(2),
               RooFit::Name("Combined pdf")
               );*/

        /*  ////GetPdfModelGivenCategory(cat)->plotOn(frame,
                          RooFit::LineColor(kRed),
                          RooFit::LineWidth(1),
                          RooFit::Name("pdf model this cat")
                          );*/

        /*GetPdfSBGivenCategory(cat)->plotOn(frame,
                       RooFit::LineColor(kYellow),
                       RooFit::LineWidth(1),
                       RooFit::Name("pdf SB this cat")
                       );*/

        /*leg->AddEntry("Asimov per cat",     "Asimov per cat", "P");
        leg->AddEntry("Combined pdf",       "Combined pdf", "LP");
        leg->AddEntry("pdf model this cat", "pdf model this cat", "LP");
        leg->AddEntry("pdf SB this cat",    "pdf SB this cat",    "LP");

        frame->Draw();
        leg->Draw("same");
        if (isStatOnly)
      c->SaveAs( ("figures/Generated_STAT_Asimov_with_initial_data_fit_" + cat + ".png").c_str() );
        else
        c->SaveAs( ("figures/Generated_FULL_Asimov_with_initial_data_fit_" + cat + ".png").c_str() );
        }*/
        //dsAsimov->SaveAs("resources/4POI.root");
        //dsAsimov->Write("asimovDS");

        /*
        cout << " # Asimov ds is created; Here's it:" << endl;
        dsAsimov->Print();
        cout << " ----------- asimovDSperCat:" << endl;
        for (const auto& cat : *asimovDSperCat) {
          cout << " --- " << cat.first << "  : " << endl;
          cat.second->Print();
          }*/

        cout << " #[INFO] set global observables to value found in fit" << endl;
        if (!isStatOnly)
            SetGlobalObservablesToValueFoundInFit();

        cout << " #[INFO]fit an asimov ds" << endl;
        RooFitResult* fitResult = combinedPdf->fitTo(*dsAsimov, RooFit::Save(), RooFit::SumW2Error(false), RooFit::BatchMode(true), RooFit::Offset(true));
        //RooFitResult* fitResult = combinedPdf->fitTo(*dsAsimov, RooFit::Save(), RooFit::SumW2Error(true));
        //fitResult->Write("FitResultOfCombinedPdfOverAsimov");
        cout << " * fit is done" << endl;
        PrintInnerParams();
        cout << " ### expected" << endl;
        PrintFoundPois();
        cout << " ###" << endl;

        ErrorType errorType;
        if (isStatOnly) errorType = ErrorType::STAT;
        else            errorType = ErrorType::TOTAL;

        WriteObtainedPoisToInternalStorage(ResultType::EXPECTED, errorType);

        //shared_ptr<TCanvas> c;
        //make plots:
        //for (auto obs : CombinedPdf_->getObservables(dataSetTotal_ ) ) {
        //RooRealVar* var = (RooRealVar*) obs;

        /*
        for (const string& cat : cats) {
          RooRealVar* var = GetObservable(cat);
          c = make_shared<TCanvas>("c", "c", 1200, 800);
          RooPlot* frame = var->frame(RooFit::Bins(55));

          dataSetTotal->plotOn(frame,
                   RooFit::Cut( (("channellist==channellist::") + cat).c_str()),
                   RooFit::DataError(RooAbsData::SumW2),
                   RooFit::MarkerColor(kGreen),
                   RooFit::LineColor(kGreen),
                   RooFit::MarkerSize(2)
                   );

          dsAsimov->plotOn(frame,
                   RooFit::Cut( (("channellist==channellist::") + cat).c_str()),
                   RooFit::DataError(RooAbsData::SumW2),
                   RooFit::MarkerColor(kBlack),
                   RooFit::LineColor(kBlack),
                   RooFit::MarkerSize(1)
                   );



          combinedPdf->plotOn(frame,
                  RooFit::Slice(*channelList, cat.c_str()),
                  //RooFit::ProjWData(*channelList, *dataSetTotal_),
                  RooFit::ProjWData(*channelList, *dsAsimov),
                  RooFit::LineColor(kBlue),
                  RooFit::LineWidth(2)
                  );
        */
        ///asimovDSperCat[cat]->plotOn(frame,
        //	  RooFit::DataError(RooAbsData::SumW2),
        //	  RooFit::MarkerColor(kBlack),
        //	  RooFit::LineWidth(1)
        //	  );

        //TO DO channelList->setIndex"the good cateogry
        /*channelList->Print();
        cout << "cat=" << cat << endl;

        //GetPdfModelGivenCategory(cat)->plotOn(frame,
      //				    RooFit::LineColor(kRed),
      //				    RooFit::LineWidth(1)
      //				    );

        frame->Draw();
        frame->BuildLegend()->Draw("same");
        if (isStatOnly)
      c->SaveAs( string(("figures/Asimov_STAT_" + var->getTitle() + ".png" )).c_str() );
        else
      c->SaveAs( string(("figures/Asimov_Full_" + var->getTitle() + ".png" )).c_str() );
      }*/

        PrintFoundPois();
        WriteObtainedPoisFromStorageToFile();
        FixPois();
    }
//**************************************************************************************************
    void FitManager::DoAsimovStudy(size_t nbCats) {
        cout << " # do Asimov study for " << nbCats << " categories" << endl;
        std::vector<std::string> cats = GetListOfFirstNcategories(nbCats);
        DoAsimovStudy(cats);
    }
//**************************************************************************************************
    inline void FitManager::PrintFoundPois() {
        cout << setfill('*') << setw(50) << "*" << setfill(' ') << endl;
        if (isStatOnly)
            cout << "* stat only \n";
        else
            cout << "* stat + sys \n";

        cout << "* found POIs:" << endl;
        for (const string& poi : poisToFit_) {
            cout << " * " <<  setw(15)
                 << poi << " : " << setw(10)
                 << static_cast<RooRealVar*>(combinedPdf_->getVariables()->find( poi.c_str() ))->getVal()
                 << " +- " << setw(10)
                 << static_cast<RooRealVar*>(combinedPdf_->getVariables()->find( poi.c_str() ))->getError()
                 << endl;
        }
        cout << setfill('*') << setw(50) << "*" << setfill(' ') << endl;
    }
    //**************************************************************************************************
    inline void FitManager::SetGlobalObservablesToValueFoundInFit() {
        for (auto& obs : *globalObservables_) {
            cout << " # set global obs: |" << obs->GetName() << "|";

            string name = string(obs->GetName());
            name = name.substr(string("RNDM__").size(), name.size() );

            double foundValue = ws_->var( name.c_str() )->getVal();
            static_cast<RooRealVar*>( obs )->setVal( foundValue  );

            cout << " to " << foundValue << endl;
        }
    }
    //**************************************************************************************************
    void FitManager::DoTest(){
        ws_->var("mu")->setConstant(false);
        cout << " --- launch the fit" << endl;
        RooFitResult* fitRes = combinedPdf_->fitTo(*dataSetTotal_, RooFit::SumW2Error(true), RooFit::Save(), RooFit::BatchMode(true), RooFit::Offset(true));
        cout << " --- done " << endl;
        cout << " ### fit results = " << endl;
        fitRes->Print("v");
        cout << " ### done" << endl;
    }
    //**************************************************************************************************
    void FitManager::DumpDataset(RooAbsData* ds = nullptr) {

        ds = dataSetTotal_;

        size_t nbEntris = ds->numEntries();

        //for (int idx_ev=0;idx_ev<nb_entries;idx_ev++) {
        for (size_t idx_ev = 0; idx_ev < 10; idx_ev++) {
            RooArgSet *argset = (RooArgSet *)ds->get(idx_ev);

            argset->Print();

            //    cout << "m_yy=" << argset->getRealValue("vec_roorealvar_finalDV[0]->GetName());
            cout << "wt=" << ds->weight() << endl;
            //cout << "shit=" << argset->getRealValue("shit") << endl;

        } //end loop events
    }
    //**************************************************************************************************
    RooSimultaneous* FitManager::CreateCombinedPdfFromGivenCategories(const std::vector<std::string>& cats) {
        cout << " # create a combined pdf from " << cats.size() << " categories" << endl;
        RooSimultaneous* combinedPdf = new RooSimultaneous("testCombinedPdf", "testCombinedPdf", *channelList_);

        for (const string& cat : cats) {
            cout << " - add a Pdf for the " << setw(55) << cat << endl;
            RooAbsPdf* pdf = GetPdfModelGivenCategory(cat);
            combinedPdf->addPdf( *pdf, cat.c_str() );
        }

        cout << " ### test combined pdf:" << endl;
        combinedPdf->Print("v");
        cout << endl;
        return combinedPdf;
    }
    //**************************************************************************************************
    RooSimultaneous* FitManager::CreateCombinedPdfFromGivenCategories(const std::string& cat) {
        std::vector<std::string> cats = {cat};
        //cats.push_back(cat);
        return CreateCombinedPdfFromGivenCategories(cats);
    }
    //**************************************************************************************************
    RooSimultaneous* FitManager::CreateCombinedPdfFromGivenNumberCategories(size_t nbCats) {
        /*std::vector<std::string> allCats;
        GetListOfCategories(allCats);

        std::vector<std::string> firstNcats(nbCats);

        for (string& cat : allCats) {
          firstNcats.push_back( move(cat) );
        }

        return CreateCombinedPdfFromGivenCategories(firstNcats);*/
        //return CreateCombinedPdfFromGivenCategories( GetListOfFirstNcategories(nbCats) );
        cout << " # in CreatePdf from " << nbCats << " categories" << endl;
        std::vector<std::string> cats = GetListOfFirstNcategories(nbCats);
        return CreateCombinedPdfFromGivenCategories( cats );
    }
    //**************************************************************************************************
    RooDataSet* FitManager::CreateCombinedDataSetFromGivenCategories(const std::vector<std::string>& cats) {
        RooArgSet vars;

        cout << " # create a combined DS for " << cats.size() << " categories" << endl;
        for (const string& cat : cats) {
            cout << " - " << setw(55) << "|" <<  cat << "|" << endl;
            RooRealVar* var  = GetObservable(cat);
            vars.add(*var);
        }

        RooRealVar* weight   = new RooRealVar("weight", "weight", 0, 1000000);
        vars.add(*weight);

        if (dsPerCat_.size() == 0) ExtractVariablesToClosure();

        RooDataSet* ds = new RooDataSet( "testDS", "testDS", vars, RooFit::Import( dsPerCat_ ), RooFit::WeightVar(*weight), RooFit::Index(*channelList_) );

        //cout << " # resulting ds:" << endl;
        //ds->Print("v");
        return ds;
    }
    //**************************************************************************************************
    RooDataSet* FitManager::CreateCombinedDataSetFromGivenCategories(const std::string& cat) {
        std::vector<std::string> cats = {cat};
        return CreateCombinedDataSetFromGivenCategories(cats);
    }
    //**************************************************************************************************
    RooDataSet* FitManager::CreateCombinedDataSetFromGivenNumberCategories(size_t nbCats) {
        /*std::vector<std::string> allCats;
        GetListOfCategories(allCats);

        std::vector<std::string> firstNcats(nbCats);
        for (string& cat : allCats) {
          firstNcats.push_back( move(cat) );
        }

        return CreateCombinedDataSetFromGivenCategories(firstNcats);*/
        std::vector<std::string> cats = GetListOfFirstNcategories(nbCats);
        return CreateCombinedDataSetFromGivenCategories( cats );
    }
    //**************************************************************************************************
    RooFitResult* FitManager::DoSimultaneouslyFitGivenCategories(std::vector<std::string>& cats) {
        RooSimultaneous* combinedPdf = CreateCombinedPdfFromGivenCategories(cats);
        RooDataSet*      combinedDS  = CreateCombinedDataSetFromGivenCategories(cats);

        RooFitResult* fitResult = combinedPdf->fitTo(*combinedDS, RooFit::Save(), RooFit::BatchMode(true), RooFit::Offset(true));

        fitResult->Print();
        return fitResult;
    }
    //**************************************************************************************************
    void FitManager::DoFullStudy() {

        std::vector<stats::POIs> schemes = {
                //stats::POIs::GLOBAL,
                stats::POIs::PROD_MODE_4_POI,
                //stats::POIs::PROD_MODE_5_POI,
                //stats::POIs::CATEGORIES
        };

        std::vector<std::string> cats =  {
                //"GG2H_0J_PTH_0_10__0",
                //"GG2H_1J_PTH_120_200__1",
                //"GG2H_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_0_25__2",
                //"UNSELECTED_ZH",
                //"QQ2HQQ_0J__0",
                //"QQ2HNUNU_PTV_0_75__1",
                //"QQ2HNUNU_PTV_0_75__2",
                //"GG2H_1J_PTH_0_60__0",
                //"GG2H_0J_PTH_0_10__1",
                //"QQ2HQQ_GE2J_MJJ_0_60__0",
                "QQ2HLNU_PTV_0_75__0",
                "HLL_PTV_0_75__0",
                "TTH_PTH_0_60__0",
                "GG2H_GE2J_MJJ_0_350_PTH_0_60__0",
                //"GG2H_1J_PTH_120_200__1",
                //"GG2H_1J_PTH_120_200__0",
                //"TTH_PTH_0_60__1",
                //"TTH_PTH_0_60__2",
                "TTH_PTH_60_120__0",
                "TTH_PTH_60_120__1",
                //"THJB__0",
                //"THJB__1",
                //"THJB__2",
                //"THW__0",
                //"THW__1"
        };

        /*vector<string> cats;
         cout << " --- create a list of categories" << endl;
         for (auto cat : *channelList_) {
           string name = cat->GetName();
           cats.push_back(name);
           cout << " - " << name << endl;
         }
         cout << " -- done " << endl;*/


        for (const auto scheme : schemes) {
            SetUpPoisToFit(scheme);

            //for (bool withNP : {true, false}) {
            /*for (bool withNP : {true, false}) {
          isStatOnly = !withNP;
          DoObservedFit(cats);
          }*/

            //for (bool withNP : {false}) {
            for (bool withNP : {true, false}) {
                //for (bool withNP : {false, true}) {
                isStatOnly = !withNP;
                //SetAllNuisanceParamsToValue(5);
                //DoAsimovStudy(cats);
                DoAsimovStudy();
                //DoAsimovStudy(20);
            }

            //FloatPOIs();
            //SetUpPoisToFit(scheme);
            FloatPOIs();

            //Fit();
            //isStatOnly = !withNP;

            //SetAllNuisanceParamsConst();


            //DoObservedFit();

            //FixPois();
            //DoAsimovStudy();
            //}
        }
    }
    //**************************************************************************************************
    void FitManager::DoSensitivityStudy(const string& mu1, const string& mu2) {
        std::vector<std::string> cats =  {
                "GG2H_0J_PTH_0_10__0",
                "GG2H_0J_PTH_GT10__0",
                //"GG2H_1J_PTH_60_120__0",
                "GG2H_1J_PTH_120_200__1", //
                "GG2H_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_0_25__2", //
                "GG2H_GE2J_MJJ_0_350_PTH_0_60__0",
                //"UNSELECTED_ZH",
                "QQ2HQQ_0J__0",
                "QQ2HQQ_0J__1",
                "QQ2HQQ_1J__0",
                "QQ2HQQ_1J__1",
                "QQ2HQQ_GE2J_MJJ_0_60__0",
                "QQ2HQQ_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_0_25__1",
                "HLL_PTV_0_75__0",
                "QQ2HLNU_PTV_0_75__0",
                //"HLL_PTV_0_75__1",
                "TWH__0",
                //"QQ2HNUNU_PTV_0_75__1",
                //"QQ2HNUNU_PTV_0_75__2",
                //"GG2H_1J_PTH_0_60__0",
                //"GG2H_0J_PTH_0_10__1",
                //"QQ2HQQ_GE2J_MJJ_0_60__0",
                //"QQ2HLNU_PTV_0_75__0",
                //"TTH_PTH_0_60__0",
                //"GG2H_GE2J_MJJ_0_350_PTH_60_120__0",
                //"GG2H_1J_PTH_120_200__1",
                //"GG2H_1J_PTH_120_200__0",
                "TTH_PTH_0_60__1",
                //"TTH_PTH_0_60__2",
                //"TTH_PTH_60_120__0",
                //"TTH_PTH_60_120__1",
                //"THJB__0",
                //"THJB__1",
                //"THJB__2",
                //"THW__0",
                //"THW__1"
        };


        isStatOnly = false;
        SetUpPoisToFit(stats::POIs::PROD_MODE_6_POI);
        //ComputeSensitivityGivenPOI(mu1, mu2, cats);
        ComputeSensitivityGivenPOI(mu1, mu2);
    }
    //**************************************************************************************************
    void FitManager::DoSensitivity1DStudy(const string& mu) {
        isStatOnly = false;
        SetUpPoisToFit(stats::POIs::PROD_MODE_6_POI);

        string statFlag;
        if (isStatOnly)
            statFlag = "stat";
        else
            statFlag = "full";

        cout << " #[INFO]: {ComputeSensitiity for: " << mu << "}" << endl;
        if (isStatOnly) {
            cout << " in stat only regime -> LOAD np" << endl;
            cout << "[Worker]: stat only -> load NP" << endl;
            LoadNPvalues("source/NPvalues.txt");
            //SetAllNuisanceParamsConst();
        }
        else {
            cout << " in stat + syst regime " << endl;
            SetAllNuisanceParamsFloat();
        }

        FloatPOIs();

        cout << "[Worker]: status before fit:" << endl;
        cout << "[NP]:" << endl;
        nuisanceParameters_->Print("v");
        cout << "[GlobalObs]:" << endl;
        globalObservables_->Print("v");
        cout << "[Obs]:" << endl;
        observables_->Print("v");
        cout << "[POIs]:" << endl;
        POIs_->Print("v");

        RooFitResult* res = combinedPdf_->fitTo(*dataSetTotal_,
                                                RooFit::SumW2Error(false),
                                                RooFit::BatchMode(true),
                                                RooFit::Offset(true),
                                                RooFit::Save(true)
        );

        ofstream fsRes("source/config_" + statFlag + "_" + mu, std::ios_base::out);

        for (const string& mu : {"mu_ggF", "mu_VBF", "mu_WH", "mu_ZH", "mu_ttH", "mu_tH"}) {
            fsRes << "mu_central: " << GetParameterValue(mu) << " +- " << GetParameterError(mu) << endl;
            fsRes << "nll: " << setprecision(10) << res->minNll() << endl;

            cout << "mu_central: " << GetParameterValue(mu) << " +- " << GetParameterError(mu) << endl;
            cout << "nll: " << setprecision(10) << res->minNll() << endl;
        }
        if (isStatOnly)
            fsRes << "stat" << endl;
        else
            fsRes << "full" << endl;


        /*if (! isStatOnly) {
          cout << "[Worker] SaveNP" << endl;// << mu << endl;
         cout << "set all np constant: " << endl;
         SetAllNuisanceParamsConst();
         SetGlobalObservablesToValueFoundInFit();

         cout << "save snapshot with values found in fit: " << endl;
         cout << "[NP]:" << endl;
         nuisanceParameters_->Print("v");
         cout << "[GlobalObs]:" << endl;
         globalObservables_->Print("v");
         cout << "[Obs]:" << endl;
         observables_->Print("v");
         cout << "[POIs]:" << endl;
         POIs_->Print("v");

         RooArgSet as;
         as.add(*nuisanceParameters_);
         as.add(*globalObservables_);
         as.add(*observables_);
         as.add(*POIs_);

         ws_->saveSnapshot(("source/Snapshot_1D" + mu + ".root").c_str(), as);
         //string fileNameNP = "source/np_snapshot_" + mu + ".txt";
         //cout << "[Worker] save np to the file: " << fileNameNP << endl;
         //nuisanceParameters_->SaveAs(fileNameNP.c_str());
         }*/

        fsRes.close();
    }
    //**************************************************************************************************
    inline void FitManager::RunStudyOneWorker(size_t jobID) {
        cout << " # RUN STUDY FOR WORKER #" << jobID << endl;
        std::map<size_t, stats::POIs> poisPerWorker= {
                {1, POIs::GLOBAL},
                {2, POIs::PROD_MODE_4_POI},
                {3, POIs::PROD_MODE_5_POI},
                {4, POIs::CATEGORIES}
        };

        POIs poisThisWorker = poisPerWorker[jobID];
        cout << " ## POI scheme = " << poisThisWorker << endl;
        RunStudyOnePoiScheme(poisThisWorker);
    }
    //**************************************************************************************************
    void FitManager::RunStudyOnePoiScheme(stats::POIs scheme) {
        cout << " ## RunStudyForOnePOI scheme: " << scheme << endl;
        SetUpPoisToFit(scheme);

        for (bool withNP : {true, false}) {
            isStatOnly = !withNP;
            DoAsimovStudy();
        } // with and wo np


        cout << " ## DO observed fit" << endl;
        for (bool withNP : {true, false}) {
            isStatOnly = !withNP;
            DoObservedFit();
        }

    }
    //**************************************************************************************************
    inline void FitManager::PrintStudyResults(std::string& path) {

        if (study_ == stats::POIs::GLOBAL)
            path += "global.txt";
        else if (study_ == stats::POIs::PROD_MODE_4_POI)
            path += "4POI.txt";
        else if (study_ == stats::POIs::PROD_MODE_5_POI)
            path += "5POI.txt";

        //fstream fs(path.c_str(), std::ios_base::out);

        //for (
    }
    //**************************************************************************************************
    RooFitResult* FitManager::DoObservedFit(RooAbsPdf* combinedPdf, RooAbsData* dataSet) {

        if (isStatOnly)
            SetAllNuisanceParamsConst();

        cout << " ### do observed fit" << endl;
        cout << " ** float POIs" << endl;
        FloatPOIs();
        cout << " ** fit" << endl;
        RooFitResult* fitRes = combinedPdf->fitTo(*dataSet, RooFit::Save(), RooFit::SumW2Error(false), RooFit::BatchMode(true), RooFit::Offset(true));
        cout << " * fit is done" << endl;

        ErrorType errorType;
        if (isStatOnly) errorType = ErrorType::STAT;
        else            errorType = ErrorType::TOTAL;

        WriteObtainedPoisToInternalStorage(ResultType::OBSERVED, errorType);

        FixPois();
        return fitRes;
    }
    //**************************************************************************************************
    inline RooFitResult* FitManager::DoObservedFit() {
        return DoObservedFit(combinedPdf_, dataSetTotal_);
    }
    //**************************************************************************************************
    RooFitResult* FitManager::DoObservedFit(std::vector<std::string>& cats) {
        RooSimultaneous* combinedPdf  = CreateCombinedPdfFromGivenCategories(cats);
        RooDataSet*      dataSetTotal = CreateCombinedDataSetFromGivenCategories(cats);

        return DoObservedFit(combinedPdf, dataSetTotal);
    }
    //**************************************************************************************************
    /*double FitManager::ComputeNLLgivenConfiguration(MuSensitivityConfig& config) {
      /********************
       *  For a given fixed set of values for the 2 POI makes the fit
       *  and return the value of -2 * Log(Likelihood)
       *******************
      cout << "[Likelihood scan] for the parameters: \n"
       << config.mu1.first << " -> " << config.mu1.second << endl
       << config.mu2.first << " -> " << config.mu2.second << endl;

      SetGivenPOItoValue(config.mu1);
      SetGivenPOItoValue(config.mu2);

      RooAbsPdf*  pdf = config.pdf;
      RooAbsData* ds  = config.dataset;

      RooFitResult* fitResult = pdf->fitTo(*ds, RooFit::Save(), RooFit::BatchMode(true));

      return fitResult->minNll();
    }*/
    //**************************************************************************************************
    void FitManager::WriteNLLtoFile(MuSensitivityConfig& config) {
        ofstream fs;
        //fs.open("results/NLL/)

    };
    //**************************************************************************************************
    inline void FitManager::WriteObtainedPoisToInternalStorage(ResultType resType, ErrorType errType) {
        cout << setfill('*') << setw(50) << "*" << setfill(' ') << endl;
        cout << " ** write down obtained values" << endl;

        ValueError* ptr_res;

        for (const string& var : poisToFit_) {
            cout << " * " << setw(15) << var << ": ";
            float value = GetParameterValue(var);
            float error = GetParameterError(var);
            cout << setw(5) << value << " +- " << error << endl;

            if (resType == ResultType::EXPECTED)
                ptr_res = &muResults_f_POItype_f_mu[study_][var].exp;
            else
                ptr_res = &muResults_f_POItype_f_mu[study_][var].obs;

            ptr_res->value = value;

            if (errType == ErrorType::STAT)
                ptr_res->error_stat = error;
            else if (errType == ErrorType::TOTAL)
                ptr_res->error_total = error;
            else
                throw std::runtime_error(" Error: are you sure that you've computed syst uncertainty without statistical? It's impossible to run the WriteObtainedPoisToInternalStorage function with arguments not in {STAT, TOTAL}");
        }

        cout << " ** done"  << endl;
        cout << setfill('*') << setw(50) << "*" << setfill(' ') << endl;
    }
    //**************************************************************************************************
    FitManager::~FitManager() {
        cout << " ***** in the dtor of FitManager" << endl;
        cout << " *** Summary:" << endl;
        PrintSummaryPois();
        WriteObtainedPoisFromStorageToFile();
    }
    //**************************************************************************************************
    inline void FitManager::PrintSummaryPois() {
        for (const pair<POIs, std::map<std::string, MuResult>>& poiType_muRes : muResults_f_POItype_f_mu) {
            POIs poi       = poiType_muRes.first;
            auto resPerPoi = poiType_muRes.second;
            cout << setfill('-') << setw(55) << " --- " << setfill(' ') << endl;
            cout << " *** POIS scheme = " << poi << endl;

            for (const pair<std::string, MuResult>& mu_muRes : resPerPoi) {
                std::string mu  = mu_muRes.first;
                MuResult    res = mu_muRes.second;

                res.obs.FinaliseComputation();
                res.exp.FinaliseComputation();

                cout << " ****************************" << endl;
                cout << " * mu: " << mu << endl;
                cout << " *             " << setw(10) << " value "     << "           stat " << setw(10) << "       syst" << setw(15) << "    value" << setw(20) << "    total error"                               << endl;
                cout << "  * observed = " << setw(10) << res.obs.value << " +- " << setw(10) << res.obs.error_stat  << " +- " << setw(10) << res.obs.error_syst
                     << " = "             << setw(10) << res.obs.value << " +- " << setw(10) << res.obs.error_total << endl;

                cout << "  * expected = " << setw(10) << res.exp.value << " +- " << setw(10) << res.exp.error_stat  << " +- " << setw(10) << res.exp.error_syst
                     << " = "             << setw(10) << res.exp.value << " +- " << setw(10) << res.exp.error_total << endl;
            } // mu's
        } // study

    }
    //**************************************************************************************************
    void FitManager::WriteObtainedPoisFromStorageToFile() {
        ofstream fs;

        for (const pair<POIs, std::map<std::string, MuResult>>& poiType_muRes : muResults_f_POItype_f_mu) {
            POIs poi       = poiType_muRes.first;
            auto resPerPoi = poiType_muRes.second;

            string filename = "results/" + to_string( (int) poi ) + ".txt"; //to_string( static_cast<int>(poi)) + ".txt";
            fs.open(filename, std::ios_base::out);

            fs << setprecision(8);
            fs << setw(10) << "mu" << setw(15) << " +- stat" << setw(15) << " +- syst" << setw(15) << " = " << setw(15) << "mu" << setw(15) << " +- total" << endl;

            for (const pair<std::string, MuResult>& mu_muRes : resPerPoi) {
                std::string mu  = mu_muRes.first;
                MuResult    res = mu_muRes.second;

                res.obs.FinaliseComputation();
                res.exp.FinaliseComputation();

                fs << " ****************************" << endl;
                fs << "  * mu: " << mu << endl;
                fs << "  *            " << setw(10) << " value "     << "        stat "    << "          syst "               << endl;

                fs << "  * observed = " << setw(10) << res.obs.value << " +- " << setw(10) << res.obs.error_stat << " +- " << setw(10) << res.obs.error_syst <<
                   " = "                 << setw(10) << res.obs.value << " +- " << setw(10) << res.obs.error_total <<endl;

                fs << "  * expected = " << setw(10) << res.exp.value << " +- " << setw(10) << res.exp.error_stat << " +- " << setw(10) << res.exp.error_syst <<
                   " = "                 << setw(10) << res.exp.value << " +- " << setw(10) << res.exp.error_total <<endl;

            } // mu's
            fs.close();
        } // study
    }
    //**************************************************************************************************
    void FitManager::Test() {
        cout << " ***** TEST FitManager" << endl;

        if (dsPerCat_.size() == 0)
            throw std::runtime_error("ERROR: dsPerCat_ is empty");
        else
            cout << " *** dsPerCat_  - DONE - contains " << dsPerCat_.size() << " entries" << endl;


        if (catsForStudy_.size() == 0)
            throw std::runtime_error("ERROR: catsForStudy_ is empty");
        else
            cout << " *** catsForStudy_  - DONE - contains " << catsForStudy_.size() << " entries" << endl;

        if (poisToFit_.size() == 0)
            throw std::runtime_error("ERROR: poisToFit_ is empty");
        else
            cout << " *** poisToFit_  - DONE - contains " << poisToFit_.size() << " entries" << endl;

        if (rawListDataPerCat_->GetSize() == 0)
            throw std::runtime_error("ERROR: rawListDataPerCat_ is empty");
        else
            cout << " *** rawListDataPerCat_  - DONE - contains " << rawListDataPerCat_->GetSize() << " entries" << endl;

        cout << " *** CombinedPdf:";
        combinedPdf_->Print();

        cout << " *** CombinedDS:";
        dataSetTotal_->Print();

        cout << " ****** TEST has been successfully passed" << endl;

    } // test
    //**************************************************************************************************
    void FitManager::CreateAsimovDataSet(const std::vector<std::string>& cats) {

        cout << "[CreateAsimovDataSet] generate a new asimov ds for " << cats.size() << " categories" << endl;
        asimovDSperCat = new std::map<std::string, RooDataSet*>;
        cout << "[CreateAsimovDataSet] map <cat, RooDataSet*> is initialised" << endl;
        RooArgSet argsetDV;
        RooArgSet argsetDVweighted;

        /*if (false) {
        //if (dsPerCat_.empty()) {
          cout << "[CreateAsimovDataSet] extract varialbes from closure to get ds per cat" << endl;
          SetUpCats(cats);
          ExtractSelectedVariablesToClosure();
          }*/
        //cout << "[CreateAsimovDataSet] done" << endl;

        for (const string& cat : cats) {
            //cout << " * add var for cat |" << cat << "|" << "to the argset" << endl;
            RooRealVar* var = GetObservable(cat);
            //cout << "getobsrvablse - done" << endl;
            argsetDV.add(*var);
            //cout << " add obs to argset - done" << endl;
            argsetDVweighted.add(*var);
            //cout << " add obs to weighted argset - done" << endl;
        } //cats

        cout << "[CreateAsimovDataSet] argsets have been created" << endl;
        float valuePdf = 0;
        RooRealVar* weight   = new RooRealVar("weight", "weight", 0, 1000000);
        argsetDVweighted.add(*weight);

        //argsetDV.Print("v");
        cout << " ####" << endl;

        //RooDataSet* dsAsimov;// = new RooDataSet("dsAsimov", "dsAsimov", argsetDV, RooFit::WeightVar(*weight)); // add a weight!

        cout << " ---------- add entries for all categories to the Asimov DS" << endl;
        // add an entry to Asimov for each category

        RooRealVar* nBkg;
        RooRealVar* var;
        RooAbsPdf*  pdf;

        float n_bkg, n_sig;
        float n_generate;

        cout << "[CreateAsimovDataSet] loop over categories to add entries" << endl;
        // we need to modify cat's name by adding "_HGam", so that we don't use a reference
        for (string cat : cats) {
            //cat += "_HGam";
            cout << " ------------------------------------------------------------ \n " << cat << endl;
            ///cout << " ---- cat:" << cat << endl;
            nBkg = GetNbkgGivenCategory(cat);
            cout << " -- nbkg -> ";
            nBkg->Print();

            var  = GetObservable(cat);
            cout << " -- var -> ";
            var->Print();

            pdf  = GetPdfModelGivenCategory(cat);
            //cout << " -- pdf -> ";
            //pdf->Print();
            // TO THINK: which pdf to extract? Maybe pdfSB?

            n_bkg = nBkg->getVal();
            cout << " -- n_kbg = " << n_bkg << endl;
            ///cout << " - nbkg = " << n_bkg;
            //float n_entries_tot = dsPerCat_[cat]->sumEntries();
            //n_sig = (n_entries_tot - n_bkg) / mu_found;

            //cout << "check if dspercat contains " << cat << ": " << boolalpha;
            //cout << (dsPerCat_.find(cat) != dsPerCat_.end()) << endl;

            //cout << "is pointer of ds empty? " << (dsPerCat_.at(cat) == nullptr) << endl;
            //cout << "the ds itself" << endl;
            //dsPerCat_[cat]->Print("v");

            n_generate = dsPerCat_[cat + "_HGam"]->sumEntries();
            cout << " ---- generate: " << n_generate << endl;
            ///cout << " - ngenerate = " << n_generate << endl;

            //var->setBins(55);
            RooArgSet argsetVarThisCat(*var);
            RooArgSet argsetVarThisCatAndWeight(*var);
            argsetVarThisCatAndWeight.add(*weight);

            //const char* name = ( string("ds_") + cat ).c_str();
            string name = "ds_" + cat;
            cout << " create an asimov ds for the category " << setw(30) << cat <<  " with the name " << name << endl;
            asimovDSperCat->operator[](cat) = new RooDataSet(name.c_str(), name.c_str(), argsetVarThisCatAndWeight, RooFit::WeightVar(*weight));

            size_t nb_bins = var->getBins();
            ////cout << " -------- loop over bins:" << endl;
            for (size_t idx_bin = 0; idx_bin < nb_bins; ++idx_bin) {
                //cout << " - " << idx_bin << " --> ";
                var->setBin(idx_bin);
                //RooArgSet asMyy(*var);

                //valuePdf = combinedPdf->getVal(argsetVarThisCatAndWeight);
                ////valuePdf = combinedPdf->getVal(argsetVarThisCat);
                ////valuePdf = combinedPdf->getVal(asMyy);
                valuePdf = pdf->getVal(argsetVarThisCatAndWeight);
                valuePdf *= var->getBinWidth(0);
                ////cout << " valuepdf * (n_generate) = " << setw(15) << valuePdf << " * " << setw(8) << n_generate << " = " << valuePdf * n_generate << endl;
                //cout << " valuepdf * nBkg = " << setw(15) << valuePdf << " * " << setw(8) << nBkg->getVal() << " = " << valuePdf * nBkg->getVal() << endl;
                //cout << " nBkg = " << nBkg->getVal() << endl;

                ////weight->setVal(valuePdf * (n_generate));
                //asimovDSperCat[cat]->add(argsetVarThisCatAndWeight, weight->getVal());
                ////asimovDSperCat->operator[](cat)->add(argsetVarThisCatAndWeight, valuePdf * n_generate);
                asimovDSperCat->operator[](cat)->add(argsetVarThisCat, valuePdf * n_generate);
                //asimovDSperCat[cat]->SaveAs("resources/4POI.root");
            } // bins
            //asimovDSperCat[cat]->Write( ("asimovDSperCat_" + cat).c_str() );
        } //cats

        cout << " ### create an Asimov ds for the argsetDVweighted:" << endl;
        argsetDVweighted.Print("v");
        cout << " ### activate ctor" << endl;

        RooCategory* channelList = dynamic_cast<RooCategory*>( ws_->cat("channellist") );

        dsAsimov = new RooDataSet("dsAsimov", "dsAsimov",
                                  argsetDVweighted,
                                  RooFit::WeightVar(*weight),
                                  RooFit::Import( *asimovDSperCat ),
                                  RooFit::Index(*channelList)
        );

        cout << "[ASIMOV] print it:" << endl;
        dsAsimov->Print();
    }
    //**************************************************************************************************
    void FitManager::PlotAsimovDataSet(const std::vector<std::string>& cats) {
        cout << "[PlotAsimovDataSet] start" << endl;
        //shared_ptr<TCanvas> c;
        //shared_ptr<RooRealVar> var;
        //shared_ptr<RooAbsPdf>  pdf;

        TCanvas* c;
        RooRealVar* var;
        RooAbsPdf* pdf;

        RooCategory* channelList = dynamic_cast<RooCategory*>( ws_->cat("channellist") );

        for (const string& cat : cats) {
            cout << "[PlotAsimovDataset] {" << cat << "}" << endl;
            //c = make_shared<TCanvas>("c", "c", 1200, 800);
            c = new TCanvas("c", "c", 1200, 800);
            var = GetObservable(cat);
            pdf = GetPdfModelGivenCategory(cat);
            // m_yy
            //var.reset(GetObservable(cat));
            // pdf
            //pdf.reset(GetPdfModelGivenCategory(cat));

            auto frame = var->frame(RooFit::Bins(55));

            string catExtended = cat + "_HGam";

            dataSetTotal_->plotOn(frame,
                                  RooFit::Cut( (("channellist==channellist::") + catExtended).c_str()),
                                  RooFit::SumW2Error(1),
                                  RooFit::MarkerColor(kBlack),
                                  RooFit::LineColor(kBlack),
                                  RooFit::MarkerSize(2),
                                  RooFit::LineWidth(2)
            );

            combinedPdf_->plotOn(frame,
                                 RooFit::Slice(*channelList, catExtended.c_str()),
                                 RooFit::ProjWData(*channelList, *dataSetTotal_),
                                 RooFit::LineColor(kRed),
                                 RooFit::LineWidth(4)
            );

            (*asimovDSperCat)[cat]->plotOn(frame, RooFit::MarkerColor(kBlue), RooFit::LineColor(kBlue), RooFit::MarkerSize(1), RooFit::LineWidth(1));
            pdf->plotOn(frame, RooFit::LineColor(kGreen + 1), RooFit::LineWidth(3));

            frame->Draw();
            c->SaveAs(("figures/Asimov" + cat + ".png").c_str());
        } // cats
    }
    //**************************************************************************************************
    void FitManager::ComputeSensitivityGivenPOI(const string& poi1, const string& poi2, const vector<string>& cats) {
        gStyle->SetOptStat(0000000);

        cout << " #[INFO]: {ComputeSensitiity for: " << poi1 << " and " << poi2 << "} ";
        if (isStatOnly) {
            cout << " in stat only regime " << endl;
            SetAllNuisanceParamsConst();
        }
        else {
            cout << " in stat + syst regime " << endl;
            SetAllNuisanceParamsFloat();
        }

        cout << " #[STUDY]: float pois" << endl;
        FloatPOIs();

        RooSimultaneous* combinedPdf;
        RooDataSet*      dataSetTotal;

        //if (cats.size() != channelList_->size()) {
        //combinedPdf  = CreateCombinedPdfFromGivenCategories(cats);
        //dataSetTotal = CreateCombinedDataSetFromGivenCategories(cats);
        //}
        //else {
        combinedPdf  = combinedPdf_;
        dataSetTotal = dataSetTotal_;
        //}

        MuConfig mu1(poi1);
        MuConfig mu2(poi2);

        shared_ptr<RooFitResult> obsFitRes = ComputeObservedValuesGivenPOIs(mu1, mu2, combinedPdf, dataSetTotal);

        double nllObs = obsFitRes->minNll();
        cout << " #[INFO] {minNll of obs} = " << nllObs << endl;
        cout << " #[INFO] {correleation between: " << poi1 << " & " << poi2 << "}: " << obsFitRes->correlation(poi1.c_str(), poi2.c_str()) << endl;
        shared_ptr<TH2D> correlationHist = make_shared<TH2D>(* (TH2D*)(obsFitRes->correlationHist()));
        cout << " #[INFO]: " << poi1 << " after fitting: " << GetParameterValue(poi1) << " +- "  <<  GetParameterError(poi1)  << endl;
        cout << " #[INFO]: " << poi2 << " after fitting: " << GetParameterValue(poi2) << " +- "  <<  GetParameterError(poi2)  << endl;

        SetGlobalObservablesToValueFoundInFit();

        cout << "save snapshot with values found in fit: " << endl;
        cout << "[NP]:" << endl;
        nuisanceParameters_->Print("v");
        cout << "[GlobalObs]:" << endl;
        globalObservables_->Print("v");
        cout << "[Obs]:" << endl;
        observables_->Print("v");
        cout << "[POIs]:" << endl;
        POIs_->Print("v");

        RooArgSet as;
        as.add(*nuisanceParameters_);
        as.add(*globalObservables_);
        as.add(*observables_);
        as.add(*POIs_);

        ws_->saveSnapshot("source/Snapshot_2D_muggF_muVBF.root", as);
        return;

        RooRealVar* muVar1 = GetVar(poi1);
        RooRealVar* muVar2 = GetVar(poi2);
        RooArgList muArgList;
        muArgList.add(*muVar1);
        muArgList.add(*muVar2);
        cout << " #[INFO]: create arglist for 2 mus:" << endl;
        muVar1->Print();
        muVar2->Print();
        cout << " --- the list itself:" << endl;
        muArgList.Print();
        //shared_ptr<TH2D> h_mu_cor = make_shared<TH2D>((TH2D)obsFitRes->conditionalCovarianceMatrix(muArgList));
        shared_ptr<TH2D> h_mu_cor = make_shared<TH2D>((TH2D)obsFitRes->reducedCovarianceMatrix(muArgList));

        h_mu_cor->GetXaxis()->SetBinLabel(1, muVar1->GetName());
        h_mu_cor->GetXaxis()->SetBinLabel(2, muVar2->GetName());

        h_mu_cor->GetYaxis()->SetBinLabel(1, muVar2->GetName());
        h_mu_cor->GetYaxis()->SetBinLabel(2, muVar1->GetName());


        shared_ptr<TCanvas> cCor = make_shared<TCanvas>("c_cor", "c_cor", 16000, 16000);


        correlationHist->GetXaxis()->SetLabelSize(0.015);
        correlationHist->GetYaxis()->SetLabelSize(0.015);

        gStyle->SetPaintTextFormat("2.1f");
        gStyle->SetTextSize(0.000001);
        gStyle->SetMarkerSize(0.00001);

        gPad->SetLeftMargin(0.15);
        gPad->SetBottomMargin(0.15);
        gPad->SetRightMargin(0.15);
        gPad->SetTopMargin(0.15);

        //h_mu_cor->Draw("colz text");
        correlationHist->Draw("colz text");
        cCor->SaveAs("figures/correlation_dyn_range_STAT_SYST_full.png");
        //cCor->SaveAs("figures/test_mu_cor_fitTo.png");

        double val1 = GetParameterValue(poi1);
        double val2 = GetParameterValue(poi2);

        double err1 = GetParameterError(poi1);
        double err2 = GetParameterError(poi2);

        if (err1 >= 0.5 * val1) err1 = 2 * val1 / 3;
        if (err2 >= 0.5 * val2) err2 = 2 * val2 / 3;

        size_t nb_bins = 7;
        size_t nbSigmasForContour = 3.0;

        double mu1_obs = val1;
        double mu2_obs = val2;
        double mu1_test, mu2_test;
        //double d_mu1 = 4 * err1 / (nb_bins - 1);
        //double d_mu2 = 4 * err2 / (nb_bins - 1);
        double d_mu1 = nbSigmasForContour * 2 * err1 / (nb_bins - 1);
        double d_mu2 = nbSigmasForContour * 2 * err2 / (nb_bins - 1);

        /*double mu1TestLeft  = mu1_obs - 2 * err1;
        double mu1TestRight = mu1_obs + 2 * err1;

        double mu2TestLeft  = mu2_obs - 2 * err2;
        double mu2TestRight = mu2_obs + 2 * err2;*/

        double mu1TestLeft  = mu1_obs - nbSigmasForContour * err1;
        double mu1TestRight = mu1_obs + nbSigmasForContour * err1;

        double mu2TestLeft  = mu2_obs - nbSigmasForContour * err2;
        double mu2TestRight = mu2_obs + nbSigmasForContour * err2;

        // TODO: change this temporal shit
        //d_mu1 = 0.15;
        //d_mu2 = 0.15;
        //d_mu1 = 0.0250;
        //d_mu2 = 0.0250;

        //int nb_bins = 6;

        //d_mu1 = 1.0 / nb_bins;
        //d_mu2 = 1.0 / nb_bins;

        //d_mu1 = 4 * err1 / (nb_bins - 1);
        //d_mu2 = 4 * err2 / (nb_bins - 1);

        //if (2 * err1 >= mu1_obs) d_mu1 = 2 * mu1_obs / (nb_bins - 1);
        //if (2 * err2 >= mu2_obs) d_mu2 = 2 * mu2_obs / (nb_bins - 1);

#define DEBUG

#ifndef PRINT
#ifdef  DEBUG
        const char* s = 0;
#define PRINT(var) s = #var; \
          std::cout << " * " << s << " = " << var << "\n";
#endif // debug
#else
#define PRINT(var);
#endif //print

        PRINT(mu1TestLeft);
        PRINT(mu1TestRight);
        PRINT(mu2TestLeft);
        PRINT(mu2TestRight);

        PRINT(d_mu1);
        PRINT(d_mu2);

        //if (mu1_obs < 0.8) mu1_obs = 0.8;
        //if (mu2_obs < 0.8) mu2_obs = 0.8;
        ////////////////////////////////////
        //if (d_mu1 >= mu1_obs )

        //cout << " d_mu_1 = " << d_mu1 << endl;
        //cout << " d_mu_2 = " << d_mu2 << endl;
        //TH2D* hmu = new TH2D("mus", "mus", 4, 0, 4, 4, 0, 4);
        ////TH2D* hmu = new TH2D("mus", "mus", nb_bins, 0.0, 2.0, nb_bins, 0.0, 2.0);

        double mu1HistEdgeLeft  = mu1TestLeft  - d_mu1 / 2;
        double mu1HistEdgeRight = mu1TestRight + d_mu1 / 2;
        double mu2HistEdgeLeft  = mu2TestLeft  - d_mu2 / 2;
        double mu2HistEdgeRight = mu2TestRight + d_mu2 / 2;

        PRINT(mu1HistEdgeLeft);
        PRINT(mu1HistEdgeRight);
        PRINT(mu2HistEdgeLeft);
        PRINT(mu2HistEdgeRight);

        bool doUseFixedRange = false;
        if (doUseFixedRange) {
            mu1HistEdgeLeft  = 0.7;
            mu1HistEdgeRight = 1.4;

            mu2HistEdgeLeft  = 0.7;
            mu2HistEdgeRight = 2.0;
        }

        TH2D* hmu = new TH2D("mus", "Sensitivity", nb_bins, mu1HistEdgeLeft, mu1HistEdgeRight, nb_bins, mu2HistEdgeLeft, mu2HistEdgeRight);

        shared_ptr<TCanvas> cMu = make_shared<TCanvas>("c_mu", "c_mu", 2000, 2000);
        shared_ptr<TLegend> leg = make_shared<TLegend>(0.8, 0.8, 0.95, 0.95);

        shared_ptr<TEllipse> obsPoint = make_shared<TEllipse>(mu1_obs, mu2_obs, 0.01, 0.01);
        obsPoint->SetFillColor(kRed);
        obsPoint->SetLineColor(kRed);

        shared_ptr<TEllipse> SMPoint = make_shared<TEllipse>(1, 1, 0.01, 0.01);
        SMPoint->SetFillColor(kBlue);
        SMPoint->SetLineColor(kBlue);

        leg->AddEntry(obsPoint.get(), "observed");
        leg->AddEntry(SMPoint.get(), "SM");

        gPad->SetRightMargin(0.10);
        gPad->SetLeftMargin(0.10);
        gPad->SetTopMargin(0.10);
        gPad->SetBottomMargin(0.10);

        double contours[2] = {2.3, 6.0};
        hmu->SetContour(2, contours);

        const string title1 = "#mu_{" + poi1.substr(3, poi1.size()) + "}";
        const string title2 = "#mu_{" + poi2.substr(3, poi2.size()) + "}";

        hmu->GetXaxis()->SetTitle(title1.c_str());
        hmu->GetYaxis()->SetTitle(title2.c_str());

        gStyle->SetPaintTextFormat("3.2f");
        gStyle->SetMarkerSize(1.2);
        gStyle->SetTextSize(1.2);

        /*size_t idx_x = 1;
        for (int mu_1_step = -nb_bins/2; mu_1_step < nb_bins/2; mu_1_step++) {
          size_t idx_y = 1;
          for (int mu_2_step = -nb_bins/2; mu_2_step < nb_bins/2; mu_2_step++) {
        //mu1_test = mu1_obs + d_mu1 * mu_1_step;
        //mu2_test = mu2_obs + d_mu2 * mu_2_step;
        mu1_test = hmu->GetXaxis()->GetBinCenter(idx_x);
        mu2_test = hmu->GetYaxis()->GetBinCenter(idx_y);*/


        for (size_t idx_x = 0; idx_x < nb_bins; ++idx_x) {
            for (size_t idx_y = 0; idx_y < nb_bins; ++idx_y) {
                //mu1_test = mu1TestLeft + idx_x * d_mu1;
                //mu2_test = mu2TestLeft + idx_y * d_mu2;

                mu1_test = hmu->GetXaxis()->GetBinCenter(idx_x + 1);
                mu2_test = hmu->GetYaxis()->GetBinCenter(idx_y + 1);

                cout << " #[INFO]: compute nll for mu1 = " << mu1_test << " and mu2 = " << mu2_test << endl;
                cout << " idx_x = " << idx_x << " and idx_y = " << idx_y << endl;
                FixPois({poi1, poi2});
                cout << " --- pois are fixed" << endl;
                SetGivenPOItoValue({{poi1, mu1_test}, {poi2, mu2_test}});
                shared_ptr<RooFitResult> testFitRes = make_shared<RooFitResult>(*(combinedPdf->fitTo(*dataSetTotal,
                                                                                                     RooFit::SumW2Error(false),
                                                                                                     RooFit::Save(),
                                                                                                     RooFit::BatchMode(true),
                                                                                                     RooFit::Offset(true)
                )));
                cout << "set [" << idx_x << "][" << idx_y << "] = " << testFitRes->minNll() - nllObs << endl;
                hmu->Fill(mu1_test, mu2_test, testFitRes->minNll() - nllObs);
                //hmu->SetBinContent(idx_x, idx_y, testFitRes->minNll() - nllObs);
                //cout << " nll(mu1 = " << mu1_test << " and mu2 = " << mu2_test << ") = " << hmu->GetBinContent(idx_x, idx_y);

                //////////////
                hmu->Draw("col text");
                hmu->Draw("cont3 same");
                hmu->SetLineColor(kRed);
                hmu->SetLineWidth(3);

                obsPoint->Draw("same");
                SMPoint->Draw("same");

                leg->Draw("same");

                cMu->SaveAs("figures/cMu_dyn_range_STAT_SYST_full.png");
                //cMu->SaveAs("figures/test_cmu_fitTo.png");
                hmu->SaveAs("figures/hmu_dyn_range_STAT_SYST_full.root");
                ////////////
                cout << " # go to the next idx_y" << endl;
            }
            cout << "# go to the next idx_x" << endl;
        }

        cout << " #[INFO]: content of hmu: " << endl;
        for (size_t idx_x = 1; idx_x <= nb_bins; idx_x++) {
            for (size_t idx_y = 1; idx_y <= nb_bins; idx_y++) {
                cout << hmu->GetBinContent(idx_x, idx_y) << " |  ";
            }
            cout << endl;
        }

        //hmu->SaveAs("figures/hmu_Adjusted_STAT_SYST.root");

        /*cout << " #[INFO]: boarders of hmu: " << endl;
        for (size_t idx_x = 0; idx_x < nb_bins; idx_x++) {
          for (size_t idx_y = 0; idx_y < nb_bins; idx_y++) {
        cout << "[" << mu1TestLeft + idx_x * d_mu1 << ", " << mu2TestLeft + idx_y * d_mu2 << "] ";
          }
          cout << endl;
          }*/
        /*RooCategory* channelList = dynamic_cast<RooCategory*>( ws_->cat("channellist") );

        cout << " #[INFO]: fit data to find observed value of the pair" << ednl;
        RooFitResult* fitResult = combinedPdf->fitTo(*dsAsimov, RooFit::Save(), RooFit::SumW2Error(false));*/

    }
    //**************************************************************************************************
    shared_ptr<RooFitResult> FitManager::ComputeObservedValuesGivenPOIs(MuConfig& mu1, MuConfig& mu2, RooAbsPdf* pdf, RooAbsData* ds) {
        cout << " #[INFO] {FitManager::ComputeObservedValuesGivenPois} for " << mu1.name << " and " << mu2.name << endl;
        RooFitResult* fitResult = pdf->fitTo(*ds,
                                             RooFit::Save(),
                                             RooFit::SumW2Error(false),
                                             RooFit::BatchMode(true),
                                             RooFit::Offset(true)
        );
        return make_shared<RooFitResult>(*fitResult);
    }
    //**************************************************************************************************
    void FitManager::ComputeSensitivityGivenPOI(const string& poi1, const string& poi2, size_t nb_cats) {
        if (nb_cats == 0) nb_cats = channelList_->size();
        cout << " #[INFO] {CompputeSensitivity for the first: " << nb_cats << " categories}" << endl;
        std::vector<std::string> cats = GetListOfFirstNcategories(nb_cats);
        ComputeSensitivityGivenPOI(poi1, poi2, move(cats));
    }
    //**************************************************************************************************
    void FitManager::_PerformOffsetNllTest() {
        /*auto combinedPdf  = CreateCombinedPdfFromGivenCategories(cats);
        auto dataSetTotal = CreateCombinedDataSetFromGivenCategories(cats);

        RooAbsReal* nll = combeindPdf->CreateNll(*dataSetTotal, CloneData(kFALSE),GlobalObservables(*argset_global_observables),ConditionalObservables(*argset_conditionalobservables));*/
    }
    //**************************************************************************************************
    double FitManager::ComputeNllAnd1Mu() {
        RooFitResult* res = combinedPdf_->fitTo(*dataSetTotal_,
                                                RooFit::SumW2Error(false),
                                                RooFit::BatchMode(true),
                                                RooFit::Offset(true),
                                                RooFit::Save(true)
        );

        return res->minNll();
    }
    //**************************************************************************************************
    double FitManager::ComputeNllGivenMuGivenValue(const string& muName, double muVal) {
        RooRealVar* muVar1 = GetVar(muName);

        cout << "[Worker]: status at the beginning of {ComputeNllGivenValue}:" << endl;
        cout << "[NP]:" << endl;
        nuisanceParameters_->Print("v");
        cout << "[GlobalObs]:" << endl;
        globalObservables_->Print("v");
        cout << "[Obs]:" << endl;
        observables_->Print("v");
        cout << "[POIs]:" << endl;
        POIs_->Print("v");

        FloatPOIs();
        SetGivenPOItoValue({muName, muVal});
        FixPois(muName);

        if (isStatOnly) {
            cout << "[Worker] stat only -> need to load np" << endl;
            LoadNPvalues("source/NPvalues.txt");
            SetAllNuisanceParamsConst();
            /*cout << "construct filename: ";
            string fileNameNP = "source/np_snapshot_" + muName + ".txt";
            cout << fileNameNP << endl;
            bool isRead = nuisanceParameters_->readFromFile(fileNameNP.c_str());
            if (!isRead)
          throw std::runtime_error("fileneme " + fileNameNP + " is not read");

            cout << "[Worker] before setting NP to const" << endl;
            SetGlobalObservablesToValueFoundInFit();
            SetAllNuisanceParamsConst();
            cout << "after" << endl;*/
        }
        else {
            cout << "[Worker] full  -> no need to load np" << endl;
            SetAllNuisanceParamsFloat();
        }

        cout << "[Worker]: status before fit:" << endl;
        cout << "[NP]:" << endl;
        nuisanceParameters_->Print("v");
        cout << "[GlobalObs]:" << endl;
        globalObservables_->Print("v");
        cout << "[Obs]:" << endl;
        observables_->Print("v");
        cout << "[POIs]:" << endl;
        POIs_->Print("v");

        cout << "[Worker1D]: before fit, POIs:" << endl;;
        PrintPOIs();
        /*shared_ptr<RooFitResult> testFitRes = make_shared<RooFitResult>(*(combinedPdf_->fitTo(*dataSetTotal_,
                                                  RooFit::SumW2Error(false),
                                                  RooFit::Save(),
                                                  RooFit::BatchMode(true),
                                                  RooFit::Offset(true)
                                                  )));*/

        cout << "[Worker] before creating nll" << endl;
        auto nll = CreateNll(*dataSetTotal_,
                             *combinedPdf_);
        cout << "[Worker] nll is created" << endl;
        cout << "[Worker] Minimize..." << endl;
        auto testFitRes = Minimize(*nll);

        cout << "[Worker]: status after fit:" << endl;
        cout << "[NP]:" << endl;
        nuisanceParameters_->Print("v");
        cout << "[GlobalObs]:" << endl;
        globalObservables_->Print("v");
        cout << "[Obs]:" << endl;
        observables_->Print("v");
        cout << "[POIs]:" << endl;
        POIs_->Print("v");

        cout << "[WorkerD]: after fit, POIs:" << endl;
        PrintPOIs();

        string statFlag;
        if (isStatOnly)
            statFlag = "stat";
        else
            statFlag = "full";

        ofstream fsRes("results/1D/nll_" + statFlag + "_" + muName + "_" +  to_string(muVal) + ".txt", std::ios_base::out);
        cout << "[Worker]{ComputeNllGivenMuGivenValue}: submit a job for " << muName
             << " at " << muVal
             << " for " << statFlag << " stat regime " << endl;
        //fsRes << "nll at " << muVal << ": " << setprecision(9) << testFitRes->minNll() << endl;
        fsRes << "nll at " << muVal << ": " << setprecision(9) << nll->getVal() << endl;
        if (isStatOnly)
            fsRes << "stat" << endl;
        else
            fsRes << "full" << endl;

        fsRes.close();

        cout << "[RESULT] for " << muName << " at " << to_string(muVal) << " for " << statFlag << " stat regime:"
             //<<  setprecision(9) << testFitRes->minNll();
             <<  setprecision(9) << nll->getVal() ;

        //return testFitRes->minNll();
        return nll->getVal();
    };
    //**************************************************************************************************
    void FitManager::RunWorker1DSensitivity(const string& mu, size_t workerId) {
        using std::string;
        using std::map;
        using std::cout;
        using std::endl;
        // const static std::array<string, 4> mus = {"mu_ggF", "mu_VBF", "mu_VH", "mu_top"};

        //const string muThisWorker = mus[muNb];

        //double muGGFcental       = 1.03453;
        //double deltaMuGGFcentral = 0.139592;

        // TODO: parse them automatically
        map<string, double> muCentralValues;
        map<string, double> muCentralErrors;

        isStatOnly = false;


        /* if (isStatOnly) {
          muCentralValues = {
        {"mu_ggF", 1.0645},
        {"mu_VBF", 1.23891},
        {"mu_WH",  1.64891},
        {"mu_ZH",  -0.509766},
        {"mu_ttH", 0.878991},
        {"mu_tH",  3.07202}
          };

          muCentralErrors = {
        {"mu_ggF", 0.0835572},
        {"mu_VBF", 0.184354},
        {"mu_WH",  0.52639},
        {"mu_ZH",  0.49294},
        {"mu_ttH", 0.28543},
        {"mu_tH",  3.1011}
          };
          }*/
        //else {
        muCentralValues = {
                {"mu_ggF", 1.04445},
                {"mu_VBF", 1.19914},
                {"mu_WH",  1.51078},
                {"mu_ZH",  -0.181714},
                {"mu_tH",  3.1228},
                {"mu_ttH", 0.877804},
        };

        muCentralErrors = {
                {"mu_ggF", 0.128564},
                {"mu_VBF", 0.278759},
                {"mu_WH",  0.529406},
                {"mu_ZH",  0.515287},
                {"mu_tH",  3.30348},
                {"mu_ttH", 0.305285},
        };
        //}// else

        size_t nbWorkers = 15;
        static size_t nbPoints1Side = 0.5 * (nbWorkers - 1);
        double muCentral = muCentralValues.at(mu);
        double muError   = muCentralErrors.at(mu);

        //float nbSigmaToTest = 2.5;
        float nbSigmaToTest = 3.0;
        static double muStep       = 2 * nbSigmaToTest * muError / (nbWorkers - 1);

        double muThisWorker = muCentral - (nbPoints1Side * muStep) + workerId * muStep;

        cout << "muCentral = " << muCentral << endl;
        cout << "muError   = " << muError << endl;
        cout << "muStep = " << muStep << endl;
        cout << "muThisWorker = " << muThisWorker << endl;
        cout << "POIs before fit:" << endl;
        PrintPOIs();

        /*const static vector<double> muGGFtoTest
        {
          0.63, 0.68, 0.73, 0.78, 0.83, 0.88, 0.93, 0.98, 1.03452, 1.08, 1.13, 1.18, 1.23, 1.28, 1.33, 1.38, 1.43
          };*/

        //double muValThisWorker = muGGFtoTest[muStep];

        ComputeNllGivenMuGivenValue(mu, muThisWorker);

        //const static std::array<std::array<float, 11>, 4> muValueToTest {{
        //	{}
        //  }};

    }
    //**************************************************************************************************
    void FitManager::RunWorker2DSensitivity(const string& mu1, const string& mu2, size_t nbMu1Step, size_t nbMu2Step, bool isExpected) {
        cout << "[Worker]: run to get a value for 2D sensitivity for: {" << mu1  << " step # " << nbMu1Step
             << "}, {" << mu1 << " step # " << nbMu2Step << "}";

        if (isExpected)
            cout << " EXPECTED" << endl;
        else
            cout << " OBSERVED" << endl;

        const map<string, float> muCentralValues =  {
                {"mu_ggF", 1.04566},
                {"mu_VBF", 1.19914},
                {"mu_WH",  1.51078},
                {"mu_ZH",  -0.215825},
                {"mu_ttH", 0.88809},
                {"mu_tH",  2.50585}
        };

        const map<string, float> muCentralErrors = {
                {"mu_ggF", 0.119256},
                {"mu_VBF", 0.278759},
                {"mu_WH",  0.529406},
                {"mu_ZH",  0.564193},
                {"mu_ttH", 0.318397},
                {"mu_tH",  3.56848}
        };

        //isStatOnly = false;
        cout << "[StatRegime: stat only? " << boolalpha << isStatOnly << "]" << endl;

        cout << "[Configuration]:";

        const size_t nbPoints1D = 15;
        const size_t nbPoints1DeachDirection = 0.5 * (nbPoints1D - 1);

        double mu1Central = muCentralValues.at(mu1);
        double mu2Central = muCentralValues.at(mu2);

        double mu1Error   = muCentralErrors.at(mu1);
        double mu2Error   = muCentralErrors.at(mu2);

        float nbSigmaToTest1 = 2.5;
        float nbSigmaToTest2 = 2.5;

        double muStep1 = 2 * nbSigmaToTest1 * mu1Error / nbPoints1DeachDirection;
        double muStep2 = 2 * nbSigmaToTest2 * mu2Error / nbPoints1DeachDirection;

        double mu1ThisWorker = mu1Central - (nbPoints1DeachDirection * muStep1) + nbMu1Step * muStep1;// + muStep1 / 2;
        double mu2ThisWorker = mu2Central - (nbPoints1DeachDirection * muStep2) + nbMu2Step * muStep2;// + muStep2 / 2;

        cout << "Test " << nbPoints1D << " in  a grid line -> " <<  nbPoints1DeachDirection << " points from the center at each direction" << endl;
        cout << "nbsigmas to test: " << nbSigmaToTest1 << ", " << nbSigmaToTest2 << endl;
        cout << "Mus: "
             << mu1Central << " +- " << mu1Error << ", "
             << mu2Central << " +- " << mu2Error << endl;

        cout << "muStep1 = " << muStep1 << endl;
        cout << "muStep2 = " << muStep2 << endl;

        cout << "[Worker]: launch 2D nll computations for "
             << mu1 << " at " << mu1ThisWorker << ", "
             << mu2 << " at " << mu2ThisWorker << endl;

        ComputeNll2D(mu1, mu2, mu1ThisWorker, mu2ThisWorker, isExpected);
    }
    //**************************************************************************************************
    double FitManager::ComputeNll2D(const string& mu1, const string& mu2, double muVal1, double muVal2, bool isExpected) {
        cout << "[ComputeNll2D] for: " << mu1 << " at " << muVal1 << ", "
             << mu2 << " at " << muVal2 << " in ";

        if (isExpected)
            cout << "EXPECTED regime" << endl;
        else
            cout << "OBSERVED regime" << endl;


        RooRealVar* muVar1 = GetVar(mu1);
        RooRealVar* muVar2 = GetVar(mu2);

        cout << "[ComputeNll2D] for pois" << endl;
        //FixPois({mu1, mu2});
        //SetGivenPOItoValue({{mu1, muVal1}, {mu2, muVal2}});

        /*if (isStatOnly) {
          cout << "[Worker] stat only -> need to load snapshot with np" << endl;
          //ring fileNameNP = "source/np_snapshot_" + muName + ".txt";
          //cout << fileNameNP << endl;
          //bool isRead = nuisanceParameters_->readFromFile("source/Snapshot_2D_muggF_muVBF.root");
          /* bool isRead = ws_->loadSnapshot("source/Snapshot_2D_muggF_muVBF.root");
          if (!isRead)
          throw std::runtime_error("snapshot is not loaded");*/

        //LoadNPvalues("source/NPvalues.txt");

        //cout << "[Worker] before setting NP to const" << endl;
        //SetGlobalObservablesToValueFoundInFit();
        //SetAllNuisanceParamsConst();
        //cout << "after" << endl;
        /*}
        else {
          cout << "[Worker] full  -> no need to load np" << endl;
          // SetAllNuisanceParamsFloat();
        }*/

        cout << "[ComputeNll2D] fit" << endl;
        /*shared_ptr<RooFitResult> testFitRes = make_shared<RooFitResult>(*(combinedPdf_->fitTo(*dataSetTotal_,
                                                  RooFit::SumW2Error(false),
                                                  RooFit::Save(),
                                                  RooFit::BatchMode(true),
                                                  RooFit::Offset(true)
                                                  )));*/

        cout << "[Worker] before creating nll" << endl;

        RooAbsData* ds;

        if (isExpected) {
            cout << "[ComputeNll2D]: isExpected -> use Asimov data" << endl;
            ds = (RooDataSet*) RooStats::AsymptoticCalculator::MakeAsimovData(*dataSetTotal_,
                                                                              *modelConfig_,
                                                                              *POIs_,
                                                                              *globalObservables_
            );

            ws_->saveSnapshot("condGlobObs", *globalObservables_, kTRUE);
            ws_->loadSnapshot("condGlobObs");
            cout << "[ComputeNll2D]: after loading snapshots" << endl;
            PrintInnerParams();
        }
        else {
            cout << "[ComputeNll2D]: isObserved -> use real data" << endl;
            ds = dataSetTotal_;
        }

        FixPois({mu1, mu2});
        SetGivenPOItoValue({{mu1, muVal1}, {mu2, muVal2}});

        auto nll = CreateNll(*ds,
                             *combinedPdf_);
        cout << "[Worker] nll is created" << endl;
        cout << "[Worker] Minimize..." << endl;
        auto testFitRes = Minimize(*nll);

        cout << "[ComputeNll2D] fit is done for "
             << mu1 << " at " << muVal1 << ", "
             << mu2 << " at " << muVal2 << " -> " << setprecision(12)
             << nll->getVal() << " nll " << endl;

        string statFlag;
        if (isStatOnly)
            statFlag = "stat";
        else
            statFlag = "full";

        string dataType;
        if (isExpected)
            dataType = "expected";
        else
            dataType = "observed";

        ofstream fsRes("results/xsWS/nll_" + statFlag + "_"
                       + dataType + "_"
                       + mu1 + "_" + to_string(muVal1) + "_"
                       + mu2 + "_" + to_string(muVal2) + "_"
                       + ".txt", std::ios_base::out);


        fsRes << "nll at " << muVal1 << " " << muVal2 << ": " << setprecision(12) << nll->getVal() << endl;
        if (isStatOnly)
            fsRes << "stat" << endl;
        else
            fsRes << "full" << endl;

        if (isExpected)
            fsRes << "expected";
        else
            fsRes << "observed";

        fsRes.close();

        //cout << "[RESULT] for " << muName << " at " << to_string(muVal) << " for " << statFlag << " stat regime:"
        //	 << testFitRes->minNll();

        /*RooArgSet as;
        as.add(*nuisanceParameters_);
        as.add(*globalObservables_);
        as.add(*observables_);
        as.add(*POIs_);
        ws_->saveSnapshot("source/Snapshot_2D.root", as);*/


        return nll->getVal();
    }
    //**************************************************************************************************
    void FitManager::LoadNPvalues(const string& filename) {
        cout << "[LoadNPvalues] from " << filename << endl;
        ifstream fs(filename, ios_base::in);
        string line;
        string name, to;
        double val;

        while (!fs.is_open())
            fs.open(filename);

        cout << "file is finally opened" << endl;
        while (getline(fs, line)) {
            size_t pos = line.find("|");
            cout << " ***** dealing with line: " << line << endl;
            line = line.substr(pos + 1, line.size());
            cout << " * line after prelim cut: <" << line << ">" << endl;
            istringstream is;
            is.str(line);

            is >> name >> to >> val;

            pos = name.find("|");
            name = name.substr(0, pos);

            /*cout << " *** set " << name << " to " << val << endl;
            cout << " * param before changing: ";
            ws_->var(name.c_str())->Print();
            ws_->var(name.c_str())->setVal(val);
            cout << " * param after changing: ";
            ws_->var(name.c_str())->Print();*/

            //set ATLAS__* vars
            name = name.substr(string("RNDM__").length(), name.length());
            cout << " *** set " << name << " to " << val << endl;
            ws_->var(name.c_str())->Print();
            ws_->var(name.c_str())->setVal(val);
            cout << " * param after changing: ";
            ws_->var(name.c_str())->Print();
        }
        SetAllNuisanceParamsConst();
    }
    //**************************************************************************************************
    void FitManager::DoSignificanceStudy(const string& mu, bool statOnly) {
        cout << "[DoSignificanceStudy] {" << mu << "}";

        string statFlag;
        if (isStatOnly)
            statFlag = "stat";
        else
            statFlag = "full";

        cout << " in " << statFlag << " regime" << endl;
        //SetUpPoisToFit(stats::POIs::PROD_MODE_6_POI);
        //FloatPOIs();

        cout << "[ComputeSignificance] before fixing " << mu << " to 0:" << endl;
        cout << "[Worker]: status before fit:" << endl;
        cout << "[NP]:" << endl;
        nuisanceParameters_->Print("v");
        cout << "[GlobalObs]:" << endl;
        globalObservables_->Print("v");
        cout << "[Obs]:" << endl;
        observables_->Print("v");
        cout << "[POIs]:" << endl;
        POIs_->Print("v");

        SetGivenPOItoValue(mu, 0);
        FixPois(mu);

        cout << "[ComputeSignificance] after fixing " << mu << " to 0:" << endl;
        cout << "[Worker]: status before fit:" << endl;
        cout << "[NP]:" << endl;
        nuisanceParameters_->Print("v");
        cout << "[GlobalObs]:" << endl;
        globalObservables_->Print("v");
        cout << "[Obs]:" << endl;
        observables_->Print("v");
        cout << "[POIs]:" << endl;
        POIs_->Print("v");

        /*RooFitResult* res = combinedPdf_->fitTo(*dataSetTotal_,
                            RooFit::SumW2Error(false),
                            RooFit::BatchMode(true),
                            RooFit::Offset(true),
                            RooFit::Save(true)
                            );*/

        cout << "[Worker] before creating nll" << endl;
        auto nll = CreateNll(*dataSetTotal_,
                             *combinedPdf_);
        cout << "[Worker] nll is created" << endl;
        cout << "[Worker] Minimize..." << endl;
        auto res = Minimize(*nll);

        cout << "[ComputeSignificance] after fit:" << endl;
        cout << "[Worker]: status after fit:" << endl;
        cout << "[NP]:" << endl;
        nuisanceParameters_->Print("v");
        cout << "[GlobalObs]:" << endl;
        globalObservables_->Print("v");
        cout << "[Obs]:" << endl;
        observables_->Print("v");
        cout << "[POIs]:" << endl;
        POIs_->Print("v");

        ofstream fsRes("results/significance/Nll_" + mu + "_" + statFlag, std::ios_base::out);
        fsRes << "nll for " << mu << " fixed to: " << setprecision(9) << nll->getVal() << endl;
        fsRes << statFlag << endl;


        cout << setfill('*') << setw(50) << "*" << endl;
        cout << "[Worker]: Significance for " << mu << " " << statFlag << ":" << endl;
        cout <<  setprecision(9) << nll->getVal() << " " << statFlag << endl;
        cout << setw(50) << "*" << setfill(' ') << endl;
    }
    //**************************************************************************************************
    double FitManager::ComputeGlobalNll(bool statOnly) {
        cout << "[ComputeGlobalNll]" << endl;
        statOnly;

        string statFlag;
        if (statOnly)
            statFlag = "stat";
        else
            statFlag = "full";

        //SetUpPoisToFit(stats::POIs::PROD_MODE_6_POI);
        //FloatPOIs();

        cout << "[Worker]: status before fit:" << endl;
        cout << "[NP]:" << endl;
        nuisanceParameters_->Print("v");
        cout << "[GlobalObs]:" << endl;
        globalObservables_->Print("v");
        cout << "[Obs]:" << endl;
        observables_->Print("v");
        cout << "[POIs]:" << endl;
        POIs_->Print("v");

        /*RooFitResult* res = combinedPdf_->fitTo(*dataSetTotal_,
                            RooFit::SumW2Error(false),
                            RooFit::BatchMode(true),
                            RooFit::Offset(true),
                            RooFit::Save(true)
                            );*/

        cout << "[Worker] before creating nll" << endl;
        auto nll = CreateNll(*dataSetTotal_,
                             *combinedPdf_);
        cout << "[Worker] nll is created" << endl;
        cout << "[Worker] Minimize..." << endl;
        auto res = Minimize(*nll);

        ofstream fsRes("results/significance/GlobalObservedNll_" + to_string(poisToFit_.size()) + "_mu_" + statFlag, std::ios_base::out);
        if ( ! fsRes.is_open() ) {
            cout << "[Worker]: Global Significance for " << poisToFit_.size() << " poi: " << statFlag << ":" << endl;
            cout << setprecision(9) << nll->getVal() << " " << statFlag << endl;
            cout << setw(50) << "*" << setfill(' ') << endl;
            throw std::runtime_error("impossible to open the required dirctory");
        }

        fsRes << "Global Significance for " << poisToFit_.size() << " poi: " << setprecision(9) << nll->getVal() << " " << statFlag << endl;
        cout << setfill('*') << setw(50) << "*" << endl;
        cout << "[Worker]: Global Significance for " << poisToFit_.size() << " poi: " << statFlag << ":" << endl;
        cout << setprecision(9) << nll->getVal() << " " << statFlag << endl;
        cout << setw(50) << "*" << setfill(' ') << endl;
        return res->minNll();
    }
    //**************************************************************************************************
    RooAbsReal* FitManager::CreateNll(RooAbsData& data, RooAbsPdf& pdf) {
        TStopwatch timer;
        RooAbsReal* nll = pdf.createNLL(data,
                                        RooFit::BatchMode(true),
                                        RooFit::CloneData(false),
                //IntegrateBins(_samplingRelTol),
                                        RooFit::GlobalObservables(*globalObservables_)
                //ConditionalObservables(),
                //ExternalConstraints(*_externalConstraint)
        );
        nll->enableOffsetting(true);
        timer.Stop();
        double t_cpu_  = timer.CpuTime()/60.;
        double t_real_ = timer.RealTime()/60.;

        cout << "[FitManager] {CreateNll}, cpu time spent: " << t_cpu_ << " minutes, real time: " << t_real_ << endl;
        return nll;
    }
    //**************************************************************************************************
    shared_ptr<RooFitResult> FitManager::Minimize(RooAbsReal& nll) {
        cout << "[Minimizer] create a RooMinimizerWrapper" << endl;
        RooMinimizerWrapper minim(nll);
        cout << "[Minimizer] a RooMinimizerWrapper is created" << endl;
        //if(_errorLevel>0) minim.setErrorLevel(_errorLevel);
        minim.setStrategy( 1 );
        cout << "[Minimizer] set stratego to 1" << endl;
        //minim.setPrintLevel( _printLevel-1 );
        minim.setPrintLevel( 3 );
        //if (_printLevel < 0)
        RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
        minim.setProfile(); /* print out time */
        minim.setEps( 1E-03 / 0.001 );
        cout << "[Minimizer] set EPS to 1E-6" << endl;
        minim.setOffsetting( true );
        cout << "[Minimizer] set offsetting true" << endl;
        //if (_optConst > 0) minim.optimizeConst( _optConst );
        minim.optimizeConst( 2 );
        cout << "[Minimizer] set optimize constant 2" << endl;
        // Line suggested by Stefan, to avoid running out function calls when there are many parameters
        minim.setMaxFunctionCalls(5000 * combinedPdf_->getVariables()->getSize());

        int _status = 0;

        /*if ( _useSIMPLEX ) {
          cout << endl << "Starting fit with SIMPLEX..." << endl;
          _status += minim.simplex();
          }*/

        cout << "[Minimizer] minimizerType = Minuit2" << endl;
        minim.setMinimizerType( "Minuit2" );
        // Perform fit with MIGRAD
        _status += minim.minimize( "Minuit2" );

        /*if ( _useHESSE ) {
          cout << endl << "Starting fit with HESSE..." << endl;
          _status += minim.hesse();
          minim.save("hesse","")->Print();
          }*/

        // Copied from RooAbsPdf::fitTo()
        //if (_doSumW2==1 && minim.getNPar()>0) {
        if (false) {
            cout << endl << "Evaluating SumW2 error..." << endl <<endl;
            // Make list of RooNLLVar components of FCN
            RooArgSet* comps = nll.getComponents();
            vector<RooNLLVar*> nllComponents;
            nllComponents.reserve(comps->getSize());
            TIterator* citer = comps->createIterator();
            RooAbsArg* arg;
            while ((arg=(RooAbsArg*)citer->Next())) {
                RooNLLVar* nllComp = dynamic_cast<RooNLLVar*>(arg);
                if (!nllComp) continue;
                nllComponents.push_back(nllComp);
            }
            delete citer;
            delete comps;

            // Calculated corrected errors for weighted likelihood fits
            RooFitResult* rw = minim.save();
            for (vector<RooNLLVar*>::iterator it = nllComponents.begin(); nllComponents.end() != it; ++it) {
                (*it)->applyWeightSquared(kTRUE);
            }
            cout << "Calculating sum-of-weights-squared correction matrix for covariance matrix" << endl;
            minim.hesse();
            RooFitResult* rw2 = minim.save();
            for (vector<RooNLLVar*>::iterator it = nllComponents.begin(); nllComponents.end() != it; ++it) {
                (*it)->applyWeightSquared(kFALSE);
            }

            // Apply correction matrix
            const TMatrixDSym& matV = rw->covarianceMatrix();
            TMatrixDSym matC = rw2->covarianceMatrix();
            using ROOT::Math::CholeskyDecompGenDim;
            CholeskyDecompGenDim<Double_t> decomp(matC.GetNrows(), matC);
            if (!decomp) {
                cerr << "ERROR: Cannot apply sum-of-weights correction to covariance matrix: correction matrix calculated with weight-squared is singular" << endl;
            } else {
                // replace C by its inverse
                decomp.Invert(matC);
                // the class lies about the matrix being symmetric, so fill in the
                // part above the diagonal
                for (int i = 0; i < matC.GetNrows(); ++i)
                    for (int j = 0; j < i; ++j) matC(j, i) = matC(i, j);
                matC.Similarity(matV);
                // C now contiains V C^-1 V
                // Propagate corrected errors to parameters objects
                minim.applyCovarianceMatrix(matC);
            }

            delete rw;
            delete rw2;
        }

        /*if ( _useMINOS > 0 ) {
          cout << endl << "Starting fit with MINOS..." << endl;
          if( _useMINOS == 1){
          cout << endl << "Evaluating MINOS errors for all POIs..." << endl;
          _status += minim.minos( *_paramList );
          }
          else if( _useMINOS == 2){
          cout << endl << "Evaluating MINOS errors for all NPs..." << endl;
          _status += minim.minos( *mc->GetNuisanceParameters() );
          }
          else if( _useMINOS == 3){
          cout << endl << "Evaluating MINOS errors for both POIs and NPs..." << endl;
          _status += minim.minos( RooArgSet(*_paramList, *mc->GetNuisanceParameters()) );
          }
          else{
          cout << endl << "Unknown code for MINOS fit " << _useMINOS
          << " assume running MINOS only on POIs..." << endl;
          _status += minim.minos( *_paramList );
          }
          }*/

        //if(_saveFitResult) _result.reset(minim.save("fitResult","Fit Results"));
        unique_ptr<RooFitResult> result;
        result.reset(minim.save("fitResult","Fit Results"));

        return result;
    }
    //**************************************************************************************************
    void FitManager::ComputeCentralValuesErrors(const string& mu, bool isExpected) {
        /**************************************************************
         * Perform fit in a given stat regime with a given set of POI
         * Write in a file for each mu:
         * - mu_hat
         * - symmetric error
         * - asymmetrical errors
         *
         **************************************************************/
        cout << "[ComputeCentralValuesErrors] for {" << mu << "} in stat only? - "
             << boolalpha << isStatOnly << "\n";
        cout << "[ComputeCentralValuesErrors] before creating nll" << endl;

        RooAbsData* ds;
        if (isExpected) {
            cout << "[ComputeCentralValuesErrors]: isExpected -> use Asimov data" << endl;
            ds = (RooDataSet*) RooStats::AsymptoticCalculator::MakeAsimovData(*dataSetTotal_,
                                                                              *modelConfig_,
                                                                              *POIs_,
                                                                              *globalObservables_
            );

            ws_->saveSnapshot("condGlobObs", *globalObservables_, kTRUE);
            ws_->loadSnapshot("condGlobObs");
            cout << "[ComputeCentralValuesErrors]: after loading snapshots" << endl;
            PrintInnerParams();
        }
        else {
            cout << "[ComputeCentralValuesErrors]: isObserved -> use real data" << endl;
            ds = dataSetTotal_;
        }

        auto nll = CreateNll(*ds,
                             *combinedPdf_);
        cout << "[ComputeCentralValuesErrors] nll is created" << endl;
        cout << "[ComputeCentralValuesErrors] Minimize..." << endl;
        auto res = Minimize(*nll);
        cout << "[ComputeCentralValuesErrors] Minimizing DONE" << endl;

        if (mu != "") {
            const double muVal   = GetParameterValue(mu);
            const double muErr   = GetParameterError(mu);
            const double muErrLo = GetParameterErrorLo(mu);
            const double muErrHi = GetParameterErrorHi(mu);

            string statFlag;
            if (isStatOnly)
                statFlag = "stat";
            else
                statFlag = "full";

            ostringstream os;

            os << "[ComputeCentralValuesErrors]: osults of the scan for " << mu << ":" << endl;
            os << mu << endl;
            os << setprecision(9);
            os << "val:   " << muVal   << endl;
            os << "err:   " << muErr   << endl;
            os << "errLo: " << muErrLo << endl;
            os << "errHi: " << muErrHi << endl;

            ofstream fsRes("results/pois/" + to_string(poisToFit_.size()) + "_muScheme_" + mu + "_" + statFlag, std::ios_base::out);
            if ( ! fsRes.is_open() ) {
                throw std::runtime_error("impossible to open the required dirctory");
            }

            fsRes << os.str();
            cout  << os.str();
        } // mu != ""
        else {
            // for all available mu's

            ostringstream os;

            string statFlag;
            if (isStatOnly)
                statFlag = "stat";
            else
                statFlag = "full";

            string resType = "observed";
            if (isExpected)
                resType = "expected";

            cout << "[ComputeCentralValuesErrors] write down information on the obtained POIs values" << endl;
            for (const auto& poi : poisToFit_) {
                cout << "[ComputeCentralValuesErrors] dealing with {" << poi << "} -> ";
                const double muVal = GetParameterValue(poi);
                const double muErr = GetParameterError(poi);

                cout << muVal << " +- " << muErr << endl;

                os << "mu: " << poi << endl;
                os << setprecision(9);
                os << "val:   " << muVal   << endl;
                os << "err:   " << muErr   << endl;
                os << endl;
            } // mus
            ofstream fsRes("results/pois/simultFit_" + to_string(poisToFit_.size()) + "_" + statFlag + "_" + resType, std::ios_base::out);

            fsRes << os.str();
            cout  << os.str();
            cout << "[ComputeCentralValuesErrors] close file and leave the function" << endl;
            fsRes.close();
        }
    }
    //**************************************************************************************************
    void FitManager::ComputeNPforSM() {
        cout << "[ComputeNPforSM] set POIS to 1" << endl;
        SetPOIsToValue(1.f);
        cout << "[ComputeNPforSM] fix POIs" << endl;
        FixPois();
        cout << "[ComputeNPforSM]: status:" << endl;
        cout << "[POIs] " << endl;
        PrintPOIs();
        cout << "[NP]" << endl;
        PrintNP();
        cout << "[GlobObs]" << endl;
        PrintGlobalObservables();
        cout << "[ComputeNPforSM] fit" << endl;
        cout << "[ComputeNPforSM] before creating nll" << endl;
        auto nll = CreateNll(*dataSetTotal_,
                             *combinedPdf_);
        cout << "[ComputeNPforSM] nll is created" << endl;
        cout << "[ComputeNPforSM] Minimize..." << endl;
        auto res = Minimize(*nll);
        cout << "[ComputeNPforSM] Minimizing DONE" << endl;

        cout << "[ComputeNPforSM] found values for NP. Print everything:" << endl;
        cout << "[POIs] " << endl;
        PrintPOIs();
        cout << "[NP]" << endl;
        PrintNP();
        cout << "[GlobObs]" << endl;
        PrintGlobalObservables();

        fstream fs("NP_SM.txt", std::ios_base::out);

        for (auto& obs : *globalObservables_) {
            cout << " # set global obs: |" << obs->GetName() << "|";

            string name = string(obs->GetName());
            name = name.substr(string("RNDM__").size(), name.size() );

            double foundValue = ws_->var( name.c_str() )->getVal();
            static_cast<RooRealVar*>( obs )->setVal( foundValue  );
            cout << " to " << foundValue << endl;

            fs << name << ": " << foundValue << '\n';
        }
        cout << "[ComputeNPforSM] values are stored to: NP_SM.txt - DONE" << endl;
        fs.close();
    }
    //**************************************************************************************************
    void FitManager::ComputeExpectedNll(const string&& muToZero = "") {

        if (muToZero == "")
            cout << "[ComputeExpectedNll] {GLOBAL Mode for " << poisToFit_.size() << " POI scheme}" << endl;
        else
            cout << "[ComputeExpectedNll] {Single Mode for " << poisToFit_.size() << " POI scheme: "
                 <<  muToZero << " is fixed}" << endl;

        cout << "[ComputeExpectedNll] Load params ..." << endl;
        LoadRooRealVars("source/NP_SM.txt");
        cout << "[ComputeExpectedNll] Load params DONE" << endl;
        cout << "[ComputeExpectedNll] create an Asimov DS" << endl;

        const vector<string> cats {
                "GG2H_0J_PTH_0_10__0",
                "GG2H_0J_PTH_GT10__0",
                "GG2H_1J_PTH_0_60__0",
                "GG2H_1J_PTH_0_60__1",
                "GG2H_1J_PTH_60_120__0",
                "GG2H_1J_PTH_60_120__1",
                "GG2H_1J_PTH_120_200__0",
                "GG2H_1J_PTH_120_200__1",
                "GG2H_GE2J_MJJ_0_350_PTH_0_60__0",
                "GG2H_GE2J_MJJ_0_350_PTH_0_60__1",
                "GG2H_GE2J_MJJ_0_350_PTH_0_60__2",
                "GG2H_GE2J_MJJ_0_350_PTH_60_120__0",
                "GG2H_GE2J_MJJ_0_350_PTH_60_120__1",
                "GG2H_GE2J_MJJ_0_350_PTH_120_200__0",
                "GG2H_GE2J_MJJ_0_350_PTH_120_200__1",
                "GG2H_GE2J_MJJ_350_700_PTH_0_200__0",
                "GG2H_GE2J_MJJ_350_700_PTH_0_200__1",
                "GG2H_GE2J_MJJ_350_700_PTH_0_200__2",
                "GG2H_GE2J_MJJ_700_1000_PTH_0_200__0",
                "GG2H_GE2J_MJJ_700_1000_PTH_0_200__1",
                "GG2H_GE2J_MJJ_700_1000_PTH_0_200__2",
                "GG2H_GE2J_MJJ_GT1000_PTH_0_200__0",
                "GG2H_GE2J_MJJ_GT1000_PTH_0_200__1",
                "GG2H_GE2J_MJJ_GT1000_PTH_0_200__2",
                "GG2H_PTH_200_300__0",
                "GG2H_PTH_200_300__1",
                "GG2H_PTH_300_450__0",
                "GG2H_PTH_300_450__1",
                "GG2H_PTH_300_450__2",
                "GG2H_PTH_450_650__0",
                "GG2H_PTH_450_650__1",
                "GG2H_PTH_450_650__2",
                "GG2H_PTH_GT650__0",
                "QQ2HQQ_0J__0",
                "QQ2HQQ_0J__1",
                "QQ2HQQ_0J__2",
                "QQ2HQQ_1J__0",
                "QQ2HQQ_1J__1",
                "QQ2HQQ_1J__2",
                "QQ2HQQ_GE2J_MJJ_0_60__0",
                "QQ2HQQ_GE2J_MJJ_0_60__1",
                "QQ2HQQ_GE2J_MJJ_0_60__2",
                "QQ2HQQ_GE2J_MJJ_60_120__0",
                "QQ2HQQ_GE2J_MJJ_60_120__1",
                "QQ2HQQ_GE2J_MJJ_60_120__2",
                "QQ2HQQ_GE2J_MJJ_120_350__0",
                "QQ2HQQ_GE2J_MJJ_120_350__1",
                "QQ2HQQ_GE2J_MJJ_120_350__2",
                "QQ2HQQ_GE2J_MJJ_350_700_PTH_0_200__0",
                "QQ2HQQ_GE2J_MJJ_350_700_PTH_0_200__1",
                "QQ2HQQ_GE2J_MJJ_350_700_PTH_0_200__2",
                "QQ2HQQ_GE2J_MJJ_700_1000_PTH_0_200__0",
                "QQ2HQQ_GE2J_MJJ_700_1000_PTH_0_200__1",
                "QQ2HQQ_GE2J_MJJ_GT1000_PTH_0_200__0",
                "QQ2HQQ_GE2J_MJJ_GT1000_PTH_0_200__1",
                "QQ2HQQ_GE2J_MJJ_350_700_PTH_GT200__0",
                "QQ2HQQ_GE2J_MJJ_350_700_PTH_GT200__1",
                "QQ2HQQ_GE2J_MJJ_350_700_PTH_GT200__2",
                "QQ2HQQ_GE2J_MJJ_700_1000_PTH_GT200__0",
                "QQ2HQQ_GE2J_MJJ_700_1000_PTH_GT200__1",
                "QQ2HQQ_GE2J_MJJ_GT1000_PTH_GT200__0",
                "QQ2HQQ_GE2J_MJJ_GT1000_PTH_GT200__1",
                "QQ2HLNU_PTV_0_75__0",
                "QQ2HLNU_PTV_0_75__1",
                "QQ2HLNU_PTV_75_150__0",
                "QQ2HLNU_PTV_75_150__1",
                "QQ2HLNU_PTV_150_250__0",
                "QQ2HLNU_PTV_150_250__1",
                "QQ2HLNU_PTV_GT250__0",
                "QQ2HLNU_PTV_GT250__1",
                "QQ2HLL_PTV_0_75__0",
                "QQ2HLL_PTV_0_75__1",
                "QQ2HLL_PTV_75_150__0",
                "QQ2HLL_PTV_75_150__1",
                "QQ2HLL_PTV_150_250__0",
                "QQ2HLL_PTV_150_250__1",

                "QQ2HLL_PTV_GT250__0",

                "QQ2HNUNU_PTV_0_75__0",
                "QQ2HNUNU_PTV_0_75__1",
                "QQ2HNUNU_PTV_0_75__2",

                "QQ2HNUNU_PTV_75_150__0",
                "QQ2HNUNU_PTV_75_150__1",
                "QQ2HNUNU_PTV_75_150__2",
                "QQ2HNUNU_PTV_150_250__0",
                "QQ2HNUNU_PTV_150_250__1",
                "QQ2HNUNU_PTV_150_250__2",
                "QQ2HNUNU_PTV_GT250__0",  // index in 107 scheme // undex after merging (still 0+1, so one should add "-1")
                "QQ2HNUNU_PTV_GT250__1",
                "TTH_PTH_0_60__0",   //89  // 88
                "TTH_PTH_0_60__1",   //90  // 89
                "TTH_PTH_60_120__0", //92  // 90
                "TTH_PTH_60_120__1", //93  // 91
                "TTH_PTH_120_200__0",//95  // 92
                "TTH_PTH_120_200__1",//96  // 93
                "TTH_PTH_200_300__0",//98  // 94
                "TTH_PTH_GT300__0",  //100 // 95
                "THJB__0",  //102  // 96
                "THJB__1",  //103  // 97
                "THJB_MINUS1__0", //104  // 98
                "THW__0",      //105  // 99
                "BKG_LIKE_TOP" //107  // 100
        };

        cout << "[ComputeExpectedNll] before creating Asimov. Status of POIs:" << endl;
        PrintPOIs();

        //RooArgSet* pois

        RooDataSet* asimov = (RooDataSet*) RooStats::AsymptoticCalculator::MakeAsimovData(*dataSetTotal_,
                                                                                          *modelConfig_,
                                                                                          *POIs_,
                                                                                          *globalObservables_
        );

        cout << "[ComputeExpectedNll]  after generating Asimov" << endl;
        PrintInnerParams();
        ws_->saveSnapshot("condGlobObs", *globalObservables_, kTRUE);
        cout << "[ComputeExpectedNll]: after saving snapshots" << endl;
        PrintInnerParams();
        ws_->loadSnapshot("condGlobObs");
        cout << "[ComputeExpectedNll]: after loading snapshots" << endl;
        PrintInnerParams();

        //cout << "[ComputeExpectedNll] after creating Asimov:" << endl;
        //PrintInnerParams();

        //CreateAsimovDataSet(cats);
        //cout << "[ComputeExpectedNll] create an Asimov DS DONE" << endl;
        //PlotAsimovDataSet(cats);

        if (muToZero != "") {
            cout << "[ComputeExpectedNll] fix " << muToZero << " to zero" << endl;
            SetGivenPOItoValue(muToZero, 0);
            FixPois(muToZero);
        }

        /*cout << "[ComputeExpectedNll] before creating nll" << endl;
        cout << "[ComputeExpectedNll] before creating nll, Status of POIs:" << endl;
        PrintPOIs();
        PrintInnerParams();*/
        cout << "[ComputeExpectedNLl] create nll from Asimov:" << endl;
        //dsAsimov->Print();
        auto nll = CreateNll(*asimov,
                             *combinedPdf_
        );

        //auto nll = CreateNll(*dsAsimov, *combinedPdf_);

        cout << "[ComputeExpectedNll] nll is created" << endl;
        cout << "[ComputeExpectedNll] Minimize..." << endl;
        auto res = Minimize(*nll);
        cout << "[ComputeExpectedNll] Minimizing DONE" << endl;
        //cout << "[ComputeExpectedNll] status after:" << endl;
        //PrintInnerParams();

        string statFlag;
        if (isStatOnly)
            statFlag = "stat";
        else
            statFlag = "full";

        ofstream fsRes;

        if (muToZero == "") {
            cout << "[ComputeExpectedNll] global mode, no mus are fixed" << endl;
            fsRes.open("results/xsWS/significance/ASIMOV_Global_nll_" + statFlag + ".txt", std::ios_base::out);
            fsRes << "Asimov, Global nll:" << setprecision(12) << nll->getVal() << endl;
            cout << "[ComputeExpectedNll]: result -> " << "Asimov, Global nll:" << setprecision(12) << nll->getVal() << endl;
        }
        else {
            cout << "[ComputeExpectedNll] mu: " << muToZero << " is fixed to zero" << endl;
            fsRes.open("results/xsWS/significance/ASIMOV_"
                       + to_string(poisToFit_.size()) + "_poiScheme__fixed_" + muToZero +  "_nll_" + statFlag + ".txt", std::ios_base::out
            );
            fsRes << "Asimov, fixed " << muToZero << " nll:" << setprecision(12) << nll->getVal() << endl;
            cout << "[ComputeExpectedNll]: result -> " << "fixed:" << setprecision(12) << nll->getVal() << endl;
        }
        fsRes.close();
    }
    //**************************************************************************************************
    void FitManager::LoadRooRealVars(const string& filename) {
        cout << "[LoadRooRealVars] from " << filename << endl;
        ifstream fs(filename, std::ios_base::in);

        while (!fs.is_open())
            fs.open(filename);

        string line, name;
        string eq, pm;
        float val, err;

        static const size_t offset = string("RooRealVar::").length();

        //while (fs >> line) {
        while (getline(fs, line, '\n')) {
            //cout << " ***************************" << endl;
            //cout << " * dealing with a line:" << endl;
            //cout << line << endl;
            size_t pos = line.find(' ');
            name = line.substr(offset, pos - offset);
            //cout << "name = |" << name << "|" << endl;

            istringstream is(line.substr(pos, line.length()));

            is >> eq >> val >> pm >> err;
            //cout << "val = |" << val << "|" << endl;
            //cout << "err = |" << err << "|" << endl;

            SetVarVal(name, val);
            SetVarErr(name, err);

            if (name.find("ATLAS") != std::string::npos) {
                string nameGlobal = "RNDM__" + name;
                SetVarVal(nameGlobal, val);
                SetVarErr(nameGlobal, err);
            }
            else if (name.find("TheorySig") != std::string::npos) {
                string nameGlobal = name + "_In";
                SetVarVal(nameGlobal, val);
                SetVarErr(nameGlobal, err);
            }

        }
        cout << "[LoadRooRealVars] DONE" << endl;
    }
    //**************************************************************************************************
    // map<<float, float> FitManager::ParseNllGivenMuAndStatRegime(const string& mu, bool statOnly = false) {
// //********
    // * GetListOfFiles for given mu and stat regime
    // * Parse each file: mu_test & nll @ it
    // create map to be return
    //
    //  map<float, float> res;
    //
    // }
    //**************************************************************************************************
} // namespace stats

/*

   vector<string> GetFileList(const string& directory_adress, const string& mu) {
    vector<string> file_names;
    string unix_command = "ls " + directory_adress + " | grep config*" + mu + " > tmp.txt";

    cout << " ** in getfilelist" << endl;
    cout << " *** unix command = " << unix_command << endl;
    gSystem->Exec(unix_command.c_str());
    cout << " *** it has been executed" << endl;

    string line;
    fstream input_stream;
    input_stream.open("tmp.txt", ios::in);

    while(std::getline(input_stream, line)) {
      file_names.push_back(directory_adress + "/" + line);
    }

    gSystem->Exec("rm tmp.txt");

    return move(file_names);
  }

 */

