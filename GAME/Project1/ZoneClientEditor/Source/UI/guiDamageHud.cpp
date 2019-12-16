#include <windows.h>
#include "console/consoleTypes.h"
#include "console/console.h"
#include "core/color.h"
#include "gfx/gFont.h"
#include "ui/guiDamageHud.h"
#include "Gameplay/GameplayCommon.h"


////////////////////////////////////////////////////////////
#define TICK_LEN_MS 32
#define DAMAGEHUD_MISSFILE			"gameres/gui/images/damage_miss.png"
#define DAMAGEHUD_CRITICALFILE		"gameres/gui/images/damage_critical.png"
#define DAMAGEHUD_ABSORBFILE		"gameres/gui/images/damage_absorb.png"
#define DAMAGEHUD_RESISTFILE		"gameres/gui/images/damage_resist.png"
#define DAMAGEHUD_REBOUNDFILE		"gameres/gui/images/damage_rebound.png"
#define DAMAGEHUD_REFLECTFILE		"gameres/gui/images/damage_reflect.png"
#define DAMAGEHUD_IMMUNITYFILE		"gameres/gui/images/damage_immunity.png"
#define DAMAGEHUD_GREENNUMFILE		"gameres/gui/images/damage_green.png"
#define DAMAGEHUD_REDNUMFILE		"gameres/gui/images/damage_red.png"
#define DAMAGEHUD_YELLOWNUMFILE		"gameres/gui/images/damage_yellow.png"
#define DAMAGEHUD_BLUENUMFILE		"gameres/gui/images/damage_blue.png"

GFXTexHandle	GuiDamageHud::mNumTexture[NUM_TYPE];
Vector<RectI>	GuiDamageHud::mNumBitmapArrayRects[NUM_TYPE];
GFXTexHandle	GuiDamageHud::mMissTexture;
GFXTexHandle	GuiDamageHud::mCriticalTexture;
GFXTexHandle	GuiDamageHud::mAbsorbTexture;
GFXTexHandle	GuiDamageHud::mResistTexture;
GFXTexHandle	GuiDamageHud::mReboundTexture;
GFXTexHandle	GuiDamageHud::mReflectTexture;
GFXTexHandle	GuiDamageHud::mImmunityTexture;

////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiDamageHud);

GuiDamageHud::GuiDamageHud(void)
{
	mState				= STATE_NONE;
	mFadeType			= FADE_ALL;
	mFadeInTime			= 1000;
	mFadeOutTime		= 800;
	mLifeTime			= 800;
	mMoveSpeed			= 0;
	mSpeedEnter			= 10.0f;
	mMoveSpeedNormal	= 2.0f;
	mWordPadding		= 2;

	mLastTime			= 0;
	mAddSpeedRate		= 0.f;

	mAlphaStep			= 0.005;
	mColorAlpha.set(1,1,1,1.0);
}

// ----------------------------------------------------------------------------
// 初始化纹理对象
void GuiDamageHud::Initilize()
{
	/*mNumTexture[0]		= GFXTexHandle(DAMAGEHUD_GREENNUMFILE,	&GFXDefaultGUIProfile);
	mNumTexture[1]		= GFXTexHandle(DAMAGEHUD_REDNUMFILE,	&GFXDefaultGUIProfile);
	mNumTexture[2]		= GFXTexHandle(DAMAGEHUD_YELLOWNUMFILE,	&GFXDefaultGUIProfile);
	mNumTexture[3]		= GFXTexHandle(DAMAGEHUD_BLUENUMFILE,	&GFXDefaultGUIProfile);

	mMissTexture		= GFXTexHandle(DAMAGEHUD_MISSFILE,		&GFXDefaultGUIProfile);
	mCriticalTexture	= GFXTexHandle(DAMAGEHUD_CRITICALFILE,	&GFXDefaultGUIProfile);
	mAbsorbTexture		= GFXTexHandle(DAMAGEHUD_ABSORBFILE,	&GFXDefaultGUIProfile);
	mResistTexture		= GFXTexHandle(DAMAGEHUD_RESISTFILE,	&GFXDefaultGUIProfile);
	mReboundTexture		= GFXTexHandle(DAMAGEHUD_REBOUNDFILE,	&GFXDefaultGUIProfile);
	mReflectTexture		= GFXTexHandle(DAMAGEHUD_REFLECTFILE,	&GFXDefaultGUIProfile);
	mImmunityTexture	= GFXTexHandle(DAMAGEHUD_IMMUNITYFILE,	&GFXDefaultGUIProfile);

	for( U32 i = 0; i < NUM_TYPE; ++i)
	{
		constructBitmapArray(mNumTexture[i], i);
	}*/
}

