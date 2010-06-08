#include "DataStrings.h"

#define NIBBLES_PER_ROGGEN_VALUE 4

/**
 * Helper method 
 */
string DataStrings::dataStringPart1(int id, int size) {
	string socketData = "S ";
	socketData += encodeByte(id);
	socketData += ";";
	socketData += encodeByte(size);
	socketData += ";";
	return socketData;
}

/**
 * Helper method 
 */
string DataStrings::dataStringPart2(NodeAndData nad, int position) {
	// compose data for socket transmission
	// encode running sensor number
	char buf[32];
	#ifdef WIN32
	// sprintf_s(buf, "%.*i", 4, position);
	sprintf(buf, "%.*i", 4, position);
	#else
	sprintf(buf, "%.*i", 4, position);
	#endif
	string socketData;
	socketData = buf;
	socketData += ";";
	socketData += encodeByte(nad.node);
	socketData += ";";
	socketData += encodeByte(nad.data.valid);
	socketData += ";";
	socketData += encodeWord(static_cast<int>(nad.data.accX));
	socketData += ";";
	socketData += encodeWord(static_cast<int>(nad.data.accY));
	socketData += ";";
	socketData += encodeWord(static_cast<int>(nad.data.accZ));
	socketData += ";";
	socketData += encodeWord(static_cast<int>(nad.data.magX));
	socketData += ";";
	socketData += encodeWord(static_cast<int>(nad.data.magY));
	socketData += ";";
	socketData += encodeWord(static_cast<int>(nad.data.magZ));
	socketData += ";";
	return socketData;			
}

/**
 * Helper method 
 */
string DataStrings::getDataString(NodeAndData nad) {
	string dataString;
	dataString = dataStringPart1(0,1);
	dataString += dataStringPart2(nad,0);
	return dataString;
}

/**
 * Helper method 
 */
string DataStrings::getDataString(NodeAndData nad, int id) {
	string dataString;
	dataString = dataStringPart1(id,1);
	dataString += dataStringPart2(nad,0);
	return dataString;
}

/**
 * Helper method 
 */
string DataStrings::getDataString(vector<NodeAndData> nads, int id) {
	string dataString;
	dataString = dataStringPart1(id,nads.size());
	for (int i=0; i<nads.size(); i++) {
		dataString += dataStringPart2(nads.at(i),i);
	}
	return dataString;
}

/**
 * Helper method to encode a byte for sending as a text. 
 * In the output string only the characters from A to P are used. 
 * Therefore one byte takes two characters to store. 
 * In return we have loads of escape characters left and 
 * we don't have to be afraid of eof occuring in the data stream. 
 * Values bigger then 255 aren't accepted.
 * In other words only the lowest 8 Bits are wanted.
 * Unsigned numbers are expected!
 */
string DataStrings::encodeByte(unsigned int num) {
	int n = static_cast<int>(num);
	n -= 128;
	return encode(8,n);
}

/**
 * Helper method to decode a byte for sending as a text.
 * In the input string only the characters from A to P can be used.
 * Therefore one byte is stored in two characters.
 */
unsigned int DataStrings::decodeByte(string code) {
	int n = decode(8,code);
	n += 128;
	return static_cast<unsigned>(n);
}

/**
 * Helper method to encode a word for sending as a text.
 * We assume 16bit values.
 * This is what the standard says about the minimum size of int.
 * Therefore the function exits if there accur larger values.
 * This function handles values from -32768 to 32767.
 */
string DataStrings::encodeWord(int num) {
	return encode(16,num);
}

/**
 * Helper method to decode a word after receiving it as a text.
 * We assume 16bit values.
 * This is what the standard says about the minimum size of int.
 * Therefore the function exits if there accur larger values.
 * This function returns values from -32768 to 32767.
 */
int DataStrings::decodeWord(string code) {
	return decode(16,code);
}

/**
 * Helper method to encode a small float number for sending as a text.
 * We send 16bit.
 * Legal are values between -30.0 and 30.0.
 * The function exits if there accur larger values.
 * We transmit 3 digits after the comma.
 */
string DataStrings::encodeFloat(float num) {
	if (num > 30 || num < -30) {
		cerr << "Absolute to large value for encodeFloat: " << num << endl;
		exit(1);
	}
	float mul = num * 1000;
	int mulInt = static_cast<int>(mul);
	return encodeWord(mulInt);
}

