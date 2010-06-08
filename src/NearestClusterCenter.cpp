#include <iostream>

#include "NearestClusterCenter.h"
#include "Helper.h"

using namespace std;

/**
 * Constructor: Clears all the internal fields and leaves the algorithm untrained. 
 */
NearestClusterCenter::NearestClusterCenter() {
	reset();
}

/**
 * Constructor: Clears all the internal fields and trains the algorithm with the given set of features. 
 */
NearestClusterCenter::NearestClusterCenter(FeatureSet s) {
	reset();
	train(s);
}

/**
 * Empty Destructor
 */
NearestClusterCenter::~NearestClusterCenter() {
	// ...
}

/**
 * Clears the internal fields e.g. resets the training state to untrained. 
 */
void NearestClusterCenter::reset() {
	centers.clear();
	clouds.clear();
}

/**
 * Returns if the algorithm has been trained for the given output value. 
 */
bool NearestClusterCenter::cloudsHave(OutputType out) {
	if (cloudsIndexOf(out) == -1) {
		return false;
	}
	return true;
}

/**
 * Returns the index of the cloud (cluster) of training data identified by the given output value. 
 * Return value -1 means not found in cloud vector. 
 */
int NearestClusterCenter::cloudsIndexOf(OutputType out) {
	int i = 0;
	int ret = -1;
	for (vector<Cloud>::iterator it=clouds.begin(); it!=clouds.end(); ++it) {
		if (it->value == out) {
			ret = i;
		}
		++i;		
	}
	return ret;
}

/**
 * Method to add a set of training data (features) to the internal data clouds (clusters). 
 */
void NearestClusterCenter::addSetToCloud(FeatureSet set) {
	for (FeatureSet::iterator it=set.begin(); it!=set.end(); ++it) {
		int index = cloudsIndexOf(it->out);
		if (index == -1) {
			Cloud c;
			c.value = it->out;
			c.data.clear();
			c.data.push_back(it->in);
			clouds.push_back(c);
			index = cloudsIndexOf(it->out);
			if (index == -1) {
				cerr << "NearestClusterCenter::train adding cloud failed!" << endl;
			}
		} else {
			clouds.at(index).data.push_back(it->in);
		}
	}
}

/**
 * This function goes through the clouds (clusters) of sorted training data. 
 * From this data center points are computed an stored which will be used for the future classification. 
 */
void NearestClusterCenter::addCloudCenters() {
	for (vector<Cloud>::iterator i1=clouds.begin(); i1!=clouds.end(); ++i1) {
		FeatureSample s;
		s.out = i1->value;
		Features averages;
		averages.clear();
		for (vector<Features>::iterator i2=i1->data.begin(); i2!=i1->data.end(); ++i2) {
			for (int i=0; i < i2->size(); ++i) {
				if (averages.size() <= i) {
					averages.push_back(i2->at(i));
				} else {
					averages.at(i) += i2->at(i);
				}
			}
		}
		int size = i1->data.size();
		for (Features::iterator i3=averages.begin(); i3!=averages.end(); ++i3) {
			*i3 /= size;
		}
		s.in = averages;
		centers.push_back(s);
	}	
}

/**
 * Trains the algorithm with the given training set. 
 */
void NearestClusterCenter::train(FeatureSet set) {
	reset();
	addSetToCloud(set);
	addCloudCenters();
	setUnknownThreshold();
}

/**
 * Function to compute the onedimensional euler distance between two points in feature space. 
 */
FeatureType NearestClusterCenter::distance(Features a, Features b) {
	return Helper::vectorLength(Helper::vectorDifference(a,b));
}

/**
 * Function matching a point in feature space to its nearest cluster/cloud center. 
 */
pair<OutputType,FeatureType> NearestClusterCenter::match(Features in) {
	if (centers.size() < 1) {
		cerr << "NearestClusterCenter::classify: No center points stored! Did you train?" << endl;
		exit(1);
	}
	pair<OutputType,FeatureType> p;
	p.first = centers.front().out;
	p.second = distance(centers.front().in,in);
	for (FeatureSet::iterator it=centers.begin(); it!=centers.end(); ++it) {
		FeatureType dist = distance(it->in,in);
		if (dist < p.second) {
			p.second = dist;
			p.first = it->out;
		}
	}
	return p;
}

/**
 * Classification function.
 * Matches the given point in feature space to the nearest cluster center. 
 * The classification is marked valid depending if the distance
 * between the point and the center is less then the defined threshold. 
 * The first value of the output pair is the found classification in an Output struct. 
 * The Output struct contains a flag if the classification is valid and the value of the classification. 
 * The second value of the output pair tells the distance between the given point and the matched cluster center. 
 */
pair<Output,FeatureType> NearestClusterCenter::classification(Features in) {
	pair<OutputType,FeatureType> p = match(in);
	Output o;
	o.value = p.first;
	o.valid = (p.second < unknownThreshold);
	pair<Output,FeatureType> pof;
	pof.first = o;
	pof.second = p.second;
	return pof;
}

/**
 * Classification function.
 * Matches the given point in feature space to the nearest cluster center. 
 * The classification is marked valid depending if the distance
 * between the point and the center is less then the defined threshold. 
 * The return value is the found classification as an Output struct. 
 * The Output struct contains a flag if the classification is valid and the value of the classification. 
 */
Output NearestClusterCenter::classify(Features in) {
	return classification(in).first;
}

/**
 * Automatically sets the classification threshold. 
 * If a point in feature space is classified with a distance from the 
 * nearest cluster bigger then the threshold it's marked as invalid. 
 * The function calculates the biggest distance between any two cluster centers. 
 */
void NearestClusterCenter::setUnknownThreshold() {
	unknownThreshold = 0;
	// find the biggest distance between two cluster centers
	for (FeatureSet::iterator i1=centers.begin(); i1!=centers.end(); ++i1) {
		for (FeatureSet::iterator i2=centers.begin(); i2!=centers.end(); ++i2) {
			float dist = distance(i1->in,i2->in);
			if (dist > unknownThreshold) {
				unknownThreshold = dist;
			}
		}
	}
}

/**
 * Setter function for the classification threshold. 
 * If a point in feature space is classified with a distance from the 
 * nearest cluster bigger then the threshold it's marked as invalid. 
 */
void NearestClusterCenter::setUnknownThreshold(float newUT) {
	unknownThreshold = newUT;	
}

/**
 * Getter function for the classification threshold. 
 * If a point in feature space is classified with a distance from the 
 * nearest cluster bigger then the threshold it's marked as invalid. 
 */
float NearestClusterCenter::getUnknownThreshold() {
	return unknownThreshold;	
}

/**
 * Print function for the set of cluster centers. 
 */
void NearestClusterCenter::print() {
	cout << "NCC CENTERS:" << endl;
	Helper::getInstance()->setRowSize(10);
	for (FeatureSet::iterator it=centers.begin(); it!=centers.end(); ++it) {
		Helper::getInstance()->echo(it->in);
		cout << " ==> " << it->out << endl;
	}
}



