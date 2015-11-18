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
#include <TFile.h>
#include <TH1F.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TVector3.h>
#include <TROOT.h>
#include <TH2F.h>
#include <TBenchmark.h>
#include <TMath.h>
#include <TRandom3.h>
#include <TAxis.h>

#include "constants_cuts.h"

using namespace std;

int pT_up = 0;
int pT_low = 0;


float cut_VELO_up = 0;
float cut_VELO_low = 0;
int VELO_distance = 0;

string nVELO_name(int nVELO_bin){
    string path;
    switch (nVELO_bin){
       case 0:{
           path = "0_10";
           break;
       }
       case 1:{
        path = "10_30";
        break;
       }
       case 2:{
        path = "30_60";
        break;
       }
        case 3:{
         path = "60_100";
         break;
        }
    }
    return path;
}



string input_folder(int dataset, int PT_bin, int nVELO_bin){// dataset = 0 ->minbias, dataset = 1 -> high multiplicity, dataset = 2 ->MonteCarlo
 string path ="/afs/cern.ch/work/r/rkopecna/";
 switch (dataset){
    case 0:{
        stringstream pt;
        pt << PT_bin;

        path = path + "minbias/" + nVELO_name(nVELO_bin) + "/histograms"+ pt.str() ;
        break;
    }
    case 1:{
        stringstream pt;
        pt << PT_bin;
        path = path + "high/" + nVELO_name(nVELO_bin) + "/histograms"+ pt.str();
        break;
    }
    case 2:{
        stringstream pt;
        pt << PT_bin;
        path = path + "mc/"+ nVELO_name(nVELO_bin) + "/histograms" + pt.str() ;
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


bool file_exists (string name) {
    ifstream my_file(name.c_str());
    if (my_file.good())
        return 1;
    else return 0;
}

string hist_name(int NFile, int PVZ, int dataset, int PT_bin, int nVELO_bin){
    stringstream file_PVZ;
    stringstream file_VELO;
    file_PVZ << PVZ;
    file_VELO << nVELO_bin;
    stringstream file;
    file << NFile;
    stringstream file_PT;
    file_PT << PT_bin;
    string path = input_folder(dataset,PT_bin,nVELO_bin) + "/hist_" + file.str() +"/";;
    string adr = path + "PT" + file_PT.str() + "_PVZ" + file_PVZ.str() + "_VELO0" + ".root";
    return adr;
}

string output_name(int dataset, int PT_bin, int nVELO_bin){
    string path = "/afs/cern.ch/user/r/rkopecna/plots/";
    switch (dataset){
       case 0:{
           path = path + "minbias/";
           break;
       }
       case 1:{
        path = path + "high/";
        break;
       }
       case 2:{
        path = path + "mc/";
        break;
       }
       case 100:{
        cout << "#ERROR: No dataset specified!" << endl;
        path = "";
        break;
       }
       break;
    }
    stringstream file_PT;
    file_PT << PT_bin;
    stringstream file_nVELO;
    file_nVELO << nVELO_bin;
    path = path + "pt" + file_PT.str() + "_m" + file_nVELO.str() + ".root";
       return path;

}


void pT_up_low(int PT_bin){
    switch (PT_bin){
       case 0:{
           pT_up = 1000;
           pT_low = 150;
           break;
       }
       case 1:{
            pT_up = 2000;
            pT_low = 1000;
            break;
       }
       case 2:{
            pT_up = 3000;
            pT_low = 2000;
            break;
       }
       case 3:{
            pT_up = 5000;
            pT_low = 3000;
            break;
       }
       break;
    }
    return;
}

void nVELO_up_low(int nVELO_bin, int dataset){      //nVELO_bin = 0
    switch (dataset){
       case 0:{
        switch (nVELO_bin){
           case 0:{
               cut_VELO_up = 6000;
               cut_VELO_low = 1800;
               VELO_distance = 200;
               break;
           }
           case 1:{
                cut_VELO_up = 1800;
                cut_VELO_low = 900;
                VELO_distance = 200;
                break;
           }
           case 2:{
                cut_VELO_up = 900;
                cut_VELO_low = 200;
                VELO_distance = 350;
                break;
           }
           case 3:{
                cut_VELO_up = 200;
                cut_VELO_low = 0;
                VELO_distance = 200;
                break;
           }
        }
        return;
       }
       case 1:{
        switch (nVELO_bin){
            case 0:{
                cut_VELO_up = 3500;
                cut_VELO_low = 3000;
                VELO_distance = 250;
                break;
            }
            case 1:{
                 cut_VELO_up = 3000;
                 cut_VELO_low = 2800;
                 VELO_distance = 200;
                 break;
            }
            case 2:{
                 cut_VELO_up = 2800;
                 cut_VELO_low = 2600;
                 VELO_distance = 200;
                 break;
            }
            case 3:{
                 cut_VELO_up = 2600;
                 cut_VELO_low = 2400;
                 VELO_distance = 200;
                 break;
            }
            break;
        }
        return;
       }
       case 2:{
         switch (nVELO_bin){
            case 0:{
                cut_VELO_up = 6000;
                cut_VELO_low = 1800;
                VELO_distance = 200;
                break;
            }
            case 1:{
                 cut_VELO_up = 1800;
                 cut_VELO_low = 900;
                 VELO_distance = 200;
                 break;
            }
            case 2:{
                 cut_VELO_up = 900;
                 cut_VELO_low = 200;
                 VELO_distance = 350;
                 break;
            }
            case 3:{
                 cut_VELO_up = 200;
                 VELO_distance = 200;
                 cut_VELO_low = 0;
                 break;
            }
            return;
       }
    }
    return;
   }

}


void plot_all(int dataset, int PT_bin, int nVELO_bin){

 cout << "******** Analysis begins. ********" << endl;
 cout << endl;

 //int missing[38];
 //missing {47,70,134,137,142,143,146,149,152,153,158,159,160,161,162,163,170,171,172, 174,177,178,182,181,246,256,255,350,353,362,364,367,376,382,384,388,389,416};


string path = input_folder(dataset,PT_bin,nVELO_bin);

TH2F *background_add = new TH2F("background_add", "background_add",  NoBins, 0, cut_eta_high-cut_eta_low, NoBins, 0, PI);
TH2F *signal_add = new TH2F("signal_add", "signal_add",  NoBins, 0, cut_eta_high-cut_eta_low, NoBins, 0, PI);
TH2F *final_add = new TH2F("final_add", "final_add",  NoBins, 0, cut_eta_high-cut_eta_low, NoBins, 0, PI);
 + nVELO_name(nVELO_bin) + "/";
pT_up_low(PT_bin);
nVELO_up_low(nVELO_bin,dataset);

string adr = "";

int pvz_ttree = int(cut_PVZ_high - cut_PVZ_low) / int(PVZ_distance);
int m_ttree = int(cut_VELO_up - cut_VELO_low) / int(VELO_distance);
for (int f = 1; f < 418; f++){
    for (int pv = 0; pv < pvz_ttree; pv++){
        for (int m = 0; m < m_ttree; m++){
            adr = hist_name(f,pv,dataset,PT_bin,nVELO_bin);
           // cout << adr << endl;
            if (file_exists (adr) ){
                cout << adr << endl;
                TFile *input = TFile::Open(adr.c_str());

                TH2F *background_tmp = (TH2F*)input->Get("background");
                background_add->Add(background_tmp);
                TH2F *signal_tmp = (TH2F*)input->Get("signal");
                signal_add->Add(signal_tmp);

                input->Close();
            }
        }
    }
    cout << f << endl;
}


//float trigger_particle = signal_add->GetEntries();
float trigger_particle = signal_add->Integral();

//cout << "N_trigg = " << trigger_particle << endl;
trigger_particle = (1+TMath::Sqrt(8*trigger_particle+1))/2.0;
cout << "N_trigg = " << trigger_particle << endl;

signal_add->Scale(1.0/trigger_particle);

int bin_zerox = background_add->GetXaxis()->FindBin(0.0);
int bin_zeroy = background_add->GetYaxis()->FindBin(0.0);
float B_00 = background_add->GetBinContent(bin_zerox,bin_zeroy);
background_add->Scale(1.0/B_00);

/*
float nominator = 0;
float denominator = 1;
if (fnl){
    for (int i = 1; i <= NoBins; i++){
       for (int j = 1; j <= NoBins; j++){
            nominator = signal_add->GetBinContent(i,j);
            denominator = background_add->GetBinContent(i,j);
            if (denominator >0.000001){
                final_add->SetBinContent(i,j,nominator/denominator);
            }
       }
    }
}
*/
final_add->Divide(signal_add,background_add);


TH2F *background_add2 = new TH2F("background_add2", "background_add2", 2*NoBins, -cut_eta_high+cut_eta_low, cut_eta_high-cut_eta_low, 2*NoBins, -PI/2, 3*PI/2);
TH2F *signal_add2 = new TH2F("signal_add2", "signal_add2", 2*NoBins, -cut_eta_high+cut_eta_low, cut_eta_high-cut_eta_low, 2*NoBins, -PI/2, 3*PI/2);
TH2F *final_add2 = new TH2F("final_add2", "final_add2",  2*NoBins,  -cut_eta_high+cut_eta_low, cut_eta_high-cut_eta_low, 2*NoBins, -PI/2, 3*PI/2);

float tmp;

for (int i = 1; i <= NoBins; i++){
    int k = i - 23 + NoBins;
    if (k > 2*NoBins) k = k-2*NoBins;
    int l = k;
    if (NoBins - l <= 0) l = l - 2*NoBins;

   for (int j = 1; j <= NoBins; j++){

       tmp = signal_add->GetBinContent(j,i);

       signal_add2->SetBinContent(NoBins+j,k,tmp);
       signal_add2->SetBinContent(NoBins-j+1,k,tmp);

       signal_add2->SetBinContent(NoBins-j+1,NoBins  -l,tmp);
       signal_add2->SetBinContent(j+NoBins,NoBins-l,tmp);
   }
}

for (int i = 1; i <= NoBins; i++){
    int k = i - 23 + NoBins;
    if (k > 2*NoBins) k = k-2*NoBins;
    int l = k;
    if (NoBins - l <= 0) l = l - 2*NoBins;
   for (int j = 1; j <= NoBins; j++){
       tmp = background_add->GetBinContent(j,i);
       background_add2->SetBinContent(NoBins+j,k,tmp);
       background_add2->SetBinContent(NoBins-j+1,k,tmp);

       background_add2->SetBinContent(NoBins-j+1,NoBins -l,tmp);
       background_add2->SetBinContent(j+NoBins,NoBins -l,tmp);

   }
}

for (int i = 1; i <= NoBins; i++){
    int k = i - 23 + NoBins;
    if (k > 2*NoBins) k = k-2*NoBins;
    int l = k;
    if (NoBins - l <= 0) l = l - 2*NoBins ;
   for (int j = 1; j <= NoBins; j++){
       tmp = final_add->GetBinContent(j,i);
       final_add2->SetBinContent(NoBins+j,k,tmp);
       final_add2->SetBinContent(NoBins-j+1,k,tmp);
       final_add2->SetBinContent(NoBins-j+1,NoBins-l,tmp);
       final_add2->SetBinContent(j+NoBins,NoBins-l,tmp);

   }
}


background_add->Delete();
signal_add->Delete();
final_add->Delete();


 //-------------------------------------------------------------//
                    /***HISTOGRAM STYLES***/
 gStyle->SetOptStat(0);

 //
 signal_add2->GetXaxis()->SetTitle("#Delta#eta [MeV]");
 signal_add2->GetYaxis()->SetTitle("#Delta#phi [rad]");
 signal_add2->GetZaxis()->SetTitle("S(#Delta#eta,#Delta#phi)");
 signal_add2->GetXaxis()->SetTitleOffset(1.5);
 signal_add2->GetYaxis()->SetTitleOffset(1.5);
 signal_add2->GetZaxis()->SetTitleOffset(1.5);
  signal_add2->SetTitle("");

 background_add2->GetXaxis()->SetTitle("#Delta#eta [MeV]");
 background_add2->GetYaxis()->SetTitle("#Delta#phi [rad]");
 background_add2->GetZaxis()->SetTitle("B(#Delta#eta,#Delta#phi)/B(0,0)");
 background_add2->GetXaxis()->SetTitleOffset(1.5);
 background_add2->GetYaxis()->SetTitleOffset(1.5);
 background_add2->GetZaxis()->SetTitleOffset(1.5);
 background_add2->SetTitle("");

 final_add2->GetXaxis()->SetTitle("#Delta#eta [MeV]");
 final_add2->GetYaxis()->SetTitle("#Delta#phi [rad]");
 //final_add2->GetZaxis()->SetTitle("#frac{1}{N_{trig}}#frac{d^{2}N}{d#Delta#eta d#Delta#phi}");
 final_add2->GetXaxis()->SetTitleOffset(1.5);
 final_add2->GetYaxis()->SetTitleOffset(1.5);
 final_add2->GetZaxis()->SetTitleOffset(1.5);
 final_add2->GetXaxis()->SetRangeUser(-2.9,2.9);
 final_add2->SetTitle("");

 cout << "Events fine." << endl;



 TCanvas *c1 = new TCanvas("c1", " ", 10,10,600,800);
 c1->cd();
 signal_add2->Draw("SURF1");    //eta

 TCanvas *c1a = new TCanvas("c1a", " ", 10,10,600,800);
 c1a->cd();
 signal_add2->ProjectionX()->Draw();    //eta

 TCanvas *c1b = new TCanvas("c1b", " ", 10,10,600,800);
 c1b->cd();
 signal_add2->ProjectionY()->Draw();

 /*****/

 TCanvas *c2 = new TCanvas("c2", " ", 10,10,600,800);
 c2->cd();
 background_add2->Draw("SURF1");

 TCanvas *c2a = new TCanvas("c2a", " ", 10,10,600,800);
 c2a->cd();
 background_add2->ProjectionX()->Draw();

 TCanvas *c2b = new TCanvas("c2b", " ", 10,10,600,800);
 c2b->cd();
 background_add2->ProjectionY()->Draw();

 /*****/

 TCanvas *c3 = new TCanvas("c3", " ", 10,10,600,800);
 c3->cd();
 final_add2->Draw("SURF1");

 TCanvas *c3a = new TCanvas("c3a", " ", 10,10,600,800);
 c3a->cd();
 final_add2->ProjectionX()->Draw();

  TCanvas *c3b = new TCanvas("c3b", " ", 10,10,600,800);
 c3b->cd();
 final_add2->ProjectionY()->Draw();


 //-------------------------------------------------------------//
                   /***HISTOGRAM SAVING***/

path = output_name(dataset,PT_bin,nVELO_bin);
TFile *output_signal = new TFile(path.c_str(),"RECREATE");
output_signal->cd();
signal_add2->Write();//adr.c_str());
background_add2->Write();
final_add2->Write();

output_signal->Close();

}


