//-----------------------------------------------------------------------------
// PowerEngine
// 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GFXENUMS_H_
#define _GFXENUMS_H_

#include "util/fourcc.h"

// These are for the enum translation. It will help with porting to other platforms
// and API's.
#define GFX_UNSUPPORTED_VAL 0xDEADBEEF
#define GFX_UNINIT_VAL 0xDECAFBAD

// Adjust these pools to your app's needs.  Be aware dynamic vertices are much more
// expensive than static vertices. These are in gfxEnums because they should be
// consistant across all APIs/platforms so that the dynamic buffer performance
// and behavior is also consistant. -patw
#define MAX_DYNAMIC_VERTS   (8192*2)
#define MAX_DYNAMIC_INDICES (8192*4)

enum GFXBufferType
{
      GFXBufferTypeStatic,   ///< Static vertex buffers are created and filled one time.
                   ///< incur a performance penalty.  Resizing a static vertex buffer is not
                   ///< allowed.
      GFXBufferTypeDynamic,  ///< Dynamic vertex buffers are meant for vertices that can be changed
                   ///< often.  Vertices written into dynamic vertex buffers will remain valid
                   ///< until the dynamic vertex buffer is released.  Resizing a dynamic vertex buffer is not
                   ///< allowed.
      GFXBufferTypeVolatile, ///< Volatile vertex or index buffers are meant for vertices or indices that are essentially
                   ///< only used once.  They can be resized without any performance penalty.
      
	  GFXBufferTypeNull,   //null buff for clear 

      GFXBufferType_COUNT ///< Number of buffer types.
};

enum GFXTexCallbackCode
{
   GFXZombify,
   GFXResurrect,
};


enum GFXPrimitiveType 
{
   GFXPT_FIRST = 0,
   GFXPointList = 0,
   GFXLineList,
   GFXLineStrip,
   GFXTriangleList,
   GFXTriangleStrip,
   GFXTriangleFan,
   GFXPT_COUNT
};

enum GFXTextureType 
{
   GFXTextureType_Normal,
   GFXTextureType_KeepBitmap,
   GFXTextureType_Dynamic,
   GFXTextureType_RenderTarget,
   GFXTextureType_Count
};

enum GFXBitmapFlip 
{
   GFXBitmapFlip_None = 0,
   GFXBitmapFlip_X    = 1 << 0,
   GFXBitmapFlip_Y    = 1 << 1,
   GFXBitmapFlip_XY   = GFXBitmapFlip_X | GFXBitmapFlip_Y
};

enum GFXTextureOp 
{
   GFXTOP_FIRST = 0,
   GFXTOPDisable = 0,
   GFXTOPSelectARG1,
   GFXTOPSelectARG2,
   GFXTOPModulate,
   GFXTOPModulate2X,
   GFXTOPModulate4X,
   GFXTOPAdd,
   GFXTOPAddSigned,
   GFXTOPAddSigned2X,
   GFXTOPSubtract,
   GFXTOPAddSmooth, 
   GFXTOPBlendDiffuseAlpha,
   GFXTOPBlendTextureAlpha,
   GFXTOPBlendFactorAlpha,
   GFXTOPBlendTextureAlphaPM,
   GFXTOPBlendCURRENTALPHA,
   GFXTOPPreModulate,
   GFXTOPModulateAlphaAddColor,
   GFXTOPModulateColorAddAlpha,
   GFXTOPModulateInvAlphaAddColor,
   GFXTOPModulateInvColorAddAlpha,
   GFXTOPBumpEnvMap,
   GFXTOPBumpEnvMapLuminance,
   GFXTOPDotProduct3,
   GFXTOPLERP,
   GFXTOP_COUNT
};

enum GFXTextureAddressMode 
{
   GFXAddress_FIRST = 0,
   GFXAddressWrap = 0,
   GFXAddressMirror,
   GFXAddressClamp,
   GFXAddressBorder,
   GFXAddressMirrorOnce,
   GFXAddress_COUNT
};

