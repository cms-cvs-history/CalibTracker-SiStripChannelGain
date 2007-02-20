
//#dkcira 2006.12.02  -- The first version of this analysis has been copied from S.Mersis full analysis
//#dkcira 2006.12.06  -- added comparison of tickmarks with muons


//-----------------------------------------------------------
#define _TICK_NOT_EXISTS_ -1
#define _TICK_EMPTY_ 0.01 
#define _DEFAULT_TICK_ 600.
//TCanvas c1, c2;
  TH1F *tickHeightOfEachApvPair;
  TH1F* MuonChargeOverTickHeightEachAPVPair;
  TH2F* MuonChargeVsTickHeight;
  TH1F* MuonChargeOverTickHeight;
  TGraphErrors * g_MuonChargeVsTickHeight;

  TH1F *tickGainOfEachApvPair;
  TH1F *tickGainDistribution;
  TH1F* ParticleGainOverTickGainEachAPVPair;
  TH2F* ParticleGainVsTickGain;
  TH1F* ParticleGainOverTickGain;
  TGraphErrors * g_ParticleGainVsTickGain;

//-----------------------------------------------------------
//TString tickmarks(char * tick_timing_file="/castor/cern.ch/cms/testbeam/tkmtcc/P5_data/TBmonitor/TBMonitor0020268_000.root"){ //  tick_timing_file: timing file
//TString tickmarks(char * tick_timing_file="files/TBMonitor0020303_000.root"){ //  tick_timing_file: timing file
TString tickmarks(char * tick_timing_file="files/TBMonitor0020268_000.root"){ //  tick_timing_file: timing file
  // style
  gStyle->SetOptStat(1110); gStyle->SetStatX(0.4); gStyle->SetStatY(0.9); gStyle->SetOptFit(000011);

  // open output file
  TString runnr = extractRunNumber( tick_timing_file );
  cout<<"Ticks for run "<<runnr<<endl;
  TString outputFile = "histos_tickheight" + runnr + ".root";
  TFile foutput(outputFile,"recreate");

  // histograms
  tickHeightOfEachApvPair = new TH1F("tickHeightOfEachApvPair","tickHeightOfEachApvPair",1,0,1); tickHeightOfEachApvPair->SetBit(TH1::kCanRebin);
  MuonChargeOverTickHeightEachAPVPair = new TH1F("MuonChargeOverTickHeightEachAPVPair","MuonChargeOverTickHeightEachAPVPair",1,0,1); MuonChargeOverTickHeightEachAPVPair->SetBit(TH1::kCanRebin);
  MuonChargeOverTickHeight = new TH1F("MuonChargeOverTickHeight","MuonChargeOverTickHeight",200,-4.,4.);
  MuonChargeVsTickHeight = new TH2F("MuonChargeVsTickHeight","MuonChargeVsTickHeight",200,100.,500.,200,400.,900.);

  tickGainOfEachApvPair = new TH1F("tickGainOfEachApvPair","tickGainOfEachApvPair",1,0,1);  tickGainOfEachApvPair->SetBit(TH1::kCanRebin);
  ParticleGainOverTickGainEachAPVPair = new TH1F("ParticleGainOverTickGainEachAPVPair","ParticleGainOverTickGainEachAPVPair",1,0,1); ParticleGainOverTickGainEachAPVPair->SetBit(TH1::kCanRebin);
  ParticleGainOverTickGain = new TH1F("ParticleGainOverTickGain","ParticleGainOverTickGain",200,-4.,4.);
  ParticleGainVsTickGain = new TH2F("ParticleGainVsTickGain","ParticleGainVsTickGain",200,0.7.,1.4.,200,0.7.,1.4.);
  tickGainDistribution = new TH1F("tickGainDistribution","tickGainDistribution",50,0.7,1.4);

  // calculate tickheights
  tickHeightCalc(tick_timing_file);
  // calculate tickgain out of tickheights
  tickGainCalc(tickHeightOfEachApvPair, tickGainOfEachApvPair);
  from2dto1d(tickGainOfEachApvPair, tickGainDistribution);

  //read in histograms of APV gain calibration
  TFile fgaincalib("output_histograms.root");
  TH1F * ChargeOfEachAPVPairControlView =  (TH1F*) fgaincalib.Get("ChargeOfEachAPVPairControlView");
  // compare ChargeOfEachAPVPairControlView with tickHeightOfEachApvPair
  compare_two_methods_1(ChargeOfEachAPVPairControlView, tickHeightOfEachApvPair, MuonChargeOverTickHeightEachAPVPair, MuonChargeOverTickHeight, MuonChargeVsTickHeight);
  foutput.cd(); g_MuonChargeVsTickHeight->Write("g_MuonChargeVsTickHeight"); fgaincalib.cd();
  compare_two_methods_1(CorrectionOfEachAPVPairControlView, tickGainOfEachApvPair, ParticleGainOverTickGainEachAPVPair, ParticleGainOverTickGain, ParticleGainVsTickGain);
  foutput.cd(); g_MuonChargeVsTickHeight->Write("g_ParticleGainVsTickGain"); fgaincalib.cd();

  // close file
  fgaincalib.Close();



  // write histograms to file
  foutput.Write(); foutput.Close();
  return "OK";
}

