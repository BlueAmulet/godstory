//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef GRAPHICS_PROFILE_H
#define GRAPHICS_PROFILE_H

#include "console/consoleObject.h"
#include "math/mPoint.h"

struct GFXVideoMode;

class CGraphicsProfile : public ConsoleObject
{
	typedef ConsoleObject Parent;
public:
	DECLARE_CONOBJECT(CGraphicsProfile);
	enum
	{
		MaxTinyStaticLevel	= 2
	};

	// �ֱ���ģʽ
	struct DisplayMode
	{
		Point2I	resolution;			// �ֱ���
		F32		proportion;			// ��Ļ��߱�
		U32		antialiasLevel;		// AA�ȼ�
		Vector<U32> refreshRate;	// ˢ����

		DisplayMode():resolution(1,1),proportion(1),antialiasLevel(0) { refreshRate.clear();}
	};

	Vector<DisplayMode>		mDisplayModeList;
	DisplayMode				mCurrentMode;
	
	//�ֱ���
	bool mIsFullScreen;		//�Ƿ�ȫ��
	int  mResolvingHeight;	//�ֱ���
	int  mResolvingWidth;
	int  mScreenHeight;		//��Ļ��߱�
	int  mScreenWidth;

	//��ɫ
	int  mBrightness;		//����
	int  mContrast;			//�Աȶ�

	//����
	int  mVisiblePlayerCount;	//�ɼ��������  0ֻ��ʾ�Լ�  999���
	bool mStaticAnimation;		//�������
	int  mTinyStaticLevel;		//С�����ʾ�ȼ�  0����ʾС��� 1����ʾʯͷ 2����ʾ��

	//��Ч
	int  mEffectPlayerCount;	//��Ч�������  0����ʾ�������Լ��� 1ֻ��ʾ�Լ�  999���
	bool mSkillEffect;			//������Ч�Ƿ���ʾ
	bool mWeaponEffect;			//װ����Ч�Ƿ���ʾ
	bool mWaterReflaction;		//ˮ�淴��

	//��Ļ
	bool mFSBrightness;			//ȫ������
	int  mFSVersion;			//����Ч���Ĳ�ͬ�汾
	int  mFSAA;					// 0 2x 4x 8x  ȫ�������

	//����
	int  mTexFilterType;		//0˫���� 1������ 2��������
	int  mAFLevel;				//�������Եȼ�  2x 4x 8x 16x

	//��Ӱ
	int  mStaticShadowLevel;	//�����Ӱ�ȼ� 0����Ӱ 1����Ӱ
	int  mShadowLevel;			//������Ӱ�ȼ� 0����Ӱ 1ԭ����Ӱ 2��������Ӱ 3����Ӱ

	bool mDrawNameState;        //��ʾͷ����Ϣ 0����ʾ 1��ʾ

	CGraphicsProfile()
	{
		mIsFullScreen		= false;
		mResolvingHeight	= 600;
		mResolvingWidth		= 800;
		mScreenHeight		= 600;
		mScreenWidth		= 800;

		mBrightness			= 50;
		mContrast			= 50;

		mVisiblePlayerCount = 999;
		mStaticAnimation	= true;
		mTinyStaticLevel	= 2;

		mEffectPlayerCount	= 999;
		mSkillEffect		= true;
		mWeaponEffect		= true;
		mWaterReflaction	= true;

		mFSBrightness		= false;
		mFSVersion			= 1;
		mFSAA				= 8;

		mTexFilterType		= 2;
		mAFLevel			= 16;

		mStaticShadowLevel  = 1;
		mShadowLevel		= 3;

		mDrawNameState      = true;
	}

    ~CGraphicsProfile(void);

	void Initialize();
	void InitDisplayMode();
	void ClampDisplayMode(GFXVideoMode &vm);
	void SetCurrentMode(GFXVideoMode &vm);
	DisplayMode& GetCurrentMode();
	bool getDrawNameState();
};

extern CGraphicsProfile g_GraphicsProfile;

#endif
