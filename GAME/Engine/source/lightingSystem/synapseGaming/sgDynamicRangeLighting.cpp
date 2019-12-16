//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "gfx/gfxDevice.h"
#include "gfx/primBuilder.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "lightingSystem/synapseGaming/sgDynamicRangeLighting.h"
#include "lightingSystem/synapseGaming/sgFormatManager.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "sceneGraph/sceneGraph.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
#include "terrain/sky.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* sgDRLSurfaceChain::mSetBloomSB = NULL;
GFXStateBlock* sgDRLSurfaceChain::mClearBloomSB = NULL;
GFXStateBlock* sgDRLSurfaceChain::mSetSB = NULL;
GFXStateBlock* sgDRLSurfaceChain::mSetAlphaLerpSB = NULL;
GFXStateBlock* sgDRLSurfaceChain::mSetAlphaFalseSB = NULL;
GFXStateBlock* sgDRLSurfaceChain::mAddrClampSB = NULL;
GFXStateBlock* sgDRLSurfaceChain::mClearSB = NULL;
GFXStateBlock* sgDRLSurfaceChain::mSetDrlSB = NULL;
GFXStateBlock* sgDRLSurfaceChain::mClearDrlSB = NULL;

extern GFXTexHandle *gTexture;

F32 sgDRLSurfaceChain::SceneIntensity = 0.01f;
F32 sgDRLSurfaceChain::GlowIntensity = 0.8f;
F32 sgDRLSurfaceChain::HighlightIntensity = 1.0f;
F32 sgDRLSurfaceChain::HighlightThreshold = 0.8f;
F32 sgDRLSurfaceChain::BloomScale = 0.25f;
F32 sgDRLSurfaceChain::BloomOffset = 1.0f;
F32 powPower = 1.0f;

bool sgDRLSurfaceChain::sgResetTexture = true;

#ifdef NTJ_GUI
IMPLEMENT_CONOBJECT(sgGuiTexTestCtrl);

void sgGuiTexTestCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	GFX->getDrawUtil()->clearBitmapModulation();
	RectI rect(offset, getExtent());

	if(gTexture && ((GFXTextureObject *)(*gTexture)))
		GFX->getDrawUtil()->drawBitmapStretch((*gTexture), rect);

	renderChildControls(offset, updateRect);
}

void sgGuiTexTestCtrl::initPersistFields()
{
   Parent::initPersistFields();
   addGroup("Misc");		
   addField("textureLevel", TypeS32, Offset(sgTextureLevel, sgGuiTexTestCtrl));
   endGroup("Misc");		
}

#endif

void sgDRLSystem::sgPrepSystem(const Point2I &offset, const Point2I &extent)
{
	sgPrepChain(offset, extent);

	sgDidPrep = sgLightManager::sgAllowDRLSystem() && (sgSurfaceChain.size() > 0);
	if(!sgDidPrep)
		return;

	// Ben's change - unknown cause/effect (moved from GuiTSCtrl)...
	GFX->clear( GFXClearTarget, ColorI( 255, 255, 0 ), 1.0f, 0 );

   // I decided to go with the backbuffer copy approach even though
   // it is slower because it is more reliable, scales into FSAA, and
   // won't have issues when other fullscreen effects go into place - MDF [04/01/08]
   //if (GFX->getMultisampleLevel() <= 0)
   //{
   //   // to avoid the back buffer copy just use
   //   // the copy-to texture as the surface...
   //   GFX->pushActiveRenderTarget();
   //   GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
   //   myTarg->attachTexture(GFXTextureTarget::Color0, sgSurfaceChain[0] );
   //   myTarg->attachTexture(GFXTextureTarget::DepthStencil, GFXTextureTarget::sDefaultDepthStencil);
   //   GFX->setActiveRenderTarget( myTarg );
   //}
}

void sgDRLSystem::sgRenderSystem()
{
#ifdef POWER_OS_XENON
   return;
#endif

	if(!sgDidPrep)
		return;

	sgDidPrep = false;

   // I decided to go with the backbuffer copy approach even though
   // it is slower because it is more reliable, scales into FSAA, and
   // won't have issues when other fullscreen effects go into place - MDF [04/01/08]
   //if (GFX->getMultisampleLevel() <= 0)
   //{
   //   // put back the back buffer...
   //   GFX->popActiveRenderTarget();
   //}

	sgRenderChain();
	if(!sgLightManager::UseScreenBloom())
		sgRenderDRL();
}