//-----------------------------------------------------------
void from2dto1d(TH1F * twodims, TH1F * onedim){
  for(int ibin=1; ibin <= twodims->GetNbinsX(); ibin++){
    onedim->Fill(twodims->GetBinContent(ibin));
  }
}


//-----------------------------------------------------------
void tickGainCalc(TH1F * input_distribution, TH1F * correction_distributions){
//  std::cout<<"input_distribution / correction_distributions : "<<input_distribution<<" / "<<correction_distributions<<std::endl;
  float input_distribution_mean=0.;
  for(int ibin=1; ibin <= input_distribution->GetNbinsX(); ibin++){
   input_distribution_mean += input_distribution->GetBinContent(ibin);
  }
  input_distribution_mean = input_distribution_mean / input_distribution->GetNbinsX();
  for(int ibin=1; ibin <= input_distribution->GetNbinsX(); ibin++){
    TString local_bin_label = input_distribution->GetXaxis()->GetBinLabel(ibin);
    double input_content = input_distribution->GetBinContent(ibin);
    double input_error = input_distribution->GetBinError(ibin);
    double correction = input_distribution_mean / input_content;
    if(correction>3 || correction < 0) correction = 1.01;
//    std::cout<<"---- tickGainCalc "<<ibin<<" "<<input_distribution_mean<<" "<<input_content<<" "<<correction<<std::endl;
    correction_distributions->SetBinContent(ibin,correction);
    correction_distributions->GetXaxis()->SetBinLabel(ibin,local_bin_label);
  }
  correction_distributions->LabelsDeflate("X");
}

