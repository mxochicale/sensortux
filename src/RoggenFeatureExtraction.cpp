#include "RoggenFeatureExtraction.h"

#include "RoggenBuffer.h"
#include "Helper.h"

#include <string>
#include <vector>
#include <iostream>
// #include <sstream> // Required for stringstreams

using namespace std;

#define MEAN_CROSSING_THRESHOLD 50
#define ENERGY_THRESHOLD 15000
#define ENERGY_HYSTERESIS 12000

//-------------------------------------------------------------------------------------------------
// Public: Constructor & Destructor + init & cleanup
//-------------------------------------------------------------------------------------------------
/**
 * Constructor: FeatureExtraction objects only work on a buffer. 
 * Therefore a pointer to a buffer has to be handed over for construction. 
 * Initially the index vector get's cleared. 
 * The indices are used for energy based segementation. 
 */
RoggenFeatureExtraction::RoggenFeatureExtraction(RoggenBuffer * b) {
	init();
	buffer = b;
}

/**
 * Destructor: Ready to clean up... yet nothing to do. 
 */
RoggenFeatureExtraction::~RoggenFeatureExtraction() {
	cleanUp();
}

//-------------------------------------------------------------------------------------------------
// Private Methods
//-------------------------------------------------------------------------------------------------
/**
 * Used by the constructor. 
 * Initially the index vector get's cleared. 
 * The indices are used for energy based segementation. 
 */
void RoggenFeatureExtraction::init() {
	indices.clear();
}

/**
 * Cleaning up. Used by the destructor. 
 * Up to now: Nothing to do. 
 */
void RoggenFeatureExtraction::cleanUp() {
}

//-------------------------------------------------------------------------------------------------
// Public Methods
//-------------------------------------------------------------------------------------------------
/**
 * Returns the mean value of the buffer for the given measurement. 
 * The buffer holds a set of measurements (AccX, AccY, RotX, ...) against time. 
 * With the measurement index you choose which measurement to use for calculating the average. 
 */
float RoggenFeatureExtraction::getMean(int meas) {
	return getMean(meas,0,buffer->size()-1);
}

/**
 * Returns the variance of the buffer for the given measurement. 
 * The buffer holds a set of measurements (AccX, AccY, RotX, ...) against time. 
 * For computing the variance the mean value is needed. 
 * If it has already been computed, pass it as the second parameter for better performance. 
 * There's also a version of getVariance without a second parameter. 
 */
float RoggenFeatureExtraction::getVariance(int meas, float mean) {
	return getVariance(meas,0,buffer->size()-1,mean);	
}

/**
 * Returns the standard deviation of the buffer for the given measurement. 
 * The buffer holds a set of measurements (AccX, AccY, RotX, ...) against time. 
 * For computing the standard deviation the mean value is needed. 
 * If it has already been computed, pass it as the second parameter for better performance. 
 * There's also a version of getStandardDeviation without a second parameter. 
 */
float RoggenFeatureExtraction::getStandardDeviation(int meas, float mean) {
	return sqrt(getVariance(meas,mean));
}

/**
 * Returns the mean crossing rate of the buffer for the given measurement. 
 * The buffer holds a set of measurements (AccX, AccY, RotX, ...) against time. 
 * For computing the mean crossing rate the mean value is needed. 
 * If it has already been computed, pass it as the second parameter for better performance. 
 * There's also a version of getMeanCrossingRate without a second parameter. 
 */
float RoggenFeatureExtraction::getMeanCrossingRate(int meas, float mean) {
	return getMeanCrossingRate(meas,0,buffer->size()-1,mean);	
}

/**
 * Returns the variance of the buffer for the given measurement. 
 * The buffer holds a set of measurements (AccX, AccY, RotX, ...) against time. 
 * For computing the variance the mean value is needed. 
 * If it has already been computed, use the overloaded function and pass the mean 
 * as the second parameter for better performance. 
 */
float RoggenFeatureExtraction::getVariance(int meas) {
	return getVariance(meas,getMean(meas));
}

/**
 * Returns the standard deviation of the buffer for the given measurement. 
 * The buffer holds a set of measurements (AccX, AccY, RotX, ...) against time. 
 * For computing the standard deviation the mean value is needed. 
 * If it has already been computed, use the overloaded function and pass the mean 
 * as the second parameter for better performance. 
 */
float RoggenFeatureExtraction::getStandardDeviation(int meas) {
	return sqrt(getVariance(meas));
}

