//------------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com
//------------------------------------------------------------------------------

#include "gfx/D3D8/gfxD3D8EnumTranslate.h"
#include "console/console.h"

//------------------------------------------------------------------------------

_D3DFORMAT GFXD3D8IndexFormat[GFXIndexFormat_COUNT];
_D3DTEXTURESTAGESTATETYPE GFXD3D8SamplerState[GFXSAMP_COUNT];
_D3DFORMAT GFXD3D8TextureFormat[GFXFormat_COUNT];
_D3DRENDERSTATETYPE GFXD3D8RenderState[GFXRenderState_COUNT];
_D3DTEXTUREFILTERTYPE GFXD3D8TextureFilter[GFXTextureFilter_COUNT];
_D3DBLEND GFXD3D8Blend[GFXBlend_COUNT];
_D3DBLENDOP GFXD3D8BlendOp[GFXBlendOp_COUNT]; 
_D3DSTENCILOP GFXD3D8StencilOp[GFXStencilOp_COUNT];
_D3DCMPFUNC GFXD3D8CmpFunc[GFXCmp_COUNT];
_D3DCULL GFXD3D8CullMode[GFXCull_COUNT];
_D3DPRIMITIVETYPE GFXD3D8PrimType[GFXPT_COUNT];
_D3DTEXTURESTAGESTATETYPE GFXD3D8TextureStageState[GFXTSS_COUNT];
_D3DTEXTUREADDRESS GFXD3D8TextureAddress[GFXAddress_COUNT];
_D3DTEXTUREOP GFXD3D8TextureOp[GFXTOP_COUNT];

//------------------------------------------------------------------------------

