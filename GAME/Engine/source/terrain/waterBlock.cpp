//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include <d3dx9math.h>
#include "terrain/waterBlock.h"
#include "util/safeDelete.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/sceneGraph.h"
#include "materials/matInstance.h"
#include "core/bitStream.h"
#include "math/mathIO.h"
#include "console/consoleTypes.h"
#include "gui/3d/guiTSControl.h"
#include "gfx/primBuilder.h"
#include "gfx/gfxTransformSaver.h"
#include SHADER_CONSTANT_INCLUDE_FILE
#include "renderInstance/renderInstMgr.h"
#include "sim/netConnection.h"
#include "Gameplay/Data/GraphicsProfile.h"

#include "gfx/D3D9/gfxD3D9Device.h"
#include "gfx/gfxD3D9OcclusionQuery.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"

#include "util/powerStat.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* WaterBlock::mSetQuerySB = NULL;
GFXStateBlock* WaterBlock::mColorWriteTSB = NULL;
GFXStateBlock* WaterBlock::mSetFilterSB = NULL;
GFXStateBlock* WaterBlock::mSetAddr1_1SB = NULL;
GFXStateBlock* WaterBlock::mUnderWater1_1SB = NULL;
GFXStateBlock* WaterBlock::mFogMeshSB = NULL;
GFXStateBlock* WaterBlock::mAlphaVColorSB = NULL;
GFXStateBlock* WaterBlock::mFogMesh2SB = NULL;
GFXStateBlock* WaterBlock::mClear2SB = NULL;
GFXStateBlock* WaterBlock::mClearSB = NULL;
GFXStateBlock* WaterBlock::mZTrueSB = NULL;
GFXStateBlock* WaterBlock::mSetUnderWater = NULL;
GFXStateBlock* WaterBlock::mClearUnderWaterSB = NULL;
GFXStateBlock* WaterBlock::mSetAnimSB = NULL;

#define BLEND_TEX_SIZE 256
#define V_SHADER_PARAM_OFFSET 50

IMPLEMENT_CO_NETOBJECT_V1(WaterBlock);

DECLARE_SERIALIBLE(WaterBlock);

extern bool gEditingMission;

//*****************************************************************************
// WaterBlock
//*****************************************************************************
WaterBlock::WaterBlock()
{
   mGridElementSize = 100.0;
   mObjScale.set( 100.0, 100.0, 10.0 );

   mFullReflect = true;

   mNetFlags.set(Ghostable | ScopeAlways);
   mTypeMask = WaterObjectType;

   mObjBox.min.set( -0.5, -0.5, -0.5 );
   mObjBox.max.set(  0.5,  0.5,  0.5 );

   mElapsedTime = 0.0;
   mReflectTexSize = 128;
   mCurReflectTexSize = 0;
   mBaseColor.set( 11, 26, 44, 255 );
   mUnderwaterColor.set( 25, 51, 76, 153 );
   
   for( U32 i=0; i<MAX_WAVES; i++ )
   {
      mWaveDir[i].set( 0.0f, 0.0f );
      mWaveSpeed[i] = 0.0f;
      mWaveTexScale[i].set( 0.0f, 0.0f );
   }

   dMemset( mSurfMatName, 0, sizeof(mSurfMatName) );
   dMemset( mMatInstances, 0, sizeof(mMatInstances) );

   mClarity = 0.15f;
   mFresnelBias = 0.12f;
   mFresnelPower = 6.0f;
   mVisibilityDepth = 10.0;

   mReflectUpdateTicks = 0;

   mRenderFogMesh = true;
   bInitialize = false;
   m_pQuery = NULL;
#pragma message(ENGINE(暂时去掉海岸线的渲染))
   //defaultShoreSurfData();
}

