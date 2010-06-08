/**
 * Interface class: 
 * Every descendant has to offer a getData function. 
 * Example descendants are the sensor classes. 
 * Author: Lars Widmer, www.lawi.ch
 */

#ifndef _INT_SOURCE_H_
#define _INT_SOURCE_H_

#include <vector>

class IntSource {

private:

public: 
	virtual std::vector<std::vector<int> > getData() = 0;
	virtual ~IntSource();

};

#endif // _INT_SOURCE_H_



