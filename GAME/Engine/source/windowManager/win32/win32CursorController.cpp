//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include <windows.h>
#include <tchar.h>
#include "math/mMath.h"
#include "windowManager/win32/win32Window.h"
#include "windowManager/win32/win32WindowMgr.h"
#include "windowManager/win32/winDispatch.h"
#include "windowManager/win32/win32CursorController.h"
#include "platform/platformInput.h"
#include "platformwin32/platformWin32.h"
#include <zmouse.h>
#include "gfx\gBitmap.h"

#ifdef NTJ_CLIENT
#include "../VisualStudio2008/game/resource.h"
static struct { const char* cursorName; const char* compname; DWORD resID;} sgCursorShapeMap[]=
{
	{"Default",		"Default",		IDR_DEFAULT },
	{"Attack",		"Attack",		IDR_ATTACK	},
	{"BatchBuy",	"BatchBuy",		IDR_BATCHBUY},
	{"Buy",			"Buy",			IDR_BUY		},
	{"Cutting",		"Cutting",		IDR_CUTTING },
	{"Destory",		"Destory",		IDR_DESTORY },
	{"Dialog",		"Dialog",		IDR_DIALOG	},
	{"Fish",		"Fish",			IDR_FISH	},
	{"Gather",		"Gather",		IDR_GATHER	},
	{"Mine",		"Mine",			IDR_MINE	},
	{"Pickup",		"Pickup",		IDR_PICKUP	},
	{"PlayerTrade",	"PlayerTrade",	IDR_PLAYERTRADE },
	{"Repair",		"Repair",		IDR_REPAIR	},
	{"Sell",		"Sell",			IDR_SELL	},
	{"Spilt",		"Spilt",		IDR_SPILT	},
	{"Stall",		"Stall",		IDR_STALL	},
	{"Using",		"Using",		IDR_USING	},
	{"tame",		"tame",			IDR_TAME	},
	{"Identify",	"Identify",		IDR_IDENTIFY},
	{"",			"",				0}
};
#else
static struct { const char* cursorName; const char* compname; LPTSTR resID;} sgCursorShapeMap[]=
{
	{"Default",		"Default",		IDC_ARROW },
	{"Arrow",		"Arrow",		IDC_ARROW },
	{"Wait",		"Wait",			IDC_WAIT },
	{"Plus",		"Plus",			IDC_CROSS },
	{"ResizeVert",	"ResizeVert",	IDC_SIZEWE },
	{"ResizeHorz",	"ResizeHorz",	IDC_SIZENS },
	{"ResizeAll",	"ResizeAll",	IDC_SIZEALL },
	{"IBeam",		"IBeam",		IDC_IBEAM },
	{"ResizeNESW",	"ResizeNESW",	IDC_SIZENESW },
	{"ResizeNWSE",	"ResizeNWSE",	IDC_SIZENWSE },
	{"",			"",				0}
};
#endif

StringTableEntry Win32CursorController::getSysCursor(U32 index)
{
	if(index >= 9)
		return "Default";
	return sgCursorShapeMap[index].cursorName;
}

// CodeReview I've duplicated this 'cache' trick for system settings
// because they're unlikely to change and calling into the OS for values
// repeatedly is just silly to begin with.  [6/29/2007 justind]
U32 Win32CursorController::getDoubleClickTime()
{
	static S32 sPlatWinDoubleClicktime = -1;
	if( sPlatWinDoubleClicktime == -1 )
		sPlatWinDoubleClicktime = GetDoubleClickTime();
	return sPlatWinDoubleClicktime;
}
S32 Win32CursorController::getDoubleClickWidth()
{
	static S32 sPlatWinDoubleClickwidth = -1;
	if( sPlatWinDoubleClickwidth == -1 )
		sPlatWinDoubleClickwidth = GetSystemMetrics(SM_CXDOUBLECLK);
	return sPlatWinDoubleClickwidth;
}
S32 Win32CursorController::getDoubleClickHeight()
{
	static S32 sPlatWinDoubleClickheight = -1;
	if( sPlatWinDoubleClickheight == -1 )
		sPlatWinDoubleClickheight = GetSystemMetrics(SM_CYDOUBLECLK);
	return sPlatWinDoubleClickheight;
}

