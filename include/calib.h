#ifndef CALIB_H
#define CALIB_H

#include "allroot.h"

#include "histoBook.h"
#include "constants.h"
#include "TOFrPicoDst.h"
#include "splineMaker.h"
#include <vector>

// clock_t, clock, CLOCKS_PER_SEC 
#include <time.h>       

// for testing if stdout is interactive or pipe / file
#include "unistd.h"
#include "xmlConfig.h"
#include "utils.h"


class calib{

private:

	// the canvas used to draw report hidtos
	TCanvas* can;

	// the main chain object
	TChain * _chain;

	// the histobook that stores all of our calibration histograms
	histoBook *book;

	// the pico dst for simpler chain usage
	TOFrPicoDst * pico;

	// use for timing
	clock_t startTime;

	// config file
	xmlConfig config;
	

public:

	// Constructor
	calib( TChain * chain, xmlConfig config );

	// destructor
	~calib();

	// finish the calibration by conducting fits etc.
	void savePage();

protected:

	/*
	*	Utility functions that should be moved soon
	*/ 
	void startTimer( ) { startTime = clock(); }
	double elapsed( ) { return ( (clock() - startTime) / (double)CLOCKS_PER_SEC ); }
};



#endif