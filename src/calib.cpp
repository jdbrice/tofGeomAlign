
#include "constants.h"
#include "calib.h"
#include "histoBook.h"
#include <fstream>
#include <sstream>


// provides my own string shortcuts etc.
using namespace jdbUtils;



/**********************************************************
*	Constructor
*
* 	Parameters:
*	chain:		The chain object containing all data 
*				compatible with the TOFrPicoDST format
*	xmlConfig:	The xml configuration defining calib values 
*				See repo Readme for a sample configuration.	
***********************************************************/
calib::calib( TChain* chain, xmlConfig *con )  {
	cout << "[calib.calib] " << endl;
	
	config = con;

	// create the histogram book
	cout << "[calib.calib] Creating book" << endl;
	book = new histoBook( ( config->getString( "output.baseName" ) + config->getString( "output.root" ) ), 
							config->getString( "input.root" ) );

	_chain = chain;
	cout << "[calib.calib] Creating Pico DST" << endl;
	pico = new TOFrPicoDst( _chain );

	gStyle->SetOptStat( 0 );

	nSections = constants::nSections;

	gErrorIgnoreLevel=kSysError;
	
	cout << "[calib.calib] Creating Reports" << endl;
	reportX = new reporter( config->getString( "output.baseName" ) + "x" + config->getString( "output.report" ) );
	reportY = new reporter( config->getString( "output.baseName" ) + "y" + config->getString( "output.report" ) );

	cout << "[calib.calib] Creating Module Space Map" << endl;
	// get the module start locations
	std::vector<double> tmp = config->getDoubleVector( "moduleStart" );

	if ( tmp.size() >= 1 && tmp[ 0 ] != config->getString( "moduleStart" ) )
		moduleStart = tmp;
	else {
	
		moduleStart.push_back( 1 );
		moduleStart.push_back( 7 );
		moduleStart.push_back( 9 );
		moduleStart.push_back( 10 );
		moduleStart.push_back( 15 );
		moduleStart.push_back( 19 );
		moduleStart.push_back( 21 );
		moduleStart.push_back( 33 );
	}

	// doesnt change ever, unless they rebuild tof detector
	moduleSpaceMap = {0, 16, 20, 22, 26, 30, 32};

}

/**********************************************************
*	Destructor
*	Closes the report PDF file and closes the histoBook
*	to ensure that the root file is saved
***********************************************************/
calib::~calib() {
	// close the pdf report file
	
	// delete the histoBook ensuring it is release and the file saves
	delete book;
	delete reportX;
	delete reportY;
}

/**
 * Plots the y and z local TOF hit positions against the 
 * tray# and phi coordinate. Used in the fitting steps.
 */
