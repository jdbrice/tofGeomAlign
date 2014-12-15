#include <iostream>
#include <math.h>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <iterator>

#include "StEventTypes.h"
#include "Stypes.h"
#include "StThreeVectorF.hh"
#include "StMeasuredPoint.h"
#include "StDedxPidTraits.h"
#include "StBTofPidTraits.h"
#include "StTrackPidTraits.h"
#include "StBTofPidTraits.h"
#include "StBTofCollection.h"
#include "StBTofHit.h"
#include "StBTofHeader.h"
#include "StarClassLibrary/StParticleTypes.hh"
#include "StarClassLibrary/StParticleDefinition.hh"
#include "StMuDSTMaker/COMMON/StMuUtilities.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StHelix.hh"
#include "StTrackGeometry.h"
#include "StDcaGeometry.h"
#include "StParticleTypes.hh"
#include "StTpcDedxPidAlgorithm.h"
#include "StHit.h"
#include "StAssociationMaker/StTrackPairInfo.hh"
#include "StEventUtilities/StuRefMult.hh"
#include "PhysicalConstants.h"
#include "StPhysicalHelixD.hh"
#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "StMemoryInfo.hh"
#include "StMessMgr.h"
#include "StTimer.hh"

#include "StBTofUtil/tofPathLength.hh"
#include "StBTofUtil/StBTofGeometry.h"
#include "StBTofUtil/StBTofDaqMap.h"
#include "StBTofUtil/StBTofHitCollection.h"
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuBTofHit.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuBTofPidTraits.h"
#include "StMuDSTMaker/COMMON/StMuTriggerIdCollection.h"

#include <StVpdTriggerDetector.h>
#include "StRcpMaker.h"   

#include "StEnumerations.h"

ClassImp(StRcpMaker)

//---------------------------------------------------------------------------
/// constructor sets default parameters
StRcpMaker::StRcpMaker( const Char_t *name =      "tofNtuple", 
                                      const Char_t *outname =   "tofntuple.root") 
                                      : StMaker(name) {
  // the output filename
  mTupleFileName = outname;
  
  // default debug options
  doPrintMemoryInfo = kFALSE;
  doPrintCpuInfo    = kFALSE;

}

/// default empty destructor
StRcpMaker::~StRcpMaker( ){ 

}

//---------------------------------------------------------------------------
/// initialize ntuple and daqmap, and reset counters
Int_t StRcpMaker::Init( ){

  if ( (string)"" != mTupleFileName ) bookNtuples();

  mAcceptedEvents = 0;
  mPvpdEntries = 0;
  mBTofEvents  = 0;
  mBTofEntries = 0;

  return kStOK;
}


Int_t StRcpMaker::InitRun( int runnumber ) {

  return kStOK;
}

Int_t StRcpMaker::FinishRun( int runnumber ) {
  return kStOK;
}

/// write and close the ntuple file
Int_t StRcpMaker::Finish() {

  if ( (string)"" != mTupleFileName ){
    
    mTupleFile->Write();
    mTupleFile->Close();
    
    LOG_INFO  << "StRcpMaker::Finish() ntuple file " << mTupleFileName  << " closed." << endm;

  }
 
 
  LOG_INFO << "StRcpMaker -- statistics" << endm;
  LOG_INFO << " accepted events     : " << mAcceptedEvents << endm;
  LOG_INFO << " pVPD entries        : " << mPvpdEntries << endm;
  LOG_INFO << " BTof entries/events : " << mBTofEntries << "/" << mBTofEvents << endm;
  return kStOK;
}


Int_t StRcpMaker::Make(){
  LOG_INFO << "StRcpMaker -- welcome" << endm;

  if(mMuDstIn) 
    processMuDst();
  else 
    LOG_INFO << " Set MuDstIn to kTRUE" << endm;

  return kStOK;
}


