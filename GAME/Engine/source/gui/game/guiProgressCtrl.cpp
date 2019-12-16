//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/consoleTypes.h"
#include "gui/game/guiProgressCtrl.h"


IMPLEMENT_CONOBJECT(GuiProgressCtrl);

GuiProgressCtrl::GuiProgressCtrl() : mProgress(0.0f),
                                     m_ProgressType(PROGRESSTYPE_RECT),
									 m_ProgressBackRect(0,0,1,1),
									 m_ProgressRect(0,0,1,1),
									 m_ProgressTopRect(0,0,1,1),
									 m_ProgressEffectRect(0,0,1,1),
									 m_ProgressLightRect(0,0,1,1),
									 m_ProgressBackTexture(NULL),
									 m_ProgressImageTexture(NULL),
									 m_ProgressImageTexture2(NULL),
									 m_ProgressTopTexture(NULL),
									 m_ProgressEffectTexture(NULL),
									 m_ProgressLightTexture(NULL),
									 m_bShowProgressFile2(false)
{

   m_ProgressImageFileName		= StringTable->insert("");      // 进度条	
   m_ProgressImageFileName2	    = StringTable->insert("");      // 进度条2(替换前一层进度条)
   m_ProgressBackImageFile		= StringTable->insert("");      // 底面背景
   m_ProgressTopImageFile		= StringTable->insert("");      // 进度条顶层
   m_ProgressLightImageFile	    = StringTable->insert("");      // 进度条前面闪亮的光效
   m_ProgressEffectImageFile	= StringTable->insert("");      // 结束特效
}

void GuiProgressCtrl::initPersistFields()
{
	Parent::initPersistFields();
	addGroup("guiProgressCtrl");

	addField("Progress",                TypeF32 ,   Offset(mProgress, GuiProgressCtrl));
	addField("ProgressType",            TypeS8,     Offset(m_ProgressType,GuiProgressCtrl));

	addField("ProgressBackRect",	    TypeRectI,  Offset(m_ProgressBackRect,GuiProgressCtrl));
	addField("ProgressTopRect",			TypeRectI,  Offset(m_ProgressTopRect,GuiProgressCtrl));
	addField("ProgressRect",			TypeRectI,  Offset(m_ProgressRect,GuiProgressCtrl));
	addField("ProgressLightRect",		TypeRectI,  Offset(m_ProgressLightRect,GuiProgressCtrl));
	addField("ProgressEffectRect",		TypeRectI,  Offset(m_ProgressEffectRect,GuiProgressCtrl));

	addField("ProgressBackImageFile",	TypeString, Offset(m_ProgressBackImageFile,GuiProgressCtrl));
	addField("ProgressTopImageFile",	TypeString,	Offset(m_ProgressTopImageFile,GuiProgressCtrl));
	addField("ProgressLightImageFile",	TypeString, Offset(m_ProgressLightImageFile,GuiProgressCtrl));
	addField("ProgressEffectImageFile",	TypeString, Offset(m_ProgressEffectImageFile,GuiProgressCtrl));
	addField("ProgressImageFileName",	TypeString, Offset(m_ProgressImageFileName, GuiProgressCtrl));
	addField("ProgressImageFileName2",	TypeString, Offset(m_ProgressImageFileName2, GuiProgressCtrl));

	endGroup("guiProgressCtrl");

}

void GuiProgressCtrl::setProgressValue(F32 value)
{
	mProgress = mClampF(value, 0.f, 1.f);
	setUpdate();
}

const char* GuiProgressCtrl::getScriptValue()
{
   char * ret = Con::getReturnBuffer(64);
   dSprintf(ret, 64, "%g", mProgress);
   return ret;
}

void GuiProgressCtrl::setScriptValue(const char *value)
{
   //set the value
   if (! value)
      mProgress = 0.0f;
   else
      mProgress = dAtof(value);

   //validate the value
   mProgress = mClampF(mProgress, 0.f, 1.f);
   setUpdate();
}