void calib::localPosition() {

	cout << "[calib." << __FUNCTION__ << "] Start " << endl;
	startTimer();

	int minHitsFit = config->getInt( "minHitsFit", 25);
	double ptCut = config->getDouble( "ptCut", 0.5 );
	double vzCut = config->getDouble( "vzCut", 30.0);


	if ( !_chain ){
		cout << "[calib." << __FUNCTION__ << "] ERROR: Invalid chain " << endl;
		return;
	}


	Int_t nevents = (Int_t)_chain->GetEntries();
	cout << "[calib." << __FUNCTION__ << "] Loaded: " << nevents << " events " << endl;


	// Make the Histograms for storing local hit position
	book->cd( "localPosition" );

	int nBins = config->getInt( "numBins", 100 );
	int nBinsZ = config->getInt( "numBinsZ", 200 );
	for ( int j = 0; j < nSections; j++ ){

		book->make2D( "yLocal" + ts( j ), "Y Local Position; tray; yLocal [cm]", constants::nTrays, 0.0 ,constants::nTrays, nBins, -5.0, 5.0 );
		book->make2D( "zLocal" + ts( j ), "Z Local Position; tray; zLocal [cm]", constants::nTrays, 0.0 ,constants::nTrays, nBinsZ, -10.0, 10.0 );

		book->make2D( "yLocalPhi" + ts( j ), "Y Local Position; #phi [deg]; yLocal [cm]", constants::nTrays, -3.0, 717.0, nBins, -5.0, 5.0 );
		book->make2D( "zLocalPhi" + ts( j ), "Z Local Position; #phi [deg]; zLocal [cm]", constants::nTrays, -3.0, 717.0, nBins, -5.0, 5.0 );

	}

	book->make2D( "yLocalAll", "Y Local Position All Modules; tray; yLocal [cm]", constants::nTrays, 0.0 ,constants::nTrays, nBins, -5.0, 5.0 );
	book->make2D( "zLocalAll", "Z Local Position All Modules; tray; zLocal [cm]", constants::nTrays, 0.0 ,constants::nTrays, nBinsZ, -10.0, 10.0 );
	book->make2D( "yLocalPhiAll", "Y Local Position All; #phi [deg]; yLocal", constants::nTrays, -3.0, 717.0, nBins, -5.0, 5.0 );
	book->make2D( "zLocalPhiAll", "Z Local Position All; #phi [deg]; yLocal", constants::nTrays, -3.0, 717.0, nBins, -5.0, 5.0 );

	

	cout << "[calib." << __FUNCTION__ << "] Made Histograms " << endl;

	// loop over all the events and fill the local positions for each
	for(Int_t i=0; i<nevents; i++) {
    	_chain->GetEntry(i);

		
		progressBar( i, nevents, 50 );

		double vz = pico->vertexZ;
		if ( TMath::Abs( vz ) > vzCut ) continue;

		// get the number of tof hits and loop through them
		int nHits = pico->nTofHits;
		for ( int j = 0; j < nHits; j++ ){

			// require a min number of hits in the track fit
			if ( TMath::Abs( pico->nHitsFit[ j ] ) < minHitsFit ) continue;
			// require a min pt cut
			if ( pico->pt[ j ] < ptCut ) continue;

			// get the start index of the module to know what region it is in
			// there are 7 module groups. The angles of all modules in a group is the same
			int tray = pico->tray[ j ];
			int module = pico->module[ j ];
			int startIndex = -1;
			for ( int k = 0; k < nSections; k++ ){
				if ( module >= moduleStart[ k ] && module < moduleStart[ k + 1 ] ){
					startIndex = k;
					break;
				}
			}
			if ( startIndex < 0 || startIndex >= 7 ) continue;

			// map the phi just for plotting purposes
			double phi;
			if( tray <= 60 ) {
				
				phi = 72.0 - ( tray-1 ) * 6.0;
				
				if( phi < -3.0 ) 
					phi += 360.0;

			} else {
				
				phi = 108.0 + ( tray - 61 ) * 6.0;
				
				if( phi > 357.0 ) 
					phi -= 360.0;

				phi += 360.;   // shifted by 360 for east
			}

			// fill the histograms
			double yLocal = pico->yLocal[ j ];
			double zLocal = pico->zLocal[ j ];

			book->fill( "yLocalPhi" + ts( startIndex ), phi, yLocal );
			book->fill( "zLocalPhi" + ts( startIndex ), phi, zLocal );

			book->fill( "yLocal" + ts( startIndex ), tray - 1, yLocal );
			book->fill( "zLocal" + ts( startIndex ), tray - 1, zLocal );

			book->fill( "yLocalPhiAll" , phi, yLocal );
			book->fill( "zLocalPhiAll" , phi, zLocal );

			book->fill( "yLocalAll" , tray - 1, yLocal );
			book->fill( "zLocalAll" , tray - 1, zLocal );

		}

	}

	cout << "[calib." << __FUNCTION__ << "] completed in " << elapsed() << " seconds " << endl;
	
}
/**********************************************************
*	fitY
*	Performs the fit for determining the offset for yLocal. 
*	Uses the yLocal position on each tray and fits the hit 
*	distribution to a platue function to determine the
*	over all offset for each tray.
***********************************************************/
void calib::fitY() {


	startTimer();
	cout << "[calib." << __FUNCTION__ << "] Started"  << endl;

	reportY->newPage( );
	book->cd( "localPosition" );
	book->draw( "yLocalAll", "colz" );
	reportY->savePage();


	book->cd( "fitY" );
	book->make1D( "intercept", "Fit Intercept ", 	500, 0, 5);
	book->make1D( "divider", "Fit Divider", 		500, 0, 1);

	TH2D* rebinYLocalAll = (TH2D*)book->get( "yLocalAll", "localPosition")->Clone( "rebinYLocalAll" );

	int rebin = config->getInt( "rebinY", 1 );
	rebinYLocalAll->RebinY( rebin );


	// the tray value for plotting
	std::vector<double> t;			

	// clear the canvas and divide to print the tray reports
	reportY->newPage( 4, 5 );
	for ( int i = 1; i <= constants::nTrays; i++ ){
		
		progressBar( i, constants::nTrays + 1, 50 );

		// project out the tray from the 2D histogram
		TH1D * yLocal = rebinYLocalAll->ProjectionY( ("tray" + ts( i )).c_str(), i, i );
		yLocal->Sumw2();

		book->add( "tray" + ts( i ), yLocal );

		double* fitParam, *fitError;

		if ( 1 != i)
			reportY->next();

		// create the fit function
		// the fit range of -5, 5 is the expected maximum offset distance
		TF1 *fit = new TF1("fit", calib::fitFunction, -5.0, 5.0, 5);

		// style the fit for easier viewing
		fit->SetLineWidth( 2 );
		fit->SetLineColor( kRed );

		// give the fit inital parameters to aid convergence
		// gets the bin value at the middle as the approximate height of the plateau
		fit->SetParameters( 0, yLocal->GetBinContent( 50 ) , 0.1, 0.1, 0 );	
		


		// total entries in tray
		double entries = yLocal->Integral( );

		if ( entries >= config->getInt( "fitYMinHits", 10) ){

			// perform the fit to the data
			
			yLocal->Fit( "fit", "QRN" );
			yLocal->Fit( "fit", "QRN" );
			yLocal->Fit( "fit", "QRN" );

			fitParam = (fit->GetParameters());
			fitError = (fit->GetParErrors());

			book->fill( "intercept", 	fitParam[ 2 ]);
			book->fill( "divider", 		fitParam[ 3 ]);

			// save the fit results we want
			yPos.push_back( fitParam[ 0 ] );
			yError.push_back( fitError[ 0 ] );
			t.push_back( i );

		} else {
			// no fit possible, zero the tray
			yPos.push_back( 0 );
			yError.push_back( 0 );
			t.push_back( i );
		}

		// draw the tray hits and teh fit result
		
		
		book->style( "tray" +ts( i ) )->set( "title", "Tray" + ts( i ) + " : y Local Fit" )->draw( );
		fit->Draw("same");
		//reportY->savePage();

		


		if ( constants::nTrays  == i ){
			reportY->savePage();
		}
		
	}	// loop trays

	reportY->newPage();

	TH1D *frame = new TH1D("frame","",1, 0.5, 120.5);
  	frame->SetMinimum(-2);
  	frame->SetMaximum( 2);
	frame->GetYaxis()->SetTitle("yLocal Offset [cm]");
	frame->GetXaxis()->SetTitle("Tray #");
	
	frame->Draw();

	TGraphErrors * yTray = new TGraphErrors( constants::nTrays, t.data(), yPos.data(), 0, yError.data() );
	yTray->SetMarkerStyle( 20 );
   	yTray->SetMarkerSize( 0.5 );
   	yTray->SetLineWidth( 2 );
	yTray->Draw( "p" );

	reportY->savePage();


	cout << "[calib." << __FUNCTION__ << "] completed in " << elapsed() << " seconds " << endl;
}

