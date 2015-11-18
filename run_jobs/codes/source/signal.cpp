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
#include <TVectorD.h>
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



void get_data(int NFile, int NPVZ, int Nm, int PT_bin){ // PT_bin: 0=> 0.15-1, 1=>1-2, 2=>2-3, 3=>3-5 GeV

 TH1::SetDefaultSumw2();
 string adr;
 adr = preprocessed_name(NFile,NPVZ,Nm);
 //cout << adr << endl;

       // open the files
 TFile *input = TFile::Open(adr.c_str());
 TFile *correction = TFile::Open(efficiency_name().c_str());

      //create histograms
 TH2F *signal = new TH2F("signal", "signal",  NoBins, 0, cut_eta_high-cut_eta_low, NoBins, 0, PI);
 TH3F *efficiency = (TH3F*)correction->Get("total_efficiency");
      //get Ttree and the data from it
  TTree *strom = (TTree*)input->Get("EventTuple");
  if (strom->GetEntries() < mixing+1) {   //skip files with less than 6 events
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
 float N_trigg = 0;
 int entries = strom->GetEntries();

 // Get N_trigg
 for (int i = 0; i < entries; i++) {
      strom->GetEntry(i);
      for (int j = 0; j < nTr; j++){
        Momentum.SetXYZ(TRPX[j], TRPY[j],TRPZ[j]);
        if ( select_track( TRTYPE[j], TRGHOSTPROB[j],TRIPChi2[j] ) ){ //tracktype and ghostrack cut
             if (cut(Momentum,pT_low,pT_up)){ //cut
                 weight = weighting(Momentum,*efficiency);
                 N_trigg = N_trigg + weight;
             } //first cut
        } //typetrack if
      } //loop over tracks
  } //loop over events
 //cout << N_trigg << endl;

     //*********** signal *****************//
        for (int i = 0; i < entries; i++) {
             strom->GetEntry(i);
             for (int j = 0; j < nTr; j++){
               Momentum.SetXYZ(TRPX[j], TRPY[j],TRPZ[j]);
               weight = weighting(Momentum,*efficiency);  //weight for the first track
               if ( select_track( TRTYPE[j], TRGHOSTPROB[j],TRIPChi2[j] ) ){ //tracktype and ghostrack cut
                    if (cut(Momentum,pT_low,pT_up)){ //cut
                        //second loop
                        for (int k = j+1; k < nTr; k++){
                              Momentum2.SetXYZ(TRPX[k], TRPY[k],TRPZ[k]);
                              weight2 = weighting(Momentum2,*efficiency);  //weight for the second track
                              if (select_track( TRTYPE[k], TRGHOSTPROB[k],TRIPChi2[k]) ){                                  
                                  if (cut(Momentum2,pT_low,pT_up)){
                                      DeltaPhi = delta_phi(Momentum,Momentum2);
                                      DeltaEta = delta_eta(Momentum,Momentum2);
                                      signal->Fill(DeltaEta, DeltaPhi, weight*weight2);
                                  } //second cut
                              } //tracktype if
                          }// second loop over tracks
                    } //first cut
               } //typetrack if
             } //loop over tracks
         } //loop over events


 //-------------------------------------------------------------//
                    /***HISTOGRAM STYLES***/
 gStyle->SetOptStat(0);

 signal->GetXaxis()->SetTitle("#Delta#eta [MeV]");
 signal->GetYaxis()->SetTitle("#Delta#phi [rad]");
 signal->GetZaxis()->SetTitle("S(#Delta#eta,#Delta#phi)");


  //-------------------------------------------------------------//

    //histogram saving
 adr = hist_name(NFile,NPVZ,Nm,PT_bin,1);
 TFile *output_signal = new TFile(adr.c_str(),"RECREATE");
 output_signal->cd();
 TVectorD Ntrigg(1);        //creating a TVector for saving N_trigg
 Ntrigg[0] = N_trigg;
 signal->Write();//adr.c_str());
 Ntrigg.Write("Ntrigg");

 output_signal->Close();
 signal->Delete();

  // cout << "******** Signal analysis done. ********." << endl;

  input->Close();
}

void signal(int NFile,int PT_bin, int dataset, int nVELO_bin){

    nVELO_up_low(nVELO_bin,dataset);
    cout << cut_VELO_up << " " << cut_VELO_low << endl;
    int pvz_ttree = int(cut_PVZ_high - cut_PVZ_low) / int(PVZ_distance);
    int m_ttree = int(cut_VELO_up - cut_VELO_low) / int(VELO_distance);
        for (int pv = 0; pv < pvz_ttree; pv++){
            for (int m = 0; m < m_ttree; m++){
                get_data(NFile,pv,m,PT_bin);
            }
        }
}

