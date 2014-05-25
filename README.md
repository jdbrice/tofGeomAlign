root
====


### boiler
This project is the starting place for root-based applications. It contains:

####app:
	The main application entry point

####histoBook:
	A wrapper for working with root histograms. Allows easy use  of sub directories, auto-saving, etc. Usage:

```c++
histoBook * book  = new histoBook( "outputFilename" ); // .root will be added automatically if it is left off

// make a sub directory call subdir1
book->cd( "subdir1" );

// now inside subdir1 make a TH2D just like constructor
book->make2D( "h1", "h1 title", 100, 1, 100, 200, -100, 100 );


// or make the histogram and add it to the book

TH2D* tmp = new TH2D( 	"h2", "h2 title", 								100, 1, 100, 400, -20, 20 );

// now add the histogram to the book so it is managed
book->add( "h2", tmp );


// now get a histogram from the book
sstr.str("");   	sstr << "h" << 1;
TH2D* tmp = (TH2D*) book->get( sstr.str() );


// now make a sub directory
sstr.str("");		sstr << "subdir" << 1 << "/fit";
book->cd( sstr.str() );

```