//-----------------------------------------------------------
//void  compare_two_methods_1(TH1F* ChargeOfEachAPVPairControlView, TH1F* tickHeightOfEachApvPair, TH1F* MuonChargeOverTickHeightEachAPVPair, TH1F* MuonChargeOverTickHeight, TH2F* MuonChargeVsTickHeight, TGraphErrors * g_MuonChargeVsTickHeight){
void  compare_two_methods_1(TH1F* ChargeOfEachAPVPairControlView, TH1F* tickHeightOfEachApvPair, TH1F* MuonChargeOverTickHeightEachAPVPair, TH1F* MuonChargeOverTickHeight, TH2F* MuonChargeVsTickHeight){
  if ( (!ChargeOfEachAPVPairControlView) || (!tickHeightOfEachApvPair) ){
     std::cout<<" compare_two_methods_1  one of the pointers is zero "<<std::ios::hex<<ChargeOfEachAPVPairControlView<<" - "<<tickHeightOfEachApvPair<<endl;
     std::cout<<" compare_two_methods_1  stopping here!"<<endl;
     exit(1);
  }
  vector<double> gx,gy,gex,gey;
  for(int icbin=1; icbin <= ChargeOfEachAPVPairControlView->GetNbinsX(); icbin++){ // loop over charge of APVPairs
    TString charge_bin_label = ChargeOfEachAPVPairControlView->GetXaxis()->GetBinLabel(icbin);
    if(charge_bin_label.Contains("fecCrate")){
      int fecCrate, fecSlot, fecRing, ccuAddr, ccuChan, apvPair;
      sscanf(charge_bin_label.Data(), "fecCrate%d_fecSlot%d_fecRing%d_ccuAddr%d_ccuChan%d_apvPair%d", &fecCrate, &fecSlot, &fecRing, &ccuAddr, &ccuChan, &apvPair);
//      cout<<"charge_bin_label  "<<charge_bin_label<<endl;
//      cout<<"       fecCrate="<<fecCrate<<" fecSlot="<<fecSlot<<" fecRing="<<fecRing<<" ccuAddr="<<ccuAddr<<" ccuChan="<<ccuChan<<" apvPair="<<apvPair<<endl;
      double local_charge_over_path = ChargeOfEachAPVPairControlView->GetBinContent(icbin);
      double local_error_charge_over_path = ChargeOfEachAPVPairControlView->GetBinError(icbin);
      //dk maybe can find some way to avoid double loop
      for(int itbin=1; itbin <= tickHeightOfEachApvPair->GetNbinsX(); itbin++){ // loop over tickheight of APVPairs
        TString tickmark_bin_label = tickHeightOfEachApvPair->GetXaxis()->GetBinLabel(itbin);
        int tickmark_key, tickmark_apvpair;
        sscanf(tickmark_bin_label.Data(), "key%d_apvPair%d", &tickmark_key, &tickmark_apvpair);
        if( getModuleFecRing(tickmark_key)==fecRing  && getModuleFecCcu(tickmark_key)==ccuAddr && getModuleFecChannel(tickmark_key)==ccuChan && apvPair==tickmark_apvpair){
          double local_tickheight = tickHeightOfEachApvPair->GetBinContent(itbin);
          double dedx_over_tickheight= local_charge_over_path/local_tickheight;
          if(local_charge_over_path<1. || local_tickheight<0.02 || fabs(dedx_over_tickheight) > 4. ) dedx_over_tickheight = -0.5; // set to standard dummy value if too large or too small
//          cout<<"dedx="<<local_charge_over_path<<" tickheight="<<local_tickheight<<" dedx/tickheight="<<dedx_over_tickheight<<endl;
          MuonChargeOverTickHeight->Fill(dedx_over_tickheight);
          MuonChargeOverTickHeightEachAPVPair->Fill(tickmark_bin_label,dedx_over_tickheight);
          MuonChargeVsTickHeight->Fill(local_tickheight, local_charge_over_path);
          gx.push_back(local_tickheight);
          gy.push_back(local_charge_over_path);
          gex.push_back(0.);
          gey.push_back(local_error_charge_over_path);
        }
      }
    }
  }
  MuonChargeOverTickHeightEachAPVPair->LabelsDeflate("X");
  int const nelms = gx.size();
  double agx[nelms], agy[nelms], agex[nelms], agey[nelms];
  copy_vector_into_array(gx, agx);
  copy_vector_into_array(gy, agy);
  copy_vector_into_array(gex, agex);
  copy_vector_into_array(gey, agey);
  g_MuonChargeVsTickHeight = new TGraphErrors(nelms,agx,agy,agex,agey);
//  TString graph_title = MuonChargeVsTickHeight->GetTitle();
//  gr->Write(graph_title.Data());
}


//-----------------------------------------------------------
void copy_vector_into_array(std::vector<double> i_vector, double *  a_array){
  for(int i = 0; i<i_vector.size(); ++i){
   a_array[i] = i_vector[i];
  }
}

