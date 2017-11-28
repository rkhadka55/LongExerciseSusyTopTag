//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Nov 21 14:25:55 2017 by ROOT version 6.10/05
// from TChain slimmedTuple/
//////////////////////////////////////////////////////////

#ifndef SimpleAnalyzer_h
#define SimpleAnalyzer_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include <TH1D.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include <map>
#include <string>

using namespace std;

class SimpleAnalyzer {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Double_t        HT;
   Double_t        TriggerEffMC;
   vector<TLorentzVector> *cutElecVec;
   vector<TLorentzVector> *cutMuVec;
   Double_t        eventWeight;
   vector<TLorentzVector> *jetsLVec_slimmed;
   Double_t        met;
   Double_t        metphi;
   Bool_t          passBJets;
   Bool_t          passEleVeto;
   Bool_t          passIsoTrkVeto;
   Bool_t          passMuonVeto;
   Bool_t          passNoiseEventFilter;
   Bool_t          passSearchTrigger;
   Bool_t          passdPhis;
   Bool_t          passnJets;
   vector<TLorentzVector> *puppiJetsLVec_slimmed;
   vector<vector<TLorentzVector> > *puppiSubJetsLVec_slimmed;
   vector<double>  *puppisoftDropMass_slimmed;
   vector<double>  *puppitau1_slimmed;
   vector<double>  *puppitau2_slimmed;
   vector<double>  *puppitau3_slimmed;
   vector<double>  *qgAxis2_slimmed;
   vector<double>  *qgLikelihood_slimmed;
   vector<double>  *qgMult_slimmed;
   vector<double>  *qgPtD_slimmed;
   vector<double>  *recoJetsBtag_slimmed;
   vector<double>  *recoJetschargedEmEnergyFraction_slimmed;
   vector<double>  *recoJetschargedHadronEnergyFraction_slimmed;
   vector<double>  *recoJetsneutralEmEnergyFraction_slimmed;

   // List of branches
   TBranch        *b_HT;   //!
   TBranch        *b_TriggerEffMC;   //!
   TBranch        *b_cutElecVec;   //!
   TBranch        *b_cutMuVec;   //!
   TBranch        *b_eventWeight;   //!
   TBranch        *b_jetsLVec_slimmed;   //!
   TBranch        *b_met;   //!
   TBranch        *b_metphi;   //!
   TBranch        *b_passBJets;   //!
   TBranch        *b_passEleVeto;   //!
   TBranch        *b_passIsoTrkVeto;   //!
   TBranch        *b_passMuonVeto;   //!
   TBranch        *b_passNoiseEventFilter;   //!
   TBranch        *b_passSearchTrigger;   //!
   TBranch        *b_passdPhis;   //!
   TBranch        *b_passnJets;   //!
   TBranch        *b_puppiJetsLVec_slimmed;   //!
   TBranch        *b_puppiSubJetsLVec_slimmed;   //!
   TBranch        *b_puppisoftDropMass_slimmed;   //!
   TBranch        *b_puppitau1_slimmed;   //!
   TBranch        *b_puppitau2_slimmed;   //!
   TBranch        *b_puppitau3_slimmed;   //!
   TBranch        *b_qgAxis2_slimmed;   //!
   TBranch        *b_qgLikelihood_slimmed;   //!
   TBranch        *b_qgMult_slimmed;   //!
   TBranch        *b_qgPtD_slimmed;   //!
   TBranch        *b_recoJetsBtag_slimmed;   //!
   TBranch        *b_recoJetschargedEmEnergyFraction_slimmed;   //!
   TBranch        *b_recoJetschargedHadronEnergyFraction_slimmed;   //!
   TBranch        *b_recoJetsneutralEmEnergyFraction_slimmed;   //!

   // Added variables
   std::map<std::string, TH1D*>  my_histos;

   SimpleAnalyzer(TTree *tree=0);
   virtual ~SimpleAnalyzer();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(double weight);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   virtual void     InitHistos();
   virtual void     WriteHistos();


};

#endif

#ifdef SimpleAnalyzer_cxx
SimpleAnalyzer::SimpleAnalyzer(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {

#ifdef SINGLE_TREE
      // The following code should be used if you want this class to access
      // a single tree instead of a chain
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Memory Directory");
      if (!f || !f->IsOpen()) {
         f = new TFile("Memory Directory");
      }
      f->GetObject("slimmedTuple",tree);

#else // SINGLE_TREE

      // The following code should be used if you want this class to access a chain
      // of trees.
      TChain * chain = new TChain("slimmedTuple","");
      chain->Add("/uscms_data/d3/pastika/DAS2018/CMSSW_9_3_3/src/TopTagger/Tools/susyTopTagging0_TTbarSingleLepT.root/slimmedTuple");
      tree = chain;
#endif // SINGLE_TREE

   }
   Init(tree);
}

SimpleAnalyzer::~SimpleAnalyzer()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t SimpleAnalyzer::GetEntry(Long64_t entry)

