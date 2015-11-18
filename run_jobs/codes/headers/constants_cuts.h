#ifndef CONSTANTS_CUTS_H
#define CONSTANTS_CUTS_H

/*** General constants ***/
const double PI =3.141592653589793;

/*** Other constants ***/
const bool TestRun = 0;   //analyzes just first NoTest events
const int NoTest = 50000; //number of events analyzed in the test mode

const int NoBins = 20;
const int mixing = 5;   //number of mixed events, 

/*** Cut constants ***/
const float cut_p_low = 2000;    //MeV
//const float cut_p_high = 120000;    //MeV

const float cut_eta_low = 1.5;
const float cut_eta_high = 5.2;

const float cut_ghost = 0.25;

const float cut_IP = 2.4;
const float cut_IPCHI = 9;

const float cut_PVZ_low = -80; //mm
const float cut_PVZ_high = 120; //mm
const int PVZ_distance = 20; //mm

const int MaxTracks = 5000;   //CAREFULL!!
const int MaxVertices = 50;

extern int VELO_distance;

const int tracktype = 3;  //desired track type
            /* type of tracks;
                   0 	TypeUnknown
                   1 	Velo
                   2 	VeloR
                   3 	Long
                   4 	Upstream
                   5 	Downstream
                   6 	Ttrack
                   7 	Muon
                   8 	Calo
                   9 	TT
             */

extern int pT_up;
extern int pT_low;

extern float cut_VELO_low;
extern float cut_VELO_up;
#endif // CONSTANTS_CUTS_H

