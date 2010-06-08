/**
* Class for learning and classification based on feature distances. 
* Algorithm is the nearest cluster center. 
* @author   Lars Widmer www.lawi.ch
*/

#ifndef NEAREST_CLUSTER_CENTER_H
#define NEAREST_CLUSTER_CENTER_H

// #include <math.h>
// #include <string>
// #include <iostream>

#include <vector>

#include "LearningAlgorithm.h"
#include "RoggenFeatureExtraction.h"

/*
#ifdef WIN32
#else
#endif
*/

struct Cloud {
	std::vector<Features> data;
	OutputType value;
};

class NearestClusterCenter : public LearningAlgorithm {

	private:
		float unknownThreshold;
		FeatureSet centers; 
		std::vector<Cloud> clouds;

		void setUnknownThreshold(); // e.g. calculate 150% of biggest feature distance
		bool cloudsHave(OutputType);
		int cloudsIndexOf(OutputType);
		void addSetToCloud(FeatureSet);
		void addCloudCenters();
		float distance(Features, Features);
		std::pair<int,float> match(Features);

	public:
    NearestClusterCenter();
    NearestClusterCenter(FeatureSet);
		virtual ~NearestClusterCenter();

		virtual void reset();
		virtual void train(FeatureSet);
		virtual Output classify(Features);
		virtual std::pair<Output,FeatureType> classification(Features);
		virtual void setUnknownThreshold(float);
		virtual float getUnknownThreshold();
		virtual void print();

};

#endif // NEAREST_CLUSTER_CENTER_H


