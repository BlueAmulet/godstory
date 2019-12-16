//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/Data/GraphicsProfile.h"
#include "gfx/gfxDevice.h"
#include "windowManager/win32/win32Window.h"
#include "gfx/gfxInit.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"

IMPLEMENT_CONOBJECT(CGraphicsProfile);

CGraphicsProfile g_GraphicsProfile;

CGraphicsProfile::~CGraphicsProfile(void)
{
    for (int i = 0; i < mDisplayModeList.size(); ++i)
    {
        destructInPlace(&mDisplayModeList[i]);
    }
}

void CGraphicsProfile::Initialize()
{
	// 若DisplayMode为空，则初始化
	if(mDisplayModeList.empty())
		InitDisplayMode();

		//从配置文件中读取设置
	sgLightManager::sgSetBrightnessState(g_GraphicsProfile.mFSBrightness, g_GraphicsProfile.mFSVersion);
}

void CGraphicsProfile::InitDisplayMode()
{
	const GFXAdapter& pad = GFX->getAdapter();

	for (int i=0; i<pad.mAvailableModes.size(); i++)
	{
		// 小于最小分辨率和16位色的情况不考虑
		if(pad.mAvailableModes[i].resolution.x < Win32Window::getMinimum().x || pad.mAvailableModes[i].resolution.x < Win32Window::getMinimum().y)
			continue;
		if(pad.mAvailableModes[i].bitDepth == 16)
			continue;

		bool bFind = false;
		for(int j=0; j<mDisplayModeList.size(); j++)
		{
			if(mDisplayModeList[j].resolution == pad.mAvailableModes[i].resolution)
			{
				mDisplayModeList[j].refreshRate.push_back(pad.mAvailableModes[i].refreshRate);
				mDisplayModeList[j].antialiasLevel = getMax(mDisplayModeList[j].antialiasLevel, pad.mAvailableModes[i].antialiasLevel);
				bFind = true;
				break;
			}
		}
		if(!bFind)
		{
			DisplayMode dm;
			dm.resolution = pad.mAvailableModes[i].resolution;
			dm.proportion = ((F32)pad.mAvailableModes[i].resolution.x / (F32)pad.mAvailableModes[i].resolution.y);
			dm.antialiasLevel = getMax(pad.mAvailableModes[i].antialiasLevel, dm.antialiasLevel);
			dm.refreshRate.push_back(pad.mAvailableModes[i].refreshRate);
			mDisplayModeList.push_back(dm);
		}
	}

	// 检测硬件是否支持我们的最小分辨率
	AssertRelease(!mDisplayModeList.empty(), "您的显示系统不支持游戏最小分辨率800*600,请您更新驱动后再试!");
	return;
}