/**
 * Helper method to decode a small float after receiving it as a text.
 * We receive 16bit.
 * Legal are values between -30.0 and 30.0.
 * We deal with 3 digits after the comma.
 */
float DataStrings::decodeFloat(string code) {
	int mulInt = decode(16,code);
	float mul = static_cast<float>(mulInt);
	float flo = mul / 1000;
	return flo;
}

/**
 * Encodes a given value of a given number of bytes.
 * Every 4 bits are encoded in one character.
 * Therefore the given number of bits has to be a
 * multiple of 4.
 */
string DataStrings::encode(unsigned bit, int num) {
	if (bit%4 != 0) {
		cerr << "encode: bit must be a multiple of 4 because every 4 bits get encoded to a single letter. bit=" << bit << endl;
		exit(1);
	}
	unsigned bitpow = static_cast<unsigned>(pow((double)2,(int)(bit-1)));
	if (num > (int)bitpow-1 || num < -((int)bitpow)) {
		cerr << "Absolute too large number for encode: " << num << " into number of Bits: " << bit << endl;
		num = 0;
		// exit(1);
	}
	unsigned unum = static_cast<unsigned>(num + bitpow);
	unsigned mask = 0x0000000F;
	mask = mask << (bit-4);
	string toSend = "";
	for (unsigned i=0; i<bit/4; i++) {
		unsigned a = unum&mask;
		mask = mask >> 4;
		a = a >> (bit-4-i*4);
		a += 65;
		char ac = static_cast<char>(a);
		toSend += ac;
	}
	// cout << "encode: " << toSend << "; unum:" << unum << "; num:" << num << ";  bit:" << bit << endl;
	return toSend;
}

/**
 * Decodes a given string of a given number of bytes.
 * Every character gets decoded into 4 bits. 
 * Therefore the given number of bits has to be a 
 * multiple of 4.  
 */
int DataStrings::decode(unsigned bit, string code) {
	if (bit%4 != 0) {
		cerr << "decode: bit must be a multiple of 4 because every 4 bits get encoded to a single letter. bit=" << bit << endl;
		exit(1);
	}
	if (code.length() != bit/4) {
		cerr << "Decode: Wrong code length. Expected " << bit/4 << " characters. " << code << endl;
		exit(1);
	}
	unsigned unum = 0;
	for (unsigned i=0; i<bit/4; i++) {
		char ac = code.at(i);
		unsigned a = static_cast<unsigned>(ac);
		a -= 65;
		a = a << (bit-4-i*4);
		unum += a;
	}
	int num = static_cast<int>(unum);
	unsigned bitpow = static_cast<unsigned>(pow((double)2,(int)(bit-1)));
	return(num-bitpow);
}

/**
 * Extracts a number of bytes out of a string.
 * Every byte there is encoded into two characters from A to P.
 * This method returns positive and negative numbers.
 */
int DataStrings::getBytes(int numOfBytes, string str, int start) {
	return getNibbles(numOfBytes*2,str,start);
	/*
	numOfBytes *= 2;
	if (str.length() < start+numOfBytes) {
		cerr << "Corrupted server data, premature end of data" << endl;
		exit(1);
	}
	if (str.at(start+numOfBytes) != ';') {
		cerr << "Corrupted server data, missing semicolon" << endl;
		exit(1);
	}
	return decode(numOfBytes*4,str.substr(start,numOfBytes));
	*/
}

/**
 * Extracts a number of nibbles (half bytes; 4 bits) out of a string.
 * Every byte there is encoded into two characters from A to P.
 * This method returns positive and negative numbers.
 */
int DataStrings::getNibbles(int numOfNibbles, string str, int start) {
	if (str.length() < start+numOfNibbles) {
		cerr << "Corrupted server data, premature end of data" << endl;
		exit(1);
	}
	if (str.at(start+numOfNibbles) != ';') {
		cerr << "Corrupted server data, missing semicolon" << endl;
		exit(1);
	}
	return decode(numOfNibbles*4,str.substr(start,numOfNibbles));
}

/**
 * Extracts an unsigned number ouf of a string.
 * The length is fixed to one byte.
 * Therefore the number is between 0 and 255.
 * In the string this is a value between AA and PP.
 */
