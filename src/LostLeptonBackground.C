#define LostLeptonBackground_cxx
#include "TopTagger/CfgParser/interface/TTException.h"

#include <map>
#include <string>
#include <iostream>

#include <TH2.h>
#include <TH1D.h>
#include <TStyle.h>
#include <TCanvas.h>

#include "Math/VectorUtil.h"

//mandatory includes to use top tagger
#include "TopTagger/TopTagger/interface/TopTagger.h"
#include "TopTagger/TopTagger/interface/TopTaggerResults.h"
#include "TopTagger/TopTagger/interface/TopTaggerUtilities.h"
#include "TopTagger/CfgParser/include/TTException.h"

void LostLeptonBackground::InitHistos()
{
    // histograms for the background estimation
    my_histos.emplace("sr_yield", new TH1D("sr_yield", "yield", 6, 0, 6)); // 1 bin per search bin
    my_histos["sr_yield"]->Sumw2();
    my_histos.emplace("sr_weight_sq", new TH1D("sr_weight_sq", "sr_weight_sq", 6, 0, 6)); // 1 bin per search bin

    my_histos.emplace("cr_yield", new TH1D("cr_yield", "yield", 6, 0, 6)); // 1 bin per search bin
    my_histos["cr_yield"]->Sumw2();
    my_histos.emplace("cr_weight_sq", new TH1D("cr_weight_sq", "cr_weight_sq", 6, 0, 6)); // 1 bin per search bin

}

