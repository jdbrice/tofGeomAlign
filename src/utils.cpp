
#include "utils.h"
#include "allroot.h"



namespace jdbUtils{

	std::string ts( int i, int len ){
		if ( len <= -1 )
			return (to_string( (long long int) i));
		else
			return (to_string( (long long int) i)).substr( 0, len );
	}
	std::string ts( double d, int len ){
		if ( len <= -1 )
			return to_string( (long double) d);
		else 
			return (to_string( (long double) d)).substr( 0, len );
	}
	std::string ts( uint u, int len ){
		if ( len <= -1 )
			return to_string( (long long unsigned int) u);
		else 
			return (to_string( (long long unsigned int) u)).substr( 0, len );
	}

	void progressBar( int i, int nevents, int max ){
		
		double progress =  ((double)i / (double)nevents);
		if ( i == nevents - 1)
    		progress = 1.001;


    	int res = (int)((double)nevents / (double) max);
    	if ( res == 0 || res == 1)
    		res = i;

    	// decide when to update
    	if ( i == 0 || (i % res ) == 0 || i == nevents - 1  ){
			// skip for non interactive output
			if (!isatty(fileno(stdout)) && progress <= 1 )
				return;

			double per = progress  * 100;
			per = TMath::Nint( per );

			cout << "[";
	    	for ( int ip = 0; ip < max; ip ++ ){
	    		if ( ip < TMath::Nint( (progress * (double)max) ) )
	    			cout << "=";
	    		else 
	    			cout << " ";
	    	}
	    	if (isatty(fileno(stdout)) ){ 
		 	   	cout << "]" << per << "%" << "\r";
				std::cout.flush();
				if (progress > 1)
					cout << "[" << endl;
			} else {
					cout << "]" << per << "%" << "\n";
			}
		}
		
	}

}