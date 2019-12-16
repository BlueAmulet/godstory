//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUIPROGRESSCTRL_H_
#define _GUIPROGRESSCTRL_H_
#include "gui/controls/guiTextCtrl.h"

class GuiTextCtrl;
class GuiProgressCtrl : public GuiTextCtrl
{
	typedef GuiTextCtrl Parent;

public:
	enum Constants
	{
		PROGRESSTYPE_RECT = 0,     //　颜色填充方式
		PROGRESSTYPE_IMAGE,        //  图处绘制方式
	}; 

protected:
   F32          mProgress;
   U8			m_ProgressType;
   RectI		m_ProgressBackRect;		// 底
   RectI		m_ProgressTopRect;		// 顶
   RectI		m_ProgressRect;			// 进度条
   RectI		m_ProgressEffectRect;	// 特效
   RectI		m_ProgressLightRect;	// 闪光
   bool         m_bShowProgressFile2;

   StringTableEntry		m_ProgressBackImageFile;		//底面背景图片(如:半透明灰底)
   StringTableEntry		m_ProgressImageFileName;		//进度条图片
   StringTableEntry		m_ProgressImageFileName2;		//进度条图片2
   StringTableEntry		m_ProgressTopImageFile;			//外框图片
   StringTableEntry		m_ProgressLightImageFile;		//进度条前面闪亮的光效图片
   StringTableEntry		m_ProgressEffectImageFile;		//近结束的效果图
   
   GFXTexHandle         m_ProgressBackTexture;
   GFXTexHandle      	m_ProgressImageTexture;
   GFXTexHandle     	m_ProgressImageTexture2;
   GFXTexHandle         m_ProgressTopTexture;
   GFXTexHandle         m_ProgressEffectTexture;
   GFXTexHandle         m_ProgressLightTexture;

public:
   //creation methods
   DECLARE_CONOBJECT(GuiProgressCtrl);
   GuiProgressCtrl();
   static void initPersistFields();

   inline F32      getProgressValue()          { return mProgress;}
   void            setProgressValue(F32 value);

   bool      onWake             ();
   void      onSleep            ();
   void      onPreRender        ();
   void      onRender           ( Point2I offset, const RectI &updateRect );
   void      ShowProgressFile2  ( bool value );

   //console related methods
   virtual const char*  getScriptValue();
   virtual void         setScriptValue(const char *value);
};

#endif
