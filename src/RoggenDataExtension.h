/**
 * Implements the same interface as the Sensor class itself. 
 * Therefore RoggenDataExtension and RoggenSensor can substitute themselfes. 
 * The idea of the extension is to compute additional values and append them
 * to the original sensor measurements. 
 */

#ifndef _ROGGEN_SENSOR_DATA_EXTENSION_H_
#define _ROGGEN_SENSOR_DATA_EXTENSION_H_

#include "IntSource.h"

#include <vector>

class RoggenDataExtension : public IntSource {

private: 
	IntSource * source;

public: 
	RoggenDataExtension(IntSource *);
	virtual ~RoggenDataExtension();

	virtual std::vector<std::vector<int> > getData();

};

#endif // _ROGGEN_SENSOR_DATA_EXTENSION_H_


