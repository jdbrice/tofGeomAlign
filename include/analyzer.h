#ifndef analyzer_H
#define analyzer_H

#include "allroot.h"
#include "histoBook.h"
#include "constants.h"
#include "xmlConfig.h"
#include "utils.h"
#include "reporter.h"


class analyzer{

protected:

	// the canvas used to draw report hidtos
	reporter* report;

	// the main chain object
	TChain * _chain;

	// the histobook that stores all of our analyzer histograms
	histoBook *book;

	// config file
	xmlConfig* config;

public:


	// Constructor
	analyzer( TChain * chain, xmlConfig *config );

	// destructor
	~analyzer();

	void loopEvents();

};



#endif