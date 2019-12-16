//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gfx/gfxFontRenderBatcher.h"
#include "gfx/gFont.h"
#include "materials/shaderData.h"
#include "gfx/gfxDrawUtil.h"
#include "lightingSystem/synapseGaming/sgFormatManager.h"
#include "core/unicode.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
#include "util/powerStat.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK

GFXStateBlock* FontRenderBatcher::mSetSB = NULL;

//Vector<FontRenderBatcher::SheetMarker *> FontRenderBatcher::mSheets(64);

FontRenderBatcher::FontRenderBatcher()
{
   mSheets.reserve( 64 );
}

void FontRenderBatcher::renderBorder( F32 rot, const Point2F &offset )
{
	Point2F p;

	p.x = offset.x;
	p.y = offset.y + 1;
	_render( rot, p );

	p.x = offset.x;
	p.y = offset.y - 1;
	_render( rot, p);

	p.x = offset.x - 1;
	p.y = offset.y;
	_render( rot, p);

	p.x = offset.x + 1;
	p.y = offset.y;
	_render( rot, p);
}

void FontRenderBatcher::render(F32 rot, const Point2F &offset )
{
	_render( rot, offset);
}


void FontRenderBatcher::_render( F32 rot, const Point2F &offset )
{
	if( mLength == 0 )
		return;

	//static ShaderData *mBBShader = NULL;
	//if(!mBBShader)
	//{
	//	if ( !Sim::findObject( "TextDraw", mBBShader ) )
	//	{
	//		Con::warnf("TextDraw - failed to locate TextDraw shader FontRenderBatcher!");
	//		return;
	//	}
	//}

	//GFX->setShader(mBBShader->getShader());

	GFX->setupGenericShaders( GFXDevice::GSAddColorTexture );
#ifdef STATEBLOCK
	AssertFatal(mSetSB, "FontRenderBatcher::render -- mSetSB cannot be NULL.");
	mSetSB->apply();
#else
	GFX->setLightingEnable( false );
	GFX->setCullMode( GFXCullNone );
	GFX->setAlphaBlendEnable( true );

	GFX->setTextureStageAlphaOp( 0, GFXTOPModulate );
	GFX->setTextureStageAlphaOp( 1, GFXTOPDisable );
	GFX->setTextureStageAlphaArg1( 0, GFXTATexture );
	GFX->setTextureStageAlphaArg2( 0, GFXTADiffuse );

	GFX->setTextureStageMagFilter( 0, GFXTextureFilterPoint );
	GFX->setTextureStageMinFilter( 0, GFXTextureFilterPoint );
	GFX->setTextureStageAddressModeU( 0, GFXAddressClamp );
	GFX->setTextureStageAddressModeV( 0, GFXAddressClamp );

	// This is an add operation because in D3D, when a texture of format D3DFMT_A8
	// is used, the RGB channels are all set to 0.  Therefore a modulate would 
	// result in the text always being black.  This may not be the case in OpenGL
	// so it may have to change.  -bramage
	GFX->setTextureStageColorOp( 0, GFXTOPAdd );
	GFX->setTextureStageColorOp( 1, GFXTOPDisable );
#endif

	GFX->getDrawUtil()->setBlendStatus(GFX);

	MatrixF rotMatrix;

	bool doRotation = rot != 0.f;
	if(doRotation)
		rotMatrix.set( EulerF( 0.0, 0.0, mDegToRad( rot ) ) );

	// Write verts out.
	U32 currentPt = 0;
	GFXVertexBufferHandle<GFXVertexPCT> verts(GFX, mLength * 6, GFXBufferTypeVolatile);
	GFXVertexColor &col = GFX->getDrawUtil()->getBitmapModulation();

	verts.lock();

	for( S32 i = 0; i < mSheets.size(); i++ )
	{
		// Do some early outs...
		if(!mSheets[i])
			continue;

		if(!mSheets[i]->numChars)
			continue;

		mSheets[i]->startVertex = currentPt;
		const GFXTextureObject *tex = mFont->getTextureHandle(i);

		for( S32 j = 0; j < mSheets[i]->numChars; j++ )
		{
			// Get some general info to proceed with...
			const CharMarker &m = mSheets[i]->charIndex[j];
			const PlatformFont::CharInfo &ci = mFont->getCharInfo( m.c );

			// Where are we drawing it?
			F32 drawY = offset.y + mFont->getBaseline() - ci.yOrigin * TEXT_MAG;
			F32 drawX = offset.x + m.x + ci.xOrigin;

			// Figure some values.
			const F32 texWidth = (F32)tex->getWidth();
			const F32 texHeight = (F32)tex->getHeight();
			const F32 texLeft   = (F32)(ci.xOffset)             / texWidth;
			const F32 texRight  = (F32)(ci.xOffset + ci.width)  / texWidth;
			const F32 texTop    = (F32)(ci.yOffset)             / texHeight;
			const F32 texBottom = (F32)(ci.yOffset + ci.height) / texHeight;

			const F32 fillConventionOffset = GFX->getFillConventionOffset();
			const F32 screenLeft   = drawX - fillConventionOffset;
			const F32 screenRight  = drawX - fillConventionOffset + ci.width * TEXT_MAG;
			const F32 screenTop    = drawY - fillConventionOffset;
			const F32 screenBottom = drawY - fillConventionOffset + ci.height * TEXT_MAG;

			// Build our vertices. We NEVER read back from the buffer, that's
			// incredibly slow, so for rotation do it into tmp. This code is
			// ugly as sin.
			Point3F tmp;

			
			tmp.set( screenLeft, screenTop, 0.f );
			if(doRotation)
				rotMatrix.mulP( tmp, &verts[currentPt].point);
			else
				verts[currentPt].point = tmp;
			verts[currentPt].color = col;
			verts[currentPt].texCoord.set( texLeft, texTop );
			currentPt++;

			tmp.set( screenLeft, screenBottom, 0.f );
			if(doRotation)
				rotMatrix.mulP( tmp, &verts[currentPt].point);
			else
				verts[currentPt].point = tmp;
			verts[currentPt].color = col;
			verts[currentPt].texCoord.set( texLeft, texBottom );
			currentPt++;

			tmp.set( screenRight, screenBottom, 0.f );
			if(doRotation)
				rotMatrix.mulP( tmp, &verts[currentPt].point);
			else
				verts[currentPt].point = tmp;
			verts[currentPt].color = col;
			verts[currentPt].texCoord.set( texRight, texBottom );
			currentPt++;

			tmp.set( screenRight, screenBottom, 0.f );
			if(doRotation)
				rotMatrix.mulP( tmp, &verts[currentPt].point);
			else
				verts[currentPt].point = tmp;
			verts[currentPt].color = col;
			verts[currentPt].texCoord.set( texRight, texBottom );
			currentPt++;

			tmp.set( screenRight, screenTop, 0.f );
			if(doRotation)
				rotMatrix.mulP( tmp, &verts[currentPt].point);
			else
				verts[currentPt].point = tmp;
			verts[currentPt].color = col;
			verts[currentPt].texCoord.set( texRight, texTop );
			currentPt++;

			tmp.set( screenLeft, screenTop, 0.f );
			if(doRotation)
				rotMatrix.mulP( tmp, &verts[currentPt].point);
			else
				verts[currentPt].point = tmp;
			verts[currentPt].color = col;
			verts[currentPt].texCoord.set( texLeft, texTop );
			currentPt++;
		}
	}

	verts->unlock();

	AssertFatal(currentPt <= mLength * 6, "FontRenderBatcher::render - too many verts for length of string!");

	GFX->setVertexBuffer(verts);

	// Now do an optimal render!
	for( S32 i = 0; i < mSheets.size(); i++ )
	{
		if(!mSheets[i])
			continue;

		if(!mSheets[i]->numChars )
			continue;

		GFX->setTexture( 0, mFont->getTextureHandle(i) );
		GFX->drawPrimitive(GFXTriangleList, mSheets[i]->startVertex, mSheets[i]->numChars * 2);
	}

	GFX->getDrawUtil()->clearBlendStatus(GFX);
}