void Win32CursorController::setCursorPosition( S32 x, S32 y )
{
	::SetCursorPos(x, y);
}

void Win32CursorController::getCursorPosition( Point2I &point )
{
	POINT rPoint;
	::GetCursorPos( &rPoint );

	// Return 
	point.x = rPoint.x;
	point.y = rPoint.y;
}

// ----------------------------------------------------------------------------
// 设置光标可视或隐藏
void Win32CursorController::setCursorVisible( bool visible )
{
	if( visible )
		ShowCursor( true );
	else
		while( ShowCursor(false) > 0 );
}

// ----------------------------------------------------------------------------
// 当前光标是否可视
bool Win32CursorController::isCursorVisible()
{
	CURSORINFO rCursorInfo;
	rCursorInfo.cbSize = sizeof(CURSORINFO);
	if( !GetCursorInfo( &rCursorInfo ) )
	{
		//DWORD error = GetLastError();
		return false;
	}
	return (bool)(rCursorInfo.flags == CURSOR_SHOWING);
}

// ----------------------------------------------------------------------------
// 更新当前光标
void Win32CursorController::pushCursor(StringTableEntry cursorname)
{
	mCursors.increment();
	mCursors.last().mCursorName = cursorname;
	setCursorShape(mCursors.last());
}

// ----------------------------------------------------------------------------
// 取消当前光标，恢复前一个光标
void Win32CursorController::popCursor()
{
	// 在pop一个光标时，确定保留至少有一个在
	if(mCursors.size() <= 1)
		return;
	PlatformCursor& cursor = mCursors.last();
	DeleteObject((HCURSOR)cursor.mCursorHandle);
	cursor.mCursorHandle = NULL;
	cursor.mCursorName = NULL;
	mCursors.decrement();
	::SetCursor((HCURSOR)mCursors.last().mCursorHandle);
}

// ----------------------------------------------------------------------------
// 刷新光标
void Win32CursorController::refreshCursor()
{
	if( mCursors.size() == 0 )
		return;

	::SetCursor((HCURSOR)mCursors.last().mCursorHandle);
}


#define CURRSOR_ITEMARROW  "gameres/gui/images/itemarrow.png"

// ----------------------------------------------------------------------------
// 获取Item的光标箭头数据
void GetItemArrowBits(DWORD** outBits, U32& width, U32& height)
{
	Stream *fileStream = ResourceManager->openStream(CURRSOR_ITEMARROW);
	if(!fileStream)
		return;
	GBitmap tBmp;
	if(!tBmp.readPNG(*fileStream))
	{
		ResourceManager->closeStream(fileStream);
		return;
	}
	
	ResourceManager->closeStream(fileStream);

	width = tBmp.getWidth();
	height = tBmp.getHeight();
	U8* pSrcBits = tBmp.getBits(0,0);
	DWORD* pTagBits = new DWORD[width * height];
	U8* src = pSrcBits;
	DWORD* lpdwPixel = pTagBits;
	for(U32 i = 0; i < height ; ++i)
	{
		for(U32 j = 0; j < width ; ++j)
		{
			if(src[3] < 255)
				*lpdwPixel = 0;
			else
				*lpdwPixel = src[3] << 24 | src[0] << 16 | src[1] << 8 | src[2];
			lpdwPixel++;
			src += 4;
		}
	}
	*outBits = pTagBits;
}

