#include "TuxControl.h"

/**
 * Initializes the aggregated mutex. 
 * And initially clears the queue. 
 */
TuxControl::TuxControl() {
	mutex = new Mutex();
	mutex->acquireMutex();
	queue.clear();
	mutex->releaseMutex();
}

/**
 * Deletes the mutex. 
 */
TuxControl::~TuxControl() {
	delete mutex;
}

/**
 * Returns the first event in queue. 
 * It does neither invalidate nor remove the event. 
 * Use this function if you don't want queue functionality. 
 */
ControlEvent TuxControl::getControlEvent() {
	ControlEvent tmp;
	mutex->acquireMutex();
	tmp = queue.front();
	mutex->releaseMutex();
	return tmp;
}

/**
 * Clears the queue and
 * sets the first event in queue. 
 * Use this function if you don't want queue functionality. 
 */
void TuxControl::setControlEvent(ControlEvent ce) {
	ControlEvent current;
	current.valid = ce.valid;
	current.key = ce.key;
	current.event = ce.event;
	mutex->acquireMutex();
	queue.clear();
	queue.push_back(current);
	mutex->releaseMutex();
}

/**
 * Sets the first element in queue to be valid. 
 * Use this function if you don't want queue functionality. 
 */
void TuxControl::validateControlEvent() {
	mutex->acquireMutex();
	if (queue.size() > 0) {
		queue.front().valid = true;
	}
	mutex->releaseMutex();
}

/**
 * Sets the first element in queue to be invalid. 
 * Use this function if you don't want queue functionality. 
 */
void TuxControl::invalidateControlEvent() {
	mutex->acquireMutex();
	if (queue.size() > 0) {
		queue.front().valid = false;
	}
	mutex->releaseMutex();
}

/**
 * Removes and returns the first (oldest) element from the queue. 
 */
ControlEvent TuxControl::removeControlEvent() {
	ControlEvent tmp;
	tmp.valid = false;
	tmp.key = 0;
	tmp.event = 0;
	mutex->acquireMutex();
	if (queue.size() > 0) {
		tmp = queue.front();
		queue.erase(queue.begin());
	}
	mutex->releaseMutex();
	return tmp;
}

/**
 * Adds a new element to the back of the queue. 
 */
void TuxControl::addControlEvent(ControlEvent ce) {
	mutex->acquireMutex();
	queue.push_back(ce);
	mutex->releaseMutex();
}

/**
 * Returns the current length of the queue. 
 */
int TuxControl::getEventQueueLength() {
	int tmp = 0;
	mutex->acquireMutex();
	tmp = queue.size();
	mutex->releaseMutex();
	return tmp;
}

/**
 * Clears the queue. 
 */
void TuxControl::clearQueue() {
	mutex->acquireMutex();
	queue.clear();
	mutex->releaseMutex();
}


