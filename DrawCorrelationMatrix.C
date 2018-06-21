#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TROOT.h>                        // access to gROOT, entry point to ROOT system
#include <TSystem.h>                      // interface to OS
#include <TStyle.h>                       // class to handle ROOT plotting styles
#include <TFile.h>                        // file handle class
#include <TTree.h>                        // class to access ntuples
#include <TChain.h>                        // class to access ntuples
#include <TH1D.h>
#include <TH2D.h>
#include <TProfile2D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TBenchmark.h>                   // class to track macro running statistics
#include <vector>                         // STL vector class
#include <iostream>                       // standard I/O
#include <iomanip>                        // functions to format standard I/O
#include <fstream>                        // functions for file I/O
#include <string>                         // C++ string class
#include <sstream>                        // class for parsing strings
#endif


void DrawCorrelationMatrix(TString baseDir="/afs/cern.ch/user/v/vawong/CMSSW_10_1_2/src/RecoLocalCalo/HcalRecAlgos/CoVar",
						TString fn="Data_Cosmics_2018B_Collisions"
						) {
	
	TFile *inf = new TFile(baseDir+"/CovarianceMatrixFiles/"+fn+".root", "read");
	TTree *t = (TTree*) inf->Get("HcalTree");
	
	double nEvents;
	
	double avgTS[8];
	double rmsTS[8];
	double corTS[8][8];
	
	t->SetBranchAddress("nEvents",&nEvents);
	t->SetBranchAddress("avgTS",&avgTS);
	t->SetBranchAddress("rmsTS",&rmsTS);
	t->SetBranchAddress("covTS",&corTS);
	
	TCanvas *c = new TCanvas("c","",800,800);
	gStyle->SetOptStat(0);
	gStyle->SetPalette(55);
	gStyle->SetPaintTextFormat("2.2f");
	
	for(int n=0; n<t->GetEntries(); n++) {
		t->GetEntry(n);
		cout << n << endl;
		
		TH2D* corr_matrix = new TH2D("corr_matrix","",8,-0.5,7.5,8,-0.5,7.5);
		
		for(int i=0; i<8; i++) {
			for(int j = 0; j<i+1; j++) {
				cout << corTS[i][j] << endl;
				
				corr_matrix->SetBinContent(
											corr_matrix->GetBin(i+1,j+1),
											corTS[i][j]/rmsTS[i]/rmsTS[j]);
											
				corr_matrix->SetBinContent(
											corr_matrix->GetBin(j+1,i+1),
											corTS[i][j]/rmsTS[i]/rmsTS[j]);
				
			}
		}
		
		corr_matrix->GetZaxis()->SetRangeUser(-1,1);
		corr_matrix->GetXaxis()->SetTitle("Time Slice");
		corr_matrix->GetYaxis()->SetTitle("Time Slice");
		corr_matrix->Draw("colz text");
		
		c->SaveAs(Form(baseDir+"/CovarianceMatrixPlots/"+fn+"_%i_cor.png", n));
		
		delete corr_matrix;
		corr_matrix = 0;
	}
}


