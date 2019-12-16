//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------
//
// This file contains the interface between the lighting kit and the legacy terrain rendering system.

#include "core/bitVector.h"
#include "sceneGraph/shadowVolumeBSP.h"
#include "sceneGraph/lightingInterfaces.h"
#include "terrain/terrData.h"
#include "lightingSystem/basicLighting/basicLightManager.h"
#include "lightingSystem/common/sceneLighting.h"

extern SceneLighting* gLighting;

//
// Lighting system interface
//
class blTerrainSystem : public SceneLightingInterface
{
public:
   blTerrainSystem();
   virtual void init();
   virtual U32 addObjectType();
   virtual SceneLighting::ObjectProxy* createObjectProxy(SceneObject* obj, SceneLighting::ObjectProxyList* sceneObjects);
   virtual PersistInfo::PersistChunk* createPersistChunk(const U32 chunkType);
   virtual bool createPersistChunkFromProxy(SceneLighting::ObjectProxy* objproxy, PersistInfo::PersistChunk **ret);      

   // Given a ray, this will return the color from the lightmap of this object, return true if handled
   virtual bool getColorFromRayInfo(RayInfo collision, ColorF& result);
};

struct blTerrainChunk : public PersistInfo::PersistChunk
{
   typedef PersistInfo::PersistChunk Parent;

   blTerrainChunk();
   ~blTerrainChunk();

   GBitmap *mLightmap;

   bool read(Stream &);
   bool write(Stream &);
};

//------------------------------------------------------------------------------
// Class SceneLighting::TerrainChunk
//------------------------------------------------------------------------------
blTerrainChunk::blTerrainChunk()
{
   mChunkType = PersistChunk::TerrainChunkType;
   mLightmap = NULL;
}

blTerrainChunk::~blTerrainChunk()
{
   if(mLightmap)
      delete mLightmap;
}

//------------------------------------------------------------------------------

bool blTerrainChunk::read(Stream & stream)
{
   if(!Parent::read(stream))
      return(false);

   mLightmap = new GBitmap();
   return mLightmap->readPNG(stream);
}

bool blTerrainChunk::write(Stream & stream)
{
   if(!Parent::write(stream))
      return(false);

   if(!mLightmap)
      return(false);

   if(!mLightmap->writePNG(stream))
      return(false);

   return(true);
}

class blTerrainProxy : public SceneLighting::ObjectProxy
{
private:
   typedef  ObjectProxy    Parent;

   BitVector               mShadowMask;
   ShadowVolumeBSP *       mShadowVolume;
   ColorF *                mLightmap;


   ColorF *sgBakedLightmap;
   Vector<LightInfo *> sgLights;
   bool sgMarkStaticShadow(void *terrainproxy, SceneObject *sceneobject, LightInfo *light);
   //void postLight(bool lastLight);

   void lightVector(LightInfo *);

   struct SquareStackNode
   {
      U8          mLevel;
      U16         mClipFlags;
      Point2I     mPos;
   };

   S32 testSquare(const Point3F &, const Point3F &, S32, F32, const Vector<PlaneF> &);
   bool markObjectShadow(ObjectProxy *);
   bool sgIsCorrectStaticObjectType(SceneObject *obj);
public:

   blTerrainProxy(SceneObject * obj);
   ~blTerrainProxy();
   TerrainBlock * operator->() {return(static_cast<TerrainBlock*>(static_cast<SceneObject*>(mObj)));}
   TerrainBlock * getObject() {return(static_cast<TerrainBlock*>(static_cast<SceneObject*>(mObj)));}

   bool getShadowedSquares(const Vector<PlaneF> &, Vector<U16> &);

   // lighting
   void init();
   bool preLight(LightInfo *);
   void light(LightInfo *);

   // persist
   U32 getResourceCRC();
   bool setPersistInfo(PersistInfo::PersistChunk *);
   bool getPersistInfo(PersistInfo::PersistChunk *);

   virtual bool supportsShadowVolume();
   virtual void getClipPlanes(Vector<PlaneF>& planes);
   virtual void addToShadowVolume(ShadowVolumeBSP * shadowVolume, LightInfo * light, S32 level);

   // events
   //virtual void processTGELightProcessEvent(U32 curr, U32 max, LightInfo* currlight); 
   //virtual void processSGObjectProcessEvent(LightInfo* currLight);
};