WaterBlock::~WaterBlock()
{
   cleanupMaterials();

   SAFE_DELETE(m_pQuery);
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool WaterBlock::onAdd()
{
#ifdef NTJ_SERVER
	return false;
#endif

   if( !Parent::onAdd() )
   {
      return false;
   }

   mPrevScale = mObjScale;


   if( isClientObject() )
   {
      // Load in various Material definitions
      for( U32 i=0; i<NUM_MAT_TYPES; i++ )
      {
         SceneGraphData sgData;
         sgData.setDefaultLights();

         GFXVertexPC* vert = NULL;
         GFXVertexFlags flags = (GFXVertexFlags)getGFXVertFlags(vert);
         if( dStrlen(mSurfMatName[i]) > 0 )
         {
            mMatInstances[i] = new MatInstance(mSurfMatName[i]);
            mMatInstances[i]->init(sgData, flags);
         }
         if( !mMatInstances[i] )
         {
            Con::warnf( "Invalid Material name: %s: for WaterBlock ps2.0+ surface", mSurfMatName[i] );
         }
      }


      if( GFX->getPixelShaderVersion() >= 1.4 && mFullReflect )
      {
         // add to reflective set of objects (sets up updateReflection() callback)
         SimSet *reflectSet = dynamic_cast<SimSet*>( Sim::findObject( "reflectiveSet" ) );
         reflectSet->addObject( (SimObject*)this );
         //mReflectPlane.setupTex( mReflectTexSize );
		 //mCurReflectTexSize = mReflectTexSize;
      }
	  else
	  {
		  setupRadialVBIB();
		  // Create render target for watery bump on 1.1 card implementation
		  mBumpTex.set( BLEND_TEX_SIZE, BLEND_TEX_SIZE, GFXFormatR8G8B8A8, &GFXDefaultRenderTargetProfile, 1 );
	  }
#pragma message(ENGINE(暂时去掉海岸线的渲染))
      //setupShoreSurfData();
   }

   //Container::checkGlobal(this);

   resetWorldBox();
   addToScene();

   NetConnection::smGhostAlwaysDone.notify( this, &WaterBlock::onGhostAlwaysDone );

   return true;
}

//-----------------------------------------------------------------------------
// onRemove
//-----------------------------------------------------------------------------
void WaterBlock::onRemove()
{
   clearVertBuffers();
   cleanupMaterials();

   NetConnection::smGhostAlwaysDone.remove( this, &WaterBlock::onGhostAlwaysDone );

   removeFromScene();
   Parent::onRemove();
}

void WaterBlock::onGhostAlwaysDone()
{
   if ( isClientObject() )
   {
       setupVBIB();
		 #pragma message(ENGINE(暂时去掉海岸线的渲染))
       //setupShoreSurfData();
   }
}

//-----------------------------------------------------------------------------
// packUpdate
//-----------------------------------------------------------------------------
U64 WaterBlock::packUpdate(NetConnection* con, U64 mask, BitStream* stream)
{
   U64 retMask = Parent::packUpdate(con, mask, stream);

   stream->write( mGridElementSize );

   if( stream->writeFlag( mask & (UpdateMask | InitialUpdateMask) ) )
   {
      // This is set to allow the user to modify the size of the water dynamically
      // in the editor
      mathWrite( *stream, mObjScale );
      stream->writeAffineTransform( mObjToWorld );
      stream->writeFlag( mFullReflect );
      stream->writeFlag( mRenderFogMesh );

      stream->write( mReflectTexSize );
      stream->write( mBaseColor );
      stream->write( mUnderwaterColor );
      stream->write( mClarity );
      stream->write( mFresnelBias );
      stream->write( mFresnelPower );
      stream->write( mVisibilityDepth );

      for( U32 i=0; i<MAX_WAVES; i++ )
      {
         stream->write( mWaveSpeed[i] );
         mathWrite( *stream, mWaveDir[i] );
         mathWrite( *stream, mWaveTexScale[i] );
      }
#pragma message(ENGINE(暂时去掉海岸线的渲染))
      //for ( U32 i = 0; i < MAX_SHORESURFS; i++ )
      //{
      //    stream->writeFlag( mShoreSurfEnable[i] );       

      //    stream->write( mShoreHeight[i] );

      //    stream->write( mSurfRandSeed[i] );
      //    stream->write( mSurfWidth[i] );
      //    stream->write( mSurfWidthRand[i] );
      //    stream->write( mSurfHeight[i] );
      //    stream->write( mSurfHeightRand[i] );
      //    stream->write( mSurfPos[i] );
      //    stream->write( mSurfPosRand[i] );
      //    stream->write( mSurfSpeed[i] );
      //    stream->write( mSurfSpeedRand[i] );
      //    stream->write( mSurfAlpha[i] );
      //    stream->write( mSurfAlphaRand[i] );
      //    stream->write( mSurfMiddle[i] );
      //    stream->write( mSurfMiddleRand[i] );

      //    stream->writeString( mSurfTexFilename[i] );
      //}
   }

   // InitialUpdateMask is not explicitly set b/c the first call to this
   // function has a 0xffffff mask - thus InitialUpdateMask is set the first time
   if( stream->writeFlag( !(mask & InitialUpdateMask) ) )
      return retMask;

   for( U32 i=0; i<NUM_MAT_TYPES; i++ )
   {
      stream->writeString( mSurfMatName[i] );
   }

   return retMask;
}

//-----------------------------------------------------------------------------
// unpackUpdate
//-----------------------------------------------------------------------------
void WaterBlock::unpackUpdate(NetConnection* con, BitStream* stream)
{
   Parent::unpackUpdate(con, stream);

   stream->read( &mGridElementSize );

   if( stream->readFlag() )
   {
      mathRead( *stream, &mObjScale );
      stream->readAffineTransform( &mObjToWorld );
      mFullReflect = stream->readFlag();
      mRenderFogMesh = stream->readFlag();

      stream->read( &mReflectTexSize );
      stream->read( &mBaseColor );
      stream->read( &mUnderwaterColor );
      stream->read( &mClarity );
      stream->read( &mFresnelBias );
      stream->read( &mFresnelPower );
      stream->read( &mVisibilityDepth );

      for( U32 i=0; i<MAX_WAVES; i++ )
      {
         stream->read( &mWaveSpeed[i] );
         mathRead( *stream, &mWaveDir[i] );
         mathRead( *stream, &mWaveTexScale[i] );
      }
#pragma message(ENGINE(暂时去掉海岸线的渲染))
      //for ( U32 i = 0; i < MAX_SHORESURFS; i++ )
      //{
      //    mShoreSurfEnable[i] = stream->readFlag();       

      //    stream->read( &mShoreHeight[i] );

      //    stream->read( &mSurfRandSeed[i] );
      //    stream->read( &mSurfWidth[i] );
      //    stream->read( &mSurfWidthRand[i] );
      //    stream->read( &mSurfHeight[i] );
      //    stream->read( &mSurfHeightRand[i] );
      //    stream->read( &mSurfPos[i] );
      //    stream->read( &mSurfPosRand[i] );
      //    stream->read( &mSurfSpeed[i] );
      //    stream->read( &mSurfSpeedRand[i] );
      //    stream->read( &mSurfAlpha[i] );
      //    stream->read( &mSurfAlphaRand[i] );
      //    stream->read( &mSurfMiddle[i] );
      //    stream->read( &mSurfMiddleRand[i] );

      //    mSurfTexFilename[i] = stream->readSTString();
      //}
#pragma message(ENGINE(暂时去掉海岸线的渲染))
      //setupShoreSurfData();
   }

   if (stream->readFlag())
      return;

   for( U32 i=0; i<NUM_MAT_TYPES; i++ )
   {
      mSurfMatName[i] = stream->readSTString();
   }   

   setupVBIB();
}

void WaterBlock::triggerTerrainChange(SceneObject *pTerrain)
{
	if ( isServerObject() )
		return;

	setupVBIB();
}

//-----------------------------------------------------------------------------
// Setup vertex and index buffers
//-----------------------------------------------------------------------------
void WaterBlock::setupVBIB()
{
   clearVertBuffers();

   const U32 maxIndexedVerts = 65536; // max number of indexed verts with U16 size indices

   if( mObjScale.x < mGridElementSize ||
       mObjScale.y < mGridElementSize )
   {
      AssertISV( false, "WaterBlock: Scale must be larger than gridElementSize" );
   }

   mWidth   = (U32)mFloor( mObjScale.x / mGridElementSize ) + 1;
   mHeight  = (U32)mFloor( mObjScale.y / mGridElementSize ) + 1;

   // figure out how many blocks are needed and their size
   U32 maxBlockRows = maxIndexedVerts / mWidth;
   U32 rowOffset = 0;
   
   while( (rowOffset+1) < mHeight )
   {
      U32 numRows = mHeight - rowOffset;
      if( numRows == 1 ) numRows++;
      if( numRows > maxBlockRows )
      {
         numRows = maxBlockRows;
      }

      setupVertexBlock( mWidth, numRows, rowOffset );
      setupPrimitiveBlock( mWidth, numRows );

      rowOffset += numRows - 1;
   }

   bInitialize = true;
}

//-----------------------------------------------------------------------------
// Set up a block of vertices - the width is always the width of the entire
// waterBlock, so this is a block of full rows.
//-----------------------------------------------------------------------------
void WaterBlock::setupVertexBlock( U32 width, U32 height, U32 rowOffset )
{
   Point3F pos = getPosition();
   RayInfo rInfo;
   // Do we have any lights?
	const LightInfo *sunlight = gClientSceneGraph->getLightManager()->getSpecialLight(LightManager::slSunLightType);
	
	// Yes, so fetch sunlight ( always first light ).
	VectorF sunVector = -sunlight->mDirection;

   U32 numVerts = width * height;

   GFXVertexPCTT *verts = new GFXVertexPCTT[ numVerts ];
   ColorI waterColor(31, 56, 64, 127);
   GFXVertexColor vertCol(waterColor);

   float xdis = 1.0f/(float)(width- 1);
   float ydis = 1.0f/(float)(height- 1);

   U32 index = 0;
   for( U32 i=0; i<height; i++ )
   {
      for( U32 j=0; j<width; j++, index++ )
      {
         GFXVertexPCTT *vert = &verts[index];
         vert->point.x = (-mObjScale.x / 2.0) + mGridElementSize * j;
         vert->point.y = (-mObjScale.y / 2.0) + mGridElementSize * (i + rowOffset);
         vert->point.z = 0.0;
         vert->color = vertCol;
		 vert->texCoord2.x = j * xdis;
         vert->texCoord2.y = i * ydis;
         // Calculate the water depth

         Point3F start, end;

		 //Point3F worldPoint = vert->point + pos;

		 //Ray: 水面旋转问题修改
		 Point3F worldPoint = vert->point;
		 mObjToWorld.mulP(worldPoint);

         start.x = end.x = worldPoint.x;
         start.y = end.y = worldPoint.y;
         start.z = -2000; // Really high, might be over kill
         end.z = 2000; // really low, might be overkill

         // Cast a ray to see how deep the water is. We are
         // currently just testing for terrain and atlas
         // objects, but potentially any object that responds
         // to a ray cast could detected.
         if(gClientContainer.castRay(start, end, 
            //StaticObjectType | 
            InteriorObjectType | 
            //ShapeBaseObjectType | 
            //StaticShapeObjectType | 
            //ItemObjectType |
            //StaticTSObjectType |
            TerrainObjectType |
            AtlasObjectType
            , &rInfo))
         {
            F32 depth = -(rInfo.point.z - pos.z);
#pragma message(ENGINE(水效果恢复))
            //depth = mClampF(depth / mVisibilityDepth, -1.0f, 1.0f);
             if(depth <= 0.0f)
             {
                depth = 1.0f;
             }
             else
             {
                depth = depth / mVisibilityDepth;
                if(depth > 1.0f)
                {
                   depth = 1.0f;
                }
 
                depth = 1.0f - depth;
             }

            vert->texCoord.x = depth;
         }
         else
         {
            vert->texCoord.x = 0.0f;
         }

         // Cast a ray to do some AO-style shadowing.
         F32 &shadow = vert->texCoord.y;

         if(gClientContainer.castRay(worldPoint, worldPoint + sunVector * 4000.f, 
            //StaticObjectType | 
            InteriorObjectType | 
            //ShapeBaseObjectType | 
            //StaticShapeObjectType | 
            //ItemObjectType |
            //StaticTSObjectType |
            TerrainObjectType |
            AtlasObjectType
            , &rInfo))
         {
            shadow = 0.f;
         }
         else
         {
            shadow = 1.f;
         }
      }
   }

   // copy to vertex buffer
   GFXVertexBufferHandle <GFXVertexPCTT> * vertBuff = new GFXVertexBufferHandle <GFXVertexPCTT>;

   vertBuff->set( GFX, numVerts, GFXBufferTypeStatic );
   GFXVertexPCTT *vbVerts = vertBuff->lock();
   dMemcpy( vbVerts, verts, sizeof(GFXVertexPCTT) * numVerts );
   vertBuff->unlock();
   mVertBuffList.push_back( vertBuff );
   
   CStat::Instance()->SetDesc((unsigned int)vertBuff->getPointer(),"水体");

   delete [] verts;

}

//-----------------------------------------------------------------------------
// Set up a block of indices to match the block of vertices. The width is 
// always the width of the entire waterBlock, so this is a block of full rows.
//-----------------------------------------------------------------------------
void WaterBlock::setupPrimitiveBlock( U32 width, U32 height )
{
   // setup vertex / primitive buffers
   U32 numIndices = (width-1) * (height-1) * 6;
   U16 *indices = new U16[ numIndices ];
   U32 numVerts = width * height;

   // This uses indexed triangle lists instead of strips, but it shouldn't be
   // significantly slower if the indices cache well.
   
   // Rough diagram of the index order
   //   0----2----+ ...
   //   |  / |    |
   //   |/   |    |
   //   1----3----+ ...
   //   |    |    |
   //   |    |    |
   //   +----+----+ ...

   U32 index = 0;
   for( U32 i=0; i<(height-1); i++ )
   {
      for( U32 j=0; j<(width-1); j++, index+=6 )
      {
         // Process one quad at a time.  Note it will re-use the same indices from
         // previous quad, thus optimizing vert cache.  Cache will run out at
         // end of each row with this implementation however.
         indices[index+0] = (i) * mWidth + j;         // 0
         indices[index+1] = (i+1) * mWidth + j;       // 1
         indices[index+2] =  i * mWidth + j+1;        // 2
         indices[index+3] = (i+1) * mWidth + j;       // 1
         indices[index+4] = (i+1) * mWidth + j+1;     // 3
         indices[index+5] =  i * mWidth + j+1;        // 2
      }

   }

   GFXPrimitiveBufferHandle *indexBuff = new GFXPrimitiveBufferHandle;
   
   GFXPrimitive pInfo;
   pInfo.type = GFXTriangleList;
   pInfo.numPrimitives = numIndices / 3;
   pInfo.startIndex = 0;
   pInfo.minIndex = 0;
   pInfo.numVertices = numVerts;

   U16 *ibIndices;
   GFXPrimitive *piInput;
   indexBuff->set( GFX, numIndices, 1, GFXBufferTypeStatic );
   indexBuff->lock( &ibIndices, &piInput );
   dMemcpy( ibIndices, indices, numIndices * sizeof(U16) );
   dMemcpy( piInput, &pInfo, sizeof(GFXPrimitive) );
   indexBuff->unlock();
   mPrimBuffList.push_back( indexBuff );

   CStat::Instance()->SetDesc((unsigned int)indexBuff->getPointer(),"水体");

   delete [] indices;
}


//-----------------------------------------------------------------------------
// prepRenderImage
//-----------------------------------------------------------------------------
bool WaterBlock::prepRenderImage( SceneState* state, 
                                 const U32   stateKey,
                                 const U32, 
                                 const bool )
{
   PROFILE_SCOPE(WaterBlock_prepRenderImage);

   if(!bInitialize)
   {
	   return false;
   }

   if( isLastState(state, stateKey) )
   {
      return false;
   }

   setLastState(state, stateKey);

   // This should be sufficient for most objects that don't manage zones, and
   //  don't need to return a specialized RenderImage...
   if( state->isObjectRendered(this) )
   {
      RenderInst *ri = gRenderInstManager.allocInst();
      ri->obj = this;
      ri->state = state;
      ri->type = RenderInstManager::RIT_Water;
      gRenderInstManager.addInst( ri );
   }
   return false;
}

//------------------------------------------------------------------------------
// Setup scenegraph data structure for materials
//------------------------------------------------------------------------------
SceneGraphData WaterBlock::setupSceneGraphInfo( SceneState *state )
{
   SceneGraphData sgData;

   LightManager* lm = gClientSceneGraph->getLightManager();
   sgData.light = lm->getSpecialLight(LightManager::slTranslatedSunlightType);
   sgData.lightSecondary = lm->getDefaultLight();

   // fill in camera position relative to water
   sgData.camPos = state->getCameraPosition();

   // fill in water's transform
   sgData.objTrans = getRenderTransform();

   // fog
   sgData.setFogParams();

   // misc
   sgData.backBuffTex = GFX->getSfxBackBuffer();
   sgData.reflectTex = mReflectPlane.getTex();
   sgData.miscTex = mBumpTex;


   return sgData;
}

//-----------------------------------------------------------------------------
// set shader parameters
//-----------------------------------------------------------------------------
void WaterBlock::setShaderParams()
{
   mElapsedTime = (F32)Platform::getVirtualMilliseconds()/1000.0f; // uggh, should multiply by timescale (it's in main.cpp)

   F32 reg[4];
   
   // set vertex shader constants
   //-----------------------------------
   for( U32 i=0; i<MAX_WAVES; i++ )
   {
      dMemcpy( reg, mWaveDir[i], sizeof(Point2F) );

      GFX->setVertexShaderConstF( V_SHADER_PARAM_OFFSET + i, (float*)reg, 1, 2 );
   }

   for( U32 i=0; i<MAX_WAVES; i++ )
   {
      reg[i] = mWaveSpeed[i] * mElapsedTime;
   }

   GFX->setVertexShaderConstF( V_SHADER_PARAM_OFFSET + 4, (float*)reg, 1 );


   for( U32 i=0; i<MAX_WAVES; i++ )
   {
      Point2F texScale = mWaveTexScale[i];
      if( texScale.x > 0.0 )
      {
         texScale.x = 1.0 / texScale.x;
      }
      if( texScale.y > 0.0 )
      {
         texScale.y = 1.0 / texScale.y;
      }

      GFX->setVertexShaderConstF( V_SHADER_PARAM_OFFSET + 5 + i, (float*)&texScale, 1, 2 );
   }

   reg[0] = (F32)mReflectTexSize;
   GFX->setVertexShaderConstF( V_SHADER_PARAM_OFFSET + 9, (float*)reg, 1, 1 );


   // set pixel shader constants
   //-----------------------------------
   reg[0] = mBaseColor.red   / 255.0;
   reg[1] = mBaseColor.green / 255.0;
   reg[2] = mBaseColor.blue  / 255.0;
   reg[3] = mBaseColor.alpha / 255.0;
   GFX->setPixelShaderConstF( PC_USERDEF1, (float*)reg, 1 );

   reg[0] = mClarity; // clarity
   reg[1] = mFresnelBias; // fresnel bias
   reg[2] = mFresnelPower;  // fresnel power
   GFX->setPixelShaderConstF( 10, (float*)reg, 1, 3 );

}

void WaterBlock::renderQuery()
{
	GFX->disableShaders();
#ifdef STATEBLOCK
	AssertFatal(mSetQuerySB, "WaterBlock::renderQuery -- mSetQuerySB cannot be NULL.");
	mSetQuerySB->apply();
#else
	GFX->setZEnable( true );
	GFX->setZWriteEnable( false );
	GFX->setAlphaBlendEnable( false );
	GFX->enableColorWrites(false,false,false,false);
#endif
	GFX->setTexture(0,NULL);
	GFX->setTexture(1,NULL);
	GFX->setTexture(2,NULL);

	GFX->pushWorldMatrix();   
	MatrixF world = GFX->getWorldMatrix();
	world.mul( getRenderTransform() );
	GFX->setWorldMatrix( world );

	//Box3F box = mObjBox;
	//box.min.convolve(mObjScale);
	//box.max.convolve(mObjScale);
	//Point3F size = (box.max - box.min) * 0.5;
	//Point3F center = (box.min + box.max) * 0.5;
	//GFX->getDrawUtil()->drawWireCube( size, center, ColorI( 255, 255, 255 ) );

	for( U32 i=0; i<mVertBuffList.size(); i++ )
	{
		// set vert/prim buffer
		GFX->setVertexBuffer( *mVertBuffList[i] );
		GFXPrimitiveBuffer *primBuff = *mPrimBuffList[i];
		GFX->setPrimitiveBuffer( primBuff );
		GFX->drawPrimitives();
	}

	GFX->popWorldMatrix();
#ifdef STATEBLOCK
	AssertFatal(mColorWriteTSB, "WaterBlock::renderQuery -- mColorWriteTSB cannot be NULL.");
	mColorWriteTSB->apply();
#else
	GFX->enableColorWrites(true,true,true,true);
#endif

}
//-----------------------------------------------------------------------------
// renderObject
//-----------------------------------------------------------------------------
void WaterBlock::renderObject(SceneState* state, RenderInst *ri)
{
   if( gClientSceneGraph->isReflectPass())
      return;

   if(m_pQuery && !m_pQuery->begin())
	   return;

   if((getQueryResult()<100) && (mFullReflect))
   {
		renderQuery();
   }
   else
   {

#ifdef STATEBLOCK
		AssertFatal(mSetFilterSB, "WaterBlock::renderObject -- mSetFilterSB cannot be NULL.");
		mSetFilterSB->apply();
#else
	   for( U32 i=0; i<4; i++ )
	   {
		   GFX->setTextureStageMagFilter( i, GFXTextureFilterLinear );
		   GFX->setTextureStageMinFilter( i, GFXTextureFilterLinear );
		   GFX->setTextureStageMipFilter( i, GFXTextureFilterLinear );
	   }
#endif

	   GFX->setBaseRenderState();

	   //-------------------------------------
	   // copy sfx backbuffer
	   //-------------------------------------
	   // set ortho projection matrix
	   MatrixF proj = GFX->getProjectionMatrix();
	   MatrixF newMat(true);
	   GFX->setProjectionMatrix( newMat );
	   GFX->pushWorldMatrix();
	   GFX->setWorldMatrix( newMat );

	   GFX->copyBBToSfxBuff();

	   // restore projection matrix
	   GFX->setProjectionMatrix( proj );
	   GFX->popWorldMatrix();

	   SceneGraphData sgData = setupSceneGraphInfo( state );
	   const Point3F &camPos = state->getCameraPosition();


	   if( GFX->getPixelShaderVersion() < 2.0 )
	   {
		   animBumpTex( state );
		   render1_1( sgData, camPos );
	   }
	   else
	   {
		   render2_0( sgData, camPos );
	   }

	   // restore states
	   GFX->setBaseRenderState();
   }

   if(m_pQuery)
		m_pQuery->end();
}

//-----------------------------------------------------------------------------
// render water for 1.1 pixel shader cards
//-----------------------------------------------------------------------------
void WaterBlock::render1_1( SceneGraphData &sgData, const Point3F &camPosition )
{

#ifdef STATEBLOCK
	AssertFatal(mSetAddr1_1SB, "WaterBlock::render1_1 -- mSetAddr1_1SB cannot be NULL.");
	mSetAddr1_1SB->apply();
#else
   GFX->setTextureStageAddressModeU( 0, GFXAddressWrap );
   GFX->setTextureStageAddressModeV( 0, GFXAddressWrap );
   GFX->setTextureStageAddressModeU( 1, GFXAddressClamp );  // reflection
   GFX->setTextureStageAddressModeV( 1, GFXAddressClamp );
   GFX->setTextureStageAddressModeU( 2, GFXAddressClamp );  // refraction
   GFX->setTextureStageAddressModeV( 2, GFXAddressClamp );
   GFX->setTextureStageAddressModeU( 3, GFXAddressClamp );  // fog
   GFX->setTextureStageAddressModeV( 3, GFXAddressClamp );
#endif



   // setup proj/world transform
   GFX->pushWorldMatrix();   
   MatrixF world = GFX->getWorldMatrix();
   world.mul( getRenderTransform() );
   GFX->setWorldMatrix( world );
   
   MatrixF proj = GFX->getProjectionMatrix();
   proj.mul(world);
   proj.transpose();
   GFX->setVertexShaderConstF( 0, (float*)&proj, 4 );

   // update plane
   PlaneF plane;
   Point3F norm;
   getRenderTransform().getColumn( 2, &norm );
   norm.normalize();
   plane.set( getRenderPosition(), norm );
   mReflectPlane.setPlane( plane );

   // set the material
   MatInstance *mat = NULL;
   if( !mFullReflect)
   {
      // This is here because the 1_1 pass is run by 2.0 cards when full reflect is off
      mat = mMatInstances[ NO_REFLECT ];
   }
   else
   {
      mat = mMatInstances[ BASE_PASS ];
   }

   bool underWater = false;
   if( isUnderwater( camPosition ) )
   {
      underWater = true;
      mat = mMatInstances[UNDERWATER_PASS];

#ifdef STATEBLOCK
		AssertFatal(mUnderWater1_1SB, "WaterBlock::render1_1 -- mUnderWater1_1SB cannot be NULL.");
		mUnderWater1_1SB->apply();
#else
      GFX->setTextureStageAddressModeU( 3, GFXAddressWrap );
      GFX->setTextureStageAddressModeV( 3, GFXAddressWrap );
#endif


      if( GFX->getPixelShaderVersion() >= 2.0 )
      {
         setShaderParams();
      }
   }


   // draw once to stencil
   if( mRenderFogMesh )
   {
#ifdef STATEBLOCK
		AssertFatal(mFogMeshSB, "WaterBlock::render1_1 -- mFogMeshSB cannot be NULL.");
		mFogMeshSB->apply();
#else
      GFX->setStencilEnable( true );
      GFX->setStencilRef( 2 );
      GFX->setStencilFunc( GFXCmpAlways );
      GFX->setStencilPassOp( GFXStencilOpReplace);
#endif

   }

   // render the grid geometry
    if( mat )
    {
       while( mat->setupPass( sgData ) )
       {
         for( U32 i=0; i<mVertBuffList.size(); i++ )
         {
#ifdef STATEBLOCK
				AssertFatal(mAlphaVColorSB, "WaterBlock::render1_1 -- mAlphaVColorSB cannot be NULL.");
				mAlphaVColorSB->apply();
#else
            GFX->setAlphaBlendEnable(true);
            GFX->setVertexColorEnable(true);
#endif
            // set vert/prim buffer
            GFX->setVertexBuffer( *mVertBuffList[i] );
            GFXPrimitiveBuffer *primBuff = *mPrimBuffList[i];
            GFX->setPrimitiveBuffer( primBuff );
            GFX->drawPrimitives();
         }

      }
   }

   if( mRenderFogMesh )
   {
#ifdef STATEBLOCK
		AssertFatal(mFogMesh2SB, "WaterBlock::render1_1 -- mFogMesh2SB cannot be NULL.");
		mFogMesh2SB->apply();
#else
      GFX->setStencilEnable( true );
      GFX->setStencilPassOp( GFXStencilOpZero);
      GFX->setStencilFunc( GFXCmpEqual );
      GFX->setZEnable( false );
#endif
      // render radial geometry
      if( !underWater )
      {
         GFX->popWorldMatrix();
         GFX->pushWorldMatrix();
         MatrixF world = GFX->getWorldMatrix();

         MatrixF matTrans = getRenderTransform();
         Point3F pos = plane.project( camPosition );
         matTrans.setPosition( pos );
         world.mul( matTrans );
         GFX->setWorldMatrix( world );
         sgData.objTrans = matTrans;


         MatrixF proj = GFX->getProjectionMatrix();
         proj.mul(world);
         proj.transpose();
         GFX->setVertexShaderConstF( 0, (float*)&proj, 4 );

         GFX->setVertexBuffer( mRadialVertBuff );
         GFXPrimitiveBuffer *primBuff = mRadialPrimBuff;
         GFX->setPrimitiveBuffer( primBuff );

         mat = mMatInstances[FOG_PASS];

         if( mat )
         {
            while( mat->setupPass( sgData ) )
            {
#ifdef STATEBLOCK
					AssertFatal(mAlphaVColorSB, "WaterBlock::render1_1 -- mAlphaVColorSB cannot be NULL.");
					mAlphaVColorSB->apply();
#else
               GFX->setVertexColorEnable(true);
               GFX->setAlphaBlendEnable( true );
#endif
               GFX->drawPrimitives();
            }
         }

      }
   }



   GFX->popWorldMatrix();

#ifdef STATEBLOCK
	AssertFatal(mClear2SB, "WaterBlock::render1_1 -- mClear2SB cannot be NULL.");
	mClear2SB->apply();
#else
   GFX->setStencilEnable( false );
   GFX->setZEnable( true );
#endif

   if( underWater )
   {
      drawUnderwaterFilter();
   }
#ifdef STATEBLOCK
	AssertFatal(mClearSB, "WaterBlock::render1_1 -- mClearSB cannot be NULL.");
	mClearSB->apply();
#else
   GFX->setAlphaBlendEnable( false );
   GFX->setVertexColorEnable(false);
#endif

}

//-----------------------------------------------------------------------------
// render water for 2.0+ pixel shader cards
//-----------------------------------------------------------------------------
void WaterBlock::render2_0( SceneGraphData &sgData, const Point3F &camPosition )
{

#ifdef STATEBLOCK
	AssertFatal(mSetAddr1_1SB, "WaterBlock::render2_0 -- mSetAddr1_1SB cannot be NULL.");
	mSetAddr1_1SB->apply();
#else
   GFX->setTextureStageAddressModeU( 0, GFXAddressWrap );
   GFX->setTextureStageAddressModeV( 0, GFXAddressWrap );
   GFX->setTextureStageAddressModeU( 1, GFXAddressClamp );  // reflection
   GFX->setTextureStageAddressModeV( 1, GFXAddressClamp );
   GFX->setTextureStageAddressModeU( 2, GFXAddressClamp );  // refraction
   GFX->setTextureStageAddressModeV( 2, GFXAddressClamp );
   GFX->setTextureStageAddressModeU( 3, GFXAddressClamp );  // fog
   GFX->setTextureStageAddressModeV( 3, GFXAddressClamp );
#endif

   // setup proj/world transform
   GFX->pushWorldMatrix();   
   MatrixF world = GFX->getWorldMatrix();
   world.mul( getRenderTransform() );
   GFX->setWorldMatrix( world );
   
   MatrixF proj = GFX->getProjectionMatrix();
   proj.mul(world);
   proj.transpose();
   GFX->setVertexShaderConstF( 0, (float*)&proj, 4 );

   setShaderParams();

   // set the material
   MatInstance *mat = mMatInstances[ BASE_PASS ];

   bool underwater = false;
   if( isUnderwater( camPosition ) )
   {
      underwater = true;
      mat = mMatInstances[UNDERWATER_PASS];
   }

   // render the geometry
   if( mat )
   {
      while( mat->setupPass( sgData ) )
      {
         for( U32 i=0; i<mVertBuffList.size(); i++ )
         {
            // set vert/prim buffer
            GFX->setVertexBuffer( *mVertBuffList[i] );
            GFXPrimitiveBuffer *primBuff = *mPrimBuffList[i];
            GFX->setPrimitiveBuffer( primBuff );
            GFX->drawPrimitives();
         }
      }
   }

   GFX->popWorldMatrix();

#pragma message(ENGINE(暂时去掉海岸线的渲染))
   //renderShoreSurf();

   if( underwater )
   {
      drawUnderwaterFilter();
   }
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void WaterBlock::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("WaveData");
   addField( "waveDir",       TypePoint2F,  Offset( mWaveDir, WaterBlock ), MAX_WAVES );
   addField( "waveSpeed",     TypeF32,  Offset( mWaveSpeed, WaterBlock ), MAX_WAVES );
   addField( "waveTexScale",  TypePoint2F,  Offset( mWaveTexScale, WaterBlock ), MAX_WAVES );
   endGroup("WaveData");

   addGroup("Misc");
   //addField( "reflectTexSize", TypeS32,  Offset( mReflectTexSize, WaterBlock ) );
   addField( "baseColor", TypeColorI,  Offset( mBaseColor, WaterBlock ) );
   addField( "underwaterColor", TypeColorI,  Offset( mUnderwaterColor, WaterBlock ) );
   addField( "gridSize", TypeF32,  Offset( mGridElementSize, WaterBlock ) );
   endGroup("Misc");

   addField( "surfMaterial", TypeString,  Offset( mSurfMatName, WaterBlock ), NUM_MAT_TYPES );
   addField( "fullReflect",  TypeBool,  Offset( mFullReflect, WaterBlock ) );
   addField( "clarity",  TypeF32,  Offset( mClarity, WaterBlock ) );
   addField( "fresnelBias",  TypeF32,  Offset( mFresnelBias, WaterBlock ) );
   addField( "fresnelPower",  TypeF32,  Offset( mFresnelPower, WaterBlock ) );
   addField( "visibilityDepth", TypeF32, Offset( mVisibilityDepth, WaterBlock ) );

   addField( "renderFogMesh",  TypeBool,  Offset( mRenderFogMesh, WaterBlock ) );   
#pragma message(ENGINE(暂时去掉海岸线的渲染))
   //// 四层浪花参数, 分开便于编辑
   //addGroup("ShoreSurf00");
   //addField( "shoreSurfEnable00", TypeBool, Offset( mShoreSurfEnable[0], WaterBlock ) );
   //addField( "shoreHeight00", TypeF32, Offset( mShoreHeight[0], WaterBlock ) );
   //addField( "surfRandSeed00", TypeS32, Offset( mSurfRandSeed[0], WaterBlock ) );
   //addField( "surfWidth00", TypeF32, Offset( mSurfWidth[0], WaterBlock ) );
   //addField( "surfWidthRand00", TypeF32, Offset( mSurfWidthRand[0], WaterBlock ) );
   //addField( "surfHeight00", TypeF32, Offset( mSurfHeight[0], WaterBlock ) );
   //addField( "surfHeightRand00", TypeF32, Offset( mSurfHeightRand[0], WaterBlock ) );
   //addField( "surfPos00", TypeF32, Offset( mSurfPos[0], WaterBlock ) );
   //addField( "surfPosRand00", TypeF32, Offset( mSurfPosRand[0], WaterBlock ) );
   //addField( "surfSpeed00", TypeF32, Offset( mSurfSpeed[0], WaterBlock ) );
   //addField( "surfSpeedRand00", TypeF32, Offset( mSurfSpeedRand[0], WaterBlock ) );
   //addField( "surfAlpha00", TypeF32, Offset( mSurfAlpha[0], WaterBlock ) );
   //addField( "surfAlphaRand00", TypeF32, Offset( mSurfAlphaRand[0], WaterBlock ) );
   //addField( "surfMiddle00", TypeF32, Offset( mSurfMiddle[0], WaterBlock ) );
   //addField( "surfMiddleRand00", TypeF32, Offset( mSurfMiddleRand[0], WaterBlock ) );
   //addField( "surfTexFilename00", TypeFilename,  Offset( mSurfTexFilename[0], WaterBlock ) );
   //endGroup("ShoreSurf00");

   //addGroup("ShoreSurf11");
   //addField( "shoreSurfEnable11", TypeBool, Offset( mShoreSurfEnable[1], WaterBlock ) );
   //addField( "shoreHeight11", TypeF32, Offset( mShoreHeight[1], WaterBlock ) );
   //addField( "surfRandSeed11", TypeS32, Offset( mSurfRandSeed[1], WaterBlock ) );
   //addField( "surfWidth11", TypeF32, Offset( mSurfWidth[1], WaterBlock ) );
   //addField( "surfWidthRand11", TypeF32, Offset( mSurfWidthRand[1], WaterBlock ) );
   //addField( "surfHeight11", TypeF32, Offset( mSurfHeight[1], WaterBlock ) );
   //addField( "surfHeightRand11", TypeF32, Offset( mSurfHeightRand[1], WaterBlock ) );
   //addField( "surfPos11", TypeF32, Offset( mSurfPos[1], WaterBlock ) );
   //addField( "surfPosRand11", TypeF32, Offset( mSurfPosRand[1], WaterBlock ) );
   //addField( "surfSpeed11", TypeF32, Offset( mSurfSpeed[1], WaterBlock ) );
   //addField( "surfSpeedRand11", TypeF32, Offset( mSurfSpeedRand[1], WaterBlock ) );
   //addField( "surfAlpha11", TypeF32, Offset( mSurfAlpha[1], WaterBlock ) );
   //addField( "surfAlphaRand11", TypeF32, Offset( mSurfAlphaRand[1], WaterBlock ) );
   //addField( "surfMiddle11", TypeF32, Offset( mSurfMiddle[1], WaterBlock ) );
   //addField( "surfMiddleRand11", TypeF32, Offset( mSurfMiddleRand[1], WaterBlock ) );
   //addField( "surfTexFilename11", TypeFilename,  Offset( mSurfTexFilename[1], WaterBlock ) );
   //endGroup("ShoreSurf11");

   //addGroup("ShoreSurf22");
   //addField( "shoreSurfEnable22", TypeBool, Offset( mShoreSurfEnable[2], WaterBlock ) );
   //addField( "shoreHeight22", TypeF32, Offset( mShoreHeight[2], WaterBlock ) );
   //addField( "surfRandSeed22", TypeS32, Offset( mSurfRandSeed[2], WaterBlock ) );
   //addField( "surfWidth22", TypeF32, Offset( mSurfWidth[2], WaterBlock ) );
   //addField( "surfWidthRand22", TypeF32, Offset( mSurfWidthRand[2], WaterBlock ) );
   //addField( "surfHeight22", TypeF32, Offset( mSurfHeight[2], WaterBlock ) );
   //addField( "surfHeightRand22", TypeF32, Offset( mSurfHeightRand[2], WaterBlock ) );
   //addField( "surfPos22", TypeF32, Offset( mSurfPos[2], WaterBlock ) );
   //addField( "surfPosRand22", TypeF32, Offset( mSurfPosRand[2], WaterBlock ) );
   //addField( "surfSpeed22", TypeF32, Offset( mSurfSpeed[2], WaterBlock ) );
   //addField( "surfSpeedRand22", TypeF32, Offset( mSurfSpeedRand[2], WaterBlock ) );
   //addField( "surfAlpha22", TypeF32, Offset( mSurfAlpha[2], WaterBlock ) );
   //addField( "surfAlphaRand22", TypeF32, Offset( mSurfAlphaRand[2], WaterBlock ) );
   //addField( "surfMiddle22", TypeF32, Offset( mSurfMiddle[2], WaterBlock ) );
   //addField( "surfMiddleRand22", TypeF32, Offset( mSurfMiddleRand[2], WaterBlock ) );
   //addField( "surfTexFilename22", TypeFilename,  Offset( mSurfTexFilename[2], WaterBlock ) );
   //endGroup("ShoreSurf22");

   //addGroup("ShoreSurf33");
   //addField( "shoreSurfEnable33", TypeBool, Offset( mShoreSurfEnable[3], WaterBlock ) );
   //addField( "shoreHeight33", TypeF32, Offset( mShoreHeight[3], WaterBlock ) );
   //addField( "surfRandSeed33", TypeS32, Offset( mSurfRandSeed[3], WaterBlock ) );
   //addField( "surfWidth33", TypeF32, Offset( mSurfWidth[3], WaterBlock ) );
   //addField( "surfWidthRand33", TypeF32, Offset( mSurfWidthRand[3], WaterBlock ) );
   //addField( "surfHeight33", TypeF32, Offset( mSurfHeight[3], WaterBlock ) );
   //addField( "surfHeightRand33", TypeF32, Offset( mSurfHeightRand[3], WaterBlock ) );
   //addField( "surfPos33", TypeF32, Offset( mSurfPos[3], WaterBlock ) );
   //addField( "surfPosRand33", TypeF32, Offset( mSurfPosRand[3], WaterBlock ) );
   //addField( "surfSpeed33", TypeF32, Offset( mSurfSpeed[3], WaterBlock ) );
   //addField( "surfSpeedRand33", TypeF32, Offset( mSurfSpeedRand[3], WaterBlock ) );
   //addField( "surfAlpha33", TypeF32, Offset( mSurfAlpha[3], WaterBlock ) );
   //addField( "surfAlphaRand33", TypeF32, Offset( mSurfAlphaRand[3], WaterBlock ) );
   //addField( "surfMiddle33", TypeF32, Offset( mSurfMiddle[3], WaterBlock ) );
   //addField( "surfMiddleRand33", TypeF32, Offset( mSurfMiddleRand[3], WaterBlock ) );
   //addField( "surfTexFilename33", TypeFilename,  Offset( mSurfTexFilename[3], WaterBlock ) );
   //endGroup("ShoreSurf33");
}     


//-----------------------------------------------------------------------------
// Update planar reflection
//-----------------------------------------------------------------------------
void WaterBlock::updateWaterQuery()
{
	if(!bInitialize || !g_GraphicsProfile.mWaterReflaction)
		return;

	if(m_pQuery)
	{
		int pix = m_pQuery->query();
		setQueryResult(pix);
	}
	else
		setQueryResult(0xFFFFFFFF);
}

void WaterBlock::updateReflection()
{
#ifdef NTJ_EDITOR
	if(gEditingMission)
	{
		return;
	}
#endif

	if(!bInitialize || !g_GraphicsProfile.mWaterReflaction)
		return;

	if(!m_pQuery)
	{
		m_pQuery = new GFXD3D9OcclusionQuery(GFX);
		AssertFatal(m_pQuery->initialize(), "updateReflection m_pQuery->initialize error");
	}

	PROFILE_SCOPE(WaterBlock_updateReflection);

   // Simple method of insuring reflections aren't rendered unless
   // the waterBlock is actually visible
   if(getQueryResult() <100000)
   {
	   if(mCurReflectTexSize)
	   {
		   mReflectPlane.setupTex( 0 );
		   mCurReflectTexSize = 0;
	   }

	   return;
   }
   else
   {
	   if(!mCurReflectTexSize)
	   {
		   mReflectPlane.setupTex( mReflectTexSize );
		   mCurReflectTexSize = mReflectTexSize;
	   }
   }

   // Update water reflection no more than specified interval - causes some
   // artifacts if you whip the camera around a lot, but definitely can
   // improve speed.
   U32 curTime = Platform::getVirtualMilliseconds();
   if( curTime - mReflectUpdateTicks < 16 )
   {
      return;
   }

   // grab camera transform from tsCtrl
   static GuiTSCtrl *tsCtrl = NULL;
   if(!tsCtrl)
   {
	   tsCtrl = dynamic_cast<GuiTSCtrl*>( Sim::findObject("PlayGui") );
	   if(!tsCtrl)
		   return;
   }

   CameraQuery query;
   if(!tsCtrl->processCameraQuery( &query ))
   {
	   return;
   }

   mReflectUpdateTicks = curTime;

   // store current matrices
   GFX->pushWorldMatrix();
   MatrixF proj = GFX->getProjectionMatrix();


   // set up projection - must match that of main camera
   Point2I resolution = gClientSceneGraph->getDisplayTargetResolution();
   GFX->setFrustum( mRadToDeg(query.fov),
      F32(resolution.x) / F32(resolution.y ),
      query.nearPlane, query.farPlane );

   // store "normal" camera position before changing over to reflected position
   MatrixF camTrans = query.cameraMatrix;
   gClientSceneGraph->mNormCamPos = camTrans.getPosition();

   // update plane
   PlaneF plane;
   Point3F norm;
   getRenderTransform().getColumn( 2, &norm );
   norm.normalize();
   plane.set( getRenderPosition(), norm );
   mReflectPlane.setPlane ( plane );

   // set world mat from new camera view
   MatrixF camReflectTrans = mReflectPlane.getCameraReflection( camTrans );
   camReflectTrans.inverse();
   GFX->setWorldMatrix( camReflectTrans );

   // set new projection matrix
   gClientSceneGraph->setNonClipProjection( (MatrixF&) GFX->getProjectionMatrix() );
   MatrixF clipProj = mReflectPlane.getFrustumClipProj( camReflectTrans );
   GFX->setProjectionMatrix( clipProj );


   // render a frame
   gClientSceneGraph->setReflectPass( true );

   GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
   myTarg->attachTexture(GFXTextureTarget::Color0, mReflectPlane.getTex() );
   myTarg->attachTexture(GFXTextureTarget::DepthStencil, mReflectPlane.getDepth() );
   GFX->pushActiveRenderTarget();
   GFX->setActiveRenderTarget( myTarg );
#ifdef STATEBLOCK
	AssertFatal(mZTrueSB, "WaterBlock::updateReflection -- mZTrueSB cannot be NULL.");
	mZTrueSB->apply();
#else
   GFX->setZEnable( true );
#endif

   GFX->clear( GFXClearZBuffer | GFXClearTarget, ColorI( 64, 64, 64 ), 1.0f, 0 );
   U32 objTypeFlag = -1;
   gClientSceneGraph->renderScene( objTypeFlag );
   GFX->popActiveRenderTarget();

   // cleanup
   // GFX->clear( GFXClearZBuffer | GFXClearStencil, ColorI( 255, 0, 255 ), 1.0f, 0 );
   gClientSceneGraph->setReflectPass( false );
   GFX->popWorldMatrix();
   GFX->setProjectionMatrix( proj );
}

//-----------------------------------------------------------------------------
// Draw translucent filter over screen when underwater
//-----------------------------------------------------------------------------
void WaterBlock::drawUnderwaterFilter()
{
   // set up camera transforms
   MatrixF proj = GFX->getProjectionMatrix();
   MatrixF newMat(true);
   GFX->setProjectionMatrix( newMat );
   GFX->pushWorldMatrix();
   GFX->setWorldMatrix( newMat );   


   // set up render states
   GFX->disableShaders();
#ifdef STATEBLOCK
	AssertFatal(mSetUnderWater, "WaterBlock::drawUnderwaterFilter -- mSetUnderWater cannot be NULL.");
	mSetUnderWater->apply();
#else
   GFX->setAlphaBlendEnable( true );
   GFX->setSrcBlend( GFXBlendSrcAlpha );
   GFX->setDestBlend( GFXBlendInvSrcAlpha );
   GFX->setTextureStageColorOp(0, GFXTOPDisable);
   GFX->setZEnable( false );
#endif

   // draw quad
   Point2I resolution = GFX->getActiveRenderTarget()->getSize();
   F32 copyOffsetX = 1.0 / resolution.x;
   F32 copyOffsetY = 1.0 / resolution.y;

   PrimBuild::color( mUnderwaterColor );
   PrimBuild::begin( GFXTriangleFan, 4 );
      PrimBuild::texCoord2f( 0.0, 1.0 );
      PrimBuild::vertex3f( -1.0 - copyOffsetX, -1.0 + copyOffsetY, 0.0 );
   
      PrimBuild::texCoord2f( 0.0, 0.0 );
      PrimBuild::vertex3f( -1.0 - copyOffsetX,  1.0 + copyOffsetY, 0.0 );
   
      PrimBuild::texCoord2f( 1.0, 0.0 );
      PrimBuild::vertex3f(  1.0 - copyOffsetX,  1.0 + copyOffsetY, 0.0 );
   
      PrimBuild::texCoord2f( 1.0, 1.0 );
      PrimBuild::vertex3f(  1.0 - copyOffsetX, -1.0 + copyOffsetY, 0.0 );
   PrimBuild::end();

#ifdef STATEBLOCK
	AssertFatal(mClearUnderWaterSB, "WaterBlock::drawUnderwaterFilter -- mClearUnderWaterSB cannot be NULL.");
	mClearUnderWaterSB->apply();
#else
   // reset states / transforms
   GFX->setZEnable( true );
   GFX->setAlphaBlendEnable( false );
#endif

   GFX->setProjectionMatrix( proj );
   GFX->popWorldMatrix();

}

//-----------------------------------------------------------------------------
// Animate the bump texture - for 1.1 cards
//-----------------------------------------------------------------------------
void WaterBlock::animBumpTex( SceneState *state )
{
   GFXTransformSaver saver;   // mostly for viewport.
#ifdef STATEBLOCK
	AssertFatal(mSetAnimSB, "WaterBlock::animBumpTex -- mSetAnimSB cannot be NULL.");
	mSetAnimSB->apply();
#else
   GFX->setTextureStageAddressModeU( 0, GFXAddressWrap );
   GFX->setTextureStageAddressModeV( 0, GFXAddressWrap );
   GFX->setTextureStageAddressModeU( 1, GFXAddressWrap );
   GFX->setTextureStageAddressModeV( 1, GFXAddressWrap );
#endif
   // set ortho projection matrix
   MatrixF proj = GFX->getProjectionMatrix();
   MatrixF newMat(true);
   GFX->setProjectionMatrix( newMat );
   GFX->pushWorldMatrix();
   GFX->setWorldMatrix( newMat );   
   GFX->setVertexShaderConstF( 0, (float*)&newMat, 4 );

   // set up blend shader - pass in wave params
   MatInstance *mat = mMatInstances[BLEND];

   // send time info to shader
   mElapsedTime = (F32)Platform::getVirtualMilliseconds()/1000.0f; // uggh, should multiply by timescale (it's in main.cpp)
   float timeScale = mElapsedTime * 0.15;
   GFX->setVertexShaderConstF( 54, (float*)&timeScale, 1, 1 );

   
#ifndef STATEBLOCK
   GFX->setZEnable( false );
#endif
   GFX->pushActiveRenderTarget();
   GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
   myTarg->attachTexture(GFXTextureTarget::Color0, mBumpTex);
   //myTarg->attachTexture(GFXTextureTarget::DepthStencil, GFXTextureTarget::sDefaultDepthStencil );
   GFX->setActiveRenderTarget( myTarg );
   
   SceneGraphData sgData = setupSceneGraphInfo( state ); // hrmm, eliminate this?
   F32 copyOffsetX = 1.0 / BLEND_TEX_SIZE;
   F32 copyOffsetY = 1.0 / BLEND_TEX_SIZE;

   // render out new bump texture
   while( mat->setupPass( sgData ) )
   {
      PrimBuild::begin( GFXTriangleFan, 4 );
         PrimBuild::texCoord2f( 0.0, 1.0 );
         PrimBuild::vertex3f( -1.0 - copyOffsetX, -1.0 + copyOffsetY, 0.0 );
      
         PrimBuild::texCoord2f( 0.0, 0.0 );
         PrimBuild::vertex3f( -1.0 - copyOffsetX,  1.0 + copyOffsetY, 0.0 );
      
         PrimBuild::texCoord2f( 1.0, 0.0 );
         PrimBuild::vertex3f(  1.0 - copyOffsetX,  1.0 + copyOffsetY, 0.0 );
      
         PrimBuild::texCoord2f( 1.0, 1.0 );
         PrimBuild::vertex3f(  1.0 - copyOffsetX, -1.0 + copyOffsetY, 0.0 );
      PrimBuild::end();
   }
   
   GFX->popActiveRenderTarget();

#ifdef STATEBLOCK
	AssertFatal(mZTrueSB, "WaterBlock::animBumpTex -- mZTrueSB cannot be NULL.");
	mZTrueSB->apply();
#else
   GFX->setZEnable( true );
#endif
   // done
   GFX->popWorldMatrix();
   GFX->setProjectionMatrix( proj );
}

void WaterBlock::cleanupMaterials()
{
   for (U32 i = 0; i < NUM_MAT_TYPES; i++)
   {
      SAFE_DELETE(mMatInstances[i]);
   }
}

//-----------------------------------------------------------------------------
// Set up the vertex/index buffers for a radial ( concentric rings ) mesh
//-----------------------------------------------------------------------------
void WaterBlock::setupRadialVBIB()
{
   Vector< GFXVertexPC > verts;
   Vector< U16 > indices;

   // create verts
   U32 numRadPoints = 15;
   U32 numSections = 30;

   GFXVertexPC vert;
   vert.point.set( 0.0, 0.0, 0.0 );
   ColorI waterColor(31, 56, 64, 127);
   vert.color = GFXVertexColor(waterColor);
   verts.push_back( vert );

   F32 radius = 1.0;

   // set up rings
   for( U32 i=0; i<numSections; i++ )
   {
      for( U32 j=0; j<numRadPoints; j++ )
      {
         F32 angle = F32(j) / F32(numRadPoints) * M_2PI;
         vert.point.set( radius * mSin(angle), radius * mCos(angle), 0.0 );
         verts.push_back( vert );
      }
      radius *= 1.3f;
   }

   // set up indices for innermost circle
   for( U32 i=0; i<numRadPoints; i++ )
   {
      U16 index = 0;
      indices.push_back( index );
      index = i+1;
      indices.push_back( index );
      index = ((i+1) % numRadPoints) + 1;
      indices.push_back( index );
   }

   // set up indices for concentric rings around the center
   for( U32 i=0; i<numSections-1; i++ )
   {
      for( U32 j=0; j<numRadPoints; j++ )
      {
         U16 pts[4];
         pts[0] = 1 + i * numRadPoints + j;
         pts[1] = 1 + (i+1) * numRadPoints + j;
         pts[2] = 1 + i * numRadPoints + (j+1) % numRadPoints;
         pts[3] = 1 + (i+1) * numRadPoints + (j+1) % numRadPoints;

         indices.push_back( pts[0] );
         indices.push_back( pts[1] );
         indices.push_back( pts[2] );

         indices.push_back( pts[1] );
         indices.push_back( pts[3] );
         indices.push_back( pts[2] );
      }
   }


   // copy to vertex buffer
   mRadialVertBuff.set( GFX, verts.size(), GFXBufferTypeStatic );
   GFXVertexPC *vbVerts = mRadialVertBuff.lock();
   dMemcpy( vbVerts, verts.address(), sizeof(GFXVertexPC) * verts.size() );
   mRadialVertBuff.unlock();

   // copy to index buffer
   GFXPrimitive pInfo;
   pInfo.type = GFXTriangleList;
   pInfo.numPrimitives = indices.size() / 3;
   pInfo.startIndex = 0;
   pInfo.minIndex = 0;
   pInfo.numVertices = verts.size();

   U16 *ibIndices;
   GFXPrimitive *piInput;
   mRadialPrimBuff.set( GFX, indices.size(), 1, GFXBufferTypeStatic );
   mRadialPrimBuff.lock( &ibIndices, &piInput );
   dMemcpy( ibIndices, indices.address(), indices.size() * sizeof(U16) );
   dMemcpy( piInput, &pInfo, sizeof(GFXPrimitive) );
   mRadialPrimBuff.unlock();


}

//-----------------------------------------------------------------------------
// Returns true if specified point is under the water plane and contained in
// the water's bounding box.
//-----------------------------------------------------------------------------
bool WaterBlock::isUnderwater( const Point3F &pnt )
{
   // update plane
   if( isServerObject() )
   {
      PlaneF plane;
      Point3F norm;
      getTransform().getColumn( 2, &norm );
      norm.normalize();
      plane.set( getPosition(), norm );
      mReflectPlane.setPlane( plane );
   }

   if( mReflectPlane.getPlane().distToPlane( pnt ) < -0.005 )
   {
      if( mWorldBox.isContained( pnt ) )
      {
         return true;
      }
   }

   return false;
}

//-----------------------------------------------------------------------------
// Clear vertex and primitive buffers
//-----------------------------------------------------------------------------
void WaterBlock::clearVertBuffers()
{
   for( U32 i=0; i<mVertBuffList.size(); i++ )
   {
      delete mVertBuffList[i];
   }
   mVertBuffList.clear();

   for( U32 i=0; i<mPrimBuffList.size(); i++ )
   {
      delete mPrimBuffList[i];
   }
   mPrimBuffList.clear();

   bInitialize = false;
}

//-----------------------------------------------------------------------------
// Inspect post apply
//-----------------------------------------------------------------------------
void WaterBlock::inspectPostApply()
{
   Parent::inspectPostApply();

   setMaskBits(UpdateMask);
}

//-----------------------------------------------------------------------------
// Set up projection matrix for multipass technique with different geometry.
// It basically just pushes the near plane out.  This should work across 
// fixed-function and shader geometry.
//-----------------------------------------------------------------------------
/*
void WaterBlock::setMultiPassProjection()
{
   F32 nearPlane, farPlane;
   F32 left, right, bottom, top;
   GFX->getFrustum( &left, &right, &bottom, &top, &nearPlane, &farPlane );

   F32 FOV = GameGetCameraFov();
   Point2I size = GFX->getVideoMode().resolution;

//   GFX->setFrustum( FOV, size.x/F32(size.y), nearPlane + 0.010, farPlane + 10.0 );

// note - will have to re-calc left, right, top, bottom if the above technique
// doesn't work through portals
//   GFX->setFrustum( left, right, bottom, top, nearPlane + 0.001, farPlane );


}
*/

bool WaterBlock::castRay(const Point3F& start, const Point3F& end, RayInfo* info )
{
   // Simply look for the hit on the water plane
   // and ignore any future issues with waves, etc.
   const Point3F norm(0,0,1);
   PlaneF plane( Point3F(0,0,0), norm );

   F32 hit = plane.intersect( start, end );
   if ( hit < 0.0f || hit > 1.0f )
      return false;
   
   info->t = hit;
   info->object = this;
   info->point = start + ( ( end - start ) * hit );
   info->normal = norm;

   return true;
}


void WaterBlock::defaultShoreSurfData()
{
    U32 i;
    for ( i = 0; i < MAX_SHORESURFS; i++ )
    {
        mShoreSurfEnable[i] = false;

        mShoreHeight[i] = 0.1f;

        mSurfRandSeed[i] = i * 2009;

        mSurfWidth[i] = 5.0f;
        mSurfWidthRand[i] = 3.0f;

        mSurfHeight[i] = 5.0f;
        mSurfHeightRand[i] = 3.0f;

        mSurfPos[i] = 0.0f;
        mSurfPosRand[i] = 1.0f;

        mSurfSpeed[i] = 0.1f;
        mSurfSpeedRand[i] = 0.05f;

        mSurfAlpha[i] = 1.0f;
        mSurfAlphaRand[i] = 0.0f;

        mSurfMiddle[i] = 0.5f;
        mSurfMiddleRand[i] = 0.3f;

        mSurfTexFilename[i] = StringTable->insert( "~/data/environments/water/waterShoreSurf" );
    }
}

void WaterBlock::setupShoreSurfData()
{
    U32 i;
    for ( i = 0; i < MAX_SHORESURFS; i++ )
    {
        mShoreSurfs[i].setShoreSurfEnable( mShoreSurfEnable[i] );

        mShoreSurfs[i].setShoreHeight( mShoreHeight[i] );

        mShoreSurfs[i].setSurfRandSeed( mSurfRandSeed[i] );
        mShoreSurfs[i].setSurfWidth( mSurfWidth[i], mSurfWidthRand[i] );
        mShoreSurfs[i].setSurfHeight( mSurfHeight[i], mSurfWidthRand[i] );
        mShoreSurfs[i].setSurfPos( mSurfPos[i], mSurfPosRand[i] );
        mShoreSurfs[i].setSurfSpeed( mSurfSpeed[i], mSurfSpeedRand[i] );
        mShoreSurfs[i].setSurfAlpha( mSurfAlpha[i], mSurfAlphaRand[i] );
        mShoreSurfs[i].setSurfMiddle( mSurfMiddle[i], mSurfMiddleRand[i] );

        mShoreSurfs[i].setSurfTexFilename( mSurfTexFilename[i] );   

        mShoreSurfs[i].setTerrainBlock( gClientSceneGraph->getCurrentTerrain() );
        mShoreSurfs[i].setWaterBlock( this );

        mShoreSurfs[i].update();
    }
}

void WaterBlock::renderShoreSurf()
{
    for ( U32 i = 0; i < ShoreSurf::PassType_Max; i++ )
    {
        for ( U32 j = 0; j < MAX_SHORESURFS; j++ )
            mShoreSurfs[j].render((ShoreSurf::PASS_TYPE)i);
    }
}


void WaterBlock::resetStateBlock()
{
	//mSetQuerySB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setRenderState(GFXRSColorWriteEnable, 0);//
	GFX->endStateBlock(mSetQuerySB);

	//mColorWriteTSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSColorWriteEnable, GFXCOLORWRITEENABLE_RED|GFXCOLORWRITEENABLE_GREEN|GFXCOLORWRITEENABLE_BLUE|GFXCOLORWRITEENABLE_ALPHA);//
	GFX->endStateBlock(mColorWriteTSB);

	//mSetFilterSB
	GFX->beginStateBlock();
	for( U32 i=0; i<4; i++ )
	{
		GFX->setSamplerState(i, GFXSAMPMagFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMinFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMipFilter, GFXTextureFilterLinear);;
	}
	GFX->endStateBlock(mSetFilterSB);

	//mSetAddr1_1SB
	GFX->beginStateBlock();
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressWrap);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressWrap);
	GFX->setSamplerState(1, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(1, GFXSAMPAddressV, GFXAddressClamp);
	GFX->setSamplerState(2, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(2, GFXSAMPAddressV, GFXAddressClamp);
	GFX->setSamplerState(3, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(3, GFXSAMPAddressV, GFXAddressClamp);
	GFX->endStateBlock(mSetAddr1_1SB);

	//mUnderWater1_1SB
	GFX->beginStateBlock();
	GFX->setSamplerState(3, GFXSAMPAddressU, GFXAddressWrap);
	GFX->setSamplerState(3, GFXSAMPAddressV, GFXAddressWrap);
	GFX->endStateBlock(mUnderWater1_1SB);

	//mFogMeshSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSStencilEnable, true);
	GFX->setRenderState(GFXRSStencilFunc, GFXCmpAlways);
	GFX->setRenderState(GFXRSStencilRef, 2);
	GFX->setRenderState(GFXRSStencilPass, GFXStencilOpReplace);
	GFX->endStateBlock(mFogMeshSB);

	//mAlphaVColorSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSColorVertex, true);//
	GFX->endStateBlock(mAlphaVColorSB);

	//mFogMesh2SB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSStencilEnable, true);
	GFX->setRenderState(GFXRSStencilFunc, GFXCmpEqual);
	GFX->setRenderState(GFXRSStencilPass, GFXStencilOpZero);
	GFX->setRenderState(GFXRSZEnable, false);
	GFX->endStateBlock(mFogMesh2SB);

	//mClear2SB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSStencilEnable, false);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->endStateBlock(mClear2SB);

	//mClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSStencilEnable, false);
	GFX->setRenderState(GFXRSColorVertex, false);
	GFX->endStateBlock(mClearSB);

	//mZTrueSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->endStateBlock(mZTrueSB);

	//mSetUnderWater
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPDisable);
	GFX->endStateBlock(mSetUnderWater);

	//mClearUnderWaterSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->endStateBlock(mClearUnderWaterSB);

	//mClearUnderWaterSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, false);
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressWrap);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressWrap);
	GFX->setSamplerState(1, GFXSAMPAddressU, GFXAddressWrap);
	GFX->setSamplerState(1, GFXSAMPAddressV, GFXAddressWrap);
	GFX->endStateBlock(mClearUnderWaterSB);
}


