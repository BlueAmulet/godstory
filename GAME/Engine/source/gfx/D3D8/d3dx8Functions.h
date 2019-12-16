//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

D3DX_FUNCTION( GFXD3D8XCreateTextureFromFileA, HRESULT,
               (LPDIRECT3DDEVICE8         pDevice,
               LPCSTR                    pSrcFile,
               LPDIRECT3DTEXTURE8*       ppTexture) )


D3DX_FUNCTION( GFXD3D8XLoadSurfaceFromMemory, HRESULT,
               (LPDIRECT3DSURFACE8        pDestSurface,
               CONST PALETTEENTRY*       pDestPalette,
               CONST RECT*               pDestRect,
               LPCVOID                   pSrcMemory,
               D3DFORMAT                 SrcFormat,
               UINT                      SrcPitch,
               CONST PALETTEENTRY*       pSrcPalette,
               CONST RECT*               pSrcRect,
               DWORD                     Filter,
               D3DCOLOR                  ColorKey) )

D3DX_FUNCTION( GFXD3D8XLoadSurfaceFromSurface, HRESULT,
               (LPDIRECT3DSURFACE8        pDestSurface,
               CONST PALETTEENTRY*       pDestPalette,
               CONST RECT*               pDestRect,
               LPDIRECT3DSURFACE8        pSrcSurface,
               CONST PALETTEENTRY*       pSrcPalette,
               CONST RECT*               pSrcRect,
               DWORD                     Filter,
               D3DCOLOR                  ColorKey) )

D3DX_FUNCTION( GFXD3D8XCreateVolumeTexture, HRESULT,
               (LPDIRECT3DDEVICE8         pDevice,
               UINT                      Width,
               UINT                      Height,
               UINT                      Depth,
               UINT                      MipLevels,
               DWORD                     Usage,
               D3DFORMAT                 Format,
               D3DPOOL                   Pool,
               LPDIRECT3DVOLUMETEXTURE8* ppVolumeTexture) )

D3DX_FUNCTION( GFXD3D8XCreateTexture, HRESULT, 
               (LPDIRECT3DDEVICE8         pDevice,
               UINT                      Width,
               UINT                      Height,
               UINT                      MipLevels,
               DWORD                     Usage,
               D3DFORMAT                 Format,
               D3DPOOL                   Pool,
               LPDIRECT3DTEXTURE8*       ppTexture) )

D3DX_FUNCTION( GFXD3D8XLoadVolumeFromMemory, HRESULT,
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
               D3DCOLOR                  ColorKey) )

D3DX_FUNCTION( GFXD3D8XLoadSurfaceFromFileInMemory, HRESULT,
               (LPDIRECT3DSURFACE8 pDestSurface,
               CONST PALETTEENTRY * pDestPalette,
               CONST RECT * pDestRect,
               LPCVOID pSrcData,
               UINT SrcData,
               CONST RECT * pSrcRect,
               DWORD Filter,
               D3DCOLOR ColorKey,
               D3DXIMAGE_INFO * pSrcInfo) )

D3DX_FUNCTION( GFXD3D8XMatrixOrthoOffCenterLH, D3DXMATRIX*,
               (D3DXMATRIX *pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn,
               FLOAT zf ))

D3DX_FUNCTION( GFXD3D8XMatrixPerspectiveFovRH, D3DXMATRIX*,
               (D3DXMATRIX *pOut, FLOAT fovy, FLOAT Aspect, FLOAT zn, FLOAT zf ))