//-------------------------------------------------------------------------------
// Class SceneLighting::TerrainProxy:
//-------------------------------------------------------------------------------
blTerrainProxy::blTerrainProxy(SceneObject * obj) :
Parent(obj)
{
   mLightmap = 0;
}

blTerrainProxy::~blTerrainProxy()
{
   delete [] mLightmap;
}

//-------------------------------------------------------------------------------
void blTerrainProxy::init()
{
   mLightmap = new ColorF[TerrainBlock::LightmapSize * TerrainBlock::LightmapSize];
   dMemset(mLightmap, 0, TerrainBlock::LightmapSize * TerrainBlock::LightmapSize * sizeof(ColorF));
   mShadowMask.setSize(TerrainBlock::BlockSize * TerrainBlock::BlockSize);
}

bool blTerrainProxy::preLight(LightInfo * light)
{
   if(!bool(mObj))
      return(false);

   if(light->mType != LightInfo::Vector)
      return(false);

   mShadowMask.clear();
   return(true);
}

inline ColorF getValue(S32 row, S32 column, ColorF *lmap)
{
   while(row<0)
      row += TerrainBlock::LightmapSize;
   row = row % TerrainBlock::LightmapSize;

   while(column<0)
      column += TerrainBlock::LightmapSize;
   column = column% TerrainBlock::LightmapSize;

   U32 offset = row * TerrainBlock::LightmapSize + column;

   return lmap[offset];
}

bool blTerrainProxy::markObjectShadow(ObjectProxy * proxy)
{
   if (proxy == this)
      return false;
   if (!proxy->supportsShadowVolume())
      return false;

   // setup the clip planes
   Vector<PlaneF> clipPlanes;
   proxy->getClipPlanes(clipPlanes);

   Vector<U16> shadowList;
   if(!getShadowedSquares(clipPlanes, shadowList))
      return(false);

   // set the correct bit
   for(U32 i = 0; i < shadowList.size(); i++)
      mShadowMask.set(shadowList[i]);

   return(true);
}

void blTerrainProxy::light(LightInfo * light)
{

   TerrainBlock * terrain = getObject();
   if(!terrain)
      return;

   AssertFatal(light->mType == LightInfo::Vector, "wrong light type");

   S32 time = Platform::getRealMilliseconds();

   // reset
   mShadowVolume = new ShadowVolumeBSP;

   // build interior shadow volume
   for(ObjectProxy ** itr = gLighting->mLitObjects.begin(); itr != gLighting->mLitObjects.end(); itr++)
   {
      ObjectProxy* objproxy = *itr;
      if (markObjectShadow(objproxy))
         objproxy->addToShadowVolume(mShadowVolume, light, SceneLighting::SHADOW_DETAIL);
   }

   lightVector(light);

   // set the lightmap...
   if(terrain->lightMap) delete terrain->lightMap;
   terrain->lightMap = new GBitmap(TerrainBlock::LightmapSize,TerrainBlock::LightmapSize, 0, GFXFormatR8G8B8A8);

   // Blur...
   F32 kernel[3][3] = { 1, 2, 1,
      2, 4-1, 2,
      1, 2, 1};

   F32 modifier = 1;
   F32 divisor = 0;


   for( U32 i=0; i<3; i++ )
   {
      for( U32 j=0; j<3; j++ )
      {
         if( i==1 && j==1 )
         {
            kernel[i][j] = 1 + kernel[i][j] * modifier;
         }
         else
         {
            kernel[i][j] = kernel[i][j] * modifier;
         }

         divisor += kernel[i][j];
      }
   }

   for( U32 i=0; i<TerrainBlock::LightmapSize; i++ )
   {
      for( U32 j=0; j<TerrainBlock::LightmapSize; j++ )
      {

         ColorF val;
         val  = getValue( i-1, j-1, mLightmap ) * kernel[0][0];
         val += getValue( i-1, j,   mLightmap ) * kernel[0][1];
         val += getValue( i-1, j+1, mLightmap ) * kernel[0][2];
         val += getValue(   i, j-1, mLightmap ) * kernel[1][0];
         val += getValue(   i, j,   mLightmap ) * kernel[1][1];
         val += getValue(   i, j+1, mLightmap ) * kernel[1][2];
         val += getValue( i+1, j-1, mLightmap ) * kernel[2][0];
         val += getValue( i+1, j,   mLightmap ) * kernel[2][1];
         val += getValue( i+1, j+1, mLightmap ) * kernel[2][2];


         U32 edge = 0;

         if( j == 0 || j == TerrainBlock::LightmapSize-1 )
         {
            edge++;
         }
         if( i == 0 || i == TerrainBlock::LightmapSize-1 )
         {
            edge++;
         }

         if( !edge )
         {
            val = val / divisor;
         }
         else
         {
            val = mLightmap[(i)*TerrainBlock::LightmapSize + (j)];
         }

         // clamp values
         mLightmap [ (i * TerrainBlock::LightmapSize) + j]= val;
      }
   }

   // And stuff it into the texture...
   U8 * lPtr = terrain->lightMap->getAddress(0,0);
   for(U32 i = 0; i < (TerrainBlock::LightmapSize * TerrainBlock::LightmapSize); i++)
   {
      lPtr[i*4+0] = mLightmap[i].red   * 256;
      lPtr[i*4+1] = mLightmap[i].green * 256;
      lPtr[i*4+2] = mLightmap[i].blue  * 256;
   }

   terrain->buildMaterialMap();

   delete mShadowVolume;

   Con::printf("    = terrain lit in %3.3f seconds", (Platform::getRealMilliseconds()-time)/1000.f);
}

