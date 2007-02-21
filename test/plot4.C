// loop over all (1d?) histograms in  a file and print them in gifs
void plot4(TFile f="output_histograms.root") {

define_style();
gROOT->SetStyle("BABAR");

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
gStyle->SetOptStat(0);
CorrectionOfEachAPVPair->SetXTitle("APVPair");
CorrectionOfEachAPVPair->SetYTitle("Particle Correction");
gStyle->SetErrorX(0.5);
CorrectionOfEachAPVPair->SetMarkerSize(1);
CorrectionOfEachAPVPair->SetMarkerStyle(21);
CorrectionOfEachAPVPair->SetMarkerColor(2);
z1(CorrectionOfEachAPVPair,  "Correction Of Each APV Pair", false, false);
APVPairCorrections->SetXTitle("Particle Correction");
APVPairCorrections->SetYTitle("Nr APVPairs");
TF1 *gaussfit = new TF1("gaussfit","gaus",0.,5.);
gaussfit->SetLineColor(3);
gaussfit->SetParName(0,"Const");
APVPairCorrections->Fit("gaussfit","QR",0,0.5,1.5);
gStyle->SetOptFit(000011);
z1(APVPairCorrections,  "APV Pair Corrections", false, false);

TCanvas *myCanvas = new TCanvas("zc","zC",500,0,700,700); myCanvas->Divide(2,3);
APVPairCorrectionsTIB1mono->SetYTitle("APV Pairs");
myCanvas->cd(1); d1layer(APVPairCorrectionsTIB1mono,"TIB1mono", false, false);
myCanvas->cd(2); d1layer(APVPairCorrectionsTIB1stereo,"TIB1stereo", false, false);
myCanvas->cd(3); d1layer(APVPairCorrectionsTIB2,"TIB2", false, false);
APVPairCorrectionsTOB1->SetXTitle("Particle Correction");
myCanvas->cd(4); d1layer(APVPairCorrectionsTOB1,"TOB1", false, false);
myCanvas->cd(5); d1layer(APVPairCorrectionsTOB2,"TOB2", false, false);
myCanvas->Print("ParticleGainLayers.gif");

}


d1layer(TH1 *ah1, TString mytitle, bool logx, bool logy){
  gStyle->SetOptFit(000011);
  gPad->SetBottomMargin(0.10);
  gPad->SetRightMargin(0.01);
  ah1->SetTitle(mytitle);
  ah1->Fit("gaus","Q"); ah1->Draw();
  TF1 * fitfunction = (TF1*) ah1->GetListOfFunctions()->First();
  fitfunction->SetLineColor(3);
  ah1->Draw();
  TLatex tl;
  tl.SetNDC(); //we want to draw this object in the Normalized system [0,1]
  tl.SetTextAlign(12);
  tl.SetTextSize(0.10);
  tl.SetTextColor(4);
  tl.DrawLatex(0.15,0.70,mytitle);
}

void z1(TH1 *ah1, TString mytitle, bool logx, bool logy){
 TCanvas *myCanvas = new TCanvas("zc","zC",500,0,700,700);
 if(logx) myCanvas->SetLogx(1); if(logy) myCanvas->SetLogy(1);
 ah1->SetTitle(mytitle); TString filenamewheretoprint = mytitle+".gif";
 ah1->Draw();
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
