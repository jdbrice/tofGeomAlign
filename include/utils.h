#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <time.h>   

using namespace std;


namespace jdbUtils{

	std::string ts( int );
	std::string ts( double );
	std::string ts( uint );
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