enum GFXTextureFilterType 
{
   GFXTextureFilter_FIRST = 0,
   GFXTextureFilterNone = 0,
   GFXTextureFilterPoint,
   GFXTextureFilterLinear,
   GFXTextureFilterAnisotropic,
   GFXTextureFilterPyramidalQuad,
   GFXTextureFilterGaussianQuad,
   GFXTextureFilter_COUNT
};

enum GFXFillMode 
{
   GFXFillPoint = 1,
   GFXFillWireframe,
   GFXFillSolid,
};

enum GFXFormat 
{
   // when adding formats make sure to place
   // them in the correct group!
   //
   // if displacing the first entry in the group
   // make sure to update the GFXFormat_xBIT entries!
   //
   GFXFormat_FIRST = 0,

   // 8 bit texture formats...
   GFXFormatA8 = 0,// first in group...
   GFXFormatP8,
   GFXFormatL8,

   // 16 bit texture formats...
   GFXFormatR5G6B5,// first in group...
   GFXFormatR5G5B5A1,
   GFXFormatR5G5B5X1,
   GFXFormatL16,
   GFXFormatR16F,
   GFXFormatD16,

   // 24 bit texture formats...
   GFXFormatR8G8B8,// first in group...

   // 32 bit texture formats...
   GFXFormatR8G8B8A8,// first in group...
   GFXFormatR8G8B8X8,
   GFXFormatR8G8B8X8_LE,
   GFXFormatR16G16,
   GFXFormatR16G16F,
   GFXFormatR10G10B10A2,
   GFXFormatD32,
   GFXFormatD24X8,
   GFXFormatD24S8,
   GFXFormatR32F,

   // 64 bit texture formats...
   GFXFormatR16G16B16A16,// first in group...
   GFXFormatR16G16B16A16F,

   // 128 bit texture formats...
   GFXFormatR32G32B32A32F,// first in group...

   // unknown size...
   GFXFormatDXT1,// first in group...
   GFXFormatDXT2,
   GFXFormatDXT3,
   GFXFormatDXT4,
   GFXFormatDXT5,

   GFXFormat_COUNT,

   GFXFormat_8BIT = GFXFormatA8,
   GFXFormat_16BIT = GFXFormatR5G6B5,
   GFXFormat_24BIT = GFXFormatR8G8B8,
   GFXFormat_32BIT = GFXFormatR8G8B8A8,
   GFXFormat_64BIT = GFXFormatR16G16B16A16,
   GFXFormat_128BIT = GFXFormatR32G32B32A32F,
   GFXFormat_UNKNOWNSIZE = GFXFormatDXT1,
};

//in byte
static unsigned int GFXFormatToSize(int fmt)
{
    switch(fmt)
    {
    case GFXFormatA8:
    case GFXFormatP8:
    case GFXFormatL8:
        return 1;
    case GFXFormatR5G6B5:
    case GFXFormatR5G5B5A1:
    case GFXFormatR5G5B5X1:
    case GFXFormatL16:
    case GFXFormatR16F:
    case GFXFormatD16:
        return 2;
    case GFXFormatR8G8B8:
        return 3;
    case GFXFormatR8G8B8A8:
    case GFXFormatR8G8B8X8:
    case GFXFormatR8G8B8X8_LE:
    case GFXFormatR16G16:
    case GFXFormatR16G16F:
    case GFXFormatR10G10B10A2:
    case GFXFormatD32:
    case GFXFormatD24X8:
    case GFXFormatD24S8:
    case GFXFormatR32F:
        return 4;
    case GFXFormatR16G16B16A16:
    case GFXFormatR16G16B16A16F:
        return 8;
    case GFXFormatR32G32B32A32F:
        return 16;
    }

    return 4;
}

enum GFXShadeMode 
{
   GFXShadeFlat = 1,
   GFXShadeGouraud,
   GFXShadePhong,
};

enum GFXClearFlags 
{
   GFXClearTarget = 1 << 0,
   GFXClearZBuffer = 1 << 1,
   GFXClearStencil = 1 << 2,
};

