#include "CondFormats/SiStripObjects/interface/SiStripApvGain.h"
#include "CondFormats/DataRecord/interface/SiStripApvGainRcd.h"

#include "CalibTracker/SiStripChannelGain/test/SiStripApvGainDisplay.h"

#include "CommonTools/TrackerMap/interface/TrackerMap.h"
#include "CommonTools/TrackerMap/interface/TmModule.h" 

#include "DataFormats/SiStripDetId/interface/SiStripDetId.h"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TString.h"
#include "TObjArray.h"
#include "TCanvas.h"
#include "TPostScript.h"

#include <iostream>
#include <stdio.h>
#include <sys/time.h>


void SiStripApvGainDisplay::analyze( const edm::Event& e, const edm::EventSetup& iSetup){

  edm::ESHandle<SiStripApvGain> SiStripApvGain_;
  iSetup.get<SiStripApvGainRcd>().get(SiStripApvGain_);
  edm::LogInfo("SiStripApvGainDisplay") << "[SiStripApvGainDisplay::analyze] End Reading SiStripApvGain" << std::endl;
  
  std::vector<uint32_t> detid;
  SiStripApvGain_->getDetIds(detid);
  SiStripApvGain::Range range=SiStripApvGain_->getRange(detid[1]);

  edm::LogInfo("Number of detids ")  << detid.size() << std::endl;
  int apv=0;
  edm::LogInfo(" First det gain values  ")  <<  std::endl; 	
  for(int it=0;it<range.second-range.first;it++){
    edm::LogInfo("SiStripApvGainDisplay")  << "detid " << detid[1] << " \t"
					     << " apv " << apv++ << " \t"
					     << SiStripApvGain_->getApvGain(it,range)     << " \t" 
					     << std::endl; 
  }

  TFile* fFile= new TFile("Gains.root","RECREATE");
  fFile->cd();
  TH1F *fhTIB[3], *fhTOB[3], *fhTEC[3], *fhTID[3]; 
  char title[128];

  for (int i=0;i<3;++i){
    sprintf(title,"TIB_%d",i);
    fhTIB[i] = new TH1F(title,title,50,-0.5,2.5);
    sprintf(title,"TOB_%d",i);
    fhTOB[i] = new TH1F(title,title,50,-0.5,2.5);
    sprintf(title,"TEC_%d",i);
    fhTEC[i] = new TH1F(title,title,50,-0.5,2.5);
    sprintf(title,"TID_%d",i);
    fhTID[i] = new TH1F(title,title,50,-0.5,2.5);
  }
  
  TrackerMap * tkMap[3];
  tkMap[0] = new TrackerMap( "Gain APVPair 1"  );
  tkMap[1] = new TrackerMap( "Gain APVPair 2"  );
  tkMap[2] = new TrackerMap( "Gain APVPair 3"  );
  
  for (size_t id=0;id<detid.size();id++){
    SiStripApvGain::Range range=SiStripApvGain_->getRange(detid[id]);
  
    edm::LogInfo("SiStripApvGainDisplay") << "[SiStripApvGainDisplay::analyze] detid " << detid[id]<< std::endl;

    SiStripDetId a(detid[id]);

    if(range.second-range.first==2){

      tkMap[0]->fill(detid[id],SiStripApvGain_->getApvGain(0,range) ); 
      tkMap[2]->fill(detid[id],SiStripApvGain_->getApvGain(1,range) ); 

      if ( a.subDetector() == SiStripDetId::TIB ){
	fhTIB[0]->Fill(SiStripApvGain_->getApvGain(0,range));
	fhTIB[2]->Fill(SiStripApvGain_->getApvGain(1,range));
      } else if ( a.subDetector() == SiStripDetId::TID ){
	fhTID[0]->Fill(SiStripApvGain_->getApvGain(0,range));
	fhTID[2]->Fill(SiStripApvGain_->getApvGain(1,range));
      } else if ( a.subDetector() == SiStripDetId::TOB ){
	fhTOB[0]->Fill(SiStripApvGain_->getApvGain(0,range));
	fhTOB[2]->Fill(SiStripApvGain_->getApvGain(1,range));
      } else if ( a.subDetector() == SiStripDetId::TEC ){
	fhTEC[0]->Fill(SiStripApvGain_->getApvGain(0,range));
	fhTEC[2]->Fill(SiStripApvGain_->getApvGain(1,range));
      }

    } else {

      tkMap[0]->fill(detid[id],SiStripApvGain_->getApvGain(0,range) );
      tkMap[1]->fill(detid[id],SiStripApvGain_->getApvGain(1,range) ); 
      tkMap[2]->fill(detid[id],SiStripApvGain_->getApvGain(2,range) ); 

      if ( a.subDetector() == SiStripDetId::TIB ){
	fhTIB[0]->Fill(SiStripApvGain_->getApvGain(0,range));
	fhTIB[1]->Fill(SiStripApvGain_->getApvGain(1,range));
	fhTIB[2]->Fill(SiStripApvGain_->getApvGain(2,range));
      } else if ( a.subDetector() == SiStripDetId::TID ){
	fhTID[0]->Fill(SiStripApvGain_->getApvGain(0,range));
	fhTID[1]->Fill(SiStripApvGain_->getApvGain(1,range));
	fhTID[2]->Fill(SiStripApvGain_->getApvGain(2,range));
      } else if ( a.subDetector() == SiStripDetId::TOB ){
	fhTOB[0]->Fill(SiStripApvGain_->getApvGain(0,range));
	fhTOB[1]->Fill(SiStripApvGain_->getApvGain(1,range));
	fhTOB[2]->Fill(SiStripApvGain_->getApvGain(2,range));
      } else if ( a.subDetector() == SiStripDetId::TEC ){
	fhTEC[0]->Fill(SiStripApvGain_->getApvGain(0,range));
	fhTEC[1]->Fill(SiStripApvGain_->getApvGain(1,range));
	fhTEC[2]->Fill(SiStripApvGain_->getApvGain(2,range));
      }

    }
  }
  tkMap[0]->print(true, 0, 0, "svgmap1.xml");
  tkMap[1]->print(true, 0, 0, "svgmap2.xml");
  tkMap[2]->print(true, 0, 0, "svgmap3.xml");

  fFile->Write();
  fFile->Close();
}
