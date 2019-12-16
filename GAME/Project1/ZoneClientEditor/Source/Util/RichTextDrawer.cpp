#include "RichTextDrawer.h"
#include "gfx/gfxFontRenderBatcher.h"
#include "core/unicode.h"
#include "Common/PlayerStruct.h"
#include "base/bitStream.h"
#include "Base64.h"
#include "Gameplay/item/ItemShortcut.h"
#include "UI/dGuiTextShortcut.h"

CRichTextDrawer::FontMap CRichTextDrawer::mFontMap;

void CRichTextDrawer::setContent( const char* content )
{
	mDocument.clear();

	CTinyXParser parser( content );

	parser.parseContent( mDocument );

	mDocument.refresh();

	_loadStyles();

	relayout();
}

void CRichTextDrawer::addContent( const char* content )
{
	CTinyXParser parser( content );

	parser.parseContent( mDocument, false );

	_loadStyles();

	relayout();
}

// 重新布局  (当宽度变化或内容变化时需要改变布局)
void CRichTextDrawer::relayout()
{
	// 布局规则1,当有样式出现时,只使用样式内容	
	for each( Line* pLine in mLineList )
	{
		delete pLine;
	}

	mLineList.clear();

	mMouseControlId = NULL;

	_removeControls();

	Line* pLine = new Line( getWidth(), mDefaultLineHeight );

	mLineList.push_back( pLine );

	for each( CTinyXElement* pElement in mDocument.getRoot()->getChildren() )
	{
		_addElementToLine( pLine, pElement );	
	}
}

int CRichTextDrawer::getWidth()
{
	return mWidth;
}

void CRichTextDrawer::setWidth( int nWidth )
{
	mWidth = nWidth;
}