void CGraphicsProfile::ClampDisplayMode(GFXVideoMode &vm)
{
	AssertRelease(!mDisplayModeList.empty(), "DisplayMode is Null!");

	// 检查分辨率是否可用
	bool changed = false;
	if (vm.resolution.x == 0 && vm.resolution.y > 0)
	{
		// 寻找y匹配的分辨率
		for(S32 i=0; i<mDisplayModeList.size(); i++)
		{
			if(mDisplayModeList[i].resolution.y == vm.resolution.y)
			{
				vm.resolution.x = mDisplayModeList[i].resolution.x;
				changed = true;
				break;
			}
		}
	}
	else if (vm.resolution.y == 0 && vm.resolution.x > 0)
	{
		// 寻找x匹配的分辨率
		for(S32 i=0; i<mDisplayModeList.size(); i++)
		{
			if(mDisplayModeList[i].resolution.x == vm.resolution.x)
			{
				vm.resolution.y = mDisplayModeList[i].resolution.y;
				changed = true;
				break;
			}
		}
	}
	if (vm.resolution.x == 0 || vm.resolution.y == 0)
	{
		// 设置成最小可用分辨率
		vm.resolution = mDisplayModeList[0].resolution;

		changed = true;
	}

	if (changed)
		Con::warnf("New Resolution: %d %d", vm.resolution.x, vm.resolution.y);

	// 如果是窗口模式，则要求分辨率必须不大于桌面分辨率
	if(!vm.fullScreen)
	{
		GFXVideoMode DesktopRes = GFXInit::getDesktopResolution();
		// 颜色深度与桌面一致
		vm.bitDepth = DesktopRes.bitDepth;
		vm.refreshRate = DesktopRes.refreshRate;

		// 如果分辨率必须大于桌面分辨率
		if(vm.resolution.x > DesktopRes.resolution.x || vm.resolution.y > DesktopRes.resolution.y)
		{
			Con::warnf("Warning: The requested windowed resolution is larger than the current desktop resolution. Attempting to find a better resolution");
			bool windowValid = false;
			for (S32 i=mDisplayModeList.size()-1; i>=0; i--)
			{
				if(mDisplayModeList[i].resolution.x <= DesktopRes.resolution.x && mDisplayModeList[i].resolution.x <= DesktopRes.resolution.x)
				{
					vm.resolution = mDisplayModeList[i].resolution;
					windowValid = true;
					break;
				}
			}
			AssertRelease(windowValid, "您的桌面分辨率小于游戏最小分辨率800*600,请您修改桌面分辨率后再试!");
		}
	}
	// 如果是非标准分辨率，则转化成最小标准分辨率
	bool valid = false;
	for (S32 i=0; i<mDisplayModeList.size(); i++)
		if(mDisplayModeList[i].resolution == vm.resolution)
			valid = true;
	if(!valid)
		vm.resolution = mDisplayModeList[0].resolution;

	// 钳制刷新率
	for (S32 i=0; i<mDisplayModeList.size(); i++)
	{
		if(mDisplayModeList[i].resolution == vm.resolution)
		{
			vm.antialiasLevel = getMin(vm.antialiasLevel, mDisplayModeList[i].antialiasLevel);
			S32 offset = 0xFF;
			S32 delta = 0;
			for (S32 j=0; j<mDisplayModeList[i].refreshRate.size(); j++)
			{
				delta = mDisplayModeList[i].refreshRate[j] - vm.refreshRate;
				if(delta == 0)
					return;
				else if( mAbs(offset) > mAbs(delta) )
					offset = delta;
			}
			vm.refreshRate += offset;
			return;
		}
	}
	AssertFatal(false, "WTF!");
}

void CGraphicsProfile::SetCurrentMode(GFXVideoMode &vm)
{
	for (S32 i=0; i<mDisplayModeList.size(); i++)
	{
		if(mDisplayModeList[i].resolution == vm.resolution &&
			mDisplayModeList[i].antialiasLevel >= vm.antialiasLevel)
		{
			mCurrentMode.resolution = mDisplayModeList[i].resolution;
			mCurrentMode.proportion = mDisplayModeList[i].proportion;
			mCurrentMode.antialiasLevel = mDisplayModeList[i].antialiasLevel;
			mCurrentMode.refreshRate.push_back(vm.refreshRate);
			return;
		}
	}
	AssertFatal(false, "WTF!!");
}

bool CGraphicsProfile::getDrawNameState()
{
	return mDrawNameState;
}

CGraphicsProfile::DisplayMode& CGraphicsProfile::GetCurrentMode()
{
	return mCurrentMode;
}

ConsoleFunction(setVisiblePlayerCount,void, 2, 2,"setVisiblePlayerCount(%Number)")
{
   g_GraphicsProfile.mVisiblePlayerCount = atoi(argv[1]);
}

ConsoleFunction(setBrightness, void, 2, 2, "setBrightness(%Value)")
{
	g_GraphicsProfile.mBrightness = atoi(argv[1]);
}

ConsoleFunction(setContrast,void, 2, 2, "setContrast(%Value)")
{
    g_GraphicsProfile.mContrast = atoi(argv[1]);
}

ConsoleFunction(setSkillEffect,void, 2, 2,"setSkillEffect(%bool)")
{
    g_GraphicsProfile.mSkillEffect = atoi(argv[1]);
}

ConsoleFunction(setWeaponEffect,void, 2, 2,"setWeaponEffect(%bool)")
{
   g_GraphicsProfile.mWeaponEffect = atoi(argv[1]);
}

ConsoleFunction(setStaticShadowLevel, void, 2, 2, "setStaticShadowLevel(%Level)")
{
	g_GraphicsProfile.mStaticShadowLevel = atoi(argv[1]);
}

ConsoleFunction(setShadowLevel, void, 2, 2, "setShadowLevel(%Level)")
{
	g_GraphicsProfile.mShadowLevel = atoi(argv[1]);
}

ConsoleFunction(setTinyStaticLevel, void, 2, 2, "setTinyStaticLevel(%Level)")
{
	g_GraphicsProfile.mTinyStaticLevel = atoi(argv[1]);
}

