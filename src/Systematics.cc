#include <string>
#include <iostream>

#include <TH1.h>
#include <TFile.h>

void llSystematics ()
{
    TFile *_file0 = TFile::Open("myBackground/Data_MET.root");
    TH1* cr_yield = (TH1*) _file0->Get("cr_yield");
    
    //extracting poisson errors https://twiki.cern.ch/twiki/bin/viewauth/CMS/PoissonErrorBars
    _file0->Close();
}

//To run as main compiled code 
int main()
{
    llSystematics();
    
    return 0;
}
