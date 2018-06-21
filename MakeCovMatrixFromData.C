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


void MakeCovMatrixFromData(
							TString baseDir = "/afs/cern.ch/user/v/vawong/CMSSW_10_1_2/src/RecoLocalCalo/HcalRecAlgos/CoVar",
							TString fn="Data_Cosmics_2018B_Collisions"
							) {
	TTree *it;

    cout << baseDir+"/test_v1.root" << endl;

    //TFile *f = new TFile(baseDir+"/test_v1.root");
    
	
	TFile* f1 = TFile::Open("test_v1.root");			//open the root file

  	TDirectoryFile* d1 = (TDirectoryFile*)f1->Get("mahiDebugger");  //read the directory
  	it = (TTree*) d1->Get("HcalTree");
  	

	if (it==0) return;

	int ieta=0, iphi=0, depth=0;
	uint evt=0, ls=0, run=0;
	float inDarkCurrent;
	float itPulse[10];
	float inputTS[10], inPedestal[10], inNoisePhoto[10], inNoiseADC[10], inNoiseDC[10] = {0};
	double count[10]={0};

	it->SetBranchAddress("ieta", &ieta);
	it->SetBranchAddress("iphi", &iphi);
	it->SetBranchAddress("depth", &depth);
	it->SetBranchAddress("run",&run);
	it->SetBranchAddress("evt",&evt);
	it->SetBranchAddress("ls",&ls);
	it->SetBranchAddress("inDarkCurrent",&inDarkCurrent);
	it->SetBranchAddress("itPulse",&itPulse);
	it->SetBranchAddress("count",&count);
	it->SetBranchAddress("inputTS",&inputTS);
	it->SetBranchAddress("inPedestal",&inPedestal);
	it->SetBranchAddress("inNoisePhoto",&inNoisePhoto);
	it->SetBranchAddress("inNoiseADC",&inNoiseADC);
	it->SetBranchAddress("inNoiseDC",&inNoiseDC);

	Long64_t nEntries = it->GetEntries(); // gets data from root file

	Long64_t nEvents=0;

	double avgTS[8];
	double rmsTS[8];
	double covTS[8][8];
	double corTS[8][8];

	for (int i=0; i<8; i++) { //fills matrices with zero vals
		for(int j=0; j<8; j++) {
			covTS[i][j]=0;
			corTS[i][j]=0;
		}
	}

	TFile *of = new TFile(baseDir+"/CovarianceMatrixFiles/"+fn+".root","recreate");
	TTree *t = new TTree("HcalTree","");

	t->Branch("nEvents",&nEvents, "nEvents/D");
	t->Branch("avgTS",&avgTS,"avgTS[8]/D");
	t->Branch("rmsTS",&rmsTS,"rmsTS[8]/D");
	t->Branch("covTS",&covTS,"covTS[8][8]/D");
	t->Branch("corTS",&corTS,"corTS[8][8]/D");

	for (Long64_t ii=0; ii<nEntries; ii++) {
		it->GetEntry(ii);
		nEvents++; // increments to find the number of entries
		cout << "HERE";
		// itPulse is the charge!!
		for (int i=0; i<8; i++) {
			cout << "inputTS[" << i << "]= " << inputTS[i] << endl;
			Double_t qi = inputTS[i];
			avgTS[i]+=qi; // eventually used for pedestal
			rmsTS[i]+=qi*qi; //variance

			for(int j = 0; j<i+1; j++) {
				Double_t qj = inputTS[j];
				covTS[i][j]+=qi*qj; // covariance
			}
		}
	}

	for (int i=0; i<8; i++) {
		avgTS[i]/=nEvents; // gets the average
		rmsTS[i]/=nEvents;
		rmsTS[i]=sqrt(rmsTS[i]-avgTS[i]*avgTS[i]); // std. deviation
		cout << i << ": " << avgTS[i] << ", " << rmsTS[i] << endl;
	}

	cout << endl;

	for (int i=0; i<8; i++) {
		for (int j=0; j<i+1; j++) {
			covTS[i][j]/=nEvents;
			covTS[i][j]-=avgTS[i]*avgTS[j];
			covTS[j][i] = covTS[i][j];

			// make correlation matrix
			corTS[i][j]=covTS[i][j]/(rmsTS[i]*rmsTS[j]); // rho

			corTS[j][i]=corTS[i][j];

			cout << "array[" << i << "][" << j << "]= " << covTS[i][j] << ";" << endl;
			cout << "array[" << j << "][" << i << "]= " << covTS[j][i] << ";" << endl;

		}
	}

	t->Fill();
	of->Write();
	of->Close();
}



