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
	//�豸��ʧʱ����
	//
	static void releaseStateBlock();

	//
	//�豸����ʱ����
	//
	static void resetStateBlock();
};

class CommonFontEX
{
public:
	struct FontEXCharInfo
	{
		U32	width;          // �ַ����ؿ��
		U32	height;         // �ַ����ظ߶�
		S32	xOrigin;
		S32	yOrigin;
		U32 x_index;
		U32 y_index;
		S32 xIncrement;
	};

	CommonFontEX();
	~CommonFontEX();

	/**
	@brief		����

	@return		�Ƿ�ɹ�
	*/  
	bool        Create(char* FontName, U32 FontHeight,bool UseGRAY8_BITMAP = true, S32 offsetx = 0, S32 offsety = 0, bool aliased = true, bool bItalic = false, bool bBold = false);

	/**
	@brief		��ȡ������

	@return		������
	*/  
	U32         GetTextureWidth();

	/**
	@brief		��ȡ����߶�

	@return		����߶�
	*/  
	U32         GetTextureHeight();

	/**
	@brief		���û�������

	@return		�Ƿ�ɹ�
	*/ 
	bool        SetText(Point2I pos, const ColorI &dwColor, char* strText, bool border = false, ColorI bordercolor = ColorI(0, 0, 0, 0), bool transition = true);

	/**
	@brief		��ʼ����

	@return		��
	*/ 
	void        BeginRender();

	/**
	@brief		����

	@return		��
	*/ 
	void        Render();

	/**
	@brief		���û��г���

	@return		��
	*/ 
	void        SetLineDis(U32 linedis);

	/**
	@brief		��ȡ���г���

	@return		���г���
	*/ 
	U32         GetLineDis();

	/**
	@brief		�������ַ����

	@return		��
	*/ 
	void        SetLineChangeDis(U32 data);

	/**
	@brief		�������ַ����

	@return		��
	*/ 
	void        SetColChangeDis(U32 data);

	/**
	@brief		��ȡ���ַ����

	@return		���ַ����
	*/ 
	U32         GetLineChangeDis();

	/**
	@brief		��ȡ���ַ����

	@return		���ַ����
	*/ 
	U32         GetColChangeDis();

	/**
	@brief		��ȡ����ַ�����(��õ�����Ϊ����ߵ������,����ߵ������һ�����ֻ�ܻ���1213����)

	@return		����ַ�����
	*/ 
	U32         GetMaxStringNum();
	
	/**
	@brief		����alpha�ο�ֵ

	@return		��
	*/ 
	void        SetAlphaRef(U8 AlphaRef);

	/**
	@brief		��ȡalpha�ο�ֵ

	@return		alpha�ο�ֵ
	*/ 
	U8          GetAlphaRef();

	/**
	@brief		��ȡһ���ַ��Ŀ��

	@return		���
	*/ 
	U32         getStrWidth(char* strText);

	/**
	@brief		��ȡһ������ַ��Ŀ��

	@return		���
	*/ 
	U32         getStrWidthOutline(char* strText);

	/**
	@brief		��ȡһ���ַ��Ŀ��

	@return		���
	*/ 
	U32         getStrWidthEx(char* strText,S32* count, S32 WD);

	/**
	@brief		��ȡһ���ַ��Ŀ��

	@return		���
	*/ 
	U32         getStrNWidth(char* strText, S32 pos);

	/**
	@brief		��ȡ�ַ��ĸ߶�

	@return		�߶�
	*/ 
	U32         getHeight();

	/**
	@brief		��ȡ����ַ��ĸ߶�

	@return		����ַ��ĸ߶�
	*/ 
	U32         getHeightOutline();

	/**
	@brief		���Ӽ���

	@return		��
	*/ 
	void        AddRef();

	/**
	@brief		���ټ���

	@return		true��˼�Ǳ�ɾ��
	*/ 
	bool		ReduceRef();

	/**
	@brief		��ȡ����

	@return		��
	*/ 
	U32         GetRef();

