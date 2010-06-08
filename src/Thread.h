/**
 * Author: Lars Widmer, www.lawi.ch
 */

#ifndef _THREAD_H_
#define _THREAD_H_

#include "pthread.h"
#include "Mutex.h"

/*
#ifdef WIN32
// only for windows!!!
#include "pthread.h"
#endif
*/

class Thread {

private:
	pthread_t * thread;
	bool done;
	bool ready;
	Mutex * mutex;
	void (*theMethod)(void);

	void cleanUp();
	void init();

public:
	Thread();
	Thread(bool);
	Thread(void(*meth)(void));
	Thread(void(*meth)(void),bool);
	virtual ~Thread();
	void setThreadMethod(void(*meth)(void));
	void startThread();
	void stopThread();
	bool stopped();
	virtual void threadMethod();

};

#endif // _THREAD_H_



