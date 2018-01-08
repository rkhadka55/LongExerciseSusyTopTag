#include "Math/QuantFuncMathCore.h"

#include<map>
#include<string>
#include<iostream>

#include <TH1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TFile.h>

int main (){
    TFile *_file0 = TFile::Open("myBackground/Data_MET.root");
    TH1* cr_counts = (TH1*) _file0->Get("cr_counts");
    //extrating the counts
    const double data_mu = cr_counts->GetBinContent(1);
    std::cout<<"cr counts "<<data_mu<<std::endl;
    //extracting poison errors https://twiki.cern.ch/twiki/bin/viewauth/CMS/PoissonErrorBars
    cr_counts->SetBinErrorOption(TH1::kPoisson);
    double err_low = cr_counts->GetBinErrorLow(1);
    double err_up = cr_counts->GetBinErrorUp(1);
    std::cout<<"Systematics "<< err_up <<", "<< err_low << " Central value "<< data_mu << std::endl;
    return 0;
}