	/**
	@brief		��ȡ����

	@return		����
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
	@brief		��ʼ������

	@return		��
	*/  
	void        InitSys(char* FontName, U32 FontHeight, S32 offsetx, S32 offsety, bool aliased, bool bItalic, bool bBold);

	/**
	@brief		��ȡ���ʵ��ڴ�ռ�

	@return		��
	*/  
	void        GetBestPosCache(U32 *x, U32 *y);

	/**
	@brief		�ڴ�ռ��Ƿ����

	@return		�Ƿ����
	*/  
	bool        IsExistCache(char *ch, FontEXCharInfo* size);

	/**
	@brief		�����ڴ�ռ�

	@return		��
	*/ 
	void        SetCache(char *strText, U32 x, U32 y, FontEXCharInfo* size);

	/**
	@brief		����ַ�

	@return		�Ƿ�ɹ�
	*/ 
	bool        FillCharacter(Point2I pos, ColorI dwColor, bool border, ColorI bordercolor, FontEXCharInfo* size, bool transition);

	/**
	@brief		��ʼ������ṹ

	@return		��
	*/ 
	void        InitFont2DVertex(F32 x, F32 y, ColorF color, F32 u, F32 v);

private:
    friend class CommonFontManager;

	/**
	@brief		��������

	@return		��
	*/ 
	void        SetName(string name);

	GFXTexHandle                                                                 m_pTexture;                                        //����
	static const   U32                                                           m_dwTexWidth;                                      //������
	static const   U32                                                           m_dwTexHeight;                                     //����߶�
	static HBITMAP                                                               m_hBitmap;
	static HDC		                                                             m_hDC;
	U32                                                                          m_dwFontPointWidth;                                //����������
	U32                                                                          m_dwFontPointHeight;                               //��������߶�
	U32                                                                          m_dwFontMaxLines;                                  //����һ���������µ�����ַ�
	U32                                                                          m_dwFontMaxStringLength;                           //����һ���������µ�����ַ�
    F32                                                                          m_fFontMaxStringLengthInv;
	HFONT	                                                                     m_hFont;                                           //windows����
	U32                                                                          m_dwLineDis;                                       //���г���
	static GFXVertexPWCT*                                                        m_pVertex;                                         //����ṹָ��
	U32                                                                          m_dwFontVertexNum;                                 //��ǰ��Ⱦ��������
	F32                                                                          m_fInvMaxLines;                                    //����һ���������µ�����ַ�����
	F32                                                                          m_fInvMaxStringLength;                             //����һ���������µ�����ַ�����
	U32                                                                          m_dwRenderTriangleNum;                             //��ǰ��Ⱦ������������
	static U32                                                                   m_dwNumVerts;                                      //��󶥵�����
	U32                                                                          m_dwLineChangeDis;                                 //���ַ����
	U32                                                                          m_dwColChangeDis;                                  //���ַ����
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
	//�豸��ʧʱ����
	//
	static void releaseStateBlock();

	//
	//�豸����ʱ����
	//
	static void resetStateBlock();
};

class CommonFontManager
{
public:
	CommonFontManager();
	~CommonFontManager();

	/**
	@brief		ɾ����������

	@return		��
	*/ 
	static      void                                                         DeleteAll();

	/**
	@brief		��ȡ����

	@return		����ṹָ��
	*/ 
	static      CommonFontEX*												 GetFont(char* FontName, U32 FontHeight,bool UseGRAY8_BITMAP = true, S32 offsetx = 0, S32 offsety = 0, bool aliased = true, bool bItalic = false, bool bBold = false);

	/**
	@brief		��ӡ��������

	@return		��
	*/ 
	static      void                                                         PrintAllFont();

protected:
	/**
	@brief		����

	@return		��
	*/ 
	static      void                                                         Destory();

private:
    friend class CommonFontEX;

	/**
	@brief		ɾ������

	@return		�Ƿ�ɹ�
	*/ 
	static      bool                                                         DeleteFont(string code);

	static stdext::hash_map<string ,CommonFontEX*>                           m_hashmapCommonFontManager;      
};

#endif