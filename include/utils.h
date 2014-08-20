#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <time.h>  
#include <unistd.h> 

using namespace std;


namespace jdbUtils{

	std::string ts( int, int len = -1 );
	std::string ts( double, int len = -1 );
	std::string ts( unsigned int, int len = -1 );
	void progressBar( int i, int nevents, int max );

	class timer
	{
	public:
		//timer();
		//~timer();
		clock_t startTime;

		void startTimer( ) { startTime = clock(); }
		double elapsed( ) { return ( (clock() - startTime) / (double)CLOCKS_PER_SEC ); }
		
	};
}




#endif