//------------------------------------------------------------------------------
S32 blTerrainProxy::testSquare(const Point3F & min, const Point3F & max, S32 mask, F32 expand, const Vector<PlaneF> & clipPlanes)
{
   expand = 0;
   S32 retMask = 0;
   Point3F minPoint, maxPoint;
   for(S32 i = 0; i < clipPlanes.size(); i++)
   {
      if(mask & (1 << i))
      {
         if(clipPlanes[i].x > 0)
         {
            maxPoint.x = max.x;
            minPoint.x = min.x;
         }
         else
         {
            maxPoint.x = min.x;
            minPoint.x = max.x;
         }
         if(clipPlanes[i].y > 0)
         {
            maxPoint.y = max.y;
            minPoint.y = min.y;
         }
         else
         {
            maxPoint.y = min.y;
            minPoint.y = max.y;
         }
         if(clipPlanes[i].z > 0)
         {
            maxPoint.z = max.z;
            minPoint.z = min.z;
         }
         else
         {
            maxPoint.z = min.z;
            minPoint.z = max.z;
         }
         F32 maxDot = mDot(maxPoint, clipPlanes[i]);
         F32 minDot = mDot(minPoint, clipPlanes[i]);
         F32 planeD = clipPlanes[i].d;
         if(maxDot <= -(planeD + expand))
            return(U16(-1));
         if(minDot <= -planeD)
            retMask |= (1 << i);
      }
   }
   return(retMask);
}

bool blTerrainProxy::getShadowedSquares(const Vector<PlaneF> & clipPlanes, Vector<U16> & shadowList)
{
   TerrainBlock * terrain = getObject();
   if(!terrain)
      return(false);

   SquareStackNode stack[TerrainBlock::BlockShift * 4];

   stack[0].mLevel = TerrainBlock::BlockShift;
   stack[0].mClipFlags = 0xff;
   stack[0].mPos.set(0,0);

   U32 stackSize = 1;

   Point3F blockPos;
   terrain->getTransform().getColumn(3, &blockPos);
   S32 squareSize = terrain->getSquareSize();

   bool marked = false;

   // push through all the levels of the quadtree
   while(stackSize)
   {
      SquareStackNode * node = &stack[stackSize - 1];

      S32 clipFlags = node->mClipFlags;
      Point2I pos = node->mPos;
      GridSquare * sq = terrain->findSquare(node->mLevel, pos);

      Point3F minPoint, maxPoint;
      minPoint.set(squareSize * pos.x + blockPos.x,
         squareSize * pos.y + blockPos.y,
         fixedToFloat(sq->minHeight));
      maxPoint.set(minPoint.x + (squareSize << node->mLevel),
         minPoint.y + (squareSize << node->mLevel),
         fixedToFloat(sq->maxHeight));

      // test the square against the current level
      if(clipFlags)
      {
         clipFlags = testSquare(minPoint, maxPoint, clipFlags, squareSize, clipPlanes);
         if(clipFlags == U16(-1))
         {
            stackSize--;
            continue;
         }
      }

      // shadowed?
      if(node->mLevel == 0)
      {
         marked = true;
         shadowList.push_back(pos.x + (pos.y << TerrainBlock::BlockShift));
         stackSize--;
         continue;
      }

      // setup the next level of squares
      U8 nextLevel = node->mLevel - 1;
      S32 squareHalfSize = 1 << nextLevel;

      for(U32 i = 0; i < 4; i++)
      {
         node[i].mLevel = nextLevel;
         node[i].mClipFlags = clipFlags;
      }

      node[3].mPos = pos;
      node[2].mPos.set(pos.x + squareHalfSize, pos.y);
      node[1].mPos.set(pos.x, pos.y + squareHalfSize);
      node[0].mPos.set(pos.x + squareHalfSize, pos.y + squareHalfSize);

      stackSize += 3;
   }

   return(marked);
}

