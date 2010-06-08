/**
 * Test software. 
 * Prints the sensor measurements (AccX, AccY, ..., Rot...) to the screen. 
 * Use this software to check if data acquisiation works well. 
 * Usage: ./printrawsensors <Sensor Devices> <Sensor Format>
 * If no command line parameters are given the settings from the config file are used. 
 */

#include "../src/IntSource.h"
#include "../src/RoggenSensorFusion.h"
#include "../src/Helper.h"
#include "../src/Configuration.h"
// #include "../src/DataStrings.h"
// #include "../src/RoggenBuffer.h"

#include <iostream>

#define CONFIG_FILE "config.xml"

using namespace std;

//-------------------------------------------------------------------------------------------------
// main
//-------------------------------------------------------------------------------------------------
int main(int argc, char **argv) {
	cout << "Start Roggen Sensor Data Printing" << endl; 
	// Prepare device names und format string
	vector<string> deviceNames; 
	Configuration * config = NULL;
	deviceNames.clear(); 
	string format;
	format.clear();
	if (argc < 3) {
		cout << "Using configuration file" << endl; 
		config = Configuration::getInstance(CONFIG_FILE);
		deviceNames = config->getStrings("sensor devices");
		format = config->getString("sensor format");
	} else {
		cout << "Using command line parameters" << endl; 
		for (int i=1; i<argc-1; ++i) { 
			deviceNames.push_back(argv[i]); 
		} 
		format = argv[argc-1]; 
	}
	// example/default values
	// deviceNames.clear(); 
	// format.clear();
	// deviceNames.push_back("/dev/rfcomm0");
	// deviceNames.push_back("/dev/rfcomm1");
	// format = "DX5;ccsss-s-s-ssssss";

	// print information
	cout << "Sensor device names: ";
	for (vector<string>::iterator it=deviceNames.begin(); it!=deviceNames.end(); ++it) {
		cout << *it << "; ";
	}
	cout << endl;
	cout << "Format: " << format << endl;
	IntSource * rsf = new RoggenSensorFusion(deviceNames,format); 
	
	// start printing data
	cout << "Getting Data ..." << endl; 
	bool done = false;
	while (!done) { 
		RoggenData rd = rsf->getData();
		for (int i=0; i<rd.size(); ++i) {
			cout << i << " ";			
			Helper::getInstance()->echoBr(rd.at(i));
		}
		if (rd.size() <= 0) {
			cerr << "Got empty data package" << endl; 
			done = true;
		}
		if (Helper::kbhit()) { 
			char c = Helper::getch(); 
			if (c == 27) { 
				cout << endl << "Esc hit, exiting ..." << endl; 
				done = true; 
			} else { 
				cout << endl << "Hit Esc key to terminate application" << endl; 
			} 
		} 
	}
	cout << "Printing Data done" << endl; 

	// clean up
	safeDelete(rsf); 	
	cout << "Termination done" << endl; 
	return 0;		// exit application 
} 


