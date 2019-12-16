//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _PLATFORMFONT_H_
#define _PLATFORMFONT_H_

// 常用字符集枚举
enum FontCharset
{
    TGE_ANSI_CHARSET = 0,
    TGE_SYMBOL_CHARSET,
    TGE_SHIFTJIS_CHARSET,
    TGE_HANGEUL_CHARSET,
    TGE_HANGUL_CHARSET,
    TGE_GB2312_CHARSET,
    TGE_CHINESEBIG5_CHARSET,
    TGE_OEM_CHARSET,
    TGE_JOHAB_CHARSET,
    TGE_HEBREW_CHARSET,
    TGE_ARABIC_CHARSET,
    TGE_GREEK_CHARSET,
    TGE_TURKISH_CHARSET,
    TGE_VIETNAMESE_CHARSET,
    TGE_THAI_CHARSET,
    TGE_EASTEUROPE_CHARSET,
    TGE_RUSSIAN_CHARSET,
    TGE_MAC_CHARSET,
    TGE_BALTIC_CHARSET
};

extern const char *getCharSetName(const U32 charSet);

class PlatformFont
{
public:
   struct CharInfo
   {
      S16	bitmapIndex;    // 字符表纹理列表的索引值，注:等于-1时将不渲染，例如:\n, \r等等
      U32	xOffset;		// 字符表纹理偏移位置x
      U32	yOffset;        // 字符表纹理偏移位置y
      U32	width;          // 字符像素宽度
      U32	height;         // 字符像素高度
      S32	xOrigin;
      S32	yOrigin;
      S32	xIncrement;		// 字符间跨距
      U8*	bitmapData;     // 字符的字模像素数据
   };
   
   virtual bool isValidChar(const UTF16 ch) const = 0;
   virtual bool isValidChar(const UTF8 *str) const = 0;
   virtual U32 getFontHeight() const = 0;
   virtual U32 getFontBaseLine() const = 0;
   virtual PlatformFont::CharInfo &getCharInfo(const UTF16 ch) const = 0;
   virtual PlatformFont::CharInfo &getCharInfo(const UTF8 *str) const = 0;
   virtual bool create(const char *name, U32 size, U32 charset = TGE_ANSI_CHARSET) = 0;

   static void enumeratePlatformFonts( Vector<StringTableEntry>& fonts, UTF16* fontFamily = NULL );
};

extern PlatformFont *createPlatformFont(const char *name, U32 size, U32 charset = TGE_ANSI_CHARSET);

#endif // _PLATFORMFONT_H_
