/**
 * Singleton class limiting to one instance of TuxControl. 
 * Therefore the getInstance method always returns the pointer
 * to the same instance of TuxControl. 
 * Of course singleton behaviour is only ensured if only this
 * class gets used instead of TuxControl or it's descendant. 
 * Author: Lars Widmer, www.lawi.ch
 */

#ifndef __TUXCONTROLSINGLETONKEEPER_INCLUDED__
#define __TUXCONTROLSINGLETONKEEPER_INCLUDED__

#include "TuxControl.h"
// #include <iostream>

class TuxControlSingleton {

private:
	TuxControlSingleton();

public:
	static TuxControl * getInstance();

};

#endif // __TUXCONTROLSINGLETONKEEPER_INCLUDED__