/**
 * Returns the mean crossing rate of the buffer for the given measurement. 
 * The buffer holds a set of measurements (AccX, AccY, RotX, ...) against time. 
 * For computing the mean crossing rate the mean value is needed. 
 * If it has already been computed, use the overloaded function and pass the mean 
 * as the second parameter for better performance. 
 */
float RoggenFeatureExtraction::getMeanCrossingRate(int meas) {
	return getMeanCrossingRate(meas,getMean(meas));
}

/**
 * Returns the mean value of the buffer for the given measurement. 
 * The buffer holds a set of measurements (AccX, AccY, RotX, ...) against time. 
 * With the measurement index you choose which measurement to use for calculating the average. 
 * With the second and third parameter the buffer range to use can be defined. 
 */
float RoggenFeatureExtraction::getMean(int meas,int start,int end) {
	float ret = 0;
	buffer->check(start,end);
	for (int i=start; i<=end; ++i) {
		ret += buffer->at(i).at(meas);
	}
	ret /= (end-start+1);
	// cout << "getMean - meas:" << meas << " start:" << start << " end:" << end << " mean:" << ret << endl;
	return ret;
}

/**
 * Returns the variance of the buffer for the given measurement. 
 * The buffer holds a set of measurements (AccX, AccY, RotX, ...) against time. 
 * With the second and third parameter the buffer range to use can be defined. 
 * For computing the variance the mean value is needed. 
 * If it has already been computed, pass it as the last parameter for better performance. 
 * There's also a version of getVariance without the mean parameter. 
 */
float RoggenFeatureExtraction::getVariance(int meas, int start, int end, float mean) {
	float ret = 0;
	buffer->check(start,end);
	for (int i=start; i<=end; ++i) {
		ret += pow(buffer->at(i).at(meas)-mean,2);
	}
	ret /= (end-start+1);
	return ret;
}

/**
 * Returns the standard deviation of the buffer for the given measurement. 
 * The buffer holds a set of measurements (AccX, AccY, RotX, ...) against time. 
 * With the second and third parameter the buffer range to use can be defined. 
 * For computing the standard deviation the mean value is needed. 
 * If it has already been computed, pass it as the last parameter for better performance. 
 * There's also a version of getStandardDeviation without the mean parameter. 
 */
float RoggenFeatureExtraction::getStandardDeviation(int meas, int start, int end, float mean) {
	return sqrt(getVariance(meas,start,end,mean));
}

/**
 * Returns the mean crossing rate of the buffer for the given measurement. 
 * The buffer holds a set of measurements (AccX, AccY, RotX, ...) against time. 
 * With the second and third parameter the buffer range to use can be defined. 
 * For computing the mean crossing rate the mean value is needed. 
 * If it has already been computed, pass it as the last parameter for better performance. 
 * There's also a version of getMeanCrossingRate without the mean parameter. 
 */
float RoggenFeatureExtraction::getMeanCrossingRate(int meas, int start, int end, float mean) {
	int count;
	count = 0;
	float ret;
	ret = 0;
	buffer->check(start,end);
	bool above = false;
	if (buffer->at(start).at(meas) > mean) {
		above = true;
	}
	for (int i=start; i<=end; ++i) {
		if (buffer->at(i).at(meas) > mean + MEAN_CROSSING_THRESHOLD) {
			if (!above) {
				above = true;
				++count;
			}
		}
		if (buffer->at(i).at(meas) < mean - MEAN_CROSSING_THRESHOLD) {
			if (above) {
				above = false;
				++count;
			}
		}		
	}
	ret = count * 100 / (end-start+1); // Crossing rate in % per frame
	return ret;
}

/**
 * Returns the variance of the buffer for the given measurement. 
 * The buffer holds a set of measurements (AccX, AccY, RotX, ...) against time. 
 * With the second and third parameter the buffer range to use can be defined. 
 * For computing the variance the mean value is needed. 
 * If it has already been computed, use the overloaded function and pass the mean 
 * as the last parameter for better performance. 
 */
float RoggenFeatureExtraction::getVariance(int meas, int start, int end) {
	return getVariance(meas,start,end,getMean(meas,start,end));
}

/**
 * Returns the standard deviation of the buffer for the given measurement. 
 * The buffer holds a set of measurements (AccX, AccY, RotX, ...) against time. 
 * With the second and third parameter the buffer range to use can be defined. 
 * For computing the standard deviation the mean value is needed. 
 * If it has already been computed, use the overloaded function and pass the mean 
 * as the last parameter for better performance. 
 */
