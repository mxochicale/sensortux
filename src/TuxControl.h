/**
 * Simple class to remote control the super tux game. 
 * Class can be inherited to steer the penguin. 
 * TuxControlSingleton class has to be adapted, that it instances the right descendant of TuxControl. 
 * But since TuxControl itself offers all the necessary methods, 
 * TuxControlSingleton with the raw TuxControl implementation 
 * could also be aggregated from a separate thread to control the robot. 
 * With a mutex TuxControl is thread safe. 
 * Author: Lars Widmer, www.lawi.ch
 */

#ifndef _TUX_CONTROL_H_
#define _TUX_CONTROL_H_

#include <vector>

#include "pthread.h"
#include "Mutex.h"

// Simple codes for the used keys
#define PLAYER_LEFT 1
#define PLAYER_RIGHT 2
#define PLAYER_UP 3
#define PLAYER_DOWN 4
#define PLAYER_FIRE 5

// Events
#define KEY_PRESSED 1
#define KEY_RELEASED 2

struct ControlEvent {
	bool valid;
	int key;
	int event;
};

class TuxControl {

private:
	std::vector<ControlEvent> queue;
	Mutex * mutex;

public:
	TuxControl();
	virtual ~TuxControl();
	ControlEvent getControlEvent();
	void setControlEvent(ControlEvent ce);
	void validateControlEvent();
	void invalidateControlEvent();
	ControlEvent removeControlEvent();
	void addControlEvent(ControlEvent ce);
	int getEventQueueLength();
	void clearQueue();

};

#endif // _TUX_CONTROL_H_