/**********************************************************
*
*
*
*
*
***********************************************************/
void calib::fitXAndZ() {

	

	startTimer();
	cout << "[calib." << __FUNCTION__ << "] Started"  << endl;

	reportX->newPage( );
	book->cd( "localPosition" );
	book->draw( "zLocalAll", "colz" );
	reportX->savePage();

	book->cd( "fitXAndZ" );
	book->make1D( "intercept", "Fit Intercept ", 	500, 0, 5);
	book->make1D( "divider", "Fit Divider", 		500, 0, 1);

	int rebin = config->getInt( "rebinZ", 1);

	
	double z0[ constants::nTrays ][ constants::nSections ];
	double ze[ constants::nTrays ][ constants::nSections ];
	double z00[ constants::nTrays ];
	double ze0[ constants::nTrays ];
	double t[ constants::nTrays ];
		

	reportX->newPage( 4, 5 );


	// loop through the angle sections
	for ( int j = 0; j < constants::nSections; j++ ){ 
		
		TH2* zRebin = book->get2D( "zLocal" + ts( j ), "localPosition" );
		zRebin->RebinY( rebin );

		// loop trhough the trays
		for ( int k = 1; k <= constants::nTrays; k++ ){
			t[ k ] = k;

			if ( 1 != k )
				reportX->next();

			progressBar( k + (j * constants::nTrays), constants::nTrays*constants::nSections, 50);

			
			//TH1D* zLocal = ((TH2D*)book->get( "zLocal" + ts( j ), "localPosition"  ))->ProjectionY( ("trayMod" + ts(j) + "Tray" + ts( k )).c_str(), k, k);
			TH1D* zLocal = zRebin->ProjectionY( ("trayMod" + ts(j) + "Tray" + ts( k )).c_str(), k, k);
			

			book->add( "trayMod" + ts(j) + "Tray" + ts( k ), zLocal );

	
			double* fitParam, *fitError;

			// create the fit function
			TF1 *fit = new TF1("fit", calib::fitFunctionZ, -10.0, 10.0, 4);
			fit->SetLineWidth( 2 );
			fit->SetLineColor( kRed );
			// give the fit inital parameters to aid convergence
			fit->SetParameters( 0, zLocal->GetBinContent( 50 ), 1.5, 0.05 );
			//fit->SetParLimits( 1, 1, 10000 );
			fit->SetParLimits( 2, 1.5, 3.0 );
			fit->SetParLimits( 3, 0.01, .5 );

			
			// total entries in tray
			double entries = zLocal->Integral( );

			if ( entries >= config->getInt( "fitZMinHits", 10) ){

				// perform the fit to the data
				zLocal->Fit( "fit", "QRN" );
				zLocal->Fit( "fit", "QRN" );
				zLocal->Fit( "fit", "QRN" );
				zLocal->Fit( "fit", "QRN" );
				zLocal->Fit( "fit", "QRN" );

				fitParam = (fit->GetParameters());
				fitError = (fit->GetParErrors());

				book->fill( "intercept", 	fitParam[ 2 ]);
				book->fill( "divider", 		fitParam[ 3 ]);

				// save the fit results we want
				zPos[ k - 1 ].push_back( fitParam[ 0 ] );
				zError[ k -1 ].push_back( fitError[ 0 ] );

				z0[ k -1 ][ j ] = fitParam[ 0 ];
				ze[ k -1 ][ j ] = fitError[ 0 ];
				

			} else {
				// no fit possible, zero the tray
				z0[ k - 1 ][ j ] = 0 ;
				ze[ k - 1 ][ j ] = 0 ;

				zPos[ k - 1 ].push_back( 0 );
				zError[ k -1 ].push_back( 0 );
			}

			// draw the tray hits and teh fit result
			book->style( "trayMod" + ts(j) + "Tray" + ts( k ) )->set( "title", "Module " + ts( j ) + " Tray" + ts( k ) + " : z Local Fit" )->draw( );
			fit->Draw("same");
			
			TLatex *tex = new TLatex(-8, fitParam[1]/3.0, ("Z0 = " + ts( fitParam[ 0 ] ) + " #pm " + ts( fitError[ 0 ] )).c_str()  );
			tex->SetTextSize(0.07);
      		tex->Draw("same");

			if ( k == constants::nTrays ){
				reportX->savePage();
			}
			
		}
	}


	// Now do the fit to the angle for each module group for each tray #
	reportX->newPage( 4, 5 );


	// loop trhough the trays
	for ( int k = 1; k <= constants::nTrays; k++ ){
		if ( 1 != k )
			reportX->next();

		TF1* angleFit = new TF1( "angleFit", calib::fitAngle, -10, 40, 2);

	    // make a histogram to allow the graph the be drawn
	    TH1D *frame = new TH1D("frame", ("Tray " + ts( k )).c_str(), 1, -10, 50);
	    frame->SetMinimum( -2.5 );
	    frame->SetMaximum( 1.5 );
	    frame->GetXaxis()->SetTitle("Module Angle [deg]");
	    frame->GetYaxis()->SetTitle("zLocal Offset");
	    frame->Draw();

	    TGraphErrors *zTray = new TGraphErrors( constants::nSections, moduleSpaceMap, z0[ k -1 ], 0, ze[ k - 1 ] );
	    zTray->SetMarkerStyle( 24 );
   		zTray->SetMarkerSize( 0.5 );
   		zTray->SetLineWidth( 2 );
	    zTray->Draw( "pe" );

	    angleFit->SetParameters( -0.5, 0.0 );
    	zTray->Fit("angleFit","QRN");
    	zTray->Fit("angleFit","QRN");
    	zTray->Fit("angleFit","QRN");
    	zTray->Fit("angleFit","QRN");
    	zTray->Fit("angleFit","QRN");
    	angleFit->Draw( "same" );

    	// get the first modules position -> the zero angle offset
    	angle0zOff.push_back( zPos[ k - 1 ][ 0 ] );
    	angle0zError.push_back( zError[ k - 1 ][ 0 ] );

    	z00[ k - 1 ] = z0[ k - 1 ][ 0 ];
    	ze0[ k - 1 ] = ze[ k - 1 ][ 0 ];

    	// store the offsets in the class vectors
    	xPos.push_back( angleFit->GetParameter( 0 ) );
    	xError.push_back( angleFit->GetParError( 0 ) );

    	angle0FitOff.push_back( angleFit->GetParameter( 1 ) );
    	angle0FitError.push_back( angleFit->GetParError( 1 ) );


		if ( constants::nTrays == k ){
			reportX->savePage();
		}

		

	}// loop k on Trays


	reportX->newPage();
	TH1D *frame = new TH1D( "frame", "", 1, 0.5, 120.5 );
	frame->SetMinimum( -3 );
	frame->SetMaximum( 1 );
	frame->GetXaxis()->SetTitle("Tray #");
	frame->GetYaxis()->SetTitle("zLocal Offset");
	frame->Draw();

	// Plot the z Offset
	// plot the z offsets from the first module
	TGraphErrors *angle0 = new TGraphErrors( constants::nTrays, t, angle0zOff.data(), 0, angle0zError.data() );
	angle0->SetMarkerStyle( 20 ); angle0->SetMarkerSize( 1.5 );	angle0->SetLineWidth( 2 );
	angle0->Draw("p");

	// also plot the z offset from the fit
	TGraphErrors *angle0Fit = new TGraphErrors( constants::nTrays, t, angle0FitOff.data(), 0, angle0FitError.data() );
	angle0Fit->SetMarkerStyle( 24 );	angle0Fit->SetMarkerSize( 1.5 );	angle0Fit->SetLineWidth( 2 );
	angle0Fit->Draw("p");
	reportX->savePage();

	reportX->newPage();
	// now plot the x Offsets
	frame->SetMinimum( -2 );
	frame->SetMaximum( 3 );
	frame->GetXaxis()->SetTitle("Tray #");
	frame->GetYaxis()->SetTitle("xLocal Offset [cm]");
	frame->Draw();

	TGraphErrors *xFit = new TGraphErrors( constants::nTrays, t, xPos.data(), 0, xError.data() );
	xFit->SetMarkerStyle( 24 );	xFit->SetMarkerSize( 1.5 );	xFit->SetLineWidth( 2 );
	xFit->Draw("p");
	reportX->savePage();

	cout << "[calib." << __FUNCTION__ << "] completed in " << elapsed() << " seconds " << endl;
}



