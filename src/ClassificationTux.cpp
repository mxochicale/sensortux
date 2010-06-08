#include "ClassificationTux.h"

#include "Configuration.h"
#include "Helper.h"
#include "Repercussion.h"

#define KEY_CURRENTLY_PRESSED true
#define KEY_CURRENTLY_RELEASED false
#define DEFAULT -1
#define REPERCUSSION_INCREMENT Configuration::getInstance(CONFIG_FILE)->getInt("repercussion")

using namespace std;

//-------------------------------------------------------------------------------------------------
// Constructor & Destructor
//-------------------------------------------------------------------------------------------------
/**
 * Constructor: Initializes the object and all its aggregations. 
 * The class is a decendant from TuxControl. 
 * Uses the LearningAlgorithm implementation NearestClusterCenter to control super tux
 * by gesture recognition with sensors. 
 * For the classification running in parallel a thread is started. 
 */
ClassificationTux::ClassificationTux() {
	cout << "Initialize Classification" << endl;
	lastTime = time(NULL);
	counter = 0;
	/* initialize random seed: */
	srand(lastTime);
	highEnergy.clear();

	geMan = new GestureManagement();
	for (int sensor=0; sensor<geMan->numberOfSensors(); ++sensor) {
		highEnergy.push_back(false);
	}
	
	cout << "Training" << endl;
	if (!geMan->checkAllFiles()) {
		cerr << "Terminating because of missing files" << endl;
		cerr << "Please use recordgestures to prepare all the gesture files" << endl;
		exit(1);
	}
	geMan->training();

	if (Configuration::getInstance(CONFIG_FILE)->getInt("verbosity")>0) cout << "Preparing Buffer" << endl;
	if (geMan->continousMode()) {
		geMan->setBufferToLongestGestureSize();
	} else {
		geMan->setBufferToEnergyFrameSize();
	}
	geMan->fillBuffer();
	geMan->updateBuffers();

	repercussion = new Repercussion(geMan->numberOfGestures());
	repercussion->resetAll();
	repercussion->setDelay(1000);
	repercussion->setDecrement(1);
	repercussion->setMaximum(500);
	repercussion->activate();
	
	currentlyPressed.clear();
	for (int i=0; i<geMan->numberOfGestures(); ++i) {
		currentlyPressed.push_back(false);
	}

	if (Configuration::getInstance(CONFIG_FILE)->getInt("verbosity")>0) cout << "Starting Thread" << endl;
	startThread(); // start thread 

	cout << "Initializing Classification done" << endl;
}

/**
 * Stops the classification thread and cleans up. 
 */
ClassificationTux::~ClassificationTux() {
	stopThread(); // wait for thread to join
	safeDelete(geMan);
	safeDelete(repercussion);
}

//-------------------------------------------------------------------------------------------------
// Methods
//-------------------------------------------------------------------------------------------------
/**
 * Adds a final command for super tux to the queue. 
 * The queue is used as a shared memory secured by mutual exclusions. 
 */
void ClassificationTux::save(ControlEvent ce) {
	// cout << " &  Adding control event" << endl;
	addControlEvent(ce);
	// cout << " &  done" << endl;
}

/**
 * Helper function to convert between the input style of data and the output format. 
 */
void ClassificationTux::convert(int code, bool pressed) {
	// cout << " &  conversion" << endl;
	ControlEvent ce;
	ce.valid = true;
	ce.key = DEFAULT;
	ce.event = DEFAULT;
	vector<string> cnames = Configuration::getInstance(CONFIG_FILE)->getStrings("classification names");
	if        (cnames.at(code) == "left")  {
		ce.key = PLAYER_LEFT;
	} else if (cnames.at(code) == "right") {
		ce.key = PLAYER_RIGHT;
	} else if (cnames.at(code) == "jump")  {
		ce.key = PLAYER_UP;
	} else if (cnames.at(code) == "fire")  {
		ce.key = PLAYER_FIRE;
	} else if (cnames.at(code) == "duck")  {
		ce.key = PLAYER_DOWN;
	} else {
		cerr << "Could not match: " << cnames.at(code) << " = " << code << endl;
	}
	if (pressed) {
		ce.event = KEY_PRESSED;
	} else {
		ce.event = KEY_RELEASED;
	}
	// cout << " &  conversion done, saving ..." << endl;
	save(ce);	
	// cout << " &  saving ... done" << endl;
}

