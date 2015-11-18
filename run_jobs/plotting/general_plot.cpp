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
#include<TColor.h>

#include "../codes/headers/constants_cuts.h"
#include "../codes/headers/general.h"
#include "./general_plot.h"

//g++ ridge.cpp  ../codes/source/general.cpp  ./general_plot.cpp `root-config --libs --cflags` -o execute


using namespace std;



string input_subhist_name(int NFile, int PVZ, int nM, int dataset, int PT_bin, int nVELO_bin, bool sig){
    string name = hist_name(NFile, PVZ, nM, PT_bin, sig);
    name.erase(name.begin());
    name.erase(name.begin());//erase first  two characters; meaning ./
    string adr = nVELO_name(dataset,nVELO_bin) + name;
    return adr;
}



string output_ridge_name(int dataset, int PT_bin, int nVELO_bin){
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
    path = path + "ridge_pt" + file_PT.str() + "_m" + file_nVELO.str() + ".root";
       return path;
}

string eps_ridge_name(int dataset, int PT_bin, int nVELO_bin){
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
    path = path + "ridge_pt" + file_PT.str() + "_m" + file_nVELO.str() + ".eps";
       return path;
}


string output_name(int dataset, int PT_bin, int nVELO_bin){
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
    path = path + "pt" + file_PT.str() + "_m" + file_nVELO.str() + ".root";
    return path;
}


string plot_title(int pT, int m){

        string m_str;
        string pT_str;
      //break;
        switch (m){
              case 0:{
                    m_str = "Activity: 0 - 10 %";
                    break;
              }
              case 1:{
                    m_str = "Activity: 10 - 30 %";
                    break;
              }
             case 2:{
                    m_str = "Activity: 30 - 60 %";
                    break;
             }
             case 3:{
                    m_str = "Activity: 60 - 100 %";
                    break;
             }
             break;
             case 4:{
                m_str = "Activity: 0 - 5 %";
                break;
             }
          break;
         }

        switch (pT){
              case 0:{
                    pT_str = "p_{T}: 0.15 - 1 GeV";
                    break;
              }
              case 1:{
                    pT_str = "p_{T}: 1 - 2 GeV";
                    break;
              }
             case 2:{
                    pT_str = "p_{T}: 2 - 3 GeV";
                    break;
             }
             case 3:{
                    pT_str = "p_{T}: 3- 5 GeV";
                    break;
             }
             break;
         }

    string title = "#splitline{" + m_str + "}{" + pT_str + "}";
	return title;
}



string final_name(int dataset, int PT_bin, int nVELO_bin){
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
    path = path + "final_pt" + file_PT.str() + "_m" + file_nVELO.str() + ".root";
       return path;
}


void set_plot_style()
{
    const Int_t NRGBs = 5;
    const Int_t NCont = 49;

    Double_t stops[NRGBs] = { 0.00, 0.24, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.91, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);
}


void design_signal(TH2F *signal){
    signal->GetXaxis()->SetTitle("#Delta#eta [MeV]");
    signal->GetYaxis()->SetTitle("#Delta#phi [rad]");
    signal->GetZaxis()->SetTitle("S(#Delta#eta,#Delta#phi)");
    signal->GetXaxis()->SetTitleOffset(1.5);
    signal->GetYaxis()->SetTitleOffset(1.5);
    signal->GetZaxis()->SetTitleOffset(1.5);
    signal->SetTitle("");
}

void design_background(TH2F *background){
    background->GetXaxis()->SetTitle("#Delta#eta [MeV]");
    background->GetYaxis()->SetTitle("#Delta#phi [rad]");
    background->GetZaxis()->SetTitle("B(#Delta#eta,#Delta#phi)/B(0,0)");
    background->GetXaxis()->SetTitleOffset(1.5);
    background->GetYaxis()->SetTitleOffset(1.5);
    background->GetZaxis()->SetTitleOffset(1.5);
}

