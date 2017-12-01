#include "TH1.h"
#include "THStack.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"

#include <memory>
#include <vector>
#include <string>
#include <cstdio>

class histInfo
{
public:
    std::string legEntry, histFile, histName, drawOptions;
    int color, rebin;
    std::shared_ptr<TH1> h;

    //helper function to get histogram from file and configure its optional settings
    void retrieveHistogram()
    {
        //Open the file for this histogram
        TFile *f = TFile::Open(histFile.c_str());

        //check that the file was opened successfully
        if(!f)
        {
            printf("File \"%s\" could not be opened!!!\n", histFile.c_str());
            h = nullptr;
            return;
        }

        //get the histogram from the file
        h.reset(static_cast<TH1*>(f->Get(histName.c_str())));

        //with the histogram retrieved, close the file
        f->Close();
        delete f;

        //check that the histogram was retireved from the file successfully
        if(!h)
        {
            printf("Histogram \"%s\" could not be found in file \"%s\"!!!\n", histName.c_str(), histFile.c_str());
            return;
        }

        //set the histogram color
        h->SetLineColor(color);
        h->SetLineWidth(3);
        h->SetMarkerColor(color);
        h->SetMarkerStyle(20);

        // rebin the histogram if desired
        if(rebin >0) h->Rebin(rebin);
    }

    //helper function for axes
    void setupAxes()
    {
        h->SetStats(0);
        h->SetTitle(0);
        h->GetYaxis()->SetTitleOffset(1.2);
        h->GetXaxis()->SetTitleOffset(1.1);
        h->GetXaxis()->SetTitleSize(0.045);
        h->GetXaxis()->SetLabelSize(0.045);
        h->GetYaxis()->SetTitleSize(0.045);
        h->GetYaxis()->SetLabelSize(0.045);
        if(h->GetXaxis()->GetNdivisions() % 100 > 5) h->GetXaxis()->SetNdivisions(6, 5, 0);
    }

    //wrapper to draw histogram
    void draw(const std::string& additionalOptions = "", bool noSame = false) const
    {
        h->Draw(((noSame?"":"same " + drawOptions + " " + additionalOptions)).c_str());
    }

    void setFillColor(int newColor = -1)
    {
        if(newColor >= 0) h->SetFillColor(newColor);
        else              h->SetFillColor(color);
    }

    histInfo(const std::string& legEntry, const std::string& histFile, const std::string& histName, const std::string& drawOptions, const int color, const int rebin) : legEntry(legEntry), histFile(histFile), histName(histName), drawOptions(drawOptions), color(color), rebin(rebin), h(nullptr)
    {
        retrieveHistogram();
    }

    histInfo(TH1* h) : legEntry(h->GetName()), histFile(""), histName(h->GetName()), drawOptions(""), color(0), rebin(0), h(h)
    {
    }

    ~histInfo()
    {
    }
};


void plot(const std::string& histName, const std::string& xAxisLabel, const std::string& yAxisLabel = "Events", const bool isLogY = false, const double xmin = 999.9, const double xmax = -999.9, int rebin = -1)
{
    //This is a magic incantation to disassociate opened histograms from their files so the files can be closed
    TH1::AddDirectory(false);

    //entry for data
    histInfo data = {"Data like QCD", "/uscms_data/d3/nstrobbe/DAS2018/CMSSW_9_3_3/src/LongExerciseSusyTopTag/myhistos/QCD.root", histName, "PEX0", kBlack, rebin};

    //vector summarizing background histograms to include in the plot
    std::vector<histInfo> bgEntries = {
        {"QCD",       "/uscms_data/d3/nstrobbe/DAS2018/CMSSW_9_3_3/src/LongExerciseSusyTopTag/myhistos/QCD.root", histName, "hist", kRed, rebin},
        {"MORE QCD",  "/uscms_data/d3/nstrobbe/DAS2018/CMSSW_9_3_3/src/LongExerciseSusyTopTag/myhistos/QCD.root", histName, "hist", kBlue, rebin},
    };

    //vector summarizing signal histograms to include in the plot
    std::vector<histInfo> sigEntries = {
        {"Signal QCD", "/uscms_data/d3/nstrobbe/DAS2018/CMSSW_9_3_3/src/LongExerciseSusyTopTag/myhistos/QCD.root", histName, "hist", kGreen + 2, rebin},
    };

    //Create TLegend
    TLegend *leg = new TLegend(0.50, 0.56, 0.89, 0.88);
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    leg->SetLineWidth(1);
    leg->SetNColumns(1);
    leg->SetTextFont(42);

    //get maximum from histos and fill TLegend
    leg->AddEntry(data.h.get(), data.legEntry.c_str(), data.drawOptions.c_str());
    double histMax = std::max(histMax, data.h->GetMaximum());

    //background
    for(auto& entry : bgEntries)
    {
        //set fill color so BG will have solid fill
        entry.setFillColor();

        //add histograms to TLegend
        leg->AddEntry(entry.h.get(), entry.legEntry.c_str(), "F");
    }

    //signal 
    for(const auto& entry : sigEntries)
    {
        //add histograms to TLegend
        leg->AddEntry(entry.h.get(), entry.legEntry.c_str(), "L");
    }

    //Create the THStack for the background ... warning, THStacks are terrible and must be filled "backwards"
    THStack *bgStack = new THStack();
    for(int iBG = bgEntries.size() - 1; iBG >= 0; --iBG)
    {
        bgStack->Add(bgEntries[iBG].h.get(), bgEntries[iBG].drawOptions.c_str());
    }
    histMax = std::max(histMax, bgStack->GetMaximum());

    //create the canvas for the plot
    TCanvas *c = new TCanvas("c1", "c1", 800, 800);
    //switch to the canvas to ensure it is the active object
    c->cd();

    //Set Canvas margin (gPad is root magic to access the current pad, in this case canvas "c")
    gPad->SetLeftMargin(0.12);
    gPad->SetRightMargin(0.06);
    gPad->SetTopMargin(0.10);
    gPad->SetBottomMargin(0.12);

    //create a dummy histogram to act as the axes
    histInfo dummy(new TH1D("dummy", "dummy", 1000, data.h->GetBinLowEdge(1), data.h->GetBinLowEdge(data.h->GetNbinsX()) + data.h->GetBinWidth(data.h->GetNbinsX())));
    dummy.setupAxes();
    dummy.h->GetYaxis()->SetTitle(yAxisLabel.c_str());
    dummy.h->GetXaxis()->SetTitle(xAxisLabel.c_str());
    //Set the y-range of the histogram
    if(isLogY) dummy.h->GetYaxis()->SetRangeUser(0, histMax*20);
    else       dummy.h->GetYaxis()->SetRangeUser(0, histMax*1.3);
    //set x-axis range
    if(xmin < xmax) dummy.h->GetXaxis()->SetRangeUser(xmin, xmax);

    //draw dummy axes
    dummy.draw();

    //Switch to logY if desired
    gPad->SetLogy(isLogY);

    //plot background stack
    bgStack->Draw("same");

    //plot signal hisrograms
    for(const auto& entry : sigEntries)
    {
        entry.draw();
    }

    //plot data histogram
    data.draw();

    //plot legend
    leg->Draw("same");

    //Draw dummy hist again to get axes on top of histograms
    dummy.draw("AXIS");

    //save new plot to file
    c->Print((histName + ".png").c_str());

    //clean up dynamic memory
    delete c;
    delete leg;
    delete bgStack;

}

int main()
{
    plot("HT", "H_{T} [GeV]", "Events", false, 300, 2000, 5);
    plot("Nt", "N_{T}");
}
