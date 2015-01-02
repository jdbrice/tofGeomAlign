 /*******************************************************************                                            
 *                                                                                                              
 * $Id: StBTofQAMaker.cxx,v 1.4 2010/01/22 00:30:45 tlusty Exp $                                              
 *                                                                                                              
 * Author: David Tlusty - tlusty@gmail.com                                                                                            
 *****************************************************************                                              
 *                                                                                                              
 * Description: BTof QA Maker to do the Basic offline QA for TOF 
 * To fill all branches it's neccessary to have already matched and calibrated data
 * Otherwise one must use StBTofMatchMaker, StVpdCalibMaker and StBTofCalibMaker in the macro before this maker
 *                                                                                                              
 *****************************************************************                                              
 *                                                                                                              
 * BTof is Run09 configuration: 86 active trays, 34 not installed yet
 * Missing trays(Id): 13,14,38,39,42,43,47,52,68,73,74,75,79-96,102,103,112,118
 * West Trays: 1,...,60
 * East Trays: 61,...,120
 *                                                                                                              
 *******************************************************************/
#include "StBTofQAMaker.h"
#include <iostream>
#include "StThreeVectorD.hh"
#include "StThreeVectorF.hh"
#include "StHelix.hh"
#include "StPhysicalHelixD.hh"
#include "StTrackGeometry.h"
#include "StDcaGeometry.h"
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"                                                                                              
#include "StMuDSTMaker/COMMON/StMuTrack.h"                                                                                                 
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuBTofHit.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuTriggerIdCollection.h"
//#include "StMuDSTMaker/COMMON/StMuBTofPidTraits.h"
#include "StEvent/StPrimaryVertex.h"
#include "StEvent/StBTofHeader.h"
#include "StEvent/StBTofRawHit.h"
#include "StEvent/StRunInfo.h"
#include "StEvent/StEnumerations.h"
#include "tables/St_vertexSeed_Table.h"
 
#include "StBTofUtil/tofPathLength.hh"
#include "phys_constants.h"

#include "TH2.h"                                                                                                                           
#include "TFile.h" 
#include "TTree.h"
#include "TBranch.h"
#include "TRandom1.h"

ClassImp(StBTofQAMaker)

StBTofQAMaker::StBTofQAMaker(StMuDstMaker * maker, const Char_t *outname) : StMaker("StBTofQAMaker"),mHistFile(0)
{
	outName = outname;
	mMuDstMaker      = maker ;                    // Pass MuDst pointer to DstAnlysisMaker Class member functions                            
//	histogram_output = NULL  ;                    // Zero the Pointer to histogram output file                                               
	mEventsProcessed = 0     ;                    // Zero the Number of Events processed by the maker                                        
	mMuDst = 0;
	mMuEvent = 0;
}
StBTofQAMaker::~StBTofQAMaker() {}

