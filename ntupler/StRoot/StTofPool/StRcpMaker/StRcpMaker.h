#ifndef StRcpMaker_hh     
#define StRcpMaker_hh


#include "StMaker.h"
#include "StThreeVectorD.hh"
#include "StPhysicalHelixD.hh"
#include "StRcpData.h"

#define __SIGMA_SCALE__ 1000.

#include <string>
#include <vector>

#ifndef ST_NO_NAMESPACES
  using std::string;
  using std::vector;
#endif

class StEvent;
class StTrack;
class StGlobalTrack;
class StHelix;
#include "StThreeVectorF.hh"
class StTrackGeometry;
class StBTofGeometry; 
class StBTofCollection;
class StBTofHitCollection;   
class StSPtrVecBTofHit;      
class StMuDst;

class TFile;
class TTree;

class StRcpMaker : public StMaker {
 public:
  StRcpMaker(const Char_t *name, const Char_t *outname);
  ~StRcpMaker();

  Int_t  Init();
  Int_t  InitRun(int runnumber);
  Int_t  Make();
  Int_t  FinishRun(int runnumber);
  Int_t  Finish();

  void SetNtupleFileName(Char_t*);
  void setOuterTrackGeometry();
  void setStandardTrackGeometry();
  void setInitGeomFromOther(const Bool_t);

  /// switch to read in StEvent/MuDst
  void setMuDstIn(Bool_t muDstIn=kTRUE);

private:

  ///
  void processMuDst();
            
  void bookNtuples();

private:

  StPhysicalHelix* mBeamHelix;

  StMuDst*          muDst;
  Bool_t            mMuDstIn;   //! switch - default is to read in StEvent
        
  StBTofGeometry *mBTofGeom;    //! tofr geometry
  Bool_t mInitGeomFromOther;    //! geometry initial from other makers
  Bool_t mOuterTrackGeometry;   //! select outer track geometry (true)

  // ntuple related data members
  Int_t mAcceptedEvents;        //! number of accepted events
  Int_t mPvpdEntries;           //! number of pVPD ntuple entries
  Int_t mBTofEvents;            //! number of BTof events
  Int_t mBTofEntries;           //! number of BTof ntuple entries

  string mTupleFileName;        //!
  TFile *mTupleFile;            //!
  
  StRcpData  mCellData;

  Bool_t  doPrintMemoryInfo;    //! control debug memory data
  Bool_t  doPrintCpuInfo;       //! control debug timing data

  TTree *mCellTuple;            //! BTof calibration ntuple

  virtual const char *GetCVS() const 
    {static const char cvs[]="Tag $Name:  $ $Id: StRcpMaker.h,v 1.4 2008/05/06 18:42:09 dongx Exp $ built "__DATE__" "__TIME__ ; return cvs;}

  ClassDef(StRcpMaker,1)
};

inline void StRcpMaker::setOuterTrackGeometry(){mOuterTrackGeometry=false;}
inline void StRcpMaker::setStandardTrackGeometry(){mOuterTrackGeometry=false;}
inline void StRcpMaker::SetNtupleFileName(Char_t* filename){mTupleFileName=filename;}
inline void StRcpMaker::setInitGeomFromOther(const Bool_t init) { mInitGeomFromOther = init; }
inline void StRcpMaker::setMuDstIn(Bool_t val) { mMuDstIn = val; }

#endif
