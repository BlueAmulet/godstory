//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _MATERIALS_PROCESSEDCUSTOMMATERIAL_H_
#define _MATERIALS_PROCESSEDCUSTOMMATERIAL_H_

#include "processedShaderMaterial.h"

/// Custom processed material. 
///
/// @note Mostly just copy/paste from CustomMaterial
class ProcessedCustomMaterial : public ProcessedShaderMaterial
{
   /// Texture flags
   U32            mFlags[CustomMaterial::MAX_TEX_PER_PASS];

   /// How many textures we have
   U32            mMaxTex;
public:
   ProcessedCustomMaterial(Material &mat);
   virtual bool setupPass(SceneGraphData& sgData, U32 pass);
   virtual void init(SceneGraphData& sgData, GFXVertexFlags vertFlags);
   virtual bool hasCubemap(U32 pass);
   virtual void setTextureStages(SceneGraphData &sgData, U32 pass );
   virtual void cleanup(U32 pass);

protected:
   /// @name Internal functions
   ///
   /// @{
   virtual bool setupPassInternal(SceneGraphData& sgData, U32 pass);
   /// Actually does pass setup
   virtual void setupSubPass(SceneGraphData& sgData, U32 pass);
   bool setNextRefractPass(bool refractOn, U32 pass);

   virtual void setStageData();
   /// @}
private:
	//alpha blend
	static GFXStateBlock* mAlphaTrueSB;
	static GFXStateBlock* mAlphaFalseSB;
	//z write
	static GFXStateBlock* mZWriteTrueSB;
	static GFXStateBlock* mZWriteFalseSB;
	//alpha test
	//static GFXStateBlock* mAlphaTestTrueSB;
	static GFXStateBlock* mAlphaRef0SB;//ref = 0
	static GFXStateBlock* mAlphaRef1SB;//ref = 1
	static GFXStateBlock* mAlphaRef20SB;//ref = 20
	static GFXStateBlock* mAlphaTestFalseSB;
	//cull mode
	static GFXStateBlock* mCullNoneSB;
	static GFXStateBlock* mCullStoreSB;
	//cleanup
	static GFXStateBlock* mCleanupSB;
public:
	//@state block
	//@{
	static void initsb();
	static void shutdown();
	static void resetStateBlock();
	static void releaseStateBlock();
	//@}
};

#endif