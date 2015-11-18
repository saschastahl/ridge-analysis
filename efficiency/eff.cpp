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
#include <TH3F.h>
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
#include <TColor.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TAttMarker.h>

#include "../run_jobs/codes/headers/constants_cuts.h"

using namespace std;

float cut_pT_low = 0;
float cut_pT_high = 5000;

bool cut_neutral(int PID){
    PID = abs(PID);
    if (PID == 22 || PID == 111 || PID == 2112 || PID == 130 || PID == 310 || PID == 311) return 0;
            //exclude n_0, K_0, pi_0 and gamma
    else return 1;
}

bool cut(TVector3 Momentum, float Prompt, float IPChi){    //returns 1 if track is within cut, else returns 0
       if (Momentum.Mag() > cut_p_low
                   && Momentum.Perp() > cut_pT_low && Momentum.Perp() < cut_pT_high
                   && cut_eta_low < Momentum.PseudoRapidity() && Momentum.PseudoRapidity() < cut_eta_high
                   && Prompt == 1 && IPChi < cut_IPCHI){
            return 1;
       }
       else return 0;
}

bool cut_event (int nPV){
    if (nPV ==1) return 1;
    else return 0;
}

string file_name(int NFile, int Nfold){
   stringstream fold;
   fold << Nfold;
   string path = "root://eoslhcb.cern.ch//eos/lhcb/user/s/sstahl/ridge/mc_28" + fold.str() + "/";
   stringstream file;
   file << NFile;
   path = path + "minbias_" + file.str() + ".root";
   return path;
}

void design_markers(TH1F *histogram, string xname, string yname, Color_t color){
    histogram->SetMarkerStyle(8);
    histogram->SetMarkerColor(color);
    histogram->GetXaxis()->SetTitle(xname.c_str());
    histogram->GetYaxis()->SetTitle(yname.c_str());
    //histogram->GetYaxis()->SetRangeUser(0,0.3);
    histogram->GetYaxis()->SetTitleOffset(1.5);
    histogram->SetLineColor(color);
}
void design_lines(TH1F *histogram, string xname, string yname, Color_t color){
    histogram->SetLineWidth(3);
    //histogram->SetMarkerColor(color);
    histogram->GetXaxis()->SetTitle(xname.c_str());
    histogram->GetYaxis()->SetTitle(yname.c_str());
    histogram->GetYaxis()->SetTitleOffset(1.5);
    histogram->SetLineColor(color);
}

void design_canvas(TCanvas *canvas){
    canvas->SetRightMargin(0.04);
    canvas->SetTopMargin(0.04);
    canvas->SetLeftMargin(0.12);
}