// ----------------------------------------------------------------------------
// 由RGBA格式或RGB格式的像素数据创建带Alpha通道的光标
HCURSOR CreateAlphaCursor(U32 Width, U32 Height, U8* pSrcBits, U32 bytesPerPixel)
{
	if((Width != 32 && Width != 64) || (Height != 32 && Height != 64))
	{
		Con::errorf("Mouse cursor width or height error(%d * %d)", Width, Height);
		return NULL;
	}

	if(bytesPerPixel < 3)
	{
		Con::errorf("Mouse image format error: %d", bytesPerPixel);
		return NULL;
	}

	// 因PNG的ICON图可能会大于32*32，所以要处理缩放
	bool scale = (Width == 64 || Height == 64) ? true : false;
	static DWORD* pArrowBits = NULL;
	static U32 ArrowWidth = 0;
	static U32 ArrowHeight = 0;

	//若是需要放缩的64*64ICON,则需要另外附加一个箭头图(itemarrow.png)
	if(scale)
	{
		 if(ArrowWidth == 0 && ArrowHeight == 0)
		 {
			 GetItemArrowBits(&pArrowBits, ArrowWidth, ArrowHeight);
		 }
	}

	// 目前规定光标ICON固定为32*32大小
	const DWORD dwWidth		= 32;
	const DWORD dwHeight	= 32;
	// RGB格式时需设置一个掩码色
	const DWORD cTransparent= 255 << 24 | 0 << 16 | 128 << 8 | 128;
	HCURSOR hCursor = NULL;
	void *lpBits;

	BITMAPV5HEADER bi;
	ZeroMemory(&bi,sizeof(BITMAPV5HEADER));
	bi.bV5Size			= sizeof(BITMAPV5HEADER);
	bi.bV5Width			= dwWidth;
	bi.bV5Height		= dwHeight;
	bi.bV5Planes		= 1;
	bi.bV5BitCount		= 32;
	bi.bV5Compression	= BI_BITFIELDS;
	bi.bV5RedMask		= 0x00FF0000;
	bi.bV5GreenMask		= 0x0000FF00;
	bi.bV5BlueMask		= 0x000000FF;
	bi.bV5AlphaMask		= 0xFF000000; 

	HDC hdc				= ::GetDC(NULL);
	HDC hMemDC			= ::CreateCompatibleDC(hdc);
	HBITMAP hBitmap		= ::CreateDIBSection(hdc, (BITMAPINFO *)&bi, DIB_RGB_COLORS, (void **)&lpBits, NULL, (DWORD)0);
	HBITMAP hMonoBitmap	= ::CreateBitmap(dwWidth,dwHeight,1,1,NULL);
	HBITMAP hOldBitmap	= (HBITMAP)::SelectObject(hMemDC, hBitmap);	

	DWORD *lpdwPixel = (DWORD *)lpBits;
	U8* src = pSrcBits;
	for(U32 i = 0; i < Height ; ++i)
	{
		src = pSrcBits + (Height - i - 1) * Width * bytesPerPixel;
		for(U32 j = 0; j < Width ; ++j)
		{			
			if(scale && (i % 2 == 1 || j % 2 == 1))
			{
				src += bytesPerPixel;
				continue;
			}
			if(bytesPerPixel == 3)
			{
				*lpdwPixel = 255 << 24 | src[0] << 16 | src[1] << 8 | src[2];
				if(*lpdwPixel == cTransparent)
					*lpdwPixel = 0;
			}
			else//bytesPerPixel = 4
			{
				*lpdwPixel = src[3] << 24 | src[0] << 16 | src[1] << 8 | src[2];				
			}
			lpdwPixel++;
			src += bytesPerPixel;
		}
	}

	if(scale)
	{
		lpdwPixel = (DWORD *)lpBits;
		U32 w = 0, h = 0;
		for(U32 i = 0; i < dwHeight; ++i)
		{
			h = dwHeight - i - 1;
			for(U32 j = 0; j < dwWidth; ++j)
			{
				w = j;
				if(h < ArrowHeight && w < ArrowWidth && pArrowBits[h * ArrowWidth + w] != 0)
					*lpdwPixel = pArrowBits[h * ArrowWidth + w];
				lpdwPixel++;
			}
		}
	}
	

	ICONINFO ii;
	ii.fIcon	= FALSE;
	ii.xHotspot = 0;
	ii.yHotspot = 0;
	ii.hbmMask	= hMonoBitmap;
	ii.hbmColor = hBitmap;

	hCursor = CreateIconIndirect(&ii);

	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
	DeleteObject(hBitmap);          
	DeleteObject(hMonoBitmap);
	ReleaseDC(NULL,hdc);
	return hCursor;
}

