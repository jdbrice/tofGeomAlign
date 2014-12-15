
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
    gErrorIgnoreLevel=kSysError;

    xmlConfig config( argv[ 1 ] );
    config.report();
 
    

    string jobType = config.getString( "jobType" );

    /*   Load the files into the chain */
    TChain * chain = new TChain( "tof" );
    
    // create a calibration object
    calib tofCalib( chain, &config );
    

    if ( "histogram" == jobType ){

        if ( config.nodeExists( "input.dataDir" ) ){
            chainLoader::load( chain, (char*)config.getString( "input.dataDir" ).c_str(), config.getInt( "input.dataDir:maxFiles", 10000 ) );
        } else {
            cout << " No Data Directory given please specify input.dataDir node in your config file" << endl;
            return 0;
        }
        
        cout << endl << "Extracting position histograms" << endl << endl;
        tofCalib.localPosition();        

    } else if ( "calibrate" == jobType ){

        cout << endl << "Beginning Calibration" << endl << endl;
        tofCalib.fitY();
        tofCalib.fitXAndZ();
        tofCalib.writeAlignment();
    }

	return 0;
}