void WaterBlock::releaseStateBlock()
{
	if (mSetQuerySB)
	{
		mSetQuerySB->release();
	}

	if (mColorWriteTSB)
	{
		mColorWriteTSB->release();
	}

	if (mSetFilterSB)
	{
		mSetFilterSB->release();
	}

	if (mSetAddr1_1SB)
	{
		mSetAddr1_1SB->release();
	}

	if (mUnderWater1_1SB)
	{
		mUnderWater1_1SB->release();
	}

	if (mFogMeshSB)
	{
		mFogMeshSB->release();
	}

	if (mAlphaVColorSB)
	{
		mAlphaVColorSB->release();
	}

	if (mFogMesh2SB)
	{
		mFogMesh2SB->release();
	}

	if (mClear2SB)
	{
		mClear2SB->release();
	}

	if (mClearSB)
	{
		mClearSB->release();
	}

	if (mZTrueSB)
	{
		mZTrueSB->release();
	}

	if (mSetUnderWater)
	{
		mSetUnderWater->release();
	}

	if (mClearUnderWaterSB)
	{
		mClearUnderWaterSB->release();
	}

	if (mSetAnimSB)
	{
		mSetAnimSB->release();
	}
}

void WaterBlock::init()
{
	if (mSetQuerySB == NULL)
	{
		mSetQuerySB = new GFXD3D9StateBlock;
		mSetQuerySB->registerResourceWithDevice(GFX);
		mSetQuerySB->mZombify = &releaseStateBlock;
		mSetQuerySB->mResurrect = &resetStateBlock;

		mColorWriteTSB = new GFXD3D9StateBlock;
		mSetFilterSB = new GFXD3D9StateBlock;
		mSetAddr1_1SB = new GFXD3D9StateBlock;
		mUnderWater1_1SB = new GFXD3D9StateBlock;
		mFogMeshSB = new GFXD3D9StateBlock;
		mAlphaVColorSB = new GFXD3D9StateBlock;
		mFogMesh2SB = new GFXD3D9StateBlock;
		mClear2SB = new GFXD3D9StateBlock;
		mClearSB = new GFXD3D9StateBlock;
		mZTrueSB = new GFXD3D9StateBlock;
		mSetUnderWater = new GFXD3D9StateBlock;
		mClearUnderWaterSB = new GFXD3D9StateBlock;
		mSetAnimSB = new GFXD3D9StateBlock;

		resetStateBlock();
	}
}

