#ifndef MASK_EX_H
#define MASK_EX_H

#include "../../Engine/source/platform/types.h"

#define BITEX(p) (((U64)1<<(32+p)))

namespace MaskEx
{
	const U64 TeamMask			= BITEX(0);
}

#endif