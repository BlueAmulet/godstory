//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _STRINGBUFFER_H_
#define _STRINGBUFFER_H_

//Includes
#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

#include "console/console.h"

/// Utility class to wrap string manipulation in a representation
/// independent way.
///
/// Length does NOT include the null terminator.
class StringBuffer
{
   Vector<UTF16>  mBuffer;
   mutable Vector<UTF8>   mBuffer8;
   mutable bool mDirty8;

public:
   #if defined(POWER_DEBUG)
   typedef struct RequestCounts
   {
      U64 requestCount8;
      U64 requestCount16;
   };
   RequestCounts *rc;
   #endif
   
   StringBuffer();
   StringBuffer(const StringBuffer &copy);
   StringBuffer(const StringBuffer *in);
   StringBuffer(const UTF8 *in);
   StringBuffer(const UTF16 *in);

   ~StringBuffer();

   void append(const StringBuffer &in);
   void append(const UTF8*  in);
   void append(const UTF16* in);
   void append(const UTF16* in, U32 len);
   
   void insert(const U32 charOffset, const StringBuffer &in);
   void insert(const U32 charOffset, const UTF8*  in);
   void insert(const U32 charOffset, const UTF16* in);
   void insert(const U32 charOffset, const UTF16* in, const U32 len);
   
   /// Get a StringBuffer substring of length 'len' starting from 'start'.
   /// Returns a new StringBuffer by value;
   StringBuffer substring(const U32 start, const U32 len) const;

   /// Get a pointer to a substring of length 'len' starting from 'start'.
   /// Returns a raw pointer to a unicode string.
   /// You must delete[] the returned string when you are done with it.
   /// This follows the "create rule".
   UTF8*        createSubstring8(const U32 start, const U32 len) const;
   UTF16*       createSubstring16(const U32 start, const U32 len) const;
   
   void    cut(const U32 start, const U32 len);
//   UTF8*   cut8(const U32 start, const U32 len);
//   UTF16*  cut16(const U32 start, const U32 len);

   const UTF16 getChar(const U32 offset) const;
   void setChar(const U32 offset, UTF16 c);
   
   void set(const StringBuffer *in);
   void set(const UTF8  *in);
   void set(const UTF16 *in);

   inline const U32 length() const 
   { 
      return mBuffer.size() - 1; // Don't count the NULL of course.
   }

   /// Get an upper bound size estimate for a UTF8 buffer to hold this
   /// string.
   const U32 getUTF8BufferSizeEstimate() const
   {
      return length() * 3 + 1;
   }

   void getCopy8(UTF8 *buff, const U32 buffSize) const;
   void getCopy(UTF16 *buff, const U32 buffSize) const;
   
   /// Get a copy of the contents of the string buffer.
   /// You must delete[] the returned copy when you are done with it.
   /// This follows the "create rule".
   UTF8*  createCopy8() const;
   UTF16* createCopy() const;
   
   /// Get a pointer to the StringBuffer's data store.
   /// Use this in situations where you can be sure that the StringBuffer will
   /// not be modified out from under you.
   /// The win here is, you avoid yet another data copy. Data copy is slow on
   /// most modern hardware.
   const UTF16* getPtr() const;
   const UTF8*  getPtr8() const;

private:
   void updateBuffer8();
   #if defined(POWER_DEBUG)
   void clearRequestCounts() { rc->requestCount16 = 0; rc->requestCount8 = 0; }
   #endif
   
};

#endif