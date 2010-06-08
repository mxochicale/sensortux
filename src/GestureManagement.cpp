#include "GestureManagement.h"

#include "Helper.h"
#include "RoggenSensorFusion.h"
#include "RoggenDataExtension.h"
#include "RoggenBuffer.h"
#include "NearestClusterCenter.h"
#include "XmlFileHandling.h"

using namespace std;

// *****************************************************************
//-------------------------------------------------------------------------------------------------
// Constructor & Destructor
//-------------------------------------------------------------------------------------------------
/**
 * Initialize and prepare the aggregated instances. 
 */
GestureManagement::GestureManagement() {
	init();
}

/**
 * Clean up and delete the the aggregated instances. 
 */
GestureManagement::~GestureManagement() {
	cleanUp();
}

// *****************************************************************
//-------------------------------------------------------------------------------------------------
// Methods
//-------------------------------------------------------------------------------------------------
/**
 * Put the given sensor data to the buffers. 
 */
void GestureManagement::updateBuffers(vector<vector<int> > rd) {
	for (int i=0; i<config->getInts("IDs").size(); ++i) {
		buffers.at(i)->filterPut(config->getInt("ID index"),config->getInts("IDs").at(i),rd);
	}
}

/**
 * Read data from the sensors and put it to the buffers. 
 */
void GestureManagement::updateBuffers() {
	updateBuffers(source->getData()); // getData() = blocking read
}

/**
 * Read and store data until the buffer is full. 
 */
void GestureManagement::fillBuffer() {
	cout << "    Filling buffer ... " << endl;
	bool done = false;
	while (!Helper::kbhit() && !done) {
		done = true;
		for (int i=0; i<NUMBER_OF_ROGGEN_SENSORS; ++i) {
			if (!buffers.at(i)->isFull()) {
				done = false;
			}
		}
		if (!done) {
			updateBuffers();
		}
	}
}

/**
 * Initialize the aggregated instances. 
 */
void GestureManagement::init() {
	config = Configuration::getInstance(CONFIG_FILE);
	rawSource = new RoggenSensorFusion(config->getStrings("sensor devices"),config->getString("sensor format"));
	source 	= new RoggenDataExtension(rawSource);
	for (int i=0; i<NUMBER_OF_ROGGEN_SENSORS; ++i) {
		buffers.push_back(new RoggenBuffer(config->getInt("energy frame size")));
		extr.push_back(new RoggenFeatureExtraction(buffers.at(i)));
		algs.push_back(new NearestClusterCenter());
		extr.at(i)->setIndices(config->getInts("sensor data indices"));
	}
	maxGestureLength = 0;
	fillBuffer();
}

/**
 * Delete the aggregated instances. 
 */
void GestureManagement::cleanUp() {
	safeDelete(source);
	safeDelete(rawSource);
	for (int i=0; i<NUMBER_OF_ROGGEN_SENSORS; ++i) {
		safeDelete(buffers.at(i));
		safeDelete(extr.at(i));
		safeDelete(algs.at(i));
	}
}

/**
 * Wait for the user hitting a key while still updating the buffer with current sensor data. 
 */
void GestureManagement::wait() {
	while (!Helper::kbhit()) {
		updateBuffers();
	}
}

/**
 * Wait until all sensors are inactive. 
 */
void GestureManagement::reenter() {
	cout << "    Waiting for all sensors to get inactive ..." << endl;
	updateBuffers();
	bool done = false;
	while (!done) {
		done = true;
		for (int i=0; i<NUMBER_OF_ROGGEN_SENSORS; ++i) {
			if (!extr.at(i)->lostEnergy(config->getInt("energy frame size"))) {
				done = false;
			}
		}
		updateBuffers();
	}
}

/**
 * Converts a twodimensional vector of integer data to IntData struct. 
 * IntData structs are used for saving data to XML files. 
 */
IntData GestureManagement::convert(vector<vector<int> > rd) {
	IntData ret;
	ret.name = "roggen data";
	int i = 0;
	for (vector<vector<int> >::iterator it=rd.begin(); it!=rd.end(); ++it) {
		IntStruct is;
		is.vals.clear();
		is.defs.clear();
		is.name.clear();
		is.vals = *it;
		is.num = i++;
		ret.dats.push_back(is);
	}
	return ret;
}

/**
 * Converts an IntData struct into a twodimensional vector of integer data. 
 * IntData structs are used for saving data to XML files. 
 */
vector<vector<int> > GestureManagement::convert(IntData id) {
	vector<vector<int> > ret;
	ret.clear();
	for (ISVec::iterator it=id.dats.begin(); it!=id.dats.end(); ++it) {
		ret.push_back(it->vals);
	}
	return ret;
}

/**
 * Recording a single gesture. 
 * Energy based segmentation is used. 
 * The gesture is saved in the buffers for sensor data. 
 */