void CRichTextDrawer::_addElementToLine( Line*& pLine, CTinyXElement* pElement )
{	
	Style style = getDefaultStyle();

	// 如果是可换行字符串
	if( strcmp( pElement->getTagName(), "t" ) == 0 )
	{	
		char* pStyleId = (char*)pElement->getAttribute( "s" );

		// 如果有样式列表
		if( pStyleId )
		{
			if( mStyleMap.find( pStyleId ) != mStyleMap.end() )
				style = mStyleMap[pStyleId];
		}
		else
		{
			_loadStyle( pElement, style );
		}

		if( pElement->getChildren().size() > 0 )
		{   
			Style oldStyle = getDefaultStyle();
			setDefaultStyle( style );

			for each( CTinyXElement* p in pElement->getChildren() )
			{

				_addElementToLine( pLine, p );
			}

			setDefaultStyle( oldStyle );
		}
		else
		{			
			char *strBuf = new char[STRLEN];
			wchar_t* strBufU = new wchar_t[STRLEN];
			wchar_t* strBufDataU = new wchar_t[STRLEN];
			memset( strBufU, 0, sizeof( wchar_t ) * STRLEN );
			memset( strBuf, 0, STRLEN );
			int nStrWdith = 0;
			char* pText = (char*)pElement->getContent();
			convertUTF8toUTF16( pText, strBufU, STRLEN );
			wchar_t* pTextU = strBufU;
			FontDesc fontDesc;
			strcpy_s( fontDesc.FontId, sizeof( fontDesc.FontId ), style.fontName );
			fontDesc.FontSize = style.fontSize;
			fontDesc.IsBitmap = style.fontIsBitmap;
			CommonFontEX* font = _getFont( fontDesc );
			bool isOver = false;
			int strLen = 0;
			Span* span = new Span();
			span->pFont = font;

			if( pText )
			{
				// 如果是居中对齐或右对齐
				if( style.halign == 1 || style.halign == 2 )
				{
					span->mStyle = style;					
					memset( span->mText, 0, TEXTLEN );

					dStrncpy( span->mText, sizeof(span->mText), pText, TEXTLEN-1 );
					nStrWdith = font->getStrWidth( span->mText );
					span->mStrWidth = nStrWdith;

					if( pLine->mWidth - pLine->mCurrentWidth < nStrWdith ) 
					{
						pLine = new Line( mWidth, mDefaultLineHeight );
						mLineList.push_back( pLine );
					}

					span->mWidth = pLine->mWidth - pLine->mCurrentWidth;

					if( pLine->mHeight < font->getHeight() + 3)
						pLine->mHeight = font->getHeight() + 3;

					pLine->mEntityList.push_back( span );

					pLine = new Line( mWidth, mDefaultLineHeight );
					mLineList.push_back( pLine );
				}
				else
				{
					while( !isOver )
					{		
						strLen =  wcslen( pTextU );//strlen( pText ); 

						int i;

						for( i = 1; i <= strLen && i <= TEXTLEN-1; i++ )
						{
							wcsncpy_s( strBufDataU, STRLEN, pTextU, i );

							//*( strBuf + i ) = 0;
							convertUTF16toUTF8( strBufDataU, strBuf, STRLEN );
							nStrWdith = font->getStrWidth( strBuf );
							if( nStrWdith > pLine->mWidth - pLine->mCurrentWidth )
							{
								i--;
								wcsncpy_s( strBufDataU, STRLEN, pTextU, i );
								//*	*( strBuf + i - 1 ) = 0;*/
								convertUTF16toUTF8( strBufDataU, strBuf, STRLEN );
								nStrWdith = font->getStrWidth( strBuf );
								pTextU += i;

								dStrncpy( span->mText, sizeof(span->mText), strBuf, TEXTLEN-1 );
								span->mStyle = style;
								span->mWidth = nStrWdith;

								pLine->mEntityList.push_back( span );

								if( span->pFont->getHeight() > pLine->mHeight )
									pLine->mHeight = span->pFont->getHeight();

								pLine->mCurrentWidth += nStrWdith;
								pLine = new Line( mWidth, mDefaultLineHeight );
								mLineList.push_back( pLine );
								span = new Span();
								span->pFont = font;
								break;
							}
						}

						if( i == strLen + 1 )
						{
							dStrncpy( span->mText, sizeof(span->mText), strBuf, TEXTLEN-1 );
							span->mStyle = style;
							span->mWidth = nStrWdith;
							pLine->mEntityList.push_back( span );

							pLine->mCurrentWidth += nStrWdith;
							span->pFont = font;
							if( span->pFont->getHeight()  + 3 > pLine->mHeight )
								pLine->mHeight = span->pFont->getHeight() + 3;

							isOver = true;	
						}
					}
				}
			}

			delete[] strBuf;
			delete[] strBufU;
			delete[] strBufDataU;
		}

	}
	else if( strcmp( pElement->getTagName(), "s" ) == 0 )
	{
		// 忽略样式声明 
		return;
	} 
	else if( strcmp( pElement->getTagName(), "hc" ) == 0 )
	{
		pLine->hAlign = 1;
	}
	else if( strcmp( pElement->getTagName(), "i" ) == 0 ) // 图片
	{
		Image* image = new Image();

		char* pPath  = (char*)pElement->getAttribute( "s" );
		char* width = (char*)pElement->getAttribute("pw");
		char* height = (char*)pElement->getAttribute("ph");

		if( pPath )
		{
			dStrncpy( image->path, sizeof(image->path), pPath, 511 );
			image->tex.set( pPath, &GFXDefaultGUIProfile );
			image->mWidth = image->tex.getWidth();
			image->mHeight = image->tex.getHeight();

			if( width )
				image->mWidth = atoi( width );

			if( height )
				image->mHeight = atoi( height );

			if( pLine->mWidth - pLine->mCurrentWidth < image->mWidth )
			{
				pLine = new Line( mWidth,mDefaultLineHeight );
				mLineList.push_back( pLine );
			}

			pLine->mEntityList.push_back( image );
			pLine->mCurrentWidth += image->mWidth;
			if( pLine->mHeight < image->mHeight )
				pLine->mHeight = image->mHeight;
		}
	}
	else if( strcmp( pElement->getTagName(), "c" ) == 0 ) // 控件
	{
		const char* cf = pElement->getAttribute( "cf" );
		const char* cid = pElement->getAttribute( "cid" );
		const char* cmd = pElement->getAttribute( "cmd" );

		char pid[20] = {0};
		char x[20] = {0};
		char y[20] = {0};

		dSprintf(x, sizeof(x), "%d", pLine->mCurrentWidth );
		int nH = 0;
		for each( Line* pL in mLineList )
		{
			nH += pL->mHeight;
		}

		nH -= pLine->mHeight;
		dSprintf(y, sizeof(y), "%d", nH );

		if( mParentControl )
			dSprintf( pid, sizeof(pid), "%d", mParentControl->getId() );
		else
			dSprintf( pid, sizeof(pid), "0" );

		if( cf && cmd && cid )
		{
			char strBuf[TEXTLEN];
			dSprintf(strBuf, sizeof(strBuf), "%s(\"%s\",\"%s\",\"%s\",\"%s\",%s);", cf, cid, pid, x, y, cmd);
			//GuiControl* pControl = dynamic_cast<GuiControl*>(Sim::findObject( cid ));
			//if(pControl)
			//{				
			//	ControlList::iterator it = find(mControlList.begin(), mControlList.end(), pControl);
			//	if(it != mControlList.end())
			//		mControlList.erase(it);
			//	_deleteOneEntity(pControl);
			//	pControl->deleteObject();
			//}

			Con::evaluatef( strBuf );
			int nWidth = 0;
			int nHeight = 0;

			GuiControl* pControl = dynamic_cast<GuiControl*>(Sim::findObject( cid ));
			if(!pControl)
				return;
			if( pControl )
			{
				if(!pControl->isAwake())
					pControl->awaken();

				nWidth = pControl->getWidth();
				nHeight = pControl->getHeight();	

				mControlList.push_back( pControl->getId() );
			}

			if( pLine->mWidth - pLine->mCurrentWidth < nWidth )
			{
				pLine = new Line( mWidth, mDefaultLineHeight );
				mLineList.push_back( pLine );

				int n1 = 0;
				for each( Line* pL in mLineList )
				{
					n1 += pL->mHeight;
				}
				n1 -= pLine->mHeight;

				if( pControl )
					pControl->setPosition( Point2I( 0, n1 ) );
			}

			pLine->mCurrentWidth += nWidth;
			Control* pEntity = new Control();
			pEntity->mCtrlID = pControl->getId();
			pEntity->mWidth = nWidth;
			pEntity->mHeight = nHeight; 
			pLine->mEntityList.push_back( pEntity );

			if( nHeight > pLine->mHeight )
				pLine->mHeight = nHeight;
		}
	}
	else if( strcmp( pElement->getTagName(), "e" ) == 0 ) // 空块
	{
		char* pStyleId = (char*)pElement->getAttribute( "s" );

		// 如果有样式列表
		if( pStyleId )
		{
			if( mStyleMap.find( pStyleId ) != mStyleMap.end() )
				style = mStyleMap[pStyleId];
		}
		else
		{
			_loadStyle( pElement, style );
		}
	}
	else if( strcmp( pElement->getTagName(), "l" ) == 0 ) // 超链接
	{
		char* pItemData = (char*)pElement->getAttribute( "i" );
		char* pType = (char*)pElement->getAttribute( "t" );


		if( pItemData && pType )
		{
			// 物品 
			if( strcmp( pType, "item" ) == 0 || strcmp( pType, "itemid" ) == 0 )
			{
				ItemShortcut* pItem = NULL;

				if( strcmp( pType, "item") == 0 )
				{
					stItemInfo info;
					char destBuf[MAX_PACKET_SIZE];
					dBase64::decode( pItemData, (unsigned char*)destBuf, strlen( pItemData ) );

					Base::BitStream packet( destBuf, MAX_PACKET_SIZE );
					info.ReadData( &packet );

					pItem = ItemShortcut::CreateItem(info);
				}

				if( strcmp( pType, "itemid") == 0 )
				{
					pItem = ItemShortcut::CreateItem( atoi( pItemData ), 1 );
				}

				if( pItem )
				{
#pragma region 加入一个dGuiTextShortcut控件
					int nWidth = 0;
					int nHeight = 0;

					GuiTextShortcut* pControl = new GuiTextShortcut();

					if(!pControl)
						return;

					pControl->bIsUseTextEx = true;
					pControl->registerObject();

					GuiControlProfile* profile = (GuiControlProfile*)Sim::findObject( "GuiMissionTraceButtonProfile0" );

					if( profile )
						pControl->setControlProfile( profile );

					pControl->setItemInfo( pItem );

					// 计算位置
					int nH = 0;
					for each( Line* pL in mLineList )
					{
						nH += pL->mHeight;
					}
					nH -= pLine->mHeight;

					int x = pLine->mCurrentWidth;
					int y = nH;

					pControl->setPosition( Point2I( x, y ) );
					pControl->setExtent( 150, 15 );

					//// 添加到现有容器
					//if( mParentControl )
					//	;//mParentControl->addObject( pControl );

					if( pControl )
					{
						if(!pControl->isAwake())
							pControl->awaken();

						nWidth = pControl->getWidth();
						nHeight = pControl->getHeight();

						mControlList.push_back( pControl->getId() );
					}

					if( pLine->mWidth - pLine->mCurrentWidth < nWidth )
					{
						pLine = new Line( mWidth, mDefaultLineHeight );
						mLineList.push_back( pLine );

						int n1 = 0;
						for each( Line* pL in mLineList )
						{
							n1 += pL->mHeight;
						}
						n1 -= pLine->mHeight;

						if( pControl )
							pControl->setPosition( Point2I( 0, n1 ) );
					}

					pLine->mCurrentWidth += nWidth;
					Control* pEntity = new Control();
					pEntity->mCtrlID = pControl->getId();
					pEntity->mWidth = nWidth;
					pEntity->mHeight = nHeight;
					pLine->mEntityList.push_back( pEntity );

					if( nHeight > pLine->mHeight )
						pLine->mHeight = nHeight;

#pragma endregion 加入一个dGuiTextShortcut控件
				}

				// 这里不删除对象，因为对象已经传递到dGuiTextShortcut中
				/*delete pItem;*/
			}

			// 寻径
			if( strcmp( pType, "path" ) == 0 )
			{
				Point2I p;
				dBase64::decode( pItemData, (unsigned char*)&p, strlen( pItemData ) );


			}
		}
	}
	else if( strcmp( pElement->getTagName(), "b" ) == 0 ) // 换行
	{
		pLine = new Line( mWidth,mDefaultLineHeight );
		mLineList.push_back( pLine );
	}
	else if( strcmp( pElement->getTagName(), "p" ) == 0 )
	{
		_loadStyle( pElement, style );

		if( style.visible )
		{
			for each( CTinyXElement* pE in pElement->getChildren() )
			{
				_addElementToLine( pLine, pE );	
			}
		}
		else
		{
			;
		}
	}
	else
	{
		// 未知类型不处理
	}
}

