#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <fstream>
#include <cmath>
#include <iomanip>
    // --- root headers --- //
#include <Riostream.h>
#include <TFile.h>
#include <TH1F.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLeafI.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TVector3.h>
#include <TROOT.h>
#include <TH2F.h>
#include <TBenchmark.h>
#include <TMath.h>
#include <TAxis.h>
#include <TRandom3.h>

#include "../headers/constants_cuts.h"
#include "../headers/general.h"

using namespace std;

// COMPILATION:  g++ main.cpp `root-config --libs --cflags`  -o execute

string dataset_name(int dataset, int NFile){// dataset = 0 ->minbias, dataset = 1 -> high multiplicity, dataset = 2 ->MonteCarlo
 string path;
 switch (dataset){
    case 0:{
        path = "root://eoslhcb.cern.ch//eos/lhcb/user/s/sstahl/ridge/2015/minbias_280/";
        stringstream file;
        file << NFile;
        path = path + "minbias_" + file.str() + ".root";
        break;
    }
    case 1:{
        path = "root://eoslhcb.cern.ch//eos/lhcb/user/s/sstahl/ridge/2015/highMult_291/";
        stringstream file;
        file << NFile;
        path = path + "minbias_" + file.str() + ".root";
        break;
    }
    case 2:{
        path = "root://eoslhcb.cern.ch//eos/lhcb/user/s/sstahl/ridge/mc/";
        path = path + "minbias_MC_merged.root";
        break;
    }
    case 100:{
        cout << "#ERROR: No dataset specified!" << endl;
        path = "";
        break;
    }
    break;
 }
    return path;
}


bool event_cut(int nPV, int nVeloClusters, float PVZ, int pv, int m){
    if (nPV == 1 && nVeloClusters >= cut_VELO_low + m*VELO_distance &&
                nVeloClusters < cut_VELO_up + (m+1)*VELO_distance &&
                nVeloClusters > 10 &&
                PVZ >= cut_PVZ_low + pv*PVZ_distance &&
                PVZ <  cut_PVZ_low + (pv+1)*PVZ_distance)
        return 1;
    else return 0;
}

void rewrite_data(int NFile, int dataset, int VELO_bin){

 nVELO_up_low(VELO_bin, dataset);

 cout << "******** Rewriting data... ********." << endl;
 cout << endl;
 TH1::SetDefaultSumw2();

 // open the file

     string adr;
     adr = dataset_name(dataset,NFile);
     cout << "   * Opening " << adr << endl;
     TFile *input = TFile::Open(adr.c_str());

 // load TTree
  TTree *strom = (TTree*)input->Get("EvtTuple/EventTuple");
            //copy only needed branches
  strom->SetBranchStatus("*",0); 
  strom->SetBranchStatus("nPV",1);
  strom->SetBranchStatus("PVZ", 1);
  strom->SetBranchStatus("TRPX", 1);
  strom->SetBranchStatus("TRPY",1);
  strom->SetBranchStatus("TRPZ", 1);
  strom->SetBranchStatus("TRGHOSTPROB", 1);
  strom->SetBranchStatus("TRTYPE",1);
  strom->SetBranchStatus("nTr", 1);
  strom->SetBranchStatus("nVeloClusters", 1);
  strom->SetBranchStatus("TRIP",1);
  strom->SetBranchStatus("TRIPChi2",1);


  int entries = strom->GetEntries();
  cout << "Entries:" << entries << endl;

 //number of tracks, vertices and other stuff
   int nPV = 0;
   float PVZ[MaxVertices];
   int nVeloClusters;

   strom->SetBranchAddress("nPV", &nPV); //remove pile-up
   strom->SetBranchAddress("PVZ", &PVZ); //pozition vertex z cut
   strom->SetBranchAddress("nVeloClusters", &nVeloClusters); //multiplicity cut

 //creating array of TTrees;
   int pvz_ttree_tmp = int(cut_PVZ_high - cut_PVZ_low) / int(PVZ_distance);
   int m_ttree_tmp = int(cut_VELO_up - cut_VELO_low) / int(VELO_distance);
   const int pvz_ttree = pvz_ttree_tmp;
   const int m_ttree = m_ttree_tmp;

   TFile *output_file[pvz_ttree][m_ttree];
   TTree *output[pvz_ttree][m_ttree];
   for (int pv = 0; pv < pvz_ttree; pv++){
       for (int m = 0; m < m_ttree; m++){
           adr = preprocessed_name(NFile,pv,m);
        //   cout << "adress: " << adr << endl;
           output_file[pv][m] = new TFile(adr.c_str(),"RECREATE");
           output[pv][m] = strom->CloneTree(0);
       }
   }
   cout << "Files and TTrees created." << endl;

    for (int i = 0; i < entries; i++) { //loop over all events
        strom->GetEntry(i);
        for (int pv = 0; pv < pvz_ttree; pv++){
            for (int m = 0; m < m_ttree; m++){
                if (event_cut(nPV,nVeloClusters,PVZ[0],pv,m)){
                     output[pv][m]->Fill();
                }
            } //m
        } // pv
    } // i

    cout << "Ttrees filled." << endl;

 //Saving new TTrees
    for (int pv = 0; pv < pvz_ttree; pv++ ){
        for (int m = 0; m < m_ttree; m++ ){
                  output_file[pv][m]->cd();
                  output[pv][m]->Write();
                  output_file[pv][m]->Close();
        }
    }
    input->Close();
 //deleting???

 cout << endl;
 cout << "******** Preprocessing done. ********." << endl;

}