bool GestureManagement::record() {
	for (int i=0; i<NUMBER_OF_ROGGEN_SENSORS; ++i) {
		buffers.at(i)->setLength(config->getInt("energy frame size"));
	}
	fillBuffer();
	cout << "    Recording automatically starts with movement (or space bar) " << endl;
	bool highEnergy = false;
	bool done = false;
	bool spaceHit = false;
	bool spaceHitAgain = false;
	while (!done) {
		updateBuffers();
		bool start = false;
		for (int i=0; i<NUMBER_OF_ROGGEN_SENSORS; ++i) {
			if (extr.at(i)->gotEnergy(config->getInt("energy frame size"))) {
				start = true;
			}
		}
		if (!highEnergy && (start || spaceHit)) {
			cout << "    recording starts" << endl;
			highEnergy = true;
			for (int i=0; i<NUMBER_OF_ROGGEN_SENSORS; ++i) {
				buffers.at(i)->setUnlimited();
			}
		}
		bool stop = true;
		for (int i=0; i<NUMBER_OF_ROGGEN_SENSORS; ++i) {
			if (!extr.at(i)->lostEnergy(config->getInt("energy frame size"))) {
				stop = false;
			}
		}
		if ((highEnergy && stop && !spaceHit) || spaceHitAgain) {
			highEnergy = false;
			cout << "    recording finished" << endl;	
			done = true;
		}
		if (Helper::kbhit()) {
			char key = Helper::getch();
			if (key == SPACE_KEY_CODE) { // space pressed
				if (spaceHit) {
					spaceHitAgain = true;
				}
				spaceHit = true;
			} else {
				done = true;
				return false;
			}
		}
	}
	return true;
}

/**
 * Method with user interaction. 
 * Records one take of a gesture. 
 */
void GestureManagement::RecordMovement(string name,int take) {
	cout << "--- RECORDING GESTURE ------------------------------------------- " << endl;
	cout << "    " << name << endl;
	cout << "    Take " << take << endl; 
	cout << "    Press a key when ready " << endl;
	wait();
	Helper::getch();
	record();
	IntData intD;
	RoggenBuffer tmp;
	for (int i=0; i<NUMBER_OF_ROGGEN_SENSORS; ++i) {
		tmp.put(buffers.at(i)->get()); // concat recordings of both sensors
	}
	intD = convert(tmp.get());
	intD.name = "recorded roggen data";
	cout << "    Got " << intD.dats.size() << " samples " << endl;
	cout << "    Writing data to file " << endl;
	XmlFileHandling::store(intD,XML_SUBDIRECTORY+name+Helper::toString(take)+".xml");
	cout << "--- DONE -------------------------------------------------------- " << endl;
}

/**
 * Method with user interaction. 
 * Records the given number of takes of a gesture with the given name. 
 */
void GestureManagement::RecordMovements(string name, int numberOfTakes) {
	for (int i=1; i<=numberOfTakes; ++i) {
		RecordMovement(name, i);
	}
}

/**
 * Method with user interaction. 
 * Records the configured number of takes of a gesture with the given name. 
 */
void GestureManagement::RecordMovements(string name) {
	RecordMovements(name,config->getInt("number of takes"));
}

/**
 * Returns true if all necessary files for training are available. 
 */
bool GestureManagement::checkAllFiles() {
	// check if all needed files exist
	bool filesOk = true;
	// string commands [NUMBER_OF_COMMANDS] = COMMAND_NAMES;
	// for (int f=FIRST_FILE; f<=LAST_FILE; ++f) { // 5 gestures
	for (int f=0; f<config->getStrings("classification names").size(); ++f) {
		// for (int i=1; i<=NUMBER_OF_TAKES; i++) {  // 3 takes each
		for (int i=1; i<=config->getInt("number of takes"); ++i) {
			string fname = XML_SUBDIRECTORY+config->getStrings("classification names").at(f)+Helper::toString(i)+".xml";
			if (!Helper::fileExists(fname.c_str())) {
				if (filesOk) {
					cout << "    " << fname << "Missing recordings:" << endl;						
					filesOk = false;
				}
				cout << "    " << fname << " not yet recorded" << endl;
			}
		}
	}
	return filesOk;
}

/**
 * Extracts the features from the buffers for the given sensor code. 
 * If useSpecSize is set to true only the last specSize frames in the buffer will be taken into account. 
 */
