
#ifndef XMLCONFIGLIB_H
#define XMLCONFIGLIB_H 


#include "rapidXML.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include <sstream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <cerrno>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <iomanip>


using namespace std;
using namespace rapidxml;

class xmlConfig {

public: 
	/*
	*	Constructor
	*	Parameters:
	*		filename: the config filename
	*/
	xmlConfig( const char * filename = "config.xml" ) {
		fname = filename;
		configFile = getFileContents( filename );

		char* cstr = new char[configFile.size() + 1];  	// Create char buffer to store string copy
	  	strcpy (cstr, configFile.c_str());             		// Copy string into char buffer

	  	doc.parse<0>(cstr); 
	}

	string getFilename() { return fname; }

	xmlConfig( xmlConfig & config ){
		fname = config.getFilename();
		cout << "my fnmae = " << fname << endl;

		configFile = getFileContents( fname.c_str() );

		char* cstr = new char[configFile.size() + 1];  	// Create char buffer to store string copy
	  	strcpy (cstr, configFile.c_str());             		// Copy string into char buffer

	  	doc.parse<0>(cstr);
	}

	string getFileContents(const char *filename){
		ifstream in(filename, ios::in | ios::binary);

		if (in){
			std::string contents;

			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());

			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());

			in.close();

			return(contents);
		}
		throw(errno);
	}

	int getInt( char* nName, int def = 0 ){
		if ( !nodeExists( nName ) )
			return def;

		string str = getString( nName );
		if ( str.length() >= 1 ){
			return atoi( str.c_str() );
		}

		return def;
	}
	bool getBool( char* nName, bool def = false ) {
		if ( !nodeExists( nName ) )
			return def;

		string str = getString( nName );

		// first check for string literal "true" or "false"
		// first push to lower case
		transform(str.begin(), str.end(), str.begin(), ::tolower);
		if ( str == (string)"false" )
			return false;
		if ( str == (string)"true" )
			return true;

		// if not look for an integer
		// 0 or negative = false
		// any positive = true
		if ( str.length() >= 1 ){
			return ( atoi( str.c_str() ) >= 1 );
		}
		return def;
	}
	double getDouble( char* nName, double def = 0 ){
		if ( !nodeExists( nName ) )
			return def;

		string str = getString( nName );
		if ( str.length() >= 1 ){
			return atof( str.c_str() );
		}

		return def;
	}


	string getString( char* nName, string def = "", bool trimW = true ){
		
		if ( !nodeExists( nName ) )
			return def;

		stringstream sstr; 

	  	vector<string> ntf = split( nName, '.' );
	  	vector<string> attr = split( nName, ':' );
	  	if ( attr.size() >= 2 ){
			ntf[ ntf.size() - 1 ] = ntf[ ntf.size() - 1 ].substr( 0, ntf[ ntf.size() - 1 ].length() - (attr[ 1].length() + 1) );
		}

		xml_node<> *node = doc.first_node();
		for ( uint i = 0; i < ntf.size(); i++ ){
			if ( node ){
				node = node->first_node( ntf[ i ].c_str() );
				if ( node && ntf.size() - 1 == i ){
					

					if ( attr.size() >= 2 ){
						if ( node->first_attribute( attr[ 1 ].c_str() ) ){
							sstr << node->first_attribute( attr[ 1 ].c_str() )->value();
							if ( trimW )
								return trim( sstr.str() );
							else
								return sstr.str();							
						}
							
					}

					sstr << node->value();
					if ( trimW )
						return trim( sstr.str() );
					else
						return sstr.str();
				}
			} else {
				return def;
			}
		}

		return def;
	}

	void display( char* nName ){
		// if it is a vector print it that way
		stringstream sstr;
		std::vector<string> v = getStringVector( nName );
		if ( isVector( nName ) ){
			
			for ( uint i = 0; i < v.size(); i++ ){
				sstr.str("");
				sstr << nName << "[ " << i << " ] ";
				cout << "[" << fname <<  "] " << std::left <<  setw(20) << sstr.str() << v[i] << endl;
			}
			return;
		}

		cout << "[" << fname <<  "] " << std::left << setw(20) <<  nName  << getString( nName ) << endl;
	}

	vector<string> getStringVector( char* nName, bool trimW = true ){
		stringstream sstr;

		std::vector<string> res;

		vector<string> ntf = split( nName, '.' );

		string index = "v";

		xml_node<> *node = doc.first_node();
		for ( uint i = 0; i < ntf.size(); i++ ){
			if ( node ){
				node = node->first_node( ntf[ i ].c_str() );
				if ( node->first_attribute( "index" ) ){
					//cout << " attrib : " << node->first_attribute( "index" )->value() << endl;
					sstr.str("");
					sstr << node->first_attribute( "index" )->value();
					index = sstr.str();
				}
				if ( node && ntf.size() - 1 == i ){
					for (xml_node<> *child = node->first_node(  ); child; child = child->next_sibling() ){
					    sstr.str("");
					    
					    sstr << child->name();

					    if ( sstr.str() != index )
					    	continue; 

					    sstr.str("");

					    if ( trimW )
					    	sstr << trim( child->value() );
					    else
					    	sstr << child->value();

					    res.push_back( sstr.str() );
					}	
				}
			} else {
				return res;
			}
		}

		return res;
	}
	vector<double> getDoubleVector( char* nName ){
		vector<string> str = getStringVector( nName );
		vector<double> res;
		for ( uint i = 0; i < str.size(); i++ ){
			res.push_back( atof( str[ i ].c_str() ) );
		}
		return res;
	}

	bool nodeExists( char* nName ){
		vector<string> ntf = split( nName, '.' );
		vector<string> attr = split( nName, ':' );
		if ( attr.size() >= 2 ){
			ntf[ ntf.size() - 1 ] = ntf[ ntf.size() - 1 ].substr( 0, ntf[ ntf.size() - 1 ].length() - (attr[ 1].length() + 1) );
		}

		xml_node<> *node = doc.first_node();
		for ( uint i = 0; i < ntf.size(); i++ ){
			if ( node ){
				node = node->first_node( ntf[ i ].c_str() );
				if ( attr.size() <= 1 ){
					if ( node && ntf.size() - 1 == i )
						return true;
				} else {
					if ( node && ntf.size() - 1 == i && node->first_attribute( (char*)attr[ 1 ].c_str() ) ) 
						return true;
				}
			} else
				return false;
		}

		return false;
	}

	bool isVector( char* nName ){

		if ( nodeExists( nName )){

			std::vector<string> v = getStringVector( nName );
			if ( v.size() >= 1 && v[ 0 ] != getString( nName ) ){
				return true;
			}
			return false;
		} else {
			return false;
		}
	}

	// report
	void report( xml_node<> * bn = NULL, string cp = "") {
		

		string np = cp;
		stringstream sstr;
		stringstream n;

		xml_node<> *node = doc.first_node();
		if ( bn )
			node = bn;
		if ( !node )
			return;

		n << node->name();
		if ( bn != NULL  ){

			if ( cp != "" && n.str() != "" )
				sstr << cp << '.' << node->name();
			else 
				sstr << cp << node->name();
			np = sstr.str();
		}

		//cout << "REPORT : " << np << " " << isVector( (char*)np.c_str() ) << endl;


		if ( node->first_node(  ) && !isVector( (char*)cp.c_str() )){

			if ( !isVector( (char*)np.c_str() ) ){
				for (xml_node<> *child = node->first_node(  ); child; child = child->next_sibling() ){
					report( child, np );
				}
			} else {
				// the next is a vector, let display show it as one
				display( (char*)np.c_str() );
			}
		} else {
			// end of line, actually report it now
			display( (char*)np.c_str() );
		}
	}

	vector<string> &split(const string &s, char delim, vector<string> &elems) {
	    stringstream ss(s);
	    string item;
	    while (std::getline(ss, item, delim)) {
	        elems.push_back(item);
	    }
	    return elems;
	}
	vector<string> split(const string &s, char delim) {
	    vector<string> elems;
	    split(s, delim, elems);
	    return elems;
	}

	std::string trim(const std::string& str, const std::string& whitespace = " \t\n") {
	    const auto strBegin = str.find_first_not_of(whitespace);
	    if (strBegin == std::string::npos)
	        return ""; // no content

	    const auto strEnd = str.find_last_not_of(whitespace);
	    const auto strRange = strEnd - strBegin + 1;

	    return str.substr(strBegin, strRange);
	}

private:
	string configFile;
	string fname;

	xml_document<> doc;
};



#endif