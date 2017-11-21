//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Nov 21 11:32:52 2017 by ROOT version 6.10/05
// from TTree slimmedTuple/susyTopTagging0_TTbarSingleLepT.rootslimmedTuple
// found on file: /uscms_data/d3/pastika/DAS2018/CMSSW_9_3_3/src/TopTagger/Tools/susyTopTagging0_TTbarSingleLepT.root
//////////////////////////////////////////////////////////

#ifndef SimpleAnalyzer_h
#define SimpleAnalyzer_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TLorentzVector.h>

// Header file for the classes stored in the TTree if any.
#include "vector"

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
   vector<TLorentzVector> *jetsLVec;
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
   vector<TLorentzVector> *puppiJetsLVec;
   vector<TLorentzVector> *puppiSubJetsLVec;
   vector<double>  *puppisoftDropMass;
   vector<double>  *puppitau1;
   vector<double>  *puppitau2;
   vector<double>  *puppitau3;
   vector<double>  *qgAxis2;
   vector<double>  *qgLikelihood;
   vector<int>     *qgMult;
   vector<double>  *qgPtD;
   vector<double>  *recoJetsBtag_0;
   vector<double>  *recoJetschargedEmEnergyFraction;
   vector<double>  *recoJetschargedHadronEnergyFraction;
   vector<double>  *recoJetsneutralEmEnergyFraction;

   // List of branches
   TBranch        *b_HT;   //!
   TBranch        *b_TriggerEffMC;   //!
   TBranch        *b_cutElecVec;   //!
   TBranch        *b_cutMuVec;   //!
   TBranch        *b_jetsLVec;   //!
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
   TBranch        *b_puppiJetsLVec;   //!
   TBranch        *b_puppiSubJetsLVec;   //!
   TBranch        *b_puppisoftDropMass;   //!
   TBranch        *b_puppitau1;   //!
   TBranch        *b_puppitau2;   //!
   TBranch        *b_puppitau3;   //!
   TBranch        *b_qgAxis2;   //!
   TBranch        *b_qgLikelihood;   //!
   TBranch        *b_qgMult;   //!
   TBranch        *b_qgPtD;   //!
   TBranch        *b_recoJetsBtag_0;   //!
   TBranch        *b_recoJetschargedEmEnergyFraction;   //!
   TBranch        *b_recoJetschargedHadronEnergyFraction;   //!
   TBranch        *b_recoJetsneutralEmEnergyFraction;   //!

   SimpleAnalyzer(TTree *tree=0);
   virtual ~SimpleAnalyzer();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef SimpleAnalyzer_cxx
SimpleAnalyzer::SimpleAnalyzer(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/uscms_data/d3/pastika/DAS2018/CMSSW_9_3_3/src/TopTagger/Tools/susyTopTagging0_TTbarSingleLepT.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/uscms_data/d3/pastika/DAS2018/CMSSW_9_3_3/src/TopTagger/Tools/susyTopTagging0_TTbarSingleLepT.root");
      }
      f->GetObject("slimmedTuple",tree);

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
   jetsLVec = 0;
   puppiJetsLVec = 0;
   puppiSubJetsLVec = 0;
   puppisoftDropMass = 0;
   puppitau1 = 0;
   puppitau2 = 0;
   puppitau3 = 0;
   qgAxis2 = 0;
   qgLikelihood = 0;
   qgMult = 0;
   qgPtD = 0;
   recoJetsBtag_0 = 0;
   recoJetschargedEmEnergyFraction = 0;
   recoJetschargedHadronEnergyFraction = 0;
   recoJetsneutralEmEnergyFraction = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("HT", &HT, &b_HT);
   fChain->SetBranchAddress("TriggerEffMC", &TriggerEffMC, &b_TriggerEffMC);
   fChain->SetBranchAddress("cutElecVec", &cutElecVec, &b_cutElecVec);
   fChain->SetBranchAddress("cutMuVec", &cutMuVec, &b_cutMuVec);
   fChain->SetBranchAddress("jetsLVec", &jetsLVec, &b_jetsLVec);
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
   fChain->SetBranchAddress("puppiJetsLVec", &puppiJetsLVec, &b_puppiJetsLVec);
   fChain->SetBranchAddress("puppiSubJetsLVec", &puppiSubJetsLVec, &b_puppiSubJetsLVec);
   fChain->SetBranchAddress("puppisoftDropMass", &puppisoftDropMass, &b_puppisoftDropMass);
   fChain->SetBranchAddress("puppitau1", &puppitau1, &b_puppitau1);
   fChain->SetBranchAddress("puppitau2", &puppitau2, &b_puppitau2);
   fChain->SetBranchAddress("puppitau3", &puppitau3, &b_puppitau3);
   fChain->SetBranchAddress("qgAxis2", &qgAxis2, &b_qgAxis2);
   fChain->SetBranchAddress("qgLikelihood", &qgLikelihood, &b_qgLikelihood);
   fChain->SetBranchAddress("qgMult", &qgMult, &b_qgMult);
   fChain->SetBranchAddress("qgPtD", &qgPtD, &b_qgPtD);
   fChain->SetBranchAddress("recoJetsBtag_0", &recoJetsBtag_0, &b_recoJetsBtag_0);
   fChain->SetBranchAddress("recoJetschargedEmEnergyFraction", &recoJetschargedEmEnergyFraction, &b_recoJetschargedEmEnergyFraction);
   fChain->SetBranchAddress("recoJetschargedHadronEnergyFraction", &recoJetschargedHadronEnergyFraction, &b_recoJetschargedHadronEnergyFraction);
   fChain->SetBranchAddress("recoJetsneutralEmEnergyFraction", &recoJetsneutralEmEnergyFraction, &b_recoJetsneutralEmEnergyFraction);
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