void CRichTextDrawer::_addElementToBlock( Block* pBlock, CTinyXElement* pElement )
{

}

CommonFontEX* CRichTextDrawer::_findFont( FontDesc& fontDesc )
{
	FontMap::iterator it;

	char id[32];
	dSprintf( id, sizeof(id), "%s_%d_%d", fontDesc.FontId, fontDesc.FontSize, fontDesc.IsBitmap );
	it = mFontMap.find( id );
	if( it != mFontMap.end() )
		return it->second;

	return NULL;
}

void CRichTextDrawer::_insertFont( FontDesc& fontDesc, CommonFontEX* pFont )
{
	char bufId[256];
	dSprintf( bufId, sizeof(bufId), "%s_%d_%d", fontDesc.FontId, fontDesc.FontSize, fontDesc.IsBitmap );

	//mFontMap.insert( FontMap::value_type( bufId, pFont) );

	mFontMap[bufId] = pFont;
}

void CRichTextDrawer::_removeAllFont( bool bDelete )
{
	mFontMap.clear();
}

void CRichTextDrawer::_loadStyles()
{
	std::list< CTinyXElement* > elementList;

	// 取得所有的样式列表
	mDocument.GetElementsByTagName( elementList, "s" );

	for each( CTinyXElement* pElement in elementList )
	{
		Style style;

		_loadStyle( pElement, style );

		if( style.id[0] != 0 )
			mStyleMap[style.id] = style;
	}
}

