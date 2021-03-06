#include "bitStream.h"
#include "tVector.h"
#include "Locker.h"

#define MaxPacketDataSize 1000

namespace Base
{

// bitstream utility functions

void BitStream::setStringBuffer(char buffer[256])
{
   stringBuffer = buffer;
}

bool BitStream::bindPacketStream(U8 *buffer,U32 writeSize)
{
	if(!writeSize)
		return false;

	setBuffer(buffer, writeSize);
	setPosition(0);

	return true;
}

// CodeReview WTF is this additional IsEqual? - BJG, 3/29/07

inline bool IsEqual(F32 a, F32 b) { return a == b; }



class HuffmanProcessor
{
   static const U32 csm_charFreqs[256];
   bool   m_tablesBuilt;

   void buildTables();

   struct HuffNode {
      U32 pop;

      S16 index0;
      S16 index1;
   };
   struct HuffLeaf {
      U32 pop;

      U8  numBits;
      U8  symbol;
      U32 code;   // no code should be longer than 32 bits.
   };
   // We have to be a bit careful with these, mSince they are pointers...
   struct HuffWrap {
      HuffNode* pNode;
      HuffLeaf* pLeaf;

     public:
      HuffWrap() : pNode(NULL), pLeaf(NULL) { }

      void set(HuffLeaf* in_leaf) { pNode = NULL; pLeaf = in_leaf; }
      void set(HuffNode* in_node) { pLeaf = NULL; pNode = in_node; }

      U32 getPop() { if (pNode) return pNode->pop; else return pLeaf->pop; }
   };

   Vector<HuffNode> m_huffNodes;
   Vector<HuffLeaf> m_huffLeaves;

   S16 determineIndex(HuffWrap&);

   void generateCodes(BitStream&, S32, S32);

   CMyCriticalSection mLock;

  public:
   HuffmanProcessor() : m_tablesBuilt(false) { }

   static HuffmanProcessor g_huffProcessor;