Features GestureManagement::extractFeatures(int sensor, int specSize, bool useSpecSize) {
	if (config->getInt("verbosity") > 2) {
		cout << "* FEATURE EXTRACTION *************" << endl;
		cout << "Sensor: " << sensor << endl;
	}
	Features f;
	f.clear();
	// for every measurement (accX, accY, ...)
	vector<int> indices = config->getInts("sensor data indices");
	
	int fullsize = buffers.at(sensor)->size();
	int size = fullsize;
	if (useSpecSize && fullsize > specSize) {
		size = specSize;
	}
	int offset = 0;
	if (useSpecSize) {
		offset = fullsize - size;
	}

	vector<int> meanPositions;
	meanPositions.clear();
	if (config->getInt("use mean") == 1) {
		int chunks = config->getInt("number of mean window chunks");
		for (int i=0; i<chunks; ++i) {
			meanPositions.push_back(offset+i*size/chunks);
		}
		meanPositions.push_back(fullsize-1);
	}

	vector<int> sdPositions;
	sdPositions.clear();
	if (config->getInt("use standard deviation") == 1) {
		int chunks = config->getInt("number of sd window chunks");
		for (int i=0; i<chunks; ++i) {
			sdPositions.push_back(offset+i*size/chunks);
		}
		sdPositions.push_back(fullsize-1);
	}

	vector<int> mcrPositions;
	mcrPositions.clear();
	if (config->getInt("use mean crossing rate") == 1) {
		int chunks = config->getInt("number of mcr window chunks");
		for (int i=0; i<chunks; ++i) {
			mcrPositions.push_back(offset+i*size/chunks);
		}
		mcrPositions.push_back(fullsize-1);
	}

	for (vector<int>::iterator it=indices.begin(); it!=indices.end(); ++it) {
		if (config->getInt("verbosity") > 2) {
			cout << "Index:  " << *it << endl;
		}

		if (config->getInt("use mean") == 1) {
			for (int i=1; i<meanPositions.size(); ++i) {
				float mean = extr.at(sensor)->getMean(*it,meanPositions.at(i-1),meanPositions.at(i));
				if (config->getInt("verbosity") > 2) {
					cout << "Mean:   " << mean << endl;
				}
				f.push_back(mean);
			}
		}

		if (config->getInt("use standard deviation") == 1) {
			for (int i=1; i<sdPositions.size(); ++i) {
				float sd = extr.at(sensor)->getStandardDeviation(*it,sdPositions.at(i-1),sdPositions.at(i));
				if (config->getInt("verbosity") > 2) {
					cout << "SD:     " << sd << endl;
				}
				f.push_back(sd);
			}
		}

		if (config->getInt("use mean crossing rate") == 1) {
			for (int i=1; i<mcrPositions.size(); ++i) {
				float mcr = extr.at(sensor)->getMeanCrossingRate(*it,mcrPositions.at(i-1),mcrPositions.at(i));			
				if (config->getInt("verbosity") > 2) {
					cout << "MCR:   " << mcr << endl;
				}
				f.push_back(mcr);
			}
		}
	} // for every measurement index (accX, ...)
	if (config->getInt("verbosity") > 2) {
		cout << "**********************************" << endl;
	}
	return f;
}

/**
 * Overloaded function for always using the whole buffer for classification. 
 */
Features GestureManagement::extractFeatures(int sensor) {
	return extractFeatures(sensor, 0, false);
}

/**
 * Overloaded function for always using the specified number of frames out of the buffer. 
 */
Features GestureManagement::extractFeatures(int sensor, int specSize) {
	return extractFeatures(sensor, specSize, true);
}

/**
 * Every gesture is recognized by just one sensor. 
 * Like this it's possible to recognize simultanous movements with different sensors. 
 * Therefore the information which sensor is used for which gesture is stored in an xml file. 
 * Use this method to load the activity from the file. 
 * The filename is defined in the configuration file. 
 */
void GestureManagement::loadActivity() {
	activity.clear();
	IntData id = XmlFileHandling::readIntData(XML_SUBDIRECTORY+config->getString("activity file"));
	if (id.name == "activity") {
		activity = id.dats.front().vals;
	} else {
		cerr << "invalid activity file" << endl;
		exit(1);
	}
}

/**
 * Every gesture is recognized by just one sensor. 
 * Like this it's possible to recognize simultanous movements with different sensors. 
 * Therefore the information which sensor is used for which gesture is stored in an xml file. 
 * The activity can be automatically determined or entered by hand. 
 * Use this method to save the activity to file. 
 * The filename is defined in the configuration file. 
 */
void GestureManagement::saveActivity() {
	IntData id;
	id.name = "activity";
	id.dats.clear();
	IntStruct is;
	is.vals.clear();
	is.defs.clear();
	is.name.clear();
	is.num = 0;
	is.vals = activity;
	id.dats.push_back(is);
	XmlFileHandling::store(id,XML_SUBDIRECTORY+config->getString("activity file"));
}

/**
 * Every gesture is recognized by just one sensor. 
 * Like this it's possible to recognize simultanous movements with different sensors. 
 * Therefore the information which sensor is used for which gesture is stored in an xml file. 
 * This method automatically determines the most active sensor in terms of variance measurements 
 * for each gesture. 
 */