void CRichTextDrawer::_loadStyle( CTinyXElement* pElement, Style& style )
{
	char* pId;
	char* pFontName;
	char* pFontSize;
	char* pUnderline;
	char* pFontColor;
	char* pOutline;
	char* pVAlign;
	char* pHAlign;
	char* pVisible;
	char* pBitmap;
	char* pBold;
	char* pShadow;

	unsigned long color;

	pId				= (char*)pElement->getAttribute( "id" );
	pFontName		= (char*)pElement->getAttribute( "f" );			// 字体名称
	pFontColor		= (char*)pElement->getAttribute( "c" );			// 字体颜色
	pFontSize		= (char*)pElement->getAttribute( "n" );			// 字体大小
	pUnderline		= (char*)pElement->getAttribute( "u" );			// 下划线 ( 0: 无下划线, 非0: 下划线颜色 )
	pOutline		= (char*)pElement->getAttribute( "o" );			// 轮廓线 ( 0: 无轮廓线, 非0: 轮廓线颜色 )
	pVAlign			= (char*)pElement->getAttribute( "v" );
	pHAlign			= (char*)pElement->getAttribute( "h" );
	pVisible		= (char*)pElement->getAttribute( "vb" );
	pBitmap			= (char*)pElement->getAttribute( "m" );			// 是否使用点阵字体
	pBold			= (char*)pElement->getAttribute( "b" );			// 是否使用粗体
	pShadow			= (char*)pElement->getAttribute( "p" );			// 是否使用阴影

	if( pId )
		dStrncpy( style.id, sizeof(style.id), pId, 31 );

	if( pVAlign )
		style.valign = strtol( pVAlign, NULL, 0 );

	if( pHAlign )
		style.halign = strtol( pHAlign, NULL, 0 );

	if( pFontName )
		dStrncpy( style.fontName, sizeof(style.fontName), pFontName, 15 );

	if( pFontColor )
	{
		color = strtoul( pFontColor, NULL, 0 );
		style.fontColor.alpha	= (U8)(color & 0xff);
		style.fontColor.blue	= (U8)(( color & 0xff00 ) >> 8);
		style.fontColor.green	= (U8)(( color & 0xff0000 ) >> 16);
		style.fontColor.red		= (U8)(( color & 0xff000000 ) >> 24);
	}

	if( pFontSize )
		style.fontSize = strtoul( pFontSize, NULL, 0 );

	if( pUnderline )
	{
		color = strtoul( pUnderline, NULL, 0 );
		style.underline.alpha	= (U8)(color & 0xff);
		style.underline.blue	= (U8)(( color & 0xff00 ) >> 8);
		style.underline.green	= (U8)(( color & 0xff0000 ) >> 16);
		style.underline.red		= (U8)(( color & 0xff000000 ) >> 24);
	}

	if( pOutline )
	{
		color = strtoul( pOutline, NULL, 0 );
		style.outline.alpha	= (U8)(color & 0xff);
		style.outline.blue	= (U8)(( color & 0xff00 ) >> 8);
		style.outline.green	= (U8)(( color & 0xff0000 ) >> 16);
		style.outline.red	= (U8)(( color & 0xff000000 ) >> 24);
	}

	if( pBold )
	{
		style.bold = (bool)strtoul( pBold, NULL, 0 );
	}

	if( pVisible )
		style.visible = (bool)strtoul( pVisible, NULL, 0 );

	if( pBitmap )
		style.fontIsBitmap = (bool)strtoul( pBitmap, NULL, 0 );

	if( pShadow )
	{
		color = strtoul( pShadow, NULL, 0 );
		style.shadow.alpha	= (U8)(color & 0xff);
		style.shadow.blue	= (U8)(( color & 0xff00 ) >> 8);
		style.shadow.green	= (U8)(( color & 0xff0000 ) >> 16);
		style.shadow.red	= (U8)(( color & 0xff000000 ) >> 24);
	}
}

