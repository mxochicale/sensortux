#include "RoggenSensor.h"

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include "../src/FrameParser2.h"
#include "../src/cmdline.h"

#include <stdlib.h>
#include <string>
#include <sstream> // Required for stringstreams

// #include "PracticalSocket.h"
#include "Helper.h"
#include "pthread.h"

#define DEVICE_PATH "/dev/rfcomm"
#define FORMAT "DX5;ccsss-s-s-ssssss"

using namespace std;

/**
 * Constructor initializes the object with path (sensor device e.g. /dev/rfcomm0)
 * and format (e.g. DX5;ccsss-s-s-ssssss). 
 */
RoggenSensor::RoggenSensor(string path, string format) {
	// cout << "Sensor constructor ..." << endl;
	// cout << "path:   " << path << endl;
	// cout << "format: " << format << endl;
	connect(path);
	init(format);
	// cout << "Sensor constructor ... done" << endl;
}

/**
 * Constructor initializes the object with path (sensor device e.g. /dev/rfcomm0). 
 * For the format the default value DX5;ccsss-s-s-ssssss is used. 
 */
RoggenSensor::RoggenSensor(string path) {
	connect(path);
	init();
}

/**
 * Constructor initializes the object with path (sensor device e.g. /dev/rfcomm0)
 * but instead of the whole string just the number at the end is given. 
 * For the format the default value DX5;ccsss-s-s-ssssss is used. 
 */
RoggenSensor::RoggenSensor(int number) {
	connect(number);
	init();
}

/**
 * Default constructor. 
 * Uses default path /dev/rfcomm0
 * For the format the default value DX5;ccsss-s-s-ssssss is used. 
 */
RoggenSensor::RoggenSensor() {
	connect(0);
	init();
}

/**
 * Destructor: Closes the device and cleans up (deleting the frame parser). 
 */
RoggenSensor::~RoggenSensor() {
	closeDevice();
	cleanUp();
}

/**
 * Builds up the usual unix/linux device path by adding the number to the default part 
 * and connects to the device. 
 * This method is used in the constructor. 
 */
void RoggenSensor::connect(int number) {
	ostringstream oss;
	oss << DEVICE_PATH << number; 
	string s = oss.str();
  connect(s);
}

/**
 * Uses the given device path and opens the device. 
 * This method is indirectly used in the constructor. 
 */
void RoggenSensor::connect(string path) {
  device.open(path.c_str()); 
}

/**
 * Creates the FrameParser2 object. 
 * For the format string the default value is used. 
 * This method is used in a constructor. 
 */
void RoggenSensor::init() {
	fp = new FrameParser2(FORMAT);
}

/**
 * Creates the FrameParser2 object using the given format string. 
 * This method is used in a constructor. 
 */
void RoggenSensor::init(const string & format) {
	fp = new FrameParser2(format);
}

/**
 * Deletes the FrameParser2 object. 
 * This method is used in a destructor. 
 */
void RoggenSensor::cleanUp() {
	delete fp;
}

/**
 * Receives raw data from the sensor device. 
 * This method is used for getData(). 
 */
char RoggenSensor::receive() {
	return device.get();
}

/**
 * Closes the sensor device. 
 * This method is used in a destructor. 
 */
void RoggenSensor::closeDevice() {
  device.close();	
}

/**
 * Implements the getData() method from the IntSource interface class. 
 * The method blocks when there is no data available. 
 * Therefore from the outside it looks and feels the same whether you use a 
 * single sensor (RoggenSensor) of a fusion (RoggenSensorFusion) of them. 
 * Both classes implement the same interface IntSource. 
 * The method calls the FrameParser from Daniel Roggen to convert the sensor data. 
 */
RoggenData RoggenSensor::getData() {
	// use FrameParser 
	RoggenData rd;
	rd.clear();
	while (device.good()) {
		char c = this->receive();
		rd = fp->Parser(&c,1);
		if(rd.size()) {
			return rd;
		}
	}
	return rd;
}