void StRcpMaker::processMuDst() {

  StMuDstMaker *muDstMaker = (StMuDstMaker*) GetMaker( "MuDst" );

  if( !muDstMaker ) {
    LOG_WARN << " No MuDstMaker ... goodbye" << endm;
    return;
  }
  muDst = muDstMaker->muDst();
  if(!muDst) {
    LOG_WARN << " No MuDst ... goodbye" << endm;
    return;
  }

  mAcceptedEvents++;

  StTimer timer;

  if (doPrintCpuInfo) timer.start();
  if (doPrintMemoryInfo) StMemoryInfo::instance()->snapshot();

  //.........................................................................
  // Collect global data for both ntuples

  StMuEvent *muEvent = muDst->event();
  Bool_t istrigger = 0;

  
  if(muEvent) {

    if(Debug()) { LOG_INFO<<"runId: "<<muEvent->runId()<<"  evtId: "<<muEvent->eventId()<<endm; }
    // the run number
    mCellData.run = muEvent->runId();    
    // the event number
    mCellData.evt = muEvent->eventId();
    // the reference multiplicity
    mCellData.refMult = muEvent->refMult();
    // the reference multiplicity positive tracks
    mCellData.refMultPos = muEvent->refMultPos();
    // the reference multiplicity negative tracks
    mCellData.refMultNeg = muEvent->refMultNeg();

    vector<unsigned int> tIds = muEvent->triggerIdCollection().nominal().triggerIds();


    cout << "got :" << tIds.size() << " triggers " << endl; 
    mCellData.nTriggers = tIds.size();
    for ( int i = 0; i < mCellData.nTriggers; i++ )
      mCellData.triggerIds[ i ] = tIds[ i ];

    istrigger = ((muEvent->triggerIdCollection().nominal().isTrigger(7)) 
                || (muEvent->triggerIdCollection().nominal().isTrigger(250107)));

  } else {

    mCellData.run = 0;
    mCellData.evt = 0;
    mCellData.refMult = -999;
    mCellData.refMultPos = -999;
    mCellData.refMultNeg = -999;

  }


  // get the vpd trigger electronic side data
  // BBQ ADC/TDC
  // MXQ ADC/TDC
  StVpdTriggerDetector vpdDetector = muEvent->vpdTriggerDetector();
  
  // there are 16 tubes in the trigger side electronics, not 19
  // may change in the future
  uint numVPD = vpdDetector.numberOfVpdCounters();
  
  for ( int iVPD = 0; iVPD < numVPD; iVPD ++ ){

    mCellData.vpdBbqAdcEast[ iVPD ] = vpdDetector.ADC( east, iVPD+1 );
    mCellData.vpdBbqAdcWest[ iVPD ] = vpdDetector.ADC( west, iVPD+1 );

    mCellData.vpdBbqTdcEast[ iVPD ] = vpdDetector.TDC( east, iVPD+1 );
    mCellData.vpdBbqTdcWest[ iVPD ] = vpdDetector.TDC( west, iVPD+1 );

    mCellData.vpdMxqAdcEast[ iVPD ] = vpdDetector.ADCmxq( east, iVPD+1 );
    mCellData.vpdMxqAdcWest[ iVPD ] = vpdDetector.ADCmxq( west, iVPD+1 );

    mCellData.vpdMxqTdcEast[ iVPD ] = vpdDetector.TDCmxq( east, iVPD+1 );
    mCellData.vpdMxqTdcWest[ iVPD ] = vpdDetector.TDCmxq( west, iVPD+1 );

  }




  StThreeVectorD pVtx(-999., -999., -999.);  
  if(muDst->primaryVertex()) {
    pVtx = muDst->primaryVertex()->position();
  }
  mCellData.vertexX = pVtx.x();        
  mCellData.vertexY = pVtx.y();              
  mCellData.vertexZ = pVtx.z();              


  //-- read in TOF info
  StBTofHeader* tofHeader = muDst->btofHeader();
  if(!tofHeader) {
    LOG_WARN << " No TOF Header ... goodbye" << endm;
    return;
  }
  if(Debug()&&tofHeader) { LOG_INFO << "got tof Header"<<endm; }
  
  mCellData.tStart        = tofHeader->tStart();
  mCellData.tStartError   = tofHeader->tStartError();
  mCellData.nTZero        = tofHeader->nTzero();
  mCellData.tDiff         = tofHeader->tDiff();
  mCellData.vpdVz         = tofHeader->vpdVz();

  //initialize vpd content
  for(int i=0;i<19;i++){
    mCellData.vpdLeEast[i] = 0;
    mCellData.vpdTotEast[i] = 0;
    mCellData.vpdLeWest[i] = 0;
    mCellData.vpdTotWest[i] = 0;
  } 

  unsigned int vpdEast=0, vpdWest=0, nVpdEast=0, nVpdWest=0;
  int nMax = muDst->numberOfBTofHit();
  int ntofhits = 0;
  
  if(Debug()) { LOG_INFO << nMax << " hits"<<endm; }

  for(int i=0;i<nMax;i++) {

    StMuBTofHit *aHit = (StMuBTofHit *)muDst->btofHit(i);
    int trayId = aHit->tray();
    
    StThreeVector<double> globalPos;
    
    if(Debug()) { LOG_INFO << "tray Id = "<<trayId<<endm; }
    
    // VPD EAST
    if(trayId==122){

      int tubeId = aHit->cell()-1;
      mCellData.vpdLeEast[tubeId] = aHit->leadingEdgeTime();
      mCellData.vpdTotEast[tubeId] = aHit->tot();
      vpdEast += 1<<tubeId;
      nVpdEast++;
	
    // VPD WEST
    } else if(trayId==121) {

      int tubeId = aHit->cell()-1;
      mCellData.vpdLeWest[tubeId] = aHit->leadingEdgeTime();
      mCellData.vpdTotWest[tubeId] = aHit->tot();
      vpdWest += 1<<tubeId;
      nVpdWest++;

    }
    else if(trayId<=120&&trayId>=0) {//TOF

      StMuTrack *globalTrack      = aHit->globalTrack();
      if(!globalTrack) continue;

      if(Debug()) { LOG_INFO << "got global track from TOF hit" << endm; }

      mCellData.tray[ntofhits]    = aHit->tray();
      mCellData.module[ntofhits]  = aHit->module();
      mCellData.cell[ntofhits]    = aHit->cell();
      mCellData.leTime[ntofhits]  = aHit->leadingEdgeTime();
      mCellData.tot[ntofhits]     = aHit->tot();

      if(Debug()) { LOG_INFO <<"tray/module/cell/letime/tot="<<aHit->tray()<<"/"<<aHit->module()<<"/"<<aHit->cell()<<"/"<<aHit->leadingEdgeTime()<<"/"<<aHit->tot()<<endm; }

      StMuTrack *pTrack = aHit->primaryTrack();
      if(!pTrack) continue;
      if(pTrack->vertexIndex()!=0) continue;  // only select the first one

      StThreeVectorF momentum = pTrack->momentum();

      mCellData.trackId[ntofhits]   = (Int_t) globalTrack->id();
      mCellData.charge[ntofhits]    = globalTrack->charge();
      mCellData.pt[ntofhits]        = momentum.perp();
      mCellData.eta[ntofhits]       = momentum.pseudoRapidity();
      mCellData.phi[ntofhits]       = momentum.phi();

      mCellData.pX[ ntofhits ]      = momentum.x();
      mCellData.pY[ ntofhits ]      = momentum.y();
      mCellData.pZ[ ntofhits ]      = momentum.z();
      mCellData.p[ ntofhits ]       = momentum.magnitude();

      mCellData.dedx[ntofhits]      = globalTrack->dEdx()*1e6;
      mCellData.nHitsDedx[ntofhits] =  globalTrack->nHitsDedx();

      StMuBTofPidTraits tofpid      = globalTrack->btofPidTraits();

      mCellData.matchFlag[ntofhits] = tofpid.matchFlag();
      mCellData.yLocal[ntofhits]    = tofpid.yLocal();
      mCellData.zLocal[ntofhits]    = tofpid.zLocal();
      mCellData.thetaLocal[ntofhits]= tofpid.thetaLocal();

      globalPos                     = tofpid.position();

      mCellData.xGlobal[ntofhits]   = globalPos.x();
      mCellData.yGlobal[ntofhits]   = globalPos.y();
      mCellData.zGlobal[ntofhits]   = globalPos.z();
   
      mCellData.tof[ntofhits]       = tofpid.timeOfFlight();
      mCellData.length[ntofhits]    = tofpid.pathLength();
      mCellData.beta[ntofhits]      = tofpid.beta();

      mCellData.nHits[ntofhits]     = globalTrack->nHits();
      mCellData.nHitsFit[ntofhits]  = globalTrack->nHitsFit(kTpcId);
      mCellData.nHitsPossible[ntofhits]  = globalTrack->nHitsPoss(kTpcId);

      mCellData.nSigE[ntofhits]     = globalTrack->nSigmaElectron();
      mCellData.nSigPi[ntofhits]    = globalTrack->nSigmaPion();
      mCellData.nSigK[ntofhits]     = globalTrack->nSigmaKaon();
      mCellData.nSigP[ntofhits]     = globalTrack->nSigmaProton();

      //-- project track onto beam line
      mCellData.dcaX[ntofhits]      = pTrack->dcaGlobal().x();
      mCellData.dcaY[ntofhits]      = pTrack->dcaGlobal().y();
      mCellData.dcaZ[ntofhits]      = pTrack->dcaGlobal().z();

      //-- get path length
      if(Debug()) LOG_INFO << "pathLength(TOF)=" << mCellData.length[ntofhits] << endm;

      mCellData.length[ntofhits]    = tofPathLength(&pVtx, &globalPos, pTrack->helix().curvature());
      
      if(Debug()) LOG_INFO << "pathLength=" << mCellData.length[ntofhits] << endm;

      ntofhits++;
    }
  }
  mCellData.nTofHits          = ntofhits;
  mCellData.vpdEast           = vpdEast;
  mCellData.vpdWest           = vpdWest;
  mCellData.numberOfVpdEast   = nVpdEast;
  mCellData.numberOfVpdWest   = nVpdWest;

  if(Debug()) { LOG_INFO << " There are " << ntofhits << " tof hits in this event! " << endm; }
  mBTofEntries = ntofhits;
  mCellTuple->Fill();

  //- debug info`
  if (doPrintMemoryInfo) {
        StMemoryInfo::instance()->snapshot();
        StMemoryInfo::instance()->print();
  }
  if (doPrintCpuInfo) {
    timer.stop();
    LOG_INFO << "CPU time for StEventMaker::Make(): "
	 << timer.elapsedTime() << " sec\n" << endm;
  }
}

