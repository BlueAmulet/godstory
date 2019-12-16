//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GFXFONTBATCHER_H_
#define _GFXFONTBATCHER_H_

#include "gfx/gfxDevice.h"
#include "gfx/gFont.h"
#include <string>
#include <hash_map>
#include <string>

#include <windows.h>

using namespace std; 
using namespace stdext; 

#define TEXT_MAG 1
class GFXStateBlock;

class FontRenderBatcher
{
public:
   FontRenderBatcher();

   struct CharMarker 
   {
      S32 c;
      F32 x;
      GFXVertexColor color; 
      PlatformFont::CharInfo *ci;
   };

   struct SheetMarker
   {
      S32 numChars;
      S32 startVertex;
      CharMarker charIndex[1];
   };

   FrameAllocatorMarker mAllocator;

   Vector<SheetMarker *> mSheets;
   GFont *mFont;
   U32 mLength;

   SheetMarker &getSheetMarker(U32 sheetID);

   void init(GFont *font, U32 n);

   void queueChar(UTF16 c, S32 &currentX, GFXVertexColor &currentColor);

   void _render(F32 rot, const Point2F &offset);
   void render(F32 rot, const Point2F &offset );
   void renderBorder( F32 rot, const Point2F &offset );
private:
	static GFXStateBlock* mSetSB;
public:
	static void initsb();
	static void shutdown();
	//
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();
};

class CommonFontEX
{
public:
	struct FontEXCharInfo
	{
		U32	width;          // 字符像素宽度
		U32	height;         // 字符像素高度
		S32	xOrigin;
		S32	yOrigin;
		U32 x_index;
		U32 y_index;
		S32 xIncrement;
	};

	CommonFontEX();
	~CommonFontEX();

	/**
	@brief		创建

	@return		是否成功
	*/  
	bool        Create(char* FontName, U32 FontHeight,bool UseGRAY8_BITMAP = true, S32 offsetx = 0, S32 offsety = 0, bool aliased = true, bool bItalic = false, bool bBold = false);

	/**
	@brief		获取纹理宽度

	@return		纹理宽度
	*/  
	U32         GetTextureWidth();

	/**
	@brief		获取纹理高度

	@return		纹理高度
	*/  
	U32         GetTextureHeight();

	/**
	@brief		设置绘制文字

	@return		是否成功
	*/ 
	bool        SetText(Point2I pos, const ColorI &dwColor, char* strText, bool border = false, ColorI bordercolor = ColorI(0, 0, 0, 0), bool transition = true);

	/**
	@brief		开始绘制

	@return		空
	*/ 
	void        BeginRender();

	/**
	@brief		绘制

	@return		空
	*/ 
	void        Render();

	/**
	@brief		设置换行长度

	@return		空
	*/ 
	void        SetLineDis(U32 linedis);

	/**
	@brief		获取换行长度

	@return		换行长度
	*/ 
	U32         GetLineDis();

	/**
	@brief		设置行字符间隔

	@return		空
	*/ 
	void        SetLineChangeDis(U32 data);

	/**
	@brief		设置列字符间隔

	@return		空
	*/ 
	void        SetColChangeDis(U32 data);

	/**
	@brief		获取行字符间隔

	@return		行字符间隔
	*/ 
	U32         GetLineChangeDis();

	/**
	@brief		获取列字符间隔

	@return		列字符间隔
	*/ 
	U32         GetColChangeDis();

	/**
	@brief		获取最大字符数量(获得的数量为不描边的情况下,而描边的情况下一次最多只能绘制1213个字)

	@return		最大字符数量
	*/ 
	U32         GetMaxStringNum();
	
	/**
	@brief		设置alpha参考值

	@return		空
	*/ 
	void        SetAlphaRef(U8 AlphaRef);

	/**
	@brief		获取alpha参考值

	@return		alpha参考值
	*/ 
	U8          GetAlphaRef();

	/**
	@brief		获取一串字符的宽度

	@return		宽度
	*/ 
	U32         getStrWidth(char* strText);

	/**
	@brief		获取一串描边字符的宽度

	@return		宽度
	*/ 
	U32         getStrWidthOutline(char* strText);

	/**
	@brief		获取一串字符的宽度

	@return		宽度
	*/ 
	U32         getStrWidthEx(char* strText,S32* count, S32 WD);

	/**
	@brief		获取一串字符的宽度

	@return		宽度
	*/ 
	U32         getStrNWidth(char* strText, S32 pos);

	/**
	@brief		获取字符的高度

	@return		高度
	*/ 
	U32         getHeight();

	/**
	@brief		获取描边字符的高度

	@return		描边字符的高度
	*/ 
	U32         getHeightOutline();

	/**
	@brief		增加计数

	@return		空
	*/ 
	void        AddRef();

	/**
	@brief		减少计数

	@return		true意思是被删除
	*/ 
	bool		ReduceRef();

	/**
	@brief		获取计数

	@return		空
	*/ 
	U32         GetRef();

	/**
	@brief		获取名字

	@return		名字
	*/ 
	string      GetName();

