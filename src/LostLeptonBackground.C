#define LostLeptonBackground_cxx
#include "LostLeptonBackground.h"

#include<map>
#include<string>
#include<iostream>

#include <TH2.h>
#include <TH1D.h>
#include <TStyle.h>
#include <TCanvas.h>

#include "Math/QuantFuncMathCore.h"
//mandatory includes to use top tagger
#include "TopTagger/TopTagger/include/TopTagger.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"
#include "TopTagger/TopTagger/include/TopTaggerUtilities.h"
#include "TopTagger/CfgParser/include/TTException.h"

void LostLeptonBackground::InitHistos()
{
    // Declare all your histograms here, that way we can fill them for multiple chains
    my_histos.emplace("HT",new TH1D("HT","HT",60,0,3000));
    my_histos.emplace("Nt",new TH1D("Nt","Nt",5,0,5));
    my_histos.emplace("met_SB1", new TH1D("met_SB1", "met_SB1", 100, 0, 1000));

    // histograms for the background estimation
    my_histos.emplace("sr_counts", new TH1D("sr_counts", "counts", 6, 0, 6)); // 1 bin per search bin
    my_histos.emplace("sr_weight_sq", new TH1D("sr_weight_sq", "sr_weight_sq", 6, 0, 6)); // 1 bin per search bin

    my_histos.emplace("cr_counts", new TH1D("cr_counts", "counts", 6, 0, 6)); // 1 bin per search bin
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
      //std::cout<<"Trying to Read out Size"<<cutMuVec->size()<<std::endl;
      //if(cutMuVec->size()>0){
      //std::cout<<"Try to Read out PT "<< (*cutMuVec)[0].Pt()<<std::endl;
      //}
      /*
      vector<double>  *jetsLVec;
      if(systematics == -1){
	      jetsLVec->push_back(*recoJetsJecUnc_slimmed[0]);
       }      
       */ 
      // ------------------
      // --- TOP TAGGER ---
      // ------------------
      //
      // Use helper function to create input list 
      // Create AK4 inputs object
      ttUtility::ConstAK4Inputs AK4Inputs = ttUtility::ConstAK4Inputs(
          *jetsLVec_slimmed, 
          *recoJetsBtag_slimmed,
          *qgLikelihood_slimmed
          );
    
      // Create AK8 inputs object
      ttUtility::ConstAK8Inputs AK8Inputs = ttUtility::ConstAK8Inputs(
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
      
      // ----------------------------
      // --- Apply some selection ---
      // ----------------------------

      //if(!(passNoiseEventFilter && passSearchTrigger && passnJets && passdPhis 
       //    && passMuonVeto && passIsoTrkVeto && passEleVeto && passBJets))
        //  continue;

      bool search_region  = passNoiseEventFilter && passSearchTrigger && passnJets && passdPhis
           && passMuonVeto && passIsoTrkVeto && passEleVeto && passBJets;
      bool control_region = passNoiseEventFilter && passSearchTrigger && passnJets && passdPhis
                             && passIsoTrkVeto && passBJets;

      int ntop = tops.size();
      // Make MET into a TLorentzVector
      TLorentzVector metLV;
      metLV.SetPtEtaPhiM(met, 0, metphi, 0);
      double mt2 = ttUtility::calculateMT2(ttr, metLV);
      int nb = 0;
      for(int ijet=0; ijet<recoJetsBtag_slimmed->size(); ++ijet)
      {
          if(recoJetsBtag_slimmed->at(ijet) > 0.8484)
              nb++;
      }
      //std::cout<<"Read out Size "<<cutMuVec->size()<<std::endl;
      // different search bins
      bool SB1 = ntop>=2 && nb>=1 && mt2>=200 && met>=400;
      bool SB2 = ntop>=1 && nb>=2 && mt2>=600 && met>=400;
      bool SB3 = ntop>=2 && nb>=3 && HT>=600 && met>=350;
      bool SB4 = ntop>=2 && nb>=3 && HT>=300 && met>=500;
      bool SB5 = ntop>=2 && nb>=3 && HT>=1300 && met>=500;

      // -----------------------
      // --- FILL HISTOGRAMS ---
      // -----------------------
      // weight is the sample weight, corresponding to xsec*lumi/nevents_total
      // eventWeight is the per-event weight, including genlevel weights, btagging weights etc
      double total_weight = weight*eventWeight;

      //my_histos["Mupt"]->Fill(cutMuVec->at(0).Pt(), total_weight);

      if(SB1 && search_region)
      {
          my_histos["sr_counts"]->Fill(0., total_weight);
          my_histos["sr_weight_sq"]->Fill(0., total_weight*total_weight);
      }
       if(SB1 && control_region)
      {
          my_histos["cr_counts"]->Fill(0., total_weight);
          my_histos["cr_weight_sq"]->Fill(0., total_weight*total_weight);
      }
      if(SB2 && search_region)
      {
          my_histos["sr_counts"]->Fill(1., total_weight);
          my_histos["sr_weight_sq"]->Fill(1., total_weight*total_weight);
      }
       if(SB2 && control_region)
      {
          my_histos["cr_counts"]->Fill(1., total_weight);
          my_histos["cr_weight_sq"]->Fill(1., total_weight*total_weight);
      }
      if(SB3 && search_region)
      {
          my_histos["sr_counts"]->Fill(2., total_weight);
          my_histos["sr_weight_sq"]->Fill(2., total_weight*total_weight);
      }
       if(SB3 && control_region)
      {
          my_histos["cr_counts"]->Fill(2., total_weight);
          my_histos["cr_weight_sq"]->Fill(2., total_weight*total_weight);
      }
      if(SB4 && search_region)
      {
          my_histos["sr_counts"]->Fill(3., total_weight);
          my_histos["sr_weight_sq"]->Fill(3., total_weight*total_weight);
      }
       if(SB4 && control_region)
      {
          my_histos["cr_counts"]->Fill(3., total_weight);
          my_histos["cr_weight_sq"]->Fill(3., total_weight*total_weight);
      }
      if(SB5 && search_region)
      {
          my_histos["sr_counts"]->Fill(4., total_weight);
          my_histos["sr_weight_sq"]->Fill(4., total_weight*total_weight);
      }
       if(SB5 && control_region)
      {
          my_histos["cr_counts"]->Fill(4., total_weight);
          my_histos["cr_weight_sq"]->Fill(4., total_weight*total_weight);
      }
   }

     const double data_mu = my_histos["sr_counts"]->GetBinContent(3);
     const double data_mu_dn_bound = (data_mu ==0 )? 0. : (ROOT::Math::gamma_quantile((1 - 0.6827)/2, data_mu, 1.0));
     const double data_mu_up_bound = ROOT::Math::gamma_quantile_c((1 - 0.6827)/2, data_mu+1, 1.0);
     const double data_mu_dn_err = data_mu - data_mu_dn_bound;
     const double data_mu_up_err = data_mu_up_bound - data_mu;
     std::cout<<"Systematics "<< data_mu_dn_err <<", "<< data_mu_up_err << " Central value "<< data_mu << std::endl;


}

void LostLeptonBackground::WriteHistos()
{
   for (const auto &p : my_histos) {
       p.second->Write();
   }
    
}