/**
 *	fit function for a gaussian platue signal peak
 *	par[ 0 ] = x Offset
 *	par[ 1 ] = scale ( height )
 *  par[ 2 ] = platuea Width 
 *  par[ 3 ] = sharpness
 *  par[ 4 ] = y Offset
 */
Double_t calib::fitFunction(Double_t *x, Double_t *par){
	Double_t y = ( TMath::Abs( x[0] - par[0] ) - par[2] ) / par[3];
  	return par[1] / ( 1.0 + TMath::Exp(y) ) + par[4];
}

Double_t calib::fitFunctionZ(Double_t *x, Double_t *par){
	Double_t y = ( TMath::Abs( x[0] - par[0] ) - par[2] ) / par[3];
  	return par[1] / ( 1.0 + TMath::Exp(y) );
}

Double_t calib::fitAngle(Double_t *x, Double_t *par){
  	return par[0]*TMath::Sin( x[0] * (TMath::Pi() / 180.0)) + par[1];
}


void calib::writeAlignment( ){

	startTimer();
	cout << "[calib." << __FUNCTION__ << "] Started"  << endl;

	ifstream in( config->getString( "input.geometry" ).c_str() );
	ofstream out( (config->getString( "output.baseName" ) + config->getString( "output.geometry" )).c_str() );

	if ( in.is_open() ) {
		for ( int iT = 0; iT < constants::nTrays; iT++ ){

			double iy, iz, ix;
			in >> iy >> iz >> ix;

			double y = yPos[ iT ] + iy;
			double x = xPos[ iT ] + ix;
			double z = -angle0zOff[ iT ] + iz;
			out << setw(15) << y << setw(15) << z << setw(15) << x << endl;

		}

		in.close();
		out.close();
	} else {

		for ( int iT = 0; iT < constants::nTrays; iT++ ){

			double y = yPos[ iT ];
			double x = xPos[ iT ];
			double z = -angle0zOff[ iT ];
			out << setw(15) << y << setw(15) << z << setw(15) << x << endl;
		}

		out.close();
	}

	cout << "[calib." << __FUNCTION__ << "] completed in " << elapsed() << " seconds " << endl;

}
