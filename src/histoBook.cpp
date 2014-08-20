

#include "histoBook.h"
#include "TKey.h"
#include "TObject.h"

/**
 * Creates a histobook and allows the root filename to be set. optionally read from an existing root 
 * file and include everything into the working space
 * @param name  root filename
 * @param input input filename
 * @param inDir input starting directory
 */
histoBook::histoBook( string name, string input, string inDir ){

	if (name.find(  ".root") != std::string::npos){
		filename = name;	
	} else
		filename = name + ".root";

	currentDir = "/";

	file = new TFile( filename.c_str(), "recreate" );
	file->cd();

	
	// make the legend and draw it once to apply styles etc. 
	// for some reason needed to make styling work on the first draw
	legend = new TLegend( 0.65, 0.65, 0.9, 0.9);
	legend->SetFillColor( kWhite );
	legend->Draw();
	legend->Clear();

	globalStyle();

	// if an input was given merge it into the live record
	if ( input.length() >= 5 ){
		TFile * fin = new TFile( input.c_str() );
		cd ( inDir );
		fin->cd( inDir.c_str() );
		loadRootDir( gDirectory, inDir );
	}

}

/**
 * Constructor that allows th input of a config file
 * @param name name of file to use for saving root data
 * @param con  The config file to use for all config relates calls
 */
histoBook::histoBook( string name, xmlConfig * con, string input, string inDir ){

	if (name.find(  ".root") != std::string::npos){
		filename = name;	
	} else
		filename = name + ".root";

	currentDir = "/";

	file = new TFile( filename.c_str(), "recreate" );
	file->cd();

	// make the legend and draw it once to apply styles etc. 
	// for some reason needed to make styling work on the first draw
	legend = new TLegend( 0.65, 0.65, 0.9, 0.9);
	legend->SetFillColor( kWhite );
	legend->Draw();
	legend->Clear();

	globalStyle();

	config = con;

	// if an input was given merge it into the live record
	if ( input.length() >= 5 ){
		TFile * fin = new TFile( input.c_str() );
		cd ( inDir );
		fin->cd( inDir.c_str() );
		loadRootDir( gDirectory, inDir );
	}
}

// destructor
histoBook::~histoBook(){

	delete legend;

	save();
	file->Close();
}
void histoBook::save() {

	file->Write();
}

void histoBook::loadRootDir( TDirectory* tDir, string path ){

	//cout << "histoBook.loadRootDir] Path : " << path << endl;

	TList* list;

	if ( tDir ){
		list = tDir->GetListOfKeys();  
	} else {
		cout << "[histoBook.loadRootDir] Bad Directory Given " << endl;
		return;
	}

	TIter next(list);  
	TKey* key;  
	TObject* obj;   
	
	while ( (key = (TKey*)next()) ) {    
		
		obj = key->ReadObj() ;    
		
		if ( 0 == strcmp(obj->IsA()->GetName(),"TDirectoryFile") ){
			TDirectoryFile* dir = (TDirectoryFile*)obj;
			
			string nPath = path + dir->GetName();
			if ( path == (string) "" )
				nPath = path + dir->GetName();
			else 
				nPath = path + "/" + dir->GetName();

			cd( nPath );
			loadRootDir( dir, nPath );
		} else if ( obj ){
			if (    (strcmp(obj->IsA()->GetName(),"TProfile")!=0) && (!obj->InheritsFrom("TH2") && (!obj->InheritsFrom("TH1"))) ) {      
				// not a 1d or 2d histogram
			} else {
				// add it to the book
				//cout << "Adding : " << obj->GetName() << endl;
				add( obj->GetName(), (TH1*)obj->Clone( obj->GetName() ) );
			}    
			
		}
	}	

}


void histoBook::add( string name, TH1* h ){

	string oName = name;
	if ( name.length() <= 1 || !h )
		return;

	name = currentDir + name;
	
	// dont allow duplicated name overites
	if ( book[ name ] ){
		cout << "[histoBook.add] Duplicate histogram name in this directory " << currentDir << " / " << oName << endl;
		return;
	}

	// save the histo to the map
	book[ name ] = h;

}
/*
*
* TODO:: add support for full subdirectory trees
*/
string histoBook::cd( string sdir  ){

	string old = currentDir;

	char* csdir = (char*)sdir.c_str();
	file->cd();

	if ( file->GetDirectory( csdir ) ){
		file->cd( csdir );
	} else {
		cout << "[histoBook.cd] creating directory " << sdir << endl;
		file->mkdir( csdir );
		file->cd( csdir );
	}

	currentDir = sdir;

	return old;
}