float RoggenFeatureExtraction::getStandardDeviation(int meas, int start, int end) {
	return sqrt(getVariance(meas,start,end));
}

/**
 * Returns the mean crossing rate of the buffer for the given measurement. 
 * The buffer holds a set of measurements (AccX, AccY, RotX, ...) against time. 
 * With the second and third parameter the buffer range to use can be defined. 
 * For computing the mean crossing rate the mean value is needed. 
 * If it has already been computed, use the overloaded function and pass the mean 
 * as the last parameter for better performance. 
 */
float RoggenFeatureExtraction::getMeanCrossingRate(int meas, int start, int end) {
	return getMeanCrossingRate(meas,start,end,getMean(meas,start,end));
}

/**
 * Static function for computing the mean of a given vector. 
 * The given vector holds a single measurements (like AccX or ...) against time. 
 */
float RoggenFeatureExtraction::getMean(vector<int> vec) {
	float ret;
	for (vector<int>::iterator it=vec.begin(); it!=vec.end(); ++it) {
		ret += *it;
	}
	ret /= vec.size();
	return ret;
}

/**
 * Static function for computing the variance of a given vector. 
 * The given vector holds a single measurements (like AccX or ...) against time. 
 * For the computation the mean value is needed. 
 * If the mean has already been computed, pass it as the last parameter for better performance. 
 * There's also a version of this function without the mean parameter. 
 */
float RoggenFeatureExtraction::getVariance(vector<int> vec, float mean) {
	float ret;
	for (vector<int>::iterator it=vec.begin(); it!=vec.end(); ++it) {
		ret += pow((*it)-mean,2);
	}
	ret /= vec.size();
	return ret;
}

/**
 * Static function for computing the standard deviation of a given vector. 
 * The given vector holds a single measurements (like AccX or ...) against time. 
 * For the computation the mean value is needed. 
 * If the mean has already been computed, pass it as the last parameter for better performance. 
 * There's also a version of this function without the mean parameter. 
 */
float RoggenFeatureExtraction::getStandardDeviation(vector<int> vec, float mean) {
	return sqrt(getVariance(vec,mean));
}

/**
 * Static function for computing the mean crossing rate of a given vector. 
 * The given vector holds a single measurements (like AccX or ...) against time. 
 * For the computation the mean value is needed. 
 * If the mean has already been computed, pass it as the last parameter for better performance. 
 * There's also a version of this function without the mean parameter. 
 */
float RoggenFeatureExtraction::getMeanCrossingRate(vector<int> vec, float mean) {
	int count;
	count = 0;
	float ret;
	ret = 0;
	bool above = false;
	if (vec.front() > mean) {
		above = true;
	}
	for (vector<int>::iterator it=vec.begin(); it!=vec.end(); ++it) {
		if ((*it) > mean + MEAN_CROSSING_THRESHOLD) {
			if (!above) {
				above = true;
				++count;
			}
		}
		if ((*it) < mean - MEAN_CROSSING_THRESHOLD) {
			if (above) {
				above = false;
				++count;
			}
		}		
	}
	ret = count * 100 / vec.size(); // Crossing rate in % per frame
	return ret;
}

/**
 * Static function for computing the variance of a given vector. 
 * The given vector holds a single measurements (like AccX or ...) against time. 
 * For the computation the mean value is needed. 
 * If it has already been computed, use the overloaded function and pass the mean 
 * as the last parameter for better performance. 
 */
float RoggenFeatureExtraction::getVariance(vector<int> vec) {
	return getVariance(vec, getMean(vec));
}

/**
 * Static function for computing the standard deviation of a given vector. 
 * The given vector holds a single measurements (like AccX or ...) against time. 
 * For the computation the mean value is needed. 
 * If it has already been computed, use the overloaded function and pass the mean 
 * as the last parameter for better performance. 
 */
float RoggenFeatureExtraction::getStandardDeviation(vector<int> vec) {
	return sqrt(getVariance(vec));
}

/**
 * Static function for computing the mean crossing rate of a given vector. 
 * The given vector holds a single measurements (like AccX or ...) against time. 
 * For the computation the mean value is needed. 
 * If it has already been computed, use the overloaded function and pass the mean 
 * as the last parameter for better performance. 
 */