CommonFontEX* CRichTextDrawer::_getFont( FontDesc& fontDesc )
{
	CommonFontEX* pFont = (CommonFontEX*)CommonFontManager::GetFont( fontDesc.FontId, fontDesc.FontSize, !fontDesc.IsBitmap );
	//CommonFontEX* pFont = _findFont( fontDesc );
	//if( !pFont )
	//{
	//	pFont = new CommonFontEX();
	//	pFont->Create( (char*)fontDesc.FontId, fontDesc.FontSize, !fontDesc.IsBitmap );
	//	pFont->SetColChangeDis( 2 );
	//	pFont->SetLineChangeDis( 3 );
	//	//pFont = GFont::create( fontName, fontSize, Con::getVariable("$GUI::fontCacheDirectory") );
	//	//if( pFont == NULL )
	//	//	GFont::create( "宋体", fontSize, Con::getVariable("$GUI::fontCacheDirectory") );
	//	_insertFont( fontDesc, pFont );
	//}

	return pFont;
}

bool CRichTextDrawer::onWake()
{
	LineList::iterator it;
	EntityList::iterator ite;
	for each( Line* pLine in mLineList )
	{
		for each( Entity* pEntity in pLine->mEntityList )
		{
			pEntity->onWake();
		}
	}
	return true;
}

void CRichTextDrawer::onSleep()
{
	LineList::iterator it;
	EntityList::iterator ite;
	for each( Line* pLine in mLineList )
	{
		for each( Entity* pEntity in pLine->mEntityList )
		{
			pEntity->onSleep();
		}
	}
}

void CRichTextDrawer::onPreRender()
{
	LineList::iterator it;
	EntityList::iterator ite;
	for each( Line* pLine in mLineList )
	{
		for each( Entity* pEntity in pLine->mEntityList )
		{
			pEntity->onPreRender();
		}
	}
}

void CRichTextDrawer::render( Point2I offset, RectI updateRect )
{
	LineList::iterator it; 
	EntityList::iterator ite;

	RectI clipRect = GFX->getClipRect();

	Point2I clipPos = GFX->getClipPos();
	/*clipRect.point.x += clipPos.x;
	clipRect.point.y += clipPos.y;*/
	//clipRect.inset( clipPos.x, clipPos.y );

	mCurPosition = clipPos + offset;

	RectI controlRect;

	Point2I p = offset;
	for each( Line* pLine in mLineList )
	{
		p.y += pLine->mHeight;	
		p.x = offset.x;
		if( pLine->hAlign == 1 )
		{
			int nL = 0;
			for each( Entity* pEntity in pLine->mEntityList )
				nL += pEntity->mWidth;

			p.x += ( mWidth - nL ) / 2;  // 行居中方式显示
		}

		for each( Entity* pEntity in pLine->mEntityList )
		{
			controlRect.set( p.x, p.y - pLine->mHeight, pEntity->mWidth, pLine->mHeight );

			if( controlRect.intersect( clipRect ) )
			{ 
				// 增加强制的Clip区域
				GFX->setClipRect( controlRect );

				pEntity->onRender( p, pLine->mHeight, updateRect );
			}

			p.x += pEntity->mWidth;
		}
	}

	GFX->setClipRect( clipRect );
}

