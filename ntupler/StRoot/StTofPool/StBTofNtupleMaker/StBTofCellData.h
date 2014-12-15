/*******************************************************************
 *
 * $Id: StTofrCellData.h,v 1.5 2008/05/08 21:09:36 dongx Exp $
 *
 * Author: Xin Dong
 *****************************************************************
 *
 * Description: variables list for filling TOFr TTree
 *
 *****************************************************************
 *
 * $Log: StTofrCellData.h,v $
 * Revision 1.5  2008/05/08 21:09:36  dongx
 * Changed precision of time info to double type
 *
 * Revision 1.4  2008/05/06 18:42:09  dongx
 * Updated for Run8 analysis
 *
 * Revision 1.2  2004/04/12 16:17:03  dongx
 * add AdcLoRes in the ntuple
 *
 * Revision 1.1  2004/03/11 22:39:54  dongx
 * first release
 *
 *
 *******************************************************************/
#ifndef STBTOFCELLDATA_H
#define STBTOFCELLDATA_H
  const Int_t kMaxHits = 100000;
  const Int_t kNumVpdTriggerTubes = 16;
  const Int_t kNumVpdTofTubes = 19;

  struct StBTofCellData {
    Int_t  run, evt;
    Float_t vertexX, vertexY, vertexZ;

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

    Float_t     pt[kMaxHits], 
                eta[kMaxHits], 
                phi[kMaxHits];

    Float_t     dcaX[kMaxHits], 
                dcaY[kMaxHits], 
                dcaZ[kMaxHits];//point closest approach to beam line

    Int_t       nHits[kMaxHits], 
                nHitsFit[kMaxHits];

    Float_t     dedx[kMaxHits];
    Int_t       nHitsDedx[kMaxHits];

    Float_t     nSigE[kMaxHits], 
                nSigPi[kMaxHits], 
                nSigK[kMaxHits], 
                nSigP[kMaxHits];

    Float_t     tofCorr[kMaxHits], 
                beta[kMaxHits], 
                length[kMaxHits];
  };  
#endif
