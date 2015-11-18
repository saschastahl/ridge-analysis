#ifndef GENERAL_H
#define GENERAL_H

#include <TH3F.h>


using namespace std;

string preprocessed_name(int NFile, int PVZ, int nVELO);
		//returns the name of the output of rewrite_data
string efficiency_name();
		//name of the file with efficiencies	
void nVELO_up_low(int nVELO_bin, int dataset);
		//assignes edge values for different nVELO bins
string nVELO_name(int dataset,int nVELO_bin);
		//folders for different nVELO bins
void pT_up_low(int PT_bin);
		//assignes edge values for different pT bins
bool select_track(int TRTYPE, float TRGHOSTPROB, float TRIPChi2);
		//track cut	 returns 1 if track is within cut, else returns 0
float weighting(TVector3 Momentum, TH3F efficiency);
		//returns weight for selected track
bool cut(TVector3 Momentum, float pT_low, float pT_high);   
		//returns 1 if track is within cut, else returns 0

float delta_eta(TVector3 Momentum, TVector3 Momentum2);
		//delta eta calculation
float delta_phi(TVector3 Momentum, TVector3 Momentum2);
		//delta phi calculation

string hist_name(int NFile, int PVZ, int nVELO, int PT, bool sig);
		//name of the subresult files

bool file_exists (string name);
		//if file exists, return 1, else 0

#endif // GENERAL_H
