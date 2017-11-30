#ifndef PLOT_H
#define PLOT_H

//ROOT headers
#include <TROOT.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TH1.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLatex.h>
#include <TAxis.h>

//STL headers
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

class Plot {
	public:
		Plot(string name_, double intlumi_, bool logx_=false, bool logy_=false) :
			name(name_), intlumi(intlumi_), logx(logx_), logy(logy_), isInit(false),
			histo(0), can(0), pad1(0), leg(0), paveCMS(0), paveExtra(0), paveLumi(0), pad1W(0), pad1H(0)
		{
			//canvas sizes
			canvasW = 700;
			canvasH = 550;
			canvasWextra = 4;
			canvasHextra = 28;
			
			//margins
			marginL = 95;
			marginR = 35;
			marginB = 75;
			marginT = 35;
			
			//font sizes
			sizeT = 32;
			sizeL = 28;
			sizeP = 26;
			sizeTick = 12;
			sizeLoff = 5;
			epsilon = 2;
			
			//axis divisions
			NdivX = 507;
			NdivYhisto = 510;
		}
		
		bool Initialize(TH1* histo_){
			if(isInit) return isInit;
			
			histo = histo_;
			if(!histo) return isInit; //histo creation failed
			else isInit = true;
			
			//account for window frame: 2+2px width, 2+26px height
			can = new TCanvas(name.c_str(),name.c_str(),canvasW+canvasWextra,canvasH+canvasHextra);
		
			pad1 = new TPad("graph","",0,0,1,1);
			pad1W = pad1->GetWw()*pad1->GetAbsWNDC();
			pad1H = pad1->GetWh()*pad1->GetAbsHNDC();
			pad1->SetMargin(marginL/pad1W,marginR/pad1W,marginB/pad1H,marginT/pad1H);
			pad1->SetTicks(1,1);
			if(logy) pad1->SetLogy();
			if(logx) pad1->SetLogx();
			pad1->Draw();

			FormatHist(histo);
			SetTitleOffset(histo->GetXaxis());
			SetTitleOffset(histo->GetYaxis());
			
			InitializePaves();
			
			return isInit;
		}
		
		void InitializePaves(){
			pad1->cd();
			
			//setup CMS text
			TLatex width_test_cms(0,0,"CMS");
			width_test_cms.SetTextSize(sizeP/pad1H);
			double posP = 1-(marginT-1)/pad1H;
			double uminCMS = marginL/pad1W;
			double umaxCMS = marginL/pad1W + width_test_cms.GetXsize();
			paveCMS = new TPaveText(uminCMS,posP,umaxCMS,1.0,"NDC");
			paveCMS->SetFillColor(0);
			paveCMS->SetBorderSize(0);
			paveCMS->SetTextFont(61);
			paveCMS->SetTextSize(sizeP/pad1H);
			paveCMS->AddText("CMS");
			
			//setup prelim text
			//todo: add option to enable/disable/change
			double sizePextra = sizeP - 3; //smaller
			string prelim_text = " Data Analysis School";
			TLatex width_test_extra(0,0,prelim_text.c_str());
			width_test_extra.SetTextSize(sizePextra/pad1H);
			double uminExtra = umaxCMS;
			double umaxExtra = uminExtra + width_test_extra.GetXsize();
			paveExtra = new TPaveText(uminExtra,posP,umaxExtra,1.0,"NDC");
			paveExtra->SetFillColor(0);
			paveExtra->SetBorderSize(0);
			paveExtra->SetTextFont(52);
			paveExtra->SetTextSize(sizePextra/pad1H);
			paveExtra->AddText(prelim_text.c_str());
			
			//setup lumi text
			string luminormunit = "fbinv";
			stringstream fbname_;
			if(luminormunit=="fbinv") fbname_ << fixed << setprecision(1) << intlumi/1000 << " fb^{-1} (13 TeV)";
			else if(luminormunit=="pbinv") fbname_ << fixed << setprecision(1) << intlumi << " pb^{-1} (13 TeV)";
			string fbname = fbname_.str();
			TLatex width_test_lumi(0,0,fbname.c_str());
			width_test_lumi.SetTextSize(sizeP/pad1H);
			double umaxLumi = 1-marginR/pad1W;
			double uminLumi = umaxLumi - width_test_lumi.GetXsize();
			paveLumi = new TPaveText(uminLumi,posP,umaxLumi,1.0,"NDC");
			paveLumi->SetFillColor(0);
			paveLumi->SetBorderSize(0);
			paveLumi->SetTextFont(42);
			paveLumi->SetTextSize(sizeP/pad1H);
			paveLumi->AddText(fbname.c_str());
		}
		