void FontRenderBatcher::queueChar( UTF16 c, S32 &currentX, GFXVertexColor &currentColor )
{
	const PlatformFont::CharInfo &ci = mFont->getCharInfo( c );
	U32 sidx = ci.bitmapIndex;

	if( ci.width != 0 && ci.height != 0 )
	{
		SheetMarker &sm = getSheetMarker(sidx);

		CharMarker &m = sm.charIndex[sm.numChars];
		sm.numChars++;

		m.c = c;
		m.x = (F32)currentX;
		m.color = currentColor;
	}

	currentX += ci.xIncrement;
}

FontRenderBatcher::SheetMarker & FontRenderBatcher::getSheetMarker( U32 sheetID )
{
   // Allocate if it doesn't exist...
   if(mSheets.size() <= sheetID || !mSheets[sheetID])
   {
      if(sheetID >= mSheets.size())
         mSheets.setSize(sheetID+1);

      mSheets[sheetID] = (SheetMarker *)mAllocator.alloc( 
         sizeof( SheetMarker) + mLength * sizeof( CharMarker ) );
      constructInPlace<SheetMarker>(mSheets[sheetID]);
      mSheets[sheetID]->numChars = 0;
   }

   return *mSheets[sheetID];
}

void FontRenderBatcher::init( GFont *font, U32 n )
{
   // Null everything and reset our count.
   dMemset(mSheets.address(), 0, mSheets.memSize());
   mSheets.clear();

   mFont = font;
   mLength = n;
}

void FontRenderBatcher::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setSamplerState(0, GFXSAMPMagFilter, GFXTextureFilterPoint);
	GFX->setSamplerState(0, GFXSAMPMinFilter, GFXTextureFilterPoint);
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressClamp);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPAdd);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	GFX->setTextureStageState(0, GFXTSSAlphaOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSAlphaOp, GFXTOPDisable);
	GFX->setTextureStageState(0, GFXTSSAlphaArg1, GFXTATexture);
	GFX->setTextureStageState(0, GFXTSSAlphaArg2, GFXTADiffuse);
	GFX->endStateBlock(mSetSB);
}


void FontRenderBatcher::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}
}

void FontRenderBatcher::initsb()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		resetStateBlock();
	}
}

