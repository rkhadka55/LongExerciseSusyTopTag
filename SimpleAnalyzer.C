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
#include "TopTagger/TopTagger/include/TopTagger.h"
#include "TopTagger/TopTagger/include/TopTaggerResults.h"
#include "TopTagger/TopTagger/include/TopTaggerUtilities.h"
#include "TopTagger/CfgParser/include/TTException.h"

void SimpleAnalyzer::Loop()
{
//   In a ROOT session, you can do:
//      root> .L SimpleAnalyzer.C
//      root> SimpleAnalyzer t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;

   // Define list of histograms you want to make
   std::map<std::string, TH1D*>  my_histos;
   my_histos.emplace("HT",new TH1D("HT","HT",100,0,2000));
   my_histos.emplace("Nt",new TH1D("Nt","Nt",5,0,5));



   // Create top tagger object
   TopTagger tt;
   tt.setCfgFile("TopTagger.cfg");

   for (Long64_t jentry=0; jentry<nentries; jentry++) 
   {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   
      nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      if(ientry%100 == 0)
          std::cout << "At event " << ientry << std::endl;

      // ------------------
      // --- TOP TAGGER ---
      // ------------------
          
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
          *puppiSubJetsLVec_slimmed    // These should be the subjets!
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
      
      // -----------------------
      // --- FILL HISTOGRAMS ---
      // -----------------------

      my_histos["HT"]->Fill(HT);
      my_histos["Nt"]->Fill(tops.size());
   }

   for (const auto &p : my_histos) {
       p.second->Write();
   }

}