void sgDRLSurfaceChain::sgPrepChain(const Point2I &offset, const Point2I &extent)
{
	if(!sgLightManager::sgAllowDRLSystem())
	{
		sgDestroyChain();
		return;
	}

	if (sgResetTexture)
	{
		sgDestroyChain();
	}

	sgFormatManager::prepFormats();

	//AssertFatal((sgSurfaceSize.size() == sgSurfaceChain.size()), "DRL:> Ouch!");

	//Point2I framesize = GFX->getVideoMode().resolution;

	// this doesn't affect chain...
	sgOffset = offset;
	sgExtent = extent;
	Point2I cursize = gClientSceneGraph->getDisplayTargetResolution();

	if(sgSurfaceSize.size() > 0)
	{
		Point2I &size = sgSurfaceSize[0];
		if((size.x != cursize.x) || (size.y != cursize.y))
			sgDestroyChain();
	}

	if(sgCachedIsHDR != sgLightManager::sgAllowFullHighDynamicRangeLighting())
	{
		sgDestroyChain();
	}

	if(sgSurfaceSize.size() == 0)
	{
		if(!sgLightManager::UseScreenBloom())
		{
			sgCachedIsHDR = sgLightManager::sgAllowFullHighDynamicRangeLighting();

			//Sim::findObject("AlphaBloomShader", sgAlphaBloom);
			static bool initialize = false;
			if(!initialize)
			{
				Sim::findObject("DownSample4x4Shader", sgDownSample4x4);
				Sim::findObject("DownSample4x4FinalShader", sgDownSample4x4Final);
				Sim::findObject("DownSample4x4BloomClampShader", sgDownSample4x4BloomClamp);
				Sim::findObject("BloomBlurShader", sgBloomBlur);
				Sim::findObject("DRLFullShader", sgDRLFull);
				Sim::findObject("DRLOnlyBloomToneShader", sgDRLOnlyBloomTone);

				if(//(!sgAlphaBloom) || (!sgAlphaBloom->shader) ||
					(!sgDownSample4x4) || (!sgDownSample4x4->getShader()) ||
					(!sgDownSample4x4Final) || (!sgDownSample4x4Final->getShader()) ||
					(!sgDownSample4x4BloomClamp) || (!sgDownSample4x4BloomClamp->getShader()) ||
					(!sgBloomBlur) || (!sgBloomBlur->getShader()) ||
					(!sgDRLFull) || (!sgDRLFull->getShader()) ||
					(!sgDRLOnlyBloomTone) || (!sgDRLOnlyBloomTone->getShader()))
					return;

				initialize = true;
			}

			sgSurfaceSize.increment(1);
			sgSurfaceSize[0] = cursize;
			Point2I lastsize = cursize;

			while(1)
			{
				sgSurfaceSize.increment(1);
				Point2I &size = sgSurfaceSize.last();

				size.x = lastsize.x / sgdrlscSampleWidth;
				size.y = lastsize.y / sgdrlscSampleWidth;

				if(size.x < 1)
					size.x = 1;
				if(size.y < 1)
					size.y = 1;

				if((size.x * size.y) <= 1)
					break;

				lastsize = size;
			}

			// use the sizes to create the handles...
			sgSurfaceChain.setSize(sgSurfaceSize.size());
			for(U32 i=0; i<sgSurfaceSize.size(); i++)
			{
				Point2I &size = sgSurfaceSize[i];

				Con::printf("sgDRLSurfaceChain::sgPrepChain - allocating %dx%dpx surface.", size.x, size.y);

				// this is designed to avoid too many float calcs, but having the whole chain
				// as float targets could be a good idea, hmm...
				if((i == 0) && (sgLightManager::sgAllowFullHighDynamicRangeLighting()) && (GFX->getPixelShaderVersion() >= 3.0))
				{
					sgSurfaceChain[i] = GFXTexHandle(size.x, size.y,
						sgFormatManager::sgHDRTextureFormat, &DRLTargetTextureProfile);
				}
				else
				{
					sgSurfaceChain[i] = GFXTexHandle(size.x, size.y,
						sgFormatManager::sgDRLTextureFormat, &DRLTargetTextureProfile);
				}
			}

			gTexture = &sgSurfaceChain[1];

			sgBloom = GFXTexHandle(sgSurfaceSize[sgdlrscBloomIndex].x, sgSurfaceSize[sgdlrscBloomIndex].y,
				sgFormatManager::sgDRLTextureFormat, &DRLTargetTextureProfile);
			sgBloom2 = GFXTexHandle(sgSurfaceSize[sgdlrscBloomIndex].x, sgSurfaceSize[sgdlrscBloomIndex].y,
				sgFormatManager::sgDRLTextureFormat, &DRLTargetTextureProfile);
			sgToneMap = GFXTexHandle("gameres/data/environments/lighting/sgToneMap", &GFXDefaultStaticDiffuseProfile);
			sgGrayMap = GFXTexHandle("gameres/data/environments/lighting/sgGrayMap", &GFXDefaultStaticDiffuseProfile);
			sgDRLViewMap = GFXTexHandle("gameres/data/environments/lighting/sgDRLViewMap", &GFXDefaultStaticDiffuseProfile);
		}
		else
		{
			sgSurfaceSize.increment(1);
			sgSurfaceSize[0] = cursize;
			sgSurfaceChain.setSize(3);
			sgSurfaceChain[0] = GFXTexHandle(cursize.x, cursize.y,
				GFXFormatR8G8B8A8, &DRLTargetTextureProfile);
			sgSurfaceChain[1] = GFXTexHandle(cursize.x, cursize.y,
				GFXFormatR8G8B8A8, &DRLTargetTextureProfile);
			sgSurfaceChain[2] = GFXTexHandle( cursize.x, cursize.y,
				GFXFormatR8G8B8A8, &DRLTargetTextureProfile);

			gTexture = &sgSurfaceChain[2];

			static bool initialize = false;
			if(!initialize)
			{
				if ( !Sim::findObject( "ColorEdgeDetect", sgColorEdgeDetect ) )
					Con::warnf("ColorEdgeDetect - failed to locate ColorEdgeDetect shader ColorEdgeDetectBlendData!");

				if ( !Sim::findObject( "ColorDownFilter4", sgColorDownFilter4 ) )
					Con::warnf("ColorDownFilter4 - failed to locate ColorDownFilter4 shader ColorDownFilter4BlendData!");

				if ( !Sim::findObject( "ColorBloomH", sgColorBloomH ) )
					Con::warnf("ColorBloomH - failed to locate ColorBloomH shader ColorBloomHBlendData!");

				if ( !Sim::findObject( "ColorBloomV", sgColorBloomV ) )
					Con::warnf("ColorBloomV - failed to locate ColorBloomV shader ColorBloomVBlendData!");


				if ( !Sim::findObject( "ColorCombine4", sgColorCombine4 ) )
					Con::warnf("ColorCombine4 - failed to locate ColorCombine4 shader ColorCombine4BlendData!");

				initialize = true;
			}

		}
		sgResetTexture = false;
	}
}