ConsoleFunction(setTexFilterType,void, 2, 2, "setTexFilerType(%Type)")
{
   g_GraphicsProfile.mTexFilterType = atoi(argv[1]);
}
ConsoleFunction(setWaterReflaction, void, 2, 2, "setWaterReflaction(%bool)")
{
	g_GraphicsProfile.mWaterReflaction = (atoi(argv[1])!=0);
}

ConsoleFunction(setStaticAnimation, void, 2, 2,"setStaticAnimation(%bool)")
{
	g_GraphicsProfile.mStaticAnimation = atoi(argv[1]);
}

ConsoleFunction(getDrawNameState,bool,1,1,"getDrawNameState()")
{
	return g_GraphicsProfile.mDrawNameState;
}
ConsoleFunction(setDrawNameState,void,2,2,"setDrawNameState(bool state)")
{
	g_GraphicsProfile.mDrawNameState = dAtob(argv[1]);
}

ConsoleFunction(setenableBrightness, void, 2, 2, "setenableBrightness(%bool)")
{
	g_GraphicsProfile.mFSBrightness = (atoi(argv[1])!=0);
	sgLightManager::sgSetBrightnessState(g_GraphicsProfile.mFSBrightness, g_GraphicsProfile.mFSVersion);
}

ConsoleFunction(setBrightnessVersion, void, 2, 2, "setBrightnessVersion(%Level)")
{
	g_GraphicsProfile.mFSVersion = atoi(argv[1]);
	sgLightManager::sgSetBrightnessState(g_GraphicsProfile.mFSBrightness, g_GraphicsProfile.mFSVersion);
}

ConsoleFunction(getDisplayModelList,void, 1, 1, "getDisplayModelList()")
{
  for (S32 i=0;i<g_GraphicsProfile.mDisplayModeList.size();i++)
  {
	  char resolution[24];
	  char prop[6];
	  if (g_GraphicsProfile.mDisplayModeList[i].proportion<1.5f)
		  dSprintf(prop,sizeof(prop),"%d:%d",4,3);
	  else
          dSprintf(prop,sizeof(prop),"%d:%d",16,9);
	  dSprintf(resolution,sizeof(resolution),"%d*%d  %s",g_GraphicsProfile.mDisplayModeList[i].resolution.x,g_GraphicsProfile.mDisplayModeList[i].resolution.y,prop);
	  Con::executef("getResolutionList",resolution,Con::getIntArg(i));
      char FSAA[12];
      dSprintf(FSAA,sizeof(FSAA),"32位   %dx",g_GraphicsProfile.mDisplayModeList[i].antialiasLevel);

	  //Con::executef("getFSAAList",FSAA,Con::getIntArg(i));
  }
}

ConsoleFunction(getCurrentModel,const char*,1,1,"getCurrentModel()")
{
	char* ModelText= Con::getReturnBuffer(24);
	char prop[6];
	if (g_GraphicsProfile.GetCurrentMode().proportion<1.5f)
		dSprintf(prop,sizeof(prop),"%d:%d",4,3);
	else
		dSprintf(prop,sizeof(prop),"%d:%d",16,9);
	dSprintf(ModelText,24,"%d*%d  %s",g_GraphicsProfile.GetCurrentMode().resolution.x,g_GraphicsProfile.GetCurrentMode().resolution.y,prop);

	return ModelText;
}

ConsoleFunction(getDisplayModelState, void , 1, 1, "getDisplayModelState()")
{
	Con::executef("getInitModelState",Con::getIntArg(g_GraphicsProfile.mIsFullScreen),Con::getIntArg(g_GraphicsProfile.mVisiblePlayerCount),
		Con::getIntArg(g_GraphicsProfile.mSkillEffect),Con::getIntArg(g_GraphicsProfile.mWeaponEffect),Con::getIntArg(g_GraphicsProfile.mBrightness),
		Con::getIntArg(g_GraphicsProfile.mContrast));
}

ConsoleFunction(getEntironmentState,void, 1, 1, "")
{
	Con::executef("getInitEntironmentValue",Con::getIntArg(g_GraphicsProfile.mTexFilterType),Con::getIntArg(g_GraphicsProfile.mShadowLevel),
		Con::getIntArg(g_GraphicsProfile.mTinyStaticLevel),Con::getIntArg(g_GraphicsProfile.mStaticAnimation),Con::getIntArg(g_GraphicsProfile.mFSBrightness),
		Con::getIntArg(g_GraphicsProfile.mWaterReflaction),Con::getIntArg(1));
}