void WaterBlock::shutdown()
{
	SAFE_DELETE(mSetQuerySB);
	SAFE_DELETE(mColorWriteTSB);
	SAFE_DELETE(mSetFilterSB);
	SAFE_DELETE(mSetAddr1_1SB);
	SAFE_DELETE(mUnderWater1_1SB);
	SAFE_DELETE(mFogMeshSB);
	SAFE_DELETE(mAlphaVColorSB);
	SAFE_DELETE(mFogMesh2SB);
	SAFE_DELETE(mClear2SB);
	SAFE_DELETE(mClearSB);
	SAFE_DELETE(mZTrueSB);
	SAFE_DELETE(mSetUnderWater);
	SAFE_DELETE(mClearUnderWaterSB);
	SAFE_DELETE(mSetAnimSB);
}

//addGroup("WaveData");
//addField( "waveDir",       TypePoint2F,  Offset( mWaveDir, WaterBlock ), MAX_WAVES );
//addField( "waveSpeed",     TypeF32,  Offset( mWaveSpeed, WaterBlock ), MAX_WAVES );
//addField( "waveTexScale",  TypePoint2F,  Offset( mWaveTexScale, WaterBlock ), MAX_WAVES );
//endGroup("WaveData");
//
//addGroup("Misc");
//addField( "reflectTexSize", TypeS32,  Offset( mReflectTexSize, WaterBlock ) );
//addField( "baseColor", TypeColorI,  Offset( mBaseColor, WaterBlock ) );
//addField( "underwaterColor", TypeColorI,  Offset( mUnderwaterColor, WaterBlock ) );
//addField( "gridSize", TypeF32,  Offset( mGridElementSize, WaterBlock ) );
//endGroup("Misc");
//
//addField( "surfMaterial", TypeString,  Offset( mSurfMatName, WaterBlock ), NUM_MAT_TYPES );
//addField( "fullReflect",  TypeBool,  Offset( mFullReflect, WaterBlock ) );
//addField( "clarity",  TypeF32,  Offset( mClarity, WaterBlock ) );
//addField( "fresnelBias",  TypeF32,  Offset( mFresnelBias, WaterBlock ) );
//addField( "fresnelPower",  TypeF32,  Offset( mFresnelPower, WaterBlock ) );
//addField( "visibilityDepth", TypeF32, Offset( mVisibilityDepth, WaterBlock ) );
//
//addField( "renderFogMesh",  TypeBool,  Offset( mRenderFogMesh, WaterBlock ) );

