#ifndef _BASE_BITSTREAM_H_
#define _BASE_BITSTREAM_H_

//Includes
#include "types.h"
#include "stream.h"
#include "mMathFn.h"

namespace Base
{
const int Bit8  = 8;
const int Bit16 = 16;
const int Bit32 = 32;
const int Bit64 = 64;
const int Bit128 = 128;
//-------------------------------------- Some caveats when using this class:
//                                        - Get/setPosition semantics are changed
//                                         to indicate bit position rather than
//                                         byte position.
//
class HuffmanProcessor;

class BitStream : public Stream
{
protected:
   U8 *dataPtr;
   S32  bitNum;
   S32  bufSize;
   bool error;
   S32  maxReadBitNum;
   S32  maxWriteBitNum;
   char *stringBuffer;

   friend class HuffmanProcessor;
public:
   bool bindPacketStream(U8 *buffer,U32 writeSize);

   void setBuffer(void *bufPtr, S32 bufSize, S32 maxSize = 0);
   U8*  getBuffer() { return dataPtr; }
   U8*  getBytePtr();

   U32 getReadByteSize();

   S32  getCurPos() const;
   void setCurPos(const U32);

   BitStream(void *bufPtr, S32 bufSize, S32 maxWriteSize = -1) { setBuffer(bufPtr, bufSize,maxWriteSize); stringBuffer = NULL; }
   void clear();

   void setStringBuffer(char buffer[256]);
   void writeInt(S32 value, S32 bitCount);
   S32  readInt(S32 bitCount);

   /// Use this method to write out values in a concise but ass backwards way...
   /// Good for values you expect to be frequently zero, often small. Worst case
   /// this will bloat values by nearly 20% (5 extra bits!) Best case you'll get
   /// one bit (if it's zero).
   ///
   /// This is not so much for efficiency's sake, as to make life painful for
   /// people that want to reverse engineer our network or file formats.
   void writeCussedU32(U32 val)
   {
      // Is it zero?
      if(writeFlag(val == 0))
         return;

      if(writeFlag(val <= 0xF)) // 4 bit
         writeRangedU32(val, 0, 0xF);
      else if(writeFlag(val <= 0xFF)) // 8 bit
         writeRangedU32(val, 0, 0xFF);
      else if(writeFlag(val <= 0xFFFF)) // 16 bit
         writeRangedU32(val, 0, 0xFFFF);
      else if(writeFlag(val <= 0xFFFFFF)) // 24 bit
         writeRangedU32(val, 0, 0xFFFFFF);
      else
         writeRangedU32(val, 0, 0xFFFFFFFF);
   }

   U32 readCussedU32()
   {
      if(readFlag())
         return 0;

      if(readFlag())
         return readRangedU32(0, 0xF);
      else if(readFlag())
         return readRangedU32(0, 0xFF);
      else if(readFlag())
         return readRangedU32(0, 0xFFFF);
      else if(readFlag())
         return readRangedU32(0, 0xFFFFFF);
      else
         return readRangedU32(0, 0xFFFFFFFF);
   }

   void writeSignedInt(S32 value, S32 bitCount);
   S32  readSignedInt(S32 bitCount);

   void writeRangedU32(U32 value, U32 rangeStart, U32 rangeEnd);
   U32  readRangedU32(U32 rangeStart, U32 rangeEnd);
   
   /// Writes a clamped signed integer to the stream using 
   /// an optimal amount of bits for the range.
   void writeRangedS32( S32 value, S32 min, S32 max );

   /// Reads a ranged signed integer written with writeRangedS32.
   S32 readRangedS32( S32 min, S32 max );

   // read and write floats... floats are 0 to 1 inclusive, signed floats are -1 to 1 inclusive

   F32  readFloat(S32 bitCount);
   F32  readSignedFloat(S32 bitCount);

   void writeFloat(F32 f, S32 bitCount);
   void writeSignedFloat(F32 f, S32 bitCount);