enum GFXBlend 
{
   GFXBlend_FIRST = 0,
   GFXBlendZero = 0,
   GFXBlendOne,
   GFXBlendSrcColor,
   GFXBlendInvSrcColor,
   GFXBlendSrcAlpha,
   GFXBlendInvSrcAlpha,
   GFXBlendDestAlpha,
   GFXBlendInvDestAlpha,
   GFXBlendDestColor,
   GFXBlendInvDestColor,
   GFXBlendSrcAlphaSat,
   GFXBlendBlendFactor,
   GFXBlend_COUNT
};

/// Constants that name each GFXDevice type. Any new GFXDevice subclass must be
/// added to this enum. A string representing its name must also be added to
/// GFXInit::getAdapterNameFromType().
enum GFXAdapterType 
{
   OpenGL = 0,
   Direct3D9,
   Direct3D8,
   NullDevice,
   Direct3D9_360,
   GFXAdapterType_Count
};

enum GFXCullMode
{
   GFXCull_FIRST = 0,
   GFXCullNone = 0,
   GFXCullCW,
   GFXCullCCW,
   GFXCull_COUNT
};

enum GFXCmpFunc 
{
   GFXCmp_FIRST = 0,
   GFXCmpNever = 0,
   GFXCmpLess,
   GFXCmpEqual,
   GFXCmpLessEqual,
   GFXCmpGreater,
   GFXCmpNotEqual,
   GFXCmpGreaterEqual,
   GFXCmpAlways,
   GFXCmp_COUNT
};

enum GFXStencilOp 
{
   GFXStencilOp_FIRST = 0,
   GFXStencilOpKeep = 0,
   GFXStencilOpZero,
   GFXStencilOpReplace,
   GFXStencilOpIncrSat,
   GFXStencilOpDecrSat,
   GFXStencilOpInvert,
   GFXStencilOpIncr,
   GFXStencilOpDecr,
   GFXStencilOp_COUNT
};

enum GFXMaterialColorSource 
{
   GFXMCSMaterial = 0,
   GFXMCSColor1,
   GFXMCSColor2,
};

enum GFXBlendOp 
{ 
   GFXBlendOp_FIRST = 0,
   GFXBlendOpAdd = 0,
   GFXBlendOpSubtract,
   GFXBlendOpRevSubtract,
   GFXBlendOpMin,
   GFXBlendOpMax,
   GFXBlendOp_COUNT
};

