
#ifndef HISTOBOOK_H
#define HISTOBOOK_H

#include "allroot.h"
#include <map>
#include <string>
#include <sstream>
#include <stdarg.h>
#include "xmlConfig.h"

using namespace std;

class legendAlignment {
public:
	static const int topLeft = 1;
	static const int topRight = 2;
	static const int topCenter = 3;
	static const int bottomLeft = 4;
	static const int bottomRight = 5;
	static const int bottomCenter = 6;

	static const int bottom = 0;
	static const int center = 1; // for x and y dirs
	static const int right = 2;
	static const int left = 3;
	static const int top = 4;

	// TODO: make a smart legend that trys to avoid data
	static const int best = 5;

	

};

class histoBook {

private:

	string currentDir;

	std::map<string, TH1*> book;
	
	string filename;
	
	TFile *file;

	/* Style and display */
	string styling;
	string drawOption;
	TLegend * legend;

	// optional config to use for all config related calls
	xmlConfig * config;



public:

	histoBook( string name, string input = "", string inDir = "" );
	histoBook( string name, xmlConfig* config, string input = "", string inDir = "" );
	~histoBook();

	
	string cd( string dir = "/" );
	void add( string name, TH1 * );
	TH1* get( string name, string sdir = "" );
	TH2* get2D( string name, string sdir = "" );
	void fill( string name, double bin, double weight = 1);
	void make1F( string name, string title, uint nBins, double low, double hi );
	void make1D( string name, string title, uint nBins, double low, double hi );
	void make1D( string name, string title, uint nBins, const Double_t* bins );
	void make2D( 	string name, string title, 
					uint nBinsX, double lowX, double hiX, uint nBinsY, double lowY, double hiY );
	void make2D( 	string name, string title, 
					uint nBinsX, const Double_t* xBins, uint nBinsY, double lowY, double hiY );
	void make( xmlConfig * config, string nodeName );
	void make( string nodeName );
	void makeAll( string nodeName );
	void makeAll( xmlConfig * config, string nodeName );

	TLegend* getLegend() { return legend; }

	histoBook* draw(string name = "", Option_t* opt= "" );
	

	histoBook* clearLegend() { legend->Clear(); return this; };
	
	// add a legend by setting the legendName
	

	TDirectory* getDirectory( ) { return gDirectory; }

	void save();

	histoBook* style( string hName );

	//set( string param, ... )
	// for function chaining and rapid styling
	histoBook* set( string param, string p1, string p2 = "", string p3 = "", string p4 = "" );
	histoBook* set( string param, double p1, double p2 = -1, double p3 = -1, double p4 = -1 );
	histoBook* set( xmlConfig* config, string nodePath );
	histoBook* set( string nodePath );
	histoBook* set( string opt, vector<string> nodePath );



	int color( string color ) {
		if ( "red" == color )
			return kRed;
		if ( "green" == color )
			return kGreen;
		if ( "blue" == color )
			return kBlue;
		if ( "black" == color )
			return kBlack;
		return -1;
	}

private:
	void globalStyle();
	histoBook* placeLegend( int alignmentX, int alignmentY, double width = -1, double height = -1 );
	void loadRootDir( TDirectory*, string path = "" );

	string sParam( vector<string> params, int p, string def="" ) {
		if ( p < params.size() )
			return params[ p ];
		else 
			return def;
	}
	char* cParam( vector<string> params, int p, string def="" ) {
		if ( p < params.size() )
			return (char*)(params[ p ].c_str());
		else 
			return (char*)(def.c_str());
	}
	double dParam( vector<string> params, int p, double def=0 ) {
		if ( p < params.size() )
			return atof( params[ p ].c_str() );
		else 
			return def;
	}

	



};



#endif