//---------------------------------------------------------------------------
/// create and initialize ntuple and TTrees
void StRcpMaker::bookNtuples(){
  mTupleFile = new TFile(mTupleFileName.c_str(), "RECREATE");
  LOG_INFO << "StRcpMaker::bookNtuples()  file "
       << mTupleFileName << " opened" << endm;

  // BTof calibration ntuple
  mCellTuple = new TTree("tof","BTof cell data");
  mCellTuple->SetAutoSave(1000);
  mCellTuple->Branch("run",&mCellData.run,"run/I");
  mCellTuple->Branch("evt",&mCellData.evt,"evt/I");
  mCellTuple->Branch("nTriggers",&mCellData.nTriggers,"nTriggers/I");
  mCellTuple->Branch("triggerIds",&mCellData.triggerIds,"triggerIds[nTriggers]/i");
  
  mCellTuple->Branch("refMult",&mCellData.refMult,"refMult/s");
  mCellTuple->Branch("refMultPos",&mCellData.refMultPos,"refMultPos/s");
  mCellTuple->Branch("refMultNeg",&mCellData.refMultNeg,"refMultNeg/s");

  mCellTuple->Branch("vertexX",&mCellData.vertexX,"vertexX/F");
  mCellTuple->Branch("vertexY",&mCellData.vertexY,"vertexY/F");
  mCellTuple->Branch("vertexZ",&mCellData.vertexZ,"vertexZ/F");
  mCellTuple->Branch("vpdEast",&mCellData.vpdEast,"vpdEast/I");
  mCellTuple->Branch("vpdWest",&mCellData.vpdWest,"vpdWest/I");
  mCellTuple->Branch("numberOfVpdEast",&mCellData.numberOfVpdEast,"numberOfVpdEast/I");
  mCellTuple->Branch("numberOfVpdWest",&mCellData.numberOfVpdWest,"numberOfVpdWest/I");
  
  mCellTuple->Branch("tStartError",&mCellData.tStartError,"tStartError/D");
  mCellTuple->Branch("nTZero",&mCellData.nTZero,"nTZero/I");

  mCellTuple->Branch("tDiff",&mCellData.tDiff,"tDiff/F");
  mCellTuple->Branch("tStart",&mCellData.tStart,"tStart/D");
  mCellTuple->Branch("vpdVz",&mCellData.vpdVz,"vpdVz/F");

  mCellTuple->Branch("vpdLeEast",&mCellData.vpdLeEast,"vpdLeEast[19]/D");
  mCellTuple->Branch("vpdLeWest",&mCellData.vpdLeWest,"vpdLeWest[19]/D");
  mCellTuple->Branch("vpdTotEast",&mCellData.vpdTotEast,"vpdTotEast[19]/D");
  mCellTuple->Branch("vpdTotWest",&mCellData.vpdTotWest,"vpdTotWest[19]/D");

  mCellTuple->Branch("vpdBbqAdcEast",&mCellData.vpdBbqAdcEast,"vpdBbqAdcEast[16]/s");
  mCellTuple->Branch("vpdBbqAdcWest",&mCellData.vpdBbqAdcWest,"vpdBbqAdcWest[16]/s");
  mCellTuple->Branch("vpdBbqTdcEast",&mCellData.vpdBbqTdcEast,"vpdBbqTdcEast[16]/s");
  mCellTuple->Branch("vpdBbqTdcWest",&mCellData.vpdBbqTdcWest,"vpdBbqTdcWest[16]/s");

  mCellTuple->Branch("vpdMxqAdcEast",&mCellData.vpdMxqAdcEast,"vpdMxqAdcEast[16]/s");
  mCellTuple->Branch("vpdMxqAdcWest",&mCellData.vpdMxqAdcWest,"vpdMxqAdcWest[16]/s");
  mCellTuple->Branch("vpdMxqTdcEast",&mCellData.vpdMxqTdcEast,"vpdMxqTdcEast[16]/s");
  mCellTuple->Branch("vpdMxqTdcWest",&mCellData.vpdMxqTdcWest,"vpdMxqTdcWest[16]/s");


  mCellTuple->Branch("nTofHits",&mCellData.nTofHits,"nTofHits/I");
  mCellTuple->Branch("tray",&mCellData.tray,"tray[nTofHits]/I");
  mCellTuple->Branch("module",&mCellData.module,"module[nTofHits]/I");
  mCellTuple->Branch("cell",&mCellData.cell,"cell[nTofHits]/I");
  mCellTuple->Branch("leTime",&mCellData.leTime,"leTime[nTofHits]/D");
  mCellTuple->Branch("tot",&mCellData.tot,"tot[nTofHits]/D");
  mCellTuple->Branch("matchFlag",&mCellData.matchFlag,"matchFlag[nTofHits]/I");
  mCellTuple->Branch("yLocal",&mCellData.yLocal,"yLocal[nTofHits]/F");
  mCellTuple->Branch("zLocal",&mCellData.zLocal,"zLocal[nTofHits]/F");
  mCellTuple->Branch("thetaLocal",&mCellData.thetaLocal,"thetaLocal[nTofHits]/F");
  mCellTuple->Branch("xGlobal",&mCellData.xGlobal,"xGlobal[nTofHits]/F");
  mCellTuple->Branch("yGlobal",&mCellData.yGlobal,"yGlobal[nTofHits]/F");
  mCellTuple->Branch("zGlobal",&mCellData.zGlobal,"zGlobal[nTofHits]/F");
  mCellTuple->Branch("trackId",&mCellData.trackId,"trackId[nTofHits]/I");
  mCellTuple->Branch("charge",&mCellData.charge,"charge[nTofHits]/I");
  
  mCellTuple->Branch("pt",&mCellData.pt,"pt[nTofHits]/F");
  mCellTuple->Branch("eta",&mCellData.eta,"eta[nTofHits]/F");
  mCellTuple->Branch("phi",&mCellData.phi,"phi[nTofHits]/F");
  
  mCellTuple->Branch("pX",&mCellData.pX,"pX[nTofHits]/F");
  mCellTuple->Branch("pY",&mCellData.pY,"pY[nTofHits]/F");
  mCellTuple->Branch("pZ",&mCellData.pZ,"pZ[nTofHits]/F");
  mCellTuple->Branch("p",&mCellData.p,"p[nTofHits]/F");

  mCellTuple->Branch("dcaX",&mCellData.dcaX,"dcaX[nTofHits]/F");
  mCellTuple->Branch("dcaY",&mCellData.dcaY,"dcaY[nTofHits]/F");
  mCellTuple->Branch("dcaZ",&mCellData.dcaZ,"dcaZ[nTofHits]/F");

  mCellTuple->Branch("nHits",&mCellData.nHits,"nHits[nTofHits]/I");
  mCellTuple->Branch("nHitsFit",&mCellData.nHitsFit,"nHitsFit[nTofHits]/I");
  mCellTuple->Branch("nHitsPossible",&mCellData.nHitsPossible,"nHitsPossible[nTofHits]/I");
  mCellTuple->Branch("nHitsDedx",&mCellData.nHitsDedx,"nHitsDedx[nTofHits]/I"); 

  mCellTuple->Branch("dedx",&mCellData.dedx,"dedx[nTofHits]/F"); 
  
  mCellTuple->Branch("nSigE",&mCellData.nSigE,"nSigE[nTofHits]/F");
  mCellTuple->Branch("nSigPi",&mCellData.nSigPi,"nSigPi[nTofHits]/F");
  mCellTuple->Branch("nSigK",&mCellData.nSigK,"nSigK[nTofHits]/F");
  mCellTuple->Branch("nSigP",&mCellData.nSigP,"nSigP[nTofHits]/F");

  mCellTuple->Branch("probE",&mCellData.probE,"probE[nTofHits]/F");
  mCellTuple->Branch("probPi",&mCellData.probPi,"probPi[nTofHits]/F");
  mCellTuple->Branch("probK",&mCellData.probK,"probK[nTofHits]/F");
  mCellTuple->Branch("probP",&mCellData.probP,"probP[nTofHits]/F");

  mCellTuple->Branch("length",&mCellData.length,"length[nTofHits]/F");
  mCellTuple->Branch("tof",&mCellData.tof,"tof[nTofHits]/F");
  mCellTuple->Branch("beta",&mCellData.beta,"beta[nTofHits]/F");
  
  return;
}


