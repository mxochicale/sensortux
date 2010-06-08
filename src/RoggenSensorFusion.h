/**
 * This class mixes a number of sensors into a channel similar to a single sensor. 
 * Therefore it implements the same interface as the Sensor class itself. 
 * Still it's easy to see from which sensor a value originates since every frame 
 * carries the sensor ID within.  
 * Author: Lars Widmer, www.lawi.ch
 **/

#ifndef _ROGGEN_SENSOR_FUSION_H_
#define _ROGGEN_SENSOR_FUSION_H_

#include "RoggenSensor.h"

// only for windows!!!
#ifdef WIN32
#include "../src/pthread.h"
#endif

#include <vector>
#include <string>

class RoggenSensorFusion : public IntSource {

private: 
	std::vector<pthread_t *> threads;
	std::vector<RoggenSensor *> rsVec;
	bool done;
	pthread_mutex_t mutex;
	pthread_mutex_t blockMutex;
	pthread_cond_t  blockCond;
	RoggenData newData;
	int index;

	// void * threadMethodCaller(void *);

	void startThreads();
	void stopThreads();
	void cleanUp();
	void init();
	void initSensorThreads(std::vector<std::string>, std::string);

public: 
	RoggenSensorFusion(std::vector<std::string>, std::string);
	virtual ~RoggenSensorFusion();

	virtual RoggenData getData();

	void threadMethod();

};

#endif // ROGGEN_SENSOR_FUSION



