#include "Repercussion.h"

#include "Helper.h"

using namespace std;

//-------------------------------------------------------------------------------------------------
// Constructor & Destructor
//-------------------------------------------------------------------------------------------------
/**  
 * Constructor creates and initializes an empty instance. 
 */
Repercussion::Repercussion() {
	active = false;
	decrement = 1;
	uDelay = 1000;
	maximum = 500;
	values.clear();
	startThread(); // start thread 
}

/**  
 * Constructor creates and initializes an instance for the use with the given number of values. 
 */
Repercussion::Repercussion(int number) {
	active = false;
	decrement = 1;
	uDelay = 1000;
	maximum = 500;
	values.clear();
	for (int i=0; i<number; ++i) {
		values.push_back(0);
	}
	startThread(); // start thread 
}

/**  
 * Destructor: Stops the internal thread. 
 */
Repercussion::~Repercussion() {
	stopThread(); // stop thread 
}

//-------------------------------------------------------------------------------------------------
// Methods
//-------------------------------------------------------------------------------------------------
/**  
 * Thread method aging the stored values according to the defined settings. 
 */
void Repercussion::threadMethod() {
	long int uSec = Helper::getInstance()->uSecondsSinceStart();
	long int olduSec = uSec;
	long int diff = 0;
	while (!stopped()) {
		if (active) {
			uSec = Helper::getInstance()->uSecondsSinceStart();
			diff = uSec - olduSec;
			if (diff/uDelay < 0 || diff/uDelay > 5) { // e.g. in case of an overflow
				diff = uDelay;
			}
			decAll(static_cast<int>(decrement*diff/uDelay));
			// cout << "Corrected decrement: " << static_cast<int>(decrement*diff/uDelay) << endl;
			olduSec = uSec;
		} 
		Helper::specialSleep(uDelay);
	}
}

/**  
 * Set the value with the given index to the given level. 
 */
void Repercussion::set(int val, int level) {
	acquireMutex();
	while (val >= values.size()) {
		values.push_back(0);
	}
	values.at(val) = level;
	releaseMutex();
}

/**  
 * Return the value with the given index. 
 */
int Repercussion::get(int val) {
	acquireMutex();
	while (val >= values.size()) {
		values.push_back(0);
	}
	int tmp = values.at(val);
	releaseMutex();
	return tmp; 
}

/**  
 * Reset all values to zero. 
 */
void Repercussion::resetAll() {
	for (int i=0; i<values.size(); ++i) {
		set(i,0);
	}
}

/**  
 * Reset the value with the given index number to zero. 
 */
void Repercussion::reset(int val) {
	set(val,0);
}

/**  
 * Add the amount given in the second parameter to the value with the index given in the first parameter. 
 * With this method the values can't go over the defined maximum. 
 */
void Repercussion::inc(int val, int amount) {
	set(val,get(val)+amount);
	if (get(val) > maximum) {
		set(val,maximum);
	}
}

/**  
 * Subtract the amount given in the second parameter from the value with the index given in the first parameter. 
 * With this method the values can't get negative. 
 */
void Repercussion::dec(int val, int amount) {
	set(val,get(val)-amount);
	if (get(val) < 0) {
		reset(val);
	}
}

/**  
 * Add the given amount to all values. 
 * With this method the values can't go over the defined maximum. 
 */
void Repercussion::incAll(int amount) {
	for (int i=0; i<values.size(); ++i) {
		inc(i,amount);
	}
}

/**  
 * Set all values to the given level. 
 */
void Repercussion::setAll(int level) {
	for (int i=0; i<values.size(); ++i) {
		set(i,level);
	}
}

/**  
 * Subtract the given amount from all values. 
 * With this method the values can't get negative. 
 */
void Repercussion::decAll(int amount) {
	for (int i=0; i<values.size(); ++i) {
		dec(i,amount);
	}
}

/**  
 * Returns all the values as a vector. 
 */
std::vector<int> Repercussion::getAll() {
	return values;
}

/**  
 * Activates the repercussion instance. 
 * Initially the instance of this class is inactive. 
 * It doesn't start decrementing before it's activated. 
 * First define all the settings and then activate. 
 */
void Repercussion::activate() {
	acquireMutex();
	active = true;
	releaseMutex();
}

/**  
 * Deactivates the repercussion instance. 
 * It doesn't decrement while inactive. 
 */
void Repercussion::deactivate() {
	acquireMutex();
	active = false;
	releaseMutex();
}

/**  
 * Setter function for the delay in microseconds between the decrementation steps. 
 * Use values above 1000. 
 * Default is 1000. 
 */
void Repercussion::setDelay(int uSec) {
	acquireMutex();
	uDelay = uSec;
	releaseMutex();
}

/**  
 * Setter function for the amount to subtract from the values in each step. 
 * Default is 1. 
 */
void Repercussion::setDecrement(int subtr) {
	acquireMutex();
	decrement = subtr;
	releaseMutex();
}

/**  
 * Setter function for the maximum value. 
 * Default is 500. 
 */
void Repercussion::setMaximum(int max) {
	acquireMutex();
	maximum = max;
	releaseMutex();
}

/**  
 * Returns true if the value with the given index is zero (or below). 
 */
bool Repercussion::isZero(int val) {
	return (get(val)<=0);
}




