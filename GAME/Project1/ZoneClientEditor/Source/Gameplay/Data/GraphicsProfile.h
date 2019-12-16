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

	// 分辨率模式
	struct DisplayMode
	{
		Point2I	resolution;			// 分辨率
		F32		proportion;			// 屏幕宽高比
		U32		antialiasLevel;		// AA等级
		Vector<U32> refreshRate;	// 刷新率

		DisplayMode():resolution(1,1),proportion(1),antialiasLevel(0) { refreshRate.clear();}
	};

	Vector<DisplayMode>		mDisplayModeList;
	DisplayMode				mCurrentMode;
	
	//分辨率
	bool mIsFullScreen;		//是否全屏
	int  mResolvingHeight;	//分辨率
	int  mResolvingWidth;
	int  mScreenHeight;		//屏幕宽高比
	int  mScreenWidth;

	//颜色
	int  mBrightness;		//亮度
	int  mContrast;			//对比度

	//物体
	int  mVisiblePlayerCount;	//可见玩家数量  0只显示自己  999最大
	bool mStaticAnimation;		//物件动画
	int  mTinyStaticLevel;		//小物件显示等级  0不显示小物件 1可显示石头 2可显示草

	//特效
	int  mEffectPlayerCount;	//特效玩家数量  0不显示（包括自己） 1只显示自己  999最大
	bool mSkillEffect;			//技能特效是否显示
	bool mWeaponEffect;			//装备特效是否显示
	bool mWaterReflaction;		//水面反射

	//屏幕
	bool mFSBrightness;			//全屏泛光
	int  mFSVersion;			//泛光效果的不同版本
	int  mFSAA;					// 0 2x 4x 8x  全屏抗锯齿

	//纹理
	int  mTexFilterType;		//0双线性 1三线性 2各向异性
	int  mAFLevel;				//各项异性等级  2x 4x 8x 16x

	//阴影
	int  mStaticShadowLevel;	//物件阴影等级 0无阴影 1有阴影
	int  mShadowLevel;			//人物阴影等级 0无阴影 1原形阴影 2自身软阴影 3软阴影

	bool mDrawNameState;        //显示头顶信息 0不显示 1显示

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
