
//#dkcira 2006.12.06  -- compare the tickmarks or muons of two different runs
TH1F* ratioEachAPVPair; TH1F* ratioDistribution;


//-----------------------------------------------------------
void kompare(TString fil1="files/histos_tickheight20268.root", TString fil2="files/histos_tickheight20303.root", TString whichHisto = "tickHeightOfEachApvPair"){
//void kompare(TString fil1="files/histos_firsthalf.root", TString fil2="files/histos_secondhalf.root", TString whichHisto = "ChargeOfEachAPVPair"){
  TString outputFile = "histos_comparison.root"; TFile foutput(outputFile,"recreate");
  ratioEachAPVPair  = new TH1F("ratioEachAPVPair","ratioEachAPVPair",1,0,1); ratioEachAPVPair->SetBit(TH1::kCanRebin);
  ratioDistribution = new TH1F("ratioDistribution","ratioDistribution",200,-3,3);
  std::cout<<"Opening files: "<<fil1<<" "<<fil2<<std::endl;
  TFile f1(fil1); TH1F * H1  =  (TH1F*) f1.Get(whichHisto);
  TFile f2(fil2); TH1F * H2  =  (TH1F*) f2.Get(whichHisto);
  std::cout<<" the pointers "<<std::ios::hex<<&f1<<" - "<<&f2<<endl;
  std::cout<<" the pointers "<<std::ios::hex<<H1<<" - "<<H2<<" + "<<f1.Get(whichHisto)<<" - "<<f2.Get(whichHisto)<<endl;
  compare_bin_by_bin(H1,H2,ratioEachAPVPair, ratioDistribution);
  f1.Close(); f2.Close();
  foutput.Write(); foutput.Close();
}


//-----------------------------------------------------------
compare_bin_by_bin(TH1F* H1, TH1F* H2, TH1F* ratioEachBin, TH1F* ratioDistribution){
  if ( (!H1) || (!H2) ){
     std::cout<<" kompare::compare_bin_by_bin  one of the pointers is zero "<<std::ios::hex<<H1<<" - "<<H2<<endl;
     std::cout<<" kompare::compare_bin_by_bin  stopping here!"<<endl;
     exit(1);
  }
  for(int itbin1=1; itbin1 <= H1->GetNbinsX(); itbin1++){
    TString label1 = H1->GetXaxis()->GetBinLabel(itbin1);
    double height1 = H1->GetBinContent(itbin1);
    for(int itbin2=1; itbin2 <= H2->GetNbinsX(); itbin2++){
      TString label2 = H2->GetXaxis()->GetBinLabel(itbin2);
      double height2 = H2->GetBinContent(itbin2);
      if(label1==label2){
        ratioEachBin->Fill(label1,height1/height2);
        ratioDistribution->Fill(height1/height2);
      }
    }
  }
  ratioEachBin->LabelsDeflate("X");
}