void eff(){

 cout << "******** Analysis begins. ********" << endl;
 cout << endl;

 TH1::SetDefaultSumw2();

 const float PtLowBin = 150;
 const float PtHighBin = 5000;
 const float EtaLowBin = cut_eta_low;
 const float EtaHighBin = cut_eta_high;
 const float VeloLowBin = 10;
 const float VeloHighBin = 6000;
 const int NoBins_3D = 30;
 const int NoBins_default = 100;

 TH1F *p_rec = new TH1F("p_rec", "",  NoBins_default, PtLowBin, PtHighBin);
 TH1F *eta_rec = new TH1F("eta_rec", "", NoBins_default, EtaLowBin, EtaHighBin);
 TH1F *phi_rec = new TH1F("phi_rec", "", NoBins_default, -PI, PI);
 TH3F *rec = new TH3F("rec", "", NoBins_3D, EtaLowBin, EtaHighBin, NoBins_3D, PtLowBin, PtHighBin, NoBins_3D, -PI, PI);

 TH1F *p_reb = new TH1F("p_reb", "",  NoBins_default, PtLowBin, PtHighBin);
 TH1F *eta_reb = new TH1F("eta_reb", "", NoBins_default, EtaLowBin, EtaHighBin);
 TH1F *phi_reb = new TH1F("phi_reb", "", NoBins_default, -PI, PI);
 TH3F *reb = new TH3F("reb", "", NoBins_3D, EtaLowBin, EtaHighBin,NoBins_3D, PtLowBin, PtHighBin, NoBins_3D, -PI, PI);

 TH1F *p_mc = new TH1F("p_mc", "",  NoBins_default, PtLowBin, PtHighBin);
 TH1F *eta_mc = new TH1F("eta_mc", "", NoBins_default, EtaLowBin, EtaHighBin);
 TH1F *phi_mc = new TH1F("phi_mc", "", NoBins_default, -PI, PI);
 TH3F *mc = new TH3F("mc", "", NoBins_3D, EtaLowBin, EtaHighBin, NoBins_3D, PtLowBin, PtHighBin, NoBins_3D, -PI, PI);

 TH1F *p_ghost = new TH1F("p_ghost", "",  NoBins_default, PtLowBin, PtHighBin);
 TH1F *eta_ghost = new TH1F("eta_ghost", "", NoBins_default, EtaLowBin, EtaHighBin);
 TH1F *phi_ghost = new TH1F("phi_ghost", "", NoBins_default, -PI, PI);
 TH1F *p_ghost_eff = new TH1F("p_ghost_eff", "", NoBins_default, PtLowBin, PtHighBin);
 TH1F *eta_ghost_eff = new TH1F("eta_ghost_eff", "", NoBins_default, EtaLowBin, EtaHighBin);
 TH1F *phi_ghost_eff = new TH1F("phi_ghost_eff", "", NoBins_default, -PI, PI);
 TH3F *ghost = new TH3F("ghost", "", NoBins_3D, EtaLowBin, EtaHighBin, NoBins_3D, PtLowBin, PtHighBin, NoBins_3D, -PI, PI);

 TH1F *p_prompt = new TH1F("p_prompt", "",  NoBins_default, PtLowBin, PtHighBin);
 TH1F *eta_prompt = new TH1F("eta_prompt", "", NoBins_default, EtaLowBin, EtaHighBin);
 TH1F *phi_prompt = new TH1F("phi_prompt", "", NoBins_default, -PI, PI);
 TH1F *p_prompt_eff = new TH1F("p_prompt_eff", "", NoBins_default, PtLowBin, PtHighBin);
 TH1F *eta_prompt_eff = new TH1F("eta_prompt_eff", "", NoBins_default, EtaLowBin, EtaHighBin);
 TH1F *phi_prompt_eff = new TH1F("phi_prompt_eff", "", NoBins_default, -PI, PI);
 TH3F *prompt = new TH3F("prompt", "", NoBins_3D, EtaLowBin, EtaHighBin, NoBins_3D, PtLowBin, PtHighBin, NoBins_3D, -PI, PI);


 TH1F *effective_p = new TH1F("effective_p", "",  NoBins_default, PtLowBin, PtHighBin);
 TH1F *acceptance_p = new TH1F("acceptance_p", "", NoBins_default, PtLowBin, PtHighBin);
 TH1F *effective_phi = new TH1F("effective_phi", "", NoBins_default, -PI, PI);
 TH1F *effective_eta = new TH1F("effective_eta", "",  NoBins_default, EtaLowBin, EtaHighBin);
 TH1F *acceptance_eta = new TH1F("acceptance_eta", "", NoBins_default, EtaLowBin, EtaHighBin);
 TH1F *acceptance_phi = new TH1F("acceptance_phi", "", NoBins_default, -PI, PI);

 TH3F *effective = new TH3F("effective", "", NoBins_3D, EtaLowBin, EtaHighBin, NoBins_3D, PtLowBin, PtHighBin, NoBins_3D, -PI, PI);
 TH3F *acceptance = new TH3F("acceptance", "",NoBins_3D, EtaLowBin, EtaHighBin, NoBins_3D, PtLowBin, PtHighBin, NoBins_3D, -PI, PI);

 TH1F *acc_eff_p = new TH1F("acc_eff_p", "",NoBins_default, PtLowBin, PtHighBin);
 TH1F *acc_eff_eta = new TH1F("acc_eff_eta", "",NoBins_default, EtaLowBin, EtaHighBin);
 TH1F *acc_eff_phi = new TH1F("acc_eff_phi", "", NoBins_default, -PI, PI);
 TH3F *acc_eff = new TH3F("acc_eff", "",NoBins_3D, EtaLowBin, EtaHighBin, NoBins_3D, PtLowBin, PtHighBin, NoBins_3D, -PI, PI);

 TH1F *p = new TH1F("p", "",NoBins_default, PtLowBin, PtHighBin);
 TH1F *eta = new TH1F("eta", "",NoBins_default, EtaLowBin, EtaHighBin);
 TH1F *phi = new TH1F("phi", "", NoBins_default, -PI, PI);

 TH3F *distrib = new TH3F("all", "",NoBins_3D, EtaLowBin, EtaHighBin, NoBins_3D, PtLowBin, PtHighBin, NoBins_3D, -PI, PI);

 TH1F *p_true = new TH1F("p_true", "",NoBins_default, PtLowBin, PtHighBin);
 TH1F *eta_true = new TH1F("eta_true", "", NoBins_default, EtaLowBin, EtaHighBin);
 TH1F *phi_true = new TH1F("phi_true", "", NoBins_default, -PI, PI);

 TH3F *total_efficiency = new TH3F("total_efficiency", "",NoBins_3D, EtaLowBin, EtaHighBin, NoBins_3D, PtLowBin, PtHighBin, NoBins_3D, -PI, PI);

 TH1F *chi_total = new TH1F("chi_total", "", 10*NoBins_default, 0, 150);
 TH1F *chi_prompt = new TH1F("chi_prompt", "", 10*NoBins_default, 0, 150);


/*** MC branches ***/
 int nMCP = 0;
 int MCPVs = 0;
 float MCPPX[MaxTracks];
 float MCPPY[MaxTracks];
 float MCPPZ[MaxTracks];
 float MCPIsPrompt[MaxTracks];
 float MCPIsReconstructed[MaxTracks];
 float MCPIsReconstructible[MaxTracks];
 float MCPPID[MaxTracks];

 /*** Data branches ***/
 int nTr = 0;
 int nPV = 0;
 float TRPX[MaxTracks];
 float TRPY[MaxTracks];
 float TRPZ[MaxTracks];
 float TRTYPE[MaxTracks];
 float TRGHOSTTYPE[MaxTracks];
 float TRGHOSTPROB[MaxTracks];
 float TRIsPrompt[MaxTracks];
 float TRIP[MaxTracks];
 float TRIPChi2[MaxTracks];

 float tmp_eta = 0;
 float tmp_pT = 0;
 float tmp_phi = 0;
 int entries = 0;
 TVector3 Momentum;


for (int k = 6; k < 8; k++){
     for (int f = 0; f < 30; f++){
        if (k==6) while (f==2||f==6 || f==11||f==24||f==45)f++;
        if (k==7) while (f==2||f==6||f==8||f==39||f==41 ||f==43||f==45)f++;
         string adr  = file_name(f,k);
         TFile *input = TFile::Open(adr.c_str());
         TTree *strom = (TTree*)input->Get("EvtTuple/EventTuple");
         strom->SetBranchStatus("*",0);

                     /*** MC branches ***/
         strom->SetBranchStatus("nMCP", 1);
         strom->SetBranchStatus("MCPPX", 1);
         strom->SetBranchStatus("MCPPY", 1);
         strom->SetBranchStatus("MCPPZ", 1);
         strom->SetBranchStatus("MCPPID", 1);
         strom->SetBranchStatus("MCPVs", 1);
         strom->SetBranchStatus("MCPIsPrompt", 1);
         strom->SetBranchStatus("MCPIsPrompt2", 1);
         //strom->SetBranchStatus("MCPInAcceptance", 1);
         strom->SetBranchStatus("MCPIsReconstructed", 1);
         strom->SetBranchStatus("MCPIsReconstructible", 1);

         strom->SetBranchAddress("nMCP", &nMCP);
         strom->SetBranchAddress("MCPPX", &MCPPX);
         strom->SetBranchAddress("MCPPY", &MCPPY);
         strom->SetBranchAddress("MCPPZ", &MCPPZ);
         strom->SetBranchAddress("MCPPID", &MCPPID);
         strom->SetBranchAddress("MCPIsPrompt", &MCPIsPrompt);
         strom->SetBranchAddress("MCPIsReconstructed", &MCPIsReconstructed);
         strom->SetBranchAddress("MCPIsReconstructible", &MCPIsReconstructible);
         strom->SetBranchAddress("MCPVs", &MCPVs);
          //strom->SetBranchAddress("MCPIsPrompt2", &MCPIsPrompt2);

                   /*** Data branches ***/
         strom->SetBranchStatus("nTr", 1);
         strom->SetBranchStatus("TRPX", 1);
         strom->SetBranchStatus("TRPY", 1);
         strom->SetBranchStatus("TRPZ", 1);
         strom->SetBranchStatus("TRTYPE", 1);
         strom->SetBranchStatus("TRGHOSTTYPE", 1);
         strom->SetBranchStatus("TRGHOSTPROB", 1);
         strom->SetBranchStatus("TRIsPrompt", 1);
         strom->SetBranchStatus("nPV", 1);
         strom->SetBranchStatus("TRIP", 1);
         strom->SetBranchStatus("TRIPChi2", 1);

         strom->SetBranchAddress("nTr", &nTr);
         strom->SetBranchAddress("TRPX", &TRPX);
         strom->SetBranchAddress("TRPY", &TRPY);
         strom->SetBranchAddress("TRPZ", &TRPZ);
         strom->SetBranchAddress("TRTYPE", &TRTYPE);
         strom->SetBranchAddress("TRGHOSTTYPE", &TRGHOSTTYPE);
         strom->SetBranchAddress("TRGHOSTPROB", &TRGHOSTPROB);
         strom->SetBranchAddress("TRIsPrompt", &TRIsPrompt);
         strom->SetBranchAddress("nPV", &nPV);
         strom->SetBranchAddress("TRIP", &TRIP);
         strom->SetBranchAddress("TRIPChi2", &TRIPChi2);

         Momentum;
         tmp_eta = 0;
         tmp_pT = 0;
         tmp_phi = 0;
         entries = strom->GetEntries();

         for (int i = 0; i<entries; i++){
                strom->GetEntry(i);
            //    if (cut_event(MCPVs)){
                    for (int j = 0; j < nMCP; j++){
                        Momentum.SetXYZ(MCPPX[j], MCPPY[j],MCPPZ[j]);
                        tmp_eta = Momentum.PseudoRapidity();
                        tmp_pT = Momentum.Perp();
                        tmp_phi = Momentum.Phi();
                        if (cut(Momentum,MCPIsPrompt[j], 0) && cut_neutral(MCPPID[j])){
                            eta_mc->Fill(tmp_eta);
                            p_mc->Fill(tmp_pT);
                            phi_mc->Fill(tmp_phi);
                            mc->Fill(tmp_eta,tmp_pT,tmp_phi);
                            if (MCPIsReconstructed[j]==1){ //charged long
                                eta_rec->Fill(tmp_eta);
                                p_rec->Fill(tmp_pT);
                                phi_rec->Fill(tmp_phi);
                                rec->Fill(tmp_eta,tmp_pT,tmp_phi);
                            }
                            if (MCPIsReconstructible[j] == 2){ //charged long
                                eta_reb->Fill(tmp_eta);
                                p_reb->Fill(tmp_pT);
                                phi_reb->Fill(tmp_phi);
                                reb->Fill(tmp_eta,tmp_pT,tmp_phi);
                            }
                        }
                    } //loop over tracks
           //     }
           //     if (cut_event(nPV)){
                    for (int j = 0; j < nTr; j++){
                        Momentum.SetXYZ(TRPX[j], TRPY[j],TRPZ[j]);
                        tmp_eta = Momentum.PseudoRapidity();
                        tmp_pT = Momentum.Perp();
                        tmp_phi = Momentum.Phi();
                        if (cut(Momentum,1,TRIPChi2[j]) && TRTYPE[j]==3 && TRGHOSTPROB[j] < cut_ghost){
                            p->Fill(tmp_pT);
                            phi->Fill(tmp_phi);
                            eta->Fill(tmp_eta);
                            distrib->Fill(tmp_eta,tmp_pT,tmp_phi);
                            if (TRGHOSTTYPE[j]==0){
                                p_ghost->Fill(tmp_pT);
                                phi_ghost->Fill(tmp_phi);
                                eta_ghost->Fill(tmp_eta);
                                ghost->Fill(tmp_eta,tmp_pT,tmp_phi);
                                if (TRIsPrompt[j]==1){
                                    p_prompt->Fill(tmp_pT);
                                    phi_prompt->Fill(tmp_phi);
                                    eta_prompt->Fill(tmp_eta);
                                    prompt->Fill(tmp_eta,tmp_pT,tmp_phi);
                                }
                            }
                        }
                    } //loop over tracks
           //    }
          }
/*
         for (int i = 0; i<entries; i++){
                strom->GetEntry(i);
                    for (int j = 0; j < nTr; j++){
                        Momentum.SetXYZ(TRPX[j], TRPY[j],TRPZ[j]);
                        if (cut(Momentum,1,1) && TRTYPE[j]==3 && TRGHOSTPROB[j] < cut_ghost){
                            chi_total->Fill(TRIPChi2[j]);
                            if (TRIsPrompt[j]==1){
                                chi_prompt->Fill(TRIPChi2[j]);
                            }
                        }
                    } //loop over tracks
          }
*/         strom->Delete();
          input->Close();
          cout << f << endl;

     }

}

  //gStyle->SetOptStat(0);

  //-------------------------------------------------------------//
  //-------------------------PROMPT------------------------------//
  //-------------------------------------------------------------//

  p_prompt_eff->Divide(p_prompt,p_ghost);
  phi_prompt_eff->Divide(phi_prompt,phi_ghost);
  eta_prompt_eff->Divide(eta_prompt,eta_ghost);

  p_prompt_eff->Scale(-1);
  phi_prompt_eff->Scale(-1);
  eta_prompt_eff->Scale(-1);

  for (int b = 0; b < NoBins_default; b++){
    p_prompt_eff->AddBinContent(b+1);
    phi_prompt_eff->AddBinContent(b+1);
    eta_prompt_eff->AddBinContent(b+1);
  }
/*
  TCanvas *promptp = new TCanvas("promptp", "", 10,10,600,600);
  promptp->cd();
  design_canvas(promptp);
  design_markers( p_prompt_eff, "p_{T} [MeV]","frac_{secondaries}",kRed+1);
  p_prompt_eff->Draw("P");
  p_prompt_eff->SaveAs("prompt_p.root");
  promptp->Print("promtpP.eps", "eps");

  TCanvas *promptphi = new TCanvas("promptphi", "", 10,10,600,600);
  promptphi->cd();
  design_canvas(promptphi);
  design_markers( phi_prompt_eff, "phi [rad]","frac_{secondaries}",kRed+1);
  phi_prompt_eff->Draw("P");
  phi_prompt_eff->SaveAs("prompt_phi.root");
  promptphi->Print("promtpPhi.eps", "eps");

  TCanvas *prompteta = new TCanvas("prompteta", "", 10,10,600,600);
  prompteta->cd();
  design_canvas(prompteta);
  design_markers( eta_prompt_eff, "#eta","frac_{secondaries}",kRed+1);
  eta_prompt_eff->SaveAs("prompt_eta.root");
  eta_prompt_eff->Draw("P");
  prompteta->Print("promtpEta.eps", "eps");
*/

  prompt->Divide(ghost);
  //prompt->Scale(-1);
  //for (int b = 0; b < 3*NoBins_default; b++){
    //prompt->AddBinContent(b+1);
  //}



 //-------------------------------------------------------------//
 //-------------------------GHOST-------------------------------//
 //-------------------------------------------------------------//
  p_ghost_eff->Divide(p_ghost,p);
  phi_ghost_eff->Divide(phi_ghost,phi);
  eta_ghost_eff->Divide(eta_ghost,eta);

  p_ghost_eff->Scale(-1);
  phi_ghost_eff->Scale(-1);
  eta_ghost_eff->Scale(-1);

  for (int b = 0; b < NoBins_default; b++){
    p_ghost_eff ->AddBinContent(b+1);
    phi_ghost_eff->AddBinContent(b+1);
    eta_ghost_eff->AddBinContent(b+1);
  }
/*
  TCanvas *ghostp = new TCanvas("ghostp", "", 10,10,600,600);
  ghostp->cd();
  design_canvas(ghostp);
  design_markers( p_ghost_eff, "p_{T} [MeV]","frac_{fake}",kAzure);
  p_ghost_eff->Draw("P");
  p_ghost_eff->SaveAs("ghost_p.root");
  ghostp->Print("ghostP.eps", "eps");

  TCanvas *ghostphi = new TCanvas("ghostphi", "", 10,10,600,600);
  ghostphi->cd();
  design_canvas(ghostphi);
  design_markers( phi_ghost_eff, "#phi [rad]","frac_{fake}",kAzure);
  phi_ghost_eff->Draw("P");
  phi_ghost_eff->SaveAs("ghost_phi.root");
  ghostphi->Print("ghostPhi.eps", "eps");

  TCanvas *ghosteta = new TCanvas("ghosteta", "", 10,10,600,600);
  ghosteta->cd();
  design_canvas(ghosteta);
  design_markers(eta_ghost_eff, "#eta","frac_{fake}",kAzure);
  eta_ghost_eff->Draw("P");
  eta_ghost_eff->SaveAs("ghost_eta.root");
  ghosteta->Print("ghostEta.eps", "eps");
*/
  ghost->Divide(distrib);
  //ghost->Scale(-1);
  //for (int b = 0; b < 3*NoBins_default; b++){
    //ghost ->AddBinContent(b+1);
  //}

  //-------------------------------------------------------------//
  //-------------------------Acceptance--------------------------//
  //-------------------------------------------------------------//

  acceptance_p->Divide(p_reb,p_mc);
  acceptance_eta->Divide(eta_reb,eta_mc);
  acceptance_phi->Divide(phi_reb,phi_mc);
  acceptance->Divide(reb,mc);
/*
  TCanvas *plot_pT_acc = new TCanvas("plot_pT_acc", "", 10,10,600,600);
  plot_pT_acc->cd();
  design_canvas(plot_pT_acc);
  design_markers(acceptance_p, "p_{T} [MeV]","Acceptance",kMagenta+2);
 // acceptance_p->GetYaxis()->SetRangeUser(0,1);
  acceptance_p->Draw();
  plot_pT_acc->Print("acceptance_p.eps", "eps");

  TCanvas *plot_eta_acc = new TCanvas("plot_eta_acc", "", 10,10,600,600);
  plot_eta_acc->cd();
  design_canvas(plot_eta_acc);
  design_markers(acceptance_eta, "#eta","Acceptance",kMagenta+2);
  acceptance_eta->Draw();
  plot_eta_acc->Print("acceptance_eta.eps", "eps");

  TCanvas *plot_phi_acc = new TCanvas("plot_phi_acc", "", 10,10,600,600);
  plot_phi_acc->cd();
  design_canvas(plot_phi_acc);
  design_markers(acceptance_phi, "#phi [rad]","Acceptance",kMagenta+2);
  acceptance_phi->Draw();
  plot_phi_acc->Print("acceptance_phi.eps", "eps");

  TCanvas *plot_acc = new TCanvas("plot_acc", "", 10,10,600,600);
  plot_acc->cd();
  plot_acc->SetRightMargin(0.02);
  plot_acc->SetTopMargin(0.02);
  acceptance->GetXaxis()->SetTitle("#eta");
  acceptance->GetYaxis()->SetTitle("p_{T} [MeV]");
  acceptance->GetZaxis()->SetTitle("#phi [rad]");
  acceptance->GetYaxis()->SetTitleOffset(1.5);
  acceptance->SetLineWidth(2);
  acceptance->Draw();
  plot_acc->Print("acceptance.eps", "eps");

  //-------------------------------------------------------------//
  //-------------------------Efficiency--------------------------//
  //-------------------------------------------------------------//
*/
  effective_p->Divide(p_rec,p_reb);
  effective_eta->Divide(eta_rec,eta_reb);
  effective_phi->Divide(phi_rec,phi_reb);
  effective->Divide(rec,reb);
/*
  TCanvas *plot_pT_eff = new TCanvas("plot_pT_eff", "", 10,10,600,600);
  plot_pT_eff->cd();
  design_canvas(plot_pT_eff);
  design_markers(effective_p, "p_{T} [MeV]","Efficiency",kOrange-3);
 // effective_p->GetYaxis()->SetRangeUser(0.0,1);
  //effective_p->SetLineWidth(2);
  effective_p->Draw();
  plot_pT_eff->Print("effective_p.eps", "eps");

  TCanvas *plot_eta_eff = new TCanvas("plot_eta_eff", "", 10,10,600,600);
  plot_eta_eff->cd();
  design_canvas(plot_eta_eff);
  design_markers(effective_eta, "#eta","Efficiency",kOrange-3);
  effective_eta->Draw();
  plot_eta_eff->Print("effective_eta.eps", "eps");

  TCanvas *plot_phi_eff = new TCanvas("plot_phi_eff", "", 10,10,600,600);
  plot_phi_eff->cd();
  design_canvas(plot_phi_eff);
  design_markers(effective_phi, "#phi [rad]","Efficiency",kOrange-3);
  effective_phi->Draw();
  plot_phi_eff->Print("effective_phi.eps", "eps");

  TCanvas *plot_eff = new TCanvas("plot_eff", "", 10,10,600,600);
  plot_eff->cd();
  plot_eff->SetRightMargin(0.02);
  plot_eff->SetTopMargin(0.02);
  effective->GetXaxis()->SetTitle("#eta");
  effective->GetYaxis()->SetTitle("p_{T} [MeV]");
  effective->GetZaxis()->SetTitle("#phi [rad]");
  effective->GetYaxis()->SetTitleOffset(1.5);
  effective->SetLineWidth(2);
  effective->Draw();
  plot_eff->Print("effective.eps", "eps");

  //-------------------------------------------------------------//
  //-------------------Acceptance+Efficiency---------------------//
  //-------------------------------------------------------------//
*/
   acc_eff_p->Multiply(acceptance_p,effective_p);
   acc_eff_eta->Multiply(acceptance_eta,effective_eta);
   acc_eff_phi->Multiply(acceptance_phi,effective_phi);
   acc_eff->Multiply(acceptance,effective);
/*
   TCanvas *plot_acc_eff = new TCanvas("plot_acc_eff", "", 10,10,600,600);
   plot_acc_eff->cd();
   plot_acc_eff->SetRightMargin(0.02);
   plot_acc_eff->SetTopMargin(0.02);
   acc_eff->GetXaxis()->SetTitle("#eta");
   acc_eff->GetYaxis()->SetTitle("p_{T} [MeV]");
   acc_eff->GetZaxis()->SetTitle("#phi [rad]");
   acc_eff->GetYaxis()->SetTitleOffset(1.5);
  // acc_eff->GetZaxis()->SetRangeUser(0,1);
   acc_eff->SetLineWidth(2);
   acc_eff->Draw();
   plot_acc_eff->Print("acc_eff.eps", "eps");

   TCanvas *plot_pT_acc_eff = new TCanvas("plot_pT_acc_eff", "", 10,10,600,600);
   plot_pT_acc_eff->cd();
   design_canvas(plot_pT_acc_eff);
   design_markers(acc_eff_p, "#p_{T} [MeV]","Total Efficiency",kGreen+2);
   acc_eff_p->Draw();
   plot_pT_acc_eff->Print("acc_eff_p.eps", "eps");


   TCanvas *plot_eta_acc_eff = new TCanvas("plot_eta_acc_eff", "", 10,10,600,600);
   plot_eta_acc_eff->cd();
   design_canvas(plot_eta_acc_eff);
   design_markers(acc_eff_eta, "#eta","Total Efficiency",kGreen+2);
   acc_eff_eta->Draw();
   plot_eta_acc_eff->Print("acc_eff_eta.eps", "eps");


   TCanvas *plot_phi_acc_eff = new TCanvas("plot_phi_acc_eff", "", 10,10,600,600);
   plot_phi_acc_eff->cd();
   design_canvas(plot_phi_acc_eff);
   design_markers(acc_eff_phi, "#phi [rad]","Total Efficiency",kGreen+2);
   acc_eff_phi->Draw();
   plot_phi_acc_eff->Print("acc_eff_phi.eps", "eps");


   //-------------------------------------------------------------//
   //------------------------Overall plots------------------------//
   //-------------------------------------------------------------//

 /* //overal mc-rec-reb plots
        // #p_T
   TLegend *leg1 = new TLegend(0.69,0.8,0.9,0.9);
       leg1->AddEntry(p_mc,"Generated","l");
       leg1->AddEntry(p_rec,"Reconstructed","l");
   //   leg1->AddEntry(p_reb,"Reconstrutible","l");
       leg1->SetTextFont(42);
       leg1->SetTextSize(0.025);
   TCanvas *c1 = new TCanvas("c1", "#p_T", 10,10,600,600);
   c1->cd();
        c1->SetRightMargin(0.02);
        c1->SetTopMargin(0.02);
       p_mc->GetXaxis()->SetTitle("p_{T}");
       p_mc->SetLineWidth(2);
       p_mc->SetLineColor(kBlue);
       p_mc->Draw();
       p_rec->SetLineColor(kRed);
       p_rec->GetXaxis()->SetTitle("p_{T}");
       p_rec->SetLineWidth(2);
       p_rec->Draw("same");
    /*  p_reb->SetLineColor(kGreen);
       p_reb->GetXaxis()->SetTitle("p_{T}");
       p_reb->SetLineWidth(2);
       p_reb->Draw("same");*/
/*       leg1->Draw();
   c1->SaveAs("p_rec.root");

        //#eta
   TLegend *leg2 = new TLegend(0.69,0.8,0.9,0.9);
       leg2->AddEntry(eta_mc,"Generated","l");
       leg2->AddEntry(eta_rec,"Reconstructed","l");
    //   leg2->AddEntry(eta_reb,"Reconstructible","l");
       leg2->SetTextFont(42);
       leg2->SetTextSize(0.025);
   TCanvas *c2 = new TCanvas("c2", "#eta", 10,10,600,600);
   c2->cd();
   c2->SetRightMargin(0.02);
   c2->SetTopMargin(0.02);
       eta_mc->GetXaxis()->SetTitle("#eta");
       eta_mc->SetLineColor(kBlue);
       eta_mc->SetLineWidth(2);
       eta_mc->Draw();
       eta_rec->GetXaxis()->SetTitle("#eta");
       eta_rec->SetLineColor(kRed);
       eta_rec->SetLineWidth(2);
       eta_rec->Draw("same");
     /*  eta_reb->GetXaxis()->SetTitle("#eta");
       eta_reb->SetLineColor(kGreen);
       eta_reb->SetLineWidth(2);
       eta_reb->Draw("same");*/
 /*      leg2->Draw();
   c2->SaveAs("eta_rec.root");

        //#phi
   TLegend *leg3 = new TLegend(0.69,0.8,0.9,0.9);
       leg3->AddEntry(phi_mc,"Generated","l");
       leg3->AddEntry(phi_rec,"Reconstructed","l");
  //     leg3->AddEntry(phi_reb,"Reconstructible","l");
       leg3->SetTextFont(42);
       leg3->SetTextSize(0.025);
   TCanvas *c02 = new TCanvas("c02", "#phi", 10,10,600,600);
   c02->cd();
   c02->SetRightMargin(0.02);
   c02->SetTopMargin(0.02);
        phi_mc->GetXaxis()->SetTitle("#phi [rad]");
        phi_mc->SetLineWidth(2);
        phi_mc->SetLineColor(kBlue);
        phi_mc->Draw();
        phi_rec->GetXaxis()->SetTitle("#phi [rad]");
        phi_rec->SetLineColor(kRed);
        phi_rec->SetLineWidth(2);
        phi_rec->Draw("same");
   /*     phi_reb->GetXaxis()->SetTitle("#phi [rad]");
        phi_reb->SetLineColor(kGreen);
        phi_reb->SetLineWidth(2);
        phi_reb->Draw("same");*/
 /*       leg3->Draw();
   c02->SaveAs("phi_rec.root");

   p_true->Divide(p,acc_eff_p);
   p_true->Scale(1.0/p_true->Integral(1,NoBins_default));
   p->Scale(1.0/p->Integral(1,NoBins_default));
   p_mc->Scale(1.0/p_mc->Integral(1,NoBins_default));
   TLegend *leg_p = new TLegend(0.69,0.8,0.9,0.9);
       leg_p->AddEntry(p,"Detected","l");
       leg_p->AddEntry(p_true,"Corrected","l");
       leg_p->AddEntry(p_mc,"Generated","l");
       leg_p->SetTextFont(42);
       leg_p->SetTextSize(0.025);
   TCanvas *c_p = new TCanvas("c_p", "#real_p", 10,10,600,600);
   c_p->cd();
   c_p->SetRightMargin(0.02);
   c_p->SetTopMargin(0.02);
       p_true->GetXaxis()->SetTitle("p_{T} [MeV]");
       p_true->GetYaxis()->SetTitle("#frac{dN}{dp_{T}}");
       p_true->SetTitle("");
       p_true->SetLineColor(kBlue);
       p_true->SetLineWidth(2);
       p_true->Draw();
       p->SetLineColor(kGreen);
       p->GetXaxis()->SetTitle("p_{T} [MeV]");
       p->GetYaxis()->SetTitle("#frac{dN}{dp_{T}}");
       p->SetLineWidth(2);
       p->Draw("same");
       p_mc->SetLineColor(kRed);
       p_mc->SetLineWidth(2);
       p_mc->Draw("same");
       leg_p->Draw();
   c_p->SaveAs("p_eff.root");

   phi_true->Divide(phi,acc_eff_phi);

   phi_true->Scale(1.0/phi_true->Integral(1,NoBins_default));
   phi->Scale(1.0/phi->Integral(1,NoBins_default));
   phi_mc->Scale(1.0/phi_mc->Integral(1,NoBins_default));

   TLegend *leg_phi = new TLegend(0.69,0.8,0.9,0.9);
       leg_phi->AddEntry(phi,"Detected","l");
       leg_phi->AddEntry(phi_true,"Corrected","l");
       leg_phi->AddEntry(phi_mc,"Generated","l");
       leg_phi->SetTextFont(42);
       leg_phi->SetTextSize(0.025);
   TCanvas *c_phi = new TCanvas("c_phi", "#real_phi", 10,10,600,600);
   c_phi->cd();
   c_phi->SetRightMargin(0.02);
   c_phi->SetTopMargin(0.02);
        phi_true->GetXaxis()->SetRangeUser(-PI,PI);
        phi_true->GetXaxis()->SetTitle("#phi [rad]");
        phi_true->GetYaxis()->SetTitle("#frac{dN}{d#phi}");
        phi_true->SetLineColor(kBlue);
        phi_true->SetTitle("");
        phi_true->SetLineWidth(2);
        phi_true->Draw();
        phi->SetLineColor(kGreen);
        phi->GetXaxis()->SetTitle("#phi [rad]");
        phi->GetYaxis()->SetTitle("#frac{dN}{d#phi}");
        phi->SetLineWidth(2);
        phi->Draw("same");
        phi_mc->SetLineColor(kRed);
        phi_mc->SetLineWidth(2);
        phi_mc->Draw("same");
        leg_phi->Draw();
   c_phi->SaveAs("phi_eff.root");

   eta_true->Divide(eta,acc_eff_eta);
   eta->Scale(1.0/float(eta->Integral(1,NoBins_default)));
   eta_true->Scale(1.0/float(eta_true->Integral(1,NoBins_default)));
   eta_mc->Scale(1.0/float(eta_mc->Integral(1,NoBins_default)));

   TLegend *leg_eta = new TLegend(0.69,0.8,0.9,0.9);
       leg_eta->AddEntry(eta,"Detected","l");
       leg_eta->AddEntry(eta_true,"Corrected","l");
       leg_eta->AddEntry(eta_mc,"Generated","l");
       leg_eta->SetTextFont(42);
       leg_eta->SetTextSize(0.025);
   TCanvas *c_eta = new TCanvas("c_eta", "#real_eta", 10,10,600,600);
   c_eta->cd();
   c_eta->SetRightMargin(0.02);
   c_eta->SetTopMargin(0.02);
       eta_true->GetXaxis()->SetTitle("#eta");
       eta_true->GetYaxis()->SetTitle("#frac{dN}{d#eta}");
       eta_true->SetTitle("");
       eta_true->SetLineColor(kBlue);
       eta_true->SetLineWidth(2);
       eta_true->Draw("E1");
       eta->SetLineColor(kGreen);
       eta->GetXaxis()->SetTitle("#eta");
       eta->GetYaxis()->SetTitle("#frac{dN}{d#eta}");
       eta->SetLineWidth(2);
       eta->Draw("same");
       eta_mc->SetLineColor(kRed);
       eta_mc->SetLineWidth(2);
       eta_mc->Draw("same");
       leg_eta->Draw();
   c_eta->SaveAs("eta_eff.root");
*/
   //-------------------------------------------------------------//
   //---------------------------Saving----------------------------//
   //-------------------------------------------------------------//

  // for (int b = 0; b < 3*NoBins_default; b++){
    //   total_efficiency->AddBinContent(b+1);
   //}

   total_efficiency->Multiply(prompt,ghost);
   total_efficiency->Multiply(prompt);
   total_efficiency->Divide(acc_eff);

   string adr = "./efficiency2_all.root";
   TFile *output = new TFile(adr.c_str(),"RECREATE");
   output->cd();
   acceptance_p->Write();
   acceptance_eta->Write();
   acceptance_phi->Write();
   acceptance->Write();
   effective_p->Write();
   effective_eta->Write();
   effective_phi->Write();
   effective->Write();
   acc_eff_eta->Write();
   acc_eff_phi->Write();
   acc_eff_p->Write();
   acc_eff->Write();
   ghost->Write();
   prompt->Write();
   total_efficiency->Write();
   output->Close();
/*

  TLegend *leg_chi = new TLegend(0.69,0.8,0.9,0.9);
      leg_chi->AddEntry(chi_total,"All","l");
      leg_chi->AddEntry(chi_prompt,"prompt","l");
      leg_chi->SetTextFont(42);
      leg_chi->SetTextSize(0.025);
  TCanvas *plot_prompt = new TCanvas("plot_prompt", "", 10,10,600,600);
  plot_prompt->cd();
      chi_total->SetLineColor(kBlue);
      chi_total->SetLineWidth(2);
      chi_total->Draw();
      chi_prompt->SetLineColor(kRed);
      chi_prompt->SetLineWidth(2);
      chi_prompt->Draw("same");
      leg_chi->Draw();

   TCanvas *plot_mom = new TCanvas("mom", "mom", 10,10,600,600);
      plot_mom->cd();
          p->SetLineColor(kBlue);
          p->SetLineWidth(2);
          p->Draw();
          p_ghost->SetLineColor(kRed);
          p_ghost->SetLineWidth(2);
          p_ghost->Draw("same");
          p_prompt->SetLineColor(kGreen);
          p_prompt->SetLineWidth(2);
          p_prompt->Draw("same");
*/
//  gDirectory->GetList()->Delete();

}
