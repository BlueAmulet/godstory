//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

// Grab the win32 headers so we can access QPC
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "platform/platformTimer.h"
#include "math/mMath.h"

class Win32Timer : public PlatformTimer
{
private:
   U32 mTickCountCurrent;
   U32 mTickCountNext;
   S64 mPerfCountCurrent;
   S64 mPerfCountNext;
   S64 mFrequency;
   F64 mPerfCountRemainderCurrent;
   F64 mPerfCountRemainderNext;
   bool mUsingPerfCounter;
public:

   Win32Timer()
   {
      mPerfCountRemainderCurrent = 0.0f;

      // Attempt to use QPC for high res timing, otherwise fallback to GTC.
      mUsingPerfCounter = QueryPerformanceFrequency((LARGE_INTEGER *) &mFrequency);
      if(mUsingPerfCounter)
         mUsingPerfCounter = QueryPerformanceCounter((LARGE_INTEGER *) &mPerfCountCurrent);

//	 #pragma message(ENGINE(对于多处理器的情况需要将线程绑定到指定CPU才可以，否则会有时间问题，暂时屏蔽))
//	  //Ray: 多核或者多处理器情况下可能会有问题，在我AMD4000+机器上尤为明显，INTER E6750上没有碰到过
//#ifdef NTJ_SERVER
//	  mUsingPerfCounter = false;
//#endif

	  if(!mUsingPerfCounter)
         mTickCountCurrent = GetTickCount();
   }

   const S32 getElapsedMs()
   {
      if(mUsingPerfCounter)
      {
         // Use QPC, update remainders so we don't leak time, and return the elapsed time.
         QueryPerformanceCounter( (LARGE_INTEGER *) &mPerfCountNext);
         F64 elapsedF64 = (1000.0 * F64(mPerfCountNext - mPerfCountCurrent) / F64(mFrequency));
         elapsedF64 += mPerfCountRemainderCurrent;
         U32 elapsed = (U32)mFloor(elapsedF64);
         mPerfCountRemainderNext = elapsedF64 - F64(elapsed);

         return elapsed;
      }
      else
      {
         // Do something naive with GTC.
         mTickCountNext = GetTickCount();
         return mTickCountNext - mTickCountCurrent;
      }
   }

   const S32 getElapsedUs() { return 0; }

   void reset()
   {
      // Do some simple copying to reset the timer to 0.
      mTickCountCurrent = mTickCountNext;
      mPerfCountCurrent = mPerfCountNext;
      mPerfCountRemainderCurrent = mPerfCountRemainderNext;
   }
};

class Win32HighTimer : public PlatformTimer
{
private:
	S64 mPerfCountCurrent;
	S64 mPerfCountNext;
	S64 mFrequency;
	F64 mPerfCountRemainderCurrent;
	F64 mPerfCountRemainderNext;
	bool mUsingPerfCounter;
public:

	Win32HighTimer()
	{
		mPerfCountRemainderCurrent = 0.0f;
		// Attempt to use QPC for high res timing, otherwise fallback to GTC.
		mUsingPerfCounter = QueryPerformanceFrequency((LARGE_INTEGER *) &mFrequency);
		if(mUsingPerfCounter)
			mUsingPerfCounter = QueryPerformanceCounter((LARGE_INTEGER *) &mPerfCountCurrent);
	}

	const S32 getElapsedMs() { return 0; }

	const S32 getElapsedUs()
	{
		if(mUsingPerfCounter)
		{
			/*
			// Use QPC, update remainders so we don't leak time, and return the elapsed time.
			QueryPerformanceCounter( (LARGE_INTEGER *) &mPerfCountNext);
			F64 elapsedF64 = (1000.0 * F64(mPerfCountNext - mPerfCountCurrent) / F64(mFrequency));
			elapsedF64 += mPerfCountRemainderCurrent;
			U32 elapsed = (U32)mFloor(elapsedF64);
			mPerfCountRemainderNext = elapsedF64 - F64(elapsed);

			return elapsed;
			*/



			// Use QPC, update remainders so we don't leak time, and return the elapsed time.
			QueryPerformanceCounter( (LARGE_INTEGER *) &mPerfCountNext);
			F64 elapsedF64 = (1000000.0 * F64(mPerfCountNext - mPerfCountCurrent) / F64(mFrequency));
			elapsedF64 += mPerfCountRemainderCurrent;
			U32 elapsed = (U32)mFloor(elapsedF64);
			mPerfCountRemainderNext = elapsedF64 - F64(elapsed);

			return elapsed;
		}

		return 0;
	}

	void reset()
	{
		// Do some simple copying to reset the timer to 0.
		mPerfCountCurrent = mPerfCountNext;
		mPerfCountRemainderCurrent = mPerfCountRemainderNext;
	}
};



PlatformTimer *PlatformTimer::create(timer_type type)
{
	if (type == WIN32_LOW_PERFORMANCE_TIMER)
	{
		return new Win32Timer();
	}
	else if (type == WIN32_HIGH_PERFORMANCE_TIMER)
	{
		return new Win32HighTimer();
	}
	else
	{
		return NULL;
	}
}