void histoBook::make( string nodeName ){
	if ( config )
		make( config, nodeName );
}
void histoBook::make( xmlConfig * config, string nodeName ){

	if ( config && config->nodeExists( nodeName ) ){
		
		string hName = config->tagName( nodeName );
		if ( "" == hName )
			hName = nodeName;

		// store the path in the config file
		configPath[ hName ] = nodeName;

		string type = config->getString( nodeName + ":type", "1D" );

		
		if ( "1D" == type ){
			if ( config->nodeExists( nodeName + ".xBins" ) ){
				
				vector<double> xBins = config->getDoubleVector( nodeName + ".xBins" );
				make1D( hName, config->getString( nodeName + ":title", hName ), 
					xBins.size() - 1, xBins.data() );

			} else if ( config->nodeExists( nodeName + ":xBins" ) ) {

				vector<double> xBins = config->getDoubleVector( config->getString( nodeName + ":xBins" ) );
				make1D( hName, config->getString( nodeName + ":title", hName ), 
					xBins.size() - 1, xBins.data() );

			} else {	
			make1D( hName, config->getString( nodeName + ":title", hName ), 
					config->getInt( nodeName + ":nBinsX", 1 ), config->getDouble( nodeName + ":x1", 0 ),
					config->getDouble( nodeName + ":x2", 1 ) );
			}

		} else if ( "2D" == type ){
			make2D( hName, config->getString( nodeName + ":title", hName ), 
					config->getInt( nodeName + ":nBinsX", 1 ), config->getDouble( nodeName + ":x1", 0 ),
					config->getDouble( nodeName + ":x2", 1 ),
					config->getInt( nodeName + ":nBinsY", 1 ), config->getDouble( nodeName + ":y1", 0 ),
					config->getDouble( nodeName + ":y2", 1 ) );
		}


	
	}

}
void histoBook::makeAll( xmlConfig * con, string nodeName ){
	
	if ( !con )
		return;
	vector<string> paths = con->childrenOf( nodeName );

	for ( int i=0; i < paths.size(); i++ ){
		make( paths[ i ] );
	}
}
void histoBook::makeAll( string nodeName ){
	if ( !config )
		return;

	makeAll( config, nodeName );
}

void histoBook::make1F( string name, string title, uint nBins, double low, double hi  ){

	TH1F* h;
	h = new TH1F( name.c_str(), title.c_str(), nBins, low, hi );

	this->add( name, h );
}


void histoBook::make1D( string name, string title, uint nBins, double low, double hi  ){

	TH1D* h;
	h = new TH1D( name.c_str(), title.c_str(), nBins, low, hi );

	this->add( name, h );
}

void histoBook::make1D( string name, string title, uint nBins, const Double_t* bins  ){

	TH1D* h;
	h = new TH1D( name.c_str(), title.c_str(), nBins, bins );

	this->add( name, h );
}

void histoBook::make2D( string name, string title, uint nBinsX, double lowX, double hiX, uint nBinsY, double lowY, double hiY ){

	TH2D* h;

	h = new TH2D( name.c_str(), title.c_str(), nBinsX, lowX, hiX, nBinsY, lowY, hiY );

	this->add( name, h );
}
void histoBook::make2D( string name, string title, uint nBinsX, const Double_t* xBins, uint nBinsY, double lowY, double hiY ){

	TH2D* h;
	h = new TH2D( name.c_str(), title.c_str(), nBinsX, xBins, nBinsY, lowY, hiY );

	this->add( name, h );
}


TH1* histoBook::get( string name, string sdir  ){
	if ( sdir.compare("") == 0)
		sdir = currentDir;
	return book[ ( sdir  + name  ) ];
}
TH2* histoBook::get2D( string name, string sdir  ){
	if ( sdir.compare("") == 0)
		sdir = currentDir;
	return (TH2*)book[ ( sdir  + name  ) ];
}
TH3* histoBook::get3D( string name, string sdir ){
	return (( TH3* ) get( name, sdir ));
}

void histoBook::fill( string name, double bin, double weight ){ 
	if ( get( name ) != 0)
		get( name )->Fill( bin, weight );
}


histoBook* histoBook::exportAs( string filename ) {

	string outName = styling + ".png";
	if ( "" != filename )
		outName = filename;
	gPad->SaveAs( outName.c_str() );
	return this;

}

