#include "RoggenBuffer.h"

#include "Helper.h"
// #include "RoggenSensor.h"
// #include "DataStrings.h"

#include <string>
#include <vector>
// #include <iostream>
// #include <sstream> // Required for stringstreams

using namespace std;

//-------------------------------------------------------------------------------------------------
// Public: Constructor & Destructor + init & cleanup
//-------------------------------------------------------------------------------------------------
/**
 * Constructor: Initializes the class fields and sets the buffer to default size. 
 */
RoggenBuffer::RoggenBuffer() {
	bufferMaxLength = DEFAULT_BUFFER_LENGTH;
	init();
}

/**
 * Constructor: Initializes the class fields and sets the buffer to default size. 
 * The given data is stored in the buffer. 
 */
RoggenBuffer::RoggenBuffer(std::vector<std::vector<int> > data) {
	bufferMaxLength = DEFAULT_BUFFER_LENGTH;
	init();
	put(data);
}

/**
 * Constructor: Initializes the class fields and sets the buffer to the given size. 
 */
RoggenBuffer::RoggenBuffer(int bufferSize) {
	bufferMaxLength = bufferSize;
	init();
}

/**
 * Destructor: Currently nothing to do. 
 */
RoggenBuffer::~RoggenBuffer() {
	cleanUp();
}

//-------------------------------------------------------------------------------------------------
// Private Methods
//-------------------------------------------------------------------------------------------------
/**
 * Initialize the class fields. 
 */
void RoggenBuffer::init() {
	fresh = false;
	unlimited = false;
	buffer.clear();
}

/**
 * Helper for the destructor: Currently nothing to do. 
 */
void RoggenBuffer::cleanUp() {
}

//-------------------------------------------------------------------------------------------------
// Public Methods
//-------------------------------------------------------------------------------------------------
/**
 * Getter function for the buffer pointer. 
 */
vector<vector<int> > * RoggenBuffer::getPointer() {
	fresh = false;
	return & buffer;
}

/**
 * Getter function for a certain part of the buffer. 
 * The parameters give start and end index. 
 */
vector<vector<int> > RoggenBuffer::getRange(int start,int end) {
	fresh = false;
	std::vector<std::vector<int> > part;
	part.clear();
	check(start,end);
	for (int i=start; i<=end; ++i) {
		part.push_back(buffer.at(i));
	}
	return part;
}

/**
 * Getter function for the whole buffer. 
 */
vector<vector<int> > RoggenBuffer::get() {
	fresh = false;
	return buffer;
}

/**
 * Getter function for the last element of the buffer. 
 * The return value has the same format as the buffer itself. 
 */
vector<vector<int> > RoggenBuffer::getLast() {
	fresh = false;
	vector<vector<int> > part;
	part.clear();
	part.push_back(buffer.back());
	return part;
}

/**
 * Returns if there wasn't a read operation after the last write operation. 
 */
bool RoggenBuffer::isFresh() {
	return fresh;
}

/**
 * Clears the internal buffer. 
 */
void RoggenBuffer::clear() {
	buffer.clear();
}

/**
 * Clears the internal buffer and sets its maximal length to the given length. 
 * The length can be larger then the actual number of frames in the buffer. 
 * In this case the buffer fills up before it acts like a queue. 
 */
void RoggenBuffer::setLength(int bufferSize) {
	clear();
	bufferMaxLength = bufferSize;
	unlimited = false;
}	

/**
 * Returns the actual number of frames in the buffer. 
 */
int RoggenBuffer::size() {
	return buffer.size();
}

/**
 * Returns the current maximum length of the buffer. 
 */
int RoggenBuffer::maxSize() {
	return bufferMaxLength;
}

/**
 * Returns if the buffer is full. 
 * Full means there are as much frames as there is space in the buffer. 
 * There's nothing bad about this. 
 * When full the buffer works in a usual fifo manner (queue). 
 */
bool RoggenBuffer::isFull() {
	if (unlimited) {
		return false;
	}
	return bufferMaxLength==buffer.size();
}

/**
 * Returns the frame at the given buffer position. 
 */
vector<int> RoggenBuffer::at(int index) {
	checkSize(index);
	return buffer.at(index);
}

/**
 * Set the buffer to unlimited length. 
 * This means there are no more frames removed when inserting a new one. 
 * An unlimited buffer never gets full. 
 */
void RoggenBuffer::setUnlimited() {
	unlimited = true;
}

/**
 * Sets unlimited mode of the buffer length. 
 * True means there are no more frames removed when inserting a new one. 
 * An unlimited buffer never gets full. 
 */
