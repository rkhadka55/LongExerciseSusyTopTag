#define SimpleAnalyzer_cxx
#include "SimpleAnalyzer.h"

#include<map>
#include<string>
#include<iostream>

#include <TH2.h>
#include <TH1D.h>
#include <TStyle.h>
#include <TCanvas.h>

//mandatory includes to use top tagger
#include "TopTagger/TopTagger/interface/TopTagger.h"
#include "TopTagger/TopTagger/interface/TopTaggerResults.h"
#include "TopTagger/TopTagger/interface/TopTaggerUtilities.h"
#include "TopTagger/CfgParser/include/TTException.h"
#include "TopTagger/TopTagger/interface/lester_mt2_bisect.h"

void SimpleAnalyzer::InitHistos()
{
    // Declare all your histograms here, that way we can fill them for multiple chains
    my_histos.emplace("HT",new TH1D("HT","HT",60,0,3000));
    my_histos.emplace("Nt",new TH1D("Nt","Nt",5,0,5));
    my_histos.emplace("met_SB1", new TH1D("met_SB1", "met_SB1", 100, 0, 1000));

    // histograms for the background estimation
    my_histos.emplace("yield", new TH1D("yield", "yield", 6, 0, 6)); // 1 bin per search bin
    my_histos["yield"]->Sumw2();
    my_histos.emplace("weight_sq", new TH1D("weight_sq", "weight_sq", 6, 0, 6)); // 1 bin per search bin

}

void SimpleAnalyzer::Loop(double weight, int maxevents, bool isQuiet, bool isFastSim)
{
//    This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//    Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry

   if(isQuiet) asymm_mt2_lester_bisect::disableCopyrightMessage();

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
      if (!isQuiet && jentry%5000 == 0) std::cout << "At event " << jentry << std::endl;

      // ----------------------------
      // --- Apply some selection ---
      // ----------------------------

      if(!(passNoiseEventFilter && (isFastSim || passSearchTrigger) && passnJets && passdPhis 
           && passMuonVeto && passIsoTrkVeto && passEleVeto && passBJets && HT > 300))
          continue;

      // ------------------
      // --- TOP TAGGER ---
      // ------------------
          
      // Use helper function to create input list 
      // Create AK4 inputs object
      ttUtility::ConstAK4Inputs<double> AK4Inputs = ttUtility::ConstAK4Inputs<double>(
          *jetsLVec_slimmed, 
          *recoJetsBtag_slimmed,
          *qgLikelihood_slimmed
          );
    
      // Create AK8 inputs object
      ttUtility::ConstAK8Inputs<double> AK8Inputs = ttUtility::ConstAK8Inputs<double>(
          *puppiJetsLVec_slimmed,
          *puppitau1_slimmed,
          *puppitau2_slimmed,
          *puppitau3_slimmed,
          *puppisoftDropMass_slimmed,
          *puppiSubJetsLVec_slimmed 
          );
      
      // Create jets constituents list combining AK4 and AK8 jets, these are used to construct top candidates
      // The vector of input constituents can also be constructed "by hand"
      std::vector<Constituent> constituents = ttUtility::packageConstituents(AK4Inputs, AK8Inputs);

      // run the top tagger
      tt.runTagger(constituents);

      // retrieve the top tagger results object
      const TopTaggerResults& ttr = tt.getResults();
      
      //get reconstructed tops
      const std::vector<TopObject*>& tops = ttr.getTops();
      
      int ntop = tops.size();
      // Make MET into a TLorentzVector
      TLorentzVector metLV;
      metLV.SetPtEtaPhiM(met, 0, metphi, 0);
      // Compute the MT2 variable
      double mt2 = ttUtility::calculateMT2(ttr, metLV);
      
      // Compute the number of b-tagged jets
      int nb = 0;

      // different search bins
      bool SB1 = ntop>=2 && nb>=1 && mt2>=200 && met>=400;
      bool SB2 = ntop>=1 && nb>=2 && mt2>=600 && met>=400;
      bool SB3 = ntop>=2 && nb>=3 && HT>=600 && met>=350;
      bool SB4 = ntop>=2 && nb>=3 && HT>=300 && met>=500;

      //implement baseline top & MT2 requirement now that they are calculated
      if(!(ntop >= 1 && mt2 > 200)) continue;

      // -----------------------
      // --- FILL HISTOGRAMS ---
      // -----------------------
      // weight is the sample weight, corresponding to xsec*lumi/nevents_total
      // eventWeight is the per-event weight, including genlevel weights, btagging weights etc
      double total_weight = weight*eventWeight;
      my_histos["HT"]->Fill(HT, total_weight);
      my_histos["Nt"]->Fill(tops.size(), total_weight);

      my_histos["yield"]->Fill(0., total_weight);
      my_histos["weight_sq"]->Fill(0., total_weight*total_weight);
      if(SB1)
      {
          my_histos["yield"]->Fill(1., total_weight);
          my_histos["weight_sq"]->Fill(1., total_weight*total_weight);
          my_histos["met_SB1"]->Fill(met, total_weight);
      } 
      if(SB2)
      {
          my_histos["yield"]->Fill(2, total_weight);
          my_histos["weight_sq"]->Fill(2, total_weight*total_weight);
      }
      if(SB3)
      {
          my_histos["yield"]->Fill(3, total_weight);
          my_histos["weight_sq"]->Fill(3, total_weight*total_weight);
      }
      if(SB4)
      {
          my_histos["yield"]->Fill(4, total_weight);
          my_histos["weight_sq"]->Fill(4, total_weight*total_weight);
      }
   }


}

void SimpleAnalyzer::WriteHistos()
{
   for (const auto &p : my_histos) {
       p.second->Write();
   }
    
}