// ----------------------------------------------------------------------------
// 释放纹理对象
void GuiDamageHud::Destroy()
{
	/*for( U32 i = 0; i < NUM_TYPE; ++i)
	{
		mNumTexture[i]	= NULL;
		mNumBitmapArrayRects[i].clear();
	}

	mMissTexture		= NULL;
	mCriticalTexture	= NULL;
	mAbsorbTexture		= NULL;
	mResistTexture		= NULL;
	mReboundTexture		= NULL;
	mReflectTexture		= NULL;
	mImmunityTexture	= NULL;*/
	
}

// ----------------------------------------------------------------------------
// 设置渲染的类型或数字
void GuiDamageHud::SetDrawText(U32 ShowType, const char* Number)
{
	setVisible(true);
	mColorAlpha.set(1,1,1,0);
	mState				= STATE_ENTER;
	mLastTime			= Platform::getVirtualMilliseconds();
	mAlphaStep			= 1.0f / ((float)mFadeInTime / (float)TICK_LEN_MS + 1.0f);
	mAddSpeedRate		= 1.0f;
	mMoveSpeedNormal	= mFabs(mMoveSpeedNormal);
	mMoveSpeed			= mFabs(mSpeedEnter);

	mShowType = ShowType;
	mDamageNumName[0] = 0;
	RectI oldBounds = getBounds();
	RectI newBounds;

	if(mShowType == SHOWSTATE_MISS)
	{
		if((bool)mMissTexture)
		{	
			newBounds.extent.x = mMissTexture.getWidth();
			newBounds.extent.y = mMissTexture.getHeight();
			newBounds.point.x = oldBounds.point.x - newBounds.extent.x/2;
			newBounds.point.y = oldBounds.point.y;
		}
	}
	else if(mShowType == SHOWSTATE_CRITICAL)
	{
		if((bool)mCriticalTexture)
		{
			newBounds.extent.x = mCriticalTexture.getWidth();
			newBounds.extent.y = mCriticalTexture.getHeight();
			newBounds.point.x = oldBounds.point.x - newBounds.extent.x/2;
			newBounds.point.y = oldBounds.point.y;
		}
	}
	else if(mShowType == SHOWSTATE_ABSORB)
	{
		if((bool)mAbsorbTexture)
		{
			newBounds.extent.x = mAbsorbTexture.getWidth();
			newBounds.extent.y = mAbsorbTexture.getHeight();
			newBounds.point.x = oldBounds.point.x - newBounds.extent.x/2;
			newBounds.point.y = oldBounds.point.y;
		}
	}
	else if(mShowType == SHOWSTATE_RESIST)
	{
		if((bool)mResistTexture)
		{
			newBounds.extent.x = mResistTexture.getWidth();
			newBounds.extent.y = mResistTexture.getHeight();
			newBounds.point.x = oldBounds.point.x - newBounds.extent.x/2;
			newBounds.point.y = oldBounds.point.y;
		}
	}
	else if(mShowType == SHOWSTATE_REBOUND)
	{
		if((bool)mReboundTexture)
		{
			newBounds.extent.x = mReboundTexture.getWidth();
			newBounds.extent.y = mReboundTexture.getHeight();
			newBounds.point.x = oldBounds.point.x - newBounds.extent.x/2;
			newBounds.point.y = oldBounds.point.y;
		}
	}
	else if(mShowType == SHOWSTATE_REFLECT)
	{
		if((bool)mReflectTexture)
		{
			newBounds.extent.x = mReflectTexture.getWidth();
			newBounds.extent.y = mReflectTexture.getHeight();
			newBounds.point.x = oldBounds.point.x - newBounds.extent.x/2;
			newBounds.point.y = oldBounds.point.y;
		}
	}
	else if(mShowType == SHOWSTATE_IMMUNITY)
	{
		if((bool)mImmunityTexture)
		{
			newBounds.extent.x = mImmunityTexture.getWidth();
			newBounds.extent.y = mImmunityTexture.getHeight();
			newBounds.point.x = oldBounds.point.x - newBounds.extent.x/2;
			newBounds.point.y = oldBounds.point.y;
		}
	}
	else
	{
		U32	nDigit;
		U32 nHeight		= 12;
		U32 nWidth		= 0;
		dSprintf(mDamageNumName, NUM_LENGTH, "%s", Number);
		mTextLen	= dStrlen(mDamageNumName);		
		for(int n = 0; n < mTextLen; ++n)
		{
			nDigit = mDamageNumName[n] - '0';
			if((bool)mNumTexture[mShowType])
			{
				RectI& rect = getBitmapArrayRect(mShowType, nDigit);
				nWidth += rect.len_x() + mWordPadding;
				if(nHeight < rect.len_y())
					nHeight = rect.len_y();
			}
			else
				nWidth += 12;
		}
		newBounds.extent.x = nWidth;
		newBounds.extent.y = nHeight;
		newBounds.point.x  = oldBounds.point.x - newBounds.extent.x/2;
		newBounds.point.y  = oldBounds.point.y;
	}
	setBounds(newBounds);
}