enum GFXRenderState 
{
   GFXRenderState_FIRST = 0,
   GFXRSZEnable = 0,
   GFXRSFillMode,
   GFXRSShadeMode,
   GFXRSZWriteEnable,
   GFXRSAlphaTestEnable,
   GFXRSLastPixel,
   GFXRSSrcBlend,
   GFXRSDestBlend,
   GFXRSCullMode,
   GFXRSZFunc,
   GFXRSAlphaRef,
   GFXRSAlphaFunc,
   GFXRSDitherEnable,
   GFXRSAlphaBlendEnable,
   GFXRSFogEnable,
   GFXRSSpecularEnable,
   GFXRSFogColor,
   GFXRSFogTableMode,
   GFXRSFogStart,
   GFXRSFogEnd,
   GFXRSFogDensity,
   GFXRSRangeFogEnable,
   GFXRSStencilEnable,
   GFXRSStencilFail,
   GFXRSStencilZFail,
   GFXRSStencilPass,
   GFXRSStencilFunc,
   GFXRSStencilRef,
   GFXRSStencilMask,
   GFXRSStencilWriteMask,
   GFXRSTextureFactor,
   GFXRSWrap0,
   GFXRSWrap1,
   GFXRSWrap2,
   GFXRSWrap3,
   GFXRSWrap4,
   GFXRSWrap5,
   GFXRSWrap6,
   GFXRSWrap7,
   GFXRSClipping,
   GFXRSLighting,
   GFXRSAmbient,
   GFXRSFogVertexMode,
   GFXRSColorVertex,
   GFXRSLocalViewer,
   GFXRSNormalizeNormals,
   GFXRSDiffuseMaterialSource,
   GFXRSSpecularMaterialSource,
   GFXRSAmbientMaterialSource,
   GFXRSEmissiveMaterialSource,
   GFXRSVertexBlend,
   GFXRSClipPlaneEnable,
   GFXRSPointSize,
   GFXRSPointSizeMin,
   GFXRSPointSpriteEnable,
   GFXRSPointScaleEnable,
   GFXRSPointScale_A,
   GFXRSPointScale_B,
   GFXRSPointScale_C,
   GFXRSMultiSampleantiAlias,
   GFXRSMultiSampleMask,
   GFXRSPatchEdgeStyle,
   GFXRSDebugMonitorToken,
   GFXRSPointSize_Max,
   GFXRSIndexedVertexBlendEnable,
   GFXRSColorWriteEnable,
   GFXRSTweenFactor,
   GFXRSBlendOp,
   GFXRSPositionDegree,
   GFXRSNormalDegree,
   GFXRSScissorTestEnable,
   GFXRSSlopeScaleDepthBias,
   GFXRSAntiAliasedLineEnable,
   GFXRSMinTessellationLevel,
   GFXRSMaxTessellationLevel,
   GFXRSAdaptiveTess_X,
   GFXRSAdaptiveTess_Y,
   GFXRSdaptiveTess_Z,
   GFXRSAdaptiveTess_W,
   GFXRSEnableAdaptiveTesselation,
   GFXRSTwoSidedStencilMode,
   GFXRSCCWStencilFail,
   GFXRSCCWStencilZFail,
   GFXRSCCWStencilPass,
   GFXRSCCWStencilFunc,
   GFXRSColorWriteEnable1,
   GFXRSColorWriteEnable2,
   GFXRSolorWriteEnable3,
   GFXRSBlendFactor,
   GFXRSSRGBWriteEnable,
   GFXRSDepthBias,
   GFXRSWrap8,
   GFXRSWrap9,
   GFXRSWrap10,
   GFXRSWrap11,
   GFXRSWrap12,
   GFXRSWrap13,
   GFXRSWrap14,
   GFXRSWrap15,
   GFXRSSeparateAlphaBlendEnable,
   GFXRSSrcBlendAlpha,
   GFXRSDestBlendAlpha,
   GFXRSBlendOpAlpha,
   GFXRenderState_COUNT          ///< Don't use this one, this is a counter
};

#define GFXCOLORWRITEENABLE_RED     1
#define GFXCOLORWRITEENABLE_GREEN   2
#define GFXCOLORWRITEENABLE_BLUE    4
#define GFXCOLORWRITEENABLE_ALPHA   8

enum GFXTextureStageState 
{
   GFXTSS_FIRST = 0,
   GFXTSSColorOp = 0,
   GFXTSSColorArg1,
   GFXTSSColorArg2,
   GFXTSSAlphaOp,
   GFXTSSAlphaArg1,
   GFXTSSAlphaArg2,
   GFXTSSBumpEnvMat00,
   GFXTSSBumpEnvMat01,
   GFXTSSBumpEnvMat10,
   GFXTSSBumpEnvMat11,
   GFXTSSTexCoordIndex,
   GFXTSSBumpEnvlScale,
   GFXTSSBumpEnvlOffset,
   GFXTSSTextureTransformFlags,
   GFXTSSColorArg0,
   GFXTSSAlphaArg0,
   GFXTSSResultArg,
   GFXTSSConstant,
   GFXTSS_COUNT            ///< Don't use this one, this is a counter
};

enum GFXTextureTransformFlags
{
   GFXTTFFDisable = 0,
   GFXTTFFCoord1D = 1,
   GFXTTFFCoord2D = 2,
   GFXTTFFCoord3D = 3,
   GFXTTFFCoord4D = 4,
   GFXTTFFProjected = 256,
};