void FontRenderBatcher::shutdown()
{
	SAFE_DELETE(mSetSB);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
//---------------------------------------------------------------------------
//静态成员
//状态块
//---------------------------------------------------------------------------
GFXStateBlock* CommonFontEX::mSetSB = NULL;
GFXStateBlock* CommonFontEX::mClearSB = NULL;

const U32 CommonFontEX::m_dwTexWidth = 512;
const U32 CommonFontEX::m_dwTexHeight = 512;
HBITMAP CommonFontEX::m_hBitmap = 0;
HDC CommonFontEX::m_hDC = 0;
GFXVertexPWCT* CommonFontEX::m_pVertex = NULL;
U32 CommonFontEX::m_dwNumVerts = 8000;
U32 CommonFontEX::m_dwIndexNum = 12000;
U32 CommonFontEX::m_dwMaxStringNum = 2000;
GFXPrimitiveBufferHandle CommonFontEX::primBuff;
GFXVertexBufferHandle<GFXVertexPWCT> CommonFontEX::mVertBuff;
const U32 MaxFontHeight = 80;

CommonFontEX::CommonFontEX()
{
	m_pTexture = NULL;
	m_dwFontPointWidth = m_dwFontPointHeight = m_dwFontMaxLines = m_dwFontMaxStringLength = 0;
	m_pBufUseCount = NULL;
	m_hFont = 0;
	m_dwLineDis = 0;
	m_dwFontVertexNum = 0;
	m_fInvMaxLines = 0.0f;
	m_fInvMaxStringLength = 0.0f;
	m_dwRenderTriangleNum = 0;
	m_dwLineChangeDis = m_dwColChangeDis = 0;
	m_uAlphaRef = 0;
	m_pTextInfo = NULL;
    m_bUseGRAY8_BITMAP = true; 
	m_uFillNum = 0;
	m_uRefCount = 1;
}

CommonFontEX::~CommonFontEX()
{
	if( m_hFont )
		DeleteObject( m_hFont );
	SAFE_DELETE_ARRAY(m_pBufUseCount);
	m_pTexture = NULL;
	SAFE_DELETE_ARRAY(m_pTextInfo);
	m_hashmapFontEXCharInfo.clear();
}

void CommonFontEX::AddRef()
{
	m_uRefCount += 1;
}

bool CommonFontEX::ReduceRef()
{
	m_uRefCount -= 1;
	if(m_uRefCount <= 0)
	{
		return CommonFontManager::DeleteFont(m_strName);
	}

	return false;
}

U32         CommonFontEX::GetRef()
{
    return m_uRefCount; 
}

bool CommonFontEX::Create(char* FontName, U32 FontHeight,bool UseGRAY8_BITMAP, S32 offsetx, S32 offsety ,bool aliased, bool bItalic, bool bBold)
{
	m_pTexture = GFXTexHandle(m_dwTexWidth, m_dwTexHeight, GFXFormatA8, &GFXDefaultStaticDiffuseProfile);

    //记录下纹理创建
    CStat::Instance()->SetDesc((unsigned int)m_pTexture.getPointer(),"字体");

	m_dwLineDis = m_dwTexWidth;
	InitSys(FontName, FontHeight, offsetx, offsety, aliased, bItalic, bBold);
	m_bUseGRAY8_BITMAP = UseGRAY8_BITMAP;
	return true;
}

U32 CommonFontEX::GetTextureWidth()
{
	return m_dwTexWidth;
}

U32 CommonFontEX::GetTextureHeight()
{
	return m_dwTexHeight;
}

#ifndef NTJ_SERVER

void CommonFontEX::init()
{
	m_hDC = CreateCompatibleDC( NULL );
    m_hBitmap = CreateCompatibleBitmap(m_hDC, MaxFontHeight, MaxFontHeight);
	
	if( m_hBitmap == NULL )
		return;

	SelectObject( m_hDC, m_hBitmap );

	SetTextColor( m_hDC, RGB(255,255,255) );
	SetBkColor( m_hDC, 0x00000000 );

	m_pVertex = new GFXVertexPWCT[m_dwNumVerts];

	U16* pIndex = new U16[m_dwIndexNum];

	U16 j1 = 0;
	U16 j2 = 0;
	for (size_t i = 0; i < m_dwMaxStringNum; i++)
	{
		pIndex[j2] = j1;
		pIndex[j2 + 1] = j1 + 1;
		pIndex[j2 + 2] = j1 + 2;
		pIndex[j2 + 3] = j1 + 2;
		pIndex[j2 + 4] = j1 + 1;
		pIndex[j2 + 5] = j1 + 3;
		j1 += 4;
		j2 += 6;
	}

	U16 *ibIndices;
	primBuff.set( GFX, m_dwIndexNum, 0, GFXBufferTypeStatic );
	primBuff.lock( &ibIndices );
	dMemcpy( ibIndices, pIndex, m_dwIndexNum * sizeof(U16) );
	primBuff.unlock();

	mVertBuff.set( GFX, m_dwNumVerts, GFXBufferTypeDynamic );

	SAFE_DELETE_ARRAY(pIndex);
}

void CommonFontEX::shutdown()
{
	SAFE_DELETE_ARRAY(m_pVertex);
	if( m_hBitmap )
		DeleteObject( m_hBitmap );
	if( m_hDC )
		DeleteDC( m_hDC );
	mVertBuff = NULL;
	primBuff = NULL;
}

#endif

void CommonFontEX::InitSys(char* FontName, U32 FontHeight, S32 offsetx, S32 offsety, bool aliased, bool bItalic, bool bBold)
{
#ifndef NTJ_SERVER
	static bool bInit = false;
	if(!bInit)
	{
		init();
		bInit = true;
	}
#endif

	U32 dwBold   = bBold   ? FW_EXTRABOLD : FW_NORMAL;
	U32 dwItalic = bItalic ? TRUE : FALSE;
	UTF16 n[512];
	convertUTF8toUTF16((UTF8 *)FontName, n, sizeof(n));

	if(aliased)
	{
		m_hFont = CreateFontW(FontHeight,0,0,0,dwBold,dwItalic,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,0,ANTIALIASED_QUALITY,VARIABLE_PITCH,n);
	}
	else
	{
		m_hFont = CreateFontW(FontHeight,0,0,0,dwBold,dwItalic,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,0,NONANTIALIASED_QUALITY,VARIABLE_PITCH,n);
	}

	if( m_hFont == NULL )
		return;
	
	SelectObject( m_hDC, m_hFont );
	TEXTMETRIC TextMetric;
	GetTextMetrics(m_hDC, &TextMetric);
	m_iBaseLine = TextMetric.tmAscent;

	SIZE CharExtent;
	GetTextExtentPoint32W(m_hDC, L"溺", 1, &CharExtent);
	m_dwFontPointWidth = CharExtent.cx + offsetx;
	m_dwFontPointHeight = CharExtent.cy + offsety;
	m_dwFontMaxLines = m_dwTexHeight/m_dwFontPointHeight;
	m_dwFontMaxStringLength	= m_dwTexWidth/m_dwFontPointWidth;
	m_fInvMaxLines = (F32)m_dwFontPointWidth/(F32)m_dwTexWidth;
	m_fInvMaxStringLength = (F32)m_dwFontPointHeight/(F32)m_dwTexHeight;
	m_fFontMaxStringLengthInv = 1.0f/(F32)m_dwFontMaxStringLength;

	U32 num2 = m_dwFontMaxLines * m_dwFontMaxStringLength;

	if(num2 > m_dwMaxStringNum)
		num2 = m_dwMaxStringNum;
	m_uMaxStringNum = num2;
	m_pTextInfo = new FontEXCharInfo[m_uMaxStringNum];
	
	m_pBufUseCount = new U16[m_uMaxStringNum];
}

bool CommonFontEX::SetText(Point2I pos,const ColorI &dwColor, char* strText, bool border, ColorI bordercolor, bool transition)
{
	if(!strcmp( strText, "" ))
		return true;
	if( !m_pTexture )
		return false;

	if(m_dwFontVertexNum >= m_dwNumVerts)
		return false;

	if(border)
	{
		pos.x += 1;
        pos.y += 1;
	}

	U32 XPos = 0;
	Point2I temp_pos;
	temp_pos = pos;

	size_t len = strlen(strText);
	U32 temp_dw2;
	if(border)
		temp_dw2 = m_dwFontPointHeight + m_dwLineChangeDis + 2;
	else
		temp_dw2 = m_dwFontPointHeight + m_dwLineChangeDis;

	U32 x, y;
	FontEXCharInfo temp_size;

	for (size_t i = 0; i < len; i++)
	{
		char buf = '\0';
		char temp_buf[2];
		buf = strText[i];

		if(buf == '\n')
		{
			XPos = 0;
			temp_pos.x = pos.x;
			temp_pos.y += temp_dw2;
			continue;
		}
		else if(buf == ' ')
		{
			S32 temp_s = m_dwFontPointWidth/2;
			if(XPos + temp_s> m_dwLineDis)
			{
				XPos = 0;
				temp_pos.x = pos.x;
				temp_pos.y += temp_dw2;
			}
			XPos += temp_s;
			temp_pos.x += temp_s;
			continue;
		} 
		else 
		{
			if(buf < 0)
			{
				ZeroMemory(temp_buf, 2);
				strncpy(temp_buf, &strText[i], 2);
				++i;
				if( IsExistCache(temp_buf, &temp_size) )
				{
				}
				else
				{
					GetBestPosCache(&x,&y);
					SetCache(temp_buf,x,y,&temp_size);
				}
			}
			else
			{
				if( IsExistCache(&buf, &temp_size) )
				{
				}
				else
				{
					GetBestPosCache(&x,&y);
					SetCache(&buf,x,y,&temp_size);
				}
			}
			if(XPos + temp_size.xIncrement + m_dwColChangeDis > m_dwLineDis)
			{
				XPos = 0;
				temp_pos.x = pos.x;
				temp_pos.y += temp_dw2;
			}
		}

		if(!border)
			m_dwRenderTriangleNum += 2;
		else
		{
			if(transition)
				m_dwRenderTriangleNum += 10;
			else
				m_dwRenderTriangleNum += 18;
		}
		FillCharacter( temp_pos, dwColor, border, bordercolor, &temp_size, transition);
		XPos += temp_size.xIncrement + m_dwColChangeDis;
		temp_pos.x += temp_size.xIncrement + m_dwColChangeDis;
	}
	return true;
}

U32  CommonFontEX::getStrWidth(char* strText)
{
	U32 Width = 0;

	size_t len = strlen(strText);

	U32 x, y;
	FontEXCharInfo temp_size;

	for (size_t i = 0; i < len; i++)
	{
		char buf = '\0';
		char temp_buf[2];
		buf = strText[i];

		if(buf == ' ')
		{
			Width += m_dwFontPointWidth/2;
			continue;
		} 
		else 
		{
			if(buf < 0)
			{
				ZeroMemory(temp_buf, 2);
				strncpy(temp_buf, &strText[i], 2);
				++i;
				if( IsExistCache(temp_buf, &temp_size) )
				{
				}
				else
				{
					GetBestPosCache(&x,&y);
					SetCache(temp_buf,x,y,&temp_size);
				}
			}
			else
			{
				if( IsExistCache(&buf, &temp_size) )
				{
				}
				else
				{
					GetBestPosCache(&x,&y);
					SetCache(&buf,x,y,&temp_size);
				}
			}
		}
		Width += (temp_size.xIncrement + m_dwColChangeDis);
	}
	return Width;
}

U32  CommonFontEX::getStrWidthOutline(char* strText)
{
	return (getStrWidth(strText) + 2);
}

U32  CommonFontEX::getStrWidthEx(char* strText, S32* count, S32 WD)
{
	U32 Width = 0;

	size_t len = strlen(strText);

	U32 x, y;
	FontEXCharInfo temp_size;

	for (size_t i = 0; i < len; i++)
	{
		char buf = '\0';
		char temp_buf[2];
		buf = strText[i];

		if(buf == ' ')
		{
			Width += m_dwFontPointWidth/2;
			if (Width > WD)
			{
				return Width;
			}
			*count += 1;
			continue;
		} 
		else 
		{
			if(buf < 0)
			{
				ZeroMemory(temp_buf, 2);
				strncpy(temp_buf, &strText[i], 2);
				++i;
				if( IsExistCache(temp_buf, &temp_size) )
				{
				}
				else
				{
					GetBestPosCache(&x,&y);
					SetCache(temp_buf,x,y,&temp_size);
				}
			}
			else
			{
				if( IsExistCache(&buf, &temp_size) )
				{
				}
				else
				{
					GetBestPosCache(&x,&y);
					SetCache(&buf,x,y,&temp_size);
				}
			}
		}
		Width += (temp_size.xIncrement + m_dwColChangeDis);
		if (Width > WD)
		{
			return Width;
		}
		*count += 1;
	}
	return Width;
}

U32         CommonFontEX::getStrNWidth(char* strText, S32 pos)
{
	U32 Width = 0;

	U32 x, y;
	FontEXCharInfo temp_size;

	for (size_t i = 0; i < pos; i++)
	{
		char buf = '\0';
		char temp_buf[2];
		buf = strText[i];

		if(buf == ' ')
		{
			Width += m_dwFontPointWidth/2;
			continue;
		} 
		else 
		{
			if(buf < 0)
			{
				ZeroMemory(temp_buf, 2);
				strncpy(temp_buf, &strText[i], 2);
				i++;
				pos++; //  <Edit>:[thinking] 中文长度加1
				if( IsExistCache(temp_buf, &temp_size) )
				{
				}
				else
				{
					GetBestPosCache(&x,&y);
					SetCache(temp_buf,x,y,&temp_size);
				}
			}
			else
			{
				if( IsExistCache(&buf, &temp_size) )
				{
				}
				else
				{
					GetBestPosCache(&x,&y);
					SetCache(&buf,x,y,&temp_size);
				}
			}
		}
		Width += (temp_size.xIncrement + m_dwColChangeDis);
	}
	return Width;
}

void CommonFontEX::GetBestPosCache(U32 *x,U32 *y)
{
	if(m_uFillNum < m_uMaxStringNum)
	{
		*x = m_uFillNum%m_dwFontMaxStringLength;
		*y = ((float)m_uFillNum)*m_fFontMaxStringLengthInv;
		m_uFillNum += 1;
	}
	else
	{
		for (U32 i = 0; i < m_uMaxStringNum; i++)
		{
			if (m_pBufUseCount[i] <= 0)
			{
				*x = i%m_dwFontMaxStringLength;
				*y = ((float)i)*m_fFontMaxStringLengthInv;
			}
		}
	}
}

U32 CommonFontEX::getBreakPos(const UTF16 *str16, U32 slen, U32 width, bool breakOnWhitespace)
{
	if(slen==0)
		return  0;

	UTF8 strText[1024];
	ZeroMemory(strText,sizeof(strText));
	WideCharToMultiByte( 936, WC_NO_BEST_FIT_CHARS, str16, slen, strText, sizeof(strText), NULL, false);
	size_t len = dStrlen(strText);

	U32 x, y;
	FontEXCharInfo temp_size;

	U32 ret = 0;
	U32 lastws = 0;
	U32 charCount = 0;

	for (size_t i = 0; i < len; i++)
	{
		char buf = '\0';
		char temp_buf[2];
		buf = strText[i];
		//  <Edit>:[thinking] 增加对\t处理
		if(buf == '\t')
			buf = ' ';

		if(buf == ' ')
		{
			lastws = ret+1;
			continue;
		} 
		else 
		{
			if(buf < 0)
			{
				ZeroMemory(temp_buf, 2);
				strncpy(temp_buf, &strText[i], 2);
				++i;

				if( IsExistCache(temp_buf, &temp_size) )
				{
				}
				else
				{
					GetBestPosCache(&x,&y);
					SetCache(temp_buf,x,y,&temp_size);
				}
			}
			else
			{
				if( IsExistCache(&buf, &temp_size) )
				{
				}
				else
				{
					GetBestPosCache(&x,&y);
					SetCache(&buf,x,y,&temp_size);
				}
			}
		}
		if(temp_size.xIncrement > width)
		{
			if(lastws && breakOnWhitespace)
				return lastws;
			return ret;
		}
		width -= (temp_size.xIncrement + m_dwColChangeDis);

		ret++;
	}
	return ret;
	
}


U32 CommonFontEX::getBreakPos(const UTF8 *str8, U32 slen, U32 width, bool breakOnWhitespace)
{
	if(slen==0)
		return  0;

	U32 x, y;
	FontEXCharInfo temp_size;

	U32 ret = 0;
	U32 lastws = 0;
	U32 charCount = 0;

	for (size_t i = 0; i < slen; i++)
	{
		char buf = '\0';
		char temp_buf[2];
		buf = str8[i];
		//  <Edit>:[thinking] 增加对\t处理
		if(buf == '\t')
			buf = ' ';

		if(buf == ' ')
		{
			lastws = ret+1;
			continue;
		} 
		else 
		{
			if(buf < 0)
			{
				ZeroMemory(temp_buf, 2);
				strncpy(temp_buf, &str8[i], 2);
				i++;
				ret++;
				if( IsExistCache(temp_buf, &temp_size) )
				{
				}
				else
				{
					GetBestPosCache(&x,&y);
					SetCache(temp_buf,x,y,&temp_size);
				}
			}
			else
			{
				if( IsExistCache(&buf, &temp_size) )
				{
				}
				else
				{
					GetBestPosCache(&x,&y);
					SetCache(&buf,x,y,&temp_size);
				}
			}
		}
		if(temp_size.xIncrement > width)
		{
			if(lastws && breakOnWhitespace)
				return lastws;
			return ret;
		}
		width -= (temp_size.xIncrement + m_dwColChangeDis);

		ret++;
	}
	return ret;
}

bool CommonFontEX::IsExistCache(char *ch, FontEXCharInfo* size)
{
	wchar_t wch;
	MultiByteToWideChar( CP_ACP, 0, ch, -1, &wch, 1 );
	hash_map<wchar_t, FontEXCharInfo*>::iterator it = m_hashmapFontEXCharInfo.find(wch);
	if (it != m_hashmapFontEXCharInfo.end())
	{
		memcpy(size, m_hashmapFontEXCharInfo[wch], sizeof(FontEXCharInfo));
        m_pBufUseCount[size->x_index + size->y_index * m_dwFontMaxStringLength] += 1;
		return true;
	}
	return false;
}

void CommonFontEX::SetCache(char *strText,U32 x,U32 y,FontEXCharInfo* textsize)
{
	static U8 scratchPad[65536];
	U32 count;
	count = y*m_dwFontMaxStringLength+x;
	m_pBufUseCount[count] = 1;

	SelectObject( m_hDC, m_hFont );

	RectI rcLock;
	rcLock.point.x = x * m_dwFontPointWidth;
	rcLock.point.y = y * m_dwFontPointHeight;
	rcLock.extent.x = rcLock.point.x + m_dwFontPointWidth;
	rcLock.extent.y = rcLock.point.y + m_dwFontPointHeight;

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

	UTF16 *pTmp = convertUTF8toUTF16(strText);
	if(!m_bUseGRAY8_BITMAP)
	{		
		if(GetGlyphOutlineW(m_hDC, *pTmp,GGO_BITMAP,&metrics,sizeof(scratchPad),scratchPad,&matrix) != GDI_ERROR)
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

			m_pTextInfo[count].width = metrics.gmBlackBoxX;
			m_pTextInfo[count].height = metrics.gmBlackBoxY;
			m_pTextInfo[count].xOrigin = metrics.gmptGlyphOrigin.x;
			m_pTextInfo[count].yOrigin = metrics.gmptGlyphOrigin.y;
			m_pTextInfo[count].xIncrement = metrics.gmCellIncX;
			m_pTextInfo[count].x_index = x;
			m_pTextInfo[count].y_index = y;
			wchar_t wch;
			MultiByteToWideChar( CP_ACP, 0, strText, -1, &wch, 1 );
			hash_map<wchar_t, FontEXCharInfo*>::iterator it;
			for (it = m_hashmapFontEXCharInfo.begin(); it != m_hashmapFontEXCharInfo.end(); it++)
			{
				if (((*it).second->x_index == x) && ((*it).second->y_index == y))
				{
					m_hashmapFontEXCharInfo.erase(it);
					break;
				}
			}
            m_hashmapFontEXCharInfo[wch] = &(m_pTextInfo[count]);
			*textsize = m_pTextInfo[count];

			rcLock.point.x = mClamp( rcLock.point.x, 0, m_dwTexWidth - 1 );
			rcLock.point.y = mClamp( rcLock.point.y, 0, m_dwTexHeight - 1 );
			rcLock.extent.x = mClamp( rcLock.extent.x, 0, m_dwTexWidth - rcLock.point.x );
			rcLock.extent.y = mClamp( rcLock.extent.y, 0, m_dwTexHeight - rcLock.point.y );
			GFXLockedRect* d3dlr = m_pTexture.lock(0, &rcLock);

			BYTE* pDstRow = (BYTE*)d3dlr->bits;
			U8* pDst16;

			for(U32 y = 0; S32(y) < metrics.gmBlackBoxY; y++)
			{
				pDst16 = (U8*)pDstRow;
				for(U32 x = 0; x < metrics.gmBlackBoxX; x++)
				{
					S32 spi = y * rowStride + x;
					*pDst16++ = scratchPad2[spi];
				}
				pDstRow += d3dlr->pitch;
			}
			m_pTexture.unlock(0);
		}
	}
	else
	{
		if(GetGlyphOutlineW(m_hDC, *pTmp, GGO_GRAY8_BITMAP,&metrics,sizeof(scratchPad),scratchPad,&matrix) != GDI_ERROR)
		{
			U32 rowStride = (metrics.gmBlackBoxX + 3) & ~3; 
			U32 size = rowStride * metrics.gmBlackBoxY;

			for(U32 j = 0; j < size && j < sizeof(scratchPad); j++)
			{
				U32 pad = U32(scratchPad[j]) << 2;
				if(pad > 255)
					pad = 255;
				scratchPad[j] = pad;
			}

			m_pTextInfo[count].width = metrics.gmBlackBoxX;
			m_pTextInfo[count].height = metrics.gmBlackBoxY;
			m_pTextInfo[count].xOrigin = metrics.gmptGlyphOrigin.x;
			m_pTextInfo[count].yOrigin = metrics.gmptGlyphOrigin.y;
			m_pTextInfo[count].xIncrement = metrics.gmCellIncX;
			m_pTextInfo[count].x_index = x;
			m_pTextInfo[count].y_index = y;
			wchar_t wch;
			MultiByteToWideChar( CP_ACP, 0, strText, -1, &wch, 1 );
			hash_map<wchar_t, FontEXCharInfo*>::iterator it;
			for (it = m_hashmapFontEXCharInfo.begin(); it != m_hashmapFontEXCharInfo.end(); it++)
			{
				if (((*it).second->x_index == x) && ((*it).second->y_index == y))
				{
					m_hashmapFontEXCharInfo.erase(it);
					break;
				}
			}
			m_hashmapFontEXCharInfo[wch] = &(m_pTextInfo[count]);
			*textsize = m_pTextInfo[count];

			rcLock.point.x = mClamp( rcLock.point.x, 0, m_dwTexWidth - 1 );
			rcLock.point.y = mClamp( rcLock.point.y, 0, m_dwTexHeight - 1 );
			rcLock.extent.x = mClamp( rcLock.extent.x, 0, m_dwTexWidth - rcLock.point.x );
			rcLock.extent.y = mClamp( rcLock.extent.y, 0, m_dwTexHeight - rcLock.point.y );
			GFXLockedRect* d3dlr = m_pTexture.lock(0, &rcLock);

			BYTE* pDstRow = (BYTE*)d3dlr->bits;
			U8* pDst16;

			for(U32 y = 0; S32(y) < metrics.gmBlackBoxY; y++)
			{
				pDst16 = (U8*)pDstRow;
				for(U32 x = 0; x < metrics.gmBlackBoxX; x++)
				{
					S32 spi = y * rowStride + x;
					*pDst16++ = scratchPad[spi];
				}
				pDstRow += d3dlr->pitch;
			}
			m_pTexture.unlock(0);
		}
	}

	if (pTmp)
		delete [] pTmp;
}

