#include "d3dx8Wrapper.h"

#ifdef _MSC_VER
#pragma comment(lib, "d3dx8.lib")
#endif

HRESULT GFXD3D8XCreateTextureFromFileA
(LPDIRECT3DDEVICE8         pDevice,
 LPCSTR                    pSrcFile,
 LPDIRECT3DTEXTURE8*       ppTexture)
{
   return D3DXCreateTextureFromFileA(pDevice, pSrcFile, ppTexture);
}


HRESULT GFXD3D8XLoadSurfaceFromMemory
(LPDIRECT3DSURFACE8        pDestSurface,
 CONST PALETTEENTRY*       pDestPalette,
 CONST RECT*               pDestRect,
 LPCVOID                   pSrcMemory,
 D3DFORMAT                 SrcFormat,
 UINT                      SrcPitch,
 CONST PALETTEENTRY*       pSrcPalette,
 CONST RECT*               pSrcRect,
 DWORD                     Filter,
 D3DCOLOR                  ColorKey)
{
   return D3DXLoadSurfaceFromMemory(pDestSurface, pDestPalette, pDestRect, pSrcMemory, SrcFormat, 
                                       SrcPitch, pSrcPalette, pSrcRect, Filter, ColorKey);
}

HRESULT GFXD3D8XLoadSurfaceFromSurface
(LPDIRECT3DSURFACE8        pDestSurface,
 CONST PALETTEENTRY*       pDestPalette,
 CONST RECT*               pDestRect,
 LPDIRECT3DSURFACE8        pSrcSurface,
 CONST PALETTEENTRY*       pSrcPalette,
 CONST RECT*               pSrcRect,
 DWORD                     Filter,
 D3DCOLOR                  ColorKey)
{
   return D3DXLoadSurfaceFromSurface(pDestSurface, pDestPalette, pDestRect, pSrcSurface, pSrcPalette, pSrcRect, Filter, ColorKey);
}

HRESULT GFXD3D8XCreateVolumeTexture
(LPDIRECT3DDEVICE8         pDevice,
 UINT                      Width,
 UINT                      Height,
 UINT                      Depth,
 UINT                      MipLevels,
 DWORD                     Usage,
 D3DFORMAT                 Format,
 D3DPOOL                   Pool,
 LPDIRECT3DVOLUMETEXTURE8* ppVolumeTexture)
{
   return D3DXCreateVolumeTexture(pDevice, Width, Height, Depth, MipLevels, Usage, Format, Pool, ppVolumeTexture);
}

HRESULT GFXD3D8XCreateTexture 
(LPDIRECT3DDEVICE8         pDevice,
 UINT                      Width,
 UINT                      Height,
 UINT                      MipLevels,
 DWORD                     Usage,
 D3DFORMAT                 Format,
 D3DPOOL                   Pool,
 LPDIRECT3DTEXTURE8*       ppTexture)
{
   return D3DXCreateTexture(pDevice, Width, Height, MipLevels, Usage, Format, Pool, ppTexture);
}

HRESULT GFXD3D8XLoadVolumeFromMemory
(LPDIRECT3DVOLUME8         pDestVolume,
 CONST PALETTEENTRY*       pDestPalette,
 CONST D3DBOX*             pDestBox,
 LPCVOID                   pSrcMemory,
 D3DFORMAT                 SrcFormat,
 UINT                      SrcRowPitch,
 UINT                      SrcSlicePitch,
 CONST PALETTEENTRY*       pSrcPalette,
 CONST D3DBOX*             pSrcBox,
 DWORD                     Filter,
 D3DCOLOR                  ColorKey)
{
   return D3DXLoadVolumeFromMemory(pDestVolume, pDestPalette, pDestBox, pSrcMemory, SrcFormat, SrcRowPitch, SrcSlicePitch, pSrcPalette,
                                    pSrcBox, Filter, ColorKey);
}

HRESULT GFXD3D8XLoadSurfaceFromFileInMemory
(LPDIRECT3DSURFACE8 pDestSurface,
 CONST PALETTEENTRY * pDestPalette,
 CONST RECT * pDestRect,
 LPCVOID pSrcData,
 UINT SrcData,
 CONST RECT * pSrcRect,
 DWORD Filter,
 D3DCOLOR ColorKey,
 D3DXIMAGE_INFO * pSrcInfo)
{
   return D3DXLoadSurfaceFromFileInMemory(pDestSurface, pDestPalette, pDestRect, pSrcData, SrcData, pSrcRect, Filter, ColorKey, pSrcInfo);
}

D3DXMATRIX*  GFXD3D8XMatrixOrthoOffCenterLH
(D3DXMATRIX *pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn,
 FLOAT zf )
{
   return D3DXMatrixOrthoOffCenterRH(pOut, l, r, b, t, zn, zf);
}

D3DXMATRIX*  GFXD3D8XMatrixPerspectiveFovRH
(D3DXMATRIX *pOut, FLOAT fovy, FLOAT Aspect, FLOAT zn, FLOAT zf )
{
   return D3DXMatrixPerspectiveFovRH(pOut, fovy, Aspect, zn, zf);
}