Int_t StBTofQAMaker::Init()
{
	if(mHistogramSwitch){

		// Event QA
		hEventCounter = new TH1F("eventCounter","event number",20,0.,20.);
		hRefMult0 = new TH2F("refMult0","ref mult all good",1000,0.,1000.,1000,0.,1000.);
        hRefMult1 = new TH2F("refMult1","ref mult TOF PID",1000,0.,1000.,1000,0.,1000.);

        // TOF PID
		hMass2 = new TH2F("MassSquare","Mass^{2} vs momentum",500,0.,5.,500,-0.5,4.5);
		hPID = new TH2F("PID","Basic PID",500,0.,5.,500,0.,5.);
		
		// TOF Calibration
		hBTofTimeResTrayNarrow = new TH2F("BTofTimeRes_vs_TrayNarrow","BTof time resolution vs Tray",120,1,121,3000,-.75, .75);
		hBTofTimeResTrayWide = new TH2F("BTofTimeRes_vs_TrayWide","BTof time resolution vs Tray",120,1,121,10000,-5000.,5000.);
		hBTofTimeResTray = new TH2F("BTofTimeRes_vs_Tray","BTof time resolution vs Tray",120,1,121,1000,-5.,5.);
		hBTofTimeResMult = new TH2F("BTofTimeRes_vs_Mult","BTof time resolution vs RefMult",1000,0.,1000.,100,-10.,10.);
		hYLocal = new TH2F("yLocal","yLocal vs Tray",120,1.,121.,200,-5.,5.);
        hZLocal = new TH2F("zLocal","zLocal vs Tray",120,1.,121.,200,-5.,5.);

        hStartTimeResRun = new TH1F("StartTimeRes","Vpd Start time resolution vs Run",200,-1.,1.);
		hVpdTimeRes = new TH2F("VpdTimeRes","Vpd Time resolution",38,0.5,38.5,200,-1.,1.);	
		
		hBTofTimeResT0 = new TH2F("BTofTimeRes_vs_tZeroes","BTof time resolution vs tZeroes",350,1,350,1000,-1.,1.);
        hBTofTimeResCell = new TH2F("BTofTimeRes_vs_Cell","BTof time resolution vs Cell",23050,0,23050,1000,-1.,1.);
		
		hdedxvp = new TH2F("dedxvp","dedx v p",1000,-5,5,2000,0,20);
		hZVertex = new TH1F("zvertex","Z Vertex", 200, -100,100);
		hZVertexVPD = new TH1F("zvertexVPD","VPD Z Vertex", 200, -100,100);
		hZVertexDelta = new TH1F("zvertexDelta","TPC - VPD Z Vertex", 2000, -100,100);

/*
		hMass230 = new TH2F("MassSquare30","Mass^{2} vs momentum",500,0.,5.,500,-0.5,4.5);
        hPID30 = new TH2F("PID30","Basic PID",500,0.,5.,500,0.,5.);
        hBTofTimeResTray30 = new TH2F("BTofTimeRes_vs_Tray30","BTof time resolution vs Tray",120,1,121,1000,-1.,1.);
        hBTofTimeResMult30 = new TH2F("BTofTimeRes_vs_Mult30","BTof time resolution vs RefMult",1000,0.,1000.,100,-1.,1.);
        hYLocal30 = new TH2F("yLocal30","yLocal vs Tray",120,1.,121.,200,-5.,5.);
        hZLocal30 = new TH2F("zLocal30","zLocal vs Tray",120,1.,121.,200,-5.,5.);
        hBTofTimeResT030 = new TH2F("BTofTimeRes_vs_tZeroes30","BTof time resolution vs tZeroes",350,1,350,1000,-1.,1.);
        hBTofTimeResCell30 = new TH2F("BTofTimeRes_vs_Cell30","BTof time resolution vs Cell",23050,0,23050,1000,-1.,1.);
        hdedxvp30 = new TH2F("dedxvp30","dedx v p",1000,-5,5,2000,0,20);
        hZVertex30 = new TH1F("zvertex30","Z Vertex", 5000, -30,30);

        hMass270 = new TH2F("MassSquare70","Mass^{2} vs momentum",500,0.,5.,500,-0.5,4.5);
        hPID70 = new TH2F("PID70","Basic PID",500,0.,5.,500,0.,5.);
        hBTofTimeResTray70 = new TH2F("BTofTimeRes_vs_Tray70","BTof time resolution vs Tray",120,1,121,900,-1.,1.);
        hBTofTimeResMult70 = new TH2F("BTofTimeRes_vs_Mult70","BTof time resolution vs RefMult",1000,0.,1000.,100,-1.,1.);
        hYLocal70 = new TH2F("yLocal70","yLocal vs Tray",120,1.,121.,200,-5.,5.);
        hZLocal70 = new TH2F("zLocal70","zLocal vs Tray",120,1.,121.,200,-5.,5.);
        hBTofTimeResT070 = new TH2F("BTofTimeRes_vs_tZeroes70","BTof time resolution vs tZeroes",350,1,350,1000,-1.,1.);
        hBTofTimeResCell70 = new TH2F("BTofTimeRes_vs_Cell70","BTof time resolution vs Cell",23050,0,23050,1000,-1.,1.);
        hdedxvp70 = new TH2F("dedxvp70","dedx v p",1000,-5,5,2000,0,20);
        hZVertex70 = new TH1F("zvertex70","Z Vertex", 5000, -70,70);
*/




	}
	return kStOK;
}
Int_t StBTofQAMaker::Make()
{
	hEventCounter->Fill(0);
	mMuDst = mMuDstMaker->muDst(); 
	if(!mMuDst) {
		cout<<"No StMuDst object in this event"<<endl;
		return kStOK;															         
	}
	hEventCounter->Fill(1);

	StMuEvent *event = mMuDst->event();
	if(!event) {
		cout<<"No StMuEvent" << endl;
		return kStOK;
	}
	hEventCounter->Fill(2);

	StTriggerId tId = event->triggerIdCollection().nominal();
	if(!tId.isTrigger(290001)) {
		//cout<<" Not a minbias trigger" << endl;
//		return kStOK;
	}
	hEventCounter->Fill(3);

	StThreeVectorF vertexPos = event->primaryVertexPosition();
	if(vertexPos.perp()>2.) {
		cout<<" Background event!" << endl;
		return kStOK;
	}
	hEventCounter->Fill(4);
	hRefMult0->Fill(event->refMult(),event->btofTrayMultiplicity());
	
// B) ******************************************* TPC PART *******************************************	
// main for-cycle runs over TPC Global tracks with TOF matching (condition with matchFlag() - 0 means no matching) 
//		/**/ means members of a vector 
	Bool_t validPID = kFALSE;
	if(mHistogramSwitch){
		StBTofHeader *bheader = mMuDst->btofHeader();

		cout << "#Primary = " << (int)mMuDst->numberOfPrimaryTracks() << endl;

		for (int i = 0; i<(int)mMuDst->numberOfPrimaryTracks(); i++){
			StMuTrack *mTrack = mMuDst->primaryTracks(i);

			if(!mTrack) {cout << "cont" << endl; continue;}		

			StMuBTofPidTraits mTofTrait = mTrack->btofPidTraits(); 
			
			// Matched to TOF hit
			if(mTofTrait.matchFlag()==0) continue;
			
			StMuBTofHit *mBTofHitIndexed = (StMuBTofHit *)mTrack->tofHit();
            if(!mBTofHitIndexed) continue;

            if( vertexPos.z() < -70.0 || vertexPos.z() > 70.0) continue;


			int tray = mBTofHitIndexed->tray();
			int module=mBTofHitIndexed->module();
			int cell = (tray-1)*192+(module-1)*6+(mBTofHitIndexed->cell()-1);

			// Geometry
			hYLocal->Fill(tray, mTofTrait.yLocal());
            hZLocal->Fill(tray, mTofTrait.zLocal());
			
			// ZVertex
			hZVertex->Fill(vertexPos.z());
			hZVertexVPD->Fill( bheader->vpdVz() );
			hZVertexDelta->Fill( vertexPos.z() - bheader->vpdVz() );

			StThreeVectorF mP = mTrack->p();
			double M,L,TOF_expected,TOF_meas; 
			double p = mP.magnitude();
			double dedxx= mTrack->dEdx()*1.e6;
			double etaa = mTrack->eta();
			

			//cout << "PL" << mTofTrait.pathLength()  <<endl;
			if(mTofTrait.pathLength() > 0) {
				
				TOF_expected = -9.; // matching with good track, but not pion, kaon, proton or electron
				if (fabs(mTrack->nSigmaPion())<=2. && p>=0.2 && p<=0.6) {
					
					M = M_PION_PLUS;
					L = mTofTrait.pathLength();
					double c = 29.9792458; // cm/ns
					TOF_meas = mTofTrait.timeOfFlight(); // [ns]
					TOF_expected = TMath::Sqrt( L*L / (c*c) * ( 1 + M*M / (p*p) ) ); // in nanoseconds				
					

					hBTofTimeResTrayNarrow->Fill(tray, TOF_meas-TOF_expected);
					hBTofTimeResTrayWide->Fill(tray, TOF_meas-TOF_expected);
					hBTofTimeResTray->Fill(tray, TOF_meas-TOF_expected);
					hBTofTimeResMult->Fill(event->refMult(), TOF_meas-TOF_expected);
					hBTofTimeResT0->Fill(bheader->nTzero(),TOF_meas-TOF_expected);
					hBTofTimeResCell->Fill(cell, TOF_meas-TOF_expected);
			
				}
			}
			float beta = mTofTrait.beta();
			float m2 = p*p*(1./beta/beta - 1.);
			if(beta>0) {
				hPID->Fill(p, 1./beta);
				hMass2->Fill(p, m2);
				validPID = kTRUE;
			}
			if (etaa<0.) p = (-1.*p);

			hdedxvp->Fill(p,dedxx);

/*
			if(vertexPos.z()<30.&&vertexPos.z()>-30.){
                        hYLocal30->Fill(tray, mTofTrait.yLocal());
                        hZLocal30->Fill(tray, mTofTrait.zLocal());
                        hZVertex30->Fill(vertexPos.z());
                        StThreeVectorF mP = mTrack->p();
                        float M,L,TOF_expected,TOF_meas;
                        float p = mP.magnitude();
                        float dedxx= mTrack->dEdx()*1.e6;
//                        cout<<"p,dedx: "<<p<<", "<<dedxx<<endl;
                        float etaa = mTrack->eta();
                        if(mTofTrait.pathLength() > 0) {
                                TOF_expected = -9.; // matching with good track, but not pion, kaon, proton or electron
                                if (fabs(mTrack->nSigmaPion())<2. && p>0.3 && p<0.6) {
                                        M = 0.13957; // GeV/c^2
                                        L = mTofTrait.pathLength();
                                        float c = 29.979; // cm/ns
                                        TOF_meas = mTofTrait.timeOfFlight(); // [ns]
                                        TOF_expected = TMath::Sqrt(L*L/(c*c)*(1+M*M/(p*p))); // in nanoseconds
                                        hBTofTimeResTray30->Fill(tray, TOF_meas-TOF_expected);
                                        hBTofTimeResMult30->Fill(event->refMult(), TOF_meas-TOF_expected);
                                        hBTofTimeResT030->Fill(bheader->nTzero(),TOF_meas-TOF_expected);
                                        hBTofTimeResCell30->Fill(cell, TOF_meas-TOF_expected);

                                }
                        }
                        float beta = mTofTrait.beta();
                        float m2 = p*p*(1./beta/beta - 1.);
                        if(beta>0) {
                                hPID30->Fill(p, 1./beta);
                                hMass230->Fill(p, m2);
                                validPID = kTRUE;
                        }
                        if (etaa<0.) p = (-1.*p);

                        hdedxvp30->Fill(p,dedxx);



			}

                        if(vertexPos.z()<70.&&vertexPos.z()>-70.){
                        hYLocal70->Fill(tray, mTofTrait.yLocal());
                        hZLocal70->Fill(tray, mTofTrait.zLocal());
                        hZVertex70->Fill(vertexPos.z());
                        StThreeVectorF mP = mTrack->p();
                        float M,L,TOF_expected,TOF_meas;
                        float p = mP.magnitude();
                        float dedxx= mTrack->dEdx()*1.e6;
//                        cout<<"p,dedx: "<<p<<", "<<dedxx<<endl;
                        float etaa = mTrack->eta();
                        if(mTofTrait.pathLength() > 0) {
                                TOF_expected = -9.; // matching with good track, but not pion, kaon, proton or electron
                                if (fabs(mTrack->nSigmaPion())<2. && p>0.3 && p<0.6) {
                                        M = 0.13957; // GeV/c^2
                                        L = mTofTrait.pathLength();
                                        float c = 29.979; // cm/ns
                                        TOF_meas = mTofTrait.timeOfFlight(); // [ns]
                                        TOF_expected = TMath::Sqrt(L*L/(c*c)*(1+M*M/(p*p))); // in nanoseconds
                                        hBTofTimeResTray70->Fill(tray, TOF_meas-TOF_expected);
                                        hBTofTimeResMult70->Fill(event->refMult(), TOF_meas-TOF_expected);
                                        hBTofTimeResT070->Fill(bheader->nTzero(),TOF_meas-TOF_expected);
                                        hBTofTimeResCell70->Fill(cell, TOF_meas-TOF_expected);

                                }
                        }
                        float beta = mTofTrait.beta();
                        float m2 = p*p*(1./beta/beta - 1.);
                        if(beta>0) {
                                hPID70->Fill(p, 1./beta);
                                hMass270->Fill(p, m2);
                                validPID = kTRUE;
                        }
                        if (etaa<0.) p = (-1.*p);

                        hdedxvp70->Fill(p,dedxx);



                        }*/
			


		}
	}
	if(validPID) {
		hEventCounter->Fill(5);
		hRefMult1->Fill(event->refMult(),event->btofTrayMultiplicity());
	} else {
		cout << " ***********No TOF PID for this good event!*********************** " << endl;
		cout << " Event btofTrayMulti = " << event->btofTrayMultiplicity() << " refMult = " << event->refMult() << endl;
	}
// C) *************************************** VPD AND BTOF PART **************************************	
// !!! Covention: West parts of detectors have always lower Id's than East parts 
// TPC-TOF matching variables and branches are in B) part - you can match both primary and global tracks
	if(mHistogramSwitch){
		// ******************************** Hit area ************************************
		StBTofHeader *mBTofHeader = mMuDst->btofHeader();
		if(mBTofHeader){
			hEventCounter->Fill(6);
			Int_t nWest = mBTofHeader->numberOfVpdHits(west);
			Int_t nEast = mBTofHeader->numberOfVpdHits(east);
			Double_t TSumWest = 0.;
			Double_t TSumEast = 0.;
			Double_t timeWest[19], timeEast[19];
			for(int i=0;i<19;i++) {
				timeWest[i] = mBTofHeader->vpdTime(west, i+1);
				TSumWest += timeWest[i];
				timeEast[i] = mBTofHeader->vpdTime(east, i+1);
				TSumEast += timeEast[i];
			}
			if(nWest>1) {
				for(int i=0;i<19;i++) {
					if(timeWest[i]<1.e-4) continue;
					double tdiff = timeWest[i] - (TSumWest-timeWest[i])/(nWest-1);
					hVpdTimeRes->Fill(i+1, tdiff);
				}
			}
			if(nEast>1) {
				for(int i=0;i<19;i++) {
					if(timeEast[i]<1.e-4) continue;
					double tdiff = timeEast[i] - (TSumEast-timeEast[i])/(nEast-1);
					hVpdTimeRes->Fill(i+1+19, tdiff);
				}
			}
			if(nWest>=1&&nEast>=1) hEventCounter->Fill(7);
			Double_t TStart = mBTofHeader->tStart();
			if(TStart>0.) hEventCounter->Fill(8);
		}
	}
	return kStOK;
}