unsigned DataStrings::getByte(string str, int start) {
	if (str.length() < start+2) {
		cerr << "Corrupted server data, premature end of data" << endl;
		exit(1);
	}
	if (str.at(start+2) != ';') {
		cerr << "Corrupted server data, missing semicolon" << endl;
		exit(1);
	}
	return decodeByte(str.substr(start,2));
}

/**
 * Extracts a 2 byte value out of a string.
 * Every byte there is encoded into two characters.
 * Therefore the given string must be four characters long.
 * This method returns positive and negative numbers. 
 */
int DataStrings::getWord(string str, int start) {
	return getBytes(2,str,start);
}

/**
 * Extracts the sensor data out of the given string.
 */
vector<NodeAndData> DataStrings::getData(string data) {
	vector<NodeAndData> sensorData;
	sensorData.clear();
	int pos = 0;
	if (data.substr(0,2) != "S ") {
		cerr << "Refresh: Got invalid opening character in string from server!" << endl;
		cerr << "Refresh: " << data << endl;
		// exit(1);
		return sensorData;
	} else {
		pos += 2;
	}
	int sensorNetworkID = getByte(data,pos); pos += 3;
	int numberOfNodes = getByte(data,pos); pos += 3;
	for (int i=0; i<numberOfNodes; i++) {
		NodeData nd;
		pos += 5; // jump over serial number
		int node = getByte(data,pos); pos += 3;
		nd.valid = (getByte(data,pos) == 1); pos += 3;
		nd.accX = getWord(data,pos); pos += 5;
		nd.accY = getWord(data,pos); pos += 5;
		nd.accZ = getWord(data,pos); pos += 5;
		nd.magX = getWord(data,pos); pos += 5;
		nd.magY = getWord(data,pos); pos += 5;
		nd.magZ = getWord(data,pos); pos += 5;

		NodeAndData nad;
		nad.node = node;
		nad.data = nd;
		sensorData.push_back(nad);
	}
	return sensorData;
}

/**
 * Encodes the name and position of a limb into a string.
 */
string DataStrings::encodeLimbPos(LimbNameCoordinates data) {
	string str = "LNC;";
	str += data.name;
	str += ";";
	str += DataStrings::encodeFloat(data.x);
	str += ";";
	str += DataStrings::encodeFloat(data.y);
	str += ";";
	str += DataStrings::encodeFloat(data.z);
	return str;
}

/**
 * Extracts the name and position of a limb out of the given string.
 */
LimbNameCoordinates DataStrings::decodeLimbPos(string data) {
	LimbNameCoordinates lnc;
	int pos = 0;
	if (data.substr(0,4) != "LNC;") {
		cerr << "decodeLimbPos: Got invalid opening character in string from server!" << endl;
		cerr << "decodeLimbPos: " << data << endl;
		// exit(1);
		return lnc;
	} else {
		pos += 4;
	}
	int pos2 = data.find(";",pos);
	// cout << data << " has second ; at " << pos2 << endl;
	lnc.name = data.substr(pos,pos2-pos);
	// cout << "name is " << lnc.name << endl;
	pos = pos2+1;
	lnc.x = decodeFloat(data.substr(pos,4)); pos += 5; 
	lnc.y = decodeFloat(data.substr(pos,4)); pos += 5; 
	lnc.z = decodeFloat(data.substr(pos,4)); pos += 5; 
	return lnc;
}

/**
 * Encodes a vector of int (used as stream frame for the roggen sensors) into a string. 
 */
string DataStrings::roggenEncode(vector<int> data) {
	string ret;
	ret.clear();
	ret = "R ";
	ret += DataStrings::encodeByte(data.size());
	ret += ";";
	for (vector<int>::iterator it=data.begin(); it!=data.end(); ++it) {
		ret += DataStrings::encode(NIBBLES_PER_ROGGEN_VALUE*4,*it);
		ret += ";";
	}
	return ret;
}

/**
 * Decodes a string back to vector of int (used as stream frame for the roggen sensors). 
 */
vector<int> DataStrings::roggenDecode(string str) {
	vector<int> v;
	v.clear();
	int start = 0;
	if (str.substr(0,2) == "R ") {
		start += 2;
		int length = DataStrings::getByte(str,start);
		start += 3;
		for (int i=0; i<length; ++i) {
			v.push_back(DataStrings::getNibbles(NIBBLES_PER_ROGGEN_VALUE,str,start));
			start += NIBBLES_PER_ROGGEN_VALUE+1;
		}
	}
	return v;
}


