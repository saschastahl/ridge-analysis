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
// root header
#include <TFile.h>
#include <TH1F.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TVector3.h>
#include <TVectorD.h>
#include <TROOT.h>
#include <TH2F.h>
#include <TBenchmark.h>
#include <TMath.h>
#include <TRandom3.h>
#include <TAxis.h>

//g++ plot_one.cpp  ../codes/source/general.cpp  ./general_plot.cpp `root-config --libs --cflags` -o plot

#include "../codes/headers/constants_cuts.h"
#include "../codes/headers/general.h"
#include "./general_plot.h"

using namespace std;

int pT_up = 0;
int pT_low = 0;

float cut_VELO_up = 0;
float cut_VELO_low = 0;
int VELO_distance = 0;

string plot_name_eps(int dataset, int PT_bin, int nVELO_bin,int type){
    string path = "./";
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
    switch (type){
        case 0:{
            path = path + "pt" + file_PT.str() + "_m" + file_nVELO.str() + "_signal.eps";
            break;
        }
        case 1:{
           path = path + "pt" + file_PT.str() + "_m" + file_nVELO.str() + "_background.eps";
           break;
        }
        case 2:{
           path = path + "pt" + file_PT.str() + "_m" + file_nVELO.str() + "_final.eps";
           break;
        }
        break;
    }
    return path;
}