void WaterBlock::OnSerialize( CTinyStream& stream )
{
	__super::OnSerialize( stream );

	stream << mWaveDir;
	stream << mWaveSpeed;
	stream << mWaveTexScale;
	stream << mBaseColor;
	stream << mUnderwaterColor;
	stream << mGridElementSize;
	stream << mFullReflect;
	stream << mClarity;
	stream << mFresnelBias;
	stream << mFresnelPower;
	stream << mVisibilityDepth;
	stream << mRenderFogMesh;

	for( int i=0; i<NUM_MAT_TYPES;i++)
		stream.writeString( mSurfMatName[i] );
#pragma message(ENGINE(暂时去掉海岸线的渲染))
    //for ( U32 i = 0; i < MAX_SHORESURFS; i++ )
    //{
    //    stream << mShoreSurfEnable[i];       

    //    stream << mShoreHeight[i];

    //    stream << mSurfRandSeed[i];
    //    stream << mSurfWidth[i];
    //    stream << mSurfWidthRand[i];
    //    stream << mSurfHeight[i];
    //    stream << mSurfHeightRand[i];
    //    stream << mSurfPos[i];
    //    stream << mSurfPosRand[i];
    //    stream << mSurfSpeed[i];
    //    stream << mSurfSpeedRand[i];
    //    stream << mSurfAlpha[i];
    //    stream << mSurfAlphaRand[i];
    //    stream << mSurfMiddle[i];
    //    stream << mSurfMiddleRand[i];

    //    stream.writeString( mSurfTexFilename[i] );
    //}
}