void GestureManagement::determineActivity() {
	activity.clear();
	for (int f=0; f<NUMBER_OF_CLASSIFICATIONS; ++f) {
		cout << "Determining activity for gesture " << f << endl;
		vector<float> varianceSums; // compute sums for every sensor within a gesture
		varianceSums.clear();
		for (int i=0; i<NUMBER_OF_ROGGEN_SENSORS; i++) {
			varianceSums.push_back(0); // inital value
		}
		for (int i=1; i<=config->getInt("number of takes"); ++i) {
			string fname = XML_SUBDIRECTORY+config->getStrings("classification names").at(f)+Helper::toString(i)+".xml";
			if (!Helper::fileExists(fname.c_str())) {
				cerr << fname << " not yet recorded!" << endl;
				return;
			} else {
				vector<vector<int> > rd = convert(XmlFileHandling::readIntData(fname));
				for (int sensor=0; sensor<NUMBER_OF_ROGGEN_SENSORS; ++sensor) { // for every sensor
					buffers.at(sensor)->clear();
					buffers.at(sensor)->setUnlimited();
					buffers.at(sensor)->put(RoggenBuffer::filter(0,config->getInts("IDs").at(sensor),rd));
					// for every measurement (accX, accY, ...)
					vector<int> indices = config->getInts("sensor data indices");
					for (vector<int>::iterator it=indices.begin(); it!=indices.end(); ++it) { 
						varianceSums.at(sensor) += extr.at(sensor)->getVariance(*it);
					} // for every measurement (accX, ...)
				} // for every sensor
			} // if fileexists
		} // for every take
		float maxVariance = 0;
		int maxSensor = -1;
		// find sensor with the largest variance sum
		for (int sensor=0; sensor<NUMBER_OF_ROGGEN_SENSORS; ++sensor) { // for every sensor
			if (varianceSums.at(sensor) > maxVariance) {
				maxVariance = varianceSums.at(sensor);
				maxSensor = sensor;
			}
		}
		activity.push_back(maxSensor);
		if (config->getInt("verbosity") > 1) {
			cout << "Gesture: " << f << " has sensor ";
			cout << maxSensor << " active " << endl;			
		}
	}	// for every gesture
	cout << "Energy detection done" << endl;
	saveActivity();
	cout << "Saving done" << endl;
}

/**
 * Method with user interaction. 
 * Every gesture is recognized just using one sensor. 
 * Like this it's possible to recognize simultanous movements with different sensors. 
 * Therefore the information which sensor is used for which gesture is stored in an xml file. 
 * This method allows to edit or enter the activity by hand. 
 */
void GestureManagement::editActivity() {
	loadActivity();
	if (activity.size() < NUMBER_OF_CLASSIFICATIONS) {
		cout << "    No activity stored yet ... presetting activity" << endl;
		determineActivity();
	}
	for (int i=0; i<activity.size(); ++i) {
		cout << "    Gesture " << i << " (" << config->getStrings("classification names").at(i) << ") has sensor " << activity.at(i) << " active" << endl;
		for (int sensor=0; sensor<NUMBER_OF_ROGGEN_SENSORS; ++sensor) {
			cout << "    Type " << sensor << " to set sensor " << sensor << " active for gesture " << i << endl;
		}
		cout << "    Type space to keep the current setting" << endl;
		char c = Helper::getch();
		int decint = static_cast<int>(c); 
		if (c != SPACE_KEY_CODE) {
			if (c == ESC_KEY_CODE) {
				cout << "    Esc hit, leaving" << endl;
				loadActivity();
				return;
			}
			decint -= NUMBER_ZERO_KEY;
			if (decint >= 0 && decint < NUMBER_OF_ROGGEN_SENSORS) {
				activity.at(i) = decint;
			} else {
				cout << "    invalid key" << endl;
			}
		}
	}
	saveActivity();
}

/**
 * Every gesture is recognized by using one sensor. 
 * Like this it's possible to recognize simultanous movements with different sensors. 
 * Therefore the information which sensor is used for which gesture is stored in an xml file. 
 * This method prints the current activity information. 
 */
void GestureManagement::printActivity() {
	loadActivity();
	for (int i=0; i<activity.size(); ++i) {
		cout << "    Gesture " << i << " (" << config->getStrings("classification names").at(i) << ") has sensor " << activity.at(i) << " active" << endl;
	}
}

/**
 * Reads the recorded gesture data from the files and brings it in the format ready for training. 
 */
