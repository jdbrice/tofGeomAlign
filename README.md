root
====


## boiler
This project is the starting place for root-based applications. It contains:

####app
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

TH2D* tmp = new TH2D( 	"h2", "h2 title", 100, 1, 100, 400, -20, 20 );

// now add the histogram to the book so it is managed
book->add( "h2", tmp );


// now get a histogram from the book
sstr.str("");   	sstr << "h" << 1;
TH2D* tmp = (TH2D*) book->get( sstr.str() );


// now make a sub directory
sstr.str("");		sstr << "subdir" << 1 << "/fit";
book->cd( sstr.str() );

```

###chainLoader
class for loading ntuple files into a TChain. Tt takes a chain object, a directory in which to search, and a max number of files to load. Usage:

```c++
// static usage
chainLoader::load( chain, "dataDir", maxFiles );

``` 

###xmlConfig
A lightweight xml configuration utility. Usuful for storing project configurations and runtime-parameters. Usage:

Sample Config File:
```xml

<config>

	<jobType>calibrate</jobType>

	<rootOutput>test/test.root</rootOutput>
	
	<variableBinning>1</variableBinning>

	<dataDir>/star/institutions/rice/jdb/run14AuAu15TofCalibration/TofCalibration/geomAlign/t14Data0/output/</dataDir>
	
	<maxFiles>10</maxFiles>

	<numIterations>1</numIterations>

	<paramsOutput>test/NULL.dat</paramsOutput>

	<removeOffset>1.87987</removeOffset>

	<outlierRejection>1</outlierRejection>

	<numTOTBins>25</numTOTBins>

	<reportOutput>test/qa.pdf</reportOutput>

</config>

```

Usage in code:

```c++

	// read the config file from the first passed in argument
    xmlConfig config( argv[ 1 ] );

    // get a parameter returned as string type
    cout << "[CONFIG]" << setw(20) << left << " jobType is "        << config.getAsString( "jobType" ) << endl;

    // get a parameter returned as a string type
    cout << "[CONFIG]" << setw(20) << " rootOutput "      << config.getAsString( "rootOutput" ) << endl;

    // get a parameter returned as a bool type
    cout << "[CONFIG]" << setw(20) << " variableBinning " << config.getAsBool( "variableBinning" ) << endl;

    // get a parameter returned as an int type
    cout << "[CONFIG]" << setw(20) << " maxFiles "        << config.getAsInt( "maxFiles" ) << endl;

    
	// get a parameter returned as a double type
    cout << "[CONFIG]" << setw(20) << " removeOffset "        << config.getAsDouble( "removeOffset" ) << endl;

```



## ntupleCleaner
This is a tool for cleaning ntuples when a job has returned many good files mixed with many files that did not process and therefore do not contain the desired tree.

This tool scans all ntuple files in a directory and removes the ones that do not contain the given tree.

Usage:

ntupleCleaner treeName dataDirectory maxFilesToScan

Warning: Be careful using this, improper use could delete good data.