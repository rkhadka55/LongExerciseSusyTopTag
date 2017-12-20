//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Nov 21 14:25:55 2017 by ROOT version 6.10/05
// from TChain slimmedTuple/
//////////////////////////////////////////////////////////

#ifndef LostLeptonBackground_h
#define LostLeptonBackground_h

#include <TH1D.h>
#include "BaseTuple.h"

#include <map>
#include <string>

class LostLeptonBackground : public BaseTuple {
public :
   std::map<std::string, TH1D*>  my_histos;

   LostLeptonBackground(TTree* tree) : BaseTuple(tree) {}

   virtual void     Loop(double weight, int maxevents);
   virtual void     InitHistos();
   virtual void     WriteHistos();

};

#endif