void sgDRLSurfaceChain::sgRenderChain()
{
	if(sgLightManager::UseScreenBloom())
	{
		if(sgSurfaceChain.size() <= 0)
			return;

		Point2I corner(0, 0);
		sgSurfaceChain[0]->readBackBuffer(corner);

		GFX->pushActiveRenderTarget();

		// start copy and down-sample...
		RectI rect(-1, 1, 1, -1);
		GFXTexHandle *lasttexture = &sgSurfaceChain[0];
		GFX->getDrawUtil()->clearBitmapModulation();
#ifdef STATEBLOCK
		AssertFatal(mSetBloomSB, "sgDRLSurfaceChain::sgRenderChain -- mSetBloomSB cannot be NULL.");
		mSetBloomSB->apply();
#else
		GFX->setCullMode(GFXCullNone);
		GFX->setLightingEnable(false);
		GFX->setAlphaBlendEnable(false);
        
		GFX->setTextureStageMagFilter(0, GFXTextureFilterLinear);
		GFX->setTextureStageMinFilter(0, GFXTextureFilterPoint);
		GFX->setTextureStageMipFilter(0, GFXTextureFilterLinear);
		GFX->setTextureStageMagFilter(1, GFXTextureFilterLinear);
		GFX->setTextureStageMinFilter(1, GFXTextureFilterPoint);
		GFX->setTextureStageMipFilter(1, GFXTextureFilterLinear);

        GFX->setZEnable(false);
        GFX->setZWriteEnable(false);
		GFX->setTextureStageColorOp(0, GFXTOPDisable);
		GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
		GFX->setTextureStageAddressModeV(0, GFXAddressClamp);
		GFX->setTextureStageColorOp(1, GFXTOPDisable);
		GFX->setTextureStageAddressModeU(1, GFXAddressClamp);
		GFX->setTextureStageAddressModeV(1, GFXAddressClamp);

		GFX->setTextureStageColorOp(2, GFXTOPDisable);
#endif
		GFX->setAlphaTestEnable(true);
		GFX->setAlphaFunc(GFXCmpGreaterEqual);
		GFX->setAlphaRef(1); 
        GFX->setShader(sgColorEdgeDetect->getShader());
		GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
		myTarg->attachTexture(GFXTextureTarget::Color0, sgSurfaceChain[1] );
		GFX->setActiveRenderTarget( myTarg );
		GFX->clear(GFXClearTarget, ColorI(0,0,0,0), 1.0f, 0 );
		GFX->setTexture(0, (*lasttexture));
		Point4F tempf = Point4F(1.0f/sgSurfaceSize[0].x, 1.0f/sgSurfaceSize[0].y, 0.0f, 0.0f);
        GFX->setPixelShaderConstF(2, (float*)&tempf, 1);
		tempf = Point4F(SceneIntensity, GlowIntensity, HighlightIntensity, HighlightThreshold);
		GFX->setPixelShaderConstF(1, (float*)&tempf, 1);
		ColorF glowcolor = gClientSceneGraph->getCurrentSky()->GetGlowColor();
        GFX->setPixelShaderConstF(3, (float*)&glowcolor, 1);
        mVertex[0].point = Point4F(-0.5f, -0.5f, 0.0f, 1.0f);
        mVertex[0].texCoord = Point2F(0.0f, 0.0f);
		mVertex[1].point = Point4F(sgSurfaceSize[0].x-0.5f, -0.5f, 0.0f, 1.0f);
		mVertex[1].texCoord = Point2F(1.0f, 0.0f);
		mVertex[2].point = Point4F(-0.5f, sgSurfaceSize[0].y-0.5f, 0.0f, 1.0f);
		mVertex[2].texCoord = Point2F(0.0f, 1.0f);
		mVertex[3].point = Point4F(sgSurfaceSize[0].x-0.5f, sgSurfaceSize[0].y-0.5f, 0.0f, 1.0f);
		mVertex[3].texCoord = Point2F(1.0f, 1.0f);

		mVertBuff.set(GFX, 4, GFXBufferTypeVolatile);
		GFXVertexP4WT *verts = mVertBuff.lock();
		dMemcpy( verts, &mVertex[0], 4 * sizeof(GFXVertexP4WT) );
		mVertBuff.unlock(); 
		GFX->setVertexBuffer( mVertBuff );
		GFX->drawPrimitive( GFXTriangleStrip, 0, 2 );
        GFX->setAlphaTestEnable(false);
		/*PrimBuild::color(ColorF(0.0f, 0.0f, 0.0f));
		PrimBuild::begin(GFXTriangleStrip, 4);
		PrimBuild::texCoord2f(0.0f, 0.0f);
		PrimBuild::vertex2f(rect.point.x, rect.point.y);
		PrimBuild::texCoord2f(1.0f, 0.0f);
		PrimBuild::vertex2f(rect.extent.x, rect.point.y);
		PrimBuild::texCoord2f(0.0f, 1.0f);
		PrimBuild::vertex2f(rect.point.x, rect.extent.y);
		PrimBuild::texCoord2f(1.0f, 1.0f);
		PrimBuild::vertex2f(rect.extent.x, rect.extent.y);
		PrimBuild::end(false);*/

		lasttexture = &sgSurfaceChain[1];

        GFX->setShader(sgColorDownFilter4->getShader());
        myTarg->attachTexture(GFXTextureTarget::Color0, sgSurfaceChain[2] );
		GFX->setActiveRenderTarget( myTarg );
		GFX->clear(GFXClearTarget, ColorI(0,0,0,0), 1.0f, 0 );
		GFX->setTexture(0, (*lasttexture));

		mVertex[0].point = Point4F(-0.5f, -0.5f, 0.0f, 1.0f);
		mVertex[0].texCoord = Point2F(0.0f, 0.0f);
		mVertex[1].point = Point4F((sgSurfaceSize[0].x + 0.5f)*BloomScale-0.5f, -0.5f, 0.0f, 1.0f);
		mVertex[1].texCoord = Point2F(1.0f, 0.0f);
		mVertex[2].point = Point4F(-0.5f, (sgSurfaceSize[0].y+ 0.5f)*BloomScale-0.5f, 0.0f, 1.0f);
		mVertex[2].texCoord = Point2F(0.0f, 1.0f);
		mVertex[3].point = Point4F((sgSurfaceSize[0].x+ 0.5f)*BloomScale-0.5f, (sgSurfaceSize[0].y+ 0.5f)*BloomScale-0.5f, 0.0f, 1.0f);
		mVertex[3].texCoord = Point2F(1.0f, 1.0f);

		//mVertBuff.set(GFX, 4, GFXBufferTypeVolatile);
		verts = mVertBuff.lock();
		dMemcpy( verts, &mVertex[0], 4 * sizeof(GFXVertexP4WT) );
		mVertBuff.unlock(); 
		GFX->setVertexBuffer( mVertBuff );
		GFX->drawPrimitive( GFXTriangleStrip, 0, 2 );
        

		///*PrimBuild::color(ColorF(1.0f, 1.0f, 1.0f));
		//PrimBuild::begin(GFXTriangleStrip, 4);
		//PrimBuild::texCoord2f(0.0f, 0.0f);
		//PrimBuild::vertex2f(-1, 1);
		//PrimBuild::texCoord2f(1.0f, 0.0f);
		//PrimBuild::vertex2f(-0.5f , 1);
		//PrimBuild::texCoord2f(0.0f, 1.0f);
		//PrimBuild::vertex2f(-1, 0.5f );
		//PrimBuild::texCoord2f(1.0f, 1.0f);
		//PrimBuild::vertex2f(-0.5f , 0.5f );
		//PrimBuild::end(false);*/

		lasttexture = &sgSurfaceChain[2];

		for(U32 i=1; i<sgSurfaceChain.size(); i++)
		{
			// final sample?
			if(i == 1)
			{
				GFX->setShader(sgColorBloomH->getShader());
			}
			else if(i == 2)
			{
				GFX->setShader(sgColorBloomV->getShader());
			}
        
			myTarg->attachTexture(GFXTextureTarget::Color0, sgSurfaceChain[i] );
			GFX->setActiveRenderTarget( myTarg );
            GFX->clear(GFXClearTarget, ColorI(0,0,0,0), 1.0f, 0 );
			GFX->setTexture(0, (*lasttexture));
			/*PrimBuild::color(ColorF(1.0f, 1.0f, 1.0f));
			PrimBuild::begin(GFXTriangleStrip, 4);
			PrimBuild::texCoord2f(0.0f, 0.0f);
			PrimBuild::vertex2f(-1, 1);
			PrimBuild::texCoord2f(0.25f, 0.0f);
			PrimBuild::vertex2f(-0.5f , 1);
			PrimBuild::texCoord2f(0.0f, 0.25f);
			PrimBuild::vertex2f(-1, 0.5f );
			PrimBuild::texCoord2f(0.25f, 0.25f);
			PrimBuild::vertex2f(-0.5f , 0.5f );
			PrimBuild::end(false);*/

			mVertex[0].point = Point4F(-0.5f, -0.5f, 0.0f, 1.0f);
			mVertex[0].texCoord = Point2F(0.0f, 0.0f);
			mVertex[1].point = Point4F((sgSurfaceSize[0].x + 0.5f)*BloomScale-0.5f, -0.5f, 0.0f, 1.0f);
			mVertex[1].texCoord = Point2F(BloomScale, 0.0f);
			mVertex[2].point = Point4F(-0.5f, (sgSurfaceSize[0].y+ 0.5f)*BloomScale-0.5f, 0.0f, 1.0f);
			mVertex[2].texCoord = Point2F(0.0f, BloomScale);
			mVertex[3].point = Point4F((sgSurfaceSize[0].x+ 0.5f)*BloomScale-0.5f, (sgSurfaceSize[0].y+ 0.5f)*BloomScale-0.5f, 0.0f, 1.0f);
			mVertex[3].texCoord = Point2F(BloomScale, BloomScale);

			//mVertBuff.set(GFX, 4, GFXBufferTypeVolatile);
			verts = mVertBuff.lock();
			dMemcpy( verts, &mVertex[0], 4 * sizeof(GFXVertexP4WT) );
			mVertBuff.unlock(); 
			GFX->setVertexBuffer( mVertBuff );
			GFX->drawPrimitive( GFXTriangleStrip, 0, 2 );

			lasttexture = &sgSurfaceChain[i];
		}

		GFX->popActiveRenderTarget();

		tempf = Point4F(1.0f/BloomScale, 0.0f, 0.0f, 0.0f);
		GFX->setPixelShaderConstF(3, (float*)&tempf, 1);

		GFX->setShader(sgColorCombine4->getShader());

		GFX->setTexture(0, sgSurfaceChain[0]);
		GFX->setTexture(1, (*lasttexture));

        GFX->setAlphaBlendEnable(false);

		mVertex[0].point = Point4F(-0.5f, -0.5f, 0.0f, 1.0f);
		mVertex[0].texCoord = Point2F(0.0f, 0.0f);
		mVertex[1].point = Point4F(sgSurfaceSize[0].x-0.5f, -0.5f, 0.0f, 1.0f);
		mVertex[1].texCoord = Point2F(BloomScale, 0.0f);
		mVertex[2].point = Point4F(-0.5f, sgSurfaceSize[0].y-0.5f, 0.0f, 1.0f);
		mVertex[2].texCoord = Point2F(0.0f, BloomScale);
		mVertex[3].point = Point4F(sgSurfaceSize[0].x-0.5f, sgSurfaceSize[0].y-0.5f, 0.0f, 1.0f);
		mVertex[3].texCoord = Point2F(BloomScale, BloomScale);

		//mVertBuff.set(GFX, 4, GFXBufferTypeVolatile);
		verts = mVertBuff.lock();
		dMemcpy( verts, &mVertex[0], 4 * sizeof(GFXVertexP4WT) );
		mVertBuff.unlock(); 
		GFX->setVertexBuffer( mVertBuff );
		GFX->drawPrimitive( GFXTriangleStrip, 0, 2 );

		//PrimBuild::color(ColorF(1.0f, 1.0f, 1.0f));
		//PrimBuild::begin(GFXTriangleStrip, 4);
		//PrimBuild::texCoord2f(0.0f, 0.0f);
		//PrimBuild::vertex2f(rect.point.x, rect.point.y);
		////PrimBuild::vertex2f(-1, 1);
		//PrimBuild::texCoord2f(0.25f, 0.0f);
		//PrimBuild::vertex2f(rect.extent.x, rect.point.y);
		////PrimBuild::vertex2f(-0.5f , 1);
		//PrimBuild::texCoord2f(0.0f, 0.25f);
		//PrimBuild::vertex2f(rect.point.x, rect.extent.y);
		////PrimBuild::vertex2f(-1, 0.5f );
		//PrimBuild::texCoord2f(0.25f, 0.25f);
		//PrimBuild::vertex2f(rect.extent.x, rect.extent.y);
		////PrimBuild::vertex2f(-0.5f , 0.5f );
		//PrimBuild::end(false);
#ifdef STATEBLOCK
		AssertFatal(mClearBloomSB, "sgDRLSurfaceChain::sgRenderChain -- mClearBloomSB cannot be NULL.");
		mClearBloomSB->apply();
		for(S32 i=7; i>=0; i--)
		{
			GFX->setTexture(i, NULL);
		}
#else
		for(S32 i=7; i>=0; i--)
		{
			GFX->setTexture(i, NULL);
			GFX->setTextureStageColorOp(i, GFXTOPDisable);
		}

        GFX->setZEnable(true);
        GFX->setZWriteEnable(true);
#endif

		GFX->setTextureStageMagFilter(0, GFXTextureFilterNone);
		GFX->setTextureStageMinFilter(0, GFXTextureFilterNone);
		GFX->setTextureStageMipFilter(0, GFXTextureFilterNone);
		GFX->setTextureStageMagFilter(1, GFXTextureFilterNone);
		GFX->setTextureStageMinFilter(1, GFXTextureFilterNone);
		GFX->setTextureStageMipFilter(1, GFXTextureFilterNone);
	}
	else
	{
		AssertFatal((sgSurfaceSize.size() == sgSurfaceChain.size()), "DRL:> Ouch!");

		if(sgSurfaceChain.size() <= 0)
			return;

		// I decided to go with the backbuffer copy approach even though
		// it is slower because it is more reliable, scales into FSAA, and
		// won't have issues when other fullscreen effects go into place - MDF [04/01/08]
		//if (GFX->getMultisampleLevel() > 0)
		//{
		Point2I corner(0, 0);
		sgSurfaceChain[0]->readBackBuffer(corner);
		//}

		GFX->pushActiveRenderTarget();

		// start copy and down-sample...
		RectI rect(-1, 1, 1, -1);
		GFXTexHandle *lasttexture = &sgSurfaceChain[0];
		GFX->getDrawUtil()->clearBitmapModulation();
#ifdef STATEBLOCK
		AssertFatal(mSetSB, "sgDRLSurfaceChain::sgRenderChain -- mSetSB cannot be NULL.");
		mSetSB->apply();
		GFX->setTexture(1, sgDRLViewMap);
#else
		GFX->setCullMode(GFXCullNone);
		GFX->setLightingEnable(false);
		GFX->setAlphaBlendEnable(false);

        GFX->setZEnable(false);
        GFX->setZWriteEnable(false);
		GFX->setTextureStageColorOp(0, GFXTOPModulate);
		GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
		GFX->setTextureStageAddressModeV(0, GFXAddressClamp);
		GFX->setTextureStageColorOp(1, GFXTOPModulate);
		GFX->setTextureStageAddressModeU(1, GFXAddressClamp);
		GFX->setTextureStageAddressModeV(1, GFXAddressClamp);
		GFX->setTexture(1, sgDRLViewMap);
		GFX->setTextureStageColorOp(2, GFXTOPDisable);
#endif



		Point4F temp(sgLightManager::sgBloomCutOff, sgLightManager::sgBloomCutOff,
			sgLightManager::sgBloomCutOff, sgLightManager::sgBloomCutOff);
		GFX->setPixelShaderConstF(1, temp, 1);

		temp = Point4F(sgLightManager::sgBloomAmount, sgLightManager::sgBloomAmount,
			sgLightManager::sgBloomAmount, sgLightManager::sgBloomAmount);
		GFX->setPixelShaderConstF(2, temp, 1);

		// setup blend for main samples...
		F32 blend = 1.0;
		temp = Point4F(blend, (1.0 - blend), 0, 0);
		GFX->setPixelShaderConstF(3, temp, 1);

		for(U32 i=1; i<sgSurfaceChain.size(); i++)
		{
			// final sample?
			if(i == (sgSurfaceChain.size() - 1))
			{
				// setup blend for final sample...
				// give a gradual increase/decrease...
				// this is frame based, but the min (0.05)
				// val is dependent on integer buffer precision
				// and already is a fast change, so making it
				// time based is a wash...
				F32 blend = 0.05f;
				temp = Point4F(blend, (1.0 - blend), 0, 1.0);
				GFX->setPixelShaderConstF(3, temp, 1);
#ifdef STATEBLOCK
				AssertFatal(mSetAlphaLerpSB, "sgDRLSurfaceChain::sgRenderChain -- mSetAlphaLerpSB cannot be NULL.");
				mSetAlphaLerpSB->apply();
#else
				GFX->setAlphaBlendEnable(true);
				GFX->setSrcBlend(GFXBlendSrcAlpha);
				GFX->setDestBlend(GFXBlendInvSrcAlpha);
#endif
				sgDownSample4x4Final->getShader()->process();
			}
			else if(i == 1)// first sample?
				sgDownSample4x4BloomClamp->getShader()->process();
			else
				sgDownSample4x4->getShader()->process();

			GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
			myTarg->attachTexture(GFXTextureTarget::Color0, sgSurfaceChain[i] );
			//myTarg->attachTexture(GFXTextureTarget::DepthStencil, GFXTextureTarget::sDefaultDepthStencil );
			GFX->setActiveRenderTarget( myTarg );

			GFX->setTexture(0, (*lasttexture));

			// this stuff should be in a vertex buffer?
			Point2F offset;
			offset.x = 1.0f;// + (1.0f / F32(sgSurfaceSize[i].x));
			offset.y = 1.0f;// + (1.0f / F32(sgSurfaceSize[i].y));

			Point2I &lastsize = sgSurfaceSize[i-1];
			Point4F stride((offset.x / F32(lastsize.x-1)), (offset.y / F32(lastsize.y-1)), 0.0f, 0.0f);
			GFX->setVertexShaderConstF(0, stride, 1);
			GFX->setPixelShaderConstF(0, stride, 1);

			PrimBuild::color(ColorF(1.0f, 1.0f, 1.0f));
			PrimBuild::begin(GFXTriangleStrip, 4);
			PrimBuild::texCoord2f(0.0f, 0.0f);
			PrimBuild::vertex2i(rect.point.x, rect.point.y);
			PrimBuild::texCoord2f(offset.x, 0.0f);
			PrimBuild::vertex2i(rect.extent.x, rect.point.y);
			PrimBuild::texCoord2f(0.0f, offset.y);
			PrimBuild::vertex2i(rect.point.x, rect.extent.y);
			PrimBuild::texCoord2f(offset.x, offset.y);
			PrimBuild::vertex2i(rect.extent.x, rect.extent.y);
			PrimBuild::end(false);

			if((i == sgdlrscBloomIndex) && (!sgLightManager::sgAllowFullDynamicRangeLighting()))
				break;

			lasttexture = &sgSurfaceChain[i];
		}
#ifdef STATEBLOCK
		AssertFatal(mSetAlphaFalseSB, "sgDRLSurfaceChain::sgRenderChain -- mSetAlphaFalseSB cannot be NULL.");
		mSetAlphaFalseSB->apply();
#else
		// set back...
		GFX->setAlphaBlendEnable(false);
		GFX->setSrcBlend(GFXBlendOne);
		GFX->setDestBlend(GFXBlendZero);
#endif


		if(sgLightManager::sgAllowBloom())
		{
			sgBloomBlur->getShader()->process();
			GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
			myTarg->attachTexture(GFXTextureTarget::Color0, sgBloom );
			//myTarg->attachTexture(GFXTextureTarget::DepthStencil, GFXTextureTarget::sDefaultDepthStencil );
			GFX->setActiveRenderTarget( myTarg );

			GFX->setTexture(0, sgSurfaceChain[sgdlrscBloomIndex]);
			//GFX->setTextureBorderColor(0, ColorI(0, 0, 0, 0));

#ifdef STATEBLOCK
			AssertFatal(mAddrClampSB, "sgDRLSurfaceChain::sgRenderChain -- mAddrClampSB cannot be NULL.");
			mAddrClampSB->apply();
#else
			GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
			GFX->setTextureStageAddressModeV(0, GFXAddressClamp);
#endif
			Point2F offset;
			Point2I &lastsize = sgSurfaceSize[sgdlrscBloomIndex];
			offset.x = 1.0f;// + (1.0f / F32(lastsize.x));
			offset.y = 1.0f;// + (1.0f / F32(lastsize.y));

			Point4F stride((offset.x / F32(lastsize.x-1)), (offset.y / F32(lastsize.y-1)), 0.0f, 0.0f);
			GFX->setVertexShaderConstF(0, stride, 1);
			GFX->setPixelShaderConstF(0, stride, 1);

			Point4F temp(sgLightManager::sgBloomSeedAmount, sgLightManager::sgBloomSeedAmount,
				sgLightManager::sgBloomSeedAmount, sgLightManager::sgBloomSeedAmount);
			GFX->setPixelShaderConstF(1, temp, 1);

			PrimBuild::color(ColorF(1.0f, 1.0f, 1.0f));
			PrimBuild::begin(GFXTriangleStrip, 4);
			PrimBuild::texCoord2f(0.0f, 0.0f);
			PrimBuild::vertex2i(rect.point.x, rect.point.y);
			PrimBuild::texCoord2f(offset.x, 0.0f);
			PrimBuild::vertex2i(rect.extent.x, rect.point.y);
			PrimBuild::texCoord2f(0.0f, offset.y);
			PrimBuild::vertex2i(rect.point.x, rect.extent.y);
			PrimBuild::texCoord2f(offset.x, offset.y);
			PrimBuild::vertex2i(rect.extent.x, rect.extent.y);
			PrimBuild::end(false);

			//sgBloomBlur->shader->process();
			myTarg->attachTexture(GFXTextureTarget::Color0, sgBloom2 );
			GFX->setActiveRenderTarget( myTarg );

			GFX->setTexture(0, sgBloom);
			//GFX->setTextureBorderColor(0, ColorI(0, 0, 0, 0));
#ifdef STATEBLOCK
			AssertFatal(mAddrClampSB, "sgDRLSurfaceChain::sgRenderChain -- mAddrClampSB cannot be NULL.");
			mAddrClampSB->apply();
#else
			GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
			GFX->setTextureStageAddressModeV(0, GFXAddressClamp);
#endif
			PrimBuild::color(ColorF(1.0f, 1.0f, 1.0f));
			PrimBuild::begin(GFXTriangleStrip, 4);
			PrimBuild::texCoord2f(0.0f, 0.0f);
			PrimBuild::vertex2i(rect.point.x, rect.point.y);
			PrimBuild::texCoord2f(offset.x, 0.0f);
			PrimBuild::vertex2i(rect.extent.x, rect.point.y);
			PrimBuild::texCoord2f(0.0f, offset.y);
			PrimBuild::vertex2i(rect.point.x, rect.extent.y);
			PrimBuild::texCoord2f(offset.x, offset.y);
			PrimBuild::vertex2i(rect.extent.x, rect.extent.y);
			PrimBuild::end(false);
		}
		else
		{
			// makes sure texture doesn't affect DRL/Bloom composite shader...
			GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
			myTarg->attachTexture(GFXTextureTarget::Color0, sgBloom2 );
			GFX->setActiveRenderTarget( myTarg );
			GFX->clear(GFXClearTarget, ColorI(0, 0, 0, 0), 1.0f, 0);
		}

		if(!sgLightManager::sgAllowFullDynamicRangeLighting())
		{
			// makes sure texture doesn't affect DRL/Bloom composite shader...
			GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
			myTarg->attachTexture(GFXTextureTarget::Color0, sgSurfaceChain[sgSurfaceChain.size()-1] );
			GFX->setActiveRenderTarget( myTarg );

			GFX->clear(GFXClearTarget,
				ColorF(0, 0, 0, sgLightManager::sgDRLTarget), 1.0f, 0);
		}

#ifdef STATEBLOCK
		AssertFatal(mClearSB, "sgDRLSurfaceChain::sgRenderChain -- mClearSB cannot be NULL.");
		mClearSB->apply();
#else
        GFX->setZEnable(true);
        GFX->setZWriteEnable(true);

		GFX->setAlphaBlendEnable(false);
#endif

		GFX->popActiveRenderTarget();
	}
}