vector<FeatureSet> GestureManagement::readData() {
	// read all takes for each gesture
	vector<FeatureSet> sets;
	sets.clear();
	for (int i=0; i<=NUMBER_OF_ROGGEN_SENSORS; ++i) {
		FeatureSet s;
		s.clear();
		sets.push_back(s);
	}
	for (int f=0; f<config->getStrings("classification names").size(); ++f) {
		if (config->getInt("verbosity") > 1) {
			cout << "Training classification name number " << f << endl;
		}
		// cout << "Gesture: " << f << endl;
		vector<float> varianceSums; // compute sums for every sensor within a gesture
		varianceSums.clear();
		for (int i=0; i<NUMBER_OF_ROGGEN_SENSORS; i++) {
			varianceSums.push_back(0); // inital value
		}
		int sizeSum = 0;
		// for (int i=1; i<=NUMBER_OF_TAKES; i++) {  // for each of the 3 takes
		for (int i=1; i<=config->getInt("number of takes"); ++i) {
			// cout << "Take " << i << endl;
			string fname = XML_SUBDIRECTORY+config->getStrings("classification names").at(f)+Helper::toString(i)+".xml";
			// cout << "Filename: " << fname << endl;
			if (!Helper::fileExists(fname.c_str())) {
				cerr << fname << " not yet recorded!" << endl;
				cerr << "No training possible" << endl;
				sets.clear();
				return sets;
			} else {
				vector<vector<int> > rd = convert(XmlFileHandling::readIntData(fname));
				for (int sensor=0; sensor<NUMBER_OF_ROGGEN_SENSORS; ++sensor) { // for every sensor
					// cout << "Sensor " << sensor << endl;
					buffers.at(sensor)->clear();
					buffers.at(sensor)->setUnlimited();
					buffers.at(sensor)->put(RoggenBuffer::filter(0,config->getInts("IDs").at(sensor),rd));
					maxGestureLength = Helper::getMax(maxGestureLength,buffers.at(sensor)->size());
					FeatureSample fs;
					fs.out = f;
					fs.in.clear();
					fs.in = extractFeatures(sensor);
					sets.at(sensor).push_back(fs);
					int size = buffers.at(sensor)->size();
					sizeSum += size;
				} // for every sensor
			} // if fileexists
		} // for every take
		sizeSum /= config->getInt("number of takes") * NUMBER_OF_ROGGEN_SENSORS; // compute average size of one gesture
		sizes.push_back(sizeSum); // store sizes (averages)
	}	// for every gesture
	return sets;
}

/**
 * Every gesture is recognized by using one sensor. 
 * Like this it's possible to recognize simultanous movements with different sensors. 
 * Therefore the information which sensor is used for which gesture is stored in an xml file. 
 * This method removes data of inactive sensors from the given training set. 
 */
vector<FeatureSet> GestureManagement::removeInactive(vector<FeatureSet> sets) {
	// remove inactive sensor measurements from the training sets
	if (config->getInt("verbosity") > 2) {
		for (int i=0; i<NUMBER_OF_ROGGEN_SENSORS; ++i) {
			cout << "Display set " << i << " = ";
			for (FeatureSet::iterator it=sets.at(i).begin(); it!=sets.at(i).end(); ++it) {
				cout << it->out << "; ";			
			}
			cout << endl;
		}
	}
	int sensorToRemove = -1;
	FeatureSet::iterator toRemove;
	bool done = false;
	while (!done) {
		done = true;
		for (int sensor=0; sensor<NUMBER_OF_ROGGEN_SENSORS; ++sensor) {
			for (FeatureSet::iterator it=sets.at(sensor).begin(); it!=sets.at(sensor).end(); ++it) { 
				// cout << "A " << it->out << endl;
				if (activity.at(it->out) != sensor) {
					// cout << "set to remove" << endl;
					toRemove = it;
					sensorToRemove = sensor;
					done = false;
				}
			}
		}
		if (!done) {
			if (config->getInt("verbosity") > 2) {
				cout << "removing sensor " << sensorToRemove << " of gesture " << toRemove->out << endl;
			}
			sets.at(sensorToRemove).erase(toRemove);
		}
		if (config->getInt("verbosity") > 3) {
			for (int i=0; i<NUMBER_OF_ROGGEN_SENSORS; ++i) {
				cout << "Display set " << i << " = ";
				for (FeatureSet::iterator it=sets.at(i).begin(); it!=sets.at(i).end(); ++it) {
					cout << it->out << "; ";			
				}
				cout << endl;
			}
		}
	}
	return sets;
}

/**
 * This method actually trains the algorithm using the given data. 
 */
void GestureManagement::train(vector<FeatureSet> sets) {
	// Train one ncc per sensor
	for (int i=0; i<NUMBER_OF_ROGGEN_SENSORS; ++i) {
		if (config->getInt("verbosity") > 1) {
			cout << "Display set " << i << " = ";
			for (FeatureSet::iterator it=sets.at(i).begin(); it!=sets.at(i).end(); ++it) {
				cout << it->out << "; ";			
			}
			cout << endl;
		}
		algs.at(i)->train(sets.at(i));
		algs.at(i)->setUnknownThreshold(algs.at(i)->getUnknownThreshold() * config->getInt("classification threshold percentage") / 100.0);
		if (config->getInt("verbosity") > 2) {
			algs.at(i)->print();
		}
	}
}

/**
 * High level method calls all necessary methods for training the algorithm. 
 * First the gesture data is loaded from disk. 
 * Then inactive sensors are removed and the algorithm gets trained. 
 */
bool GestureManagement::training() {
	cout << "    Training starts ..." << endl;
	sizes.clear();
	loadActivity();
	vector<FeatureSet> sets;
	sets.clear();
	sets = readData();
	if (sets.size() > 0) {
		sets = removeInactive(sets);
		train(sets);
		cout << "    ... Training finished" << endl;
		return true;
	} else {
		return false;
	}
}

// *****************************************************************

// continous classification
/**
 * Method for continous classification. 
 * There is no energy based segmentation used. 
 * For every gesture there is the average length of the training samples stored. 
 * Given this data parts of the buffer are extracted und continously fed to the classification. 
 */
