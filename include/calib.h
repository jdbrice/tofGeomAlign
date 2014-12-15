#ifndef CALIB_H
#define CALIB_H

#include "allroot.h"

#include "histoBook.h"
#include "constants.h"
#include "TOFrPicoDst.h"
#include <vector>

// clock_t, clock, CLOCKS_PER_SEC 
#include <time.h>       

// for testing if stdout is interactive or pipe / file
#include "unistd.h"
#include "xmlConfig.h"
#include "utils.h"
#include "reporter.h"

class calib{

private:

	// reporters
	reporter* reportX;
	reporter* reportY;

	// the main chain object
	TChain * _chain;

	// the histobook that stores all of our calibration histograms
	histoBook *book;

	// the pico dst for simpler chain usage
	TOFrPicoDst * pico;

	// use for timing
	clock_t startTime;

	// config file
	xmlConfig *config;

	int nSections;
	std::vector<double> moduleStart;

	// from the y fit
	std::vector<double> yPos, yError;
	
	// from the x fit
	std::vector<double> xPos, xError;
	
	// from the z fit
	std::vector<double> zPos[ constants::nTrays ], zError[ constants::nTrays ];
	std::vector<double> angle0zOff, angle0zError;
	std::vector<double> angle0FitOff, angle0FitError;

	double moduleSpaceMap[ constants::nSections ];

public:

	// Constructor
	calib( TChain * chain, xmlConfig *config );

	// destructor
	~calib();

	void localPosition();
	void fitY();
	void fitXAndZ();

	// finish the calibration by conducting fits etc.
	void savePage();

	// writes out a geom alignment file
	void writeAlignment();

protected:

	/*
	*	Utility functions that should be moved soon
	*/ 
	void startTimer( ) { startTime = clock(); }
	double elapsed( ) { return ( (clock() - startTime) / (double)CLOCKS_PER_SEC ); }


	static Double_t fitFunction( Double_t *x, Double_t *par );
	static Double_t fitFunctionZ( Double_t *x, Double_t *par );
	static Double_t fitAngle( Double_t *x, Double_t *par );
};



#endif