CRichTextDrawer::~CRichTextDrawer()
{
	// the script system have already deleted the all objects
	//_removeControls();

	_deleteAllEntitys();
}

CRichTextDrawer::Helper g_Helper;

CRichTextDrawer::CRichTextDrawer()
{
	mDefaultLineHeight = 20;
	mParentControl = NULL;
	mControlOffset.x = 0;
	mControlOffset.y = 0;
	mCurPosition.x  = 0;
	mCurPosition.y = 0;
	mMouseControlId = NULL;
}

void CRichTextDrawer::setDefaultStyle( Style style )
{
	mDefaultStyle = style;
}

CRichTextDrawer::Style CRichTextDrawer::getDefaultStyle()
{
	return mDefaultStyle;
}

int CRichTextDrawer::getHeight()
{
	int n = 0;

	for each( Line* pLine in mLineList )
	{
		n += pLine->mHeight;
	}

	return n;
}

void CRichTextDrawer::setDefaultLineHeight( int nHeight )
{
	mDefaultLineHeight = nHeight;
}

int CRichTextDrawer::getRealLineWidth()
{
	int nWidth  = 0;

	for each( Line* pLine in mLineList )
	{
		int nW = 0;
		for each( Entity* pEntity in pLine->mEntityList )
		{
			nW += pEntity->mWidth;
		}

		if( nW > nWidth )
			nWidth = nW;
	}

	return nWidth;
}

CTinyXDocument* CRichTextDrawer::getDocument()
{
	return &mDocument;
}

void CRichTextDrawer::setParentControl( GuiControl* pParent )
{
	mParentControl = pParent;
}

void CRichTextDrawer::setControlOffset( const Point2I& point )
{
	mControlOffset = point;
}

void CRichTextDrawer::onMouseUp( const GuiEvent &event )
{
	/*
	GuiControl* pHit;

	for each( GuiControl* pControl in mControlList )
	{
	pHit = NULL;

	if( pControl->pointInControl( event.mousePoint - mControlOffset ) )
	pHit = pControl;

	if( pHit )
	pHit->onMouseUp( event );
	}*/
	GuiControl* mMouseControl = (GuiControl*)Sim::findObject( mMouseControlId );

	if( mMouseControl )
		mMouseControl->onMouseUp( event );


}

void CRichTextDrawer::onMouseDown( const GuiEvent &event )
{

	/*
	GuiControl* pHit;

	for each( GuiControl* pControl in mControlList )
	{
	pHit = NULL;

	if( pControl->pointInControl( event.mousePoint - mCurPosition ) )
	pHit = pControl;
	}

	if( !mMouseControl && pHit )
	mMouseControl = pHit;*/

	GuiControl* mMouseControl = (GuiControl*)Sim::findObject( mMouseControlId );

	if( mMouseControl )
		mMouseControl->onMouseDown( event );

}

void CRichTextDrawer::onMouseMove( const GuiEvent &event )
{
	GuiControl* pHit;
	bool bFound = false;
	Point2I hitPos =  event.mousePoint - mCurPosition;

	for each( int objectId in mControlList )
	{
		GuiControl* pControl = (GuiControl*)Sim::findObject( objectId );

		if( !pControl )
			continue;

		pHit = NULL;		
		if( pControl->pointInControl( hitPos) )
		{
			hitPos -= pControl->getPosition();
			pHit = pControl->findHitControl(hitPos);
		}

		if( pHit )
		{
			bFound = true;
			if( pHit->getId() != mMouseControlId )
			{
				GuiControl* mMouseControl = (GuiControl*)Sim::findObject( mMouseControlId );
				if( mMouseControl )
					mMouseControl->onMouseLeave( event );

				pHit->onMouseEnter( event );
				mMouseControlId = pHit->getId();
				break;
			}
			else
			{
				if( pHit != this->mParentControl )
					pHit->onMouseMove( event );
				break;
			}
		}
	}

	if( !bFound )
	{
		GuiControl* mMouseControl = (GuiControl*)Sim::findObject( mMouseControlId );
		if( mMouseControl )
		{
			mMouseControl->onMouseLeave( event );
			mMouseControlId = 0;
		}
	}
}

void CRichTextDrawer::onMouseLeave( const GuiEvent &event )
{
	GuiControl* mMouseControl = (GuiControl*)Sim::findObject( mMouseControlId );

	if( mMouseControl )
	{
		mMouseControl->onMouseLeave( event );
		mMouseControlId = 0;
	}
}

