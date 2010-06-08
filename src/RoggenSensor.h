/**
 * Class to handle the Daniel Roggen Sensors. 
 * One instance (object) of this class connects to one sensor. 
 * Use a vector of objects on the higher level to use multiple sensors. 
 * This can be done by RoggenSensorFusion. 
 * Author: Lars Widmer, www.lawi.ch
 */

#ifndef _ROGGEN_SENSOR_H_
#define _ROGGEN_SENSOR_H_

#include <fstream>
#include <vector>

#include "FrameParser2.h"
#include "IntSource.h"

typedef std::vector<std::vector<int> > RoggenData;

class RoggenSensor : public IntSource {

private:
	std::ifstream device;
	FrameParser2 * fp;

	void init();
	void init(const std::string &);
	void cleanUp();
	void connect(int);
	void connect(std::string);
	char receive();
	void closeDevice();

public: 
	RoggenSensor(std::string, std::string);
	RoggenSensor(std::string);
	RoggenSensor(int);
	RoggenSensor();
	virtual ~RoggenSensor();

	virtual RoggenData getData();

};

#endif



