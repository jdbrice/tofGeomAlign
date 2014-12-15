#ifndef STBTOFCELLDATA_H
#define STBTOFCELLDATA_H
  const Int_t kMaxHits = 100000;
  const Int_t kMaxTriggers = 400;
  const Int_t kNumVpdTriggerTubes = 16;
  const Int_t kNumVpdTofTubes = 19;

  struct StRcpData {

    // from StMuEvent
    Int_t       run, 
                evt;
    Int_t       nTriggers;
    UInt_t      triggerIds[ kMaxTriggers ];
    //vector<unsigned int> triggerIds;

    UShort_t    refMult, 
                refMultPos, 
                refMultNeg;

    Float_t     vertexX, 
                vertexY, 
                vertexZ;

    Int_t       vpdEast, 
                vpdWest;

    Int_t       numberOfVpdEast, 
                numberOfVpdWest;

    Int_t       nTZero;

    Double_t    tStartError;

    Double_t    tStart;

    Float_t     tDiff;
    Float_t     vpdVz;

    Double_t    vpdLeEast[kNumVpdTofTubes], 
                vpdTotEast[kNumVpdTofTubes];

    Double_t    vpdLeWest[kNumVpdTofTubes], 
                vpdTotWest[kNumVpdTofTubes];

    // VPD trigger side bbq electronics
    UShort_t    vpdBbqAdcEast[kNumVpdTriggerTubes], 
                vpdBbqTdcEast[kNumVpdTriggerTubes];
    UShort_t    vpdBbqAdcWest[kNumVpdTriggerTubes], 
                vpdBbqTdcWest[kNumVpdTriggerTubes];

    // VPD trigger side mxq electronics
    UShort_t    vpdMxqAdcEast[kNumVpdTriggerTubes], 
                vpdMxqTdcEast[kNumVpdTriggerTubes];
    UShort_t    vpdMxqAdcWest[kNumVpdTriggerTubes], 
                vpdMxqTdcWest[kNumVpdTriggerTubes];
    
    // tof detector 
    Int_t       nTofHits;
    Int_t       tray[kMaxHits];
    Int_t       module[kMaxHits];
    Int_t       cell[kMaxHits];
    Double_t    leTime[kMaxHits];
    Double_t    tot[kMaxHits];
    Int_t       matchFlag[kMaxHits];
    Float_t     yLocal[kMaxHits], 
                zLocal[kMaxHits], 
                thetaLocal[kMaxHits];

    Float_t     xGlobal[kMaxHits], 
                yGlobal[kMaxHits], 
                zGlobal[kMaxHits];

    Int_t       trackId[kMaxHits], 
                charge[kMaxHits];


    // from primary track
    Float_t     pt[kMaxHits], 
                eta[kMaxHits], 
                phi[kMaxHits];

    Float_t     pX[kMaxHits], 
                pY[kMaxHits], 
                pZ[kMaxHits],
                p[kMaxHits];

    Float_t     dcaX[kMaxHits], 
                dcaY[kMaxHits], 
                dcaZ[kMaxHits];

    Int_t       nHits[kMaxHits], 
                nHitsFit[kMaxHits],
                nHitsPossible[kMaxHits];

    Float_t     dedx[kMaxHits];
    Int_t       nHitsDedx[kMaxHits];

    Float_t     nSigE[kMaxHits], 
                nSigPi[kMaxHits], 
                nSigK[kMaxHits], 
                nSigP[kMaxHits];

    Float_t     probPi[kMaxHits], 
                probE[kMaxHits], 
                probP[kMaxHits], 
                probK[kMaxHits];

    Float_t     tof[kMaxHits], 
                beta[kMaxHits], 
                length[kMaxHits];
  };  
#endif
