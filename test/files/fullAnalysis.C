        static unsigned int
GetModuleFecRing (int x)
{
  return ((x >> 16) & 0xF);
}
        static unsigned int
GetModuleFecCcu (int x)
{
  return ((x >> 8) & 0xFF);
}
        static unsigned int
GetModuleFecChannel (int x)
{
  return ((x) & 0x1F);
}

BuildCompleteKey (int Ring, int Ccu, int Channel)
{
  return ((Ring & 0xF) << 16) | ((Ccu & 0xFF) << 8) | (Channel & 0x1F);
}

#define _TICK_NOT_EXISTS_ -1
#define _TICK_EMPTY_ 0.01 
#define _DEFAULT_TICK_ 600.

double tickCalibrationNew[200][3];
double tickCalibrationRef[200][3];


double _suspectXlow=0;
double _suspectXhi=0;
double _suspectYlow=0;
double _suspectYhi=0;

char moduleNameNew[300][50];
int moduleIdNew[300];
int moduleIdRef[300];
char moduleNameRef[300][50];
int moduleNewBadCounter[300];
int moduleSuspectCounter[300]; 

char runNumber1[20], runNumber2[20];
int run_Number1, run_Number2;
int _tickHeightPlotCounter=0;
int _noiseRatioPlotCounter=0;
int _noiseScatterPlotCounter=0;
int _noiseDistributionPlotCounter=0;

//TH1D *clusterdist;

//TCanvas c1, c2, c3, c4, c6, c7;
//TCanvas c6;
TCanvas c5;
//TCanvas c8;

TH1D *get_pedestal_by_key (char *tbmonitorFile1, TCollection * tcol_str,
                TFile * myFile, int keySearch, int& detnfound)
{
  TObject *tobj_str;
  char nome[200];
  int key, detn;
  TH1D *foundHisto;
  bool found = false;
  TIterator *iter_str = tcol_str->MakeIterator ();


  while ((tobj_str = iter_str->Next ()) && (!found))
  {
    // cout << "Adding " << tobj_str->GetName() << "..." << endl;
    myFile->FindObject (tobj_str->GetName ());
    sprintf (nome, "%s:/%s/Pedestal", tbmonitorFile1, tobj_str->GetName ());
    // printf("%s\n", nome);
    sscanf (tobj_str->GetName (), "Detector%d", &detn);
    // cout << "maybe detector " << dec << detn << " ?" << endl;
    gFile->Cd (nome);
    TDirectory *det =(TDirectory *) gFile->FindObject (tobj_str->GetName ());
    // cout << tobj_str->GetName () << endl;
    // cout << "Detector pointer: " << det << endl;
    TDirectory *ped = (TDirectory *) det->FindObject ("Pedestal");
    foundHisto = get_pedestal_by_dir (ped, detn, key);
    // cout << "Detector " << dec << detn << " has key 0x" << hex << key << endl;
    if (key == keySearch)
    {
      detnfound=detn;
      found = true;
    }
  }
  if (found)
  {
    return foundHisto;
  }
  else
  {
    return NULL;
  }
}

        TH1D *
