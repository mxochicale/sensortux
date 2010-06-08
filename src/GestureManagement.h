/**
 * Class written for the purpose of handling gestures on a relatively high level. 
 * For the use with learning algorithms gestures need to be recorded, saved and loaded. 
 * As well as live gestures need to be captured and prepared for classification. 
 * The function manageGestures of this class make heavy use of user interaction 
 * and command line based menus. 
 * Author: Lars Widmer, www.lawi.ch
 */

#ifndef _GESTURE_MANAGEMENT_H_
#define _GESTURE_MANAGEMENT_H_

#include <iostream>
#include <vector>

#include "IntSource.h"
#include "Configuration.h"
#include "LearningAlgorithm.h"
#include "RoggenFeatureExtraction.h"

#define CONFIG_FILE "config.xml"
#define SPACE_KEY_CODE 32
#define NUMBER_ZERO_KEY 48
#define ESC_KEY_CODE 27
#define XML_SUBDIRECTORY config->getString("xml subdirectory")
#define NUMBER_OF_ROGGEN_SENSORS config->getInts("IDs").size()
#define NUMBER_OF_CLASSIFICATIONS config->getStrings("classification names").size()
#define TEST_MODE config->getStrings("classification names").size()
#define PRINT_ACTIVITY config->getStrings("classification names").size()+1
#define AUTOSET_ACTIVITY config->getStrings("classification names").size()+2
#define CHANGE_ACTIVITY config->getStrings("classification names").size()+3
#define EXIT config->getStrings("classification names").size()+4

class GestureManagement {

private:
	IntSource * source;
	IntSource * rawSource;
	Configuration * config;
	std::vector<LearningAlgorithm*> algs;
	std::vector<RoggenBuffer*> buffers;
	std::vector<RoggenFeatureExtraction*> extr;
	std::vector<int> sizes;
	std::vector<int> activity;
	int maxGestureLength;

	void updateBuffers(std::vector<std::vector<int> > rd);
	IntData convert(std::vector<std::vector<int> > rd);
	std::vector<std::vector<int> > convert(IntData id);
	bool record();
	void RecordMovement(std::string name,int take);
	void RecordMovements(std::string name, int numberOfTakes);
	void RecordMovements(std::string name);
	Features extractFeatures(int sensor, int specSize, bool useSpecSize);
	Features extractFeatures(int sensor);
	Features extractFeatures(int sensor, int specSize);
	void loadActivity();
	void saveActivity();
	void determineActivity();
	void editActivity();
	void printActivity();
	std::vector<FeatureSet> readData();
	std::vector<FeatureSet> removeInactive(std::vector<FeatureSet> sets);
	void train(std::vector<FeatureSet> sets);
	void classifyContinous();
	void classifySegmented();
	void playing();
	int mainMenu(int defaultDecision);
	void mainLoop();
	int mainMenu();

	void init();
	void cleanUp();

public:
	GestureManagement();
	virtual ~GestureManagement();

	bool checkAllFiles();
	void updateBuffers();
	void fillBuffer();
	bool training();
	void wait();
	void reenter();

	int numberOfSensors();
	void setBufferToEnergyFrameSize(int);
	void setBufferToEnergyFrameSize();
	void setBufferToLongestGestureSize(int);
	void setBufferToLongestGestureSize();
	bool bufferFull(int);
	bool bufferFull();
	bool gotEnergy(int);
	bool gotEnergy();
	bool lostEnergy(int);
	bool lostEnergy();
	void setBufferUnlimited(int);
	void setBufferUnlimited();
	bool isSensorTrained(int);
	std::pair<Output,FeatureType> classify(int);
	std::pair<Output,FeatureType> classify(int, int);
	bool isGestureLengthOk(int, int);
	int sensorOfGesture(int);
	bool continousMode();
	int numberOfGestures(); 

	// interactive function
	void manageGestures();

};

#endif // _GESTURE_MANAGEMENT_H_



