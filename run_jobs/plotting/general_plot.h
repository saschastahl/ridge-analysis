#ifndef GENERAL_PLOT_H
#define GENERAL_PLOT_H


using namespace std;

string input_subhist_name(int NFile, int PVZ, int nM, int dataset, int PT_bin, int nVELO_bin, bool sig);
string output_ridge_name(int dataset, int PT_bin, int nVELO_bin);
string output_name(int dataset, int PT_bin, int nVELO_bin);
string final_name(int dataset, int PT_bin, int nVELO_bin);
string eps_ridge_name(int dataset, int PT_bin, int nVELO_bin);

void design_signal(TH2F *signal);
void design_background(TH2F *background);
void design_final(TH2F *final, int pt, int m);
void design_canvas(TCanvas *canvas);
void design_ridge(TH1F *histogram, Color_t color);
string plot_title(int pT, int m);

void set_plot_style();

#endif // GENERAL_PLOT_H