bool CommonFontEX::FillCharacter(Point2I pos, ColorI dwColor, bool border, ColorI bordercolor, FontEXCharInfo* size, bool transition)
{
	static F32 TexWidth = 1.0f/(F32)m_dwTexWidth;
	static F32 TexHeight = 1.0f/(F32)m_dwTexHeight;
	const F32 tx1 = (F32)size->x_index * m_fInvMaxLines;
	const F32 tx2 = tx1 + (F32)(size->width) * TexWidth;
	const F32 ty1 = (F32)size->y_index * m_fInvMaxStringLength;
	const F32 ty2 = ty1 + (F32)(size->height) * TexHeight;

	const F32 fillConventionOffset = GFX->getFillConventionOffset();
	const F32 screenLeft   = pos.x - fillConventionOffset + (S32)(size->xIncrement - size->width) / 2;
	const F32 screenRight  = screenLeft + size->width;
	const F32 screenTop    = pos.y - fillConventionOffset + m_iBaseLine - size->yOrigin;
	const F32 screenBottom = screenTop + size->height;

    if (border)
    {
	    INT offset = 1;
		InitFont2DVertex( screenLeft - offset, screenBottom, bordercolor, tx1, ty2 );
		m_dwFontVertexNum++;
		InitFont2DVertex( screenLeft - offset, screenTop, bordercolor, tx1, ty1 );
		m_dwFontVertexNum++;
		InitFont2DVertex( screenRight - offset, screenBottom, bordercolor, tx2, ty2 );
		m_dwFontVertexNum++;
		InitFont2DVertex( screenRight - offset, screenTop, bordercolor, tx2, ty1 );
		m_dwFontVertexNum++;	

		InitFont2DVertex( screenLeft + offset, screenBottom, bordercolor, tx1, ty2 );
		m_dwFontVertexNum++;
		InitFont2DVertex( screenLeft + offset, screenTop, bordercolor, tx1, ty1 );
		m_dwFontVertexNum++;
		InitFont2DVertex( screenRight + offset, screenBottom, bordercolor, tx2, ty2 );
		m_dwFontVertexNum++;
		InitFont2DVertex( screenRight + offset, screenTop, bordercolor, tx2, ty1 );
		m_dwFontVertexNum++;	

		InitFont2DVertex( screenLeft, screenBottom - offset, bordercolor, tx1, ty2 );
		m_dwFontVertexNum++;
		InitFont2DVertex( screenLeft, screenTop - offset, bordercolor, tx1, ty1 );
		m_dwFontVertexNum++;
		InitFont2DVertex( screenRight, screenBottom - offset, bordercolor, tx2, ty2 );
		m_dwFontVertexNum++;
		InitFont2DVertex( screenRight, screenTop - offset, bordercolor, tx2, ty1 );
		m_dwFontVertexNum++;	

		InitFont2DVertex( screenLeft, screenBottom + offset, bordercolor, tx1, ty2 );
		m_dwFontVertexNum++;
		InitFont2DVertex( screenLeft, screenTop + offset, bordercolor, tx1, ty1 );
		m_dwFontVertexNum++;
		InitFont2DVertex( screenRight, screenBottom + offset, bordercolor, tx2, ty2 );
		m_dwFontVertexNum++;
		InitFont2DVertex( screenRight, screenTop + offset, bordercolor, tx2, ty1 );
		m_dwFontVertexNum++;	

		if(!transition)
		{
			InitFont2DVertex( screenLeft - offset, screenBottom - offset, bordercolor, tx1, ty2 );
			m_dwFontVertexNum++;
			InitFont2DVertex( screenLeft - offset, screenTop - offset, bordercolor, tx1, ty1 );
			m_dwFontVertexNum++;
			InitFont2DVertex( screenRight - offset, screenBottom - offset, bordercolor, tx2, ty2 );
			m_dwFontVertexNum++;
			InitFont2DVertex( screenRight - offset, screenTop - offset, bordercolor, tx2, ty1 );
			m_dwFontVertexNum++;	

			InitFont2DVertex( screenLeft + offset, screenBottom + offset, bordercolor, tx1, ty2 );
			m_dwFontVertexNum++;
			InitFont2DVertex( screenLeft + offset, screenTop + offset, bordercolor, tx1, ty1 );
			m_dwFontVertexNum++;
			InitFont2DVertex( screenRight + offset, screenBottom + offset, bordercolor, tx2, ty2 );
			m_dwFontVertexNum++;
			InitFont2DVertex( screenRight + offset, screenTop + offset, bordercolor, tx2, ty1 );
			m_dwFontVertexNum++;	

			InitFont2DVertex( screenLeft + offset, screenBottom - offset, bordercolor, tx1, ty2 );
			m_dwFontVertexNum++;
			InitFont2DVertex( screenLeft + offset, screenTop - offset, bordercolor, tx1, ty1 );
			m_dwFontVertexNum++;
			InitFont2DVertex( screenRight + offset, screenBottom - offset, bordercolor, tx2, ty2 );
			m_dwFontVertexNum++;
			InitFont2DVertex( screenRight + offset, screenTop - offset, bordercolor, tx2, ty1 );
			m_dwFontVertexNum++;	

			InitFont2DVertex( screenLeft - offset, screenBottom + offset, bordercolor, tx1, ty2 );
			m_dwFontVertexNum++;
			InitFont2DVertex( screenLeft - offset, screenTop + offset, bordercolor, tx1, ty1 );
			m_dwFontVertexNum++;
			InitFont2DVertex( screenRight - offset, screenBottom + offset, bordercolor, tx2, ty2 );
			m_dwFontVertexNum++;
			InitFont2DVertex( screenRight - offset, screenTop + offset, bordercolor, tx2, ty1 );
			m_dwFontVertexNum++;	
		}
	}

	InitFont2DVertex( screenLeft, screenBottom, dwColor, tx1, ty2 );
	m_dwFontVertexNum++;
	InitFont2DVertex( screenLeft, screenTop, dwColor, tx1, ty1 );
	m_dwFontVertexNum++;
	InitFont2DVertex( screenRight, screenBottom, dwColor, tx2, ty2 );
	m_dwFontVertexNum++;
	InitFont2DVertex( screenRight, screenTop, dwColor, tx2, ty1 );
	m_dwFontVertexNum++;	

	return true;
}