get_pedestal_by_dir (TDirectory * ped, int detn, int &key)
{
  TCollection *tcol_str = ped->GetListOfKeys ();
  TIterator *iter_str = tcol_str->MakeIterator ();
  TObject *tobj_str;
  TObject *FoundObject;
  TH1D *ClusterPlot;
  TH1D *NoiseProfile;
  int n1, n2;
  char expectedNameNoise[100];
  char expectedNameCMSNoise[100];

  // We use this to find raw noise (not used)
  sprintf (expectedNameNoise, "%d-%d", detn * 2, detn * 2 + 1);
  //  We use this to find CommonModeSubtr noise
  sprintf (expectedNameCMSNoise, "%d-%d", detn * 2 + 1, detn * 2 + 2);

  while ((tobj_str = iter_str->Next ()))
  {
    // cout << " " << tobj_str->GetName() << endl;
    if ((strstr (tobj_str->GetName (), "RMS-Pedestal-Id") ==tobj_str->GetName ())
                    && (strstr (tobj_str->GetName (), expectedNameCMSNoise) != NULL))
    {
      // cout << "Found " << tobj_str->GetName () << endl;
      ped->GetObject (tobj_str->GetName (), FoundObject);
      if (FoundObject != 0)
      {
        // Useless by now...
        // sprintf(expectedNameNoise, "RMS-Pedestal-Id%%x-%d-%d", detn*2, detn*2+1);

        // Using this to extract module's Id
        sprintf (expectedNameCMSNoise, "RMS-Pedestal-Id%%x-%d-%d",
                        detn * 2 + 1, detn * 2 + 2);

        // debug
        // cout << expectedNameNoise << " " << expectedNameCMSNoise << endl;

        sscanf (tobj_str->GetName (), expectedNameCMSNoise, &key);
        // cout << hex << "0x" << key << endl;
        NoiseProfile = (TH1D *) FoundObject;
        // cout << GetModuleFecRing(key) << " ";
        // cout << GetModuleFecCcu(key) << " ";
        // cout << GetModuleFecChannel(key) << " ** " << endl;
        key = BuildCompleteKey (GetModuleFecRing (key),
                        GetModuleFecCcu (key),
                        GetModuleFecChannel (key));
      }
    }
  }
  return NoiseProfile;
}

void noiseCalComparisonFill (int key, TH1D * newNoiseProfile, TH1D * referenceNoiseProfile, TH2F * noiseScatter, TH1F * noiseRatio, int detnNew, int detnRef)
{
  int apvP;
  double newCalFactor[3];
  double refCalFactor[3];
  if (newNoiseProfile->GetNbinsX () != referenceNoiseProfile->GetNbinsX ())
  {
    cout << "Error: module 0x" << hex << key << " in new file has ";
    cout << dec << newNoiseProfile->GetNbinsX () << " strips, and in reference file it has ";
    cout << referenceNoiseProfile->GetNbinsX () << "strips." << endl;
  }
  else
  {
    //    cout << "debug: module 0x" << hex << key << endl;
    //    cout << "reference: " << moduleNameRef[detnRef] << endl;
    //    cout << "new      : " << moduleNameNew[detnNew] << endl;
    for (int i=0; i<3; i++) {
      newCalFactor[i] = _DEFAULT_TICK_ / tickCalibrationNew[detnNew][i];
      refCalFactor[i] = _DEFAULT_TICK_ / tickCalibrationRef[detnRef][i];
    }
    for (int i = 0; i < newNoiseProfile->GetNbinsX (); i++)
    {
      apvP=int((double)i/256.);
      // we have a problem here. This goes well Only if missing
      // Lasers are in scheme 3 or 3,2 or 3,2,1 (MAYBE) 
      noiseScatter->Fill ((referenceNoiseProfile->GetBinContent (i)*refCalFactor[apvP]),
                      (newNoiseProfile->GetBinContent (i)*newCalFactor[apvP]));
      if (referenceNoiseProfile->GetBinContent (i) !=0) {
        noiseRatio->Fill ((newNoiseProfile->GetBinContent (i)*(newCalFactor[apvP])) /
                        (referenceNoiseProfile->GetBinContent (i)*refCalFactor[apvP]));


        // debug faulty modules
        if ((referenceNoiseProfile->GetBinContent (i)>0.5)
                        &&(newNoiseProfile->GetBinContent (i)<0.5)) {
          moduleNewBadCounter[detnNew]++; 
        }

        if ((referenceNoiseProfile->GetBinContent (i)>_suspectXlow)
                        &&(referenceNoiseProfile->GetBinContent (i)<_suspectXhi)
                        &&(newNoiseProfile->GetBinContent (i)<_suspectYhi)
                        &&(newNoiseProfile->GetBinContent (i)>_suspectYlow)) {
          moduleSuspectCounter[detnNew]++;
        }
      }
    }
  }
}

