#include "types.h"
#include "stream.h"

namespace Base
{

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
   read(S32(len), buf);
   buf[len] = 0;
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

}//namespace base