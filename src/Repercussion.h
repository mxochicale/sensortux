/**
 * Class for managing values which decrease against time. 
 * It can be used to make single events lasting over a defined
 * amount of time. 
 * Author: Lars Widmer, www.lawi.ch
 */

#ifndef _REPERCUSSION_H_
#define _REPERCUSSION_H_

#include <vector>

#include "Thread.h"
#include "Mutex.h"

class Repercussion : private Thread, private Mutex {

private:
	std::vector<int> values;
	bool active;
	int decrement;
	int uDelay;
	int maximum;

public:
	Repercussion();
	Repercussion(int);
	virtual ~Repercussion();

	void threadMethod();

	void set(int, int = 1);
	int get(int);

	void resetAll();
	void reset(int);
	void inc(int, int = 1);
	void dec(int, int = 1);
	void incAll(int = 1);
	void setAll(int = 1);
	void decAll(int = 1);

	std::vector<int> getAll();
	bool isZero(int);

	void activate();
	void deactivate();
	
	void setDelay(int);
	void setDecrement(int);
	void setMaximum(int);
	
};

#endif // _REPERCUSSION_H_



