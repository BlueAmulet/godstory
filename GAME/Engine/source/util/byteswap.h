//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef BYTESWAP
#define BYTESWAP(x, y) x = x ^ y; y = x ^ y; x = x ^y;
#endif //defined(BYTESWAP)