float RoggenFeatureExtraction::getMeanCrossingRate(vector<int> vec) {
	return getMeanCrossingRate(vec, getMean(vec));
}

/**
 * Method used for energy based segementation. 
 * Returns true if the variance exceeds a fixed threshold plus hysteresis. 
 * This method just checks for a single measurement. 
 * The buffer holds a set of measurements (AccX, AccY, RotX, ...) against time. 
 * With the second and third parameter the buffer range to use can be defined. 
 */
bool RoggenFeatureExtraction::gotEnergy(int meas, int start, int end) {
	// cout << "start: " << start << "; end: " << end << endl; 
	return getVariance(meas,start,end)>ENERGY_THRESHOLD+ENERGY_HYSTERESIS;
}

/**
 * Method used for energy based segementation. 
 * Returns true if the variance is below a fixed threshold minus hysteresis. 
 * This method just checks for a single measurement. 
 * The buffer holds a set of measurements (AccX, AccY, RotX, ...) against time. 
 * With the second and third parameter the buffer range to use can be defined. 
 */
bool RoggenFeatureExtraction::lostEnergy(int meas, int start, int end) {
	return getVariance(meas,start,end)<ENERGY_THRESHOLD-ENERGY_HYSTERESIS;
}

/**
 * Method used for energy based segementation. 
 * Some segmentation methods don't take a parameter for the measurement index. 
 * They use a prepared list of measuerment indices. 
 * Use this method to set those indices (e.g. 5,6,7). 
 */
void RoggenFeatureExtraction::setIndices(vector<int> ind) {
	indices.clear();
	indices = ind;
}

/**
 * Method used for energy based segementation. 
 * Returns true if the variance is above a fixed threshold plus hysteresis. 
 * With the parameters the buffer range to use can be defined. 
 * For the measurement indices the indices stored by setIndices are used. 
 */
bool RoggenFeatureExtraction::gotEnergy(int start, int end) {
	if (indices.size() < 1) {
		cerr << "No indices stored for energy detection!" << endl;
	}
	for (vector<int>::iterator it=indices.begin(); it!=indices.end(); ++it) {
		if (gotEnergy(*it,start,end)) {
			return true;
		}
	}
	return false;
}

/**
 * Method used for energy based segementation. 
 * Returns true if the variance is below a fixed threshold minus hysteresis. 
 * With the parameters the buffer range to use can be defined. 
 * For the measurement indices the indices stored by setIndices are used. 
 */
bool RoggenFeatureExtraction::lostEnergy(int start, int end) {
	if (indices.size() < 1) {
		cerr << "No indices stored for energy detection!" << endl;
	}
	for (vector<int>::iterator it=indices.begin(); it!=indices.end(); ++it) {
		if (!lostEnergy(*it,start,end)) {
			return false;
		}
	}
	return true;
}

/**
 * Method used for energy based segementation. 
 * Returns true if the variance is above a fixed threshold plus hysteresis. 
 * For the range the whole buffer length is used. 
 * For the measurement indices the indices stored by setIndices are used. 
 */
bool RoggenFeatureExtraction::gotEnergy() {
	return gotEnergy(0,buffer->size()-1);
}

/**
 * Method used for energy based segementation. 
 * Returns true if the variance is below a fixed threshold minus hysteresis. 
 * For the range the whole buffer length is used. 
 * For the measurement indices the indices stored by setIndices are used. 
 */
bool RoggenFeatureExtraction::lostEnergy() {
	return lostEnergy(0,buffer->size()-1);
}

/**
 * Method used for energy based segementation. 
 * Returns true if the variance is above a fixed threshold plus hysteresis. 
 * For the measurement indices the indices stored by setIndices are used. 
 * For the range the given number of the last frames in the buffer is used. 
 */
bool RoggenFeatureExtraction::gotEnergy(int numberOfFrames) {
	int end = buffer->size()-1;
	int begin = end-numberOfFrames+1;
	return gotEnergy(begin,end);
}

/**
 * Method used for energy based segementation. 
 * Returns true if the variance is below a fixed threshold minus hysteresis. 
 * For the measurement indices the indices stored by setIndices are used. 
 * For the range the given number of the last frames in the buffer is used. 
 */
bool RoggenFeatureExtraction::lostEnergy(int numberOfFrames) {
	int end = buffer->size()-1;
	int begin = end-numberOfFrames+1;
	return lostEnergy(begin,end);
}