// CodeReview: This number is used for the declaration of variables, but it
// should *not* be used for any run-time purposes [7/2/2007 Pat]
// Ray: 增加一张VertexTex
#define VERTEX_TEXTURE_NUM 5
#define TEXTURE_STAGE_COUNT (VERTEX_TEXTURE_NUM + 16)
#define VERTEX_TEXTURE_STAGE3 (TEXTURE_STAGE_COUNT-1)
#define VERTEX_TEXTURE_STAGE2 (TEXTURE_STAGE_COUNT-2)
#define VERTEX_TEXTURE_STAGE1 (TEXTURE_STAGE_COUNT-3)
#define VERTEX_TEXTURE_STAGE0 (TEXTURE_STAGE_COUNT-4)
#define VERTEX_TEXTURE_BUFF	  (TEXTURE_STAGE_COUNT-5)

enum GFXSamplerState 
{
   GFXSAMP_FIRST = 0,
   GFXSAMPAddressU = 0,
   GFXSAMPAddressV,
   GFXSAMPAddressW,
   GFXSAMPBorderColor,
   GFXSAMPMagFilter,
   GFXSAMPMinFilter,
   GFXSAMPMipFilter,
   GFXSAMPMipMapLODBias,
   GFXSAMPMaxMipLevel,
   GFXSAMPMaxAnisotropy,
   GFXSAMPSRGBTexture,
   GFXSAMPElementIndex,
   GFXSAMPDMapOffset,
   GFXSAMP_COUNT          ///< Don't use this one, this is a counter
};

enum GFXTextureArgument 
{
   GFXTA_FIRST = 0,
   GFXTADiffuse = 0,
   GFXTACurrent,
   GFXTATexture,
   GFXTATFactor,
   GFXTASpecular,
   GFXTATemp,
   GFXTAConstant,
   GFXTAComplement = 0x00000010,       // take 1.0 - x (read modifier)
   GFXTAAlphaReplicate = 0x00000020,   // replicate alpha to color components (read modifier)
   GFXTA_COUNT
};

enum GFXTexTransformFlags
{
   GFXTTFDisable = 0,
   GFXTTFCount1 = 1,
   GFXTTFCount2 = 2,
   GFXTTFCount3 = 3,
   GFXTTFCount4 = 4,
   GFXTTFProjected = 256,
};

// Matrix stuff
#define WORLD_STACK_MAX 24

enum GFXMatrixType 
{
   GFXMatrixWorld = 256,
   GFXMatrixView = 2,
   GFXMatrixProjection = 3,
   GFXMatrixTexture = 16,     // This value is texture matrix for sampler 0, can use this for offset
   GFXMatrixTexture0 = 16,
   GFXMatrixTexture1 = 17,
   GFXMatrixTexture2 = 18,
   GFXMatrixTexture3 = 19,
   GFXMatrixTexture4 = 20,
   GFXMatrixTexture5 = 21,
   GFXMatrixTexture6 = 22,
   GFXMatrixTexture7 = 23,
};

// Light define
#define LIGHT_STAGE_COUNT 8

#define GFXVERTEXFLAG_F32     3
#define GFXVERTEXFLAG_POINT2F 0
#define GFXVERTEXFLAG_POINT3F 1 
#define GFXVERTEXFLAG_POINT4F 2

#define GFXVERTEXFLAG_TEXCOORD_F32(CoordIndex)     ( GFXVERTEXFLAG_F32     << ( CoordIndex * 2 + 16 ) ) 
#define GFXVERTEXFLAG_TEXCOORD_POINT2F(CoordIndex) ( GFXVERTEXFLAG_POINT2F ) 
#define GFXVERTEXFLAG_TEXCOORD_POINT3F(CoordIndex) ( GFXVERTEXFLAG_POINT3F << ( CoordIndex * 2 + 16 ) ) 
#define GFXVERTEXFLAG_TEXCOORD_POINT4F(CoordIndex) ( GFXVERTEXFLAG_POINT4F << ( CoordIndex * 2 + 16 ) )

#define STATE_STACK_SIZE 32

