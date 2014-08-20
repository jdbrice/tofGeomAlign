
#include "constants.h"
#include "analyzer.h"
#include "histoBook.h"

// provides my own string shortcuts etc.
using namespace jdbUtils;


/**
 * Constructor - Initializes all of the pidHistogram parameters from the configuration file
 * @param chain       The chain object containing all data compatible with the TOFrPicoDST format
 * @param con         The xml configuration defining key aspects of the calibration
 *					such as number of tot bins to use, data location etc. See repo Readme
 *					for a sample configuration.
 */
analyzer::analyzer( TChain* chain, xmlConfig* con )  {
	cout << "[analyzer.analyzer] " << endl;
	
	gErrorIgnoreLevel=kError;

	config = con;

	// set the histogram info verbosity to show nothing
	gStyle->SetOptStat( 0 );
	
	// create the histogram book
	book = new histoBook( ( config->getString( "output.base" ) + config->getString( "output.root" ) ), config );
	
	// create a report builder 
	report = new reporter( config->getString( "output.base" ) + config->getString( "output.report" ) );

	_chain = chain;
}

/**
 *	Destructor - Deletes the histoBook ensuring it is saved.
 */
analyzer::~analyzer() {
	
	delete book;
	delete report;
	
	cout << "[analyzer.~analyzer] " << endl;
}



void analyzer::loopEvents() {

	jdbUtils::timer t;
	t.startTimer();

	if ( !_chain ){
		cout << "[analyzer." << __FUNCTION__ << "] ERROR: Invalid chain " << endl;
		return;
	}

	Int_t nEvents = (Int_t)_chain->GetEntries();
	cout << "[analyzer." << __FUNCTION__ << "] Loaded: " << nEvents << " events " << endl;

	book->cd( "" );

	// make some histos here
	
	// loop over all events
	for(Int_t i=0; i<nEvents; i++) {
    	_chain->GetEntry(i);

    	
    	
	} // end loop on events

	


	cout << "[analyzer." << __FUNCTION__ << "] completed in " << t.elapsed() << " seconds " << endl;
}
