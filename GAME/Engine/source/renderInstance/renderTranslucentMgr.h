//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _RENDER_TRANSLUCENT_MGR_H_
#define _RENDER_TRANSLUCENT_MGR_H_

#include "renderElemMgr.h"

class ShaderData;
//**************************************************************************
// RenderTranslucentMgr
//**************************************************************************
class RenderTranslucentMgr : public RenderElemMgr
{
private:
   void sort();
   void addElement( RenderInst *inst );

   static S32 FN_CDECL cmpTranslucentKeyFunc(const void* p1, const void* p2);  
protected:
   void setupSGData( RenderInst *ri, SceneGraphData &data );
public:
	typedef RenderElemMgr Parent;

	enum RENDERTRANSLUCENT_TYPE
	{
		RENDERTRANSLUCENT_TYPE_EDGEBLUR = 10086,
		RENDERTRANSLUCENT_TYPE_ELECTRICITY,
		RENDERTRANSLUCENT_TYPE_VOLUMEFOG,
		RENDERTRANSLUCENT_TYPE_PARTICLE,
		RENDERTRANSLUCENT_TYPE_RIBBON,
	};

   virtual void render();
private:
	static GFXStateBlock* mSetSB;
	//cull
	static GFXStateBlock* mCullCCWSB;
	static GFXStateBlock* mCullNoneSB;
	//object translucent
	static GFXStateBlock* mSetTransSB;
	//particles
	static GFXStateBlock* mParticleOneSB;//1,5
	static GFXStateBlock* mParticleTwoSB;//2,6
	static GFXStateBlock* mParticleTriSB;//3,7
	static GFXStateBlock* mParticleForSB;//4,8
	//light enable
	static GFXStateBlock* mLightEnableSB;
	static GFXStateBlock* mLightStoreSB;
	//mat set
	static GFXStateBlock* mTexOpSB;
	static GFXStateBlock* mTransflagSB;
	static GFXStateBlock* mTransSB;
	//alpha blend
	static GFXStateBlock* mAlphaBlendSB;
	//z write
	static GFXStateBlock* mZWriteFalseSB;
	//z enable
	static GFXStateBlock* mZTrueSB;
	static GFXStateBlock* mZFalseSB;
	//clear
	static GFXStateBlock* mClearSB;
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




#endif
