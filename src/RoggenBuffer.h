/**
 * Class to store data from the sensors in memory. 
 * The class works as a queue with some helpful access methods. 
 * Author: Lars Widmer, www.lawi.ch
 */

#ifndef _ROGGEN_BUFFER_H_
#define _ROGGEN_BUFFER_H_

#define DEFAULT_BUFFER_LENGTH 128

#include <string>
#include <vector>

class RoggenBuffer {

private: 
	std::vector<std::vector<int> > buffer;
	int bufferMaxLength;
	bool fresh;
	bool unlimited;

	void init();
	void cleanUp();

public: 
	// Constructors
	RoggenBuffer(); 
	RoggenBuffer(std::vector<std::vector<int> >); 
	RoggenBuffer(int); 
	virtual ~RoggenBuffer(); 

	std::vector<std::vector<int> > * getPointer(); 
	std::vector<std::vector<int> > getRange(int,int); 
	std::vector<std::vector<int> > get(); 
	std::vector<std::vector<int> > getLast(); 

	bool isFresh();

	void clear();
	void setLength(int);
	int size();
	int maxSize();
	bool isFull();

	std::vector<int> at(int);

	void put(std::vector<int>);
	void put(std::vector<std::vector<int> >);

	std::vector<int> getSingleMeasurement(int);
	static std::vector<int> getSingleMeasurement(int,std::vector<std::vector<int> >);
	std::vector<std::vector<int> > getFrames(int,int);
	static std::vector<std::vector<int> > getFrames(int,int,std::vector<std::vector<int> >);

	static std::vector<int> getSum(std::vector<int>,std::vector<int>);
	static std::vector<int> getSum(std::vector<int>,std::vector<int>,std::vector<int>);
	static std::vector<int> getVectorialSum(std::vector<int>,std::vector<int>);
	static std::vector<int> getVectorialSum(std::vector<int>,std::vector<int>,std::vector<int>);

	static void checkSize(int, int);
	void checkSize(int);
	static void check(int, int, int);
	void check(int, int);

	static std::vector<std::vector<int> > filter(int,int,std::vector<std::vector<int> > *);
	static std::vector<std::vector<int> > filter(int,int,std::vector<std::vector<int> >);
	std::vector<std::vector<int> > filter(int,int);

	void filterPut(int,int,std::vector<int>);
	void filterPut(int,int,std::vector<std::vector<int> >);

	void setUnlimited();
	void resetUnlimited();
	void setUnlimited(bool flag);

};

#endif // ROGGEN BUFFER H