void CommonFontEX::InitFont2DVertex(F32 x, F32 y, ColorF color, F32 u, F32 v)
{
	m_pVertex[m_dwFontVertexNum].point.x = x;		
	m_pVertex[m_dwFontVertexNum].point.y = y;		
	m_pVertex[m_dwFontVertexNum].point.z = 0.001f;	
    m_pVertex[m_dwFontVertexNum].point.w = 1.0f;
	m_pVertex[m_dwFontVertexNum].color = color;		
	m_pVertex[m_dwFontVertexNum].texCoord.x = u;		
	m_pVertex[m_dwFontVertexNum].texCoord.y = v;
}

void CommonFontEX::BeginRender()
{
	m_dwFontVertexNum = 0;
	m_dwRenderTriangleNum = 0;
	ZeroMemory(m_pBufUseCount, 2 * m_uFillNum);
}
#ifdef STATEBLOCK
void CommonFontEX::Render()
{
    if ( m_dwRenderTriangleNum == 0 )
        return;

	GFXVertexPWCT *verts = mVertBuff.lock();
	dMemcpy( verts, m_pVertex, m_dwFontVertexNum * sizeof(GFXVertexPWCT) );
	mVertBuff.unlock();   

	GFX->disableShaders();
	GFX->setTexture( 0, m_pTexture );

	AssertFatal(mSetSB, "CommonFontEX::Render -- mSetSB cannot be NULL.");
	mSetSB->apply();
	GFX->setRenderState(GFXRSAlphaRef, m_uAlphaRef);//变量直接设置
	GFX->getDrawUtil()->setBlendStatus(GFX);

	GFX->setPrimitiveBuffer( primBuff );
	GFX->setVertexBuffer( mVertBuff );

	GFX->drawIndexedPrimitive( GFXTriangleList, 0, m_dwNumVerts, 0, m_dwRenderTriangleNum );

	AssertFatal(mClearSB, "CommonFontEX::Render -- mClearSB cannot be NULL.");
	mClearSB->apply();
	GFX->setTexture( 0, NULL );
}

