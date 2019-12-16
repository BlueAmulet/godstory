//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#ifndef _SGDRL_H_
#define _SGDRL_H_

#include "core/tVector.h"
#include "gfx/gfxTextureHandle.h"
#include "gui/core/guiControl.h"
#include "materials/shaderData.h"


class sgDRLSurfaceChain
{
private:
	enum
	{
		sgdrlscSampleWidth = 4,
		sgdlrscBloomIndex = 2
	};

	Point2I sgOffset;
	Point2I sgExtent;

	//ShaderData *sgAlphaBloom;
	ShaderData *sgDownSample4x4;
	ShaderData *sgDownSample4x4Final;
	ShaderData *sgDownSample4x4BloomClamp;
	ShaderData *sgBloomBlur;
	ShaderData *sgDRLFull;
	ShaderData *sgDRLOnlyBloomTone;

	ShaderData *sgColorEdgeDetect;
    ShaderData *sgColorDownFilter4;
    ShaderData *sgColorBloomH;
	ShaderData *sgColorBloomV;
	ShaderData *sgColorCombine4;

    GFXVertexBufferHandle<GFXVertexP4WT> mVertBuff;
    GFXVertexP4WT mVertex[4];

	Vector<Point2I> sgSurfaceSize;
	GFXTexHandle sgBloom;
	GFXTexHandle sgBloom2;
	GFXTexHandle sgToneMap;
	GFXTexHandle sgGrayMap;
	GFXTexHandle sgDRLViewMap;

	bool sgCachedIsHDR;
	void sgDestroyChain();

	static F32 SceneIntensity;
	static F32 GlowIntensity;
	static F32 HighlightIntensity;
    static F32 HighlightThreshold;
	static F32 BloomScale;
	static F32 BloomOffset;
	static bool sgResetTexture;
protected:
	Vector<GFXTexHandle> sgSurfaceChain;

	sgDRLSurfaceChain()
	{
		sgOffset = Point2I(0, 0);
		//sgExtent = Point2I(0, 0);
		sgDownSample4x4 = NULL;
		sgDownSample4x4Final = NULL;
		sgDownSample4x4BloomClamp = NULL;
		sgBloomBlur = NULL;
		sgDRLFull = NULL;
		sgDRLOnlyBloomTone = NULL;
		sgCachedIsHDR = false;
		sgColorEdgeDetect = NULL;
		sgColorDownFilter4 = NULL;
		sgColorBloomH = NULL;
		sgColorBloomV = NULL;
        sgColorCombine4 = NULL;
		mVertBuff = NULL;
	}
	virtual ~sgDRLSurfaceChain() {sgDestroyChain();}

	// detect primary surface changes...
	void sgPrepChain(const Point2I &offset, const Point2I &extent);
	// make copies and down-sample...
	void sgRenderChain();
	// render...
	void sgRenderDRL();

public:
	static void SetGlowConstants(float c1, float c2, float c3, float c4, float c5);
	static void ResetTexture(bool data);
private:
	static GFXStateBlock* mSetBloomSB;
	static GFXStateBlock* mClearBloomSB;
	static GFXStateBlock* mSetSB;
	static GFXStateBlock* mSetAlphaLerpSB;
	static GFXStateBlock* mSetAlphaFalseSB;
	static GFXStateBlock* mAddrClampSB;
	static GFXStateBlock* mClearSB;
	static GFXStateBlock* mSetDrlSB;
	static GFXStateBlock* mClearDrlSB;
public:
	static void init();
	static void shutdown();
	//
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();
};

class sgDRLSystem : public sgDRLSurfaceChain
{
public:
	bool sgDidPrep;
	sgDRLSystem() {sgDidPrep = false;}
	void sgPrepSystem(const Point2I &offset, const Point2I &extent);
	void sgRenderSystem();
};

#ifdef NTJ_GUI
class sgGuiTexTestCtrl : public GuiControl
{
private:
   typedef GuiControl Parent;

   S32 sgTextureLevel;

public:
   //creation methods
   DECLARE_CONOBJECT(sgGuiTexTestCtrl);

   sgGuiTexTestCtrl() {sgTextureLevel = 0;}
   static void initPersistFields();
   void onRender(Point2I offset, const RectI &updateRect);
};
#endif

#endif//_SGDRL_H_