	U32         getBreakPos(const UTF16 *string, U32 strlen, U32 width, bool breakOnWhitespace);
	U32         getBreakPos(const UTF8  *string, U32 strlen, U32 width, bool breakOnWhitespace);

	const U32 getBaseline() const { return m_iBaseLine; }

	const U32 getDescent() const  { return m_dwFontPointHeight - m_iBaseLine; }
#ifndef NTJ_SERVER
	static void      init();

	static void      shutdown();
#endif
protected:
	/**
	@brief		初始化变量

	@return		空
	*/  
	void        InitSys(char* FontName, U32 FontHeight, S32 offsetx, S32 offsety, bool aliased, bool bItalic, bool bBold);

	/**
	@brief		获取合适的内存空间

	@return		空
	*/  
	void        GetBestPosCache(U32 *x, U32 *y);

	/**
	@brief		内存空间是否存在

	@return		是否存在
	*/  
	bool        IsExistCache(char *ch, FontEXCharInfo* size);

	/**
	@brief		设置内存空间

	@return		空
	*/ 
	void        SetCache(char *strText, U32 x, U32 y, FontEXCharInfo* size);

	/**
	@brief		填充字符

	@return		是否成功
	*/ 
	bool        FillCharacter(Point2I pos, ColorI dwColor, bool border, ColorI bordercolor, FontEXCharInfo* size, bool transition);

	/**
	@brief		初始化顶点结构

	@return		空
	*/ 
	void        InitFont2DVertex(F32 x, F32 y, ColorF color, F32 u, F32 v);

private:
    friend class CommonFontManager;

	/**
	@brief		设置名字

	@return		空
	*/ 
	void        SetName(string name);

	GFXTexHandle                                                                 m_pTexture;                                        //纹理
	static const   U32                                                           m_dwTexWidth;                                      //纹理宽度
	static const   U32                                                           m_dwTexHeight;                                     //纹理高度
	static HBITMAP                                                               m_hBitmap;
	static HDC		                                                             m_hDC;
	U32                                                                          m_dwFontPointWidth;                                //单个字体宽度
	U32                                                                          m_dwFontPointHeight;                               //单个字体高度
	U32                                                                          m_dwFontMaxLines;                                  //纹理一行所能容下的最多字符
	U32                                                                          m_dwFontMaxStringLength;                           //纹理一列所能容下的最多字符
    F32                                                                          m_fFontMaxStringLengthInv;
	HFONT	                                                                     m_hFont;                                           //windows字体
	U32                                                                          m_dwLineDis;                                       //换行长度
	static GFXVertexPWCT*                                                        m_pVertex;                                         //顶点结构指针
	U32                                                                          m_dwFontVertexNum;                                 //当前渲染顶点数量
	F32                                                                          m_fInvMaxLines;                                    //纹理一行所能容下的最多字符倒数
	F32                                                                          m_fInvMaxStringLength;                             //纹理一列所能容下的最多字符倒数
	U32                                                                          m_dwRenderTriangleNum;                             //当前渲染的三角形数量
	static U32                                                                   m_dwNumVerts;                                      //最大顶点数量
	U32                                                                          m_dwLineChangeDis;                                 //行字符间隔
	U32                                                                          m_dwColChangeDis;                                  //列字符间隔
	static GFXPrimitiveBufferHandle                                              primBuff;
    static GFXVertexBufferHandle<GFXVertexPWCT>                                  mVertBuff;
	U8                                                                           m_uAlphaRef;
	U32                                                                          m_uMaxStringNum;
	static U32                                                                   m_dwIndexNum;
	static U32                                                                   m_dwMaxStringNum;
	FontEXCharInfo*                                                              m_pTextInfo;
	bool                                                                         m_bUseGRAY8_BITMAP;  	
	S32                                                                          m_iBaseLine;
	hash_map<wchar_t, FontEXCharInfo*>                                           m_hashmapFontEXCharInfo;
	U16*                                                                         m_pBufUseCount;
	U32                                                                          m_uFillNum;
	U32                                                                          m_uRefCount;
	string                                                                       m_strName;
private:
	static GFXStateBlock* mSetSB;
	static GFXStateBlock* mClearSB;
public:
	static void initsb();
	static void shutdownsb();
	//
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();
};

class CommonFontManager
{
public:
	CommonFontManager();
	~CommonFontManager();

	/**
	@brief		删除所有字体

	@return		空
	*/ 
	static      void                                                         DeleteAll();

	/**
	@brief		获取字体

	@return		字体结构指针
	*/ 
	static      CommonFontEX*												 GetFont(char* FontName, U32 FontHeight,bool UseGRAY8_BITMAP = true, S32 offsetx = 0, S32 offsety = 0, bool aliased = true, bool bItalic = false, bool bBold = false);

	/**
	@brief		打印所有字体

	@return		空
	*/ 
	static      void                                                         PrintAllFont();

protected:
	/**
	@brief		销毁

	@return		空
	*/ 
	static      void                                                         Destory();

private:
    friend class CommonFontEX;

	/**
	@brief		删除字体

	@return		是否成功
	*/ 
	static      bool                                                         DeleteFont(string code);

	static stdext::hash_map<string ,CommonFontEX*>                           m_hashmapCommonFontManager;      
};

#endif