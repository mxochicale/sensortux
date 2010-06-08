/**
 * Simple class to remote control the super tux game. 
 * Class can be inherited to steer the penguin. 
 * TuxControlSingleton class has to be adapted, that it creates an 
 * instance of the right descendant of TuxControl. 
 * Since TuxControl itself offers all the necessary methods, 
 * TuxControlSingleton could also be used with the raw 
 * TuxControl implementation 
 * In that case the same instance of TuxControl (via TuxControlSingleton) 
 * would had to be aggregated from a separate thread to control the robot. 
 * With a mutex TuxControl is thread safe. 
 * Author: Lars Widmer, www.lawi.ch
 */

#ifndef _CLASSIFICATION_TUX_H_
#define _CLASSIFICATION_TUX_H_

#include <vector>
#include "pthread.h"
#include "Thread.h"
#include "TuxControl.h"
#include "GestureManagement.h"
#include "Repercussion.h"

class ClassificationTux : public TuxControl, private Thread {

private:
	GestureManagement * geMan;
	std::vector<bool> highEnergy;

	std::vector<bool> currentlyPressed;
	Repercussion * repercussion;

	int lastTime;
	int counter;

	void save(ControlEvent);
	void convert(int, bool);
	void extractChanges(int, bool);
	void filter(int, bool);
	void evaluate();

	void continous();
	void segmented();

public:
	ClassificationTux();
	virtual ~ClassificationTux();

	void threadMethod();

};

#endif // _CLASSIFICATION_TUX_H_



