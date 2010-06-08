/**
	FrameParser - author: Daniel Roggen
	Version 2.0

	This library provides basic functions to parse fixed-length frame-based data streams. 
	
	It is motivated by the need to decode data streams originating from USB, Bluetooth or serial sensor nodes.
	In such applications, the loss of data samples is not critical to the operation of the whole system, but it is
	important to keep a synchronized decoding of the data stream.
	The use of a frame-based data stream is a simple way to provide a synchronization mechanism.

	A frame-based data stream has the following format:

	<FRAMEHEADER><data0><data1><data2><data3>...(<CHECKSUM><FRAMEFOOTER>)

	With :
		FRAMEHEADER:	A set of characters delimiting the start of the frame.
		data0...n		A set of payload data, each of which may be in a different data format.
		CHECKSUM:		Not implemented - typically a frame checksum to ensure.
		FRAMEFOOTER:	Not implemented - optional - delimits frame end - The checksum or a longer header can play this role.

	The advantages of a frame-based data format are:
		- Frame-based data streams are well suited to devices continuously streaming data from power on.
		- Data can be continuously streamed over a (possibly lossy) communication channel.
		- The frame header identifies the data start: this allows to recover synchronization when the data sink connects to the data source, or if bytes are lost over the channel.
		- It avoids bidirectionnal communication, to e.g. order a "start of data streaming"
		- The complexity is limited, both on the sender and receiver, which makes suitable for micro-controller systems.
		- The data format is flexible: char, short, int, long....

	What this library is not:
		- A way to reliably transport data over a lossy channel: corrupt frames are discarded, 
		  possibly incorrect frames may be decoded, as in any streaming protocol.

	Notes:
		The frame header should not appear in the data payload to avoid synchronization errors. 
		A checksum (not yet implemented here) minimizes synchronization errors by checking frame consistency and rejecting corrupt frames.
		As a checksum is not implemented (yet), a sufficiently long frame header should be selected so that the probability
		that the data payload is identical to the frame header is kept low. 
		Usually, a 5 to 6 bytes frame header is sufficient for most applications.

		Note that even with a checksum, there is no guarantee that synchronization errors are avoided: 
		their probability is simply reduced.
		

	Functions:

	int FrameParser(const char *format,const char *frame,...);
		
		Parses a single synchronized frame 'stream' of format 'format' and returns the decoded
		data items in the variable argument list '...'.
		Assumption: synchronized frame, i.e. the frame header is at the start of the frame.
		
			format:	null-terminated string defining the frame-based data format
			stream:	synchronized data stream (i.e. the frame header is at the start of the stream)
			...:		variable argument list - pointers to ints (int*) that receive the decoded data.
						

		The 'format' string defines the frame header and data format as follows:
			format: "FRAMEHEADER;<data0><data1><data2>..."
		The frame header is delimited by a semicolon from the definition of the data types <data0><data1>....
		Data types have the following format:
			<datan>: [-]c¦s¦S¦i¦I
			-:		indicates that the next item is signed; by default unsigned is assumed.	
			c:		8-bit character
			s:		16-bit short, little endian
			S:		16-bit short, big endian
			i:		32-bit int, little endian
			I:		32-bit int, big endian
		
		Returns:
			0:			No errors
			other:	Error


History:
	1.1	2008-10-09	Converted to class.
	1.1	2007-11-29	Added vectorial version of parser, and many support functions.
	1.0	2007-11-28	First release.
*/



#ifndef __FRAMEPARSER2_H
#define __FRAMEPARSER2_H

#define FRAMEPARSER_NOERROR		0
#define FRAMEPARSER_ERROR_FORMAT	-1

#include <string>
#include <vector>

class FrameParser2
{
	private:
		std::string format;				// format string
		std::string frame;				// placeholder for incoming frame data
		std::string header;				// frame header
		std::vector<bool> sign;			// Sign of channel
		std::vector<bool> littleendian;	// Endianness of channel
		std::vector<int> bpc;  			// Byte per channel
		int framesize,channel;
		int FrameParser_Stats(int &channel,int &framesize,std::string &header,std::vector<bool> &sign,std::vector<bool> &littleendian,std::vector<int> &bpc);
		bool ok;
		
	public:
		FrameParser2(std::string _format);
		~FrameParser2();

		void Status();

		std::vector<std::vector<int> > Parser(const char *data,int n);
		/*int FrameParser(const char *format,const char *frame,...);
		int FrameParserV(const char *format,const char *frame,unsigned int *vector);*/
		bool IsValid();
		int GetFrameSize();
		int GetNumChannels();
		/*int FrameParser_GetFrameSize(const char *format);
		int FrameParser_GetNumChannels(const char *format);
		int FrameParser_IsSigned(const char *format,int c);
		int FrameParser_IsFrame(const char *format,const char *stream);*/

};

#endif



