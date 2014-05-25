
#include <iostream>
#include "allroot.h"
#include "dirent.h"
#include <cstdio>

void clean( char * tree, char* ntdir, uint maxFiles = 1000 );

int main( int argc, char* argv[] ) {

    
    if ( argc < 4 ){
    	
    	cout << "Usage: \n\t"
    			"ntupleCleaner treeName dataDirectory numFiles"
                "\n\n\nBe careful using this, improper use could delete good data"
                << endl;

       	return 0;

    } 
    
    clean( argv[1], argv[2], atoi( argv[ 3] ) );
    
	return 0;
}




void clean(  char * tree,
                        char* ntdir,        // the directory in which to look for ntuples
                        uint maxFiles 
                        ) {
    //cout << " [chainLoader] searching " << ntdir << " for ntuples" << endl;

    if (maxFiles == 0)
        maxFiles = 1000;

    uint nFiles = 0;
    DIR *dir;
    struct dirent *ent;
    bool go = true;
    if ( (dir = opendir ( ntdir ) ) != NULL) {
        
        while ( go && (ent = readdir ( dir) ) != NULL) {

            if ( strstr( ent->d_name, "root") ){
                
                if ( nFiles >= maxFiles ) 
                    go = false;

                char fn[ 1024 ];
                sprintf( fn, "%s%s", ntdir, ent->d_name );
                
                TFile * f = new TFile( fn );

                TTree* t = (TTree*)f->Get( tree );
                if ( !t ){
                    cout << "NO TREE IN " << fn << endl;
                    remove( fn );
                }

                delete f;

                //cout << "[chainLoader] Adding file " << fn << " to chain" << endl;
                nFiles++;

            }
        }
        
        cout << "[chainLoader] " << (nFiles - 1) << " files loaded into chain" << endl;

        closedir (dir);
    }

}
