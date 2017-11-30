#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraphAsymmErrors.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TMarker.h>
#include <TBox.h>
#include <TLine.h>
#include <TMath.h>
#include <TAxis.h>
#include <TStyle.h>
#include <TVirtualPadPainter.h>

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <iterator>

#include "Plot.h"

using namespace std;

void multiplyXsec(double* rvals, const vector<double>& xsecs){
	for(unsigned i = 0; i < xsecs.size(); ++i){
		rvals[i] = rvals[i]*xsecs[i];
	}
}

TGraph* getBand(TTree* limit, double q_dn, double q_up, const vector<double>& xsecs){
	stringstream ss_dn;
	ss_dn << "abs(quantileExpected-" << q_dn << ")<0.01";
	int npts = limit->Draw("limit:mh",ss_dn.str().c_str(),"goff");
	double* rtmp_dn = limit->GetV1();
	double* mtmp_dn = limit->GetV2();
	multiplyXsec(rtmp_dn,xsecs);
	
	double* rtmp = new double[npts*2];
	double* mtmp = new double[npts*2];
	for(int m = 0; m < npts; ++m){
		rtmp[npts*2-1-m] = rtmp_dn[m];
		mtmp[npts*2-1-m] = mtmp_dn[m];
	}
	
	stringstream ss_up;
	ss_up << "abs(quantileExpected-" << q_up << ")<0.01";
	npts = limit->Draw("limit:mh",ss_up.str().c_str(),"goff");
	double* rtmp_up = limit->GetV1();
	double* mtmp_up = limit->GetV2();
	multiplyXsec(rtmp_up,xsecs);
	
	for(int m = 0; m < npts; ++m){
		rtmp[m] = rtmp_up[m];
		mtmp[m] = mtmp_up[m];
	}
	
	TGraph* gtmp = new TGraph(npts*2,mtmp,rtmp);
	return gtmp;
}

void getRange(int n, double* arr, double& ymin, double& ymax){
	double ymin_ = TMath::MinElement(n,arr);
	if(ymin_ < ymin) ymin = ymin_;
	
	double ymax_ = TMath::MaxElement(n,arr);
	if(ymax_ > ymax) ymax = ymax_;
}

