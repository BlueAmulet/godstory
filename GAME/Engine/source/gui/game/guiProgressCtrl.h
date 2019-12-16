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
		PROGRESSTYPE_RECT = 0,     //����ɫ��䷽ʽ
		PROGRESSTYPE_IMAGE,        //  ͼ�����Ʒ�ʽ
	}; 

protected:
   F32          mProgress;
   U8			m_ProgressType;
   RectI		m_ProgressBackRect;		// ��
   RectI		m_ProgressTopRect;		// ��
   RectI		m_ProgressRect;			// ������
   RectI		m_ProgressEffectRect;	// ��Ч
   RectI		m_ProgressLightRect;	// ����
   bool         m_bShowProgressFile2;

   StringTableEntry		m_ProgressBackImageFile;		//���汳��ͼƬ(��:��͸���ҵ�)
   StringTableEntry		m_ProgressImageFileName;		//������ͼƬ
   StringTableEntry		m_ProgressImageFileName2;		//������ͼƬ2
   StringTableEntry		m_ProgressTopImageFile;			//���ͼƬ
   StringTableEntry		m_ProgressLightImageFile;		//������ǰ�������Ĺ�ЧͼƬ
   StringTableEntry		m_ProgressEffectImageFile;		//��������Ч��ͼ
   
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