void GuiDamageHud::ClearText()
{
	mShowType			= 0;			
	mDamageNumName[0]	= 0;
	mLastTime			= 0;
	mState				= STATE_NONE;
	setVisible(false);
}

static EnumTable::Enums gs_HudFades[] =
{
	{ GuiDamageHud::FADE_NONE,	"none"     },
	{ GuiDamageHud::FADE_IN,	"fadeIn"   },
	{ GuiDamageHud::FADE_OUT,   "fadeOut"  },
	{ GuiDamageHud::FADE_ALL,   "fadeAll"  },
};
static EnumTable s_HudFades(5, &gs_HudFades[0]);


void GuiDamageHud::initPersistFields()
{
	Parent::initPersistFields();
	addGroup("Move");		
	addField("wordPadding", TypeS32,     Offset(mWordPadding,		GuiDamageHud));
	addField("lifeTime",	TypeS32,     Offset(mLifeTime,			GuiDamageHud));
	addField("fadeInTime",	TypeS32,     Offset(mFadeInTime,		GuiDamageHud));
	addField("fadeOutTime",	TypeS32,     Offset(mFadeOutTime,		GuiDamageHud));
	addField("speedNormal",	TypeF32,     Offset(mMoveSpeedNormal,    GuiDamageHud));
	addField("speedEnter",	TypeF32,     Offset(mSpeedEnter,		GuiDamageHud));
	addField("fadeType",	TypeEnum,    Offset(mFadeType,			GuiDamageHud),1, &s_HudFades);
	endGroup("Move");
}

void GuiDamageHud::processTick() 
{
	if(mState == STATE_NONE)
		return;

	U32 dwTimer;
	RectI mBounds = getBounds();

	switch(mState)
	{
	case STATE_ENTER:
		{
			if(mColorAlpha.alpha <= 0.95f)
			{
				mColorAlpha.alpha	+= mAlphaStep;
				if(mMoveSpeed >= 0.0f)
					mMoveSpeed		-= mAddSpeedRate *(1 + (1.0f-mColorAlpha.alpha)*2.f);
				else
					mMoveSpeed		= 0.f;
				break;
			}
			mState			= STATE_STEP;
			mLastTime		= Platform::getVirtualMilliseconds();
		}

	case STATE_STEP:
		{
			if(mMoveSpeed > mMoveSpeedNormal )
				mMoveSpeed	-= mAddSpeedRate;
			else
				mMoveSpeed	+= mAddSpeedRate;

			dwTimer = Platform::getVirtualMilliseconds();
			if(dwTimer - mLastTime < mLifeTime)
				break;

			mState			= STATE_LEAVE;
			mAlphaStep		= 1.0f / ((float)mFadeOutTime / (float)TICK_LEN_MS + 1.0f);
		}

	case STATE_LEAVE:
		{
			if(mColorAlpha.alpha >= 0.05f)
			{
				mColorAlpha.alpha	-= mAlphaStep;
				if(mMoveSpeed > 0.05f)
					mMoveSpeed	-= mAddSpeedRate;
				else
					mMoveSpeed	= 0.f;
				break;
			}
			ClearText();
		}
		break;
	}

	if(mShowType <= SHOWSTATE_MISS)
	{
		mBounds.point.y	-= (S32)mMoveSpeed;
		//mBounds.point.x += (S32)mMoveSpeed;
	}
	else
	{
		mBounds.point.x	-= (S32)mMoveSpeed;
		mBounds.point.y	-= (S32)mMoveSpeed;
	}
	setBounds(mBounds);
}

