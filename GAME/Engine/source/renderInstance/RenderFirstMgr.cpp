//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "renderFirstMgr.h"
#include "materials/sceneData.h"
#include "sceneGraph/sceneGraph.h"
#include "materials/matInstance.h"
#include "gfx/gfxTransformSaver.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
#include "gui/core/guiCanvas.h"

void RenderFirstMgr::render()
{
   // Early out if nothing to draw.
   if(!mElementList.size())
      return;

   PROFILE_SCOPE(RenderFirstMgrRender);

   // Automagically save & restore our viewport and transforms.
   GFXTransformSaver saver;

   static ShaderData *sgR2VBShader = NULL;
   if(!sgR2VBShader)
   {
	   Sim::findObject("R2VBShader", sgR2VBShader);
   }

   static GuiCanvas* sgCanvas = NULL;
   if(!sgCanvas)
   {
	   sgCanvas = dynamic_cast<GuiCanvas*>(Sim::findObject("Canvas"));
   }

   GFX->setZWriteEnable(false);
   GFX->setZEnable(false);
   GFX->setAlphaBlendEnable(false);
   GFX->setAlphaTestEnable(false);

   for( U32 i=0; i<4; i++ )
   {
	   GFX->setTextureStageAddressModeU( i, GFXAddressWrap );
	   GFX->setTextureStageAddressModeV( i, GFXAddressWrap );

	   GFX->setTextureStageMagFilter( i, GFXTextureFilterPoint );
	   GFX->setTextureStageMinFilter( i, GFXTextureFilterPoint );
	   GFX->setTextureStageMipFilter( i, GFXTextureFilterPoint );
   }

   U32 size = mElementList.size();
   for( U32 j=0; j<size; j++)
   {
      RenderInst *ri1 = mElementList[j].inst;

	  if(ri1->SortedIndex == 0) //r2vb
	  {
		  if(sgR2VBShader)
		  {
			  GFX->setShader(sgR2VBShader->getShader());

			  GFX->pushActiveRenderTarget();
			  GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();

			  U32 k;
			  for(k=j;k<size;k++)
			  {
				  RenderInst *ri = mElementList[k].inst;
				  if(ri->SortedIndex == 0)
				  {
					  myTarg->attachTexture(GFXTextureTarget::Color0, ri->vertexTex );
					  myTarg->attachTexture(GFXTextureTarget::DepthStencil, NULL );
					  GFX->setActiveRenderTarget( myTarg );
					  GFX->clear( GFXClearTarget, ColorI( 0, 0, 0, 0 ), 1.0f, 0 );

					  GFX->setVertexBuffer(*ri->vertBuff);
					  GFX->setTexture(0,ri->normLightmap);  //initVertex,initNormal
					  GFX->setTexture(1,ri->miscTex);		//BoneIdx
					  GFX->setTexture(2,ri->lightmap);		//Weight
					  GFX->setTexture(3,ri->reflectTex);	//BoneMx

					  GFX->setPixelShaderConstF( PC_USERDEF1, &ri->boneNum, 1);

					  float xbias = 0.1/ri->vertexTex->getWidth();
					  GFX->setPixelShaderConstF( PC_USERDEF1+1, &xbias, 1);

					  GFX->drawPrimitive(GFXTriangleStrip, 0, 2);
				  }
				  else
					  break;
			  }
			  j = k-1;

			  GFX->popActiveRenderTarget();
		  }
	  }
   }

   GFX->setZEnable(true);
   GFX->setAlphaTestEnable(true);
   GFX->setBaseRenderState();
}