/// Vertex flags
/// @note If you add to these flags make sure you know what you are doing and 
///       go and change the prepare() method for D3D vertex buffers for the
///       color flipping or it will hose you big time.
enum GFXVertexFlags 
{
   GFXVertexFlagXYZ = 0x002, 
   GFXVertexFlagXYZW = 0x4002,
   GFXVertexFlagXYZRHW = 0x004,
   GFXVertexFlagNormal = 0x010,
   GFXVertexFlagPointSize = 0x020,
   GFXVertexFlagDiffuse = 0x040,
   GFXVertexFlagSpecular = 0x080,
   // Number of textures
   GFXVertexFlagTextureCount0 = 0x000,
   GFXVertexFlagTextureCount1 = 0x100,
   GFXVertexFlagTextureCount2 = 0x200,
   GFXVertexFlagTextureCount3 = 0x300,
   GFXVertexFlagTextureCount4 = 0x400,
   GFXVertexFlagTextureCount5 = 0x500,
   GFXVertexFlagTextureCount6 = 0x600,
   GFXVertexFlagTextureCount7 = 0x700,
   GFXVertexFlagTextureCount8 = 0x800,
   GFXVertexFlagTexureCountMask = 0xF00,

   // 1D textures
   GFXVertexFlagU0 = GFXVERTEXFLAG_TEXCOORD_F32(0),
   GFXVertexFlagU1 = GFXVERTEXFLAG_TEXCOORD_F32(1),
   GFXVertexFlagU2 = GFXVERTEXFLAG_TEXCOORD_F32(2),
   GFXVertexFlagU3 = GFXVERTEXFLAG_TEXCOORD_F32(3),
   GFXVertexFlagU4 = GFXVERTEXFLAG_TEXCOORD_F32(4),
   GFXVertexFlagU5 = GFXVERTEXFLAG_TEXCOORD_F32(5),
   GFXVertexFlagU6 = GFXVERTEXFLAG_TEXCOORD_F32(6),
   GFXVertexFlagU7 = GFXVERTEXFLAG_TEXCOORD_F32(7),
   // 2D textures
   GFXVertexFlagUV0 = GFXVERTEXFLAG_TEXCOORD_POINT2F(0),
   GFXVertexFlagUV1 = GFXVERTEXFLAG_TEXCOORD_POINT2F(1),
   GFXVertexFlagUV2 = GFXVERTEXFLAG_TEXCOORD_POINT2F(2),
   GFXVertexFlagUV3 = GFXVERTEXFLAG_TEXCOORD_POINT2F(3),
   GFXVertexFlagUV4 = GFXVERTEXFLAG_TEXCOORD_POINT2F(4),
   GFXVertexFlagUV5 = GFXVERTEXFLAG_TEXCOORD_POINT2F(5),
   GFXVertexFlagUV6 = GFXVERTEXFLAG_TEXCOORD_POINT2F(6),
   GFXVertexFlagUV7 = GFXVERTEXFLAG_TEXCOORD_POINT2F(7),
   // 3D textures
   GFXVertexFlagUVW0 = GFXVERTEXFLAG_TEXCOORD_POINT3F(0),
   GFXVertexFlagUVW1 = GFXVERTEXFLAG_TEXCOORD_POINT3F(1),
   GFXVertexFlagUVW2 = GFXVERTEXFLAG_TEXCOORD_POINT3F(2),
   GFXVertexFlagUVW3 = GFXVERTEXFLAG_TEXCOORD_POINT3F(3),
   GFXVertexFlagUVW4 = GFXVERTEXFLAG_TEXCOORD_POINT3F(4),
   GFXVertexFlagUVW5 = GFXVERTEXFLAG_TEXCOORD_POINT3F(5),
   GFXVertexFlagUVW6 = GFXVERTEXFLAG_TEXCOORD_POINT3F(6),
   GFXVertexFlagUVW7 = GFXVERTEXFLAG_TEXCOORD_POINT3F(7),
   // 4D textures
   GFXVertexFlagUVWQ0 = GFXVERTEXFLAG_TEXCOORD_POINT4F(0),
   GFXVertexFlagUVWQ1 = GFXVERTEXFLAG_TEXCOORD_POINT4F(1),
   GFXVertexFlagUVWQ2 = GFXVERTEXFLAG_TEXCOORD_POINT4F(2),
   GFXVertexFlagUVWQ3 = GFXVERTEXFLAG_TEXCOORD_POINT4F(3),
   GFXVertexFlagUVWQ4 = GFXVERTEXFLAG_TEXCOORD_POINT4F(4),
   GFXVertexFlagUVWQ5 = GFXVERTEXFLAG_TEXCOORD_POINT4F(5),
   GFXVertexFlagUVWQ6 = GFXVERTEXFLAG_TEXCOORD_POINT4F(6),
   GFXVertexFlagUVWQ7 = GFXVERTEXFLAG_TEXCOORD_POINT4F(7),
};