void GuiDamageHud::onRender(Point2I offset, const RectI &updateRect)
{
	if(mState == STATE_NONE)
		return;

	GFX->getDrawUtil()->setBitmapModulation(mColorAlpha);

	if(mShowType == SHOWSTATE_MISS)
	{
		GFX->getDrawUtil()->drawBitmap(mMissTexture, offset);
		U32		nIndex;
		Point2I	ptDraw(offset);
		for(U32 n = 0; n < mTextLen; n++)
		{
			nIndex = mDamageNumName[n] - '0';
			if(nIndex >= 10)
			{	
				ptDraw.x += 12;
				continue;
			}
			RectI& rect = getBitmapArrayRect(mShowType, nIndex);
			GFX->getDrawUtil()->drawBitmapSR(mNumTexture[mShowType], ptDraw, rect);
			ptDraw.x += rect.len_x() + mWordPadding;
		}
	}
	else if(mShowType == SHOWSTATE_CRITICAL)
	{
		GFX->getDrawUtil()->drawBitmap(mCriticalTexture, offset);
	}
	else
	{
		U32		nIndex;
		Point2I	ptDraw(offset);
		for(U32 n = 0; n < mTextLen; n++)
		{
			nIndex = mDamageNumName[n] - '0';
			if(nIndex >= 10)
			{	
				ptDraw.x += 12;
				continue;
			}
			RectI& rect = getBitmapArrayRect(mShowType, nIndex);
			GFX->getDrawUtil()->drawBitmapSR(mNumTexture[mShowType], ptDraw, rect);
			ptDraw.x += rect.len_x() + mWordPadding;
		}
	}
}

void GuiDamageHud::interpolateTick( F32 delta ) 
{
}

void GuiDamageHud::advanceTime( F32 timeDelta )
{
}

bool GuiDamageHud::onWake()
{
	if(!Parent::onWake())
		return(false);
	return(true);
}

void GuiDamageHud::onSleep()
{
	Parent::onSleep();
}

RectI GuiDamageHud::getBitmapArrayRect(U32 ShowType, U32 i)
{
	if(ShowType >= SHOWSTATE_MISS || i >= mNumBitmapArrayRects[ShowType].size())
		return RectI(0,0,0,0);

	return mNumBitmapArrayRects[ShowType][i];
}

S32 GuiDamageHud::constructBitmapArray(GFXTexHandle& mTextureObject, U32 ShowType)
{
	if(mNumBitmapArrayRects[ShowType].size())
		return mNumBitmapArrayRects[ShowType].size();

	if( mTextureObject.isNull() )
		return 0;

	GBitmap *bmp = mTextureObject->getBitmap();

	ColorI sepColor;
	if ( !bmp || !bmp->getColor( 0, 0, sepColor ) )
	{
		return 0;
	}

	//now loop through all the scroll pieces, and find the bounding rectangle for each piece in each state
	S32 curY = 0;

	// ascertain the height of this row...
	ColorI color;
	mNumBitmapArrayRects[ShowType].clear();
	while(curY < bmp->getHeight())
	{
		// skip any sep colors
		bmp->getColor( 0, curY, color);
		if(color == sepColor)
		{
			curY++;
			continue;
		}
		// ok, process left to right, grabbing bitmaps as we go...
		S32 curX = 0;
		while(curX < bmp->getWidth())
		{
			bmp->getColor(curX, curY, color);
			if(color == sepColor)
			{
				curX++;
				continue;
			}
			S32 startX = curX;
			while(curX < bmp->getWidth())
			{
				bmp->getColor(curX, curY, color);
				if(color == sepColor)
					break;
				curX++;
			}
			S32 stepY = curY;
			while(stepY < bmp->getHeight())
			{
				bmp->getColor(startX, stepY, color);
				if(color == sepColor)
					break;
				stepY++;
			}
			mNumBitmapArrayRects[ShowType].push_back(RectI(startX, curY, curX - startX, stepY - curY));
		}
		// ok, now skip to the next separation color on column 0
		while(curY < bmp->getHeight())
		{
			bmp->getColor(0, curY, color);
			if(color == sepColor)
				break;
			curY++;
		}
	}
	return mNumBitmapArrayRects[ShowType].size();
}

ConsoleMethod(GuiDamageHud, SetDrawText, void, 4,4,"(nNumber, bRed)")
{
	object->SetDrawText(dAtol(argv[2]), argv[3]);
}