void LostLeptonBackground::Loop(double weight, int maxevents=-1, int systematics =0)
{
//    This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//    Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry

    if (fChain == 0) return;

    Long64_t nentries = fChain->GetEntriesFast();
    Long64_t nbytes = 0, nb = 0;

    // Create top tagger object
    TopTagger tt;
    tt.setCfgFile("TopTagger.cfg");

    // event loop
    for (Long64_t jentry=0; jentry<nentries; jentry++) 
    {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        if (maxevents!=-1 && jentry>=maxevents) break;
        nb = fChain->GetEntry(jentry);   
        nbytes += nb;
        if (jentry%5000 == 0)
            std::cout << "At event " << jentry << std::endl;
      
        vector<TLorentzVector> jetsLVec;
        if(systematics == -1)
        {
            for(int ijet=0; ijet<jetsLVec_slimmed->size(); ++ijet)
            {
                jetsLVec.push_back( (*jetsLVec_slimmed)[ijet] * (1 - (*recoJetsJecUnc_slimmed)[ijet]));
            }      
        }
      
        else if(systematics == 1)
        {
            for(int ijet=0; ijet<jetsLVec_slimmed->size(); ++ijet)
            {
                jetsLVec.push_back( (*jetsLVec_slimmed)[ijet] * (1 + (*recoJetsJecUnc_slimmed)[ijet]));
            }
        }
        else
        {
            //deep copy, slight waste of time
            jetsLVec = *jetsLVec_slimmed;
        }
      
        int nJet_30=0;
        int nJet_50=0;
        double HT_updated = 0.0;
        for(int nJ=0; nJ<jetsLVec.size(); ++nJ)
        {
            if(jetsLVec[nJ].Pt()>30 && jetsLVec[nJ].Eta()< 2.4)
            { 
                ++nJet_30;
            }
            //Jets greater than 50 are also > than 30
            if(jetsLVec[nJ].Pt()>50 && jetsLVec[nJ].Eta()< 2.4)
            {
                ++nJet_50;
            }
            if(jetsLVec[nJ].Pt()>20 && jetsLVec[nJ].Eta()< 2.4)
            {
                HT_updated += jetsLVec[nJ].Pt();
            }
        }

        // Make MET into a TLorentzVector
        TLorentzVector metLV;
        metLV.SetPtEtaPhiM(met, 0, metphi, 0);

        // ----------------------------
        // --- Apply some selection ---
        // ----------------------------

        bool passnJetRequirement = nJet_50>=2 && nJet_30>=4;
        bool search_region  = passNoiseEventFilter && passSearchTrigger && passnJetRequirement && passdPhis
            && passMuonVeto && passIsoTrkVeto && passEleVeto && passBJets && HT_updated > 300;
        //The control region replaces the lepton veto with an explicit selection of exactly 1 electron xor muon
        bool control_region = passNoiseEventFilter && passSearchTrigger && passnJetRequirement && passdPhis
            && ((cutMuVec->size() + cutElecVec->size()) == 1) && passBJets && HT_updated > 300;

        if(!(search_region || control_region)) continue;

        // ------------------
        // --- TOP TAGGER ---
        // ------------------
        //
        // Use helper function to create input list 
        // Create AK4 inputs object
        ttUtility::ConstAK4Inputs<double> AK4Inputs = ttUtility::ConstAK4Inputs<double>(
            jetsLVec,
            *recoJetsBtag_slimmed,
            *qgLikelihood_slimmed
            );
        AK4Inputs.addSupplamentalVector("qgPtD",    *qgPtD_slimmed);
        AK4Inputs.addSupplamentalVector("qgAxis2",  *qgAxis2_slimmed);
        AK4Inputs.addSupplamentalVector("qgMult",   *qgMult_slimmed); 
    
        // Create AK8 inputs object
        ttUtility::ConstAK8Inputs<double> AK8Inputs = ttUtility::ConstAK8Inputs<double>(
            *puppiJetsLVec_slimmed,
            *puppitau1_slimmed,
            *puppitau2_slimmed,
            *puppitau3_slimmed,
            *puppisoftDropMass_slimmed,
            *puppiSubJetsLVec_slimmed 
            );
      
        // Create jets constituents list combining AK4 and AK8 jets, these are used to construct top candiates
        // The vector of input constituents can also be constructed "by hand"
        std::vector<Constituent> constituents = ttUtility::packageConstituents(AK4Inputs, AK8Inputs);

        // run the top tagger
        tt.runTagger(constituents);

        // retrieve the top tagger results object
        const TopTaggerResults& ttr = tt.getResults();
      
        //get reconstructed tops
        const std::vector<TopObject*>& tops = ttr.getTops();
      
        int ntop = tops.size();

        double mt2 = ttUtility::calculateMT2(ttr, metLV);

        int nb = 0;
        for(int ijet=0; ijet<recoJetsBtag_slimmed->size(); ++ijet)
        {
            // b-tagged jets must satisfy pt>30, |eta| < 2.4
            if(jetsLVec[ijet].Pt() < 30 || std::abs(jetsLVec[ijet].Eta()) > 2.4) continue;
            if(recoJetsBtag_slimmed->at(ijet) > 0.8484)
                nb++;
        }

        // different search bins
        bool SB1 = ntop>=2 && nb>=1 && mt2>=200 && met>=400;
        bool SB2 = ntop>=1 && nb>=2 && mt2>=600 && met>=400;
        bool SB3 = ntop>=2 && nb>=3 && HT_updated>=600 && met>=350;
        bool SB4 = ntop>=2 && nb>=3 && HT_updated>=300 && met>=500;
        bool SB5 = ntop>=2 && nb>=3 && HT_updated>=1300 && met>=500;

        //Select MT2 and Nt cuts
        if(!(ntop>=1 && mt2>=200)) continue;
        
        // -----------------------
        // --- FILL HISTOGRAMS ---
        // -----------------------
        // weight is the sample weight, corresponding to xsec*lumi/nevents_total
        // eventWeight is the per-event weight, including genlevel weights, btagging weights etc
        double total_weight = weight*eventWeight;

        //my_histos["Mupt"]->Fill(cutMuVec->at(0).Pt(), total_weight);

        if(search_region)
        {
            my_histos["sr_yield"]->Fill(0., total_weight);
            my_histos["sr_weight_sq"]->Fill(0., total_weight*total_weight);
        }
        if(control_region)
        {
            my_histos["cr_yield"]->Fill(0., total_weight);
            my_histos["cr_weight_sq"]->Fill(0., total_weight*total_weight);
        }
        if(SB1 && search_region)
        {
            my_histos["sr_yield"]->Fill(1., total_weight);
            my_histos["sr_weight_sq"]->Fill(1., total_weight*total_weight);
        }
        if(SB1 && control_region)
        {
            my_histos["cr_yield"]->Fill(1., total_weight);
            my_histos["cr_weight_sq"]->Fill(1., total_weight*total_weight);
        }
        if(SB2 && search_region)
        {
            my_histos["sr_yield"]->Fill(2., total_weight);
            my_histos["sr_weight_sq"]->Fill(2., total_weight*total_weight);
        }
        if(SB2 && control_region)
        {
            my_histos["cr_yield"]->Fill(2., total_weight);
            my_histos["cr_weight_sq"]->Fill(2., total_weight*total_weight);
        }
        if(SB3 && search_region)
        {
            my_histos["sr_yield"]->Fill(3., total_weight);
            my_histos["sr_weight_sq"]->Fill(3., total_weight*total_weight);
        }
        if(SB3 && control_region)
        {
            my_histos["cr_yield"]->Fill(3., total_weight);
            my_histos["cr_weight_sq"]->Fill(3., total_weight*total_weight);
        }
        if(SB4 && search_region)
        {
            my_histos["sr_yield"]->Fill(4., total_weight);
            my_histos["sr_weight_sq"]->Fill(4., total_weight*total_weight);
        }
        if(SB4 && control_region)
        {
            my_histos["cr_yield"]->Fill(4., total_weight);
            my_histos["cr_weight_sq"]->Fill(4., total_weight*total_weight);
        }
        if(SB5 && search_region)
        {
            my_histos["sr_yield"]->Fill(5., total_weight);
            my_histos["sr_weight_sq"]->Fill(5., total_weight*total_weight);
        }
        if(SB5 && control_region)
        {
            my_histos["cr_yield"]->Fill(5., total_weight);
            my_histos["cr_weight_sq"]->Fill(5., total_weight*total_weight);
        }
    }


}

void LostLeptonBackground::WriteHistos()
{
    for (const auto &p : my_histos) {
        p.second->Write();
    }
    
}
