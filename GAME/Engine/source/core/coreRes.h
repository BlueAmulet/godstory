//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _CORE_CORERES_H_
#define _CORE_CORERES_H_

class RawData
{
private:
   bool ownMemory;

public:
   char *data;
   U32 size;

   RawData(const RawData &rd)
   {
      data = rd.data;
      size = rd.size;
      ownMemory = false;
   }

   RawData()
      : ownMemory(false), data(NULL), size(0)
   { 
   }

   ~RawData() 
   {
      reset();
   }

   void reset()
   {
      if (ownMemory)
         delete [] data;
      data      = NULL;
      ownMemory = false;
      size      = 0;
   }

   void alloc(const U32 newSize)
   {
      reset();

      ownMemory = true;
      size = newSize;
      data = new char[newSize];
   }

   void operator =(const RawData &rd)
   {
      data = rd.data;
      size = rd.size;
      ownMemory = false;
   }
};

#endif //_CORERES_H_

