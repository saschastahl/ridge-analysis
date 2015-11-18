from ROOT import *
gROOT.ProcessLine(".x lhcbStyle.C")


pvCut = "nPVs>0"
file50 = TFile("minbias_50ns.root")
file25 = TFile("minbias_25ns.root")

tree50 = file50.Get("EvtTuple/EventTuple")
tree25 = file25.Get("EvtTuple/EventTuple")

c1 = TCanvas("c1","c1",800,800)

nPVs25 = TH1D("nPVs25","nPVs25;#PV",7,0,7)
nPVs50 = TH1D("nPVs50","nPVs50;#PV",7,0,7)

nVeloClusters25 = TH1D("nVeloClusters25","25 ns;#Velo Clusters",100,0,7000)
nVeloClusters50 = TH1D("nVeloClusters50","50 ns;#Velo Clusters",100,0,7000)

nOTClusters25 = TH1D("nOTClusters25","25 ns;#OT Clusters",100,0,15000)
nOTClusters25_0PV = TH1D("nOTClusters25_0PV","25 ns 0 PV;#OT Clusters",100,0,15000)
nOTClusters50 = TH1D("nOTClusters50","50 ns;#OT Clusters",100,0,15000)

nITClusters25 = TH1D("nITClusters25","25 ns;#IT Clusters",100,0,4000)
nITClusters50 = TH1D("nITClusters50","50 ns;#IT Clusters",100,0,4000)

nTTClusters25 = TH1D("nTTClusters25","25 ns;#TT Clusters",100,0,2000)
nTTClusters50 = TH1D("nTTClusters50","50 ns;#TT Clusters",100,0,2000)

nSPDClusters25 = TH1D("nSPDClusters25","25 ns;#SPD hits",100,0,1000)
nSPDClusters50 = TH1D("nSPDClusters50","50 ns;#SPD hits",100,0,1000)

for h in [nVeloClusters25, nOTClusters25,nITClusters25,nTTClusters25,nSPDClusters25, nPVs25]:
    h.SetLineColor(2)
    h.SetLineWidth(2)
    h.Sumw2()
    h.SetMarkerColor(2)
tree25.Draw("nPVs>>nPVs25")
tree50.Draw("nPVs>>nPVs50")

tree25.Draw("nVeloClusters>>nVeloClusters25",pvCut)
tree50.Draw("nVeloClusters>>nVeloClusters50",pvCut)

tree25.Draw("nOTClusters>>nOTClusters25",pvCut)
tree25.Draw("nOTClusters>>nOTClusters25_0PV","nPVs==0")
tree50.Draw("nOTClusters>>nOTClusters50",pvCut)

tree25.Draw("nITClusters>>nITClusters25",pvCut)
tree50.Draw("nITClusters>>nITClusters50",pvCut)

tree25.Draw("nTTClusters>>nTTClusters25",pvCut)
tree50.Draw("nTTClusters>>nTTClusters50",pvCut)

tree25.Draw("nSPDHits>>nSPDClusters25",pvCut)
tree50.Draw("nSPDHits>>nSPDClusters50",pvCut)


c1.cd()
nPVs25.DrawNormalized()
nPVs25.GetYaxis().SetRangeUser(0,nPVs25.GetMaximum()*1.5)
nPVs50.DrawNormalized("same")
c1.BuildLegend()
c1.Print("nPVs.eps")
pvCut = pvCut.replace(">0","1")
pvCut = pvCut.replace("==","0")
nVeloClusters25.DrawNormalized()
nVeloClusters50.DrawNormalized("same")
c1.BuildLegend()
c1.Print("nVeloClusters"+pvCut+".eps")

nOTClusters50.DrawNormalized()
nOTClusters25.DrawNormalized("same")
c1.BuildLegend()
c1.Print("nOTClusters"+pvCut+".eps")

nOTClusters25.SetTitle("25 ns >0 PV")
nOTClusters25_0PV.DrawNormalized()
nOTClusters25.DrawNormalized("same")
c1.BuildLegend()
c1.Print("nOTClusters0PV.eps")


nITClusters50.DrawNormalized()
nITClusters25.DrawNormalized("same")
c1.BuildLegend()
c1.Print("nITClusters"+pvCut+".eps")

nTTClusters50.DrawNormalized()
nTTClusters25.DrawNormalized("same")
c1.BuildLegend()
c1.Print("nTTClusters"+pvCut+".eps")

nSPDClusters25.DrawNormalized()
nSPDClusters50.DrawNormalized("same")
c1.BuildLegend()
c1.Print("nSPDClusters"+pvCut+".eps")