void sgDRLSurfaceChain::sgRenderDRL()
{
	if(!sgLightManager::sgAllowDRLSystem())
		return;

	if(sgSurfaceChain.size() < 3)
		return;

	// fix me - should use the right sized buffer and map directly to the screen...
	// chances are the original code worked correctly just the viewport wasn't set...
	//RectI rect(-1, 1, 1, -1);
	RectF rect;
   Point2I resolution = GFX->getActiveRenderTarget()->getSize();
	
   rect.extent.x =        (F32(sgOffset.x + sgExtent.x) / F32(resolution.x));
	rect.extent.y = 1.0f - (F32(sgOffset.y + sgExtent.y) / F32(resolution.y));
	rect.extent   = (rect.extent * 2.0) - Point2F(1, 1);
	
   rect.point.x  =                     (F32(sgOffset.x) / F32(resolution.x));
	rect.point.y  =              1.0f - (F32(sgOffset.y) / F32(resolution.y));
	rect.point    = (rect.point * 2.0) - Point2F(1, 1);

	F32 uoff = 0.5f / F32(sgExtent.x);
	F32 voff = 0.5f / F32(sgExtent.y);
	RectF texcoord;
	texcoord.point = Point2F(uoff, voff);

	// yes this is correct! otherwise mapping is on pixels-1...
	texcoord.extent = Point2F(1.0, 1.0) + texcoord.point;

#ifdef STATEBLOCK
	AssertFatal(mSetDrlSB, "sgDRLSurfaceChain::sgRenderDRL -- mSetDrlSB cannot be NULL.");
	mSetDrlSB->apply();
	GFX->setTexture(0, sgSurfaceChain[sgSurfaceChain.size()-1]);
	GFX->setTexture(1, sgBloom2);
	GFX->setTexture(2, sgBloom2);
	GFX->setTexture(3, sgSurfaceChain[0]);
	GFX->setTexture(4, sgSurfaceChain[0]);
	if(sgLightManager::sgAllowToneMapping())
		GFX->setTexture(5, sgToneMap);
	else
		GFX->setTexture(5, sgGrayMap);
#else
	GFX->setCullMode(GFXCullNone);
	GFX->setLightingEnable(false);
	GFX->setAlphaBlendEnable(true);
	GFX->setSrcBlend(GFXBlendOne);
	GFX->setDestBlend(GFXBlendZero);

	GFX->setZEnable(false);

	// intensity texel...
	GFX->setTextureStageColorOp(0, GFXTOPModulate);
	GFX->setTexture(0, sgSurfaceChain[sgSurfaceChain.size()-1]);

	GFX->setTextureStageColorOp(1, GFXTOPModulate);
	GFX->setTexture(1, sgBloom2);
	GFX->setTextureStageColorOp(2, GFXTOPModulate);
	GFX->setTexture(2, sgBloom2);

	GFX->setTextureStageColorOp(3, GFXTOPModulate);
	GFX->setTexture(3, sgSurfaceChain[0]);

	// tone test...
	GFX->setTextureStageColorOp(4, GFXTOPModulate);
	GFX->setTexture(4, sgSurfaceChain[0]);
	GFX->setTextureStageColorOp(5, GFXTOPModulate);
	GFX->setTextureStageAddressModeU(5, GFXAddressClamp);
	GFX->setTextureStageAddressModeV(5, GFXAddressClamp);
	if(sgLightManager::sgAllowToneMapping())
		GFX->setTexture(5, sgToneMap);
	else
		GFX->setTexture(5, sgGrayMap);

	// no more needed...
	GFX->setTextureStageColorOp(6, GFXTOPDisable);
#endif


	if(sgLightManager::sgAllowFullDynamicRangeLighting())
		sgDRLFull->getShader()->process();
	else
		sgDRLOnlyBloomTone->getShader()->process();

	//Point4F temp(intensity, fullrange, 0, sgLightManager::sgDRLMultiplier);
	Point4F temp(sgLightManager::sgDRLMax, sgLightManager::sgDRLMin,
		sgLightManager::sgDRLTarget, sgLightManager::sgDRLMultiplier);
	if(!sgLightManager::sgAllowFullDynamicRangeLighting())
		temp.w = 1.0 / (sgLightManager::sgDRLTarget + 0.0001);
	GFX->setPixelShaderConstF(0, temp, 1);

	// render...
	PrimBuild::color(ColorF(1.0f, 1.0f, 1.0f));
	PrimBuild::begin(GFXTriangleStrip, 4);
	PrimBuild::texCoord2f(texcoord.point.x, texcoord.point.y);
	PrimBuild::vertex2f(rect.point.x, rect.point.y);
	PrimBuild::texCoord2f(texcoord.extent.x, texcoord.point.y);
	PrimBuild::vertex2f(rect.extent.x, rect.point.y);
	PrimBuild::texCoord2f(texcoord.point.x, texcoord.extent.y);
	PrimBuild::vertex2f(rect.point.x, rect.extent.y);
	PrimBuild::texCoord2f(texcoord.extent.x, texcoord.extent.y);
	PrimBuild::vertex2f(rect.extent.x, rect.extent.y);
	PrimBuild::end(false);
#ifdef STATEBLOCK
	AssertFatal(mClearDrlSB, "sgDRLSurfaceChain::sgRenderDRL -- mClearDrlSB cannot be NULL.");
	mClearDrlSB->apply();
	for(S32 i=7; i>=0; i--)
	{
		GFX->setTexture(i, NULL);
	}
#else
	for(S32 i=7; i>=0; i--)
	{
		GFX->setTexture(i, NULL);
		GFX->setTextureStageColorOp(i, GFXTOPDisable);
	}

	GFX->setZEnable(true);
	GFX->setAlphaBlendEnable(false);
#endif
}