//-------------------------------------------------------------------------------
// BUGS: does not work with x or y directions of 0
//    : light dir of 0.1, 0.3, -0.8 causes strange behavior
void blTerrainProxy::lightVector(LightInfo * light)
{
   TerrainBlock * terrain = getObject();
   if(!terrain)
      return;

   Point3F lightDir = light->mDirection;
   lightDir.normalize();

   ColorF & ambient = light->mAmbient;
   ColorF & lightColor = light->mColor;

   if(lightDir.x == 0 && lightDir.y == 0)
      return;

   S32 generateLevel = Con::getIntVariable("$pref::sceneLighting::terrainGenerateLevel", 4);
   generateLevel = mClamp(generateLevel, 0, 4);

   bool allowLexelSplits = Con::getBoolVariable("$pref::sceneLighting::terrainAllowLexelSplits", false);

   U32 generateDim = TerrainBlock::LightmapSize << generateLevel;
   U32 generateShift = TerrainBlock::LightmapShift + generateLevel;
   U32 generateMask = generateDim - 1;

   F32 zStep;
   F32 frac;

   Point2I blockColStep;
   Point2I blockRowStep;
   Point2I blockFirstPos;
   Point2I lmFirstPos;

   F32 stepSize = F32(terrain->getSquareSize()) / F32(generateDim / TerrainBlock::BlockSize);

   if(mFabs(lightDir.x) >= mFabs(lightDir.y))
   {
      if(lightDir.x > 0)
      {
         zStep = lightDir.z / lightDir.x;
         frac = lightDir.y / lightDir.x;

         blockColStep.set(1, 0);
         blockRowStep.set(0, 1);
         blockFirstPos.set(0, 0);
         lmFirstPos.set(0,0);
      }
      else
      {
         zStep = -lightDir.z / lightDir.x;
         frac = -lightDir.y / lightDir.x;

         blockColStep.set(-1, 0);
         blockRowStep.set(0, 1);
         blockFirstPos.set(255, 0);
         lmFirstPos.set(TerrainBlock::LightmapSize - 1,0);
      }
   }
   else
   {
      if(lightDir.y > 0)
      {
         zStep = lightDir.z / lightDir.y;
         frac = lightDir.x / lightDir.y;

         blockColStep.set(0, 1);
         blockRowStep.set(1, 0);
         blockFirstPos.set(0, 0);
         lmFirstPos.set(0,0);
      }
      else
      {
         zStep = -lightDir.z / lightDir.y;
         frac = -lightDir.x / lightDir.y;

         blockColStep.set(0, -1);
         blockRowStep.set(1, 0);
         blockFirstPos.set(0, 255);
         lmFirstPos.set(0, TerrainBlock::LightmapSize - 1);
      }
   }
   zStep *= stepSize;

   F32 * heightArray = new F32[generateDim];

   S32 fracStep = -1;
   if(frac < 0)
   {
      fracStep = 1;
      frac = -frac;
   }

   F32 * nextHeightArray = new F32[generateDim];
   F32 oneMinusFrac = 1 - frac;

   U32 blockShift = generateShift - TerrainBlock::BlockShift;
   U32 lightmapShift = generateShift - TerrainBlock::LightmapShift;

   U32 blockStep = 1 << blockShift;
   U32 blockMask = blockStep - 1;
   U32 lightmapStep = 1 << lightmapShift;
   U32 lightmapNormalOffset = lightmapStep >> 1;
   U32 lightmapMask = lightmapStep - 1;

   Point2I bp = blockFirstPos;
   F32 terrainHeights[2][TerrainBlock::BlockSize];
   U32 i;

   F32 * pTerrainHeights = static_cast<F32*>(terrainHeights[0]);
   F32 * pNextTerrainHeights = static_cast<F32*>(terrainHeights[1]);

   // get first set of heights
   for(i = 0; i < TerrainBlock::BlockSize; i++) {
      pTerrainHeights[i] = fixedToFloat(terrain->getHeight(bp.x, bp.y));
      bp += blockRowStep;
   }

   // get second set of heights
   bp = blockFirstPos + blockColStep;
   for(i = 0; i < TerrainBlock::BlockSize; i++) {
      pNextTerrainHeights[i] = fixedToFloat(terrain->getHeight(bp.x, bp.y));
      bp += blockRowStep;
   }

   F32 heightStep = 1.f / blockStep;

   F32 terrainZRowStep[TerrainBlock::BlockSize];
   F32 nextTerrainZRowStep[TerrainBlock::BlockSize];
   F32 terrainZColStep[TerrainBlock::BlockSize];

   // fill in the row steps
   for(i = 0; i < TerrainBlock::BlockSize; i++)
   {
      terrainZRowStep[i] = (pTerrainHeights[(i+1) & TerrainBlock::BlockMask] - pTerrainHeights[i]) * heightStep;
      nextTerrainZRowStep[i] = (pNextTerrainHeights[(i+1) & TerrainBlock::BlockMask] - pNextTerrainHeights[i]) * heightStep;
      terrainZColStep[i] = (pNextTerrainHeights[i] - pTerrainHeights[i]) * heightStep;
   }

   // get first row of process heights
   for(i = 0; i < generateDim; i++)
   {
      U32 bi = i >> blockShift;
      heightArray[i] = pTerrainHeights[bi] + (i & blockMask) * terrainZRowStep[bi];
   }

   bp = blockFirstPos;
   if(generateDim == TerrainBlock::BlockSize)
      bp += blockColStep;

   // generate the initial run
   U32 x, y;
   for(x = 1; x < generateDim; x++)
   {
      U32 xmask = x & blockMask;

      // generate new height step rows?
      if(!xmask)
      {
         F32 * tmp = pTerrainHeights;
         pTerrainHeights = pNextTerrainHeights;
         pNextTerrainHeights = tmp;

         bp += blockColStep;

         Point2I bwalk = bp;
         for(i = 0; i < TerrainBlock::BlockSize; i++, bwalk += blockRowStep)
            pNextTerrainHeights[i] = fixedToFloat(terrain->getHeight(bwalk.x, bwalk.y));

         // fill in the row steps
         for(i = 0; i < TerrainBlock::BlockSize; i++)
         {
            terrainZRowStep[i] = (pTerrainHeights[(i+1) & TerrainBlock::BlockMask] - pTerrainHeights[i]) * heightStep;
            nextTerrainZRowStep[i] = (pNextTerrainHeights[(i+1) & TerrainBlock::BlockMask] - pNextTerrainHeights[i]) * heightStep;
            terrainZColStep[i] = (pNextTerrainHeights[i] - pTerrainHeights[i]) * heightStep;
         }
      }

      Point2I bwalk = bp - blockRowStep;
      for(y = 0; y < generateDim; y++)
      {
         U32 ymask = y & blockMask;
         if(!ymask)
            bwalk += blockRowStep;

         U32 bi = y >> blockShift;
         U32 binext = (bi + 1) & TerrainBlock::BlockMask;

         F32 height;

         // 135?
         if((bwalk.x ^ bwalk.y) & 1)
         {
            U32 xsub = blockStep - xmask;
            if(xsub > ymask) // bottom
               height = pTerrainHeights[bi] + xmask * terrainZColStep[bi] +
               ymask * terrainZRowStep[bi];
            else // top
               height = pNextTerrainHeights[bi] - xmask * terrainZColStep[binext] +
               ymask * nextTerrainZRowStep[bi];
         }
         else
         {
            if(xmask > ymask) // bottom
               height = pTerrainHeights[bi] + xmask * terrainZColStep[bi] +
               ymask * nextTerrainZRowStep[bi];
            else // top
               height = pTerrainHeights[bi] + xmask * terrainZColStep[binext] +
               ymask * terrainZRowStep[bi];
         }

         F32 intHeight = heightArray[y] * oneMinusFrac + heightArray[(y + fracStep) & generateMask] * frac + zStep;
         nextHeightArray[y] = getMax(height, intHeight);
      }

      // swap the height rows
      F32 * tmp = heightArray;
      heightArray = nextHeightArray;
      nextHeightArray = tmp;
   }

   F32 squareSize = terrain->getSquareSize();
   F32 lexelDim = squareSize * F32(TerrainBlock::BlockSize) / F32(TerrainBlock::LightmapSize);

   // calculate normal runs
   Point3F normals[2][TerrainBlock::BlockSize];

   Point3F * pNormals = static_cast<Point3F*>(normals[0]);
   Point3F * pNextNormals = static_cast<Point3F*>(normals[1]);

   // calculate the normal lookup table
   F32 * normTable = new F32 [blockStep * blockStep * 4];

   Point2F corners[4] = {
      Point2F(0.f, 0.f),
      Point2F(1.f, 0.f),
      Point2F(1.f, 1.f),
      Point2F(0.f, 1.f)
   };

   U32 idx = 0;
   F32 step = 1.f / blockStep;
   Point2F pos(0.f, 0.f);

   // fill it
   for(x = 0; x < blockStep; x++, pos.x += step, pos.y = 0.f) {
      for(y = 0; y < blockStep; y++, pos.y += step) {
         for(i = 0; i < 4; i++, idx++)
            normTable[idx] = 1.f - getMin(Point2F(pos - corners[i]).len(), 1.f);
      }
   }

   // fill first column
   bp = blockFirstPos;
   for(x = 0; x < TerrainBlock::BlockSize; x++)
   {
      pNextTerrainHeights[x] = fixedToFloat(terrain->getHeight(bp.x, bp.y));
      Point2F pos(bp.x * squareSize, bp.y * squareSize);
      terrain->getNormal(pos, &pNextNormals[x]);
      bp += blockRowStep;
   }

   // get swapped on first pass
   pTerrainHeights = static_cast<F32*>(terrainHeights[1]);
   pNextTerrainHeights = static_cast<F32*>(terrainHeights[0]);

   // get the world offset of the terrain
   const MatrixF & transform = terrain->getTransform();
   Point3F worldOffset;
   transform.getColumn(3, &worldOffset);

   F32 ratio = F32(1 << lightmapShift);

   F32 lightScale[TerrainBlock::LightmapSize];

   // walk it...
   bp = blockFirstPos - blockColStep;
   for(x = 0; x < generateDim; x++)
   {
      U32 xmask = x & blockMask;
      U32 lxmask = x & lightmapMask;

      // generate new runs?
      if(!xmask)
      {
         bp += blockColStep;

         // do the normals
         Point3F * temp = pNormals;
         pNormals = pNextNormals;
         pNextNormals = temp;

         // fill the row
         Point2I bwalk = bp + blockColStep;
         for(i = 0; i < TerrainBlock::BlockSize; i++)
         {
            Point2F pos(bwalk.x * squareSize, bwalk.y * squareSize);
            terrain->getNormal(pos, &pNextNormals[i]);
            bwalk += blockRowStep;
         }

         // do the heights
         F32 * tmp = pTerrainHeights;
         pTerrainHeights = pNextTerrainHeights;
         pNextTerrainHeights = tmp;

         bwalk = bp + blockColStep;
         for(i = 0; i < TerrainBlock::BlockSize; i++, bwalk += blockRowStep)
            pNextTerrainHeights[i] = fixedToFloat(terrain->getHeight(bwalk.x, bwalk.y));

         // fill in the row steps
         for(i = 0; i < TerrainBlock::BlockSize; i++)
         {
            terrainZRowStep[i] = (pTerrainHeights[(i+1) & TerrainBlock::BlockMask] - pTerrainHeights[i]) * heightStep;
            nextTerrainZRowStep[i] = (pNextTerrainHeights[(i+1) & TerrainBlock::BlockMask] - pNextTerrainHeights[i]) * heightStep;
            terrainZColStep[i] = (pNextTerrainHeights[i] - pTerrainHeights[i]) * heightStep;
         }
      }

      // reset the light scale table
      if(!lxmask)
         for(i = 0; i < TerrainBlock::LightmapSize; i++)
            lightScale[i] = 1.f;

      Point2I bwalk = bp - blockRowStep;
      for(y = 0; y < generateDim; y++)
      {
         U32 lymask = y & lightmapMask;
         U32 ymask = y & blockMask;
         if(!ymask)
            bwalk += blockRowStep;

         U32 bi = y >> blockShift;
         U32 binext = (bi + 1) & TerrainBlock::BlockMask;

         F32 height;
         F32 xstep, ystep;

         // 135?
         if((bwalk.x ^ bwalk.y) & 1)
         {
            U32 xsub = blockStep - xmask;
            if(xsub > ymask) // bottom
            {
               xstep = terrainZColStep[bi];
               ystep = terrainZRowStep[bi];
               height = pTerrainHeights[bi] + xmask * xstep + ymask * ystep;
            }
            else // top
            {
               xstep = -terrainZColStep[binext];
               ystep = nextTerrainZRowStep[bi];
               height = pNextTerrainHeights[bi] + xsub * xstep + ymask * ystep;
            }
         }
         else
         {
            if(xmask > ymask) // bottom
            {
               xstep = terrainZColStep[bi];
               ystep = nextTerrainZRowStep[bi];
               height = pTerrainHeights[bi] + xmask * xstep + ymask * ystep;
            }
            else // top
            {
               xstep = terrainZColStep[binext];
               ystep = terrainZRowStep[bi];
               height = pTerrainHeights[bi] + xmask * xstep + ymask * ystep;
            }
         }

         F32 intHeight = heightArray[y] * oneMinusFrac + heightArray[(y + fracStep) & generateMask] * frac + zStep;

         U32 lsi = y >> lightmapShift;
         /* ColorF & col = mLightmap[(x >> lightmapShift) + (lsi << TerrainBlock::LightmapShift)]; */
         Point2I lmPos = lmFirstPos + blockColStep * (x >> lightmapShift) + blockRowStep * lsi;
         ColorF & col = mLightmap[lmPos.x + (lmPos.y << TerrainBlock::LightmapShift)];

         // lexel shaded by an interior?
         Point2I terrPos = lmPos;
         terrPos.x >>= TerrainBlock::LightmapShift - TerrainBlock::BlockShift;
         terrPos.y >>= TerrainBlock::LightmapShift - TerrainBlock::BlockShift;

         if(!lxmask && !lymask && mShadowMask.test(terrPos.x + (terrPos.y << TerrainBlock::BlockShift)))
         {
            U32 idx = (xmask + lightmapNormalOffset + ((ymask + lightmapNormalOffset) << blockShift)) << 2;

            // get the normal for this lexel
            Point3F normal = pNormals[bi] * normTable[idx++];
            normal += pNormals[binext] * normTable[idx++];
            normal += pNextNormals[binext] * normTable[idx++];
            normal += pNextNormals[bi] * normTable[idx];
            normal.normalize();

            nextHeightArray[y] = height;
            F32 colorScale = -mDot(normal, lightDir);

            if(colorScale > 0.f)
            {
               // split lexels which cross the square split?
               if(allowLexelSplits)
               {
                  // jff:todo
               }
               else
               {
                  Point2F wPos((lmPos.x) * lexelDim + worldOffset.x,
                     (lmPos.y) * lexelDim + worldOffset.y);

                  F32 xh = xstep * ratio;
                  F32 yh = ystep * ratio;

                  ShadowVolumeBSP::SVPoly * poly = mShadowVolume->createPoly();
                  poly->mWindingCount = 4;
                  poly->mWinding[0].set(wPos.x, wPos.y, height);
                  poly->mWinding[1].set(wPos.x + lexelDim, wPos.y, height + xh);
                  poly->mWinding[2].set(wPos.x + lexelDim, wPos.y + lexelDim, height + xh + yh);
                  poly->mWinding[3].set(wPos.x, wPos.y + lexelDim, height + yh);
                  poly->mPlane.set(poly->mWinding[0], poly->mWinding[1], poly->mWinding[2]);

                  F32 lexelSize = mShadowVolume->getPolySurfaceArea(poly);
                  F32 intensity = mShadowVolume->getClippedSurfaceArea(poly) / lexelSize;
                  lightScale[lsi] = mClampF(intensity, 0.f, 1.f);
               }
            }
            else
               lightScale[lsi] = 0.f;
         }

         // non shadowed?
         if(height >= intHeight)
         {
            U32 idx = (xmask + (ymask << blockShift)) << 2;

            Point3F normal = pNormals[bi] * normTable[idx++];
            normal += pNormals[binext] * normTable[idx++];
            normal += pNextNormals[binext] * normTable[idx++];
            normal += pNextNormals[bi] * normTable[idx];
            normal.normalize();

            nextHeightArray[y] = height;
            F32 colorScale = -mDot(normal, lightDir);

            if(colorScale > 0.f)
               col = ambient + lightColor * colorScale * lightScale[lsi];
            else
               col = ambient;
         }
         else
         {
            nextHeightArray[y] = intHeight;
            col = ambient;
         }
      }

      F32 * tmp = heightArray;
      heightArray = nextHeightArray;
      nextHeightArray = tmp;
   }

   // set the proper color
   for(i = 0; i < TerrainBlock::LightmapSize * TerrainBlock::LightmapSize; i++)
   {
      //mLightmap[i] *= inverseRatioSquared;
      mLightmap[i] /= 2.0; //TERRAIN_OVERRANGE;
      mLightmap[i].clamp();
   }

   delete [] normTable;
   delete [] heightArray;
   delete [] nextHeightArray;
}

