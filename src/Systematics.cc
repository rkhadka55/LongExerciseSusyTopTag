#include <string>
#include <iostream>

#include <TH1.h>
#include <TFile.h>

void llSystematics ()
{
    TFile *_file0 = TFile::Open("myBackground/Data_MET.root");
    TH1* cr_yield = (TH1*) _file0->Get("cr_yield");
    
    //extracting poisson errors https://twiki.cern.ch/twiki/bin/viewauth/CMS/PoissonErrorBars
    cr_yield->SetBinErrorOption(TH1::kPoisson);

    //extract JEC uncertainty 
    TFile *fJECDn = TFile::Open("myBackground/ttbarW_Sys_down.root");
    TFile *fJECUp = TFile::Open("myBackground/ttbarW_Sys_up.root");
    TFile *fJECNom = TFile::Open("myBackground/ttbarW.root");
    TH1 *hFluctUp_cr = static_cast<TH1*>(fJECUp->Get("cr_yield"));
    TH1 *hFluctDn_cr = static_cast<TH1*>(fJECDn->Get("cr_yield"));
    TH1 *hFluctNom_cr = static_cast<TH1*>(fJECNom->Get("cr_yield"));
    TH1 *hFluctUp_sr = static_cast<TH1*>(fJECUp->Get("sr_yield"));
    TH1 *hFluctDn_sr = static_cast<TH1*>(fJECDn->Get("sr_yield"));
    TH1 *hFluctNom_sr = static_cast<TH1*>(fJECNom->Get("sr_yield"));

    //calculate the central prediction
    TH1 *tFactors = (TH1*)hFluctNom_sr->Clone("tFactors");
    tFactors->Divide(hFluctNom_cr);
    TH1 *centralPred = (TH1*)cr_yield->Clone("yield");
    centralPred->Multiply(tFactors);

    for(int iBin = 1; iBin <= 6; ++iBin)
    {
        std::cout << "iBin: " << iBin << std::endl;

        //extrating the counts
        const double data_mu = cr_yield->GetBinContent(iBin);
        std::cout<<"cr yield: "<<data_mu<<std::endl;
        std::cout<<"tFactor: " << tFactors->GetBinContent(iBin)<<std::endl;
        std::cout<<"central prediction "<<centralPred->GetBinContent(iBin)<<std::endl;
    
        double err_low = cr_yield->GetBinErrorLow(iBin);
        double err_up = cr_yield->GetBinErrorUp(iBin);
        std::cout<<"Systematics "<< err_up <<", "<< err_low << " Central value "<< data_mu << std::endl;

        double jecFluctUp_cr  = hFluctUp_cr->GetBinContent(iBin);
        double jecFluctDn_cr  = hFluctDn_cr->GetBinContent(iBin);
        double jecFluctNom_cr = hFluctNom_cr->GetBinContent(iBin);

        double jecFluctUp_sr  = hFluctUp_sr->GetBinContent(iBin);
        double jecFluctDn_sr  = hFluctDn_sr->GetBinContent(iBin);
        double jecFluctNom_sr = hFluctNom_sr->GetBinContent(iBin);

        double jecErrUp_cr = jecFluctUp_cr - jecFluctNom_cr;
        double jecErrDn_cr = jecFluctNom_cr - jecFluctDn_cr;
        std::cout << "JEC Uncertainty up CR:   " << jecErrUp_cr
                  << "\t"
                  << "JEC Uncertainty Down CR: " << jecErrDn_cr
                  << std::endl;

        double jecErrUp_sr = jecFluctUp_sr - jecFluctNom_sr;
        double jecErrDn_sr = jecFluctNom_sr - jecFluctDn_sr;
        std::cout << "JEC Uncertainty up SR:   " << jecErrUp_sr
                  << "\t"
                  << "JEC Uncertainty Down SR: " << jecErrDn_sr
                  << std::endl;

        double tFactor = jecFluctNom_sr/jecFluctNom_cr;
        double tFactorRelErrUp = sqrt(pow(jecErrUp_cr/jecFluctNom_cr, 2) + pow(jecErrUp_sr/jecFluctNom_sr, 2));
        double tFactorRelErrDn = sqrt(pow(jecErrDn_cr/jecFluctNom_cr, 2) + pow(jecErrDn_sr/jecFluctNom_sr, 2));

        std::cout << "Tfactor relative errors: +" << tFactorRelErrUp << "/-" << tFactorRelErrDn << std::endl;
    }

    TFile *fout = TFile::Open("ttbarW_central.root", "RECREATE");
    fout->cd();
    centralPred->Write();
    tFactors->Write();
    fout->Close();

    fJECDn->Close();
    fJECUp->Close();
    fJECNom->Close();
    _file0->Close();
}

int main()
{
    llSystematics();
    
    return 0;
}
