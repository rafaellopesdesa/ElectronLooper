#include <fstream>
#include <iostream>

#include "CMS2.h"
#include "goodrun.h"
#include "electronSelections.h"
#include "eventSelections.h"

using namespace std;
using namespace tas;
using namespace wp2012;

int main(int argc, char** argv) {
//void ElectronLooper(TString inputList) {
// PassAllWP2012Cuts

  enum Classification { UNKNOWN=-1, GOLDEN=0, BIGBREM=1, BADTRACK=2, SHOWERING=3, GAP=4 } ;

  TString reader;
  ifstream fileReader(argv[1]);

  TFile* outputFile = TFile::Open("tight_small_Tree.root", "recreate");
  TTree* redTree = new TTree ("redTree","a small Tree");
   /*------------------------------ this is for the mass---------------------------------------------------------------------------------------------*/
   //Float_t dielectron_mass;
  //  Float_t dielectron_mass,dielectron_mass_lowfbrem,dielectron_mass_onecluster,dielectron_mass_golden;
  // RCLSA: this is a tree, not a histogram
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

  
  // redTree->Branch("diElectMass", &dielectron_mass, "diElectMass/F");
  // redTree->Branch("lowfbremMass", &dielectron_mass_lowfbrem, "lowfbremMass/F");
  // redTree->Branch("oneclusterMass", &dielectron_mass_onecluster, "oneclusterMass/F");
  // redTree->Branch("goldenMass", &dielectron_mass_golden, "goldenMass/F");
  // /*---------------------------this is for pt---------------------------------------------------------------------------------------------------------*/
  
  // Float_t  dielectron_pt,dielectron_pt_lowfbrem,dielectron_pt_onecluster,dielectron_pt_golden;
  // redTree->Branch("diElectPt", &dielectron_pt, "diElectPt/F");
  // redTree->Branch("lowfbremPt", &dielectron_pt_lowfbrem, "lowfbremPt/F");
  // redTree->Branch("oneclusterPt", &dielectron_pt_onecluster, "oneclusterPt/F");
  // redTree->Branch("goldenPt", &dielectron_pt_golden, "goldenPt/F");
  //  /*here I decided to add eta in my reduiced ntuple in order to be able to change its value for others selections, I declared it as an array because of two electrons*/
  // /*eta[0] to get the first electron and eta[1] the second electron so I will be able to do for instance redTree -> Draw("mass","|eta[0]| < 0.8 && |eta[0]| < 0.8","prof")*/
  // Float_t electron_eta[2]={0};
  // Float_t electron_eta_lowfbrem[2]={0};
  // Float_t electron_eta_onecluster[2]={0};
  // Float_t electron_eta_golden[2]={0};
  // redTree->Branch("ElectronEta", electron_eta, "ElectronEta[2]/F");
  // redTree->Branch("lowfbremEta", electron_eta_lowfbrem, "lowfbremEta[2]/F");
  // redTree->Branch("oneclusterEta", electron_eta_onecluster, "oneclusterEta[2]/F");
  // redTree->Branch("goldenEta", electron_eta_golden, "goldenEta[2]/F");
  // /*------------------------this is for energy-----------------------------------------------------------------------------------------------------------*/
  // Float_t dielectron_energy,dielectron_energy_lowfbrem,dielectron_energy_onecluster,dielectron_energy_golden;
  // redTree->Branch("diElectEn", &dielectron_energy, "diElectEn/F");
  // redTree->Branch("lowfbremEn", &dielectron_energy_lowfbrem, "lowfbremEn/F");
  // redTree->Branch("oneclusterEn", &dielectron_energy_onecluster, "oneclusterEn/F");
  // redTree->Branch("goldenEn", &dielectron_energy_golden, "goldenEn/F");
  // /*---------------------------------------------------------////-----------------------------------------------------------------------------------------*/
 
  set_goodrun_file("DQ/Cert_190456-208686_8TeV_PromptReco_Collisions12_JSON_goodruns.txt");
  
  while (!fileReader.eof()) {

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

      // cout << "I found " << els_p4().size() << " electrons" << endl;      
      // for (int ielec = 0; ielec < els_p4().size(); ielec++) {
      // 	cout << "   Electron " << ielec << endl;
      // 	cout << "       Four-momentum:         " << els_p4().at(ielec).px() << " " << els_p4().at(ielec).py() << " " << els_p4().at(ielec).pz() << " " << els_p4().at(ielec).e() << endl;
      // 	cout << "       eta, phi, pt:          " << els_p4().at(ielec).eta() << " " << els_p4().at(ielec).phi() << " " << els_p4().at(ielec).pt() << endl;
      // 	cout << "       Number of pixel hits:  " << els_valid_pixelhits().at(ielec) << endl;
      // 	cout << "       R9:                    " << els_r9().at(ielec) << endl;
      // }

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
      // /*----------------------------Filling the mass,  Pt  , En  , Eta with tight eletrons-----------------------------------------------------*/
      // 	   dielectron_mass = (els_p4().at(0)+els_p4().at(1)).mass(); 
      // 	   //cout << els_p4().at(0).pt()+els_p4().at(1).pt()<< endl;
      // 	   dielectron_pt = els_p4().at(0).pt()+els_p4().at(1).pt();
      // 	   dielectron_energy = els_p4().at(0).e()+els_p4().at(1).e();
      // 	   electron_eta[0]=els_p4().at(0).eta();
      // 	   electron_eta[1]=els_p4().at(1).eta();
	   
      // /*-----------------------filling mass, pt, eta, and Energy for lowfbrem------------------------------*/
      //      if (els_fbrem().at(0) < 0.5 && els_fbrem().at(1) < 0.5) {
      // 	   dielectron_mass_lowfbrem = (els_p4().at(0)+els_p4().at(1)).mass(); 
      // 	   dielectron_pt_lowfbrem = els_p4().at(0).pt()+els_p4().at(1).pt();
      // 	   dielectron_energy_lowfbrem = els_p4().at(0).e()+els_p4().at(1).e();
      // 	   electron_eta_lowfbrem[0]=els_p4().at(0).eta();
      // 	   electron_eta_lowfbrem[1]=els_p4().at(1).eta();
      // 	    }
      // /*----------------------filling -filling mass, pt, eta, and Energy for oncluster---------------------- */
      // 	    if (els_nSeed().at(0) == 0 && els_nSeed().at(1) == 0) {
      // 	   dielectron_mass_onecluster = (els_p4().at(0)+els_p4().at(1)).mass(); 
      // 	   dielectron_pt_onecluster = els_p4().at(0).pt()+els_p4().at(1).pt();
      // 	   electron_eta_onecluster[0]=els_p4().at(0).eta();
      // 	   electron_eta_onecluster[1]=els_p4().at(1).eta();
      // 	   dielectron_energy_onecluster = els_p4().at(0).e()+els_p4().at(1).e();
      //  /*----------------------filling -filling mass, pt, eta, and Energy for golden---------------------- */
      // 	   }

      // 	   if (els_class().at(0) == GOLDEN && els_class().at(1) == GOLDEN) {
      // 	    dielectron_mass_golden  = (els_p4().at(0)+els_p4().at(1)).mass(); 
      // 	    dielectron_pt_golden = els_p4().at(0).pt()+els_p4().at(1).pt();
      // 	    electron_eta_golden[0]=els_p4().at(0).eta();
      // 	    electron_eta_golden[1]=els_p4().at(1).eta();
      // 	    dielectron_energy_golden = els_p4().at(0).e()+els_p4().at(1).e();
      
      // 	     }
      // redTree->Fill();
    }
  }

  outputFile->cd();
  //dielectron_mass->Write();
  //dielectron_mass_lowfbrem->Write();
  //  dielectron_mass_onecluster->Write();
  //dielectron_mass_golden->Write();
  //redTree->Fill();
  redTree->Write();
  outputFile->Close();
  
}
