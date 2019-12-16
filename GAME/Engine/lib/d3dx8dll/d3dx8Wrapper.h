#include <d3dx8.h>

#ifdef D3DX8DLL_EXPORTS
#define GFXD3D8XDECL __declspec( dllexport )
#else
#define GFXD3D8XDECL
#endif

extern "C"
{

GFXD3D8XDECL HRESULT GFXD3D8XCreateTextureFromFileA
               (LPDIRECT3DDEVICE8         pDevice,
                LPCSTR                    pSrcFile,
                LPDIRECT3DTEXTURE8*       ppTexture);


GFXD3D8XDECL HRESULT GFXD3D8XLoadSurfaceFromMemory
               (LPDIRECT3DSURFACE8        pDestSurface,
               CONST PALETTEENTRY*       pDestPalette,
               CONST RECT*               pDestRect,
               LPCVOID                   pSrcMemory,
               D3DFORMAT                 SrcFormat,
               UINT                      SrcPitch,
               CONST PALETTEENTRY*       pSrcPalette,
               CONST RECT*               pSrcRect,
               DWORD                     Filter,
               D3DCOLOR                  ColorKey);

GFXD3D8XDECL HRESULT GFXD3D8XLoadSurfaceFromSurface
               (LPDIRECT3DSURFACE8        pDestSurface,
               CONST PALETTEENTRY*       pDestPalette,
               CONST RECT*               pDestRect,
               LPDIRECT3DSURFACE8        pSrcSurface,
               CONST PALETTEENTRY*       pSrcPalette,
               CONST RECT*               pSrcRect,
               DWORD                     Filter,
               D3DCOLOR                  ColorKey);

GFXD3D8XDECL HRESULT GFXD3D8XCreateVolumeTexture
               (LPDIRECT3DDEVICE8         pDevice,
               UINT                      Width,
               UINT                      Height,
               UINT                      Depth,
               UINT                      MipLevels,
               DWORD                     Usage,
               D3DFORMAT                 Format,
               D3DPOOL                   Pool,
               LPDIRECT3DVOLUMETEXTURE8* ppVolumeTexture);

GFXD3D8XDECL HRESULT GFXD3D8XCreateTexture 
               (LPDIRECT3DDEVICE8         pDevice,
               UINT                      Width,
               UINT                      Height,
               UINT                      MipLevels,
               DWORD                     Usage,
               D3DFORMAT                 Format,
               D3DPOOL                   Pool,
               LPDIRECT3DTEXTURE8*       ppTexture);

GFXD3D8XDECL HRESULT GFXD3D8XLoadVolumeFromMemory
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
               D3DCOLOR                  ColorKey);

GFXD3D8XDECL HRESULT GFXD3D8XLoadSurfaceFromFileInMemory
               (LPDIRECT3DSURFACE8 pDestSurface,
               CONST PALETTEENTRY * pDestPalette,
               CONST RECT * pDestRect,
               LPCVOID pSrcData,
               UINT SrcData,
               CONST RECT * pSrcRect,
               DWORD Filter,
               D3DCOLOR ColorKey,
               D3DXIMAGE_INFO * pSrcInfo);

GFXD3D8XDECL D3DXMATRIX*  GFXD3D8XMatrixOrthoOffCenterLH
               (D3DXMATRIX *pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn,
               FLOAT zf );

GFXD3D8XDECL D3DXMATRIX*  GFXD3D8XMatrixPerspectiveFovRH
               (D3DXMATRIX *pOut, FLOAT fovy, FLOAT Aspect, FLOAT zn, FLOAT zf );

};
