#include "CondFormats/SiStripObjects/interface/SiStripApvGain.h"
#include "CondFormats/DataRecord/interface/SiStripApvGainRcd.h"

#include "CalibTracker/SiStripChannelGain/test/SiStripApvGainDisplay.h"

#include "CommonTools/TrackerMap/interface/TrackerMap.h"
#include "CommonTools/TrackerMap/interface/TmModule.h" 

#include "DataFormats/SiStripDetId/interface/SiStripDetId.h"

#include "TROOT.h"
#include "TTree.h"
#include "TH1F.h"
#include "TString.h"
#include "TObjArray.h"
#include "TCanvas.h"
#include "TPostScript.h"

#include <sstream>
#include <iostream>
#include <stdio.h>
#include <sys/time.h>
#include <iomanip>

void SiStripApvGainDisplay::analyze( const edm::Event& e, const edm::EventSetup& iSetup){

  unsigned long long cacheID = iSetup.get<SiStripApvGainRcd>().cacheIdentifier();
  
  if (m_cacheID_ == cacheID) 
    return;

  m_cacheID_=cacheID;

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

  std::stringstream ss;

  fFile->cd();
  ss.str("");
  ss<< e.id().run();
  fFile->mkdir(ss.str().c_str());
  fFile->cd(ss.str().c_str());
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
  
    SiStripDetId a(detid[id]);

    //Looking only at the first apv of an apvPair
    if(range.second-range.first==4){

      edm::LogInfo("SiStripApvGainDisplay") << "[SiStripApvGainDisplay::analyze] 4 apvs det " << detid[id]<< std::endl;


      tkMap[0]->fill(detid[id],SiStripApvGain_->getApvGain(0,range) ); 
      tkMap[2]->fill(detid[id],SiStripApvGain_->getApvGain(2,range) ); 

      if ( a.subDetector() == SiStripDetId::TIB ){
	fhTIB[0]->Fill(SiStripApvGain_->getApvGain(0,range));
	fhTIB[2]->Fill(SiStripApvGain_->getApvGain(2,range));
      } else if ( a.subDetector() == SiStripDetId::TID ){
	fhTID[0]->Fill(SiStripApvGain_->getApvGain(0,range));
	fhTID[2]->Fill(SiStripApvGain_->getApvGain(2,range));
      } else if ( a.subDetector() == SiStripDetId::TOB ){
	fhTOB[0]->Fill(SiStripApvGain_->getApvGain(0,range));
	fhTOB[2]->Fill(SiStripApvGain_->getApvGain(2,range));
      } else if ( a.subDetector() == SiStripDetId::TEC ){
	fhTEC[0]->Fill(SiStripApvGain_->getApvGain(0,range));
	fhTEC[2]->Fill(SiStripApvGain_->getApvGain(2,range));
      }

    } else if(range.second-range.first==6){

      edm::LogInfo("SiStripApvGainDisplay") << "[SiStripApvGainDisplay::analyze] 6 apvs det " << detid[id]<< std::endl;

      tkMap[0]->fill(detid[id],SiStripApvGain_->getApvGain(0,range) );
      tkMap[1]->fill(detid[id],SiStripApvGain_->getApvGain(2,range) ); 
      tkMap[2]->fill(detid[id],SiStripApvGain_->getApvGain(4,range) ); 

      if ( a.subDetector() == SiStripDetId::TIB ){
	fhTIB[0]->Fill(SiStripApvGain_->getApvGain(0,range));
	fhTIB[1]->Fill(SiStripApvGain_->getApvGain(2,range));
	fhTIB[2]->Fill(SiStripApvGain_->getApvGain(4,range));
      } else if ( a.subDetector() == SiStripDetId::TID ){
	fhTID[0]->Fill(SiStripApvGain_->getApvGain(0,range));
	fhTID[1]->Fill(SiStripApvGain_->getApvGain(2,range));
	fhTID[2]->Fill(SiStripApvGain_->getApvGain(4,range));
      } else if ( a.subDetector() == SiStripDetId::TOB ){
	fhTOB[0]->Fill(SiStripApvGain_->getApvGain(0,range));
	fhTOB[1]->Fill(SiStripApvGain_->getApvGain(2,range));
	fhTOB[2]->Fill(SiStripApvGain_->getApvGain(4,range));
      } else if ( a.subDetector() == SiStripDetId::TEC ){
	fhTEC[0]->Fill(SiStripApvGain_->getApvGain(0,range));
	fhTEC[1]->Fill(SiStripApvGain_->getApvGain(2,range));
	fhTEC[2]->Fill(SiStripApvGain_->getApvGain(4,range));
      }

    } else {
      edm::LogInfo("SiStripApvGainDisplay") << "[SiStripApvGainDisplay::analyze] det " << detid[id]<< " has only " << range.second-range.first << " apvs recorded" << std::endl;
    }
  }

  tkMap[0]->showPalette(true);
  tkMap[1]->showPalette(true);
  tkMap[2]->showPalette(true);
  ss.str(""); ss << "TkMapGain_Run_" << std::setw(6) << std::setfill('0') << e.id().run() << std::setw(0) << "_Fiber";
  std::stringstream ssFull;

  for (int i=0;i<3;++i){
    ssFull.str(""); ssFull << ss.str() << i <<".pdf";
  tkMap[i]->save(true, 0, 0, ssFull.str().c_str());
    ssFull.str(""); ssFull << ss.str() << i << ".png";
  tkMap[i]->save(true, 0, 0, ssFull.str().c_str());
    ssFull.str(""); ssFull << ss.str() << i;
  tkMap[i]->print(true, 0, 0, ssFull.str().c_str());
  }

  fFile->Write();
  fFile->Save();
  for (int i=0;i<3;++i){
   delete  fhTIB[i];
   delete  fhTID[i];
   delete  fhTOB[i];
   delete  fhTEC[i];
  }
}
