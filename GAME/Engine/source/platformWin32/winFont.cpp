//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platformWin32/platformWin32.h"
#include "platformWin32/winFont.h"
#include "gfx/gFont.h"
#include "gfx/gFont.h"
#include "gfx/gBitmap.h"
#include "math/mRect.h"
#include "console/console.h"
#include "core/unicode.h"
#include <tchar.h>

static HDC fontHDC = NULL;
static HBITMAP fontBMP = NULL;

static U32 charsetMap[]=
{
    ANSI_CHARSET,
    SYMBOL_CHARSET,
    SHIFTJIS_CHARSET,
    HANGEUL_CHARSET,
    HANGUL_CHARSET,
    GB2312_CHARSET,
    CHINESEBIG5_CHARSET,
    OEM_CHARSET,
    JOHAB_CHARSET,
    HEBREW_CHARSET,
    ARABIC_CHARSET,
    GREEK_CHARSET,
    TURKISH_CHARSET,
    VIETNAMESE_CHARSET,
    THAI_CHARSET,
    EASTEUROPE_CHARSET,
    RUSSIAN_CHARSET,
    MAC_CHARSET,
    BALTIC_CHARSET,
};
#define NUMCHARSETMAP (sizeof(charsetMap) / sizeof(U32))

void createFontInit(void);
void createFontShutdown(void);
void CopyCharToBitmap(GBitmap *pDstBMP, HDC hSrcHDC, const RectI &r);

// ----------------------------------------------------------------------------
// 初始化构建字体对象
void createFontInit()
{
   fontHDC = CreateCompatibleDC(NULL);
   fontBMP = CreateCompatibleBitmap(fontHDC, 256, 256);
   // 添加自定义字体资源目录
   ::AddFontResourceA("fonts\\MSYH.TTF");
   ::SendNotifyMessage(HWND_BROADCAST,WM_FONTCHANGE,0,0);
}

// ----------------------------------------------------------------------------
// 销毁字体对象
void createFontShutdown()
{
   DeleteObject(fontBMP);
   DeleteObject(fontHDC);
   // 移除自定义字体资源目录
   ::RemoveFontResourceA("fonts\\MSYH.TTF");
   ::SendNotifyMessage(HWND_BROADCAST,WM_FONTCHANGE,0,0);
}

// ----------------------------------------------------------------------------
// 从位图对象复制像素数据到GBitmap对象
void CopyCharToBitmap(GBitmap *pDstBMP, HDC hSrcHDC, const RectI &r)
{
   for (S32 i = r.point.y; i < r.point.y + r.extent.y; i++)
   {
      for (S32 j = r.point.x; j < r.point.x + r.extent.x; j++)
      {
         COLORREF color = GetPixel(hSrcHDC, j, i);
         if (color)
            *pDstBMP->getAddress(j, i) = 255;
         else
            *pDstBMP->getAddress(j, i) = 0;
      }
   }
}

//-----------------------------------------------------------------------------
// WinFont class
//-----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 枚举系统中所有字体回调的函数方法(登记所有字体名到列表fonts中)
BOOL CALLBACK EnumFamCallBack(LPLOGFONT logFont, LPNEWTEXTMETRIC textMetric, DWORD fontType, LPARAM lParam)
{
   if( !( fontType & TRUETYPE_FONTTYPE ) )
      return true;

   Vector<StringTableEntry>* fonts = (Vector< StringTableEntry>*)lParam;
   const U32 len = dStrlen( logFont->lfFaceName ) * 3 + 1;
   FrameTemp<UTF8> buffer( len );
   convertUTF16toUTF8( logFont->lfFaceName, buffer, len );
   fonts->push_back( StringTable->insert( buffer ) );
   return true;
}

// ----------------------------------------------------------------------------
// 枚举系统中所有字体
void PlatformFont::enumeratePlatformFonts( Vector<StringTableEntry>& fonts, UTF16* fontFamily )
{
   EnumFontFamilies( fontHDC, fontFamily, (FONTENUMPROC)EnumFamCallBack, (LPARAM)&fonts );
}