#define INIT_LOOKUPTABLE( tablearray, enumprefix, type ) \
   for( int i = enumprefix##_FIRST; i < enumprefix##_COUNT; i++ ) \
      tablearray##[i] = (##type##)GFX_UNINIT_VAL;

#define VALIDATE_LOOKUPTABLE( tablearray, enumprefix ) \
   for( int i = enumprefix##_FIRST; i < enumprefix##_COUNT; i++ ) \
      if( (int)tablearray##[i] == GFX_UNINIT_VAL ) \
         Con::warnf( "GFXD3DEnumTranslate: Unassigned value in " #tablearray ": %i", i ); \
      else if( (int)tablearray##[i] == GFX_UNSUPPORTED_VAL ) \
         Con::warnf( "GFXD3DEnumTranslate: Unsupported value in " #tablearray ": %i", i );

//------------------------------------------------------------------------------

void GFXD3D8EnumTranslate::init()
{
   INIT_LOOKUPTABLE( GFXD3D8IndexFormat, GFXIndexFormat, _D3DFORMAT );
   GFXD3D8IndexFormat[GFXIndexFormat16] = D3DFMT_INDEX16;
   GFXD3D8IndexFormat[GFXIndexFormat32] = D3DFMT_INDEX32;
   VALIDATE_LOOKUPTABLE( GFXD3D8IndexFormat, GFXIndexFormat );
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   INIT_LOOKUPTABLE( GFXD3D8SamplerState, GFXSAMP, _D3DTEXTURESTAGESTATETYPE );
   GFXD3D8SamplerState[GFXSAMPAddressU] = D3DTSS_ADDRESSU;
   GFXD3D8SamplerState[GFXSAMPAddressV] = D3DTSS_ADDRESSV;
   GFXD3D8SamplerState[GFXSAMPAddressW] = D3DTSS_ADDRESSW;
   GFXD3D8SamplerState[GFXSAMPBorderColor] = D3DTSS_BORDERCOLOR;
   GFXD3D8SamplerState[GFXSAMPMagFilter] = D3DTSS_MAGFILTER;
   GFXD3D8SamplerState[GFXSAMPMinFilter] = D3DTSS_MINFILTER;
   GFXD3D8SamplerState[GFXSAMPMipFilter] = D3DTSS_MIPFILTER;
   GFXD3D8SamplerState[GFXSAMPMipMapLODBias] = D3DTSS_MIPMAPLODBIAS;
   GFXD3D8SamplerState[GFXSAMPMaxMipLevel] = D3DTSS_MAXMIPLEVEL;
   GFXD3D8SamplerState[GFXSAMPMaxAnisotropy] = D3DTSS_MAXANISOTROPY;

   GFXD3D8SamplerState[GFXSAMPSRGBTexture] = (_D3DTEXTURESTAGESTATETYPE)GFX_UNSUPPORTED_VAL;;
   GFXD3D8SamplerState[GFXSAMPElementIndex] = (_D3DTEXTURESTAGESTATETYPE)GFX_UNSUPPORTED_VAL;;
   GFXD3D8SamplerState[GFXSAMPDMapOffset] = (_D3DTEXTURESTAGESTATETYPE)GFX_UNSUPPORTED_VAL;;

   VALIDATE_LOOKUPTABLE( GFXD3D8SamplerState, GFXSAMP );
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   INIT_LOOKUPTABLE( GFXD3D8TextureFormat, GFXFormat, _D3DFORMAT );
   GFXD3D8TextureFormat[GFXFormatR8G8B8] = D3DFMT_R8G8B8;
   GFXD3D8TextureFormat[GFXFormatR8G8B8A8] = D3DFMT_A8R8G8B8;
   GFXD3D8TextureFormat[GFXFormatR8G8B8X8] = D3DFMT_X8R8G8B8;
   GFXD3D8TextureFormat[GFXFormatR8G8B8X8_LE] = D3DFMT_X8R8G8B8;
   GFXD3D8TextureFormat[GFXFormatR5G6B5] = D3DFMT_R5G6B5;
   GFXD3D8TextureFormat[GFXFormatR5G5B5A1] = D3DFMT_A1R5G5B5;
   GFXD3D8TextureFormat[GFXFormatR5G5B5X1] = D3DFMT_X1R5G5B5;
   GFXD3D8TextureFormat[GFXFormatA8] = D3DFMT_A8;
   GFXD3D8TextureFormat[GFXFormatP8] = D3DFMT_P8;
   GFXD3D8TextureFormat[GFXFormatL8] = D3DFMT_L8;
   GFXD3D8TextureFormat[GFXFormatDXT1] = D3DFMT_DXT1;
   GFXD3D8TextureFormat[GFXFormatDXT2] = D3DFMT_DXT2;
   GFXD3D8TextureFormat[GFXFormatDXT3] = D3DFMT_DXT3;
   GFXD3D8TextureFormat[GFXFormatDXT4] = D3DFMT_DXT4;
   GFXD3D8TextureFormat[GFXFormatDXT5] = D3DFMT_DXT5;
   GFXD3D8TextureFormat[GFXFormatD32] = D3DFMT_D32;
   GFXD3D8TextureFormat[GFXFormatD24X8] = D3DFMT_D24X8;
   GFXD3D8TextureFormat[GFXFormatD24S8] = D3DFMT_D24S8;
   GFXD3D8TextureFormat[GFXFormatD16] = D3DFMT_D16;

   GFXD3D8TextureFormat[GFXFormatR32G32B32A32F] = (_D3DFORMAT)GFX_UNSUPPORTED_VAL;
   GFXD3D8TextureFormat[GFXFormatR16G16B16A16F] = (_D3DFORMAT)GFX_UNSUPPORTED_VAL;
   GFXD3D8TextureFormat[GFXFormatL16] = (_D3DFORMAT)GFX_UNSUPPORTED_VAL;
   GFXD3D8TextureFormat[GFXFormatR16G16B16A16] = (_D3DFORMAT)GFX_UNSUPPORTED_VAL;
   GFXD3D8TextureFormat[GFXFormatR16G16] = (_D3DFORMAT)GFX_UNSUPPORTED_VAL;
   GFXD3D8TextureFormat[GFXFormatR16F] = (_D3DFORMAT)GFX_UNSUPPORTED_VAL;
   GFXD3D8TextureFormat[GFXFormatR16G16F] = (_D3DFORMAT)GFX_UNSUPPORTED_VAL;
   GFXD3D8TextureFormat[GFXFormatR10G10B10A2] = (_D3DFORMAT)GFX_UNSUPPORTED_VAL;
   VALIDATE_LOOKUPTABLE( GFXD3D8TextureFormat, GFXFormat);
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   INIT_LOOKUPTABLE( GFXD3D8RenderState, GFXRenderState, _D3DRENDERSTATETYPE );
   GFXD3D8RenderState[GFXRSZEnable] = D3DRS_ZENABLE;
   GFXD3D8RenderState[GFXRSFillMode] = D3DRS_FILLMODE;
   GFXD3D8RenderState[GFXRSZWriteEnable] = D3DRS_ZWRITEENABLE;
   GFXD3D8RenderState[GFXRSAlphaTestEnable] = D3DRS_ALPHATESTENABLE;
   GFXD3D8RenderState[GFXRSSrcBlend] = D3DRS_SRCBLEND;
   GFXD3D8RenderState[GFXRSDestBlend] = D3DRS_DESTBLEND;
   GFXD3D8RenderState[GFXRSCullMode] = D3DRS_CULLMODE;
   GFXD3D8RenderState[GFXRSZFunc] = D3DRS_ZFUNC;
   GFXD3D8RenderState[GFXRSAlphaRef] = D3DRS_ALPHAREF;
   GFXD3D8RenderState[GFXRSAlphaFunc] = D3DRS_ALPHAFUNC;
   GFXD3D8RenderState[GFXRSAlphaBlendEnable] = D3DRS_ALPHABLENDENABLE;
   GFXD3D8RenderState[GFXRSStencilEnable] = D3DRS_STENCILENABLE;
   GFXD3D8RenderState[GFXRSStencilFail] = D3DRS_STENCILFAIL;
   GFXD3D8RenderState[GFXRSStencilZFail] = D3DRS_STENCILZFAIL;
   GFXD3D8RenderState[GFXRSStencilPass] = D3DRS_STENCILPASS;
   GFXD3D8RenderState[GFXRSStencilFunc] = D3DRS_STENCILFUNC;
   GFXD3D8RenderState[GFXRSStencilRef] = D3DRS_STENCILREF;
   GFXD3D8RenderState[GFXRSStencilMask] = D3DRS_STENCILMASK;
   GFXD3D8RenderState[GFXRSStencilWriteMask] = D3DRS_STENCILWRITEMASK;
   GFXD3D8RenderState[GFXRSWrap0] = D3DRS_WRAP0;
   GFXD3D8RenderState[GFXRSWrap1] = D3DRS_WRAP1;
   GFXD3D8RenderState[GFXRSWrap2] = D3DRS_WRAP2;
   GFXD3D8RenderState[GFXRSWrap3] = D3DRS_WRAP3;
   GFXD3D8RenderState[GFXRSWrap4] = D3DRS_WRAP4;
   GFXD3D8RenderState[GFXRSWrap5] = D3DRS_WRAP5;
   GFXD3D8RenderState[GFXRSWrap6] = D3DRS_WRAP6;
   GFXD3D8RenderState[GFXRSWrap7] = D3DRS_WRAP7;
   GFXD3D8RenderState[GFXRSClipPlaneEnable] = D3DRS_CLIPPLANEENABLE;
   GFXD3D8RenderState[GFXRSPointSize] = D3DRS_POINTSIZE;
   GFXD3D8RenderState[GFXRSPointSizeMin] = D3DRS_POINTSIZE_MIN;
   GFXD3D8RenderState[GFXRSPointSize_Max] = D3DRS_POINTSIZE_MAX;
   GFXD3D8RenderState[GFXRSPointSpriteEnable] = D3DRS_POINTSPRITEENABLE;
   GFXD3D8RenderState[GFXRSMultiSampleantiAlias] = D3DRS_MULTISAMPLEANTIALIAS;
   GFXD3D8RenderState[GFXRSMultiSampleMask] = D3DRS_MULTISAMPLEMASK;
   GFXD3D8RenderState[GFXRSShadeMode] = D3DRS_SHADEMODE;
   GFXD3D8RenderState[GFXRSLastPixel] = D3DRS_LASTPIXEL;
   GFXD3D8RenderState[GFXRSClipping] = D3DRS_CLIPPING;
   GFXD3D8RenderState[GFXRSPointScaleEnable] = D3DRS_POINTSCALEENABLE;
   GFXD3D8RenderState[GFXRSPointScale_A] = D3DRS_POINTSCALE_A;
   GFXD3D8RenderState[GFXRSPointScale_B] = D3DRS_POINTSCALE_B;
   GFXD3D8RenderState[GFXRSPointScale_C] = D3DRS_POINTSCALE_C;
   GFXD3D8RenderState[GFXRSLighting] = D3DRS_LIGHTING;
   GFXD3D8RenderState[GFXRSAmbient] = D3DRS_AMBIENT;
   GFXD3D8RenderState[GFXRSFogVertexMode] = D3DRS_FOGVERTEXMODE;
   GFXD3D8RenderState[GFXRSColorVertex] = D3DRS_COLORVERTEX;
   GFXD3D8RenderState[GFXRSLocalViewer] = D3DRS_LOCALVIEWER;
   GFXD3D8RenderState[GFXRSNormalizeNormals] = D3DRS_NORMALIZENORMALS;
   GFXD3D8RenderState[GFXRSDiffuseMaterialSource] = D3DRS_DIFFUSEMATERIALSOURCE;
   GFXD3D8RenderState[GFXRSSpecularMaterialSource] = D3DRS_SPECULARMATERIALSOURCE;
   GFXD3D8RenderState[GFXRSAmbientMaterialSource] = D3DRS_AMBIENTMATERIALSOURCE;
   GFXD3D8RenderState[GFXRSEmissiveMaterialSource] = D3DRS_EMISSIVEMATERIALSOURCE;
   GFXD3D8RenderState[GFXRSVertexBlend] = D3DRS_VERTEXBLEND;
   GFXD3D8RenderState[GFXRSFogEnable] = D3DRS_FOGENABLE;
   GFXD3D8RenderState[GFXRSSpecularEnable] = D3DRS_SPECULARENABLE;
   GFXD3D8RenderState[GFXRSFogColor] = D3DRS_FOGCOLOR;
   GFXD3D8RenderState[GFXRSFogTableMode] = D3DRS_FOGTABLEMODE;
   GFXD3D8RenderState[GFXRSFogStart] = D3DRS_FOGSTART;
   GFXD3D8RenderState[GFXRSFogEnd] = D3DRS_FOGEND;
   GFXD3D8RenderState[GFXRSFogDensity] = D3DRS_FOGDENSITY;
   GFXD3D8RenderState[GFXRSRangeFogEnable] = D3DRS_RANGEFOGENABLE;
   GFXD3D8RenderState[GFXRSDebugMonitorToken] = D3DRS_DEBUGMONITORTOKEN;
   GFXD3D8RenderState[GFXRSIndexedVertexBlendEnable] = D3DRS_INDEXEDVERTEXBLENDENABLE;
   GFXD3D8RenderState[GFXRSTweenFactor] = D3DRS_TWEENFACTOR;
   GFXD3D8RenderState[GFXRSTextureFactor] = D3DRS_TEXTUREFACTOR;
   GFXD3D8RenderState[GFXRSPatchEdgeStyle] = D3DRS_PATCHEDGESTYLE;
   GFXD3D8RenderState[GFXRSDitherEnable] = D3DRS_DITHERENABLE;
   GFXD3D8RenderState[GFXRSColorWriteEnable] = D3DRS_COLORWRITEENABLE;
   GFXD3D8RenderState[GFXRSBlendOp] = D3DRS_BLENDOP;
   GFXD3D8RenderState[GFXRSDepthBias] = D3DRS_ZBIAS;
   

   GFXD3D8RenderState[GFXRSPositionDegree] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSNormalDegree] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSAntiAliasedLineEnable] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSAdaptiveTess_X] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSAdaptiveTess_Y] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSdaptiveTess_Z] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSAdaptiveTess_W] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSEnableAdaptiveTesselation] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSScissorTestEnable] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSSlopeScaleDepthBias] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSMinTessellationLevel] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSMaxTessellationLevel] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSTwoSidedStencilMode] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSCCWStencilFail] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSCCWStencilZFail] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSCCWStencilPass] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSCCWStencilFunc] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSColorWriteEnable1] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSColorWriteEnable2] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSolorWriteEnable3] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSBlendFactor] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSSRGBWriteEnable] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSWrap8] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSWrap9] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSWrap10] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSWrap11] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSWrap12] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSWrap13] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSWrap14] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSWrap15] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSSeparateAlphaBlendEnable] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSSrcBlendAlpha] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSDestBlendAlpha] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8RenderState[GFXRSBlendOpAlpha] = (_D3DRENDERSTATETYPE)GFX_UNSUPPORTED_VAL;

   VALIDATE_LOOKUPTABLE( GFXD3D8RenderState, GFXRenderState );
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   INIT_LOOKUPTABLE( GFXD3D8TextureFilter, GFXTextureFilter, _D3DTEXTUREFILTERTYPE );
   GFXD3D8TextureFilter[GFXTextureFilterNone] = D3DTEXF_NONE;
   GFXD3D8TextureFilter[GFXTextureFilterPoint] = D3DTEXF_POINT;
   GFXD3D8TextureFilter[GFXTextureFilterLinear] = D3DTEXF_LINEAR;
   GFXD3D8TextureFilter[GFXTextureFilterAnisotropic] = D3DTEXF_ANISOTROPIC;

   GFXD3D8TextureFilter[GFXTextureFilterPyramidalQuad] = (_D3DTEXTUREFILTERTYPE)GFX_UNSUPPORTED_VAL;
   GFXD3D8TextureFilter[GFXTextureFilterGaussianQuad] = (_D3DTEXTUREFILTERTYPE)GFX_UNSUPPORTED_VAL;

   VALIDATE_LOOKUPTABLE( GFXD3D8TextureFilter, GFXTextureFilter );
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   INIT_LOOKUPTABLE( GFXD3D8Blend, GFXBlend, _D3DBLEND );
   GFXD3D8Blend[GFXBlendZero] = D3DBLEND_ZERO;
   GFXD3D8Blend[GFXBlendOne] = D3DBLEND_ONE;
   GFXD3D8Blend[GFXBlendSrcColor] = D3DBLEND_SRCCOLOR;
   GFXD3D8Blend[GFXBlendInvSrcColor] = D3DBLEND_INVSRCCOLOR;
   GFXD3D8Blend[GFXBlendSrcAlpha] = D3DBLEND_SRCALPHA;
   GFXD3D8Blend[GFXBlendInvSrcAlpha] = D3DBLEND_INVSRCALPHA;
   GFXD3D8Blend[GFXBlendDestAlpha] = D3DBLEND_DESTALPHA;
   GFXD3D8Blend[GFXBlendInvDestAlpha] = D3DBLEND_INVDESTALPHA;
   GFXD3D8Blend[GFXBlendDestColor] = D3DBLEND_DESTCOLOR;
   GFXD3D8Blend[GFXBlendInvDestColor] = D3DBLEND_INVDESTCOLOR;
   GFXD3D8Blend[GFXBlendSrcAlphaSat] = D3DBLEND_SRCALPHASAT;
   VALIDATE_LOOKUPTABLE( GFXD3D8Blend, GFXBlend );
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   INIT_LOOKUPTABLE( GFXD3D8BlendOp, GFXBlendOp, _D3DBLENDOP );
   GFXD3D8BlendOp[GFXBlendOpAdd] = D3DBLENDOP_ADD;
   GFXD3D8BlendOp[GFXBlendOpSubtract] = D3DBLENDOP_SUBTRACT;
   GFXD3D8BlendOp[GFXBlendOpRevSubtract] = D3DBLENDOP_REVSUBTRACT;
   GFXD3D8BlendOp[GFXBlendOpMin] = D3DBLENDOP_MIN;
   GFXD3D8BlendOp[GFXBlendOpMax] = D3DBLENDOP_MAX;
   VALIDATE_LOOKUPTABLE( GFXD3D8BlendOp, GFXBlendOp );
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   INIT_LOOKUPTABLE( GFXD3D8StencilOp, GFXStencilOp, _D3DSTENCILOP );
   GFXD3D8StencilOp[GFXStencilOpKeep] = D3DSTENCILOP_KEEP;
   GFXD3D8StencilOp[GFXStencilOpZero] = D3DSTENCILOP_ZERO;
   GFXD3D8StencilOp[GFXStencilOpReplace] = D3DSTENCILOP_REPLACE;
   GFXD3D8StencilOp[GFXStencilOpIncrSat] = D3DSTENCILOP_INCRSAT;
   GFXD3D8StencilOp[GFXStencilOpDecrSat] = D3DSTENCILOP_DECRSAT;
   GFXD3D8StencilOp[GFXStencilOpInvert] = D3DSTENCILOP_INVERT;
   GFXD3D8StencilOp[GFXStencilOpIncr] = D3DSTENCILOP_INCR;
   GFXD3D8StencilOp[GFXStencilOpDecr] = D3DSTENCILOP_DECR;
   VALIDATE_LOOKUPTABLE( GFXD3D8StencilOp, GFXStencilOp );
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   INIT_LOOKUPTABLE( GFXD3D8CmpFunc, GFXCmp, _D3DCMPFUNC );
   GFXD3D8CmpFunc[GFXCmpNever] = D3DCMP_NEVER;
   GFXD3D8CmpFunc[GFXCmpLess] = D3DCMP_LESS;
   GFXD3D8CmpFunc[GFXCmpEqual] = D3DCMP_EQUAL;
   GFXD3D8CmpFunc[GFXCmpLessEqual] = D3DCMP_LESSEQUAL;
   GFXD3D8CmpFunc[GFXCmpGreater] = D3DCMP_GREATER;
   GFXD3D8CmpFunc[GFXCmpNotEqual] = D3DCMP_NOTEQUAL;
   GFXD3D8CmpFunc[GFXCmpGreaterEqual] = D3DCMP_GREATEREQUAL;
   GFXD3D8CmpFunc[GFXCmpAlways] = D3DCMP_ALWAYS;
   VALIDATE_LOOKUPTABLE( GFXD3D8CmpFunc, GFXCmp );
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   INIT_LOOKUPTABLE( GFXD3D8CullMode, GFXCull, _D3DCULL );
   GFXD3D8CullMode[GFXCullNone] = D3DCULL_NONE;
   GFXD3D8CullMode[GFXCullCW] = D3DCULL_CW;
   GFXD3D8CullMode[GFXCullCCW] = D3DCULL_CCW;
   VALIDATE_LOOKUPTABLE( GFXD3D8CullMode, GFXCull );
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   INIT_LOOKUPTABLE( GFXD3D8PrimType, GFXPT, _D3DPRIMITIVETYPE );
   GFXD3D8PrimType[GFXPointList] = D3DPT_POINTLIST;
   GFXD3D8PrimType[GFXLineList] = D3DPT_LINELIST;
   GFXD3D8PrimType[GFXLineStrip] = D3DPT_LINESTRIP;
   GFXD3D8PrimType[GFXTriangleList] = D3DPT_TRIANGLELIST;
   GFXD3D8PrimType[GFXTriangleStrip] = D3DPT_TRIANGLESTRIP;
   GFXD3D8PrimType[GFXTriangleFan] = D3DPT_TRIANGLEFAN;
   VALIDATE_LOOKUPTABLE( GFXD3D8PrimType, GFXPT );
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   INIT_LOOKUPTABLE( GFXD3D8TextureStageState, GFXTSS, _D3DTEXTURESTAGESTATETYPE );
   GFXD3D8TextureStageState[GFXTSSColorOp] = D3DTSS_COLOROP;
   GFXD3D8TextureStageState[GFXTSSColorArg1] = D3DTSS_COLORARG1;
   GFXD3D8TextureStageState[GFXTSSColorArg2] = D3DTSS_COLORARG2;
   GFXD3D8TextureStageState[GFXTSSAlphaOp] = D3DTSS_ALPHAOP;
   GFXD3D8TextureStageState[GFXTSSAlphaArg1] = D3DTSS_ALPHAARG1;
   GFXD3D8TextureStageState[GFXTSSAlphaArg2] = D3DTSS_ALPHAARG2;
   GFXD3D8TextureStageState[GFXTSSBumpEnvMat00] = D3DTSS_BUMPENVMAT00;
   GFXD3D8TextureStageState[GFXTSSBumpEnvMat01] = D3DTSS_BUMPENVMAT01;
   GFXD3D8TextureStageState[GFXTSSBumpEnvMat10] = D3DTSS_BUMPENVMAT10;
   GFXD3D8TextureStageState[GFXTSSBumpEnvMat11] = D3DTSS_BUMPENVMAT11;
   GFXD3D8TextureStageState[GFXTSSTexCoordIndex] = D3DTSS_TEXCOORDINDEX;
   GFXD3D8TextureStageState[GFXTSSBumpEnvlScale] = D3DTSS_BUMPENVLSCALE;
   GFXD3D8TextureStageState[GFXTSSBumpEnvlOffset] = D3DTSS_BUMPENVLOFFSET;
   GFXD3D8TextureStageState[GFXTSSTextureTransformFlags] = D3DTSS_TEXTURETRANSFORMFLAGS;
   GFXD3D8TextureStageState[GFXTSSColorArg0] = D3DTSS_COLORARG0;
   GFXD3D8TextureStageState[GFXTSSAlphaArg0] = D3DTSS_ALPHAARG0;
   GFXD3D8TextureStageState[GFXTSSResultArg] = D3DTSS_RESULTARG;

   GFXD3D8TextureStageState[GFXTSSConstant] = (_D3DTEXTURESTAGESTATETYPE)GFX_UNSUPPORTED_VAL;;
   VALIDATE_LOOKUPTABLE( GFXD3D8TextureStageState, GFXTSS );
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   INIT_LOOKUPTABLE( GFXD3D8TextureAddress, GFXAddress, _D3DTEXTUREADDRESS );
   GFXD3D8TextureAddress[GFXAddressWrap] = D3DTADDRESS_WRAP ;
   GFXD3D8TextureAddress[GFXAddressMirror] = D3DTADDRESS_MIRROR;
   GFXD3D8TextureAddress[GFXAddressClamp] = D3DTADDRESS_CLAMP;
   GFXD3D8TextureAddress[GFXAddressBorder] = D3DTADDRESS_BORDER;
   GFXD3D8TextureAddress[GFXAddressMirrorOnce] = D3DTADDRESS_MIRRORONCE;
   VALIDATE_LOOKUPTABLE(GFXD3D8TextureAddress, GFXAddress );
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   INIT_LOOKUPTABLE( GFXD3D8TextureOp, GFXTOP, _D3DTEXTUREOP );
   GFXD3D8TextureOp[GFXTOPDisable] = D3DTOP_DISABLE;
   GFXD3D8TextureOp[GFXTOPSelectARG1] = D3DTOP_SELECTARG1;
   GFXD3D8TextureOp[GFXTOPSelectARG2] = D3DTOP_SELECTARG2;
   GFXD3D8TextureOp[GFXTOPModulate] = D3DTOP_MODULATE;
   GFXD3D8TextureOp[GFXTOPModulate2X] = D3DTOP_MODULATE2X;
   GFXD3D8TextureOp[GFXTOPModulate4X] = D3DTOP_MODULATE4X;
   GFXD3D8TextureOp[GFXTOPAdd] = D3DTOP_ADD;
   GFXD3D8TextureOp[GFXTOPAddSigned] = D3DTOP_ADDSIGNED;
   GFXD3D8TextureOp[GFXTOPAddSigned2X] = D3DTOP_ADDSIGNED2X;
   GFXD3D8TextureOp[GFXTOPSubtract] = D3DTOP_SUBTRACT;
   GFXD3D8TextureOp[GFXTOPAddSmooth] = D3DTOP_ADDSMOOTH;
   GFXD3D8TextureOp[GFXTOPBlendDiffuseAlpha] = D3DTOP_BLENDDIFFUSEALPHA;
   GFXD3D8TextureOp[GFXTOPBlendTextureAlpha] = D3DTOP_BLENDTEXTUREALPHA;
   GFXD3D8TextureOp[GFXTOPBlendFactorAlpha] = D3DTOP_BLENDFACTORALPHA;
   GFXD3D8TextureOp[GFXTOPBlendTextureAlphaPM] = D3DTOP_BLENDTEXTUREALPHAPM;
   GFXD3D8TextureOp[GFXTOPBlendCURRENTALPHA] = D3DTOP_BLENDCURRENTALPHA;
   GFXD3D8TextureOp[GFXTOPPreModulate] = D3DTOP_PREMODULATE;
   GFXD3D8TextureOp[GFXTOPModulateAlphaAddColor] = D3DTOP_MODULATEALPHA_ADDCOLOR;
   GFXD3D8TextureOp[GFXTOPModulateColorAddAlpha] = D3DTOP_MODULATECOLOR_ADDALPHA;
   GFXD3D8TextureOp[GFXTOPModulateInvAlphaAddColor] = D3DTOP_MODULATEINVALPHA_ADDCOLOR;
   GFXD3D8TextureOp[GFXTOPModulateInvColorAddAlpha] = D3DTOP_MODULATEINVCOLOR_ADDALPHA;
   GFXD3D8TextureOp[GFXTOPBumpEnvMap] = D3DTOP_BUMPENVMAP;
   GFXD3D8TextureOp[GFXTOPBumpEnvMapLuminance] = D3DTOP_BUMPENVMAPLUMINANCE;
   GFXD3D8TextureOp[GFXTOPDotProduct3] = D3DTOP_DOTPRODUCT3;
   GFXD3D8TextureOp[GFXTOPLERP] = D3DTOP_LERP;
   VALIDATE_LOOKUPTABLE( GFXD3D8TextureOp, GFXTOP );
}

