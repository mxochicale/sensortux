/*
	FrameParser - author: Daniel Roggen
	Version 2.0
*/

#include <stdio.h>
//#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "FrameParser2.h"


FrameParser2::FrameParser2(std::string _format)
{
	// Assign the string
	format=_format;
	
	// Check if the format is valid and get the number of channels and buffer length
	int r= FrameParser_Stats(channel,framesize,header,sign,littleendian,bpc);
	if(r==FRAMEPARSER_NOERROR)
	{
		ok=true;
		frame.resize(framesize,0);
	}
	else
	{
		ok=false;
	}
	
	//Status();
}
FrameParser2::~FrameParser2()
{


}
void FrameParser2::Status()
{
	printf("FrameParser Status\n");
	printf("ok: %d\n",(int)ok);
	printf("Channel: %d\n",channel);
	printf("Framesize: %d\n",framesize);
	printf("Header: %s\n",header.c_str());
	printf("Sign: ");
	for(int i=0;i<sign.size();i++)
		printf("%d ",(int)sign[i]);
	printf("\n");
	printf("LittleEndian: ");
	for(int i=0;i<littleendian.size();i++)
		printf("%d ",(int)littleendian[i]);
	printf("\n");
	printf("Bpc: ");
	for(int i=0;i<bpc.size();i++)
		printf("%d ",(int)bpc[i]);
	printf("\n");
	
}
int FrameParser2::GetFrameSize()
{
	return ok?framesize:-1;
}
int FrameParser2::GetNumChannels()
{
	return ok?channel:-1;
}