void histoBook::globalStyle(){

	gStyle->SetCanvasColor(kWhite);     // background is no longer mouse-dropping white
  	gStyle->SetPalette(1,0);            // blue to red false color palette. Use 9 for b/w
  	gStyle->SetCanvasBorderMode(0);     // turn off canvas borders
  	gStyle->SetPadBorderMode(0);
  	gStyle->SetPaintTextFormat("5.2f");  // What precision to put numbers if plotted with "TEXT"


  	// For publishing:
  	gStyle->SetLineWidth(2.);
  	gStyle->SetTextSize(0.7);
  	gStyle->SetLabelSize(0.05,"xy");
  	gStyle->SetTitleSize(0.05,"xy");
  	gStyle->SetTitleOffset(1.0,"x");
  	gStyle->SetTitleOffset(1.5,"y");
  	gStyle->SetPadTopMargin(0.1);
  	gStyle->SetPadRightMargin(0.1);
  	gStyle->SetPadBottomMargin(0.16);
  	gStyle->SetPadLeftMargin(0.2);

  	gStyle->SetFillColor(-1); 
	gStyle->SetFillStyle(4000); 


	
}


histoBook* histoBook::style( string histName ){
	styling = histName;

	// set the default style if it is there
	if ( config && config->nodeExists( configPath[ histName ] + ".style" ) ){
		set( configPath[histName] + ".style" );
	} else if ( config && config->nodeExists( configPath[ histName ] + ":style" ) && config->nodeExists( config->getString( configPath[ histName ] + ":style" ) ) ){
		set( config->getString( configPath[ histName ] + ":style" ) );
	}

	return this;
}

histoBook* histoBook::set( string param, string p1, string p2, string p3, string p4 ){
	
	vector<string> l;
	l.push_back( p1 );
	l.push_back( p2 );
	l.push_back( p3 );
	l.push_back( p4 );
	set( param, l );

	return this;
}

histoBook* histoBook::set( string param, double p1, double p2, double p3, double p4  ){

	vector<string> list;
	stringstream sstr;
	sstr.str("");
	sstr << p1;
	list.push_back( sstr.str() );

	sstr.str("");
	sstr << p2;
	list.push_back( sstr.str() );

	sstr.str("");
	sstr << p3;
	list.push_back( sstr.str() );

	sstr.str("");
	sstr << p4;
	list.push_back( sstr.str() );

	set( param, list );
    
	return this;
}

histoBook* histoBook::set( xmlConfig* config, string nodePath ){

	// get the list of attributes and set the style from that
	vector< pair< string, string > > list = config->getAttributes( nodePath );

	for ( int i = 0; i < list.size(); i++ ){

		vector<string> params = config->split( list[ i ].second, ',' );
		for ( int p = 0; p < params.size(); p++ ){
			params[ p ] = config->trim(params[ p ]);
		}
		

		set( list[ i ].first, params );

	}

	return this;
}
histoBook* histoBook::set( string opt, vector<string> params ){

	//cout  << "Setting : " << opt << endl;
	//for ( int i = 0; i < params.size(); i++ ){
	//	cout << params[ i ] << " ";
	//}
	//cout << endl;
	// force the param name to lowercase
	transform(opt.begin(), opt.end(), opt.begin(), ::tolower);

    TH1* h = get( styling );
    if ( h ){

	    if ( "title" == opt ){
	    	h->SetTitle( cParam(params, 0) );
	    } else if ( "x" == opt ){
	    	h->GetXaxis()->SetTitle( cParam(params, 0) );
	    } else if ( "y" == opt ){
	    	h->GetYaxis()->SetTitle( cParam(params, 0) );
	    } else if ( "legend" == opt ){
	    	legend->AddEntry( h, cParam(params, 0), cParam(params, 1, "lpf") );
			legend->Draw();
	    } else if ( "draw" == opt ){
	    	drawOption = cParam(params, 0);
	    } else if ( "linecolor" == opt ){
	    	int c = color( cParam( params, 0) );
	    	if ( c  < 0 )
	    		c = (int) dParam( params, 0);
	    	h->SetLineColor( c );
	    } else if ( "fillcolor" == opt ){
	    	int c = color( cParam( params, 0) );
	    	if ( c  < 0 )
	    		c = (int) dParam( params, 0);
	    	h->SetFillColor( c );
	    } else if ( "linewidth" == opt ){
	    	h->SetLineWidth( dParam( params, 0) );
	    } else if ( "domain" == opt ){
	    	double min = dParam( params, 0);
	    	double max = dParam( params, 1);
		    h->GetXaxis()->SetRangeUser( min, max );
	    } else if ( "dynamicdomain" == opt ){
	    	double thresh = dParam( params, 0);
	    	int min = (int)dParam( params, 1);
	    	int max = (int)dParam( params, 2);
	    	int axis = (int)dParam( params, 3);		// 1 = x, 2 = y

	    	if ( 1 != axis && 2 != axis )
	    		axis = 1;
	    	
	    	if ( thresh >= 0) {
	    		if ( -1 >= min )
	    			min = h->FindFirstBinAbove( thresh, axis );
	    		if ( -1 >= max )
	    			max = h->FindLastBinAbove( thresh, axis );
	    	}
	    	
	    	if ( 1 == axis )
		  	  h->GetXaxis()->SetRange( min, max );
		  	else if ( 2 == axis )
		  		h->GetYaxis()->SetRange( min, max );

	    }  else if ( "range" == opt ){

	    	double min = dParam( params, 0);
	    	double max = dParam( params, 1);
	    	
	    	h->GetYaxis()->SetRangeUser( min, max );
	    } else if ( "markercolor" == opt ) {
	    	int c = color( cParam( params, 0) );
	    	if ( c  < 0 )
	    		c = (int) dParam( params, 0);
	    	h->SetMarkerColor( c );
	    } else if ( "markerstyle" == opt ) {
	    	h->SetMarkerStyle( (int)dParam( params, 0) );
	    } else if ( "legend" == opt ){
	    	// p1 - alignmentX
	    	// p2 - alignmentY
	    	// p3 - width
	    	// p4 - height

	    	// make sure option is valid
	    	double p1 = dParam( params, 0);
	    	double p2 = dParam( params, 1);
	    	if ( !(legendAlignment::center == p1 || legendAlignment::left == p1 || legendAlignment::right == p1) )
	    		p1 = legendAlignment::best;
	    	if ( !(legendAlignment::center == p2 || legendAlignment::top == p2 || legendAlignment::bottom == p2) )
	    		p2 = legendAlignment::best;
	    	placeLegend( p1, p2, dParam( params, 3), dParam( params, 3) );
	    } else if ( "numberofticks" == opt ){
	    	// p1 - # of primary divisions
	    	// p2 - # of secondary divisions
	    	// p3 - axis : 0 or 1 = x, 2 = y
	    	double p1 = dParam( params, 0);
	    	double p2 = dParam( params, 1);
	    	double p3 = dParam( params, 2);

	    	if ( p2 == -1 )
	    		p2 = 0;

		    if ( 2 == (int)p3 )
		    	h->GetYaxis()->SetNdivisions( (int) p1, (int) p2, 0, true );
		    else 
		    	h->GetXaxis()->SetNdivisions( (int) p1, (int) p2, 0, true );
	    } else if ( "logy" == opt ){
	    	gPad->SetLogy( (int)dParam( params, 0 ) );
	    } else if ( "logx" == opt ){
	    	gPad->SetLogx( (int)dParam( params, 0 ) );
	    } else if ( "logz" == opt ){
	    	gPad->SetLogz( (int)dParam( params, 0 ) );
	    }




	}

	return this;



}
histoBook * histoBook::set( string nodeName ){
	if ( config )
		set( config, nodeName );
	return this;
}



