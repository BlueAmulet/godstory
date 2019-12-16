#pragma once

#include "TinyX/TinyXParser.h"
#include "TinyX/TinyXDocument.h"

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

class CommonFontEX;

#include <list>

class GuiControl;

#include "platform/MemoryManagerInclude.h"

class CRichTextDrawer : public CVirtualAllocBase
{
public:
	static const U32 STRLEN = 2048;
	static const U32 TEXTLEN = 2048;

	struct strHashComp
	{
		bool operator() ( const char* str1, const char* str2 ) const
		{
			return strcmp( str1, str2 ) < 0;
		}
	};

	enum 
	{
		VALIGN_TOP = 0,
		VALIGN_CENTER,
		VALIGN_BOTTOM,
	};

	enum 
	{
		HALIGN_TOP = 0,
		HALIGN_CENTER,
		HALIGN_BOTTOM,
	};

	struct Font
	{
		char fontId[32];				// 字体名_字号
        CommonFontEX* mFont;
		Font();
		Font( const Font& font );

		void _copy( const Font &font );
		Font& operator = ( const Font& font );
	};

	struct FontDesc
	{
		char FontId[32];
		int FontSize;
		bool IsBitmap;

		FontDesc()
		{
			memset( this, 0, sizeof( FontDesc )); 
		}
	};

	//typedef stdext::hash_map< std::string, CommonFontEX*, stdext::hash_compare< const char*, strHashComp > > FontMap;
	typedef stdext::hash_map< std::string, CommonFontEX* > FontMap;

	// 样式
	struct Style
	{
		char	fontName[16];
		int		fontSize;
		ColorI	underline;		// 下划线
		ColorI	outline;		// 描边
		ColorI	fontColor;
		bool	bold;			// 加粗
		char	id[32];
		int		valign;			// 垂直对齐方式
		int		halign;			// 水平对齐方式
		bool	visible;
		bool	fontIsBitmap;

		ColorI  shadow;			// 阴影

		Style();
		Style( const Style& style );

		void _copy( const Style &style );
		Style& operator = ( const Style& style );
	};

	struct Entity
	{
		Style	mStyle;
		int		mWidth;

		Entity() { mWidth = 0; } 

		virtual ~Entity() { ; }

		virtual void onRender( Point2I offset, int nHeight, RectI updateRect );
		virtual void onPreRender();
		virtual void onWake() { return;}
		virtual void onSleep() { return;}
	} ;

	typedef std::list< Entity* > EntityList;

	// 可折行
	struct Span : public Entity
	{
		char mText[TEXTLEN];
		CommonFontEX* pFont;
		int mStrWidth;

		Span();
		Span( const Span& span );

		Span& operator = ( const Span& span );

		void _copy( const Span &span );
		
		void onRender( Point2I offset, int nHeight, RectI updateRect );
	};

	// 不可折行
	struct Block : public Entity
	{
		EntityList mEntityList;

		virtual ~Block();
		Block();

		int mHeight;
	};

	struct Control : Block
	{
		void onRender( Point2I offset, int nHeight, RectI updateRect );
		void onPreRender();
		void onWake();
		void onSleep();

		int mCtrlID;
	};

	struct Image : public Block
	{
		char path[512];
		GFXTexHandle tex;

		Image();
		Image( const Image& image );
		virtual ~Image();

		void _copy( const Image &image );
		Image& operator = ( const Image& image );
		
		void onRender( Point2I offset, int nHeight, RectI updateRect );
	};

	struct Line : public Block
	{
		int mCurrentWidth;
		Line( int nWidth, int nHeight );
		int vAlign;
		int hAlign;
	};

	struct Helper
	{

		~Helper();
		
	};

	typedef std::list< Line* > LineList;
	typedef stdext::hash_map< const char*, Style, stdext::hash_compare< const char*, strHashComp > > StyleMap;

	typedef std::list< int > ControlList;

public:
	CRichTextDrawer();
	virtual ~CRichTextDrawer();

	static void cleanup();

	void _deleteAllEntitys();
	void _deleteOneEntity(GuiControl*);
	void setContent( const char* content );
	void addContent( const char* content );

	void _removeControls();
	int getRealLineWidth();

	CTinyXDocument* getDocument();
	void relayout();

	void setDefaultStyle( Style style );
	Style getDefaultStyle();

	void setWidth( int nWidth );
	int getWidth();

	bool onWake();
	void onSleep();
	void render( Point2I offset, RectI updateRect );
	void onPreRender();
	int getHeight();

	void setControlOffset( const Point2I& point );
	void setParentControl( GuiControl* pParent );
	void setDefaultLineHeight( int nHeight );

	void onMouseUp(const GuiEvent &event);
	void onMouseDown(const GuiEvent &event);
	void onMouseMove(const GuiEvent &event);
	void onMouseLeave( const GuiEvent &event);
	void onMouseDragged( const GuiEvent	&event );

private:
	void _addElementToLine( Line*& pLine, CTinyXElement* pElement );
	void _addElementToBlock( Block* pBlock, CTinyXElement* pElement );

	static CommonFontEX* _getFont(  FontDesc& fontDesc );
	static CommonFontEX* _findFont( FontDesc& fontDesc );
	static void _insertFont( FontDesc& fontDesc, CommonFontEX* pFont );

	void _removeAllFont( bool bDelete = true );

	void _loadStyles();

	void _loadStyle( CTinyXElement* pElement, Style& style );

public:
	CTinyXDocument mDocument;
private:

	Line* mCurrentLine;

	int			mMouseControlId;
	LineList	mLineList;
	ControlList mControlList;
	Point2I		mCurPosition;

	GuiControl*	mParentControl;
	Point2I		mControlOffset;

	Style		mDefaultStyle;
	StyleMap	mStyleMap;
	static FontMap		mFontMap;
	int			mWidth;
	int			mDefaultLineHeight;
};