/**
* Super class for learning and classification based on various algorithms. 
* @author   Lars Widmer www.lawi.ch
*/

#ifndef LEARNING_ALGORITHM_H
#define LEARNING_ALGORITHM_H

// #include <math.h>
// #include <string>
// #include <iostream>
#include <vector>

#include "RoggenBuffer.h"

/*
#ifdef WIN32
#else
#endif
*/

typedef int OutputType;

typedef int InputType;

typedef float FeatureType;

struct Output {
	OutputType value;
	bool valid;
};

typedef std::vector<InputType> Input;

struct Sample {
	Input in;
	OutputType out;
};

typedef std::vector<Sample> Set;

typedef std::vector<FeatureType> Features;

struct FeatureSample {
	Features in;
	OutputType out;
};

typedef std::vector<FeatureSample> FeatureSet;

class LearningAlgorithm {

	public:
		virtual ~LearningAlgorithm();
		virtual void reset() = 0;
		virtual void train(FeatureSet) = 0;
		virtual Output classify(Features) = 0;
		virtual std::pair<Output,FeatureType> classification(Features) = 0;
		virtual void setUnknownThreshold(float) = 0;
		virtual float getUnknownThreshold() = 0;
		virtual void print() = 0;

};

#endif // LEARNING_ALGORITHM_H