void RoggenBuffer::setUnlimited(bool flag) {
	unlimited = flag;
}

/**
 * Set the buffer to limited length. 
 * This means if full there is always the oldest frame removed when inserting a new one. 
 * A limited buffer is full when acting as a Queue. 
 */
void RoggenBuffer::resetUnlimited() {
	unlimited = false;
}

/**
 * Add a new frame to the buffer. 
 */
void RoggenBuffer::put(vector<int> elem) {
	fresh = true;
	buffer.push_back(elem);
	if (unlimited) {
		if (buffer.size() > bufferMaxLength) {
			bufferMaxLength = buffer.size();
		}		
	} else {
		if (buffer.size() > bufferMaxLength) {
			buffer.erase(buffer.begin());
		}
	}
}

/**
 * Add a vector of new frames to the buffer. 
 */
void RoggenBuffer::put(vector<vector<int> > elVec) {
	for (vector<vector<int> >::iterator it=elVec.begin(); it!=elVec.end(); ++it) {
		put(*it);
	}
}

/**
 * Returns a vector of a single measurement for all frames in the buffer. 
 * A measurement for examle is AccX. 
 * The measurement index must be within the valid range for the vector index. 
 */
vector<int> RoggenBuffer::getSingleMeasurement(int meas) {
	vector<int> ret;
	ret.clear();
	// if (meas < 0 || meas >= buffer.size()) {
	// 	cerr << "RoggenBuffer::getting single measurement: Index mismatch! Select a valid measurement index!" << endl;
	// } else {
	for (vector<vector<int> >::iterator it=buffer.begin(); it!=buffer.end(); ++it) {
		ret.push_back(it->at(meas));
	}
	// }
	return ret;
}

/**
 * Returns a vector of a single measurement for all frames in the given 2dimensional vector. 
 * This static method doesn't operate on the internal buffer data. 
 * A measurement for example is AccX. 
 * Measurement must be within the valid range for the vector index. 
 */
vector<int> RoggenBuffer::getSingleMeasurement(int meas,vector<vector<int> > vec) {
	vector<int> ret;
	ret.clear();
	// if (meas < 0 || meas >= vec.at(0).size()) {
	// 	cerr << "RoggenBuffer::getting single measurement: Index mismatch! Select a valid measurement index!" << endl;
	// } else {
	for (vector<vector<int> >::iterator it=vec.begin(); it!=vec.end(); ++it) {
		ret.push_back(it->at(meas));
	}
	// }
	return ret;
}

/**
 * This method returns a part of the buffer. 
 * The parameters tell from which start index to which stop index the frames should be returned. 
 * The frame at the start position is the first to be returend. 
 * And the frame at the end position is the last to be returend. 
 * Same functionality as getRange. 
 */
vector<vector<int> > RoggenBuffer::getFrames(int start, int end) {
	vector<vector<int> > ret;
	ret.clear();
	check(start,end);
	for (int i=start; i<=end; ++i) {
		ret.push_back(buffer.at(i));
	}
	return ret;	
}

/**
 * This method returns a part of the given buffer. 
 * This static method doesn't operate on the internal buffer data. 
 * The parameters tell from which start index to which stop index the frames should be returned. 
 * The frame at the start position is the first to be returend. 
 * And the frame at the end position is the last to be returend. 
 */
vector<vector<int> > RoggenBuffer::getFrames(int start,int end,vector<vector<int> > vec) {
	vector<vector<int> > ret;
	ret.clear();
	check(start,end,vec.size());
	for (int i=start; i<=end; ++i) {
		ret.push_back(vec.at(i));
	}
	return ret;	
}

/**
 * Returns the sum of the given two vectors. 
 */
vector<int> RoggenBuffer::getSum(vector<int> vecA, vector<int> vecB) {
	vector<int> ret;
	ret.clear();
	if (vecA.size() != vecB.size()) {
		cerr << "RoggenBuffer::getSum: Size mismatch! Vectors must have equal length!" << endl;
	} else {
		for (int i=0; i<vecA.size(); ++i) {
			ret.push_back(vecA.at(i)+vecB.at(i));
		}
	}
	return ret;	
}

/**
 * Returns the sum of the given three vectors. 
 */
vector<int> RoggenBuffer::getSum(vector<int> vecA,vector<int> vecB,vector<int> vecC) {
	vector<int> ret;
	ret.clear();
	if (vecA.size() != vecB.size() || vecA.size() != vecC.size()) {
		cerr << "RoggenBuffer::getSum: Size mismatch! Vectors must have equal length!" << endl;
	} else {
		for (int i=0; i<vecA.size(); ++i) {
			ret.push_back(vecA.at(i)+vecB.at(i)+vecC.at(i));
		}
	}
	return ret;		
}