//--------------------------------------------------------------------------
U32 blTerrainProxy::getResourceCRC()
{
   TerrainBlock * terrain = getObject();
   if(!terrain)
      return(0);
   return(terrain->getCRC());
}

//--------------------------------------------------------------------------
bool blTerrainProxy::setPersistInfo(PersistInfo::PersistChunk * info)
{
   if(!Parent::setPersistInfo(info))
      return(false);

   blTerrainChunk * chunk = dynamic_cast<blTerrainChunk*>(info);
   AssertFatal(chunk, "blTerrainProxy::setPersistInfo: invalid info chunk!");

   TerrainBlock * terrain = getObject();
   if(!terrain || !terrain->lightMap)
      return(false);

   if(terrain->lightMap) delete terrain->lightMap;

   terrain->lightMap = new GBitmap( *chunk->mLightmap);

   terrain->buildMaterialMap();
   return(true);
}

bool blTerrainProxy::getPersistInfo(PersistInfo::PersistChunk * info)
{
   if(!Parent::getPersistInfo(info))
      return(false);

   blTerrainChunk * chunk = dynamic_cast<blTerrainChunk*>(info);
   AssertFatal(chunk, "blTerrainProxy::getPersistInfo: invalid info chunk!");

   TerrainBlock * terrain = getObject();
   if(!terrain || !terrain->lightMap)
      return(false);

   if(chunk->mLightmap) delete chunk->mLightmap;

   chunk->mLightmap = new GBitmap(*terrain->lightMap);

   return(true);
}

