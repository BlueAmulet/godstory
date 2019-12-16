//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _MRANDOM_H_
#define _MRANDOM_H_

#include "core/color.h"
#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#include "math/mPoint.h"

//--------------------------------------
/// Base class for random number generators
class MRandomGenerator
{
protected:
   MRandomGenerator() {}
   S32  mSeed;

public:
   void setSeed();
   S32  getSeed() { return mSeed; }
   virtual void setSeed(S32 s) = 0;

   virtual U32 randI( void ) = 0;      ///< 0..2^31 random number generator
   virtual F32  randF( void );         ///< 0.0 .. 1.0 F32 random number generator
   S32 randI(S32 i, S32 n);            ///< i..n integer random number generator
   F32 randF(F32 i, F32 n);            ///< i..n F32 random number generator
   Point3F randP3(Point3F i, Point3F n);
   ColorF  randC(ColorF i, ColorF n);
};


//--------------------------------------
inline F32 MRandomGenerator::randF()
{
   // default: multiply by 1/(2^31)
   return  F32(randI()) * (1.0f/2147483647.0f);
}

inline S32 MRandomGenerator::randI(S32 i, S32 n)
{
   AssertFatal(i<=n, "MRandomGenerator::randi: inverted range.");
   return (S32)(i + (randI() % (n - i + 1)) );
}

inline F32 MRandomGenerator::randF(F32 i, F32 n)
{
   AssertFatal(i<=n, "MRandomGenerator::randf: inverted range.");
   return (i + (n - i) * randF());
}

inline Point3F MRandomGenerator::randP3(Point3F i, Point3F n)
{
	Point3F temp;
	AssertFatal(i.x<=n.x, "MRandomGenerator::randp3: inverted range.");
	temp.x = (i.x + (n.x - i.x) * randF());
    AssertFatal(i.y<=n.y, "MRandomGenerator::randp3: inverted range.");
    temp.y = (i.y + (n.y - i.y) * randF());
    AssertFatal(i.z<=n.z, "MRandomGenerator::randp3: inverted range.");
    temp.z = (i.z + (n.z - i.z) * randF());
    return temp;
}

inline ColorF MRandomGenerator::randC(ColorF i, ColorF n)
{
	ColorF temp;
	AssertFatal(i.red<=n.red, "MRandomGenerator::randp3: inverted range.");
	temp.red = (i.red + (n.red - i.red) * randF());
	AssertFatal(i.green<=n.green, "MRandomGenerator::randp3: inverted range.");
	temp.green = (i.green + (n.green - i.green) * randF());
	AssertFatal(i.blue<=n.blue, "MRandomGenerator::randp3: inverted range.");
	temp.blue = (i.blue + (n.blue - i.blue) * randF());
	AssertFatal(i.alpha<=n.alpha, "MRandomGenerator::randp3: inverted range.");
	temp.alpha = (i.alpha + (n.alpha - i.alpha) * randF());
	return temp;
}

//--------------------------------------
/// Linear Congruential Method, the "minimal standard generator"
///
/// Fast, farly good random numbers (better than using rand)
///
/// @author Park & Miller, 1988, Comm of the ACM, 31(10), pp. 1192-1201
class MRandomLCG : public MRandomGenerator
{
protected:
   static const S32 msQuotient;
   static const S32 msRemainder;

public:
   MRandomLCG();
   MRandomLCG(S32 s);

   static void setGlobalRandSeed(U32 seed);

   void setSeed(S32 s);
//   using MRandomGenerator::randI;
    S32 randI(S32 i, S32 n);            ///< i..n integer generator

   U32 randI( void );

};

// Solution to "using" problem.
inline S32 MRandomLCG::randI(S32 i, S32 n)
{
    return( MRandomGenerator::randI(i,n) );
}


//--------------------------------------
/// Fast, very good random numbers
///
/// Period = 2^249
///
/// Kirkpatrick, S., and E. Stoll, 1981; A Very Fast Shift-Register
///       Sequence Random Number Generator, Journal of Computational Physics,
/// V. 40.
///
/// Maier, W.L., 1991; A Fast Pseudo Random Number Generator,
/// Dr. Dobb's Journal, May, pp. 152 - 157
class MRandomR250: public MRandomGenerator
{
private:
   U32 mBuffer[250];
   S32 mIndex;

public:
   MRandomR250();
   MRandomR250(S32 s);

   void setSeed(S32 s);
// using MRandomGenerator::randI;
   U32 randI();
};


typedef MRandomLCG MRandom;

extern MRandomLCG gRandGen;


#endif //_MRANDOM_H_