/**
 * Returns the vectorial sum of the given two vectors. 
 * The two numbers of a pair from the two input vectors are put to the square. 
 * The squares are added and from the result the root is drawn and put to the result vector. 
 */
vector<int> RoggenBuffer::getVectorialSum(vector<int> vecA,vector<int> vecB) {
	vector<int> ret;
	ret.clear();
	if (vecA.size() != vecB.size()) {
		cerr << "RoggenBuffer::getSum: Size mismatch! Vectors must have equal length!" << endl;
	} else {
		for (int i=0; i<vecA.size(); ++i) {
			ret.push_back(static_cast<int>(sqrt(pow(vecA.at(i),2)+pow(vecB.at(i),2))));
		}
	}
	return ret;	
}

/**
 * Returns the vectorial sum of the given three vectors. 
 * The three numbers of a triple from the three input vectors are put to the square. 
 * The squares are added and from the result the root is drawn and put to the result vector. 
 */
vector<int> RoggenBuffer::getVectorialSum(vector<int> vecA,vector<int> vecB,vector<int> vecC) {
	vector<int> ret;
	ret.clear();
	if (vecA.size() != vecB.size() || vecA.size() != vecC.size()) {
		cerr << "RoggenBuffer::getSum: Size mismatch! Vectors must have equal length!" << endl;
	} else {
		for (int i=0; i<vecA.size(); ++i) {
			ret.push_back(static_cast<int>(sqrt(pow(vecA.at(i),2)+pow(vecB.at(i),2)+pow(vecC.at(i),2))));
		}
	}
	return ret;		
}

/**
 * Returns true if the given first number is above zero and below or equal to the second parameter. 
 */
void RoggenBuffer::checkSize(int num, int max) {
	if (num < 0 || num >= max) {
		cerr 
			<< "RoggenBuffer: Vector index " << num 
			<< " out of range! (max: " << max-1 << ")" << endl;
		exit(1); 
	}
}

/**
 * Returns true if the given number is above zero and below or equal to the buffer size. 
 */
void RoggenBuffer::checkSize(int num) {
	checkSize(num,buffer.size());
}

/**
 * Returns true if the given two numbers both are above zero and below or equal to the buffer size. 
 */
void RoggenBuffer::check(int start, int end) {
	check(start,end,buffer.size());
}

/**
 * Returns true if the given first two numbers both are above zero and below or equal to the third parameter. 
 */
void RoggenBuffer::check(int start, int end, int max) {
	checkSize(start,max);
	checkSize(end,max);
	if (start > end) {
		cerr << "RoggenFeatureExtraction: Vector range selection start index is higher then the ending index!" << endl;
		exit(1); 
	}	
}

/**
 * Filters the data from the buffer given by a pointer. 
 * The measurement with the given index must have the given value. 
 * Frames which satisfy this condition are put to the result vector. 
 */
vector<vector<int> > RoggenBuffer::filter(int meas,int val,vector<vector<int> >*bufp) {
	vector<vector<int> > ret;
	ret.clear();
	for (vector<vector<int> >::iterator it=bufp->begin(); it!=bufp->end(); ++it) {
		if (it->at(meas)==val) {
			ret.push_back(*it);
		}
	}
	return ret;
}

/**
 * Filters the internal buffer. 
 * The measurement with the given index must have the given value. 
 * Frames which satisfy this condition are put to the result vector. 
 */
vector<vector<int> > RoggenBuffer::filter(int meas,int val) {
	return filter(meas,val,&buffer);
}

/**
 * Filters the data from the given buffer. 
 * The measurement with the given index must have the given value. 
 * Frames which satisfy this condition are put to the result vector. 
 */
vector<vector<int> > RoggenBuffer::filter(int meas,int val,std::vector<std::vector<int> > buf) {
	return filter(meas,val,&buf);
}

/**
 * Filters and puts the given frame. 
 * The measurement with the given index must have the given value. 
 * If the given frame satisfies this condition it's put to the internal buffer. 
 */
void RoggenBuffer::filterPut(int meas,int val,std::vector<int> line) {
	if (line.at(meas) == val) {
		put(line);	
	}
}

/**
 * Filters and puts the given buffer. 
 * The measurement with the given index must have the given value. 
 * The given frames which satisfy this condition are put to the internal buffer. 
 */
void RoggenBuffer::filterPut(int meas,int val,std::vector<std::vector<int> > lines) {
	put(filter(meas,val,lines));
}


