/**
 * DataStrings.h
 * Helper class to convert between string data and sensor data. 
 * With this class sensor data can be converted to a string. 
 * This is helpful to store it to a text file or send it 
 * over the network. 
 */

#ifndef _DATASTRINGS_HELPER_H_
#define _DATASTRINGS_HELPER_H_

#include "Defines.h"
#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

using namespace std;

class DataStrings {

	private:
		static string dataStringPart1(int,int);
		static string dataStringPart2(NodeAndData,int);

	public:
		static int getNibbles(int, string, int); 
		static int getBytes(int, string, int);
		static unsigned getByte(string, int);
		static int getWord(string, int);
		static unsigned int decodeByte(string code);
		static string encodeByte(unsigned int num);
		static int decodeWord(string code);
		static string encodeWord(int num);
		static int decode(unsigned bit, string code);
		static string encode(unsigned bit, int num);

		static string encodeFloat(float);
		static float  decodeFloat(string);

		static vector<NodeAndData> getData(string);
		static string getDataString(NodeAndData);
		static string getDataString(NodeAndData, int);
		static string getDataString(vector<NodeAndData>, int);

		static string encodeLimbPos(LimbNameCoordinates); 
		static LimbNameCoordinates decodeLimbPos(string); 

		static string roggenEncode(vector<int>);
		static vector<int> roggenDecode(string);

};

#endif // _DATASTRINGS_HELPER_H_



