#ifndef SiStripApvGainDisplay_H
#define SiStripApvGainDisplay_H

// system include files
//#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "TFile.h"

class SiStripApvGainDisplay : public edm::EDAnalyzer {

 public:
  explicit SiStripApvGainDisplay( const edm::ParameterSet& ):m_cacheID_(0){fFile=new TFile("Gains.root","RECREATE");}
  ~SiStripApvGainDisplay(){  fFile->Close();  delete fFile;}
  
  void analyze( const edm::Event&, const edm::EventSetup& );
  
 private:

  TFile* fFile;
  unsigned long long m_cacheID_;  
};
#endif
