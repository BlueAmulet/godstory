
#include "UI/guiMoveBitmapCtrl.h"
#include "console/consoleTypes.h"

IMPLEMENT_CONOBJECT(GuiMoveBitmapCtrl);

GuiMoveBitmapCtrl::GuiMoveBitmapCtrl()
{
	mMoveSpeed = 0;
	mIsMove = false;
	mIsXDirection = false;
	mIsYDirection = false;
	mChangeNum = 0;
	mOldTime = 0;
	mNowTime = 0;
	mMoveType = 0;
	mUpdateTime = 0;
	mCounter = 0;
	mTexPoint.set(0,0);
	mTexRect.set(0,0,1,1);
	mXDirCounter = 0;
	mYDirCounter = 0;
}

void GuiMoveBitmapCtrl::initPersistFields()
{
	Parent::initPersistFields();
	
	addField("MoveSpeed",         TypeF32,   Offset(mMoveSpeed,     GuiMoveBitmapCtrl));
	addField("IsMove",            TypeBool,  Offset(mIsMove,        GuiMoveBitmapCtrl));
	addField("IsXDirection",      TypeBool,  Offset(mIsXDirection,  GuiMoveBitmapCtrl));
	addField("IsYDirection",      TypeBool,  Offset(mIsYDirection,  GuiMoveBitmapCtrl));
	addField("MoveType",          TypeS32,   Offset(mMoveType,      GuiMoveBitmapCtrl));
	addField("UpdateTime",        TypeS32,   Offset(mUpdateTime,    GuiMoveBitmapCtrl));
}

bool GuiMoveBitmapCtrl::onWake()
{
	if ( !Parent::onWake())
		return false;
	setActive(true);
	mOldTime = Platform::getVirtualMilliseconds();
	return true;
}

void GuiMoveBitmapCtrl::onRender(Point2I offset, const RectI &updateRect)
{

	mNowTime = Platform::getVirtualMilliseconds();
	switch (mMoveType)
	{
	case 0:
		{
			mChangeNum = (mNowTime - mOldTime) * mMoveSpeed;
			RectI rect(offset,getExtent());

			if (!mIsMove)
			{
				mChangeNum = 0;
			}

			if (mIsXDirection)
			{
				RectF singleRect1(offset.x + mChangeNum, offset.y, rect.extent.x / 2, rect.extent.y);
				RectF singleRect2(offset.x + mChangeNum + rect.extent.x / 2, offset.y, rect.extent.x / 2, rect.extent.y);
				if (mMoveSpeed > 0)
				{
					if (singleRect2.point.x >= (rect.extent.x / 2))
					{
						mOldTime = mNowTime;
					}
					GFX->getDrawUtil()->clearBitmapModulation();
					GFX->getDrawUtil()->drawBitmapStretch(mTextureObject,singleRect1);
					GFX->getDrawUtil()->drawBitmapStretch(mTextureObject,singleRect2);
				}
				else
				{
					if (singleRect2.point.x <= rect.point.x)
					{
						mOldTime = mNowTime;
					}
					GFX->getDrawUtil()->clearBitmapModulation();
					GFX->getDrawUtil()->drawBitmapStretch(mTextureObject,singleRect1);
					GFX->getDrawUtil()->drawBitmapStretch(mTextureObject,singleRect2);
				}
			}
			if (mIsYDirection)
			{
				RectF singleRect1(offset.x, offset.y + mChangeNum, rect.extent.x, rect.extent.y / 2);
				RectF singleRect2(offset.x, offset.y + mChangeNum + rect.extent.y / 2, rect.extent.x, rect.extent.y / 2);
				if (mMoveSpeed > 0)
				{
					if (singleRect2.point.y >= (rect.extent.y / 2))
					{
						mOldTime = mNowTime;
					}
					GFX->getDrawUtil()->clearBitmapModulation();
					GFX->getDrawUtil()->drawBitmapStretch(mTextureObject,singleRect1);
					GFX->getDrawUtil()->drawBitmapStretch(mTextureObject,singleRect2);
				}
				else
				{
					if (singleRect2.point.y <= rect.point.y)
					{
						mOldTime = mNowTime;
					}
					GFX->getDrawUtil()->clearBitmapModulation();
					GFX->getDrawUtil()->drawBitmapStretch(mTextureObject,singleRect1);
					GFX->getDrawUtil()->drawBitmapStretch(mTextureObject,singleRect2);
				}
			}
		}
		break;
	case 1:
		{
			RectI rect(offset,getExtent());

			if ( mCounter < mUpdateTime )
			{
				mCounter = (mNowTime - mOldTime);
			}
			else
			{
				if (!isUpdate(rect))
				{
					return;
				}
				mCounter = 0;
				mOldTime = mNowTime;
			}
			GFX->getDrawUtil()->clearBitmapModulation();
			GFX->getDrawUtil()->drawBitmapSR(mTextureObject,mTexPoint + offset,mTexRect);

		}
		break;
	}
	
	renderChildControls(offset, updateRect);
}

bool GuiMoveBitmapCtrl::isUpdate(RectI rect)
{
	
	int xNum = mTextureObject.getWidth() / rect.extent.x;
	int yNum = mTextureObject.getHeight() / rect.extent.y;

	if (mTextureObject)
	{
		mTexPoint.set(0,0);
		mTexRect.set(rect.extent.x * mXDirCounter,rect.extent.y * mYDirCounter,mTextureObject.getWidth(),mTextureObject.getHeight());
		if (mXDirCounter < xNum - 1)
		{
			mXDirCounter++;
		}
		else
		{
			if (mYDirCounter < yNum - 1)
			{
				mYDirCounter++;
			}
			else
				mYDirCounter = 0;
			mXDirCounter = 0;
		}
	}

	return true;
}

void GuiMoveBitmapCtrl::setMove(bool isMove)
{
	mIsMove = isMove; 
}

ConsoleMethod(GuiMoveBitmapCtrl, setMove, void, 2, 2, "obj.setMove( isMove )")
{
	object->setMove(dAtoi(argv[1]));
}