// ----------------------------------------------------------------------------
// 创建系统平台所用字体
PlatformFont *createPlatformFont(const char *name, U32 size, U32 charset /* = TGE_ANSI_CHARSET */)
{
    PlatformFont *retFont = new WinFont;
    if(retFont->create(name, size, charset))
        return retFont;

    delete retFont;
    return NULL;
}

WinFont::WinFont() : mFont(NULL)
{
}

WinFont::~WinFont()
{
    if(mFont)
    {
        DeleteObject(mFont);
    }
}

// ----------------------------------------------------------------------------
// 创建字体
bool WinFont::create(const char *name, U32 size, U32 charset /* = TGE_ANSI_CHARSET */)
{
    if(name == NULL || size < 1)
        return false;

    if(charset > NUMCHARSETMAP)
       charset = TGE_ANSI_CHARSET;

    U32 weight = 0;
    U32 doItalic = 0;

    char* bold = dStrstr(name, "Bold");
    char* italic = dStrstr(name, "Italic");

    if (bold != NULL && bold != name)
       weight = 700;
    else if (italic != NULL && italic != name)
       doItalic = 1;

    // Just in case...
    bold = dStrstr(name, "bold");
    italic = dStrstr(name, "italic");

    if (bold != NULL && bold != name)
       weight = 700;
    else if (italic != NULL && italic != name)
       doItalic = 1;

#ifdef UNICODE
    UTF16 n[512];
    convertUTF8toUTF16((UTF8 *)name, n, sizeof(n));
	mFont = CreateFont(size,0,0,0,weight,doItalic,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,0,NONANTIALIASED_QUALITY,FF_DONTCARE,n);
#else
    mFont = CreateFont(size,0,0,0,weight,doItalic,0,0,charsetMap[charset],OUT_TT_PRECIS,0,PROOF_QUALITY,0,name);
#endif
    if(mFont == NULL)
      return false;

    SelectObject(fontHDC, fontBMP);
    SelectObject(fontHDC, mFont);
    GetTextMetrics(fontHDC, &mTextMetric);

    return true;
}

// ----------------------------------------------------------------------------
// 是否有效字符
bool WinFont::isValidChar(const UTF16 ch) const
{
    return ch != 0 /* && (ch >= mTextMetric.tmFirstChar && ch <= mTextMetric.tmLastChar)*/;
}

bool WinFont::isValidChar(const UTF8 *str) const
{
    return isValidChar(oneUTF8toUTF32(str));
}