histoBook* histoBook::draw(string name, Option_t* opt ){

	// no parameters
	if ( name == ""){
		TH1* h = get( styling );
		if ( h ){
			// use the draw option set in its styling
			h->Draw( drawOption.c_str() );
			drawOption = "";
		}	
	} else {
		TH1* h = get( name );
		if ( h ){
			h->Draw( opt );
		}
	}
	
	return this;
}


histoBook* histoBook::placeLegend( int alignmentX, int alignmentY, double width, double height ){

	double mR = 1 - gPad->GetRightMargin();
	double mL = gPad->GetLeftMargin();
	double mT = 1- gPad->GetTopMargin();
	double mB = gPad->GetBottomMargin();

	double x1, x2, y1, y2;

	if ( width <= 0 || width > 1 )
		width = .2;
	if ( height <= 0 || height > 1 )
		height = .2;

	// alignment best needs a current histo
	if ( !(get( styling )) ){
		if ( legendAlignment::best == alignmentX )
			alignmentX = legendAlignment::right;
		if ( legendAlignment::best == alignmentY )
			alignmentY = legendAlignment::top;
	} else {

		//TODO

	}


	if ( 	legendAlignment::left ==  alignmentX ){
		x1 =  mL ;
		x2 =  mL + width;
	}
	if ( 	legendAlignment::right ==  alignmentX ){
		x1 =  mR - width;
		x2 =  mR ;
	}
	if ( 	legendAlignment::center ==  alignmentX ){
		x1 =  0.55 - width/2.0;
		x2 =  0.55 + width/2.0;
	}
	if ( 	legendAlignment::top ==  alignmentY ){
		y1 =  mT - height;
		y2 = mT ;
	}
	if ( 	legendAlignment::bottom ==  alignmentY ){
		y1 =  mB ;
		y2 =  mB + height;
	}
	if ( 	legendAlignment::center ==  alignmentY ){
		y1 =  0.55 - height/2.0;
		y2 =  0.55 + height/2.0;
	}
	legend->SetX1NDC( x1 );
	legend->SetX2NDC( x2 );
	legend->SetY1NDC( y1 );
	legend->SetY2NDC( y2 );

	return this;
}