#else
void CommonFontEX::Render()
{
    if ( m_dwRenderTriangleNum == 0 )
        return;

	GFXVertexPWCT *verts = mVertBuff.lock();
	dMemcpy( verts, m_pVertex, m_dwFontVertexNum * sizeof(GFXVertexPWCT) );
	mVertBuff.unlock();   

	GFX->disableShaders();
	GFX->setTexture( 0, m_pTexture );

	GFX->setAlphaBlendEnable( true );
	GFX->setAlphaFunc(GFXCmpGreater);
	GFX->setAlphaRef(m_uAlphaRef);
	GFX->setAlphaTestEnable(true);
	GFX->setTextureStageAlphaOp( 0, GFXTOPModulate );
	GFX->setTextureStageAlphaOp( 1, GFXTOPDisable );
	GFX->setTextureStageAlphaArg1( 0, GFXTATexture );
	GFX->setTextureStageAlphaArg2( 0, GFXTADiffuse );
	GFX->setTextureStageColorOp( 0, GFXTOPSelectARG2 );
	GFX->setTextureStageColorArg2( 0, GFXTADiffuse );
	GFX->setTextureStageColorOp( 1, GFXTOPDisable );
	GFX->getDrawUtil()->setBlendStatus(GFX);
	GFX->setZEnable(false);
	GFX->setZWriteEnable(false);
	GFX->setTextureStageMagFilter( 0, GFXTextureFilterPoint );
	GFX->setTextureStageMinFilter( 0, GFXTextureFilterPoint );
    GFX->setTextureStageMipFilter( 0, GFXTextureFilterNone );

	GFX->setPrimitiveBuffer( primBuff );
	GFX->setVertexBuffer( mVertBuff );

	GFX->drawIndexedPrimitive( GFXTriangleList, 0, m_dwNumVerts, 0, m_dwRenderTriangleNum );

	GFX->setTextureStageAlphaOp( 0, GFXTOPModulate );
	GFX->setTextureStageColorOp( 0, GFXTOPModulate );
    GFX->setTextureStageColorArg1( 0, GFXTATexture );
	GFX->setTextureStageColorArg2( 0, GFXTADiffuse );

	//GFX->setTexture( 0, NULL );
	GFX->setAlphaRef(0);
	GFX->setZEnable(true);
	GFX->setZWriteEnable(true);
}

