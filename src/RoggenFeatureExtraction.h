/**
 * Class to handle the data processing of Daniel Roggen Sensors. 
 * The feature extraction works on a buffer. 
 * It offers easy access to many types of features. 
 * Energy based segmentation is supported as well. 
 * Author: Lars Widmer, www.lawi.ch
 */

#ifndef _ROGGEN_FEATURE_EXTRACTION_H_
#define _ROGGEN_FEATURE_EXTRACTION_H_

#include <string>
#include <vector>

#include "RoggenBuffer.h"

class RoggenFeatureExtraction {

private: 
	RoggenBuffer * buffer;
	std::vector<int> indices;

	void init();
	void cleanUp();

public: 
	// Constructors
	// RoggenFeatureExtraction(); 
	RoggenFeatureExtraction(RoggenBuffer *); 
	virtual ~RoggenFeatureExtraction(); 

	// public methods
	float getMean(int);
	float getVariance(int,float);
	float getVariance(int);
	float getStandardDeviation(int,float);
	float getStandardDeviation(int);
	float getMeanCrossingRate(int,float);
	float getMeanCrossingRate(int);

	float getMean(int,int,int);
	float getVariance(int,int,int,float);
	float getStandardDeviation(int,int,int,float);
	float getMeanCrossingRate(int,int,int,float);
	float getVariance(int,int,int);
	float getStandardDeviation(int,int,int);
	float getMeanCrossingRate(int,int,int);

	static float getMean(std::vector<int>);
	static float getVariance(std::vector<int>,float);
	static float getStandardDeviation(std::vector<int>,float);
	static float getMeanCrossingRate(std::vector<int>,float);
	static float getVariance(std::vector<int>);
	static float getStandardDeviation(std::vector<int>);
	static float getMeanCrossingRate(std::vector<int>);

	void setIndices(std::vector<int>);

	bool gotEnergy(int, int, int);
	bool lostEnergy(int, int, int);
	bool gotEnergy(int, int);
	bool lostEnergy(int, int);
	bool gotEnergy();
	bool lostEnergy();
	bool gotEnergy(int);
	bool lostEnergy(int);

};

#endif // _ROGGEN_FEATURE_EXTRACTION_H_






