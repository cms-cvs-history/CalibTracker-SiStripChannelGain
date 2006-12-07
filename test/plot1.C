// loop over all (1d?) histograms in  a file and print them in gifs
void plot1(TFile f="output_histograms.root") {

// automatically read in all histograms (flat structure, all in 1 directory)
  TH1 *h; TKey *key;
  TIter nextkey(gDirectory->GetListOfKeys());
  while (key = (TKey*)nextkey()) {
    obj = key->ReadObj();
    if (obj->IsA()->InheritsFrom("TH1")) {
      h = (TH1*)obj;
      TString the_title = h->GetName();
//      cout << h->GetName()<< "            entries="<<h->GetEntries() << endl;
    }
  }

// plot some histograms
z1(LocalAngle,  "Local Angle", false, false);
z1(LocalAngleAbsoluteCosine,  "abs(cos(Local Angle))", false, false);
z1(SiStripRecHitType,  "SiStripRecHit Type", false, false);
z1(LocalPosition_cm,  "Local Position (cm)", false, false);
z1(LocalPosition_normalized,  "Local Position divided with total width", false, false);
z1(ChargeOfEachAPVPair,  "Charge Of Each APV Pair", false, false);
z1(EntriesApvPairs,  "Entries Apv Pairs", false, false);
z1(CorrectionOfEachAPVPair,  "Correction Of Each APV Pair", false, false);

z1(ModuleThickness,  "Module Thickness", false, false);
z1(ModuleWidth,  "Module Width", false, false);

NrOfEntries->SetYTitle("Nr APVPairs"); NrOfEntries->SetXTitle("Nr of Entries");
NrOfEntries->GetXaxis()->SetRange(1,5); z1(NrOfEntries,  "Nr Of Entries small", false, false);
NrOfEntries->GetXaxis()->SetRange(1,351);
NrOfEntries->Rebin(70);
z1(NrOfEntries,  "Nr Of Entries all", false, false);

APVPairCorrections->SetYTitle("Nr APVPairs");
APVPairCorrections->Fit("gaus","QR",0,0.,1.);
gStyle->SetOptFit(000011);  APVPairCorrections->GetFunction("gaus")->SetLineColor(3);
z1(APVPairCorrections,  "APV Pair Corrections", false, false);

}



void z1(TH1 *ah1, TString mytitle, bool logx, bool logy){
  gStyle->SetOptStat(1110); gStyle->SetCanvasColor(0); gStyle->SetStatBorderSize(1);
  gStyle->SetHistFillColor(0); gStyle->SetHistLineColor(1);
  gStyle->SetMarkerStyle(23); gStyle->SetMarkerSize(2.5); gStyle->SetMarkerColor(1);
  gStyle->SetTitleSize(1.0);
//
 TCanvas *myCanvas = new TCanvas("zc","zC",500,0,700,700);
 if(logx) myCanvas->SetLogx(1); if(logy) myCanvas->SetLogy(1);
// ah1->SetXTitle(ah1->GetTitle());
 ah1->SetTitle(mytitle); TString filenamewheretoprint = mytitle+".gif";
 ah1->Draw();
// char *s = new char[1]; cout<<"hit return to continue"<<endl; gets(s);  // "return" is enough, root waits until you hit a key
 zc->Print(filenamewheretoprint);
}