#endif

void CommonFontEX::SetLineDis(U32 linedis)
{
	m_dwLineDis = linedis;
}

U32 CommonFontEX::GetLineDis()
{
	return m_dwLineDis;
}

void CommonFontEX::SetLineChangeDis(U32 data)
{
	m_dwLineChangeDis = data;
}

void CommonFontEX::SetColChangeDis(U32 data)
{
	m_dwColChangeDis = data;
}

U32 CommonFontEX::GetLineChangeDis()
{
	return m_dwLineChangeDis;
}

U32 CommonFontEX::GetColChangeDis()
{
	return m_dwColChangeDis;
}

U32 CommonFontEX::GetMaxStringNum()
{
     return m_uMaxStringNum;
}

void CommonFontEX::SetAlphaRef(U8 AlphaRef)
{
	m_uAlphaRef = AlphaRef;
}

U8 CommonFontEX::GetAlphaRef()
{
	return m_uAlphaRef;
}

U32 CommonFontEX::getHeight()
{
	return m_dwFontPointHeight;
}

U32  CommonFontEX::getHeightOutline()
{
	return m_dwFontPointHeight + 2;
}

void CommonFontEX::SetName(string name)
{
	m_strName = name;
}

string      CommonFontEX::GetName()
{
	return m_strName;
}