//usage:
//root -l 'plotLimit.C+("T2tt",2)'
//root -l 'plotLimit.C+("T1tttt",2)'
void plotLimit(string signame, int nsigma=0){
	//cross section values
	vector<double> masses = {};
	vector<double> xsecs = {};
	if(signame=="T2tt") {
		masses = {800,900,1000,1100,1200};
		xsecs = {0.0283338,0.0128895,0.00615134,0.00307413,0.00159844}; //stop masses 800-1200
	}
	else {
		masses = {1700,1800,1900,2000,2100};
		xsecs = {0.00470323,0.00276133,0.00163547,0.000981077,0.000591918}; //gluino masses 1700-2100
	}
	
	//ranges for plotting
	double ymin = 1e10, xmin = 1e10;
	double ymax = 0, xmax = 0;
	gStyle->SetOptStat(0);
	
	//extract info from hadded limit file
	string fname = "cards/higgsCombine_"+signame+"_best.root";
	TFile* file = TFile::Open(fname.c_str());
	if(!file) {
		cout << "Couldn't open " << fname << endl;
		return;
	}
	TTree* limit = (TTree*)file->Get("limit");
	if(!limit) {
		cout << "Couldn't get limit tree from " << fname << endl;
		return;
	}

	//setup plotting options
	string process, yname, xname;
	if(signame=="T2tt"){
		process = "#tilde{t} #rightarrow t #tilde{#chi}_{1}^{0}";
		yname = "#sigma(pp#rightarrow#tilde{t}#tilde{t}) [pb]";
		xname = "m_{#tilde{t}} [GeV]";
	}
	else{
		process = "#tilde{g} #rightarrow t #bar{t} #tilde{#chi}_{1}^{0}";
		yname = "#sigma(pp#rightarrow#tilde{g}#tilde{g}) [pb]";
		xname = "m_{#tilde{g}} [GeV]";
	}
	
	//initialize legend
	double legsize = 0.04;
	double legx1 = 0.6;
	double legx2 = 0.93;
	double legy2 = 0.9;
	double legy1 = legy2-legsize*(4+nsigma+0.5);
	TLegend* leg = new TLegend(legx1,legy1,legx2,legy2);
	leg->SetFillColor(0);
	leg->SetBorderSize(0);
	leg->SetTextSize(legsize);
	leg->SetTextFont(42);
	leg->SetMargin(0.15);
	
	//initialize pave
	double pavex1 = 0.2;
	double pavex2 = 0.45;
	double pavey2 = 0.9;
	double pavey1 = pavey2-legsize*3;
	TPaveText* pave = new TPaveText(pavex1,pavey1,pavex2,pavey2,"NDC");
	pave->SetFillColor(0);
	pave->SetBorderSize(0);
	pave->SetTextSize(legsize);
	pave->SetTextFont(42);
	pave->SetTextAlign(12);
	pave->AddText(process.c_str());
	pave->AddText("m_{#tilde{#chi}_{1}^{0}} = 1 GeV");
	
	//preamble of legend
	leg->AddEntry((TObject*)NULL,"95% CL upper limits","");
	
	//get cross section
	TGraph* g_xsec = new TGraph(xsecs.size(),masses.data(),xsecs.data());
	g_xsec->SetLineColor(kMagenta);
	g_xsec->SetLineStyle(1);
	g_xsec->SetLineWidth(2);
	leg->AddEntry(g_xsec,"Theoretical","l");
	getRange(xsecs.size(),xsecs.data(),ymin,ymax);
	//only get x range once
	getRange(masses.size(),masses.data(),xmin,xmax);
	
	//get observed limit
	int npts = limit->Draw("limit:mh","abs(quantileExpected+1)<0.01","goff");
	double* rtmp = limit->GetV1();
	double* mtmp = limit->GetV2();
	multiplyXsec(rtmp,xsecs);
	TGraph* g_obs = new TGraph(npts,mtmp,rtmp);
	g_obs->SetMarkerColor(kBlack);
	g_obs->SetLineColor(kBlack);
	g_obs->SetMarkerStyle(20);
	g_obs->SetLineStyle(1);
	g_obs->SetLineWidth(2);
	leg->AddEntry(g_obs,"Observed","pe");
	getRange(npts,rtmp,ymin,ymax);
	
	//get central value (expected)
	int nptsC = limit->Draw("limit:mh","abs(quantileExpected-0.5)<0.01","goff");
	double* rtmpC = limit->GetV1();
	double* mtmpC = limit->GetV2();
	multiplyXsec(rtmpC,xsecs);
	TGraph* g_central = new TGraph(npts,mtmpC,rtmpC);
	g_central->SetLineColor(kBlue);
	g_central->SetLineStyle(2);
	g_central->SetLineWidth(2);
	leg->AddEntry(g_central,"Median expected","l");
	getRange(npts,rtmpC,ymin,ymax);
	
	//get bands (expected)
	TGraph* g_one = NULL;
	if(nsigma>=1){
		g_one = getBand(limit,0.16,0.84,xsecs);
		g_one->SetFillColor(kGreen+1);
		leg->AddEntry(g_one,"68% expected","f");
		getRange(npts*2,g_one->GetY(),ymin,ymax);
	}
	TGraph* g_two = NULL;
	if(nsigma>=2){
		g_two = getBand(limit,0.025,0.975,xsecs);
		g_two->SetFillColor(kOrange);
		leg->AddEntry(g_two,"95% expected","f");
		getRange(npts*2,g_two->GetY(),ymin,ymax);
	}
	
	//extend range
	ymax = ymax*10;
	ymin = ymin/10;
	//xmax = xmax + 100;
	//xmin = xmin - 100;
	
	//make histo for axes
	TH1F* hbase = new TH1F("hbase","",100,xmin,xmax);
	hbase->GetYaxis()->SetTitle(yname.c_str());
	hbase->GetXaxis()->SetTitle(xname.c_str());
	hbase->GetYaxis()->SetRangeUser(ymin,ymax);
	
	//make plot
	Plot plot("plotLimit_"+signame,35900,false,true);
	plot.Initialize(hbase);
	plot.SetLegend(leg);
	TCanvas* can = plot.GetCanvas();
	TPad* pad1 = plot.GetPad1();
	pad1->cd();
	
	//draw blank histo for axes
	plot.DrawHist();
	
	//draw graphs
	if(nsigma>=2) g_two->Draw("f same");
	if(nsigma>=1) g_one->Draw("f same");
	g_central->Draw("C same");
	g_obs->Draw("pC same");
	g_xsec->Draw("C same");

	plot.GetHisto()->Draw("sameaxis"); //draw again so axes on top
	plot.DrawText();
	pave->Draw("same");
	
	//print image
	can->Print((plot.GetName()+".png").c_str(),"png");
}