void noiseFill (int key, TH1D* noiseProfile, TH1F* noiseDistribution, TH1F* noiseDistributionCal, double tickCalibration[][3], int detn)
{
  double CalFactor[3];
  int apvP;

  for (int i=0; i<3; i++) {
    CalFactor[i] = _DEFAULT_TICK_ / tickCalibration[detn][i];
  }
  for (int i = 0; i < noiseProfile->GetNbinsX (); i++)
  {
    apvP=int((double)i/256.);
    noiseDistribution->Fill (noiseProfile->GetBinContent (i));
    noiseDistributionCal->Fill (noiseProfile->GetBinContent (i) * CalFactor[apvP]);
  }
}


void noiseComparisonFill (int key, TH1D * newNoiseProfile, TH1D * referenceNoiseProfile, TH2F * noiseScatter, TH1F * noiseRatio)
{
  if (newNoiseProfile->GetNbinsX () != referenceNoiseProfile->GetNbinsX ())
  {
    cout << "Error: module 0x" << hex << key << " in new file has ";
    cout << dec << newNoiseProfile->GetNbinsX () << " strips, and in reference file it has ";
    cout << referenceNoiseProfile->GetNbinsX () << "strips." << endl;
  }
  else
  {
    for (int i = 0; i < newNoiseProfile->GetNbinsX (); i++)
    {
      noiseScatter->Fill (referenceNoiseProfile->GetBinContent (i), newNoiseProfile->GetBinContent (i));
      if (referenceNoiseProfile->GetBinContent (i) !=0) {
        noiseRatio->Fill (newNoiseProfile->GetBinContent (i) / referenceNoiseProfile->GetBinContent (i));
      }
    }
  }
}

TH2F* createNoiseScatter(char* title = "", int color=1, TLegend* legend=NULL, char* comment=""){
  TH2F* newNoiseScatter;
  char plotName[100];
  char plotTitle[100];
  char axisTitle[100];

  sprintf (plotName, "NoiseScatter_%d", ++_noiseScatterPlotCounter);
  sprintf (plotTitle, "Noise comparison%s", comment);

  newNoiseScatter = new TH2F (plotName, plotTitle, 1000, 0., 10., 1000, 0., 10.);

  sprintf (axisTitle, "Reference noise: %d",run_Number2);
  newNoiseScatter->SetXTitle(axisTitle);
  sprintf (axisTitle, "New noise: %d",run_Number1);
  newNoiseScatter->SetYTitle(axisTitle);
  newNoiseScatter->SetMarkerSize(.5);
  newNoiseScatter->SetMarkerStyle(2);
  newNoiseScatter->SetMarkerColor(color);

  if (legend!=NULL) {
    legend->AddEntry(newNoiseScatter,title, "P");
  }

  return newNoiseScatter;
}

TH1F* createNoiseRatio(char* title = "", int color=1, TLegend* legend=NULL, char* comment=""){
  TH1F* newNoiseRatio;
  char plotTitle[100];
  char plotName[100];

  sprintf (plotName, "NoiseRatio_%d", ++_noiseRatioPlotCounter);
  sprintf (plotTitle, "Noise comparison: %d/%d%s", run_Number1, run_Number2, comment);
  newNoiseRatio = new TH1F (plotName, plotTitle, 300, 0., 3.);
  newNoiseRatio->SetXTitle("New/reference noise");
  newNoiseRatio->SetLineColor(color);

  if (legend!=NULL) {
    legend->AddEntry(newNoiseRatio,title, "L");
  }

  return newNoiseRatio;
}

TH1F* createNoiseDistribution(int runNumber=0, char* title = "", int color=1, TLegend* legend=NULL, char* comment=""){
  TH1F* newNoise;
  char plotTitle[100];
  char plotName[100];

  sprintf (plotName, "NoiseDistrib_%d", ++_noiseDistributionPlotCounter);
  sprintf (plotTitle, "Noise distribution. Run # %d%s", runNumber, comment);
  newNoise= new TH1F (plotName, plotTitle, 1000, 0., 10.);
  newNoise->SetXTitle("Strip noise");
  newNoise->SetLineColor(color);
  if (legend!=NULL) {
    legend->AddEntry(newNoise,title, "L");
  }

  return newNoise;
}


