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

#include <TVector3.h>
#include <TH3F.h>
#include <TH2F.h>
#include <TH1F.h>

#include "../headers/constants_cuts.h"

using namespace std;


string preprocessed_name(int NFile, int PVZ, int nVELO){
    string adr;
    string path_output = "./preprocessed/";
    stringstream file_PVZ;
    stringstream file_VELO;
    file_PVZ << PVZ;
    file_VELO << nVELO;
    stringstream file;
    file << NFile;
    adr = path_output + file.str() + "_PVZ" + file_PVZ.str() + "_VELO" + file_VELO.str() + ".root";
    return adr;
}

string efficiency_name(){
    string path = "/afs/cern.ch/user/r/rkopecna/efficiency/efficiency_all.root";
    return path;
}

void nVELO_up_low(int nVELO_bin, int dataset){   //sets boundaries for nVELO_bin for different datasets
    switch (dataset){
       case 0:{ //minbias
        switch (nVELO_bin){
           case 0:{
               cut_VELO_up = 6000;
               cut_VELO_low = 2400;
               VELO_distance = 200;
               break;
           }
           case 1:{
                cut_VELO_up = 2400;
                cut_VELO_low = 1200;
                VELO_distance = 200;
                break;
           }
           case 2:{
                cut_VELO_up = 1200;
                cut_VELO_low = 600;
                VELO_distance = 20;
                break;
           }
           case 3:{
                cut_VELO_up = 600;
                cut_VELO_low = 10;
                VELO_distance = 200;
                break;
           }
        }
        return;
       }
       case 1:{ //high-multiplicit trigger
        switch (nVELO_bin){
            case 0:{
                cut_VELO_up = 6000; //top 2%
                cut_VELO_low = 3000;
                VELO_distance = 200;
                break;
            }
            case 1:{
                 cut_VELO_up = 3000; //2-3%
                 cut_VELO_low = 2800;
                 VELO_distance = 200;
                 break;
            }
            case 2:{
                 cut_VELO_up = 2800;    //3-4%
                 cut_VELO_low = 2600;
                 VELO_distance = 200;
                 break;
            }
            case 3:{
                 cut_VELO_up = 2600; //4-5%
                 cut_VELO_low = 2400;
                 VELO_distance = 200;
                 break;
            }
            break;
            case 9:{
                cut_VELO_up = 3500;
                cut_VELO_low = 0;
                VELO_distance = 200;
                break;
            }
            break;
        }
        return;
       }
       case 2:{ //mc
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
                 VELO_distance = 300;
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

string nVELO_name(int dataset,int nVELO_bin){
    string path ="/afs/cern.ch/work/r/rkopecna/";
    switch (dataset){
         case 0:{
               path = path + "minbias/";
               switch (nVELO_bin){
                   case 0:{
                       path = path + "0_5/";
                       break;
                   }
                   case 1:{
                       path = path +  "5_30/";
                       break;
                   }
                   case 2:{
                       path = path +  "30_60/";
                       break;
                   }
                   case 3:{
                       path =  path + "60_100/";
                   }
               }
               break;
       }
       case 1:{
              path = path + "high/";
              switch (nVELO_bin){
                  case 0:{
                      path =  path + "0_2/";
                      break;
                  }
                  case 1:{
                     path =  path + "2_3/";
                     break;
                  }
                  case 2:{
                     path = path +  "3_4/";
                     break;
                  }
                  case 3:{
                     path =  path + "4_5/";
                     break;
                  }
              }
              break;
      }
      case 3:{
              path = path + "mc/";
              switch (nVELO_bin){
                  case 0:{
                      path = path +  "0_10/";
                      break;
                  }
                  case 1:{
                     path =  path + "10_30/";
                     break;
                  }
                  case 2:{
                     path =  path + "30_60/";
                     break;
                  }
                  case 3:{
                     path =  path + "60_100/";
                     break;
                  }
              }
              break;
       }
    break;
    }
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

bool cut(TVector3 Momentum, float pT_low, float pT_high){    //returns 1 if track is within cut, else returns 0
       if (pT_low < Momentum.Perp() && pT_high > Momentum.Perp() && Momentum.Mag() > cut_p_low
                   && cut_eta_low < Momentum.PseudoRapidity() && Momentum.PseudoRapidity() < cut_eta_high){
            return 1;
       }
       else return 0;
}

bool select_track(int TRTYPE, float TRGHOSTPROB, float TRIPChi2){
 if (TRTYPE == tracktype && TRGHOSTPROB < cut_ghost && TRIPChi2 < cut_IPCHI)
       return 1;
 else return 0;
}

float weighting(TVector3 Momentum, TH3F efficiency){
     
    double tmp_eta = Momentum.PseudoRapidity();
    double tmp_pT = Momentum.Perp();
    double tmp_phi = Momentum.Phi();
    int bin[3];
    bin[0] = efficiency.GetXaxis()->FindBin(tmp_eta);
    bin[1] = efficiency.GetYaxis()->FindBin(tmp_pT);
    bin[2] = efficiency.GetZaxis()->FindBin(tmp_phi);
    float eff;
    eff = efficiency.GetBinContent(bin[0],bin[1],bin[2]);
    if (eff == 0 ) return 1;
    return (eff);
}

float delta_eta(TVector3 Momentum, TVector3 Momentum2){
   float DeltaEta = TMath::Abs(Momentum.PseudoRapidity() - Momentum2.PseudoRapidity());
   return DeltaEta;
}


float delta_phi(TVector3 Momentum, TVector3 Momentum2){
    float DeltaPhi = 0;
    DeltaPhi = Momentum.Phi() - Momentum2.Phi();
    if (DeltaPhi < -PI) DeltaPhi = DeltaPhi + 2*PI;
    if (DeltaPhi > PI) DeltaPhi = DeltaPhi - 2*PI;
    return (TMath::Abs(DeltaPhi));
}

string hist_name(int NFile, int PVZ, int nVELO, int PT, bool sig){
    stringstream file_PVZ;
    stringstream file_VELO;
    file_PVZ << PVZ;
    file_VELO << nVELO;
    stringstream file;
    file << NFile;
    stringstream file_PT;
    file_PT << PT;
    string path = "./histograms"+ file_PT.str() + "/hist_" + file.str() +"/";
    string adr = "";
    if (sig) adr = path + "sig_PT" + file_PT.str() + "_PVZ" + file_PVZ.str() + "_VELO" + file_VELO.str() + ".root";
     else adr = path + "back_PT" + file_PT.str() + "_PVZ" + file_PVZ.str() + "_VELO" + file_VELO.str() + ".root";
    return adr;
}

bool file_exists (string name) {
    ifstream my_file(name.c_str());
    if (my_file.good())
        return 1;
    else return 0;
}