//-----------------------------------------------------------
void tickHeightCalc( char* tbmonitorFile, char *options = "", int linecolor = 1)
{
  // book histograms
  TLegend *heightLegend = new TLegend(0.73,0.62,0.91,0.96,"");
//  tickHeightOfEachApvPair = new TH1F("tickHeightOfEachApvPair","tickHeightOfEachApvPair",1,0,1); tickHeightOfEachApvPair->SetBit(TH1::kCanRebin);

  TH1F* tickHeight; TH1F *tickHeightByRing[4];
  tickHeightDistribution = createTickHeightHisto("tickHeightDistribution",1,heightLegend);
  tickHeightByRing[0] = createTickHeightHisto("TIB_L2",2, heightLegend);
  tickHeightByRing[1] = createTickHeightHisto("TIB_L3",4, heightLegend);
  tickHeightByRing[2] = createTickHeightHisto(   "TOB",6, heightLegend);
  tickHeightByRing[3] = createTickHeightHisto(   "TEC",8, heightLegend);
  // open input TBMonitor file and create iterator over collection of objects inside it
  TString run_Number = extractRunNumber( tbmonitorFile ); // extract runnr
  cout<<"Reading run #"<<run_Number<<endl<<"Reading file: "<<tbmonitorFile<< endl;
//  TFile *myFile = TFile::Open(tbmonitorFile);
  TRFIOFile *myFile = TRFIOFile::Open(tbmonitorFile);
  TDirectory *basedirectory = (TDirectory *) myFile->GetDirectory("/");
  TCollection *list_of_keys = (TCollection *) basedirectory->GetListOfKeys();
  TIterator *iterator_over_keys = list_of_keys->MakeIterator();
  // loop over keys, get timing directory for each detector module
  TObject * tobj_str;
  while( tobj_str = iterator_over_keys->Next() ){
    char timing_dirname[200];
    int detn, key, fullkey;
    double referenceTick[3];
//    sprintf(timing_dirname, "%s:/%s/Timing", tbmonitorFile, tobj_str->GetName());
    sprintf(timing_dirname, "/%s/Timing", tobj_str->GetName());
    sscanf(tobj_str->GetName(), "Detector%d", &detn);
    gFile->Cd(timing_dirname);
    TDirectory *det = (TDirectory *) gFile->FindObject(tobj_str->GetName());
    TDirectory *time = (TDirectory *) det->FindObject("Timing");
    get_tick_by_dir(time, detn, key, fullkey, referenceTick);
/*
    cout<<"---------- "<<hex<<key<<" - "<<fullkey<<endl;
    cout<<"---------- "<<dec<<key<<" - "<<fullkey<<endl;
        cout<<"FecRing="<<getModuleFecRing(fullkey) << " ";
        cout<<"FecCcu="<<getModuleFecCcu(fullkey) << " ";
        cout<<"FecChannel="<<getModuleFecChannel(fullkey) << " ** " << endl;
*/
    // fill histograms
    tickFill(detn, key, referenceTick, tickHeightDistribution);
    int ring=getModuleFecRing(key); tickFill(detn, key, referenceTick, tickHeightByRing[ring]);
    for (int j=0; j<3; j++) {
      if (referenceTick[j]>=0){
        std::ostringstream oslabel; oslabel.str(""); oslabel <<"key"<<key<< "_apvPair"<<j; TString label = oslabel.str();
        tickHeightOfEachApvPair->Fill(label, referenceTick[j]);
      }
    }
  }
  tickHeightOfEachApvPair->LabelsDeflate("X"); // trim unused bins
}

/*
     c1.cd();
     c1.SetFillColor(0);
     TString fullfilename = tbmonitorFile;
     TString th_title = fullfilename(17,5);
     tickHeightDistribution->SetTitle(th_title);
     tickHeightDistribution->Fit("gaus");
     tickHeightDistribution->GetFunction("gaus")->SetLineColor(3);
     tickHeightDistribution->Draw(options);
     c1.Print("tickheight.gif");
     c2.cd();
     tickHeightOfEachApvPair->Draw("p");
     c2.Print("tickmarkofeachapvpair.gif");
*/


//-----------------------------------------------------------
// create histograms of tickheights
TH1F* createTickHeightHisto(char* histoTitle = "", int color=1, TLegend* legend=NULL){
  TH1F* newTickHeight = new TH1F(histoTitle, histoTitle, 50, 0., 1000.);
  newTickHeight->SetXTitle("Tick height (ADC)");
  newTickHeight->SetLineColor(color);
  if (legend!=NULL) legend->AddEntry(newTickHeight, histoTitle, "L");
  return newTickHeight;
}