   /// Writes a clamped floating point value to the 
   /// stream with the desired bits of precision.
   void writeRangedF32( F32 value, F32 min, F32 max, U32 numBits );

   /// Reads a ranged floating point value written with writeRangedF32.
   F32 readRangedF32( F32 min, F32 max, U32 numBits );

   virtual void writeBits(S32 bitCount, const void *bitPtr);
   virtual void readBits(S32 bitCount, void *bitPtr);
   virtual bool writeFlag(bool val);
   
/*
   inline bool writeFlag(U32 val)
   {
      return writeFlag(val != 0);
   }*/


   inline bool writeFlag(void *val)
   {
      return writeFlag(val != 0);
   }

   virtual bool readFlag();

   void setBit(S32 bitCount, bool set);
   bool testBit(S32 bitCount);

   bool isFull() { return bitNum > (bufSize << 3); }
   bool isValid() { return !error; }

   bool _read (const U32 size,void* d);
   bool _write(const U32 size,const void* d);

   void readString(char *stringBuf,int maxSize);
   void writeString(const char *stringBuf, S32 maxLen=255);

   bool hasCapability(const Capability) const { return true; }
   U32  getPosition() const;
   bool setPosition(const U32 in_newPosition);
   U32  getStreamSize();
};

//------------------------------------------------------------------------------
//-------------------------------------- INLINES
//
inline S32 BitStream::getCurPos() const
{
   return bitNum;
}

inline void BitStream::setCurPos(const U32 in_position)
{
   AssertFatal(in_position < (U32)(bufSize << 3), "Out of range bitposition");
   bitNum = S32(in_position);
}

inline bool BitStream::readFlag()
{
   if(bitNum > maxReadBitNum)
   {
      error = true;
      AssertFatal(false, "Out of range read");
      return false;
   }
   S32 mask = 1 << (bitNum & 0x7);
   bool ret = (*(dataPtr + (bitNum >> 3)) & mask) != 0;
   bitNum++;
   return ret;
}

inline void BitStream::writeRangedU32(U32 value, U32 rangeStart, U32 rangeEnd)
{
   AssertFatal(value >= rangeStart && value <= rangeEnd, "Out of bounds value!");
   AssertFatal(rangeEnd >= rangeStart, "error, end of range less than start");

   U32 rangeSize = rangeEnd - rangeStart + 1;
   U32 rangeBits = getBinLog2(getNextPow2(rangeSize));

   writeInt(S32(value - rangeStart), S32(rangeBits));
}

inline U32 BitStream::readRangedU32(U32 rangeStart, U32 rangeEnd)
{
   AssertFatal(rangeEnd >= rangeStart, "error, end of range less than start");

   U32 rangeSize = rangeEnd - rangeStart + 1;
   U32 rangeBits = getBinLog2(getNextPow2(rangeSize));

   U32 val = U32(readInt(S32(rangeBits)));
   return val + rangeStart;
}

inline void BitStream::writeRangedS32( S32 value, S32 min, S32 max )
{
   value = mClamp( value, min, max );
   writeRangedU32( ( value - min ), 0, ( max - min ) );
}

inline S32 BitStream::readRangedS32( S32 min, S32 max )
{
   return readRangedU32( 0, ( max - min ) ) + min;
}

inline void BitStream::writeRangedF32( F32 value, F32 min, F32 max, U32 numBits )
{
   value = ( mClampF( value, min, max ) - min ) / ( max - min );
   writeInt( (S32)mFloor(value * F32( (1 << numBits) - 1 )), numBits );
}

inline F32 BitStream::readRangedF32( F32 min, F32 max, U32 numBits )
{
   F32 value = (F32)readInt( numBits );
   value /= F32( ( 1 << numBits ) - 1 );
   return min + value * ( max - min );
}

}

#endif //_BITSTREAM_H_
