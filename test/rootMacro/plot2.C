// loop over all (1d?) histograms in  a file and print them in gifs
//void plot2(TString filename="histos_tickheight20303.root") {
void plot2(TString filename="histos_tickheight20268.root") {

define_style();
gROOT->SetStyle("BABAR");


TFile f=filename.Data();

// automatically read in all histograms (flat structure, all in 1 directory)
  TH1 *h; TKey *key; TGraphErrors * g;
  TIter nextkey(gDirectory->GetListOfKeys());
  while (key = (TKey*)nextkey()) {
    obj = key->ReadObj();
    if (obj->IsA()->InheritsFrom("TH1")) {
      h = (TH1*)obj;
      TString the_title = h->GetName();
    }
    if (obj->IsA()->InheritsFrom("TGraphErrors")) {
      g = (TGraphErrors*)obj;
    }
  }

// plot some histograms

/*
MuonChargeOverTickHeightEachAPVPair->GetXaxis()->SetLabelOffset(10000000);
MuonChargeOverTickHeightEachAPVPair->SetXTitle("APVPair Id Control View");
z1(MuonChargeOverTickHeightEachAPVPair,  "MuonChargeOverTickHeightEachAPVPair", false, false);
*/


TF1 *gaussfit = new TF1("gaussfit","gaus",400.,700.);
gaussfit->SetLineColor(3);
gaussfit->SetParName(0,"Const");
tickHeightDistribution->SetAxisRange(300.,900.);
tickHeightDistribution->Fit("gaussfit","QR",0,300.,900.);
gStyle->SetOptFit(000011);
z1(tickHeightDistribution,  "tickHeightDistribution", false, false);

TF1 *gaussfit2 = new TF1("gaussfit2","gaus",0.,5.);
gaussfit2->SetLineColor(3);
gaussfit2->SetParName(0,"Const");
tickGainDistribution->Fit("gaussfit","QR",0,0.5,1.5);
gStyle->SetOptFit(000011);
tickGainDistribution->SetXTitle("Tick Gain");
tickGainDistribution->SetYTitle("APV Pairs");
z1(tickGainDistribution,  "tickGainDistribution", false, false);

/*
gStyle->SetOptStat(100000);
MuonChargeOverTickHeight->SetYTitle("Nr APVPairs");
MuonChargeOverTickHeight->Fit("gaus","QR",0,0.,1.);
gStyle->SetOptFit(000011);  MuonChargeOverTickHeight->GetFunction("gaus")->SetLineColor(3);
z1(MuonChargeOverTickHeight,  "MuonChargeOverTickHeight", false, false);
*/

gStyle->SetOptStat(000000);
ParticleGainVsTickGain->SetMarkerSize(1);
ParticleGainVsTickGain->SetMarkerStyle(21);
ParticleGainVsTickGain->SetMarkerColor(2);
ParticleGainVsTickGain->SetXTitle("Tickmark Gain");
ParticleGainVsTickGain->SetYTitle("Particle Gain");
w1(ParticleGainVsTickGain,"ParticleGainVsTickGain",false, false);

// draw TGraphErrors
TH2F *frame = new TH2F("frame","frame",10,0.7,1.4,10,0.7,1.4);
frame->SetXTitle("Tickmark Gain");
frame->SetYTitle("Particle Gain");
g_ParticleGainVsTickGain->SetMarkerSize(1);
g_ParticleGainVsTickGain->SetMarkerStyle(21);
g_ParticleGainVsTickGain->SetMarkerColor(2);
zgraph(frame, g_ParticleGainVsTickGain,"g_ParticleGainVsTickGain",false, false);
}

void zgraph(TH2F *frame, TGraphErrors * graph, TString mytitle, bool logx, bool logy){
TCanvas *myCanvas = new TCanvas("zc","zC",500,0,700,700);
 if(logx) myCanvas->SetLogx(1); if(logy) myCanvas->SetLogy(1);
 frame->SetTitle(mytitle); TString filenamewheretoprint = mytitle+".gif";
 frame->Draw();
 graph->Draw("SP");
 zc->Print(filenamewheretoprint);
}


void z1(TH1 *ah1, TString mytitle, bool logx, bool logy){
 TCanvas *myCanvas = new TCanvas("zc","zC",500,0,700,700);
 if(logx) myCanvas->SetLogx(1); if(logy) myCanvas->SetLogy(1);
 ah1->SetTitle(mytitle); TString filenamewheretoprint = mytitle+".gif";
 ah1->Draw();
 zc->Print(filenamewheretoprint);
}


void w1(TH2 *ah2, TString mytitle, bool logx, bool logy){
 TCanvas *myCanvas = new TCanvas("zc","zC",500,0,700,700);
 if(logx) myCanvas->SetLogx(1); if(logy) myCanvas->SetLogy(1);
 ah2->SetTitle(mytitle); TString filenamewheretoprint = mytitle+".gif";
 ah2->Draw();
 zc->Print(filenamewheretoprint);
}



void define_style(){
 /*
if (getenv("CMSSW_RELEASE_BASE")!='\0'){
 printf("CMSSW FWLite loaded\n\n");
 gSystem->Load("libFWCoreFWLite");
 AutoLibraryLoader::enable();
}
    
cout << "\nFor approved plots use: gROOT->SetStyle(\"BABAR\");" << endl << endl;
  */                                                                                
                                                                                    
//..BABAR style from RooLogon.C in workdir
TStyle *babarStyle= new TStyle("BABAR","BaBar approved plots style");
                                                                                    
// use plain black on white colors
babarStyle->SetFrameBorderMode(0);
babarStyle->SetCanvasBorderMode(0);
babarStyle->SetPadBorderMode(0);
babarStyle->SetPadColor(0);
babarStyle->SetCanvasColor(0);
babarStyle->SetStatColor(0);
babarStyle->SetFillColor(0);
                                                                                    
// set the paper & margin sizes
babarStyle->SetPaperSize(20,26);
babarStyle->SetPadTopMargin(0.05);
babarStyle->SetPadRightMargin(0.05);
babarStyle->SetPadBottomMargin(0.16);
babarStyle->SetPadLeftMargin(0.12);
                                                                                    
// use large Times-Roman fonts
babarStyle->SetTextFont(132);
babarStyle->SetTextSize(0.08);
babarStyle->SetLabelFont(132,"x");
babarStyle->SetLabelFont(132,"y");
babarStyle->SetLabelFont(132,"z");
babarStyle->SetLabelSize(0.05,"x");
babarStyle->SetTitleSize(0.06,"x");
babarStyle->SetLabelSize(0.05,"y");
babarStyle->SetTitleSize(0.06,"y");
babarStyle->SetLabelSize(0.05,"z");
babarStyle->SetTitleSize(0.06,"z");
                                                                                    
// use bold lines and markers
babarStyle->SetMarkerStyle(20);
babarStyle->SetHistLineWidth(1.85);
babarStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes
                                                                                    
// get rid of X error bars and y error bar caps
babarStyle->SetErrorX(0.001);
                                                                                    
// do not display any of the standard histogram decorations
babarStyle->SetOptTitle(0);
babarStyle->SetOptStat(0);
babarStyle->SetOptFit(0);
                                                                                    
// put tick marks on top and RHS of plots
babarStyle->SetPadTickX(1);
babarStyle->SetPadTickY(1);
                                                                                    
gROOT->SetStyle("Plain");
gStyle->SetOptStat(1111111);
gStyle->SetPadTickX(1);
gStyle->SetPadTickY(1);

}