// ----------------------------------------------------------------------------
// 从内部资源获取动画光标
HCURSOR LoadAniCursor(DWORD dwResID) 
{
	TCHAR szTempPath[_MAX_PATH]; 
	TCHAR szTempFile[_MAX_PATH]; 

	if(!GetTempPath(_MAX_PATH, szTempPath)) return false; 
	if(!GetTempFileName(szTempPath, _T(L"ani"), 0, szTempFile)) return false; 

	HANDLE hFile = CreateFile(szTempFile, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0); 
	if(hFile == INVALID_HANDLE_VALUE) goto failure; 

	HRSRC hrsrc = FindResource(0, MAKEINTRESOURCE(dwResID), RT_ANICURSOR); 
	if(hrsrc == 0) goto failure; 
	HGLOBAL hrc = LoadResource(0, hrsrc); 
	if(hrc == 0) goto failure; 
	LPBYTE pRcData = (LPBYTE)LockResource(hrc); 
	if(pRcData == 0) goto failure; 
	DWORD dwRcSize = SizeofResource(0, hrsrc); 
	if(dwRcSize == 0) goto failure; 

	DWORD cb = 0; 
	if(!WriteFile(hFile, pRcData, dwRcSize, &cb, 0)) goto failure; 
	CloseHandle(hFile); 

	HCURSOR hcur = (HCURSOR)LoadImage(0, szTempFile, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE | LR_SHARED); 
	DeleteFile(szTempFile); 
	return hcur; 

failure: 
	if(hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile); 
	DeleteFile(szTempFile); 
	return 0; 
}


// ----------------------------------------------------------------------------
// 设置光标ICON数据
void Win32CursorController::setCursorShape(PlatformCursor& cursor)
{
	// 这里注意，会优先判定是否系统光标
	for(S32 i = 0; sgCursorShapeMap[i].resID != NULL; ++i)
	{
		if(dStricmp(cursor.mCursorName, sgCursorShapeMap[i].compname) == 0)
		{
			
#ifdef NTJ_CLIENT
			cursor.mCursorHandle = LoadAniCursor(sgCursorShapeMap[i].resID);
#else
			cursor.mCursorHandle = LoadCursor(NULL, sgCursorShapeMap[i].resID);
#endif
			if(cursor.mCursorHandle)
				SetCursor((HCURSOR)cursor.mCursorHandle);
			return;
		}
	}

	char path[128] = "\n";
	dSprintf(path, sizeof(path), "%s%s%s", "gameres/data/icon/", cursor.mCursorName, ".png");

	static GBitmap tBmp;
	tBmp.deleteImage();

	Stream *fileStream = ResourceManager->openStream(path);
	if(!fileStream)
	{
		dSprintf(path, sizeof(path), "%s%s%s", "gameres/data/icon/", cursor.mCursorName, ".bmp");
		fileStream = ResourceManager->openStream(path);
		if(!fileStream)
		{
			return;
		}
		else
		{			
			if( !tBmp.readMSBmp(*fileStream) )
			{
				ResourceManager->closeStream(fileStream);
				return;
			}
		}		
	}
	else
	{
		if(!tBmp.readPNG(*fileStream))
		{
			ResourceManager->closeStream(fileStream);
			return;
		}
	}
	ResourceManager->closeStream(fileStream);
	
	cursor.mCursorHandle = CreateAlphaCursor(tBmp.getWidth(), tBmp.getHeight(), tBmp.getBits(0,0), tBmp.bytesPerPixel);
	::SetCursor( (HCURSOR)cursor.mCursorHandle );
}