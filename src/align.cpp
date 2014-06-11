
#include <iostream>
#include "allroot.h"
#include "constants.h"
#include "histoBook.h"
#include "chainLoader.h"
#include "calib.h"
#include "utils.h"



int main( int argc, char* argv[] ) {

    cout << endl << endl << endl << " TOF to TPC Geometry Calibration" << endl;
    
    if ( argc < 2 ){
    	
    	cout << "Call executable with \n "
    			"1) xml config filename"
                << endl;

       	return 0;

    } 

    /* Give a summary of config file */
    xmlConfig config( argv[ 1 ] );

    config.display( "jobType" );
    config.display( "baseName" );
    config.display( "rootOutput" );
    config.display( "reportOutput" );
    cout << endl;
    config.display( "minHitsFit" );
    config.display( "ptCut" );
    config.display( "vzCut" );
    cout << endl;
    config.display( "dataDir" );
    config.display( "maxFiles" );
    cout << endl;
    
    /* Give a summary of config file */

    cout << endl << endl << "Beginning Calibration" << endl << endl;

    string jobType = config.getAsString( "jobType" );

    /*   Load the files into the chain */
    TChain * chain = new TChain( "tof" );
    
    if ( config.nodeExists( "dataDir" ) ){
        chainLoader::load( chain, (char*)config.getAsString( "dataDir" ).c_str(), config.getAsInt( "maxFiles", 10000 ) );
    } else {
        cout << " No Data Directory given please specify <dataDir></dataDir> node in your config file" << endl;
        return 0;
    }
    

    // create a calibration object
    calib tofCalib( chain, config );
 
    if ( jobType == (string)"calibrate" ){

        tofCalib.localPosition();

        tofCalib.fitY();

        tofCalib.fitXAndZ();
        
    }

	return 0;
}