Int_t StBTofQAMaker::Finish()
{
        mHistFile = new TFile(outName.Data(),"RECREATE");
	hEventCounter->Write();
	hRefMult0->Write();
	hRefMult1->Write();
	hMass2->Write();
	hPID->Write();
	hVpdTimeRes->Write();
	hStartTimeResRun->Write();
	hBTofTimeResTray->Write();
	hBTofTimeResTrayWide->Write();
	hBTofTimeResTrayNarrow->Write();
    hBTofTimeResMult->Write();
	hBTofTimeResT0->Write();
    hBTofTimeResCell->Write();
	hYLocal->Write();
	hZLocal->Write();
	hdedxvp->Write();
	hZVertex->Write();
	hZVertexVPD->Write();
	hZVertexDelta->Write();
/*
        hBTofTimeResTray30->Write();
        hBTofTimeResMult30->Write();
        hBTofTimeResT030->Write();
        hBTofTimeResCell30->Write();
        hYLocal30->Write();
        hZLocal30->Write();
        hdedxvp30->Write();
        hMass230->Write();
        hPID30->Write();
        hYLocal30->Write();
        hZLocal30->Write();
        hdedxvp30->Write();
       hZVertex30->Write();

        hBTofTimeResTray70->Write();
        hBTofTimeResMult70->Write();
        hBTofTimeResT070->Write();
        hBTofTimeResCell70->Write();
        hYLocal70->Write();
        hZLocal70->Write();
        hdedxvp70->Write();
        hMass270->Write();
        hPID70->Write();
        hdedxvp70->Write();
	hZVertex70->Write();
*/
	mHistFile->Close();


	
	cout << "Total Events Processed in DstMaker " << mEventsProcessed << endl;
	return kStOK;
}