void sgDRLSurfaceChain::sgDestroyChain()
{
	// free up the texture resources...
	for(U32 i=0; i<sgSurfaceChain.size(); i++)
		sgSurfaceChain[i] = NULL;

	sgBloom = NULL;
	sgBloom2 = NULL;
	sgToneMap = NULL;
	sgGrayMap = NULL;
	sgDRLViewMap = NULL;
	
	sgSurfaceSize.clear();
	sgSurfaceChain.clear();
}

void sgDRLSurfaceChain::SetGlowConstants(float c1, float c2, float c3, float c4, float c5)
{
	SceneIntensity = c1;
	GlowIntensity = c2;
	HighlightIntensity = c3;
	HighlightThreshold = c4;
	BloomScale = c5;
}

void sgDRLSurfaceChain::ResetTexture(bool data)
{
	sgResetTexture = data;
}



void sgDRLSurfaceChain::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setSamplerState(0, GFXSAMPMagFilter, GFXTextureFilterLinear);
	GFX->setSamplerState(0, GFXSAMPMinFilter, GFXTextureFilterLinear);
	GFX->setSamplerState(0, GFXSAMPMipFilter, GFXTextureFilterLinear);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressClamp);
	GFX->setTextureStageState( 1, GFXTSSColorOp, GFXTOPModulate );
	GFX->setSamplerState(1, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(1, GFXSAMPAddressV, GFXAddressClamp);
	GFX->setTextureStageState( 2, GFXTSSColorOp, GFXTOPDisable );
	GFX->endStateBlock(mSetSB);

	//mClearBloomSB
	GFX->beginStateBlock();
	for(S32 i=7; i>=0; i--)
	{
		GFX->setTextureStageState( i, GFXTSSColorOp, GFXTOPDisable );
	}
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->endStateBlock(mClearBloomSB);

	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressClamp);
	GFX->setTextureStageState( 1, GFXTSSColorOp, GFXTOPModulate );
	GFX->setSamplerState(1, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(1, GFXSAMPAddressV, GFXAddressClamp);
	GFX->setTextureStageState( 2, GFXTSSColorOp, GFXTOPDisable );
	GFX->endStateBlock(mSetSB);

	//mSetAlphaLerpSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->endStateBlock(mSetAlphaLerpSB);

	//mSetAlphaFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendZero);
	GFX->endStateBlock(mSetAlphaFalseSB);

	//mAddrClampSB
	GFX->beginStateBlock();
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressClamp);
	GFX->endStateBlock(mAddrClampSB);

	//mClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->endStateBlock(mClearSB);

	//mSetDrlSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendZero);
	GFX->setRenderState(GFXRSZEnable, false);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(2, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(3, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(4, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(5, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(6, GFXTSSColorOp, GFXTOPDisable );
	GFX->endStateBlock(mSetDrlSB);

	//mClearDrlSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSZEnable, true);
	for(S32 i=7; i>=0; i--)
	{
	GFX->setTextureStageState(i, GFXTSSColorOp, GFXTOPDisable );
	}
	GFX->endStateBlock(mClearDrlSB);
}


void sgDRLSurfaceChain::releaseStateBlock()
{
	if (mSetBloomSB)
	{
		mSetBloomSB->release();
	}

	if (mClearBloomSB)
	{
		mClearBloomSB->release();
	}

	if (mSetSB)
	{
		mSetSB->release();
	}

	if (mSetAlphaLerpSB)
	{
		mSetAlphaLerpSB->release();
	}

	if (mSetAlphaFalseSB)
	{
		mSetAlphaFalseSB->release();
	}

	if (mAddrClampSB)
	{
		mAddrClampSB->release();
	}

	if (mClearSB)
	{
		mClearSB->release();
	}

	if (mSetDrlSB)
	{
		mSetDrlSB->release();
	}

	if (mClearDrlSB)
	{
		mClearDrlSB->release();
	}

}

void sgDRLSurfaceChain::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		mSetBloomSB = new GFXD3D9StateBlock;
		mClearBloomSB = new GFXD3D9StateBlock;
		mSetAlphaLerpSB = new GFXD3D9StateBlock;
		mSetAlphaFalseSB = new GFXD3D9StateBlock;
		mAddrClampSB = new GFXD3D9StateBlock;
		mClearSB = new GFXD3D9StateBlock;
		mSetDrlSB = new GFXD3D9StateBlock;
		mClearDrlSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}

void sgDRLSurfaceChain::shutdown()
{
	SAFE_DELETE(mSetBloomSB);
	SAFE_DELETE(mClearBloomSB);
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mSetAlphaLerpSB);
	SAFE_DELETE(mSetAlphaFalseSB);
	SAFE_DELETE(mAddrClampSB);
	SAFE_DELETE(mClearSB);
	SAFE_DELETE(mSetDrlSB);
	SAFE_DELETE(mClearDrlSB);
}
