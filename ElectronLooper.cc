#include <fstream>
#include <iostream>

#include "CMS2.h"
#include "electronSelections.h"

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
  Float_t dielectron_mass,dielectron_mass_lowfbrem,dielectron_mass_onecluster,dielectron_mass_golden;
  TBranch* diElectMass     = redTree->Branch("diElectMass", &dielectron_mass, "dielectron_mass/F");
  TBranch* lowfbremMass    = redTree->Branch("lowfbrem", &dielectron_mass_lowfbrem, "dielectron_mass_lowfbrem/F");
  TBranch* oneclusterMass  = redTree->Branch("onecluster", &dielectron_mass_onecluster, "dielectron_mass_onecluster/F");
  TBranch* goldenMass      = redTree->Branch("golden", &dielectron_mass_golden, "dielectron_mass_golden/F");
  /*---------------------------this is for pt---------------------------------------------------------------------------------------------------------*/
  Float_t  dielectron_pt,dielectron_pt_lowfbrem,dielectron_pt_onecluster,dielectron_pt_golden;
  TBranch* diElectPt       = redTree->Branch("diElectpt", &dielectron_pt, "dielectron_pt/F");
  TBranch* lowfbremPt      = redTree->Branch("lowfbrempt", &dielectron_pt_lowfbrem, "dielectron_pt_lowfbrem/F");
  TBranch* oneclusterPt    = redTree->Branch("oneclusterpt", &dielectron_pt_onecluster, "dielectron_pt_onecluster/F");
  TBranch* goldenPt        = redTree->Branch("goldenpt", &dielectron_pt_golden, "dielectron_pt_golden/F");
   /*here I decided to add eta in my reduiced ntuple in order to be able to change its value for others selections, I declared it as an array because of two electrons*/
  /*eta[0] to get the first electron and eta[1] the second electron so I will be able to do for instance redTree -> Draw("mass","|eta[0]| < 0.8 && |eta[0]| < 0.8","prof")*/
  Float_t electron_eta[2]={0};
  Float_t electron_eta_lowfbrem[2]={0};
  Float_t electron_eta_onecluster[2]={0};
  Float_t electron_eta_golden[2]={0};
  TBranch* ElectronEta      = redTree->Branch("ElectronEta", electron_eta, "electron_eta[2]/F");
  TBranch* lowfbremEta      = redTree->Branch("lowfbremEta", electron_eta_lowfbrem, "electron_pt_lowfbrem[2]/F");
  TBranch* oneclusterEta    = redTree->Branch("oneclusterEta", electron_eta_onecluster, "electron_eta_onecluster[2]/F");
  TBranch* goldenEta        = redTree->Branch("goldenEta", electron_eta_golden, "electron_eta_golden[2]/F");
  /*------------------------this is for energy-----------------------------------------------------------------------------------------------------------*/
  Float_t dielectron_energy,dielectron_energy_lowfbrem,dielectron_energy_onecluster,dielectron_energy_golden;
  TBranch* diElectEn       = redTree->Branch("diElectEn", &dielectron_energy, "dielectron_energy/F");
  TBranch* lowfbremEn      = redTree->Branch("lowfbremEn", &dielectron_energy_lowfbrem, "dielectron_energy_lowfbrem/F");
  TBranch* oneclusterEn    = redTree->Branch("oneclusterEn", &dielectron_energy_onecluster, "dielectron_energy_onecluster/F");
  TBranch* goldenEn        = redTree->Branch("goldenEn", &dielectron_energy_golden, "dielectron_energy_golden/F");
  /*---------------------------------------------------------////-----------------------------------------------------------------------------------------*/
 
  
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
     

      // cout << "I found " << els_p4().size() << " electrons" << endl;      
      // for (int ielec = 0; ielec < els_p4().size(); ielec++) {
      // 	cout << "   Electron " << ielec << endl;
      // 	cout << "       Four-momentum:         " << els_p4().at(ielec).px() << " " << els_p4().at(ielec).py() << " " << els_p4().at(ielec).pz() << " " << els_p4().at(ielec).e() << endl;
      // 	cout << "       eta, phi, pt:          " << els_p4().at(ielec).eta() << " " << els_p4().at(ielec).phi() << " " << els_p4().at(ielec).pt() << endl;
      // 	cout << "       Number of pixel hits:  " << els_valid_pixelhits().at(ielec) << endl;
      // 	cout << "       R9:                    " << els_r9().at(ielec) << endl;
      // }

           if (els_p4().size() < 2) continue;
           if ((electronId_WP2012_v3(0, TIGHT) != PassAllWP2012Cuts) || (electronId_WP2012_v3(1, TIGHT) != PassAllWP2012Cuts)) continue;
      /*----------------------------Filling the mass,  Pt  , En  , Eta with tight eletrons-----------------------------------------------------*/
	   diElectMass->Fill((els_p4().at(0)+els_p4().at(1)).mass(), evt_scale1fb()); 
	   diElectPt->Fill((els_p4().at(0)+els_p4().at(1)), evt_scale1fb());
	   diElectEn->Fill(els_p4().at(0).e()+els_p4().at(1).e(),evt_scale1fb());
	   electron_eta[0]=els_p4().at(0).eta();
	   electron_eta[1]=els_p4().at(1).eta();
	   ElectronEta->Fill(electron_eta[0],electron_eta[1], evt_scale1fb())
      /*-----------------------filling mass, pt, eta, and Energy for lowfbrem------------------------------*/
           if (els_fbrem().at(0) < 0.5 && els_fbrem().at(1) < 0.5) {
	     lowfbremMass->Fill((els_p4().at(0)+els_p4().at(1)).mass(), evt_scale1fb()); 
	     lowfbremPt->Fill((els_p4().at(0)+els_p4().at(1)), evt_scale1fb());
	     electron_eta_lowfbrem[0]=els_p4().at(0).eta();
	     electron_eta_lowfbrem[1]=els_p4().at(1).eta();
	     lowfbremEta->Fill(electron_eta_lowfbrem[0],electron_eta_lowfbrem[1], evt_scale1fb());
	     lowfbremEn->Fill(els_p4().at(0).e()+els_p4().at(1).e(),evt_scale1fb())
      }
      /*----------------------filling -filling mass, pt, eta, and Energy for oncluster---------------------- */
           if (els_nSeed().at(0) == 0 && els_nSeed().at(1) == 0) {
	     oneclusterMass->Fill((els_p4().at(0)+els_p4().at(1)).mass(), evt_scale1fb()); 
	     oneclusterPt->Fill((els_p4().at(0)+els_p4().at(1)), evt_scale1fb());
	     electron_eta_onecluster[0]=els_p4().at(0).eta();
	     electron_eta_onecluster[1]=els_p4().at(1).eta();
	     oneclusterEta->Fill(electron_eta_onecluster[0],electron_eta_onecluster[1], evt_scale1fb());
	     oneclusterEn->Fill(els_p4().at(0).e()+els_p4().at(1).e(),evt_scale1fb())
       /*----------------------filling -filling mass, pt, eta, and Energy for golden---------------------- */
            }

	   if (els_class().at(0) == GOLDEN && els_class().at(1) == GOLDEN) {
	     goldenMass->Fill((els_p4().at(0)+els_p4().at(1)).mass(), evt_scale1fb()); 
	     goldenPt->Fill((els_p4().at(0)+els_p4().at(1)), evt_scale1fb());
	     electron_eta_golden[0]=els_p4().at(0).eta();
	     electron_eta_golden[1]=els_p4().at(1).eta();
	     goldenEta->Fill(electron_eta_golden[0],electron_eta_golden[1], evt_scale1fb());
	     goldenEn->Fill(els_p4().at(0).e()+els_p4().at(1).e(),evt_scale1fb())
      
      }
      
    }
  }

  outputFile->cd();
  //dielectron_mass->Write();
  //dielectron_mass_lowfbrem->Write();
  //  dielectron_mass_onecluster->Write();
  //dielectron_mass_golden->Write();
  redTree->Write();
  outputFile->Close();
  
}