void GestureManagement::classifyContinous() {
	for (int sensor=0; sensor<NUMBER_OF_ROGGEN_SENSORS; ++sensor) {
		buffers.at(sensor)->setLength(maxGestureLength+1);
	}
	fillBuffer();
	bool done = false;
	// vector<pair<OutputType,FeatureType> > results;
	if (activity.size() != sizes.size()) {
		cerr << "activity matrix and size matrix sizes mismatched! Activity: " << activity.size() << " Sizes: " << sizes.size() << endl; 
	}
	while (!done) {
		// results.clear();
		// cout << "Number of gestures: " << sizes.size() << " **************************" << endl;
		for (int i=0; i<sizes.size(); ++i) {
			// cout << "gesture number: " << i << endl;
			int sensor = activity.at(i);
			// cout << "sensor: " << sensor << endl;
			updateBuffers();
			pair<Output,FeatureType> p = algs.at(sensor)->classification(extractFeatures(sensor,sizes.at(i)));
			if (p.first.valid) {
				if (p.first.value == i) {
					cout << "    Found classification: " << i << endl;
				}
			}
		}
		if (Helper::kbhit()) {
			Helper::getch();
			done = true;
		}
	}
}

/**
 * Method for segmented classification. 
 * Therefore energy based segmentation is used. 
 * As soon as a sensor gets active (variance above threshold) captering starts. 
 * When the sensor gets inactive again the captured data is converted und fed to the classification. 
 * Segmented classification produces much less workload then continous classification. 
 * This method is just used for testing in recordgestures. 
 * For the classification used in supertux and testclassification see the code in the class ClassificationTux. 
 */
void GestureManagement::classifySegmented() {
	vector<bool> highEnergy;
	bool done = false;
	for (int sensor=0; sensor<NUMBER_OF_ROGGEN_SENSORS; ++sensor) {
		buffers.at(sensor)->setLength(config->getInt("energy frame size"));
		highEnergy.push_back(false);
	}
	fillBuffer();
	cout << "    Recording automatically starts with movement" << endl;
	while (!done) {
		updateBuffers();
		for (int sensor=0; sensor<NUMBER_OF_ROGGEN_SENSORS; ++sensor) {
			if (!highEnergy.at(sensor) && buffers.at(sensor)->isFull() && extr.at(sensor)->gotEnergy(config->getInt("energy frame size"))) {
				buffers.at(sensor)->setUnlimited();
				highEnergy.at(sensor) = true;
				if (config->getInt("verbosity") > 0) {
					cout << "    recording " << sensor << " starts" << endl;
				}
			}
			if (highEnergy.at(sensor) && extr.at(sensor)->lostEnergy(config->getInt("energy frame size"))) {
				highEnergy.at(sensor) = false;
				if (config->getInt("verbosity") > 0) {
					cout << "    recording " << sensor << " finished" << endl;	
				}
				if (Helper::has(activity,sensor)) {
					// classify ...
					pair<Output,FeatureType> p = algs.at(sensor)->classification(extractFeatures(sensor));
					if (config->getInt("compare gesture length") == 1) {
						bool lengthCheck = false;
						int lengthIs = buffers.at(sensor)->size();
						int lengthShould = sizes.at(p.first.value);
						float glTol = config->getInt("gesture length tolerance percentage")/100.0;
						if (lengthIs < lengthShould*(1+glTol) && lengthIs > lengthShould*(1-glTol)) {
							lengthCheck = true;
						} else {
							if (config->getInt("verbosity") > 1) {
								cout << "Length check failed. Is: " << lengthIs << "; Should: " << lengthShould << endl;
							}
							p.first.valid = false;
						}
					}
					if (config->getInt("verbosity") > 1) {
						if (p.first.valid) {
							cout << "Classification is valid: " << endl;
						} else {
							cout << "Classification is INVALID" << endl;
						}
						cout << "Distance: " << p.second << endl;
						// print out percentage of distance in relation to the threshold		
						cout << "Percent invalid: " << static_cast<int>(p.second/algs.at(sensor)->getUnknownThreshold()*100.0) << endl;
						cout << "Sensor: " << sensor << " did " << p.first.value << endl;
					} else {
						if (p.first.valid) {
							cout << "    Sensor: " << sensor << " did " << p.first.value << endl;
						}
					}
				} else {
					cout << "Untrained sensor used! No classification possible!" << endl;
				}
				buffers.at(sensor)->setLength(config->getInt("energy frame size"));
			}		
		}
		if (Helper::kbhit()) {
			Helper::getch();
			done = true;
		}
	}
}

// *****************************************************************

/**
 * If continous or segmented classification is used can be set in the configuration file. 
 * This method checks the configuration and runs the according classification to 
 * test the classification by command line output. 
 * This method is just used for testing in recordgestures. 
 * For the classification used in supertux and testclassification see the code in the class ClassificationTux. 
 */
