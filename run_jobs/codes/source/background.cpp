#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <sstream>
#include <fstream>
#include <cmath>
#include <iomanip>
    // --- root headers --- //
#include <TFile.h>
#include <TH1F.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TVector3.h>
#include <TROOT.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TBenchmark.h>
#include <TMath.h>
#include <TRandom3.h>
#include <TAxis.h>

#include "../headers/constants_cuts.h"
#include "../headers/general.h"

using namespace std;

void get_random(int random[], int entries, int i){      //makes an array of selected random events
    TRandom3 r; //seed ok
    int select = 0;
    for (int e = 0; e < mixing; e++){
        do {
            random[e] = r.Integer(entries);
            select = 0;
            for (int ee = 0; ee < e; ee++){
                if (random[e] == random[ee]) select++;
            }
        } while (random[e] == i || select > 0);
    }
    return;
}


void get_data2(int NFile, int NPVZ, int Nm, int PT_bin){ // PT_bin: 0=> 0.15-1, 1=>1-2, 2=>2-3, 3=>3-5 GeV

 TH1::SetDefaultSumw2();
 string adr;
 adr = preprocessed_name(NFile,NPVZ,Nm);
// cout << adr << endl;

       // open the file
 TFile *input = TFile::Open(adr.c_str());
 TFile *correction = TFile::Open(efficiency_name().c_str());

      //create hitograms
 TH2F *background = new TH2F("background", "background",  NoBins, 0, cut_eta_high-cut_eta_low, NoBins, 0, PI);
 TH3F *efficiency = (TH3F*)correction->Get("total_efficiency");

     //get Ttree and the data from it
  TTree *strom = (TTree*)input->Get("EventTuple");
  if (strom->GetEntries() < mixing+1) {     //skip files with less than 6 events
      return;
  }

  strom->SetBranchStatus("PVZ", 0);
  strom->SetBranchStatus("nPV", 0);

  float TRPX[MaxTracks];
  float TRPY[MaxTracks];
  float TRPZ[MaxTracks];
  float TRIPChi2[MaxTracks];
  float TRGHOSTPROB[MaxTracks];
  float TRTYPE[MaxTracks];

  float PVZ[MaxVertices];

  int nTr = 0;
  int entries = strom->GetEntries();
   //vertex position
        strom->SetBranchAddress("PVZ", &PVZ);
   //momenta composition
        strom->SetBranchAddress("TRPX", &TRPX);
        strom->SetBranchAddress("TRPY", &TRPY);
        strom->SetBranchAddress("TRPZ", &TRPZ);
   //number of tracks, vertices and other stuff
        strom->SetBranchAddress("nTr", &nTr);
        strom->SetBranchAddress("TRTYPE", &TRTYPE);
        strom->SetBranchAddress("TRIPChi2",&TRIPChi2);
   // ghost or real?
        strom->SetBranchAddress("TRGHOSTPROB", &TRGHOSTPROB); //probability, that the track is a ghost


 TVector3 Momentum;
 TVector3 Momentum2;

 float DeltaPhi = 0;
 float DeltaEta = 0;
 float weight;
 float weight2;

 int random[mixing];        //initialize array
 for (int ee= 0; ee < mixing; ee++){
     random[ee]=0;
 }

   //*********** background *****************//
        for (int i = 0; i < entries; i++) {
           strom->GetEntry(i);
           get_random(random,entries,i);
            for (int j = 0; j < nTr; j++){
                if (select_track( TRTYPE[j], TRGHOSTPROB[j],TRIPChi2[j]) ){ //tracktype and ghostrack cut
                    Momentum.SetXYZ(TRPX[j], TRPY[j],TRPZ[j]); //cut
                    weight = weighting(Momentum,*efficiency);       //weight for the first track
                    if (cut(Momentum,pT_low,pT_up)){
                        for (int e = 0; e < mixing; e++){
                            strom->GetEntry(random[e]);
                            for (int m = 0; m < nTr; m++){
                                if (select_track( TRTYPE[m], TRGHOSTPROB[m],TRIPChi2[m])  ){
                                    Momentum2.SetXYZ(TRPX[m], TRPY[m],TRPZ[m]);
                                    weight2 = weighting(Momentum2,*efficiency);  //weight for the second track
                                    if (cut(Momentum2,pT_low,pT_up)){
                                        DeltaPhi = delta_phi(Momentum,Momentum2);
                                        DeltaEta = delta_eta(Momentum,Momentum2);
                                        background->Fill(DeltaEta, DeltaPhi, weight*weight2);
                                              //   cout << "background filling " << m << "  " << e << endl;
                                    } //second cut
                                } //track if
                            } //track loop
                        } //event mixing loop
                    }// cut if
                    strom->GetEntry(i);//returns the tree back to the proper event
                } //tracktype if
            } //loop over tracks
         //   if (i%1000 == 0) cout << "Backgorund " << i << " analyzed." << endl;
       } //loop over events

  /*****************************************/

 //-------------------------------------------------------------//
                    /***HISTOGRAM STYLES***/
 gStyle->SetOptStat(0);

 background->GetXaxis()->SetTitle("#Delta#eta [MeV]");
 background->GetYaxis()->SetTitle("#Delta#phi [rad]");
 background->GetZaxis()->SetTitle("B(#Delta#eta,#Delta#phi)");

 //-------------------------------------------------------------//

        // histogram saving
 adr = hist_name(NFile,NPVZ,Nm,PT_bin,0);
 TFile *output_signal = new TFile(adr.c_str(),"RECREATE");
 output_signal->cd();
 background->Write();//adr.c_str());
 output_signal->Close();
 background->Delete();

 // cout << "******** Background analysis done. ********." << endl;

  input->Close();
}

void background(int NFile,int PT_bin){

   // cout << cut_VELO_up << " " << cut_VELO_low << endl;
    int pvz_ttree = int(cut_PVZ_high - cut_PVZ_low) / int(PVZ_distance);
    int m_ttree = int(cut_VELO_up - cut_VELO_low) / int(VELO_distance);
        for (int pv = 0; pv < pvz_ttree; pv++){
            for (int m = 0; m < m_ttree; m++){
                get_data2(NFile,pv,m,PT_bin);
            }
        }
}

