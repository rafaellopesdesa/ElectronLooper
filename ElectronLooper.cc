#include <fstream>
#include <iostream>

#include "CMS2.h"
#include "goodrun.h"
#include "electronSelections.h"
#include "eventSelections.h"

#include "TSystem.h"

using namespace std;
using namespace tas;
using namespace wp2012;

int main(int argc, char** argv) {
//void ElectronLooper(TString inputList) {
// PassAllWP2012Cuts

  enum Classification { UNKNOWN=-1, GOLDEN=0, BIGBREM=1, BADTRACK=2, SHOWERING=3, GAP=4 } ;

  TString reader;
  ifstream fileReader(argv[1]);

  int nfile = -1;
  TString outputDir(".");
  if (argc > 2)
    nfile = TString(argv[2]).Atoi();
  if (argc > 3)
    outputDir = argv[3];
    
  TFile* outputFile;
  if (nfile > 0) {
    outputFile = TFile::Open(TString::Format("%s/ntuple_%d.root", outputDir.Data(), nfile), "recreate");
  } else {
    outputFile = TFile::Open(TString::Format("%s/ntuple.root", outputDir.Data()), "recreate");
  }
  TTree* redTree = new TTree ("redTree","a small Tree");

  LorentzVector electron0_p4;
  LorentzVector electron1_p4;
  LorentzVector dielectron_p4;

  int electron0_class;
  int electron1_class;
  float electron0_fbrem;
  float electron1_fbrem;

  float dielectron_mass;
  float dielectron_pt;

  float scale1fb;
  int run;
  int event;
  int lumi;

  redTree->Branch("electron0_p4", &electron0_p4);
  redTree->Branch("electron1_p4", &electron1_p4);
  redTree->Branch("dielectron_p4", &dielectron_p4);
  redTree->Branch("electron0_class", &electron0_class);
  redTree->Branch("electron1_class", &electron1_class);
  redTree->Branch("electron0_fbrem", &electron0_fbrem);
  redTree->Branch("electron1_fbrem", &electron1_fbrem);
  
  redTree->Branch("dielectron_mass", &dielectron_mass);
  redTree->Branch("dielectron_pt", &dielectron_pt);
  

  redTree->Branch("scale1fb", &scale1fb);
  redTree->Branch("run", &run);
  redTree->Branch("event", &event);
  redTree->Branch("lumi", &lumi);

  
  TString DQdir(gSystem->Getenv("EL_DQDIR"));
  if (DQdir.Length() < 2) DQdir = "./DQ";
  set_goodrun_file(TString::Format("%s/Cert_190456-208686_8TeV_PromptReco_Collisions12_JSON_goodruns.txt", DQdir.Data()));
  
  int ifile = -1; // counts the files
  while (!fileReader.eof()) {

    // for cluster running
    ifile++;
    if (nfile > 0 && nfile != ifile) continue;

    reader.ReadLine(fileReader);
    if (reader.Length() < 2) continue;
    cout << "Opening file " << reader.Data() << endl;

    TFile* fileHandle = TFile::Open(reader);
    TTree* treeHandle = static_cast<TTree*>(fileHandle->Get("Events"));
    Long64_t nentries = treeHandle->GetEntries();
    
    cms2.Init(treeHandle);
    for (Long64_t ientry = 0; ientry < nentries; ientry++) {
      CMS2::progress(ientry,nentries);
      cms2.GetEntry(ientry);
     
      // clean up data
      if (!cleaning_goodVertexApril2011()) continue;
      if( evt_isRealData() && !goodrun(evt_run(), evt_lumiBlock()) ) continue;

      electron0_p4 = LorentzVector(0,0,0,0);
      electron1_p4 = LorentzVector(0,0,0,0);
      
      electron0_class = -999;
      electron1_class = -999;
      electron0_fbrem = -999.;
      electron1_fbrem = -999.;

      scale1fb = -999.;
      run = -999;
      event = -999;
      lumi = -999;

      int electron0_idx = -1;
      int electron1_idx = -1;
      
      // Analysis selection
      for (int ielec=0; ielec<els_p4().size(); ielec++) {
	if ((electronId_WP2012_v3(ielec, TIGHT) == PassAllWP2012Cuts) && electron0_idx < 0) electron0_idx = ielec;
	else if ((electronId_WP2012_v3(ielec, TIGHT) == PassAllWP2012Cuts) && electron0_idx >= 0 && electron1_idx < 0) electron1_idx = ielec;
      }

      if (electron0_idx < 0 || electron1_idx < 0) continue;

      electron0_p4 = els_p4().at(electron0_idx);
      electron1_p4 = els_p4().at(electron1_idx);

      dielectron_p4 = electron0_p4+electron1_p4;
      dielectron_mass = dielectron_p4.mass();
      dielectron_pt = dielectron_p4.pt();
      
      electron0_class = els_class().at(electron0_idx);
      electron1_class = els_class().at(electron1_idx);

      electron0_fbrem = els_fbrem().at(electron0_idx);
      electron1_fbrem = els_fbrem().at(electron1_idx);

      scale1fb = evt_scale1fb();
      run = evt_run();
      event = evt_event();
      lumi = evt_lumiBlock();
      
      redTree->Fill();
    }
  }

  outputFile->cd();
  redTree->Write();
  outputFile->Close();

}