void design_final(TH2F *final, int pt, int m){
    final->GetXaxis()->SetTitle("#Delta#eta [MeV]");
    final->GetYaxis()->SetTitle("#Delta#phi [rad]");
    final->GetZaxis()->SetTitle("#frac{1}{N_{trig}} #frac{d^{2}N}{d#Delta#eta d#Delta#phi}");
    final->GetXaxis()->SetTitleOffset(1.5);
    final->GetYaxis()->SetTitleOffset(1.4);
    final->GetZaxis()->SetTitleOffset(2.2);
    final->GetXaxis()->SetLabelSize(0.05);
    final->GetYaxis()->SetLabelSize(0.05);
    final->GetZaxis()->SetLabelSize(0.05);
    final->GetXaxis()->SetTitleSize(0.05);
    final->GetYaxis()->SetTitleSize(0.05);
    final->GetZaxis()->SetTitleSize(0.05);
    final->GetXaxis()->SetRangeUser(-2.5,2.5);
    final->SetTitle("");

    if (pt == 0 && m == 0){
        final->GetZaxis()->SetRangeUser(0.0005,0.00056);;
    }
    if (pt == 1 && m == 0){
        final->GetZaxis()->SetRangeUser(0.00045,0.0006);;
    }
    if (pt == 2 && m == 0){
        final->GetZaxis()->SetRangeUser(0.00045,0.0008);;
    }
    if (pt == 3 && m == 0){
        final->GetZaxis()->SetRangeUser(0.00055,0.0009);;
    }


    if (pt == 0 && m == 1){
        final->GetZaxis()->SetRangeUser(0.0005,0.00056);;
    }
    if (pt == 1 && m == 1){
        final->GetZaxis()->SetRangeUser(0.00045,0.0006);;
    }
    if (pt == 2 && m == 1){
        final->GetZaxis()->SetRangeUser(0.0005,0.0008);;
    }
    if (pt == 3 && m == 1){
        final->GetZaxis()->SetRangeUser(0.0005,0.0009);;
    }


    if (pt == 0 && m == 2){
        final->GetZaxis()->SetRangeUser(0.0005,0.00056);;
    }
    if (pt == 1 && m == 2){
        final->GetZaxis()->SetRangeUser(0.00045,0.0006);;
    }
    if (pt == 2 && m == 2){
        final->GetZaxis()->SetRangeUser(0.00045,0.0008);;
    }
    if (pt == 3 && m == 2){
        final->GetZaxis()->SetRangeUser(0.00045,0.0012);;
    }

    if (pt == 0 && m == 3){
        final->GetZaxis()->SetRangeUser(0.0005,0.0007);;
    }
    if (pt == 1 && m == 3){
        final->GetZaxis()->SetRangeUser(0.00045,0.0008);;
    }
    if (pt == 2 && m == 3){
        final->GetZaxis()->SetRangeUser(0.0005,0.0006);;
    }
    if (pt == 3 && m == 3){
        final->GetZaxis()->SetRangeUser(0.0005,0.0006);;
    }


    if (pt == 0 && m == 4){
        final->GetZaxis()->SetRangeUser(0.0005,0.00055);;
    }
    if (pt == 1 && m == 4){
        final->GetZaxis()->SetRangeUser(0.00049,0.00055);;
    }
    if (pt == 2 && m == 4){
        final->GetZaxis()->SetRangeUser(0.0005,0.00065);;
    }
    if (pt == 3 && m == 4){
        final->GetZaxis()->SetRangeUser(0.00045,0.0008);;
    }



}

void design_ridge(TH1F *histogram, Color_t color){
    histogram->SetMarkerStyle(8);
    histogram->SetMarkerColor(color);
    histogram->SetMarkerSize(2);
    histogram->GetYaxis()->SetTitle("   Y(#Delta#phi)");
    histogram->GetXaxis()->SetTitle("#Delta#phi");
    //histogram->GetYaxis()->SetRangeUser(0,0.3);
    histogram->SetLineColor(color);
    histogram->GetXaxis()->SetTitleOffset(1.0);
    histogram->GetYaxis()->SetTitleOffset(1.85);
    //histogram->GetZaxis()->SetTitleOffset(1.5);
    histogram->SetTitle("");
    histogram->GetXaxis()->SetLabelSize(0.05);
    histogram->GetYaxis()->SetLabelSize(0.04);
    histogram->GetXaxis()->SetTitleSize(0.05);
    histogram->GetYaxis()->SetTitleSize(0.05);
}


void design_canvas(TCanvas *canvas){
    canvas->SetRightMargin(0.04);
    canvas->SetTopMargin(0.03);
    canvas->SetLeftMargin(0.0);
    canvas->SetBottomMargin(0.04);
}