inline bool isGFXTextureChannelPresent(const U32 vertFormat, const U32 channel, const U32 count)
{
   // Parse out channel count
   const U32 chanCount = (vertFormat & GFXVertexFlagTexureCountMask) >> 8;

   // Make sure the channel is present - channel is 0-7 inclusive
   if(chanCount <= channel)
      return false;

   // Ok, it is - check if the count matches - count is 1-4 inclusive
   // so index zero is skipped.
   U32 lookup[5] = { 0,3,0,1,2 };
   const U32 soughtMask = (lookup[count] << ( channel * 2 + 16 ));
   return ((vertFormat & soughtMask) == soughtMask);
}

// Index Formats
enum GFXIndexFormat 
{
   GFXIndexFormat_FIRST = 0,
   GFXIndexFormat16 = 0,
   GFXIndexFormat32,
   GFXIndexFormat_COUNT
};

// bins are rendered in this order
// Note: This all will change drastically when new render manager is in -patw
enum GFXRenderBinTypes
{
   GFXBin_First = 0,
   GFXBin_Sky,
   GFXBin_Begin,
   GFXBin_Interior,
   GFXBin_InteriorDynamicLighting,
   GFXBin_MiscObject,

   GFXBin_MeshField,
   GFXBin_OnFieldTranslucent,

   GFXBin_Mesh,
   GFXBin_Decal,
   GFXBin_Refraction,
   GFXBin_Water,
   GFXBin_Glow,
   GFXBin_Foliage,
   GFXBin_Shadow,			//Ray: 存在于有半透明物体之上的阴影混合可能会有问题，但貌似不严重
   GFXBin_MeshQuery,
   GFXBin_Translucent,
   GFXBin_MeshShadowQuery,
   GFXBin_NumRenderBins
};

enum GFXAttribType {
	GFX_TYPE_GENERIC  = 0,
	GFX_TYPE_VERTEX   = 1,  /**< POSITION */
	GFX_TYPE_NORMAL   = 2,  /**< NORMAL */
	GFX_TYPE_TEXCOORD = 3,  /**< TEXCOORD<i>n</i> */
	GFX_TYPE_COLOR    = 4,  /**< COLOR<i>n</i> */
	GFX_TYPE_TANGENT  = 5,  /**< TANGENT */
	GFX_TYPE_BINORMAL = 6,  /**< BINORMAL */
};

/**
Vertex attribute format enumeration.  These values are used as input to
constructing vertex or stream attributes; they map 1:1 with, for example,
D3DDECLTYPE_<FORMAT> in Direct3D and the 'type' parameter to gl<ARRAY>Pointer()
in OpenGL.
*/
enum GFXAttribFormat {
	GFX_FORMAT_FLOAT    = 0, /**< FLOAT<i>n</i> */
	GFX_FORMAT_UBYTE    = 1, /**< UBYTE */
	GFX_FORMAT_UBYTEUN  = 2, /**< UBYTEUN */
	GFX_FORMAT_SHORT    = 3, /**< SHORT */
	GFX_FORMAT_USHORT   = 4, /**< USHORT */
	GFX_FORMAT_SHORTUN  = 5, /**< SHORTUN */
	GFX_FORMAT_HALF     = 6, /**< HALF */
	GFX_FORMAT_DEC      = 7, /**< DEC */
	GFX_FORMAT_UDECUN   = 8, /**< UDECUN */
	GFX_FORMAT_D3DCOLOR = 9, /**< D3DCOLOR */
};
#endif