void CRichTextDrawer::onMouseDragged( const GuiEvent &event )
{
	GuiControl* mMouseControl = (GuiControl*)Sim::findObject( mMouseControlId );

	if( mMouseControl )
		mMouseControl->onMouseUp(event);
}

void CRichTextDrawer::_removeControls()
{
	for each( int objectId in mControlList )
	{
		SimObject* pObject = Sim::findObject( objectId );
		if( pObject )
			pObject->deleteObject();
	}

	mControlList.clear();

	mMouseControlId = NULL;
}

void CRichTextDrawer::_deleteAllEntitys()
{
	for each( Line* pLine in mLineList )
	{
		delete pLine;
	}

	mLineList.clear();
}

void CRichTextDrawer::_deleteOneEntity(GuiControl* mCtrl)
{
	for each( Line* pLine in mLineList )
	{
		EntityList::iterator itA = pLine->mEntityList.begin();
		for(; itA != pLine->mEntityList.end();)
		{
			Entity* pEntity = *itA;
			Control* pCtrl = dynamic_cast<Control*>(pEntity);
			if(pCtrl && (GuiControl*)Sim::findObject( pCtrl->mCtrlID ) == mCtrl)
			{
				delete pEntity;
				pLine->mEntityList.erase(itA++);
			}
			else
				++itA;
		}
	}
}

void CRichTextDrawer::cleanup()
{
	CRichTextDrawer::FontMap::iterator it;
	for( it = CRichTextDrawer::mFontMap.begin(); it != CRichTextDrawer::mFontMap.end(); it++ )
	{
		delete it->second;
	}
}

CRichTextDrawer::Style::Style()
{
	memset( this, 0, sizeof( Style ) );

	visible = true;

	fontColor.alpha = 255;
	fontColor.red = 255;
	fontColor.green = 255;
	fontColor.blue = 255;

	fontIsBitmap = true;
}

CRichTextDrawer::Style::Style( const Style& style )
{
	_copy(style);
}

CRichTextDrawer::Style& CRichTextDrawer::Style::operator=( const Style& style )
{
	_copy(style);

	return *this;
}

void CRichTextDrawer::Style::_copy( const Style &style )
{
	dStrcpy( id, sizeof(id), style.id );
	dStrcpy( fontName, sizeof(fontName), style.fontName );
	fontColor = style.fontColor;
	fontSize = style.fontSize;
	underline = style.underline;
	outline = style.outline;
	valign = style.valign;
	halign = style.halign;
	visible = style.visible;
	fontIsBitmap = style.fontIsBitmap;
	bold = style.bold;
	shadow = style.shadow;
}

CRichTextDrawer::Line::Line( int nWidth, int nHeight )
{
	mCurrentWidth = 0;
	mWidth = nWidth;
	mHeight = nHeight;
	vAlign = 0;
	hAlign = 0;
}

CRichTextDrawer::Span::Span()
{
	mText[0] = 0;
	pFont = NULL;
	mStrWidth = 0;
}

CRichTextDrawer::Span::Span( const Span& span )
{
	_copy(span);
}

void CRichTextDrawer::Span::_copy( const Span &span )
{
	dStrcpy( mText, sizeof(mText), span.mText );
	pFont = span.pFont;
	mStrWidth = span.mStrWidth;
}

CRichTextDrawer::Span& CRichTextDrawer::Span::operator=( const Span& span )
{
	_copy( span );
	return *this;
}