void WaterBlock::OnUnserialize( CTinyStream& stream )
{
	__super::OnUnserialize( stream );

	stream >> mWaveDir;
	stream >> mWaveSpeed;
	stream >> mWaveTexScale;
	stream >> mBaseColor;
	stream >> mUnderwaterColor;
	stream >> mGridElementSize;
	stream >> mFullReflect;
	stream >> mClarity;
	stream >> mFresnelBias;
	stream >> mFresnelPower;
	stream >> mVisibilityDepth;
	stream >> mRenderFogMesh;

	char buf[1024];
	for( int i =0; i < NUM_MAT_TYPES; i++ )
	{
		stream.readString( buf, 1024 );
		mSurfMatName[i] = StringTable->insert( buf );
	}
#pragma message(ENGINE(暂时去掉海岸线的渲染))
    //for ( U32 i = 0; i < MAX_SHORESURFS; i++ )
    //{
    //    stream >> mShoreSurfEnable[i];       

    //    stream >> mShoreHeight[i];

    //    stream >> mSurfRandSeed[i];
    //    stream >> mSurfWidth[i];
    //    stream >> mSurfWidthRand[i];
    //    stream >> mSurfHeight[i];
    //    stream >> mSurfHeightRand[i];
    //    stream >> mSurfPos[i];
    //    stream >> mSurfPosRand[i];
    //    stream >> mSurfSpeed[i];
    //    stream >> mSurfSpeedRand[i];
    //    stream >> mSurfAlpha[i];
    //    stream >> mSurfAlphaRand[i];
    //    stream >> mSurfMiddle[i];
    //    stream >> mSurfMiddleRand[i];

    //    stream.readString( buf, 1024 );
    //    mSurfTexFilename[i] = StringTable->insert( buf );
    //}
}