void get_tick_by_dir (TDirectory * time, int detn, int &key, double* foundTicks)
{
  TCollection *tcol_str = time->GetListOfKeys ();
  TIterator *iter_str = tcol_str->MakeIterator ();
  TObject *tobj_str;
  TObject *FoundObject;
  TProfile *tickProfile;
  int n1, n2;
  int tickNum;

  foundTicks[0]=_TICK_NOT_EXISTS_;
  foundTicks[1]=_TICK_NOT_EXISTS_;
  foundTicks[2]=_TICK_NOT_EXISTS_;
  while ((tobj_str = iter_str->Next ()))
  {
    // cout << " " << tobj_str->GetName() << endl;
    if (strstr (tobj_str->GetName (), "tick_chip_0x") ==tobj_str->GetName ())
    {
      // cout << "Found " << tobj_str->GetName () << endl; 
      time->GetObject (tobj_str->GetName (), FoundObject);
      if (FoundObject != 0)
      {
        sscanf (tobj_str->GetName (), "tick_chip_0x%x-%d", &key, &tickNum);
        // cout << hex << "0x" << key << dec << " - " << tickNum << endl;
        tickProfile = (TProfile *) FoundObject;
        foundTicks[tickNum] = findTickHeight(tickProfile);
        // cout << GetModuleFecRing(key) << " ";
        // cout << GetModuleFecCcu(key) << " ";
        // cout << GetModuleFecChannel(key) << " ** " << endl;
        key = BuildCompleteKey (GetModuleFecRing (key),
                        GetModuleFecCcu (key),
                        GetModuleFecChannel (key));
      }
    }
  }
  return;
}

TH1F* createTickHeight(char* title = "", int color=1, TLegend* legend=NULL){
  TH1F* newTickHeight;
  char plotTitle[100];
  char plotName[100];

  sprintf (plotName, "TickHeight_%d", ++_tickHeightPlotCounter);
  sprintf (plotTitle, "Tick height distribution");
  newTickHeight = new TH1F (plotName, plotTitle, 50, 0., 1000.);
  newTickHeight->SetXTitle("Hick height (ADC)");
  newTickHeight->SetLineColor(color);

  if (legend!=NULL) {
    legend->AddEntry(newTickHeight,title, "L");
  }

  return newTickHeight;
}    

double findTickHeight(TProfile* tick) {
  double thres;
  double totUnder, totOver;
  totUnder=0; totOver=0;
  int nUnder, nOver;

  nUnder=0;
  nOver=0;
  thres=(tick->GetMaximum()+tick->GetMinimum())/2;
  for (int i=0; i< tick->GetNbinsX(); i++) {
    if (tick->GetBinContent(i)<thres) {
      nUnder++;
      totUnder+=tick->GetBinContent(i);       
    } else {
      nOver++;
      totOver+=tick->GetBinContent(i);
    }
  }
  if ((nOver==0)||(nUnder==0)) return _TICK_EMPTY_;
  totOver/=(double)nOver;
  totUnder/=(double)nUnder;
  return ((totOver)-(totUnder));
}

