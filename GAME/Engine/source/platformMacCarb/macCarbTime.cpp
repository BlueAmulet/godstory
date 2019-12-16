//-----------------------------------------------------------------------------
// Torque Game Engine Advanced
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "PlatformMacCarb/platformMacCarb.h"
#include <time.h>
#include <unistd.h>

#pragma message("time code needs eval -- may not be accurate in all cases, and might cause performance hit.")

//--------------------------------------
void Platform::getLocalTime(LocalTime &lt)
{
   struct tm systime;
   time_t long_time;

   /// Get time as long integer.
   time( &long_time );
   /// Convert to local time, thread safe.
   localtime_r( &long_time, &systime );
   
   /// Fill the return struct
   lt.sec      = systime.tm_sec;
   lt.min      = systime.tm_min;
   lt.hour     = systime.tm_hour;
   lt.month    = systime.tm_mon;
   lt.monthday = systime.tm_mday;
   lt.weekday  = systime.tm_wday;
   lt.year     = systime.tm_year;
   lt.yearday  = systime.tm_yday;
   lt.isdst    = systime.tm_isdst;
}   

/// Gets the time in seconds since the Epoch
U32 Platform::getTime()
{
   time_t epoch_time;
   time( &epoch_time );
   return epoch_time;
}   

/// Gets the time in milliseconds since some epoch. In this case, system start time.
/// Storing milliseconds in a U32 overflows every 49.71 days
U32 Platform::getRealMilliseconds()
{
   // Duration is a S32 value.
   // if negative, it is in microseconds.
   // if positive, it is in milliseconds.
   Duration durTime = AbsoluteToDuration(UpTime());
   U32 ret;
   if( durTime < 0 )
      ret = durTime / -1000;
   else 
      ret = durTime;

   return ret;
}   

U32 Platform::getVirtualMilliseconds()
{
   return platState.currentTime;   
}   

void Platform::advanceTime(U32 delta)
{
   platState.currentTime += delta;
}   

/// Asks the operating system to put the process to sleep for at least ms milliseconds
void Platform::sleep(U32 ms)
{
    // note: this will overflow if you want to sleep for more than 49 days. just so ye know.
    usleep( ms * 1000 );
}
