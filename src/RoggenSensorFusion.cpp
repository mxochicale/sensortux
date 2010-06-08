#include "RoggenSensorFusion.h"

#include "RoggenSensor.h"
#include "DataStrings.h"
#include "Helper.h"

// #include "../src/pthread.h" // only for windows!!!
// #include "../src/PracticalSocket.h"

#include <iostream>

using namespace std;

/**
 * Constructor: 
 * Prepares the threads and starts them. 
 * Always use this constructor. 
 */
RoggenSensorFusion::RoggenSensorFusion(vector<string> deviceNames, string format) {
	init(); 																// clear fields
	initSensorThreads(deviceNames,format); 	// init sensors and threads
	startThreads(); 												// start a thread for every sensor device
}

/**
 * Destructor: 
 * Stops the threads and cleans up. 
 */
RoggenSensorFusion::~RoggenSensorFusion() {
	// pthread_mutex_lock(&mutex);
	// cout << "Destructor" << endl;
	// pthread_mutex_unlock(&mutex);
	stopThreads(); 									// wait for threads to join
	cleanUp(); 											// delete resources	
}

/**
 * Hidden method, not part of the class interface. 
 * Used because of the pthread limitations. 
 */
void * threadMethodCaller(void * ptr) {
	RoggenSensorFusion * thiz = reinterpret_cast<RoggenSensorFusion*>(ptr);
	thiz->threadMethod();
}

/**
 * Thread Method: This method is called once. 
 * After it runs out the thread stops. 
 * That's why it uses a while loop within. 
 * It get's data from the input sensors and puts it into a queue. 
 * The output method getData() reads from this queue. 
 * pthread conditionals are used to block getData() when there is no data available. 
 */
void RoggenSensorFusion::threadMethod() {
	int myIndex;
	pthread_mutex_lock(&mutex);
	myIndex = index;
	++index;
	pthread_mutex_unlock(&mutex);

	RoggenData rd;
	rd.clear();

	RoggenSensor * rs = rsVec.at(myIndex);
	bool tmpDone = false;
	int errCount = 0;
	pthread_mutex_lock(&mutex);
	tmpDone = done;
	pthread_mutex_unlock(&mutex);
	while (!tmpDone) {
		rd = rs->getData(); 				// disable for testing
		// Helper::specialSleep(7000+myIndex*1000); 	// enable for testing
		if(rd.size()) {
			pthread_mutex_lock(&mutex);
			for (vector<vector<int> >::iterator it=rd.begin(); it!=rd.end(); ++it) {
				newData.push_back(*it);
			}
			// cout << "got new data: unblocking!" << endl;
			// pthread_mutex_unlock(&blocker);
			pthread_mutex_unlock(&mutex);
			pthread_mutex_lock(&blockMutex);
			pthread_cond_signal(&blockCond);
			pthread_mutex_unlock(&blockMutex);
		} else {
			pthread_mutex_lock(&mutex);
			cerr << "Got inproper data" << endl;
			pthread_mutex_unlock(&mutex);
			if (++errCount > 10) {
				pthread_mutex_lock(&mutex);
				cerr << "Due to inproper data ending data acquisition!" << endl;
				cerr << "(Hit Esc to terminate application.)" << endl;
				done = true;
				pthread_mutex_unlock(&mutex);
			}
		}
		pthread_mutex_lock(&mutex);
		tmpDone = done;
		pthread_mutex_unlock(&mutex);
	}
}

/**
 * Starts the threads used for this class. 
 * This method gets called by the constructor. 
 */
void RoggenSensorFusion::startThreads() {
	/* Create an independent thread */
	pthread_mutex_lock(&mutex);
	done = false;
	index = 0;
	pthread_mutex_unlock(&mutex);
	for (int i=0; i<threads.size(); ++i) {
		pthread_mutex_lock(&mutex);
		pthread_create(threads.at(i), NULL, threadMethodCaller, reinterpret_cast<void*>(this));
		pthread_mutex_unlock(&mutex);
	}
}

/**
 * Stops the threads used for this class. 
 * This method gets called by the destructor. 
 */
void RoggenSensorFusion::stopThreads() {
	pthread_mutex_lock(&mutex);
	done = true;
	pthread_mutex_unlock(&mutex);
	// wait for threads to terminate
	for (int index=0; index<threads.size(); ++index) {
		pthread_join(*threads.at(index),NULL);
	}
}

/**
 * CleanUp deletes the sensor and thread instances and destroys the mutexes and conditional. 
 */
void RoggenSensorFusion::cleanUp() {
	// clean up
	for (vector<RoggenSensor*>::iterator it=rsVec.begin(); it!=rsVec.end(); ++it) {
		delete *it;	// delete Sensor objects
	}
	for (vector<pthread_t *>::iterator it=threads.begin(); it!=threads.end(); ++it) {
		delete *it;	// delete thread objects
	}
	pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&blockMutex);
	pthread_cond_destroy(&blockCond);
}

/**
 * Init prepares the mutexes and conditional and initializes the fields. 
 */
void RoggenSensorFusion::init() {
	done = false;
	pthread_mutex_init(&mutex,NULL);
	pthread_mutex_init(&blockMutex,NULL);
	pthread_cond_init(&blockCond,NULL);
	// pthread_mutex_init(&blocker,NULL);
	// mutex = PTHREAD_MUTEX_INITIALIZER;
	// blockCond = PTHREAD_COND_INITIALIZER;
	Helper::getInstance()->setRowSize(7);
	rsVec.clear();
	newData.clear();
	// cout << "Data Acquisition for Roggen sensors" << endl;
}

/**
 * Prepares the sensors and threads. 
 */
void RoggenSensorFusion::initSensorThreads(vector<string> deviceNames, string format) {
	for (int i=0; i<deviceNames.size(); ++i) {
		rsVec.push_back(new RoggenSensor(deviceNames.at(i),format));
		threads.push_back(new pthread_t);
	}
}

/**
 * Implements the getData() method from the IntSource interface class. 
 * Reads data from the internal queue. 
 * Using conditional a blocking read is simulated. 
 * Therefore from the outside it looks and feels the same whether you use a 
 * single sensor (RoggenSensor) of a fusion (RoggenSensorFusion) of them. 
 * Both classes implement the same interface IntSource. 
 */
RoggenData RoggenSensorFusion::getData() {
	RoggenData tmp;

	// pthread_mutex_lock(&blocker);
	int tmpSize;
	bool tmpDone;
	pthread_mutex_lock(&mutex);
	tmpSize = newData.size();
	tmpDone = done;
	pthread_mutex_unlock(&mutex);
	while (tmpSize < 1 && !tmpDone) {
		pthread_mutex_lock(&blockMutex);
		pthread_cond_wait(&blockCond, &blockMutex);
		pthread_mutex_unlock(&blockMutex);
		pthread_mutex_lock(&mutex);
		// cout << "cond wait unblocks!" << endl;
		tmpSize = newData.size();
		tmpDone = done;
		pthread_mutex_unlock(&mutex);
	}

	pthread_mutex_lock(&mutex);
	tmp = newData;
	newData.clear();
	pthread_mutex_unlock(&mutex);
	return tmp;
}



