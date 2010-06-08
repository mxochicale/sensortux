#include "RoggenDataExtension.h"

using namespace std;

/**
 * Constructor: The original source has to be given when constructing the extension. 
 * This class just adds two computed measurements to the stream. 
 */
RoggenDataExtension::RoggenDataExtension(IntSource * givenSource) {
	source = givenSource;
}

/**
 * Empty destructor. 
 */
RoggenDataExtension::~RoggenDataExtension() {
	// nothing to destruct 
}

/**
 * Overwrites the getData() function of the IntSource interface. 
 * The given data is used to compute two more measurements. 
 * The extra data then gets appended to the end of each frame. 
 */
vector<vector<int> > RoggenDataExtension::getData() {
	vector<vector<int> > ret;
	ret.clear();
	vector<vector<int> > in;
	in = source->getData();
	vector<int> line;
	for (vector<vector<int> >::iterator it=in.begin(); it!=in.end(); ++it) {
		line.clear();
		line = *it;
		if (it->size() > 12) { // there must be 13 measurements (0..12)
			// read data
			int ref = it->at(12);
			int xAmp = it->at(9);
			int yAmp = it->at(11);
			// compute new data
			int xNew = (xAmp-ref)*2;
			int yNew = (yAmp-ref)*2;
			// add data
			line.push_back(xNew);
			line.push_back(yNew);
		}
		ret.push_back(line);
	}
	return ret;
}