bool blTerrainProxy::supportsShadowVolume()
{
   return true;
}

void blTerrainProxy::getClipPlanes(Vector<PlaneF>& planes)
{

}

void blTerrainProxy::addToShadowVolume(ShadowVolumeBSP * shadowVolume, LightInfo * light, S32 level)
{

}


//
// blTerrainSystem
//
blTerrainSystem::blTerrainSystem()
{
   BLM->getSceneLightingInterface()->registerSystem(this);   
}

void blTerrainSystem::init()
{
}

U32 blTerrainSystem::addObjectType()
{
   return TerrainObjectType;
}

SceneLighting::ObjectProxy* blTerrainSystem::createObjectProxy(SceneObject* obj, SceneLighting::ObjectProxyList* sceneObjects)
{
   if ((obj->getTypeMask() & TerrainObjectType) != 0)
      return new blTerrainProxy(obj);
   else
      return NULL;
}

PersistInfo::PersistChunk* blTerrainSystem::createPersistChunk(const U32 chunkType)
{
   if (chunkType == PersistInfo::PersistChunk::TerrainChunkType)
      return new blTerrainChunk();
   else
      return NULL;
}

bool blTerrainSystem::createPersistChunkFromProxy(SceneLighting::ObjectProxy* objproxy, PersistInfo::PersistChunk **ret)
{
   if (dynamic_cast<blTerrainProxy*>(objproxy) != NULL)
   {
      *ret = new blTerrainChunk();
      return true;
   } else {
      return NULL;
   }
}

// Given a ray, this will return the color from the lightmap of this object, return true if handled
bool blTerrainSystem::getColorFromRayInfo(RayInfo collision, ColorF& result)
{
   TerrainBlock *terrain = dynamic_cast<TerrainBlock *>(collision.object);
   if (!terrain)
      return false;

   Point2F uv;
   F32 terrainlength = (F32)(terrain->getSquareSize() * TerrainBlock::BlockSize);
   uv.x = (collision.point.x + (terrainlength * 0.5)) / terrainlength;
   uv.y = (collision.point.y + (terrainlength * 0.5)) / terrainlength;
   // similar to x = x & width...
   uv.x = uv.x - F32(U32(uv.x));
   uv.y = uv.y - F32(U32(uv.y));
   GBitmap* lightmap = terrain->lightMap;
   if (!lightmap)
      return false;

   result = lightmap->sampleTexel(uv.x, uv.y);
   // terrain lighting is dim - look into this (same thing done in shaders)...
   result *= 2.0;
   return true;
}

// File private static factory
static blTerrainSystem p_blTerrainSystem;