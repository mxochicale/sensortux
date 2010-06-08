#include "TuxControlSingleton.h"

#include "ClassificationTux.h"

/**
 * Singleton constructor limiting to one instance of TuxControl. 
 * Therefore this method always returns the pointer to the same
 * instance of TuxControl. 
 * Adapt the actual instatiation to use the required descendant of TuxControl (e.g. ClassificationTux). 
 */
TuxControl * TuxControlSingleton::getInstance() {
	static ClassificationTux tc; // change class name according to desired subclass
	return &tc;
}

TuxControlSingleton::TuxControlSingleton() {
}