int tickFill (int detn, int key, double* referenceTick, TH1F* tickHeightByRing) {
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

void tickHeightCalc (double tickCalibration[][3], char moduleName[][50], int moduleId[], char *tbmonitorFile2, char *options = "", int linecolor = 1)
{

  // Firest file is the new file
  // The second file is the reference
  // that is to say:
  // "Compare file1 with respect to file2"
  // If you find 2 for all lasers, this means that tick height is doubled
  // in file1 with respect to file2

  int detn, key;
  TObject *tobj_str;
  double referenceTick[3];
  double newTick[3];
  TH1F *tickHeightByRing[4];
  TH1F* tickHeight;
  char plotTitle[100];
  char axisTitle[100];
  int run_Number;
  TLegend *heightLegend = new TLegend(0.73,0.62,0.91,0.96,"");

  TFile *myFile2 = TFile::Open (tbmonitorFile2);

  TDirectory *base2 = (TDirectory *) myFile2->GetDirectory ("/");
  TCollection *tcol_str2 = (TCollection *) base2->GetListOfKeys ();
  TIterator *iter_str2 = tcol_str2->MakeIterator ();
  sscanf (tbmonitorFile2, "TBMonitor%d_000.root", &run_Number);

  tickHeightByRing[0] = createTickHeight("TIB L2",2, heightLegend);
  tickHeightByRing[1] = createTickHeight("TIB L3",4, heightLegend);
  tickHeightByRing[2] = createTickHeight("TOB",6, heightLegend);
  tickHeightByRing[3] = createTickHeight("TEC",8, heightLegend);
  tickHeight = createTickHeight();

  cout << "Reading run #" << run_Number << endl;
  cout << "Reading file: " << tbmonitorFile2 << endl;

  char nome[200];
  char nomebis[200];
  while ((tobj_str = iter_str2->Next ()))
  {
    myFile2->FindObject (tobj_str->GetName ());
    sprintf (nome, "%s:/%s/Timing", tbmonitorFile2, tobj_str->GetName ());
    sprintf (nomebis, "/%s/Timing", tobj_str->GetName ());
    // printf("%s\n", nome);
    sscanf (tobj_str->GetName (), "Detector%d", &detn);
    // cout << tobj_str->GetName() << endl;
    // cout << "Detector " << dec << detn << endl;
    gFile->Cd (nome);
    // cout << "I am now in " << gFile->pwd() << endl;
    // cout << "In directory " << gDirectory->pwd() << endl;
    TDirectory *det =
            (TDirectory *) gFile->FindObject (tobj_str->GetName ());
    // cout << "Detector pointer: " << det << endl;
    TDirectory *time = (TDirectory *) det->FindObject ("Timing");
    // cout << "Timing tick plot pointer: " << time << endl;
    get_tick_by_dir (time, detn, key, referenceTick);
    {
      // Do the comparison
      int ring=GetModuleFecRing(key);
      //tickFill (detn, key, referenceTick, tickHeightByRing[ring]);

      for (int j=0; j<3; j++) {
        // cout << "ok: detn=" << detn<< " j="<< j<< endl;
        // cout << "ok: referenceTick[j]="<<referenceTick[j]<< endl;
        tickCalibration[detn][j]=referenceTick[j];
      }
      sprintf(moduleName[detn], "0x%x", key);
      moduleId[detn]=key;
      tickFill (detn, key, referenceTick, tickHeight);
    }
  }


  // Noew let's define some good plot options

  /*
     c4.cd ();
     c4.SetFillColor(0);
     double max=0; 
     for (int i=0; i<4; i++) {
     if (tickHeightByRing[i]->GetMaximum()>max)
     max=tickHeightByRing[i]->GetMaximum();
     }

     max*=1.1;
     for (int i=0; i<4; i++) {
     tickHeightByRing[i]->SetMaximum(max);
     }

     tickHeightByRing[0]->Draw (options);
     for (int i=1; i<4; i++) {
     tickHeightByRing[i]->Draw ("same");
     }
     heightLegend->Draw();

     c3.cd ();
     c3.SetFillColor(0);
     tickHeight->Draw (options);
     */

}

void compareNoise (char *tbmonitorFile1, char *tbmonitorFile2, char *options = "", int linecolor = 1)
{

  // Firest file is the new file
  // The second file is the reference
  // that is to say:
  // "Compare file1 with respect to file2"
  // If you find 2 for all strips, tis means that noise is doubled
  // in file1 with respect to file2

  int detnNew, key, detnRef;
  TObject *tobj_str;
  TH1D *referenceNoiseProfile;
  TH1D *newNoiseProfile;
  // TH2F *noiseScatter;
  TH2F *noiseScatterByRing[4];
  TH2F *noiseScatterByRingCal[4];
  TH1F *noiseRatioByRing[4];
  TH1F *noiseRatioByRingCal[4];
  TH1F *refNoiseDistribution;
  TH1F *refNoiseDistributionCal;
  TH1F *newNoiseDistribution;
  TH1F *newNoiseDistributionCal;
  TH1F *newNoiseDistributionByRing[4];
  TH1F *newNoiseDistributionByRingCal[4];

  // TH1F *noiseRatio;
  char plotTitle[100];
  char axisTitle[100];
  TVirtualPad* myPad;

  TLegend *scatterLegend = new TLegend(0.76,0.17,0.94,0.48,"");
  TLegend *ratioLegend = new TLegend(0.73,0.62,0.91,0.96,"");
  TLegend *noiseLegend = new TLegend(0.60,0.67,0.97,0.81,"");
  TLegend *noiseBRLegend = new TLegend(0.73,0.62,0.91,0.96,"");

  TFile *myFile1 = TFile::Open (tbmonitorFile1);
  TDirectory *base1 = (TDirectory *) myFile1->GetDirectory ("/");
  TCollection *tcol_str1 = (TCollection *) base1->GetListOfKeys ();
  sscanf (tbmonitorFile1, "TBMonitor%d_000.root", &run_Number1);

  TFile *myFile2 = TFile::Open (tbmonitorFile2);
  TDirectory *base2 = (TDirectory *) myFile2->GetDirectory ("/");
  TCollection *tcol_str2 = (TCollection *) base2->GetListOfKeys ();
  TIterator *iter_str2 = tcol_str2->MakeIterator ();
  sscanf (tbmonitorFile2, "TBMonitor%d_000.root", &run_Number2);

  sprintf (runNumber1, "%d", run_Number1);
  sprintf (runNumber2, "%d", run_Number2);

  noiseRatioByRing[0] = createNoiseRatio("TIB L2",2);
  noiseRatioByRing[1] = createNoiseRatio("TIB L3",4);
  noiseRatioByRing[2] = createNoiseRatio("TOB",6);
  noiseRatioByRing[3] = createNoiseRatio("TEC",8);

  noiseRatioByRingCal[0] = createNoiseRatio("TIB L2",2, ratioLegend, " (calib.)");
  noiseRatioByRingCal[1] = createNoiseRatio("TIB L3",4, ratioLegend, " (calib.)");
  noiseRatioByRingCal[2] = createNoiseRatio("TOB",6, ratioLegend, " (calib.)");
  noiseRatioByRingCal[3] = createNoiseRatio("TEC",8, ratioLegend, " (calib.)");

  // noiseScatter = createNoiseScatter();
  noiseScatterByRing[0] = createNoiseScatter("TIB L2",2);
  noiseScatterByRing[1] = createNoiseScatter("TIB L3",4);
  noiseScatterByRing[2] = createNoiseScatter("TOB",6);
  noiseScatterByRing[3] = createNoiseScatter("TEC",8);

  // noiseScatterCal = createNoiseScatter();
  noiseScatterByRingCal[0] = createNoiseScatter("TIB L2",2, scatterLegend, " (calib.)");
  noiseScatterByRingCal[1] = createNoiseScatter("TIB L3",4, scatterLegend, " (calib.)");
  noiseScatterByRingCal[2] = createNoiseScatter("TOB",6, scatterLegend, " (calib.)");
  noiseScatterByRingCal[3] = createNoiseScatter("TEC",8, scatterLegend, " (calib.)");

  newNoiseDistribution = createNoiseDistribution(run_Number1, "", 1, noiseLegend );
  newNoiseDistributionCal = createNoiseDistribution(run_Number1, "", 2, noiseLegend, " (calib.)");;

  newNoiseDistributionByRing[0] = createNoiseDistribution(run_Number1, "TIB L2", 2 );
  newNoiseDistributionByRing[1] = createNoiseDistribution(run_Number1, "TIB L3", 4 );
  newNoiseDistributionByRing[2] = createNoiseDistribution(run_Number1, "TOB", 6 );
  newNoiseDistributionByRing[3] = createNoiseDistribution(run_Number1, "TEC", 8 );

  newNoiseDistributionByRingCal[0] = createNoiseDistribution(run_Number1, "TIB L2", 2, noiseBRLegend , " (calib.)" );
  newNoiseDistributionByRingCal[1] = createNoiseDistribution(run_Number1, "TIB L3", 4, noiseBRLegend , " (calib.)" );
  newNoiseDistributionByRingCal[2] = createNoiseDistribution(run_Number1, "TOB", 6, noiseBRLegend, " (calib.)" );
  newNoiseDistributionByRingCal[3] = createNoiseDistribution(run_Number1, "TEC", 8, noiseBRLegend, " (calib.)" );

  refNoiseDistribution = createNoiseDistribution(run_Number2, "", 1);
  refNoiseDistributionCal = createNoiseDistribution(run_Number2, "", 2, NULL, " (calib.)");

  cout << "Reading run #" << runNumber1 << ". Reference noise is run #" <<  runNumber2 << endl;

  char nome[200];
  char nomebis[200];

  while ((tobj_str = iter_str2->Next ()))
  {
    // cout << "Adding " << tobj_str->GetName() << "..." << endl;
    myFile2->FindObject (tobj_str->GetName ());
    sprintf (nome, "%s:/%s/Pedestal", tbmonitorFile2, tobj_str->GetName ());
    sprintf (nomebis, "/%s/Pedestal", tobj_str->GetName ());
    // printf("%s\n", nome);
    sscanf (tobj_str->GetName (), "Detector%d", &detnRef);
    // cout << tobj_str->GetName() << endl;
    // cout << "Detector " << dec << detnRef << endl;
    gFile->Cd (nome);
    // cout << "I am now in " << gFile->pwd() << endl;
    // cout << "In directory " << gDirectory->pwd() << endl;
    TDirectory *det =
            (TDirectory *) gFile->FindObject (tobj_str->GetName ());
    // cout << "Detector pointer: " << det << endl;
    TDirectory *ped = (TDirectory *) det->FindObject ("Pedestal");
    // cout << "Pedestal pointer: " << ped << endl;
    referenceNoiseProfile = get_pedestal_by_dir (ped, detnRef, key);
    // cout << " LOOKING FOR 0x" << hex << key << " IN RUN #"<<dec<<runNumber2<<endl;
    newNoiseProfile = get_pedestal_by_key (tbmonitorFile1, tcol_str1, myFile1, key, detnNew);
    if (newNoiseProfile == NULL)
    {
      cout << "Error! Could not find in file " << tbmonitorFile1 << " the plot ";
      cout << " for detector with key 0x" << hex << key;
      cout << " which was in reference run file " << tbmonitorFile2 << endl;
    }
    else
    {
      // Do the comparison
      int ring=GetModuleFecRing(key);
      noiseComparisonFill (key, newNoiseProfile, referenceNoiseProfile, noiseScatterByRing[ring], noiseRatioByRing[ring]);
      noiseCalComparisonFill (key, newNoiseProfile, referenceNoiseProfile, noiseScatterByRingCal[ring], noiseRatioByRingCal[ring], detnNew, detnRef);

      noiseFill (key, newNoiseProfile, newNoiseDistribution, newNoiseDistributionCal, tickCalibrationNew, detnNew );
      noiseFill (key, referenceNoiseProfile, refNoiseDistribution, refNoiseDistributionCal, tickCalibrationRef, detnRef);
      noiseFill (key, newNoiseProfile, newNoiseDistributionByRing[ring], newNoiseDistributionByRingCal[ring], tickCalibrationNew, detnNew );
    }
  }

  /*  c1.cd ();
      c1.SetFillColor(0);
      myPad=c1.GetPad(0);
      myPad->SetLogy();
      noiseRatioByRing[0]->Draw (options);
      for (int i=1; i<4; i++) {
      noiseRatioByRing[i]->Draw ("same");
      }
      ratioLegend->Draw();
      c2.cd ();
      c2.SetFillColor(0);
      noiseScatterByRing[0]->Draw (options);
      for (int i=1; i<4; i++) {
      noiseScatterByRing[i]->Draw ("same");
      }
      scatterLegend->Draw();

      c4.cd ();
      c4.SetFillColor(0);
      myPad=c4.GetPad(0);
      myPad->SetLogy();
      noiseRatioByRingCal[0]->Draw (options);
      for (int i=1; i<4; i++) {
      noiseRatioByRingCal[i]->Draw ("same");
      } 
      ratioLegend->Draw(); */


  c5.cd ();
  c5.SetFillColor(0);
  noiseScatterByRingCal[0]->Draw (options);
  for (int i=1; i<4; i++) {
    noiseScatterByRingCal[i]->Draw ("same");
  }
  scatterLegend->Draw();

/*
  c6.cd();
  c6.SetFillColor(0);
  myPad=c6.GetPad(0);
  myPad->SetLogy();
  newNoiseDistribution->Draw(options);
  newNoiseDistributionCal->Draw("same");
  noiseLegend->Draw();
  */
 /* 
     c7.cd();
     c7.SetFillColor(0);
     myPad=c7.GetPad(0);
     myPad->SetLogy();
     refNoiseDistribution->Draw(options);
     refNoiseDistributionCal->Draw("same");
     noiseLegend->Draw();

*/
  /*
  c8.cd();
  c8.SetFillColor(0);
  myPad=c8.GetPad(0);
  myPad->SetLogy();
  newNoiseDistributionByRing[0]->Draw(options);
  for (int i=1; i<4; i++) {
    newNoiseDistributionByRing[i]->Draw("same");
  }
  noiseBRLegend->Draw();
*/
}

void prova (double tickCalibration[][3], char moduleName[][50]){
  tickCalibration[0][0]=1.2;
  cout << "tickCalibration[0][0] = " << tickCalibration[0][0] << endl ;
  tickCalibration[100][0]=1.3;
  cout << "tickCalibration[100][0] = " << tickCalibration[0][0] << endl ;
  for (int i=100; i<125; i++) { 
    sprintf(moduleName[i], "%d: Ciao!\n", i);
  }
  for (int i=100; i<125; i++) {
    cout << moduleName[i];
  }
}

// Parameters as follows:
//  file1_noise: noise file to be analyzed
//  file2_noise: reference noise file
//  file1_tick: timing file (the last one taken before file1_noise) 
//  file2_tick: timing reference file (the last one taken before file2_noise) 
int fullAnalysis(char* file1_tick, char* file2_tick, char* file1_noise, char* file2_noise, double suspectXlow=-1, double suspectXhi=-1,double suspectYlow=-1,double suspectYhi=-1) {

  _suspectXlow=suspectXlow;
  _suspectXhi=suspectXhi;
  _suspectYlow=suspectYlow;
  _suspectYhi=suspectYhi;

  gStyle->SetOptStat(0);

  for (int i=0; i<200; i++) {
    moduleNewBadCounter[i]=0;
    moduleSuspectCounter[i]=0;
  }

  cout << "Ticks for latest run" << endl;
  tickHeightCalc (tickCalibrationNew, moduleNameNew, moduleIdNew, file1_tick);
  cout << "Ticks for reference run" << endl;
  tickHeightCalc (tickCalibrationRef, moduleNameRef, moduleIdRef, file2_tick);

  compareNoise (file1_noise , file2_noise);

  // Bad modules debug
  for (int i=0; i<200; i++) {
    if (moduleNewBadCounter[i]!=0) {
      cout << "Module " << moduleNameNew[i] << " (number " << i << " : Ring "<< GetModuleFecRing(moduleIdNew[i]) <<", Ccu: "<< GetModuleFecCcu(moduleIdNew[i]) <<", I2CChannel: "<< GetModuleFecChannel(moduleIdNew[i]) <<") has " << moduleNewBadCounter[i] << " strips which died out" << endl;
    }
    if (moduleSuspectCounter[i]!=0) {
      cout << "Module " << moduleNameNew[i] << " (number " << i << " : Ring "<< GetModuleFecRing(moduleIdNew[i]) <<", Ccu: "<< GetModuleFecCcu(moduleIdNew[i]) <<", I2CChannel: "<< GetModuleFecChannel(moduleIdNew[i]) <<") has " << moduleSuspectCounter[i] << " strips in selected zone!" << endl;
    }
  }

  return 0;
}