{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t SimpleAnalyzer::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void SimpleAnalyzer::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   cutElecVec = 0;
   cutMuVec = 0;
   jetsLVec_slimmed = 0;
   puppiJetsLVec_slimmed = 0;
   puppiSubJetsLVec_slimmed = 0;
   puppisoftDropMass_slimmed = 0;
   puppitau1_slimmed = 0;
   puppitau2_slimmed = 0;
   puppitau3_slimmed = 0;
   qgAxis2_slimmed = 0;
   qgLikelihood_slimmed = 0;
   qgMult_slimmed = 0;
   qgPtD_slimmed = 0;
   recoJetsBtag_slimmed = 0;
   recoJetschargedEmEnergyFraction_slimmed = 0;
   recoJetschargedHadronEnergyFraction_slimmed = 0;
   recoJetsneutralEmEnergyFraction_slimmed = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("HT", &HT, &b_HT);
   fChain->SetBranchAddress("TriggerEffMC", &TriggerEffMC, &b_TriggerEffMC);
   fChain->SetBranchAddress("cutElecVec", &cutElecVec, &b_cutElecVec);
   fChain->SetBranchAddress("cutMuVec", &cutMuVec, &b_cutMuVec);
   fChain->SetBranchAddress("eventWeight", &eventWeight, &b_eventWeight);
   fChain->SetBranchAddress("jetsLVec_slimmed", &jetsLVec_slimmed, &b_jetsLVec_slimmed);
   fChain->SetBranchAddress("met", &met, &b_met);
   fChain->SetBranchAddress("metphi", &metphi, &b_metphi);
   fChain->SetBranchAddress("passBJets", &passBJets, &b_passBJets);
   fChain->SetBranchAddress("passEleVeto", &passEleVeto, &b_passEleVeto);
   fChain->SetBranchAddress("passIsoTrkVeto", &passIsoTrkVeto, &b_passIsoTrkVeto);
   fChain->SetBranchAddress("passMuonVeto", &passMuonVeto, &b_passMuonVeto);
   fChain->SetBranchAddress("passNoiseEventFilter", &passNoiseEventFilter, &b_passNoiseEventFilter);
   fChain->SetBranchAddress("passSearchTrigger", &passSearchTrigger, &b_passSearchTrigger);
   fChain->SetBranchAddress("passdPhis", &passdPhis, &b_passdPhis);
   fChain->SetBranchAddress("passnJets", &passnJets, &b_passnJets);
   fChain->SetBranchAddress("puppiJetsLVec_slimmed", &puppiJetsLVec_slimmed, &b_puppiJetsLVec_slimmed);
   fChain->SetBranchAddress("puppiSubJetsLVec_slimmed", &puppiSubJetsLVec_slimmed, &b_puppiSubJetsLVec_slimmed);
   fChain->SetBranchAddress("puppisoftDropMass_slimmed", &puppisoftDropMass_slimmed, &b_puppisoftDropMass_slimmed);
   fChain->SetBranchAddress("puppitau1_slimmed", &puppitau1_slimmed, &b_puppitau1_slimmed);
   fChain->SetBranchAddress("puppitau2_slimmed", &puppitau2_slimmed, &b_puppitau2_slimmed);
   fChain->SetBranchAddress("puppitau3_slimmed", &puppitau3_slimmed, &b_puppitau3_slimmed);
   fChain->SetBranchAddress("qgAxis2_slimmed", &qgAxis2_slimmed, &b_qgAxis2_slimmed);
   fChain->SetBranchAddress("qgLikelihood_slimmed", &qgLikelihood_slimmed, &b_qgLikelihood_slimmed);
   fChain->SetBranchAddress("qgMult_slimmed", &qgMult_slimmed, &b_qgMult_slimmed);
   fChain->SetBranchAddress("qgPtD_slimmed", &qgPtD_slimmed, &b_qgPtD_slimmed);
   fChain->SetBranchAddress("recoJetsBtag_slimmed", &recoJetsBtag_slimmed, &b_recoJetsBtag_slimmed);
   fChain->SetBranchAddress("recoJetschargedEmEnergyFraction_slimmed", &recoJetschargedEmEnergyFraction_slimmed, &b_recoJetschargedEmEnergyFraction_slimmed);
   fChain->SetBranchAddress("recoJetschargedHadronEnergyFraction_slimmed", &recoJetschargedHadronEnergyFraction_slimmed, &b_recoJetschargedHadronEnergyFraction_slimmed);
   fChain->SetBranchAddress("recoJetsneutralEmEnergyFraction_slimmed", &recoJetsneutralEmEnergyFraction_slimmed, &b_recoJetsneutralEmEnergyFraction_slimmed);
   Notify();
}

Bool_t SimpleAnalyzer::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void SimpleAnalyzer::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t SimpleAnalyzer::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef SimpleAnalyzer_cxx
