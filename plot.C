#include "TH1.h"
#include "THStack.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"

#include <memory>
#include <vector>
#include <string>
#include <cstdio>

void smartMax(const TH1 * const h, const TLegend* const l, const TPad* const p, double& gmin, double& gmax, double& gpThreshMax, const bool error)
{
    const bool isLog = p->GetLogy();
    double min = 9e99;
    double max = -9e99;
    double pThreshMax = -9e99;
    int threshold = static_cast<int>(h->GetNbinsX()*(l->GetX1() - p->GetLeftMargin())/((1 - p->GetRightMargin()) - p->GetLeftMargin()));
    printf("thresh %d\n", threshold);

    for(int i = 1; i <= h->GetNbinsX(); ++i)
    {
        double bin = 0.0;
        if(error) bin = h->GetBinContent(i) + h->GetBinError(i);
        else      bin = h->GetBinContent(i);
        if(bin > max) max = bin;
        else if(bin > 1e-10 && bin < min) min = bin;
        if(i >= threshold && bin > pThreshMax) pThreshMax = bin;
    }

    gpThreshMax = std::max(gpThreshMax, pThreshMax);
    gmax = std::max(gmax, max);
    gmin = std::min(gmin, min);
}

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


void plot(const std::string& histName, const std::string& xAxisLabel, const std::string& yAxisLabel = "Events", const bool isLogY = false, const double xmin = 999.9, const double xmax = -999.9, int rebin = -1, double lumi = 36100)
{
    //This is a magic incantation to disassociate opened histograms from their files so the files can be closed
    TH1::AddDirectory(false);

    //entry for data
    histInfo data = {"Data", "/uscms_data/d3/nstrobbe/DAS2018/CMSSW_9_3_3/src/LongExerciseSusyTopTag/myhistos/Data_MET.root", histName, "PEX0", kBlack, rebin};

    //vector summarizing background histograms to include in the plot
    std::vector<histInfo> bgEntries = {
        {"t#bar{t}",           "/uscms_data/d3/nstrobbe/DAS2018/CMSSW_9_3_3/src/LongExerciseSusyTopTag/myhistos/TTbarNoHad.root",  histName, "hist", kRed,        rebin},
        {"W+Jets",             "/uscms_data/d3/nstrobbe/DAS2018/CMSSW_9_3_3/src/LongExerciseSusyTopTag/myhistos/WJetsToLNu.root",  histName, "hist", kBlue,       rebin},
        {"Single top",         "/uscms_data/d3/nstrobbe/DAS2018/CMSSW_9_3_3/src/LongExerciseSusyTopTag/myhistos/ST.root",          histName, "hist", kOrange,     rebin},
        {"Z#rightarrow#nu#nu", "/uscms_data/d3/nstrobbe/DAS2018/CMSSW_9_3_3/src/LongExerciseSusyTopTag/myhistos/ZJetsToNuNu.root", histName, "hist", kYellow + 2, rebin},
        {"QCD",                "/uscms_data/d3/nstrobbe/DAS2018/CMSSW_9_3_3/src/LongExerciseSusyTopTag/myhistos/QCD.root",         histName, "hist", kMagenta,    rebin},
        {"Rare",               "/uscms_data/d3/nstrobbe/DAS2018/CMSSW_9_3_3/src/LongExerciseSusyTopTag/myhistos/Rare.root",        histName, "hist", kGray,       rebin},
    };

    //vector summarizing signal histograms to include in the plot
    std::vector<histInfo> sigEntries = {
        {"Signal QCD", "/uscms_data/d3/nstrobbe/DAS2018/CMSSW_9_3_3/src/LongExerciseSusyTopTag/myhistos/QCD.root", histName, "hist", kGreen + 2, rebin},
    };

    //create the canvas for the plot
    TCanvas *c = new TCanvas("c1", "c1", 800, 800);
    //switch to the canvas to ensure it is the active object
    c->cd();

    //Create TLegend
    TLegend *leg = new TLegend(0.50, 0.56, 0.89, 0.88);
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    leg->SetLineWidth(1);
    leg->SetNColumns(1);
    leg->SetTextFont(42);

    //get maximum from histos and fill TLegend
    double min = 0.0;
    double max = 0.0;
    double lmax = 0.0;

    //Create the THStack for the background ... warning, THStacks are terrible and must be filled "backwards"
    THStack *bgStack = new THStack();
    //Make seperate histogram from sum of BG histograms because I don't know how to make a THStack give me this 
    TH1* hbgSum = nullptr;
    for(int iBG = bgEntries.size() - 1; iBG >= 0; --iBG)
    {
        bgStack->Add(bgEntries[iBG].h.get(), bgEntries[iBG].drawOptions.c_str());
        if(!hbgSum) hbgSum = static_cast<TH1*>(bgEntries[iBG].h->Clone());
        else        hbgSum->Add(bgEntries[iBG].h.get());
    }

    //data
    leg->AddEntry(data.h.get(), data.legEntry.c_str(), data.drawOptions.c_str());
    smartMax(hbgSum, leg, static_cast<TPad*>(gPad), min, max, lmax, true);

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
        smartMax(entry.h.get(), leg, static_cast<TPad*>(gPad), min, max, lmax, false);
    }

    //Set Canvas margin (gPad is root magic to access the current pad, in this case canvas "c")
    gPad->SetLeftMargin(0.12);
    gPad->SetRightMargin(0.06);
    gPad->SetTopMargin(0.08);
    gPad->SetBottomMargin(0.12);

    //create a dummy histogram to act as the axes
    histInfo dummy(new TH1D("dummy", "dummy", 1000, data.h->GetBinLowEdge(1), data.h->GetBinLowEdge(data.h->GetNbinsX()) + data.h->GetBinWidth(data.h->GetNbinsX())));
    dummy.setupAxes();
    dummy.h->GetYaxis()->SetTitle(yAxisLabel.c_str());
    dummy.h->GetXaxis()->SetTitle(xAxisLabel.c_str());
    //Set the y-range of the histogram

    printf("%f %f %f\n", min, max, lmax);
    if(isLogY)
    {
        double locMin = std::min(0.2, std::max(0.2, 0.05 * min));
        double legSpan = (log10(3*max) - log10(locMin)) * (leg->GetY1() - gPad->GetBottomMargin()) / ((1 - gPad->GetTopMargin()) - gPad->GetBottomMargin());
        double legMin = legSpan + log10(locMin);
        if(log10(lmax) > legMin)
        {
            double scale = (log10(lmax) - log10(locMin)) / (legMin - log10(locMin));
            max = pow(max/locMin, scale)*locMin;
        }
        dummy.h->GetYaxis()->SetRangeUser(locMin, 10*max);
    }
    else
    {
        double locMin = 0.0;
        double legMin = (1.2*max - locMin) * (leg->GetY1() - gPad->GetBottomMargin()) / ((1 - gPad->GetTopMargin()) - gPad->GetBottomMargin());
        if(lmax > legMin) max *= (lmax - locMin)/(legMin - locMin);
        dummy.h->GetYaxis()->SetRangeUser(0.0, max*1.2);
    }
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

    //Draw CMS and lumi lables
    char lumistamp[128];
    sprintf(lumistamp, "%.1f fb^{-1} (13 TeV)", lumi / 1000.0);

    TLatex mark;
    mark.SetNDC(true);

    //Draw CMS mark
    mark.SetTextAlign(11);
    mark.SetTextSize(0.050);
    mark.SetTextFont(61);
    mark.DrawLatex(gPad->GetLeftMargin(), 1 - (gPad->GetTopMargin() - 0.017), "CMS"); // #scale[0.8]{#it{Preliminary}}");
    mark.SetTextSize(0.040);
    mark.SetTextFont(52);
    mark.DrawLatex(gPad->GetLeftMargin() + 0.11, 1 - (gPad->GetTopMargin() - 0.017), "DAS 2018");

    //Draw lumistamp
    mark.SetTextFont(42);
    mark.SetTextAlign(31);
    mark.DrawLatex(1 - gPad->GetRightMargin(), 1 - (gPad->GetTopMargin() - 0.017), lumistamp);

    //save new plot to file
    c->Print((histName + ".png").c_str());

    //clean up dynamic memory
    delete c;
    delete leg;
    delete bgStack;
    delete hbgSum;
}

int main()
{
    plot("HT", "H_{T} [GeV]", "Events", true, -1, -1, 5);
    plot("Nt", "N_{T}");
}