// ----------------------------------------------------------------------------
// 获取字符的轮廓字体或点阵字体的字模像素数据
PlatformFont::CharInfo &WinFont::getCharInfo(const UTF16 ch) const
{
	static PlatformFont::CharInfo c;

	dMemset(&c, 0, sizeof(c));
	c.bitmapIndex = -1;

	static U8 scratchPad[65536];

	COLORREF backgroundColorRef = RGB(  0,   0,   0);
	COLORREF foregroundColorRef = RGB(255, 255, 255);
	SelectObject(fontHDC, fontBMP);
	SelectObject(fontHDC, mFont);
	SetBkColor(fontHDC, backgroundColorRef);
	SetTextColor(fontHDC, foregroundColorRef);

	MAT2 matrix;
	GLYPHMETRICS metrics;
	RectI clip;

	FIXED zero;
	zero.fract = 0;
	zero.value = 0;
	FIXED one;
	one.fract = 0;
	one.value = 1;

	matrix.eM11 = one;
	matrix.eM12 = zero;
	matrix.eM21 = zero;
	matrix.eM22 = one;

	LOGFONT logFont;   
	GetObject(mFont,sizeof(LOGFONT),&logFont);
	const U32 len = dStrlen( logFont.lfFaceName ) * 3 + 1;
	char szFontName[32];
	dMemset(szFontName, 0, sizeof(szFontName));
	convertUTF16toUTF8( logFont.lfFaceName, szFontName, 32 );

	// 只对所有宋体12或宋体14设置为点阵字,其它点阵字体难看
	if(dStrstr(szFontName, "宋体")  && (logFont.lfHeight == 12 || logFont.lfHeight == 14))
	{
		if(GetGlyphOutline(
							fontHDC,			// handle of device context 
							ch,					// character to query 
							GGO_BITMAP,			// format of data to return 
							&metrics,			// address of structure for metrics 
							sizeof(scratchPad),	// size of buffer for data 
							scratchPad,			// address of buffer for data 
							&matrix 			// address of transformation matrix structure  
							) != GDI_ERROR)
		{
			U32 rowStride = 32 * ( metrics.gmBlackBoxX / 32 + 1 );
			static U8 scratchPad2[65536];

			for( U32 m = 0; m < metrics.gmBlackBoxY; m++ )
			{
				const U8 t = 0x80;
				int byteNums = rowStride / 8;

				for( U32 n = 0; n < byteNums; n++ )
					for( int s = 0; s < 8; s++ )
					{
						if( scratchPad[ m * byteNums + n ] & (t >> s) )
							scratchPad2[ m * rowStride + n * 8 + s ] = 240;
						else
							scratchPad2[  m * rowStride + n * 8 + s ] = 0;
					}
			}

			c.xOffset = 0;
			c.yOffset = 0;
			c.width = metrics.gmBlackBoxX;
			c.height = metrics.gmBlackBoxY;
			c.xOrigin = metrics.gmptGlyphOrigin.x;
			c.yOrigin = metrics.gmptGlyphOrigin.y;
			c.xIncrement = metrics.gmCellIncX;

			c.bitmapData = new U8[c.width * c.height];
			AssertFatal( c.bitmapData != NULL, "Could not allocate memory for font bitmap data!");
			for(U32 y = 0; S32(y) < c.height; y++)
			{
				for(U32 x = 0; x < c.width; x++)
				{
					S32 spi = y * rowStride + x;
					if( spi >= sizeof(scratchPad))
						return c;
					c.bitmapData[y * c.width + x] = scratchPad2[spi];
				}
			}
		}
		else
		{
			SIZE size;
			GetTextExtentPoint32W(fontHDC, &ch, 1, &size);
			if(size.cx)
			{
				c.xIncrement = size.cx;
				c.bitmapIndex = 0;
			}
		}
	}
	else
	{
		if(GetGlyphOutline(
							fontHDC,			// handle of device context 
							ch,					// character to query 
							GGO_GRAY8_BITMAP,	// format of data to return 
							&metrics,			// address of structure for metrics 
							sizeof(scratchPad),	// size of buffer for data 
							scratchPad,			// address of buffer for data 
							&matrix 			// address of transformation matrix structure  
							) != GDI_ERROR)
		{
			U32 rowStride = (metrics.gmBlackBoxX + 3) & ~3; // DWORD aligned
			U32 size = rowStride * metrics.gmBlackBoxY;

			for(U32 j = 0; j < size && j < sizeof(scratchPad); j++)
			{
				U32 pad = U32(scratchPad[j]) << 2;
				if(pad > 255)
					pad = 255;
				scratchPad[j] = pad;
			}

			c.xOffset = 0;
			c.yOffset = 0;
			c.width = metrics.gmBlackBoxX;
			c.height = metrics.gmBlackBoxY;
			c.xOrigin = metrics.gmptGlyphOrigin.x;
			c.yOrigin = metrics.gmptGlyphOrigin.y;
			c.xIncrement = metrics.gmCellIncX;

			c.bitmapData = new U8[c.width * c.height];
			AssertFatal( c.bitmapData != NULL, "Could not allocate memory for font bitmap data!");
			for(U32 y = 0; S32(y) < c.height; y++)
			{
				for(U32 x = 0; x < c.width; x++)
				{
					S32 spi = y * rowStride + x;
					if( spi >= sizeof(scratchPad) )
						return c;
					c.bitmapData[y * c.width + x] = scratchPad[spi];
				}
			}
		}
		else
		{
			SIZE size;
			GetTextExtentPoint32W(fontHDC, &ch, 1, &size);
			if(size.cx)
			{
				c.xIncrement = size.cx;
				c.bitmapIndex = 0;
			}
		}
	}	
	return c;
}

PlatformFont::CharInfo &WinFont::getCharInfo(const UTF8 *str) const
{
    return getCharInfo(oneUTF8toUTF32(str));
}
