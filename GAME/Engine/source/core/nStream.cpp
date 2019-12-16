//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "core/stream.h"
#include "core/stringTable.h"
#include "core/color.h"
#include "platform/platformNet.h"
#include "core/coreRes.h"
#include "math/mPoint.h"
#include "math/mMatrix.h"

Stream::Stream()
 : m_streamStatus(Closed)
{
   //
}

Stream::~Stream()
{
   //
}

const char* Stream::getStatusString(const Status in_status)
{
   switch (in_status) {
      case Ok:
         return "StreamOk";
      case IOError:
         return "StreamIOError";
      case EOS:
         return "StreamEOS";
      case IllegalCall:
         return "StreamIllegalCall";
      case Closed:
         return "StreamClosed";
      case UnknownError:
         return "StreamUnknownError";

     default:
      return "Invalid Stream::Status";
   }
}

void Stream::writeString(const char *string, S32 maxLen)
{
   S32 len = string ? dStrlen(string) : 0;
   if(len > maxLen)
      len = maxLen;

   write(U8(len));
   if(len)
      write(len, string);
}

void Stream::readString(char *buf,int maxLen)
{
   U8 len;
   read(&len);
   AssertRelease(len<maxLen,"Stream::readString");
   read(S32(len), buf);
   buf[len] = 0;
}

const char *Stream::readSTString(bool casesens)
{
   char buf[256];
   readString(buf,256);
   return StringTable->insert(buf, casesens);
}

void Stream::readLongString(U32 maxStringLen, char *stringBuf)
{
   U32 len;
   read(&len);
   if(len > maxStringLen)
   {
      m_streamStatus = IOError;
      return;
   }
   read(len, stringBuf);
   stringBuf[len] = 0;
}

void Stream::writeLongString(U32 maxStringLen, const char *string)
{
   U32 len = dStrlen(string);
   if(len > maxStringLen)
      len = maxStringLen;
   write(len);
   write(len, string);
}

void Stream::readLine(U8 *buffer, U32 bufferSize)
{
   bufferSize--;  // account for NULL terminator
   U8 *buff = buffer;
   U8 *buffEnd = buff + bufferSize;
   *buff = '\r';

   // strip off preceding white space
   while ( *buff == '\r' )
   {
      if ( !read(buff) || *buff == '\n' )
      {
         *buff = 0;
         return;
      }
   }

   // read line
   while ( buff != buffEnd && read(++buff) && *buff != '\n' )
   {
      if ( *buff == '\r' )
      {

#if defined(POWER_OS_MAC)
      U32 pushPos = getPosition(); // in case we need to back up.
      if (read(buff)) // feeling free to overwrite the \r as the NULL below will overwrite again...
	      if (*buff != '\n') // then push our position back.
	         setPosition(pushPos);
	   break; // we're always done after seeing the CR...
#else
      buff--; // 'erases' the CR of a CRLF
#endif

      }
   }
   *buff = 0;
}

void Stream::writeLine(const U8 *buffer)
{
   write(dStrlen((StringTableEntry)buffer), buffer);
   write(2, "\r\n");
}

bool Stream::write(const ColorI& rColor)
{
   bool success = write(rColor.red);
   success     |= write(rColor.green);
   success     |= write(rColor.blue);
   success     |= write(rColor.alpha);

   return success;
}

bool Stream::write(const ColorF& rColor)
{
   //ColorI temp = rColor;
   //return write(temp);

	bool success = write(rColor.red);
	success     |= write(rColor.green);
	success     |= write(rColor.blue);
	success     |= write(rColor.alpha);

	return success;
}

bool Stream::read(ColorI* pColor)
{
   bool success = read(&pColor->red);
   success     |= read(&pColor->green);
   success     |= read(&pColor->blue);
   success     |= read(&pColor->alpha);

   return success;
}

bool Stream::read(ColorF* pColor)
{
   //ColorI temp;
   //bool success = read(&temp);

   //*pColor = temp;
   //return success;

	bool success = read(&pColor->red);
	success     |= read(&pColor->green);
	success     |= read(&pColor->blue);
	success     |= read(&pColor->alpha);
	return success;
}

bool Stream::write(const Point3F& pos)
{
	bool success = write(pos.x);
	success     |= write(pos.y);
	success     |= write(pos.z);

	return success;
}

bool Stream::read(Point3F* pPos)
{
	bool success = read(&pPos->x);
	success     |= read(&pPos->y);
	success     |= read(&pPos->z);
	return success;
}

bool Stream::write(const MatrixF& mat)
{
	bool success;
	Point3F Pos;

	for(int i=0;i<4;i++)
	{
		mat.getColumn(i,&Pos);
		success |= write(Pos);
	}

	return success;
}

bool Stream::read(MatrixF* pMat)
{
	bool success;
	Point3F Pos;

	for(int i=0;i<4;i++)
	{
		success |= read(&Pos);
		pMat->setColumn(i,Pos);
	}

	return success;
}

bool Stream::write(const NetAddress &na)
{
   bool success = write(na.type);
   success &= write(na.port);
   success &= write(na.netNum[0]);
   success &= write(na.netNum[1]);
   success &= write(na.netNum[2]);
   success &= write(na.netNum[3]);
   success &= write(na.nodeNum[0]);
   success &= write(na.nodeNum[1]);
   success &= write(na.nodeNum[2]);
   success &= write(na.nodeNum[3]);
   success &= write(na.nodeNum[4]);
   success &= write(na.nodeNum[5]);
   return success;
}

bool Stream::read(NetAddress *na)
{
   bool success = read(&na->type);
   success &= read(&na->port);
   success &= read(&na->netNum[0]);
   success &= read(&na->netNum[1]);
   success &= read(&na->netNum[2]);
   success &= read(&na->netNum[3]);
   success &= read(&na->nodeNum[0]);
   success &= read(&na->nodeNum[1]);
   success &= read(&na->nodeNum[2]);
   success &= read(&na->nodeNum[3]);
   success &= read(&na->nodeNum[4]);
   success &= read(&na->nodeNum[5]);
   return success;
}

bool Stream::write(const RawData &rd)
{
   bool s = write(rd.size);
   s &= write(rd.size, rd.data);
   return s;
}

bool Stream::read(RawData *rd)
{
   U32 size = 0;
   bool s = read(&size);

   rd->alloc(size);
   s &= read(rd->size, rd->data);

   return s;
}

bool Stream::copyFrom(Stream *other)
{
   U8 buffer[1024];
   U32 numBytes = other->getStreamSize() - other->getPosition();
   while((other->getStatus() != Stream::EOS) && numBytes > 0)
   {
      U32 numRead = numBytes > sizeof(buffer) ? sizeof(buffer) : numBytes;
      if(! other->read(numRead, buffer))
         return false;

      if(! write(numRead, buffer))
         return false;

      numBytes -= numRead;
   }

   return true;
}
