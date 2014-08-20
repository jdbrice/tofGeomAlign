#ifndef REPORTER_H
#define REPORTER_H

#include "allroot.h"

class reporter
{
public:
	reporter( string filename, int w = 791, int h = 1024 );
	~reporter();

	void newPage( int dx = 1, int dy = 1);
	void cd( int pad );
	void cd( int padX, int padY);
	void next();
	void savePage( string name = "" );
	void saveImage( string name = "" );

private:

	TCanvas* canvas;
	int currentPad;
	int dx, dy;

	string filename;
	static int instances;

};

#endif