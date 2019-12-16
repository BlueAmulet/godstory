//------------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com
//------------------------------------------------------------------------------
#include <d3d8.h>
#include "platform/platform.h"
#include "gfx/gfxEnums.h"

//------------------------------------------------------------------------------

namespace GFXD3D8EnumTranslate
{
   void init();
};

//------------------------------------------------------------------------------

extern _D3DFORMAT GFXD3D8IndexFormat[GFXIndexFormat_COUNT];
extern _D3DTEXTURESTAGESTATETYPE GFXD3D8SamplerState[GFXSAMP_COUNT]; // "Sampler State" is rolled into TSS for 8.1
extern _D3DFORMAT GFXD3D8TextureFormat[GFXFormat_COUNT];
extern _D3DRENDERSTATETYPE GFXD3D8RenderState[GFXRenderState_COUNT];
extern _D3DTEXTUREFILTERTYPE GFXD3D8TextureFilter[GFXTextureFilter_COUNT];
extern _D3DBLEND GFXD3D8Blend[GFXBlend_COUNT];
extern _D3DBLENDOP GFXD3D8BlendOp[GFXBlendOp_COUNT];
extern _D3DSTENCILOP GFXD3D8StencilOp[GFXStencilOp_COUNT];
extern _D3DCMPFUNC GFXD3D8CmpFunc[GFXCmp_COUNT];
extern _D3DCULL GFXD3D8CullMode[GFXCull_COUNT];
extern _D3DPRIMITIVETYPE GFXD3D8PrimType[GFXPT_COUNT];
extern _D3DTEXTURESTAGESTATETYPE GFXD3D8TextureStageState[GFXTSS_COUNT];
extern _D3DTEXTUREADDRESS GFXD3D8TextureAddress[GFXAddress_COUNT];
extern _D3DTEXTUREOP GFXD3D8TextureOp[GFXTOP_COUNT];

#define GFXREVERSE_LOOKUP( tablearray, enumprefix, val ) \
   for( int i = enumprefix##_FIRST; i < enumprefix##_COUNT; i++ ) \
      if( (int)tablearray##[i] == val ) \
      { \
         val = i; \
         break; \
      } \

