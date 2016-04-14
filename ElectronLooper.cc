
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

  TFile* outputFile = TFile::Open("output_histograms.root", "recreate");
  TH1D* dielectron_mass = new TH1D("dielectron_mass", "", 200, 0., 200.);
  TH1D* dielectron_mass_lowfbrem = new TH1D("dielectron_mass_lowfbrem", "", 200, 0., 200.);
  TH1D* dielectron_mass_onecluster = new TH1D("dielectron_mass_onecluster", "", 200, 0., 200.);
  TH1D* dielectron_mass_golden = new TH1D("dielectron_mass_golden", "", 200, 0., 200.);
  
  while (!fileReader.eof()) {

    reader.ReadLine(fileReader);
    if (reader.Length() < 5) continue;
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

      dielectron_mass->Fill((els_p4().at(0)+els_p4().at(1)).mass(), evt_scale1fb()); 
      
      if (els_fbrem().at(0) < 0.5 && els_fbrem().at(1) < 0.5) {
	dielectron_mass_lowfbrem->Fill((els_p4().at(0)+els_p4().at(1)).mass(), evt_scale1fb()); 
      }

      if (els_nSeed().at(0) == 0 && els_nSeed().at(1) == 0) {
	dielectron_mass_onecluster->Fill((els_p4().at(0)+els_p4().at(1)).mass(), evt_scale1fb()); 
      }

      if (els_class().at(0) == GOLDEN && els_class().at(1) == GOLDEN) {
	dielectron_mass_golden->Fill((els_p4().at(0)+els_p4().at(1)).mass(), evt_scale1fb()); 
      }



    }
  }

  outputFile->cd();
  dielectron_mass->Write();
  dielectron_mass_lowfbrem->Write();
  dielectron_mass_onecluster->Write();
  dielectron_mass_golden->Write();
  outputFile->Close();
  
}