   bool readHuffBuffer(BitStream* pStream, char* out_pBuffer);
   bool writeHuffBuffer(BitStream* pStream, const char* out_pBuffer, S32 maxLen);
};

HuffmanProcessor HuffmanProcessor::g_huffProcessor;

void BitStream::setBuffer(void *bufPtr, S32 size, S32 maxSize)
{
   dataPtr = (U8 *) bufPtr;
   bitNum = 0;
   bufSize = size;
   maxReadBitNum = size << 3;
   if(maxSize < 0)
      maxSize = size;
   maxWriteBitNum = maxSize << 3;
   error = false;
}

U32 BitStream::getPosition() const
{
   return (bitNum + 7) >> 3;
}


bool BitStream::setPosition(const U32 pos)
{
   bitNum = pos << 3;
   return (true);
}

U32 BitStream::getStreamSize()
{
   return bufSize;
}

U8 *BitStream::getBytePtr()
{
   return dataPtr + getPosition();
}


U32 BitStream::getReadByteSize()
{
   return (maxReadBitNum >> 3) - getPosition();
}

void BitStream::clear()
{
   dMemset(dataPtr, 0, bufSize);
}

void BitStream::writeBits(S32 bitCount, const void *bitPtr)
{
   if(!bitCount)
      return;

   if(bitCount + bitNum > maxWriteBitNum)
   {
      error = true;
      AssertFatal(false, "Out of range write");
      return;
   }

   // [tom, 8/17/2006] This is probably a lot lamer then it needs to be. However,
   // at least it doesnt clobber data or overrun the buffer like the old code did.
   const U8 *ptr = (U8 *)bitPtr;

   for(S32 srcBitNum = 0;srcBitNum < bitCount;srcBitNum++)
   {
      if((*(ptr + (srcBitNum >> 3)) & (1 << (srcBitNum & 0x7))) != 0)
         *(dataPtr + (bitNum >> 3)) |= (1 << (bitNum & 0x7));
      else
         *(dataPtr + (bitNum >> 3)) &= ~(1 << (bitNum & 0x7));
      bitNum++;
   }
}

void BitStream::setBit(S32 bitCount, bool set)
{
   if(set)
      *(dataPtr + (bitCount >> 3)) |= (1 << (bitCount & 0x7));
   else
      *(dataPtr + (bitCount >> 3)) &= ~(1 << (bitCount & 0x7));
}

bool BitStream::testBit(S32 bitCount)
{
   return (*(dataPtr + (bitCount >> 3)) & (1 << (bitCount & 0x7))) != 0;
}

bool BitStream::writeFlag(bool val)
{
   if(bitNum + 1 > maxWriteBitNum)
   {
      error = true;
      AssertFatal(false, "Out of range write");
      return false;
   }
   if(val)
      *(dataPtr + (bitNum >> 3)) |= (1 << (bitNum & 0x7));
   else
      *(dataPtr + (bitNum >> 3)) &= ~(1 << (bitNum & 0x7));
   bitNum++;
   return (val);
}

void BitStream::readBits(S32 bitCount, void *bitPtr)
{
   if(!bitCount)
      return;
   if(bitCount + bitNum > maxReadBitNum)
   {
      error = true;
      //AssertFatal(false, "Out of range read");
      AssertWarn(false, "Out of range read");
      return;
   }
   U8 *stPtr = dataPtr + (bitNum >> 3);
   S32 byteCount = (bitCount + 7) >> 3;

   U8 *ptr = (U8 *) bitPtr;

   S32 downShift = bitNum & 0x7;
   S32 upShift = 8 - downShift;

   U8 curB = *stPtr;
   while(byteCount--)
   {
      U8 nextB = *++stPtr;
      *ptr++ = (curB >> downShift) | (nextB << upShift);
      curB = nextB;
   }

   bitNum += bitCount;
}

bool BitStream::_read(U32 size, void *dataPtr)
{
   readBits(size << 3, dataPtr);
   return true;
}

bool BitStream::_write(U32 size, const void *dataPtr)
{
   writeBits(size << 3, dataPtr);
   return true;
}

S32 BitStream::readInt(S32 bitCount)
{
   S32 ret = 0;
   readBits(bitCount, &ret);
   if(bitCount == 32)
      return ret;
   else
      ret &= (1 << bitCount) - 1;
   return ret;
}

void BitStream::writeInt(S32 val, S32 bitCount)
{
   writeBits(bitCount, &val);
}

void BitStream::writeFloat(F32 f, S32 bitCount)
{
   writeInt((S32)(f * ((1 << bitCount) - 1)), bitCount);
}

F32 BitStream::readFloat(S32 bitCount)
{
   return readInt(bitCount) / F32((1 << bitCount) - 1);
}

void BitStream::writeSignedFloat(F32 f, S32 bitCount)
{
   writeInt((S32)(((f + 1) * .5) * ((1 << bitCount) - 1)), bitCount);
}

F32 BitStream::readSignedFloat(S32 bitCount)
{
   return readInt(bitCount) * 2 / F32((1 << bitCount) - 1) - 1.0f;
}

void BitStream::writeSignedInt(S32 value, S32 bitCount)
{
   if(writeFlag(value < 0))
      writeInt(-value, bitCount - 1);
   else
      writeInt(value, bitCount - 1);
}

S32 BitStream::readSignedInt(S32 bitCount)
{
   if(readFlag())
      return -readInt(bitCount - 1);
   else
      return readInt(bitCount - 1);
}

//----------------------------------------------------------------------------


//------------------------------------------------------------------------------

void BitStream::readString(char *buf,int maxSize)
{
	AssertFatal(maxSize <=256, "Base::BitStream::readString maxSize>256");

   if(stringBuffer)
   {
      if(readFlag())
      {
         S32 offset = readInt(8);
         HuffmanProcessor::g_huffProcessor.readHuffBuffer(this, stringBuffer + offset);
         dStrcpy(buf, stringBuffer);
         return;
      }
   }
   HuffmanProcessor::g_huffProcessor.readHuffBuffer(this, buf);
   if(stringBuffer)
      dStrcpy(stringBuffer, buf);
}

void BitStream::writeString(const char *string, S32 maxLen)
{
   if(!string)
      string = "";
   if(stringBuffer)
   {
      S32 j;
      for(j = 0; j < maxLen && stringBuffer[j] == string[j] && string[j];j++)
         ;
      dStrncpy(stringBuffer, string, maxLen);
      stringBuffer[maxLen] = 0;

      if(writeFlag(j > 2))
      {
         writeInt(j, 8);
         HuffmanProcessor::g_huffProcessor.writeHuffBuffer(this, string + j, maxLen - j);
         return;
      }
   }
   HuffmanProcessor::g_huffProcessor.writeHuffBuffer(this, string, maxLen);
}

void HuffmanProcessor::buildTables()
{
   AssertFatal(m_tablesBuilt == false, "Cannot build tables twice!");
   m_tablesBuilt = true;

   S32 i;

   // First, construct the array of wraps...
   //
   m_huffLeaves.setSize(256);
   m_huffNodes.reserve(256);
   m_huffNodes.increment();
   for (i = 0; i < 256; i++) {
      HuffLeaf& rLeaf = m_huffLeaves[i];

      rLeaf.pop    = csm_charFreqs[i] + 1;
      rLeaf.symbol = U8(i);

      dMemset(&rLeaf.code, 0, sizeof(rLeaf.code));
      rLeaf.numBits = 0;
   }

   S32 currWraps = 256;
   HuffWrap* pWrap = new HuffWrap[256];
   for (i = 0; i < 256; i++) {
      pWrap[i].set(&m_huffLeaves[i]);
   }

   while (currWraps != 1) {
      U32 min1 = 0xfffffffe, min2 = 0xffffffff;
      S32 index1 = -1, index2 = -1;

      for (i = 0; i < currWraps; i++) {
         if (pWrap[i].getPop() < min1) {
            min2   = min1;
            index2 = index1;

            min1   = pWrap[i].getPop();
            index1 = i;
         } else if (pWrap[i].getPop() < min2) {
            min2   = pWrap[i].getPop();
            index2 = i;
         }
      }
      AssertFatal(index1 != -1 && index2 != -1 && index1 != index2, "hrph");

      // Create a node for this...
      m_huffNodes.increment();
      HuffNode& rNode = m_huffNodes.last();
      rNode.pop    = pWrap[index1].getPop() + pWrap[index2].getPop();
      rNode.index0 = determineIndex(pWrap[index1]);
      rNode.index1 = determineIndex(pWrap[index2]);

      S32 mergeIndex = index1 > index2 ? index2 : index1;
      S32 nukeIndex  = index1 > index2 ? index1 : index2;
      pWrap[mergeIndex].set(&rNode);

      if (index2 != (currWraps - 1)) {
         pWrap[nukeIndex] = pWrap[currWraps - 1];
      }
      currWraps--;
   }
   AssertFatal(currWraps == 1, "wrong wraps?");
   AssertFatal(pWrap[0].pNode != NULL && pWrap[0].pLeaf == NULL, "Wrong wrap type!");

   // Ok, now we have one wrap, which is a node.  we need to make sure that this
   //  is the first node in the node list.
   m_huffNodes[0] = *(pWrap[0].pNode);
   delete [] pWrap;

   U32 code = 0;
   BitStream bs(&code, 4);

   generateCodes(bs, 0, 0);
}

void HuffmanProcessor::generateCodes(BitStream& rBS, S32 index, S32 depth)
{
   if (index < 0) {
      // leaf node, copy the code in, and back out...
      HuffLeaf& rLeaf = m_huffLeaves[-(index + 1)];

      dMemcpy(&rLeaf.code, rBS.dataPtr, sizeof(rLeaf.code));
      rLeaf.numBits = depth;
   } else {
      HuffNode& rNode = m_huffNodes[index];

      S32 pos = rBS.getCurPos();

      rBS.writeFlag(false);
      generateCodes(rBS, rNode.index0, depth + 1);

      rBS.setCurPos(pos);
      rBS.writeFlag(true);
      generateCodes(rBS, rNode.index1, depth + 1);

      rBS.setCurPos(pos);
   }
}

S16 HuffmanProcessor::determineIndex(HuffWrap& rWrap)
{
   if (rWrap.pLeaf != NULL) {
      AssertFatal(rWrap.pNode == NULL, "Got a non-NULL pNode in a HuffWrap with a non-NULL leaf.");

      return -((rWrap.pLeaf - m_huffLeaves.address()) + 1);
   } else {
      AssertFatal(rWrap.pNode != NULL, "Got a NULL pNode in a HuffWrap with a NULL leaf.");

      return rWrap.pNode - m_huffNodes.address();
   }
}

bool HuffmanProcessor::readHuffBuffer(BitStream* pStream, char* out_pBuffer)
{
	CLocker lock( mLock );

   if (m_tablesBuilt == false)
      buildTables();

   if (pStream->readFlag()) {
      S32 len = pStream->readInt(8);
      for (S32 i = 0; i < len; i++) {
         S32 index = 0;
         while (true) {
            if (index >= 0) {
               if (pStream->readFlag() == true) {
                  index = m_huffNodes[index].index1;
               } else {
                  index = m_huffNodes[index].index0;
               }
            } else {
               out_pBuffer[i] = m_huffLeaves[-(index+1)].symbol;
               break;
            }
         }
      }
      out_pBuffer[len] = '\0';
      return true;
   } else {
      // Uncompressed string...
      U32 len = pStream->readInt(8);
      pStream->read(len, out_pBuffer);
      out_pBuffer[len] = '\0';
      return true;
   }
}

bool HuffmanProcessor::writeHuffBuffer(BitStream* pStream, const char* out_pBuffer, S32 maxLen)
{
   CLocker lock( mLock );
   
   if (out_pBuffer == NULL) {
      pStream->writeFlag(false);
      pStream->writeInt(0, 8);
      return true;
   }

   if (m_tablesBuilt == false)
      buildTables();

   S32 len = out_pBuffer ? dStrlen(out_pBuffer) : 0;
   AssertWarn(len <= 255, "String TOO long for writeString");
   AssertWarn(len <= 255, out_pBuffer);
   if (len > maxLen)
      len = maxLen;

   S32 numBits = 0;
   S32 i;
   for (i = 0; i < len; i++)
      numBits += m_huffLeaves[(unsigned char)out_pBuffer[i]].numBits;

   if (numBits >= (len * 8)) {
      pStream->writeFlag(false);
      pStream->writeInt(len, 8);
      pStream->write(len, out_pBuffer);
   } else {
      pStream->writeFlag(true);
      pStream->writeInt(len, 8);
      for (i = 0; i < len; i++) {
         HuffLeaf& rLeaf = m_huffLeaves[((unsigned char)out_pBuffer[i])];
         pStream->writeBits(rLeaf.numBits, &rLeaf.code);
      }
   }

   return true;
}

const U32 HuffmanProcessor::csm_charFreqs[256] = {
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
329   ,
21    ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
2809  ,
68    ,
0     ,
27    ,
0     ,
58    ,
3     ,
62    ,
4     ,
7     ,
0     ,
0     ,
15    ,
65    ,
554   ,
3     ,
394   ,
404   ,
189   ,
117   ,
30    ,
51    ,
27    ,
15    ,
34    ,
32    ,
80    ,
1     ,
142   ,
3     ,
142   ,
39    ,
0     ,
144   ,
125   ,
44    ,
122   ,
275   ,
70    ,
135   ,
61    ,
127   ,
8     ,
12    ,
113   ,
246   ,
122   ,
36    ,
185   ,
1     ,
149   ,
309   ,
335   ,
12    ,
11    ,
14    ,
54    ,
151   ,
0     ,
0     ,
2     ,
0     ,
0     ,
211   ,
0     ,
2090  ,
344   ,
736   ,
993   ,
2872  ,
701   ,
605   ,
646   ,
1552  ,
328   ,
305   ,
1240  ,
735   ,
1533  ,
1713  ,
562   ,
3     ,
1775  ,
1149  ,
1469  ,
979   ,
407   ,
553   ,
59    ,
279   ,
31    ,
0     ,
0     ,
0     ,
68    ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0     ,
0
};

};