//-----------------------------------------------------------
void get_tick_by_dir(TDirectory * time, int detn, int &reducedkey, int &fullkey, double* foundTicks)
{
  TCollection *tcol_str = time->GetListOfKeys();
  TIterator *iter_str = tcol_str->MakeIterator();
  TObject *tobj_str;
  TObject *FoundObject;
  TProfile *tickProfile;
  int tickNum;

  foundTicks[0]=_TICK_NOT_EXISTS_;
  foundTicks[1]=_TICK_NOT_EXISTS_;
  foundTicks[2]=_TICK_NOT_EXISTS_;
  while ((tobj_str = iter_str->Next()))
  {
    if (strstr (tobj_str->GetName(), "tick_chip_0x") ==tobj_str->GetName())
    {
      time->GetObject(tobj_str->GetName(), FoundObject);
      if (FoundObject != 0)
      {
        sscanf(tobj_str->GetName(), "tick_chip_0x%x-%d", &fullkey, &tickNum);
//        cout << hex << "0x" << fullkey << dec << " - " << tickNum << endl;
//        cout << dec << fullkey << " - " << tickNum << endl;
        tickProfile = (TProfile *) FoundObject;
        foundTicks[tickNum] = findTickHeight(tickProfile);
        // cout << getModuleFecRing(fullkey) << " ";
        // cout << getModuleFecCcu(fullkey) << " ";
        // cout << getModuleFecChannel(fullkey) << " ** " << endl;
        reducedkey = BuildCompleteKey(getModuleFecRing(fullkey),
                        getModuleFecCcu(fullkey),
                        getModuleFecChannel(fullkey));
      }
    }
  }
  return;
}


//-----------------------------------------------------------
double findTickHeight(TProfile* tick) {
  double totUnder=0; double totOver=0;
  int nUnder=0; int nOver=0;
  double thres=(tick->GetMaximum()+tick->GetMinimum())/2;
  for (int i=0; i< tick->GetNbinsX(); i++) {
    if (tick->GetBinContent(i)<thres) {
      nUnder++; totUnder+=tick->GetBinContent(i);       
    } else {
      nOver++; totOver+=tick->GetBinContent(i);
    }
  }
  if ((nOver==0)||(nUnder==0)) return _TICK_EMPTY_;
  totOver/=(double)nOver; totUnder/=(double)nUnder;
  return ((totOver)-(totUnder));
}


//-----------------------------------------------------------
int tickFill(int detn, int key, double* referenceTick, TH1F* tickHeightByRing) {
  int found=0;
  for (int i=0; i<3; i++) {
    if (referenceTick[i]>=0) {
      tickHeightByRing->Fill(referenceTick[i]);
      found++;
    }
    if (referenceTick[i]==0) {
      cout << "************************************************************************" << endl;
      cout << "WARNING: fiber 0x" << hex << key <<"-"<<dec<<i<<" had (detector "<<detn<<") empty tick!" <<endl;
      cout << "************************************************************************" << endl;
    }
  }
}


//-----------------------------------------------------------
static unsigned int getModuleFecRing(int x) { return ((x >> 16) & 0xF); }


//-----------------------------------------------------------
static unsigned int getModuleFecCcu(int x) { return ((x >> 8) & 0xFF); }


//-----------------------------------------------------------
static unsigned int getModuleFecChannel(int x) { return ((x) & 0x1F); }


//-----------------------------------------------------------
BuildCompleteKey(int Ring, int Ccu, int Channel) { return ((Ring & 0xF) << 16) | ((Ccu & 0xFF) << 8) | (Channel & 0x1F); }

//-----------------------------------------------------------
TString extractRunNumber( char* inputFile){
  TString basename_filename = gSystem->BaseName(inputFile);
  cout << "basename_filename="<<basename_filename<<endl;
  return basename_filename(11,5);
}


