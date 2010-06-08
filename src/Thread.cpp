#include "Thread.h"

/**
 * Constructor: Calls the init function to initialize the object. 
 * The thread isn't started by default. 
 */
Thread::Thread() {
	init();
}

/**
 * Constructor: If the parameter is set to true, the thread gets automatically started. 
 */
Thread::Thread(bool autostart) {
	init();
	if (autostart) {
		startThread(); // start a thread for every sensor device
	}
}

/**
 * Constructor: The function in the parameter is used as thread function. 
 * This constructor could be helpful when aggregating this class. 
 */
Thread::Thread(void(*meth)(void)) {
	theMethod = meth;
	ready = true;
}

/**
 * Constructor: 
 * The function in the first parameter is used as thread function. 
 * If the second parameter is set to true, the thread gets automatically started. 
 * This constructor could be helpful when aggregating this class. 
 */
Thread::Thread(void(*meth)(void),bool autostart) {
	theMethod = meth;
	ready = true;
	if (autostart) {
		startThread(); // start a thread for every sensor device
	}
}

/**
 * Destructor: 
 * Stops the thread and cleans up. 
 */
Thread::~Thread() {
	stopThread();
	cleanUp(); // delete resources	
}

/**
 * The function in the parameter is used as thread function. 
 * This constructor could be helpful when aggregating this class. 
 */
void Thread::setThreadMethod(void(*meth)(void)) {
	theMethod = meth;
	ready = true;
}

/**
 * By default this method calls the method set by setThreadMethod or the according constructors. 
 * If there hasn't been a method set a null pointer exception will occur. 
 * When inheriting this class this method should be overwritten! 
 * In general inheritance is the easier way of using this class. 
 * Aggregation is more useful when you want to have more then one thread. 
 */
void Thread::threadMethod() { 
	if (ready) {
		void(*theMethod)(void);
	}
}

/**
 * Initializes the object fields. 
 */
void Thread::init() {
	theMethod = NULL;
	ready = false;
	mutex = new Mutex();
	mutex->acquireMutex();
	done = true;
	mutex->releaseMutex();
}

/**
 * Deletes Mutex and p_thread object. 
 */
void Thread::cleanUp() {
	// clean up
	ready = false;
	delete mutex;
	delete thread; // delete thread objects
}

/**
 * Hidden method which doesn't belong to the class Thread itself. 
 * That's because of the way pthread threads are defined. 
 */
void * hiddenThreadMethodCaller(void * ptr) {
	Thread * thiz = reinterpret_cast<Thread*>(ptr);
	thiz->threadMethod();
}

/**
 * Starts the thread. 
 */
void Thread::startThread() {
	/* Create an independent thread */
	bool tmp = false;
	mutex->acquireMutex();
	tmp = done;
	mutex->releaseMutex();
	if (tmp) {
		mutex->acquireMutex();
		done = false;
		thread = new pthread_t;
		pthread_create(thread, NULL, hiddenThreadMethodCaller, reinterpret_cast<void*>(this));
		mutex->releaseMutex();
	}
}

/**
 * Stops the thread and waits for its termination. 
 */
void Thread::stopThread() {
	bool tmp = true;
	mutex->acquireMutex();
	tmp = done;
	mutex->releaseMutex();
	if (!tmp) {
		mutex->acquireMutex();
		done = true;
		mutex->releaseMutex();
		// wait for thread to terminate
		pthread_join(*thread,NULL);
	}
}

/**
 * Returns if the thread has been stopped. 
 * Use this method within the thread method. 
 * As soon as stopped gets true the thread method has to return. 
 */
bool Thread::stopped() {
	bool tmp = true;
	mutex->acquireMutex();
	tmp = done;
	mutex->releaseMutex();
	return tmp;
}