bool GuiProgressCtrl::onWake()
{
	if(!Parent::onWake())
		return false;

	if(m_ProgressType == PROGRESSTYPE_IMAGE)
	{
		if(m_ProgressImageFileName && m_ProgressImageFileName[0] && m_ProgressImageTexture.isNull())
			m_ProgressImageTexture = GFXTexHandle(m_ProgressImageFileName, &GFXDefaultStaticDiffuseProfile, true);

		if(m_ProgressImageFileName2 && m_ProgressImageFileName2[0] && m_ProgressImageTexture2.isNull())
			m_ProgressImageTexture2 = GFXTexHandle(m_ProgressImageFileName2, &GFXDefaultStaticDiffuseProfile, true);

		if(m_ProgressBackImageFile && m_ProgressBackImageFile[0] && m_ProgressBackTexture.isNull())
			m_ProgressBackTexture =  GFXTexHandle(m_ProgressBackImageFile, &GFXDefaultStaticDiffuseProfile, true);

		if(m_ProgressTopImageFile && m_ProgressTopImageFile[0] && m_ProgressTopTexture.isNull())
			m_ProgressTopTexture = GFXTexHandle(m_ProgressTopImageFile,&GFXDefaultStaticDiffuseProfile,true);

		if(m_ProgressLightImageFile && m_ProgressLightImageFile[0] && m_ProgressLightTexture.isNull())
			m_ProgressLightTexture = GFXTexHandle(m_ProgressLightImageFile, &GFXDefaultStaticDiffuseProfile, true);

		if(m_ProgressEffectImageFile && m_ProgressEffectImageFile[0] && m_ProgressEffectTexture.isNull())
			m_ProgressEffectTexture = GFXTexHandle(m_ProgressEffectImageFile, &GFXDefaultStaticDiffuseProfile, true);
	}

	return true;
}


void GuiProgressCtrl::onSleep()
{
	if(m_ProgressType == PROGRESSTYPE_IMAGE)
	{
		m_ProgressBackTexture = NULL;
		m_ProgressImageTexture = NULL;
		m_ProgressImageTexture2 = NULL;
		m_ProgressTopTexture = NULL;
		m_ProgressEffectTexture = NULL;
		m_ProgressLightTexture = NULL;
	}

	Parent::onSleep();
}


void GuiProgressCtrl::onPreRender()
{
   const char * var = getVariable();
   if(var)
   {
      F32 value = mClampF(dAtof(var), 0.f, 1.f);
      if(value != mProgress)
      {
         mProgress = value;
         setUpdate();
      }
   }
}

//draw the progress
void GuiProgressCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   if(mProgress <= 0)
	   return;

   GFX->getDrawUtil()->clearBitmapModulation();

   if(m_ProgressType == PROGRESSTYPE_IMAGE)
   {
	   RectI rect(offset + m_ProgressRect.point,m_ProgressRect.extent);
	   rect.extent.x *= mProgress;

	   if(rect.extent.x <= 0)
		   return;

	   RectI tempRect;
	   if(m_ProgressBackTexture.isValid())
	   {
		   tempRect.set(offset + m_ProgressBackRect.point,m_ProgressBackRect.extent);
		   GFX->getDrawUtil()->drawBitmapStretch(m_ProgressBackTexture,tempRect);
	   }

	   if(m_bShowProgressFile2 && m_ProgressImageTexture2.isValid())
	   {
		   GFX->getDrawUtil()->drawBitmapStretch(m_ProgressImageTexture2, rect);
	   }
	   else if(m_ProgressImageTexture.isValid())
	   {
		   GFX->getDrawUtil()->drawBitmapStretch(m_ProgressImageTexture, rect);
	   }

	   if(m_ProgressLightTexture.isValid())
	   {
		   Point2I point(rect.extent.x + m_ProgressRect.point.x - m_ProgressLightRect.extent.x / 2,m_ProgressLightRect.point.y);
		   tempRect.set(offset + point,m_ProgressLightRect.extent);
		   GFX->getDrawUtil()->drawBitmapStretch(m_ProgressLightTexture,tempRect);
	   }

	   if(m_ProgressTopTexture.isValid())
	   {
		   tempRect.set(offset + m_ProgressTopRect.point, m_ProgressTopRect.extent);
		   GFX->getDrawUtil()->drawBitmapStretch(m_ProgressTopTexture,tempRect);
	   }

	   //结束特效在guiProgressCtrlEx里处理
   }
   else
   {
	   RectI ctrlRect(offset, getExtent());
	   ctrlRect.extent.x *= mProgress;

	   if(ctrlRect.extent.x <=0)
		   return;
	   
	   GFX->getDrawUtil()->drawRectFill(ctrlRect, mProfile->mFillColor);
	   
	   if (mProfile->mBorder)
		   GFX->getDrawUtil()->drawRect(ctrlRect, mProfile->mBorderColor);
   }

   Parent::onRender( offset, updateRect );

   //render the children
   renderChildControls(offset, updateRect);
}

void GuiProgressCtrl::ShowProgressFile2(bool value)
{
	m_bShowProgressFile2 = value;
}


//------------------------------------------------------------------------------------------------
ConsoleMethod(GuiProgressCtrl,setScriptValue,void,3,3,"setScriptValue( value );")
{
	return object->setScriptValue(argv[2]);
}

ConsoleMethod(GuiProgressCtrl,getScriptValue,const char*,2,2,"getScriptValue();")
{
	return object->getScriptValue();
}

ConsoleMethod(GuiProgressCtrl,setShowProgressLayer2,void,3,3,"obj.setShowProgressLayer2(%isShow)")
{
	object->ShowProgressFile2(atoi(argv[2]));
}