/*
	Parse the data up to n bytes and decode frames as they are found.
	Returns the number of frames decoded.
*/
std::vector<std::vector<int> > FrameParser2::Parser(const char *data,int n)
{
	unsigned int dataunsigned;
	std::vector<std::vector<int> > v;
	std::vector<int> channels(channel,0);	
	if(!ok)
		return v;
	

	for(int i=0;i<n;i++)
	{
		// Feed in a byte.
		char *framedata = (char*)frame.data();
		memmove(framedata,framedata+1,framesize-1);
		framedata[framesize-1]=data[i];
	
		if(strncmp(framedata,header.data(),header.size()) == 0)
		{
			// Found a frame - decode it
			unsigned char *dataptr = (unsigned char*)framedata+header.size();
			
			for(int c=0;c<channel;c++)
			{
				
				switch(bpc[c])
				{
				case 1:
					dataunsigned = *dataptr++;
					if(sign[c] && dataunsigned&0x80)
						dataunsigned|=0xFFFFFF00;
					break;
				case 2:
					if(littleendian[c])
					{
						dataunsigned = *dataptr++;
						dataunsigned |= (*dataptr++)<<8;
					}
					else
					{
						dataunsigned = (*dataptr++)<<8;
						dataunsigned |= *dataptr++;
					}
					if(sign[c] && dataunsigned&0x8000)
						dataunsigned|=0xFFFF0000;
					break;
				case 4:
					if(littleendian[c])
					{
						dataunsigned = *dataptr++;
						dataunsigned |= (*dataptr++)<<8;
						dataunsigned |= (*dataptr++)<<16;
						dataunsigned |= (*dataptr++)<<24;
					}
					else
					{
						dataunsigned = (*dataptr++)<<24;
						dataunsigned |= (*dataptr++)<<16;
						dataunsigned |= (*dataptr++)<<8;
						dataunsigned |= *dataptr++;							
					}
					break;
				default:
					assert(0);
					break;
				} // switch
				channels[c]=dataunsigned;
			} // for channel
			v.push_back(channels);
		} // if header

	} // for input data 
	return v;
}
/*int FrameParser::FrameParser(const char *format,const char *frame,...)
{
	unsigned frameheaderlen,channel;
	char type,*t;
	unsigned int *vaptr;
	const unsigned char *dataptr;
	unsigned int dataunsigned;
	const char *formatptr;
	

	va_list ap;
	va_start(ap,frame);

	
	
	// Find frame header length
	t=(char*)strchr(format,';');
	if(t==0)
	{
		assert(t);
		return FRAMEPARSER_ERROR_FORMAT;
	}
	frameheaderlen=t-format;

	//printf("Length of header: %d\n",frameheaderlen);

	// Iterate the channels and decode the data
	dataptr = ((const unsigned char*)frame)+frameheaderlen;
	formatptr=format+frameheaderlen+1;
	channel=0;
	while(1)
	{
		
		bool issigned;
		// Get next data type
		type = *formatptr++;
		if(type==0)
			break;		// Last item processed -> return.
		if(type=='-')
		{
			// Signed element. We have to read the data type.
			issigned=true;
			type = *formatptr++;
			if(type==0)
			{
				va_end(ap);
				assert(0);
				return FRAMEPARSER_ERROR_FORMAT;		// Error: we have a sign, but no type specifier -> crash.
			}
		}
		else
			issigned=false;
		

		// Assume all arguments passed as 32-bits - that's a documentation requirement.
		vaptr = va_arg(ap,unsigned int *);		

		switch(type)
		{
			case 'c':
				dataunsigned = *dataptr++;
				if(issigned && dataunsigned&0x80)
					dataunsigned|=0xFFFFFF00;
				break;
			case 's':
				dataunsigned = *dataptr++;
				dataunsigned |= (*dataptr++)<<8;
				if(issigned && dataunsigned&0x8000)
					dataunsigned|=0xFFFF0000;
				break;
			case 'S':
				dataunsigned = (*dataptr++)<<8;
				dataunsigned |= *dataptr++;
				if(issigned && dataunsigned&0x8000)
					dataunsigned|=0xFFFF0000;
				break;
			case 'i':
				dataunsigned = *dataptr++;
				dataunsigned |= (*dataptr++)<<8;
				dataunsigned |= (*dataptr++)<<16;
				dataunsigned |= (*dataptr++)<<24;
				break;
			case 'I':
				dataunsigned = (*dataptr++)<<24;
				dataunsigned |= (*dataptr++)<<16;
				dataunsigned |= (*dataptr++)<<8;
				dataunsigned |= *dataptr++;
				break;
			default:
				va_end(ap);
				assert(0);
				return FRAMEPARSER_ERROR_FORMAT;
		}
		*vaptr=dataunsigned;
		channel++;
	}	// do
	
	va_end(ap);
	return FRAMEPARSER_NOERROR;
}
int FrameParser::FrameParserV(const char *format,const char *frame,unsigned int *vector)
{
	unsigned frameheaderlen,channel;
	char type,*t;
	const unsigned char *dataptr;
	unsigned int dataunsigned;
	const char *formatptr;
	unsigned int *vaptr;
	
	
	// Find frame header length
	t=(char*)strchr(format,';');
	if(t==0)
	{
		assert(t);
		return FRAMEPARSER_ERROR_FORMAT;
	}
	frameheaderlen=t-format;

	//printf("Length of header: %d\n",frameheaderlen);

	// Iterate the channels and decode the data
	dataptr = ((const unsigned char*)frame)+frameheaderlen;
	formatptr=format+frameheaderlen+1;
	channel=0;
	while(1)
	{
		
		bool issigned;
		// Get next data type
		type = *formatptr++;
		if(type==0)
			break;		// Last item processed -> return.
		if(type=='-')
		{
			// Signed element. We have to read the data type.
			issigned=true;
			type = *formatptr++;
			if(type==0)
			{
				assert(0);
				return FRAMEPARSER_ERROR_FORMAT;		// Error: we have a sign, but no type specifier -> crash.
			}
		}
		else
			issigned=false;
		
		switch(type)
		{
			case 'c':
				dataunsigned = *dataptr++;
				if(issigned && dataunsigned&0x80)
					dataunsigned|=0xFFFFFF00;
				break;
			case 's':
				dataunsigned = *dataptr++;
				dataunsigned |= (*dataptr++)<<8;
				if(issigned && dataunsigned&0x8000)
					dataunsigned|=0xFFFF0000;
				break;
			case 'S':
				dataunsigned = (*dataptr++)<<8;
				dataunsigned |= *dataptr++;
				if(issigned && dataunsigned&0x8000)
					dataunsigned|=0xFFFF0000;
				break;
			case 'i':
				dataunsigned = *dataptr++;
				dataunsigned |= (*dataptr++)<<8;
				dataunsigned |= (*dataptr++)<<16;
				dataunsigned |= (*dataptr++)<<24;
				break;
			case 'I':
				dataunsigned = (*dataptr++)<<24;
				dataunsigned |= (*dataptr++)<<16;
				dataunsigned |= (*dataptr++)<<8;
				dataunsigned |= *dataptr++;
				break;
			default:
				assert(0);
				return FRAMEPARSER_ERROR_FORMAT;
		}
		vector[channel]=dataunsigned;
		channel++;
	}	// do
	
	return FRAMEPARSER_NOERROR;
}
int FrameParser::FrameParser_GetFrameSize(const char *format)
{
	int c,s;
	int rv;
	rv=FrameParser_Stats(format,c,s);
	if(rv!=FRAMEPARSER_NOERROR)
		return rv;
	return s;
}
int FrameParser::FrameParser_GetNumChannels(const char *format)
{
	int c,s;
	int rv;
	rv=FrameParser_Stats(format,c,s);
	if(rv!=FRAMEPARSER_NOERROR)
		return rv;
	return c;
}
*/
bool FrameParser2::IsValid()
{
	return ok;
}
/*
int FrameParser::FrameParser_IsFrame(const char *format,const char *stream)
{
	unsigned frameheaderlen;
	char type,*t;
	const char *formatptr;

// Find frame header length
	t=(char*)strchr(format,';');
	if(t==0)
	{
		assert(t);
		return FRAMEPARSER_ERROR_FORMAT;
	}
	frameheaderlen=t-format;
	
	if(strncmp(stream,format,frameheaderlen) == 0)
		return 1;
	return 0;
}
int FrameParser::FrameParser_IsSigned(const char *format,int c)
{
	unsigned int channel;
	unsigned frameheaderlen;
	char type,*t;
	const char *formatptr;

// Find frame header length
	t=(char*)strchr(format,';');
	if(t==0)
	{
		return FRAMEPARSER_ERROR_FORMAT;
	}
	frameheaderlen=t-format;

	// Decode the format string
	formatptr=format+frameheaderlen+1;
	channel=0;
	while(1)
	{
		bool issigned;
		// Get next data type
		type = *formatptr++;
		if(type==0)
			break;		// Last item processed -> return.
		if(type=='-')
		{
			issigned=true;
			// Signed element. We have to read the data type.
			type = *formatptr++;
			if(type==0)
			{
				assert(0);
				return FRAMEPARSER_ERROR_FORMAT;		// Error: we have a sign, but no type specifier -> crash.
			}
		}
		else
			issigned=false;
		// Don't care about type; just return signed or not.
		if(channel==c)
			return issigned;
		
		channel++;
	}	// do
	return FRAMEPARSER_ERROR_FORMAT;
}

*/
int FrameParser2::FrameParser_Stats(int &channel,int &framesize,std::string &header,std::vector<bool> &sign,std::vector<bool> &littleendian,std::vector<int> &bpc)
{
	unsigned frameheaderlen;
	char type;
	unsigned formatptr;

// Find frame header length
	//t=(char*)strchr(format,';');
	size_t t = format.find_first_of(';');
	if(t==std::string::npos || t==0)
		return FRAMEPARSER_ERROR_FORMAT;
	
	// keep the header
	header=format.substr(0,t);
	
	frameheaderlen=t;

	// Decode the format string
	formatptr=frameheaderlen+1;
	channel=0;
	framesize=frameheaderlen;
	while(1)
	{
		// Get next data type
		type = format[formatptr++];
		if(type==0)
			break;		// Last item processed -> return.
		if(type=='-')
		{
			sign.push_back(true);
			// Signed element. We have to read the data type.
			type=format[formatptr++];
			if(type==0)
			{
				assert(0);
				return FRAMEPARSER_ERROR_FORMAT;		// Error: we have a sign, but no type specifier -> crash.
			}
		}
		else
			sign.push_back(false);
	
		switch(type)
		{
			case 'c':
				framesize+=1;
				littleendian.push_back(true);
				bpc.push_back(1);
				break;
			case 's':
				framesize+=2;
				littleendian.push_back(true);
				bpc.push_back(2);
				break;
			case 'S':
				framesize+=2;
				littleendian.push_back(false);
				bpc.push_back(2);
				break;
			case 'i':
				framesize+=4;
				littleendian.push_back(true);
				bpc.push_back(4);
				break;
			case 'I':
				framesize+=4;
				littleendian.push_back(false);
				bpc.push_back(4);
				break;
			default:
				printf("Invalid format spec!\n");
				//assert(0);
				printf("Invalid format spec! after assert\n");
				return FRAMEPARSER_ERROR_FORMAT;
		}
		channel++;
	}	// do
	return FRAMEPARSER_NOERROR;
}