/**
 * Helper function: For control super tux uses key events like pressed or released. 
 * Given our classification results we increment an according self decrementing value. 
 * Based on the set of values the evaluate method finally generates the desired key events. 
 */
void ClassificationTux::extractChanges(int value, bool valid) {
	if (valid) {
		// use Repercussion
		repercussion->inc(value,REPERCUSSION_INCREMENT);
	}
}

/**
 * Prepared filter function for the classification results. 
 * Currently there is no filtering applied. 
 */
void ClassificationTux::filter(int value, bool valid) {
	// no filtering for now
	if (valid && Configuration::getInstance(CONFIG_FILE)->getInt("verbosity")>1) {
		cout << "VALID: " << value << endl;
	}
	extractChanges(value,valid);	
}

/**
 * Work method to classify in a continous manner. 
 * There is no energy based segmentation used. 
 * Instead the buffer is continously classified. 
 */
void ClassificationTux::continous() {
	for (int i=0; i<geMan->numberOfGestures(); ++i) {
		int sensor = geMan->sensorOfGesture(i);
		geMan->updateBuffers();
		pair<Output,FeatureType> p = geMan->classify(sensor,i);
		if (p.first.value == i) {
			// Pass on the result
			filter(p.first.value,p.first.valid);
		}
	}
}

/**
 * Work method to classify in a segmented manner. 
 * There is energy based segmentation used. 
 */
void ClassificationTux::segmented() {
	geMan->updateBuffers();
	for (int sensor=0; sensor<geMan->numberOfSensors(); ++sensor) {
		if (!highEnergy.at(sensor) && geMan->bufferFull(sensor) && geMan->gotEnergy(sensor)) {
			geMan->setBufferUnlimited(sensor);
			highEnergy.at(sensor) = true;
		}
		if (highEnergy.at(sensor) && geMan->lostEnergy(sensor)) {
			highEnergy.at(sensor) = false;
			if (geMan->isSensorTrained(sensor)) {
				pair<Output,FeatureType> p = geMan->classify(sensor);
				if (!geMan->isGestureLengthOk(sensor, p.first.value)) {
					p.first.valid = false;
				}
				// Pass on the result
				filter(p.first.value,p.first.valid);
			}
			geMan->setBufferToEnergyFrameSize(sensor);
		}
	}
}

/**
 * For control super tux uses key events like pressed or released. 
 * Given our classification results extraceChanges increments a self decrementing value assigned to the according command. 
 * Based on the set of (self decrementing) values this method generates the desired key events. 
 * The set of values is managed by a Repercussion instance. 
 * If the value e.g. for walking right reaches zero a release event is issued for the walking right key. 
 * If it goes above zero a pressed event is issued. 
 * Everytime a gesture is recognized the according value is incremented. 
 * There is a maximum value for the values define in the constructor when initializing the Repercussion instance. 
 */
void ClassificationTux::evaluate() {
	for (int i=0; i<geMan->numberOfGestures(); ++i) {
		if (currentlyPressed.at(i) && repercussion->isZero(i)) {
			currentlyPressed.at(i) = false;
			if (Configuration::getInstance(CONFIG_FILE)->getInt("verbosity")>1) cout << " _  key release event " << i << endl;
			convert(i, false);
		} else if (!currentlyPressed.at(i) && !repercussion->isZero(i)) {
			currentlyPressed.at(i) = true;
			if (Configuration::getInstance(CONFIG_FILE)->getInt("verbosity")>1) cout << " _  key press event   " << i << endl;
			convert(i, true);
		} 
	}
}

/**
 * Actual thread method calling a classification method like continous or segmented and evaluate to generate the key events. 
 */
void ClassificationTux::threadMethod() {
	while (!stopped()) {
		if (geMan->continousMode()) {
			continous();
		} else {
			segmented();
		}
		evaluate();
	}
}