void plot_one(int dataset, int PT_bin, int nVELO_bin){

 cout << "******** Creation of histograms begins. ********" << endl;
 cout << endl;

 TH1::SetDefaultSumw2();

TH2F *background_add = new TH2F("background_add", "background_add",  NoBins, 0, cut_eta_high-cut_eta_low, NoBins, 0, PI);
TH2F *signal_add = new TH2F("signal_add", "signal_add",  NoBins, 0, cut_eta_high-cut_eta_low, NoBins, 0, PI);
TH2F *final_add = new TH2F("final_add", "final_add",  NoBins, 0, cut_eta_high-cut_eta_low, NoBins, 0, PI);

TVectorD N_trigg(1);
N_trigg[0]= 0;

pT_up_low(PT_bin);
nVELO_up_low(nVELO_bin,dataset);

string adr = "";

int pvz_ttree = int(cut_PVZ_high - cut_PVZ_low) / int(PVZ_distance);
int m_ttree = int(cut_VELO_up - cut_VELO_low) / int(VELO_distance);
for (int f = 0; f < 480; f++){
    for (int pv = 0; pv < pvz_ttree; pv++){
        for (int m = 0; m < m_ttree; m++){
              //for (int velo = 0; velo < 4; velo++){
                    adr = input_subhist_name(f,pv,m,dataset,PT_bin,nVELO_bin,1);
                    nVELO_up_low(nVELO_bin,dataset);
             //       cout << adr << endl;
                    if (file_exists (adr) ){
                    //    cout << adr << endl;
                        TFile *input = TFile::Open(adr.c_str());
                        TH2F *signal_tmp = (TH2F*)input->Get("signal");
                        TVectorD *N = (TVectorD*)input->Get("Ntrigg");
                        N_trigg.Add(N_trigg,*N);
                        signal_add->Add(signal_tmp);
                        input->Close();
                    }
                    adr = input_subhist_name(f,pv,m,dataset,PT_bin,nVELO_bin,0);
                 //   cout << adr << endl;
                    if (file_exists (adr) ){
                       // cout << adr << endl;
                        TFile *input = TFile::Open(adr.c_str());
                        TH2F *background_tmp = (TH2F*)input->Get("background");
                        background_add->Add(background_tmp);
                        input->Close();
                    }
                //}
            }
        }
    cout << f << endl;
}

signal_add->Scale(1.0/N_trigg[0]);

int bin_zerox = background_add->GetXaxis()->FindBin(0.0);
int bin_zeroy = background_add->GetYaxis()->FindBin(0.0);
float B_00 = background_add->GetBinContent(bin_zerox,bin_zeroy);
background_add->Scale(1.0/B_00);

final_add->Divide(signal_add,background_add);

// replot histograms; mirror and shift
TH2F *background_add2 = new TH2F("background_add2", "background_add2", 2*NoBins, -cut_eta_high+cut_eta_low, cut_eta_high-cut_eta_low, 2*NoBins-1, -PI/2, 3*PI/2);
TH2F *signal_add2 = new TH2F("signal_add2", "signal_add2", 2*NoBins, -cut_eta_high+cut_eta_low, cut_eta_high-cut_eta_low, 2*NoBins-1, -PI/2, 3*PI/2);
TH2F *final_add2 = new TH2F("final_add2", "final_add2",  2*NoBins,  -cut_eta_high+cut_eta_low, cut_eta_high-cut_eta_low, 2*NoBins-1, -PI/2, 3*PI/2);

float tmp;


for (int i = 1; i <= NoBins; i++){
    int k = i - 11 + NoBins;
    if (k > 2*NoBins) k = k-2*NoBins;
    int l = k;
    if (NoBins - l <= 0) l = l - 2*NoBins;
   for (int j = 1; j <= NoBins; j++){
       tmp = signal_add->GetBinContent(j,i);
       signal_add2->SetBinContent(NoBins+j,k,tmp);
       signal_add2->SetBinContent(NoBins-j+1,k,tmp);
       signal_add2->SetBinContent(NoBins-j+1,NoBins-l-1,tmp);
       signal_add2->SetBinContent(j+NoBins,NoBins-l-1,tmp);
   }
}

for (int i = 1; i <= NoBins; i++){
    int k = i - 11 + NoBins;
    if (k > 2*NoBins) k = k-2*NoBins;
    int l = k;
    if (NoBins - l <= 0) l = l - 2*NoBins;
   for (int j = 1; j <= NoBins; j++){
       tmp = background_add->GetBinContent(j,i);
       background_add2->SetBinContent(NoBins+j,k,tmp);
       background_add2->SetBinContent(NoBins-j+1,k,tmp);
       background_add2->SetBinContent(NoBins-j+1,NoBins-1 -l,tmp);
       background_add2->SetBinContent(j+NoBins,NoBins-1 -l,tmp);

   }
}

for (int i = 1; i <= NoBins; i++){
    int k = i - 11 + NoBins;
    if (k > 2*NoBins) k = k-2*NoBins;
    int l = k;
    if (NoBins - l <= 0) l = l - 2*NoBins ;
   for (int j = 1; j <= NoBins; j++){
       tmp = final_add->GetBinContent(j,i);
       final_add2->SetBinContent(NoBins+j,k,tmp);
       final_add2->SetBinContent(NoBins-j+1,k,tmp);
       final_add2->SetBinContent(NoBins-j+1,NoBins-1-l,tmp);
       final_add2->SetBinContent(j+NoBins,NoBins-l-1,tmp);
   }
}


background_add->Delete();
signal_add->Delete();
final_add->Delete();


 //-------------------------------------------------------------//
                    /***HISTOGRAM STYLES***/
//-------------------------------------------------------------//

 gStyle->SetOptStat(0);

    //
 set_plot_style();
 design_signal(signal_add2);
 design_background(background_add2);
 design_final(final_add2);

 //-------------------------------------------------------------//
                   /***HISTOGRAM SAVING***/

string path = output_name(dataset,PT_bin,nVELO_bin);
TFile *output_signal = new TFile(path.c_str(),"RECREATE");
output_signal->cd();
signal_add2->Write();//adr.c_str());
background_add2->Write();
final_add2->Write();
output_signal->Close();

background_add2->Delete();
signal_add2->Delete();
final_add2->Delete();

}

int main (){

    for (int dataset = 0; dataset < 2; dataset++){
        for (int pT_bin = 0; pT_bin < 4; pT_bin++){
            for (int nVELO = 0; nVELO < 4; nVELO++){
                plot_one(dataset, pT_bin, nVELO);
            }
        }
    }
    return 0;
}