		//helpers
		void SetTitleOffset(TAxis* axis){			
			TLatex height_test(0,0,axis->GetTitle());
			height_test.SetTextSize(sizeT/pad1H);
			double Theight = height_test.GetYsize();
			double Toff = 1;
			
			//note: axis titles are middle-aligned
			if(strcmp(axis->GetName(),"xaxis")==0){
				Toff = (marginB/pad1H - epsilon/pad1H - Theight/2.)/(1.6*sizeT/pad1H);
			}
			else if(strcmp(axis->GetName(),"yaxis")==0){
				//need to scale title height value from pad height to pad width for y axis
				Theight *= pad1H/pad1W;
				Toff = (marginL/pad1W - epsilon/pad1W - Theight/2.)/(1.6*sizeT/pad1H);
			}
			
			axis->SetTitleOffset(Toff);
		}

		void FormatHist(TH1* hist){
			double tickScaleX, tickScaleY;
			pad1->cd();
			tickScaleY = (pad1H - marginB - marginT)/pad1H*pad1W;
			tickScaleX = (pad1W - marginL - marginR)/pad1W*pad1H;

			//fix log-scale label offsets (not final)
			double offX, offY;
			offX = offY = 1.0;
			if(pad1->GetLogx()) {
				offX = 0.5;
			}
			if(pad1->GetLogy()) {
				offY = 0.5;
			}
			
			hist->GetYaxis()->SetTitleSize(sizeT/pad1H);
			hist->GetYaxis()->SetLabelSize(sizeL/pad1H);
			hist->GetYaxis()->SetLabelOffset(offY*sizeLoff/pad1W);
			hist->GetYaxis()->SetTickLength(sizeTick/tickScaleY);
			hist->GetXaxis()->SetTitleSize(sizeT/pad1H);
			hist->GetXaxis()->SetLabelSize(sizeL/pad1H);
			hist->GetXaxis()->SetLabelOffset(offX*sizeLoff/pad1H);
			hist->GetXaxis()->SetTickLength(sizeTick/tickScaleX);
			hist->GetXaxis()->SetNdivisions(NdivX);
			hist->GetYaxis()->SetNdivisions(NdivYhisto);
		}
		
		//drawing:
		void DrawHist(){
			pad1->cd();
			histo->Draw("hist");
		}

		void DrawText(){
			pad1->cd();
			pad1->Update();
			
			if(leg) leg->Draw();
			paveCMS->Draw("same");
			paveExtra->Draw("same");
			paveLumi->Draw("same");
		}
		
		//accessors
		string GetName() { return name; }
		bool IsInit() { return isInit; }
		void SetName(string name_) { name = name_; }
		TH1* GetHisto() { return histo; }
		TCanvas* GetCanvas() { return can; }
		TPad* GetPad1() { return pad1; }
		TLegend* GetLegend() { return leg; }
		void SetLegend(TLegend* leg_) { leg = leg_; }
		TPaveText* GetCMSText() { return paveCMS; }
		TPaveText* GetExtraText() { return paveExtra; }
		TPaveText* GetLumiText() { return paveLumi; }
		
	private:
		//member variables
		string name;
		double intlumi;
		bool logx, logy;
		bool isInit;
		TH1 *histo;
		TCanvas *can;
		TPad *pad1;
		TLegend* leg;
		TPaveText* paveCMS;
		TPaveText* paveExtra;
		TPaveText* paveLumi;
		double canvasW, canvasH, canvasWextra, canvasHextra;
		double marginL, marginR, marginB, marginT;
		double sizeT, sizeL, sizeP, sizeTick, sizeLoff, epsilon;
		double NdivX, NdivYhisto;
		double pad1W, pad1H;
};

#endif
