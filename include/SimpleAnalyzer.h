//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Nov 21 14:25:55 2017 by ROOT version 6.10/05
// from TChain slimmedTuple/
//////////////////////////////////////////////////////////

#ifndef SimpleAnalyzer_h
#define SimpleAnalyzer_h

#include <TH1D.h>
#include "BaseTuple.h"
#include "samples.h"

#include <map>
#include <string>

class SimpleAnalyzer : public BaseTuple {
public :
   std::map<std::string, TH1D*>  my_histos;

   SimpleAnalyzer(TTree* tree) : BaseTuple(tree) {}

   virtual void     Loop(double weight, int maxevents = -1, bool isQuiet = false, bool isFastSim = false);
   virtual void     InitHistos();
   virtual void     WriteHistos();

};

#endif