void CommonFontEX::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	//GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	//GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSAlphaFunc, GFXCmpGreater);
	GFX->setTextureStageState(0, GFXTSSAlphaOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSAlphaOp, GFXTOPDisable);
	GFX->setTextureStageState(0, GFXTSSAlphaArg1, GFXTATexture);
	GFX->setTextureStageState(0, GFXTSSAlphaArg2, GFXTADiffuse);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPSelectARG2);
	GFX->setTextureStageState(0, GFXTSSColorArg2, GFXTOPDisable);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	GFX->setRenderState(GFXRSZEnable, false);
	GFX->endStateBlock(mSetSB);

	//mClearSB
	GFX->beginStateBlock();
	GFX->setTextureStageState(0, GFXTSSAlphaOp, GFXTOPModulate);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(0, GFXTSSColorArg1, GFXTATexture);
	GFX->setTextureStageState(0, GFXTSSColorArg2, GFXTADiffuse);
	GFX->setRenderState(GFXRSAlphaRef, 0);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->endStateBlock(mClearSB);
}


void CommonFontEX::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}

	if (mClearSB)
	{
		mClearSB->release();
	}
}

void CommonFontEX::initsb()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		mClearSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}

void CommonFontEX::shutdownsb()
{
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mClearSB);
}

stdext::hash_map<string ,CommonFontEX*> CommonFontManager::m_hashmapCommonFontManager;

CommonFontManager::CommonFontManager()
{
}

CommonFontManager::~CommonFontManager()
{
	Destory();
}

void CommonFontManager::DeleteAll()
{
	Destory();
}

void CommonFontManager::Destory()
{
	stdext::hash_map<string ,CommonFontEX*>::iterator it = m_hashmapCommonFontManager.begin();
	for (;it != m_hashmapCommonFontManager.end(); it++)
	{
		SAFE_DELETE((*it).second);
	}
	m_hashmapCommonFontManager.clear();
}

CommonFontEX* CommonFontManager::GetFont(char* FontName, U32 FontHeight,bool UseGRAY8_BITMAP, S32 offsetx, S32 offsety, bool aliased, bool bItalic, bool bBold)
{
	std::string code;
	code = FontName;
	char name[512];
	dSprintf(name, 512, "%d%d%d%d%d%d%d", FontHeight, UseGRAY8_BITMAP, offsetx, offsety, aliased, bItalic, bBold);
	string code1 = name;
	code += code1;
	stdext::hash_map<string ,CommonFontEX*>::iterator it = m_hashmapCommonFontManager.find(code);
	if (it != m_hashmapCommonFontManager.end())
	{
		(*it).second->AddRef();
		return (*it).second;
	}
	CommonFontEX* newIns = new CommonFontEX;
	m_hashmapCommonFontManager[code] = newIns;
	newIns->Create(FontName, FontHeight, UseGRAY8_BITMAP, offsetx, offsety, aliased, bItalic, bBold);
	//newIns->SetColChangeDis(2);
	newIns->SetName(code);
	return newIns;
}

bool CommonFontManager::DeleteFont(string code)
{
	stdext::hash_map<string ,CommonFontEX*>::iterator it = m_hashmapCommonFontManager.find(code);
	if (it != m_hashmapCommonFontManager.end())
	{
		SAFE_DELETE((*it).second);
		m_hashmapCommonFontManager.erase(it);
		return true;
	}
	return false;
}

void CommonFontManager::PrintAllFont()
{
	stdext::hash_map<string ,CommonFontEX*>::iterator it = m_hashmapCommonFontManager.begin();
	for (;it != m_hashmapCommonFontManager.end(); it++)
	{
		Con::printf((*it).first.c_str());
	}
	float sz = 0.25f * m_hashmapCommonFontManager.size();
	Con::printf("理论上字体占用了%0.02fM显存", sz);
}