void GestureManagement::playing() {
	cout << "    Playing ..." << endl;
	if (config->getInt("continous classification") == 1) {
		classifyContinous();
	} else {
		classifySegmented();
	}
	cout << "    ... Playing done" << endl;
}

/**
 * Method with user interaction. 
 * Displays the main menu for the recordgestures program and prompts the user input. 
 */
int GestureManagement::mainMenu(int defaultDecision) {
	cout << "--- MAIN MENU --------------------------------------------------- " << endl;
	for (int i=0; i<config->getStrings("classification names").size(); ++i) {
		cout << "    " << i  << ": Record gesture \"" << config->getStrings("classification names").at(i) << "\"" << endl;
	}
	cout << "    " << TEST_MODE << ": Test classification" << endl;
	cout << "    " << PRINT_ACTIVITY << ": Print activity data" << endl; 
	cout << "    " << AUTOSET_ACTIVITY << ": Autoset activity data" << endl;
	cout << "    " << CHANGE_ACTIVITY << ": Edit activity data" << endl;
	cout << "    " << EXIT << ": Exit" << endl;
	cout << "    " << "Default: " << defaultDecision << endl;
	cout << "    " << ">>> " << flush;
	int decint = defaultDecision;
	if (EXIT < 10) {
		wait();
		char c = Helper::getch();
		decint = static_cast<int>(c); 
		if (c == 'q' || c == 'Q' || c == ESC_KEY_CODE) {
			decint = EXIT;
		} else {
			decint -= NUMBER_ZERO_KEY;
		}
		cout << endl;
	} else {
		// TODO test software with 6 gestures or more
		cin >> decint; // finish number with enter
	}
	if (decint > EXIT || decint < 0) {
		decint = defaultDecision;
		cout << "    invalid key" << endl;
	}
	cout << "--- EXECUTING --------------------------------------------------- " << endl;
	return decint;
}

/**
 * Method used for the recordgestures program. 
 * Shows the main menu and calls the requested methods. 
 */
void GestureManagement::mainLoop() {
	int decision = TEST_MODE;
	while (decision != EXIT) {
		decision = mainMenu(decision);
		if (decision == TEST_MODE && checkAllFiles()) {
			if (training()) {
				playing();
			}
		} else if (decision == CHANGE_ACTIVITY) {
			editActivity();
			decision = PRINT_ACTIVITY;
		} else if (decision == PRINT_ACTIVITY) {
			printActivity();
			decision = TEST_MODE;
		} else if (decision == AUTOSET_ACTIVITY) {
			determineActivity();
			decision = PRINT_ACTIVITY;
		} else if (decision < NUMBER_OF_CLASSIFICATIONS) {
			RecordMovements(config->getStrings("classification names").at(decision));
			decision += 1; // adapt default decision
		}
	}
}

/**
 * Overloaded function with the testing mode as default decision for the main menu. 
 */
int GestureManagement::mainMenu() {
	return mainMenu(TEST_MODE);
}

/**
 * Highest level function. 
 * Calls mainLoop. 
 */
void GestureManagement::manageGestures() {
	cout << "--- STARTING ---------------------------------------------------- " << endl;
	mainLoop();
	cout << "--- EXIT -------------------------------------------------------- " << endl;
}

// *****************************************************************

/**
 * Method used by the ClassificationTux class. 
 * Returns the number of sensors used as defined in the configuration file.  
 */
int GestureManagement::numberOfSensors() {
	return NUMBER_OF_ROGGEN_SENSORS;
}

/**
 * Method used by the ClassificationTux class. 
 * Sets the buffer of the given sensor to the size for energy based segmentation. 
 * This size can be configured in the configuration file. 
 */
void GestureManagement::setBufferToEnergyFrameSize(int sensor) {
	buffers.at(sensor)->setLength(config->getInt("energy frame size"));
}

/**
 * Method used by the ClassificationTux class. 
 * Sets the buffer of the given sensor to the size long enough 
 * that even the longest recorded gesture fits inside. 
 * Before this method readData or training have to be called. 
 */
void GestureManagement::setBufferToLongestGestureSize(int sensor) {
	buffers.at(sensor)->setLength(maxGestureLength+1);
}

/**
 * Method used by the ClassificationTux class. 
 * Returns true if the buffer of the given sensor 
 * is full and servers as a queue. 
 */
bool GestureManagement::bufferFull(int sensor) {
	return buffers.at(sensor)->isFull();
}

/**
 * Method used by the ClassificationTux class. 
 * Sets all buffers to the size for energy based segmentation. 
 * This size can be configured in the configuration file. 
 */
void GestureManagement::setBufferToEnergyFrameSize() {
	for (int sensor=0; sensor<NUMBER_OF_ROGGEN_SENSORS; ++sensor) {
		setBufferToEnergyFrameSize(sensor);
	}
}

/**
 * Method used by the ClassificationTux class. 
 * Sets all buffers to the size long enough 
 * that even the longest recorded gesture will fit inside. 
 * Before this method readData or training have to be called. 
 */
