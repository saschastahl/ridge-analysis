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
#include <TRandom.h>
#include <TAxis.h>
#include <TF1.h>
#include <TPad.h>

#include "../codes/headers/constants_cuts.h"
#include "./general_plot.h"
#include "../codes/headers/general.h"
//g++ ridge.cpp  ../codes/source/general.cpp  ./general_plot.cpp `root-config --libs --cflags` -o execute


using namespace std;

float cut_VELO_up = 0;
float cut_VELO_low = 0;
int VELO_distance = 0;
int pT_up = 0;
int pT_low = 0;

float minimum (float a, float b, float c){      //minimum of a quadratic fit
    float min = 0;
    min = -b/(2.0*a);
    return min;
}

float ridge(int dataset, int PT_bin, int nVELO_bin){//(int pT){

 cout << "******** Analysis begins. ********" << endl;
 cout << endl;

 TH1::SetDefaultSumw2();
 float min = 0;
 string adr = output_name(dataset,PT_bin,nVELO_bin);
 if (file_exists(adr)){
    TFile *input = TFile::Open(adr.c_str());
    TH2F *final_tmp = (TH2F*)input->Get("final_add2");


     float xmin  = 2;
     float xmax  = 2.5;

     final_tmp->SetAxisRange(xmin,xmax);
     TH1F *project = (TH1F*)final_tmp->ProjectionY()->Clone("project");
     TF1  *f1 = new TF1("f1","[0]*x*x+[1]*x+[2]",-0.1,2);
     f1->SetParameters(0.2, 0.2,0.34);
     project->Scale(1/(project->GetEntries()));

     project->Fit("f1", "r");
     float a = f1->GetParameter(0);
     float b = f1->GetParameter(1);
     float c = f1->GetParameter(2);
     min = minimum(a,b,c);


    //-------------------------------------------------------------//
                       /***HISTOGRAM STYLES***/
     cout << adr << endl;
     TCanvas *c1a = new TCanvas("c1a", "", 10,10,800,800);
     c1a->cd();
     c1a->SetRightMargin(0.03);
     c1a->SetTopMargin(0.03);
     c1a->SetLeftMargin(0.2);
     //c1a->SetBottomMargin(0.04);
     design_ridge(project, kGreen+2);
     project->DrawCopy("P");    //eta
     f1->Draw("same");
     string path = eps_ridge_name(dataset, PT_bin,nVELO_bin);

     cout << path << endl;
     c1a->Print(path.c_str(),"eps");

     c1a->Delete();

     path = output_ridge_name(dataset, PT_bin,nVELO_bin);
     TFile *output_signal = new TFile(path.c_str(),"RECREATE");
     output_signal->cd();
     project->Write();//adr.c_str());
     output_signal->Close();
     input->Close();
 }

return min;

}



void plot_all(int dataset){

    string adr = "";
    float tmp = 0;
    string figure_name = "";
    int bin = 0;
    set_plot_style();
    float min = 0;
    TCanvas *all = new TCanvas("all", "", 10,10,1600,900);
   //all->cd();
    all->Divide(4,4,0.01,0.01,0);
    for (int m = 0; m < 4; m++){
        for (int pT = 0; pT < 4; pT++){
            tmp = 0;
            bin++;
         //   cout << bin << endl;
            adr = output_name(dataset,pT,m);
            if (file_exists(adr)){
                TFile *input = TFile::Open(adr.c_str());
                TH2F *final_tmp = (TH2F*)input->Get("final_add2");
                TH2F *final = new TH2F("final", "final",  2*NoBins,  -cut_eta_high+cut_eta_low, cut_eta_high-cut_eta_low, 2*NoBins-1, -PI/2, 3*PI/2);
                min =  ridge(dataset, pT, m);

                for (int i = 0; i < NoBins; i++){
                    for (int j = 0; j < NoBins; j++){
                        tmp = tmp+final_tmp->GetBinContent(i+1,j+1);
                    }
                }
                for (int i = 1; i < 2*NoBins+1; i++){
                    for (int j = 1; j < 2*NoBins; j++){
                        tmp = final_tmp->GetBinContent(i,j);
                        final->SetBinContent(i,j,tmp-min);
                    }
                }
                final->Scale(1.0/tmp);
                gStyle->SetOptStat(0);
                design_final(final,pT,m);
                all->cd(bin);
                figure_name = plot_title(pT,m);
                final->SetTitle(figure_name.c_str());
                final->DrawCopy("SURF2");
                final_tmp->Delete();
                final->Delete();
                input->Close();
            }
        }
    }
    string save_adr = "";
    if (dataset==0) save_adr = "all_minbias.root";
    if (dataset ==1) save_adr = "all_minbias.root";
    all->SaveAs(save_adr.c_str());
    if (dataset==0) save_adr = "all_high.root";
    if (dataset ==1) save_adr = "all_high.root";
    all->Print("bagr3.pdf","pdf");
    return;

}



int main ( int argc, char* argv[]){

    ofstream myfile ("parameters.txt");
    float min = 0;
    for (int dataset = 0; dataset < 1; dataset++){
        for (int pt = 0; pt < 4; pt++){
            for (int velo = 0; velo < 4; velo++){
                min = ridge(dataset,pt,velo);
                myfile << dataset << "  " << pt << "  " << velo << "  " << min << endl;
            }
        }
    }

    plot_all(0);
    plot_all(1);

    myfile.close();

    return 0;
}