void CRichTextDrawer::Span::onRender( Point2I offset, int nHeight, RectI updateRect )
{
	bool isBold = mStyle.bold;

	if( mStyle.underline.alpha != 0 )
	{
		//GFX->getDrawUtil()->setBitmapModulation( mStyle.outline );
		GFX->getDrawUtil()->drawLine( offset.x, offset.y - 1 , offset.x + mWidth, offset.y - 1, mStyle.underline );
	}

	if( mStyle.valign == 0 ) // 底对齐
		offset.y -= pFont->getHeight() + 2;
	else if( mStyle.valign == 1 ) // 居中对齐
		offset.y -= pFont->getHeight() + ( nHeight - pFont->getHeight() ) / 2;
	else
		offset.y -= nHeight; // 顶对齐

	if( mStyle.halign == 1 ) // 居中对齐
		offset.x += ( mWidth - mStrWidth ) / 2;
	else if( mStyle.halign == 2 ) // 右对齐
		offset.x += mWidth - mStrWidth;

	ColorI textColor = mStyle.fontColor;
	ColorI outlineColor = mStyle.outline;

	if( GFX->getDrawUtil()->isAlphaLocked() )
	{
		textColor.alpha = GFX->getDrawUtil()->getLockedAlpha();
		outlineColor.alpha = GFX->getDrawUtil()->getLockedAlpha();
	}

	if( mStyle.outline !=  ColorI(0,0,0) )
	{
		//GFX->getDrawUtil()->setBitmapModulation( mStyle.outline );
		//GFX->getDrawUtil()->drawTextBorder( pFont, offset, mText );
		this->pFont->BeginRender();
		this->pFont->SetText( Point2I( offset.x - 1, offset.y - 1 ), textColor, mText, true, outlineColor );
		this->pFont->Render();
	}
	else
	{
		this->pFont->BeginRender();

		// 绘制阴影
		if( mStyle.shadow != ColorI( 0, 0, 0 ) )
		{
			this->pFont->SetText( Point2I( offset.x + 1, offset.y + 1 ), mStyle.shadow, mText );
			this->pFont->Render();
		}

		this->pFont->SetText( Point2I( offset.x, offset.y ), textColor, mText );
		this->pFont->Render();
	}

	// 绘制粗体
	if( isBold )
	{
		this->pFont->SetText( Point2I( offset.x + 1, offset.y ), textColor, mText );
		this->pFont->Render();
	}


	//GFX->getDrawUtil()->setBitmapModulation( mStyle.fontColor );
	//GFX->getDrawUtil()->drawText( pFont, offset, mText );
}

CRichTextDrawer::Font::Font()
{
	fontId[0] = 0;
	mFont = NULL;
}

CRichTextDrawer::Font::Font( const Font& font )
{
	_copy(font);
}

CRichTextDrawer::Font& CRichTextDrawer::Font::operator=( const Font& font )
{
	_copy( font );
	return *this;
}

void CRichTextDrawer::Font::_copy( const Font &font )
{
	dStrcpy( fontId, sizeof(fontId), font.fontId );
	mFont = font.mFont;
}


CRichTextDrawer::Image::Image()
{
	path[0] = 0;
}

CRichTextDrawer::Image::Image( const Image& image )
{
	_copy(image);
}

void CRichTextDrawer::Image::_copy( const Image &image )
{
	dStrcpy( path, sizeof(path), image.path );

	tex = image.tex;
}

CRichTextDrawer::Image& CRichTextDrawer::Image::operator=( const Image& image )
{
	_copy(image);
	return *this;
}

void CRichTextDrawer::Image::onRender( Point2I offset, int nHeight, RectI updateRect /*= 0 */ )
{
	if( tex.isNull() )
		return ;

	GFX->getDrawUtil()->clearBitmapModulation();
	offset.y -= mHeight;
	RectI rect( offset.x, offset.y, mWidth, mHeight );
	GFX->getDrawUtil()->drawBitmapStretch( tex, rect );
}

CRichTextDrawer::Image::~Image()
{
	tex.free();
}

CRichTextDrawer::Block::~Block()
{
	for each( Entity* e in mEntityList )
	{
		delete e;
	}
}

CRichTextDrawer::Block::Block()
{
	mHeight = 20;
}

void  CRichTextDrawer::Entity::onPreRender()
{
	return;
}

void CRichTextDrawer::Entity::onRender( Point2I offset, int nHeight, RectI updateRect )
{
	return ;
	// here we do nothing;
}

void CRichTextDrawer::Control::onWake()
{
	GuiControl* mCtrl = (GuiControl*)Sim::findObject(mCtrlID);

	if( !mCtrl || mCtrl->isAwake())
		return;
	mCtrl->awaken();
}

void CRichTextDrawer::Control::onSleep()
{
	GuiControl* mCtrl = (GuiControl*)Sim::findObject(mCtrlID);

	if( !mCtrl || !mCtrl->isAwake())
		return ;
	mCtrl->sleep();
}

void CRichTextDrawer::Control::onPreRender()
{
	GuiControl* mCtrl = (GuiControl*)Sim::findObject(mCtrlID);

	if( !mCtrl )
		return ;
	mCtrl->preRender();
}

void CRichTextDrawer::Control::onRender( Point2I offset, int nHeight, RectI updateRect /*= 0 */ )
{
	GuiControl* mCtrl = (GuiControl*)Sim::findObject(mCtrlID);

	if( !mCtrl )
		return ;

	offset.y = offset.y - nHeight + ( nHeight - mCtrl->getExtent().y );
	//RectI rect = mCtrl->getBounds();
	//rect.point = offset;
	if( mCtrl->isVisible() )
		mCtrl->onRender( offset, updateRect );
}

CRichTextDrawer::Helper::~Helper()
{

}