void GestureManagement::setBufferToLongestGestureSize() {
	for (int sensor=0; sensor<NUMBER_OF_ROGGEN_SENSORS; ++sensor) {
		setBufferToLongestGestureSize(sensor);
	}
}

/**
 * Method used by the ClassificationTux class. 
 * Returns true if all buffers are full and serve as a queue. 
 */
bool GestureManagement::bufferFull() {
	for (int sensor=0; sensor<NUMBER_OF_ROGGEN_SENSORS; ++sensor) {
		if (!bufferFull(sensor)) {
			return false;
		}
	}
	return true;
}

/**
 * Method used by the ClassificationTux class. 
 * Returns true when the given sensor (0, 1, ...) is active. 
 */
bool GestureManagement::gotEnergy(int sensor) {
	return extr.at(sensor)->gotEnergy(config->getInt("energy frame size"));
}

/**
 * Method used by the ClassificationTux class. 
 * Returns true when the given sensor (0, 1, ...) is inactive. 
 */
bool GestureManagement::lostEnergy(int sensor) {
	return extr.at(sensor)->lostEnergy(config->getInt("energy frame size"));
}

/**
 * Method used by the ClassificationTux class. 
 * Returns true when all sensors (0, 1, ...) are active. 
 */
bool GestureManagement::gotEnergy() {
	for (int sensor=0; sensor<NUMBER_OF_ROGGEN_SENSORS; ++sensor) {
		if (gotEnergy(sensor)) {
			return true;
		}
	}
	return false;
}

/**
 * Method used by the ClassificationTux class. 
 * Returns true when all sensors (0, 1, ...) are inactive. 
 */
bool GestureManagement::lostEnergy() {
	for (int sensor=0; sensor<NUMBER_OF_ROGGEN_SENSORS; ++sensor) {
		if (!lostEnergy(sensor)) {
			return false;
		}
	}
	return true;
}

/**
 * Method used by the ClassificationTux class. 
 * Sets the buffer of the given sensor (0,1,...) to 
 * unlimited size. 
 * So it will never remove frames from the queue. 
 */
void GestureManagement::setBufferUnlimited(int sensor) {
	buffers.at(sensor)->setUnlimited();
}

/**
 * Method used by the ClassificationTux class. 
 * Sets all buffers to unlimited size. 
 * So they will never remove frames from the queue.  
 */
void GestureManagement::setBufferUnlimited() {
	for (int sensor=0; sensor<NUMBER_OF_ROGGEN_SENSORS; ++sensor) {
		setBufferUnlimited(sensor);
	}
}

/**
 * Method used by the ClassificationTux class. 
 * Returns true if the given sensor is used for recognition and has been trained. 
 */
bool GestureManagement::isSensorTrained(int sensor) {
	return Helper::has(activity,sensor);
}

/**
 * Method used by the ClassificationTux class. 
 * Calls the classification function of the learning algorithm. 
 */
pair<Output,FeatureType> GestureManagement::classify(int sensor) {
	return algs.at(sensor)->classification(extractFeatures(sensor));
}

/**
 * Method used by the ClassificationTux class. 
 * Calls the classification function of the learning algorithm for the size of the given gesture. 
 * This method is used for continous classification. 
 */
pair<Output,FeatureType> GestureManagement::classify(int sensor, int gesture) {
	return algs.at(sensor)->classification(extractFeatures(sensor,sizes.at(gesture)));
}

/**
 * Method used by the ClassificationTux class. 
 * Checks if the gesture length we got by energy based segmentation is in the same range
 * as the recorded movements for this gestures. 
 * This method is used for segmented classification. 
 */
bool GestureManagement::isGestureLengthOk(int sensor, int gesture) {
	if (config->getInt("compare gesture length") == 1) {
		int lengthIs = buffers.at(sensor)->size();
		int lengthShould = sizes.at(gesture);
		float glTol = config->getInt("gesture length tolerance percentage")/100.0;
		if (lengthIs < lengthShould*(1+glTol) && lengthIs > lengthShould*(1-glTol)) {
			return true;
		}
		return false;
	}
	return true;
}

/**
 * Method used by the ClassificationTux class. 
 * Returns the number of the sensor for a given gesture number. 
 * Both are numbered internally from zero onwards. 
 * Basically this method accesses the activity information. 
 * For a given gesture it tells which sensor is the active one. 
 */
int GestureManagement::sensorOfGesture(int gesture) {
	return activity.at(gesture);
}

/**
 * Method used by the ClassificationTux class. 
 * Returns true if the mode is set to continous classification. 
 * This can be set in the configuration file. 
 */
bool GestureManagement::continousMode() {
	return (config->getInt("continous classification") == 1);
}

/**
 * Method used by the ClassificationTux class. 
 * Returns the number of gestures. 
 * Gestures are numbered from zero to number of gestures - 1. 
 */
int GestureManagement::numberOfGestures() {
	if (activity.size() != sizes.size()) {
		cerr << "activity matrix and size matrix sizes mismatched! Activity: " << activity.size() << " Sizes: " << sizes.size() << endl; 
	}
	return activity.size();
}

// *****************************************************************

