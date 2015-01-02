/*******************************************************************
 *
 * $Id: StBTofQAMaker.h,v 1.3 2010/01/22 22:26:34 tlusty Exp $
 *
 * Author: David Tlusty
 *****************************************************************
 *
 * Description: Tof QA Maker to do the offline QA for VPD and TOF 
 *
 *****************************************************************
 *
 *
 *
 *******************************************************************/
#ifndef StBTofQAMaker_def
#define StBTofQAMaker_def

#include "StMaker.h"
#include "TString.h"
#include "TTree.h"
#include "TH3F.h"
#include "StMuDSTMaker/COMMON/StMuBTofPidTraits.h"
#include "/afs/rhic.bnl.gov/star/packages/DEV/StRoot/StarClassLibrary/StPhysicalHelixD.hh"

class StMuDstMaker;
class StMuDst;
class StMuEvent;
class TFile;
class TH2D;
class TH2F;
class TH1F;
class TH3F;
class TH1I;
class TH2I;
class TString;
class TTree;

class StBTofQAMaker : public StMaker 
	{
	private:
		
		TString outName; 
		StMuDstMaker* mMuDstMaker; // Make MuDst pointer available to member functions
		TFile *mHistFile;                 //  Histograms outputfile pointer                                  	
		UInt_t mEventsProcessed;                 //  Number of Events read and processed 
		//TString mHistogramOutputFileName;         //  Name of the histogram output file
		
		//  This is needed to make your maker known to root4star.
		//  It must be always the last statement in the class.
		//  Note that this is a macro, that's why the ';' is missing.
		//
		StMuDst*  mMuDst;
		StMuEvent* mMuEvent;
		TH2F *hdedxvp;		
		TH1F *hEventCounter;
		TH2F *hRefMult0;
		TH2F *hRefMult1;
		TH2F *hMass2;
		TH2F *hPID;
		TH2F *hVpdTimeRes;
		TH1F *hStartTimeResRun;

		TH2F *hBTofTimeResTrayWide;
		TH2F *hBTofTimeResTrayNarrow;
		TH2F *hBTofTimeResTray;
                TH2F *hBTofTimeResMult;
                TH2F *hYLocal;
                TH2F *hZLocal;
		TH2F *hBTofTimeResT0;                		
		TH2F *hBTofTimeResCell;
		TH1F *hZVertex;
		TH1F *hZVertexVPD;
		TH1F *hZVertexDelta;

                TH2F *hBTofTimeResTray30;
                TH2F *hBTofTimeResMult30;
                TH2F *hYLocal30;
                TH2F *hZLocal30;
                TH2F *hBTofTimeResT030;
                TH2F *hBTofTimeResCell30;
                TH2F *hMass230;
                TH2F *hPID30;
                TH2F *hdedxvp30;
                TH1F *hZVertex30;

                TH2F *hBTofTimeResTray70;
                TH2F *hBTofTimeResMult70;
                TH2F *hYLocal70;
                TH2F *hZLocal70;
                TH2F *hBTofTimeResT070;
                TH2F *hBTofTimeResCell70;
                TH2F *hMass270;
                TH2F *hPID70;
                TH2F *hdedxvp70;
                TH1F *hZVertex70;

	public:

		StBTofQAMaker(StMuDstMaker *maker,const Char_t *outname="");     // constructor
		virtual		~StBTofQAMaker();           // destructor
		Bool_t mHistogramSwitch;
		void setHistogramOn(const bool val=kTRUE);
		
		Int_t  Init();                      // called once at the beginning of your job
		Int_t  Make();                      // invoked for every event
		Int_t  Finish();                    // called once at the end

		//void SetOutputFileName(TString name) {mHistogramOutputFileName = name;} // Make name available to member functions

		ClassDef(StBTofQAMaker,1)
		
};

inline void StBTofQAMaker::setHistogramOn(const bool val) { mHistogramSwitch = val; }

#endif
