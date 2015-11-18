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
#include <sys/stat.h> //mkdir
#include <errno.h>
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

#include "./headers/constants_cuts.h"
#include "./headers/background.h"
#include "./headers/rewrite_data.h"
#include "./headers/signal.h"
#include "./headers/general.h"

using namespace std;

float cut_VELO_up = 0;
float cut_VELO_low = 0;
int VELO_distance = 0;

int pT_up = 0;
int pT_low = 0;


// COMPILATION:  g++ main.cpp  ./source/rewrite_data.cpp ./source/signal.cpp ./source/background.cpp ./source/general.cpp  `root-config --libs --cflags` -o execute
int main ( int argc, char* argv[]){
    int file = 0;
    int dataset = 100;
    int pT = 0;
    int nVELO = 0;
    bool sig = 0; //if sig = 1; if back = 0
    if ( argc == 6 ) {
      file = atoi( argv[1] );
      dataset = atoi( argv[2] );
      pT = atoi( argv[3] );
      nVELO = atoi( argv[4] );
      sig = atoi( argv[5] );        //if sig == 1 => signal
                                    //if sig == 0 => background
    }
    else {
      cout << "Wrong number of parameters! 5 parameters required: file dataset pT_bin, nVelo_bin, sig/back" << endl;
      return 0;
    }
    nVELO_up_low(nVELO,dataset); //setting nVELO boundaries
    pT_up_low(pT);  //setting pT boundaries

    rewrite_data(file,dataset, nVELO);

    stringstream ifile;
    ifile << file;
    stringstream ibin;
    ibin << pT;

    string path =  "./histograms"+ ibin.str() + "/hist_" + ifile.str() +"/";
    mkdir(path.c_str(), 0777);
    cout << "### Creation of " << path << ": " << strerror(errno) << endl;

    if (sig == 1){
        cout << "Signal.. . .  .  ." << endl;
        signal(file,pT,dataset,nVELO);
        return 0;
    }

    if (sig == 0){
        cout << "Background.. . .  .  ." << endl;
        background(file,pT);
        return 0;
    }

    return 0;
}
