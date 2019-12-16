//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _TERRDATA_H_
#define _TERRDATA_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _MPOINT_H_
#include "math/mPoint.h"
#endif
#ifndef _RENDERABLESCENEOBJECT_H
#include "sceneGraph/RenderableSceneObject.h"
#endif
#ifndef _RESMANAGER_H_
#include "core/resManager.h"
#endif
#ifndef _CONVEX_H_
#include "collision/convex.h"
#endif

#include "materials/materialList.h"
#include "gfx/gfxDevice.h"

#include "clipmap/clipMap.h"
#include "clipmap/clipMapBlenderCache.h"
#include "terrain/terrClipMapImageSource.h"

#pragma message(ENGINE(添加std::string头文件))
#include <string>

class GBitmap;
class TerrainFile;
class TerrainBlock;
class ColorF;
class Blender;
class GFXStateBlock;

//---RBP Global declaration of a function to retrieve the terrain under a given point
TerrainBlock* getTerrainUnderWorldPoint(const Point3F & wPos);
//--------------------------------------------------------------------------

class TerrainConvex: public Convex
{
   friend class TerrainBlock;
   TerrainConvex *square;     ///< Alternate convex if square is concave
   bool halfA;                ///< Which half of square
   bool split45;              ///< Square split pattern
   U32 squareId;              ///< Used to match squares
   U32 material;
   Point3F point[4];          ///< 3-4 vertices
   VectorF normal[2];
   Box3F box;                 ///< Bounding box

  public:
   TerrainConvex() 
   {
      mType = TerrainConvexType; 
   }
   
   TerrainConvex(const TerrainConvex& cv) 
   {
      mType = TerrainConvexType;

      // Only a partial copy...
      mObject = cv.mObject;
      split45 = cv.split45;
      squareId = cv.squareId;
      material = cv.material;
      point[0] = cv.point[0];
      point[1] = cv.point[1];
      point[2] = cv.point[2];
      point[3] = cv.point[3];
      normal[0] = cv.normal[0];
      normal[1] = cv.normal[1];
      box = cv.box;
   }
   Box3F getBoundingBox() const;
   Box3F getBoundingBox(const MatrixF& mat, const Point3F& scale) const;
   Point3F support(const VectorF& v) const;
   void getFeatures(const MatrixF& mat,const VectorF& n, ConvexFeature* cf);
   void getPolyList(AbstractPolyList* list);
};



//--------------------------------------------------------------------------
struct GridSquare
{
   U16 minHeight;
   U16 maxHeight;
   U16 heightDeviance;
   U16 flags;
   enum 
   {
      Split45 = 1,
      Empty = 2,
      HasEmpty = 4,
      MaterialShift = 3,
      MaterialStart = 8,
      Material0 = 8,
      Material1 = 16,
      Material2 = 32,
      Material3 = 64,
   };
};

struct GridChunk
{
   U16 heightDeviance[3]; // levels 0-1, 1-2, 2
   U16 emptyFlags;
};

void sgProcessLightMap(TerrainBlock *terrain, ColorF *mLightmap, ColorF *sgBakedLightmap);
void sgProcessLightMapOptimize(TerrainBlock *terrain, ColorF *mLightmap, ColorF *sgBakedLightmap);
//--------------------------------------------------------------------------
//范围标记：像素的alpha值
//0-0.3标识该像素可以被太阳直接照到					一般设像素的 alpha = 0.0
//0.3-0.6标识该像素不能被太阳直接照到，为阴影	一般设像素的 alpha = 0.5
//0.6-1.0标识该像素是刷的颜色							一般设像素的 alpha = 1.0
#define MIN_SUN_ALPHA			0.0 
#define MIN_SHADOW_ALPHA		0.3
#define MIN_BRUSH_COLOR_ALPHA 0.6
#define SUN_ALPHA					0.0 
#define SHADOW_ALPHA				0.5
#define BRUSH_COLOR_ALPHA		1.0
class TerrainBlock : public RenderableSceneObject
{
   typedef RenderableSceneObject Parent;
	bool unInit;
  public:
   struct Material 
   {
      enum Flags 
      {
         Plain          = 0,
         Rotate         = 1,
         FlipX          = 2,
         FlipXRotate    = 3,
         FlipY          = 4,
         FlipYRotate    = 5,
         FlipXY         = 6,
         FlipXYRotate   = 7,
         RotateMask     = 7,
         Empty          = 8,
         Modified       = BIT(7),

         // must not clobber TerrainFile::MATERIAL_GROUP_MASK bits!
         PersistMask       = BIT(7)
      };
      U8 flags;
      U8 index;
   };
   
   enum 
   {
      TerrainSize = 4,		///< 目前地形最大可以有4*4块
      BlockSize = 256,
      BlockShift = 8,
      LightmapSize = 1024,
      LightmapShift = 10,
      ChunkSquareWidth = 64,
      ChunkSize = 4,
      ChunkDownShift = 2,
		ChunkShiftMask = 3,
		ChunkMask = 63,
      ChunkShift = BlockShift - ChunkDownShift,
      BlockSquareWidth = 256,
      SquareMaxPoints = 1024,
      BlockMask = 255,
      GridMapSize = 0x15555,
      FlagMapWidth = 128, ///< Flags that map is for 2x2 squares.
      FlagMapMask = 127,
      MaxMipLevel = 6,
      NumBaseTextures = 16,
      MaterialGroups = 50,
      MaxEmptyRunPairs = 100,
		Max_Textures = 5,
		Max_Texture_Layer = 4,
		Blend_Texture_Num = MaterialGroups/4 + 1
   };

   enum UpdateMaskBits
   {
      InitMask           = BIT(1),
      VisibilityMask     = BIT(2),
      TerrainChangedMask = BIT(3),
      EmptyMask          = BIT(4),
      MoveMask           = BIT(5),
   };

   enum Edge
   {
      EDGE_LEFT = 0,
      EDGE_TOP,
      EDGE_RIGHT,
      EDGE_BOTTOM,

      EDGE_MAX,
   };

   //GFXTexHandle baseTextures[NumBaseTextures];
   GFXTexHandle mBaseMaterials[MaterialGroups];
	GFXTexHandle mBlendTex[Blend_Texture_Num];
   //GFXTexHandle mAlphaMaterials[MaterialGroups];
	//chunk纹理，0-3：基础纹理；4：混合纹理；(6：细节纹理；7：光度图)
	GFXTexHandle mTextures[ChunkSize*ChunkSize][Max_Textures];
	//U8 mTexIndex[ChunkSize*ChunkSize][Max_Texture_Layer];//纹理层
	//U8* mTexAlpha[ChunkSize*ChunkSize][Max_Textures];//每个纹理对应的alpha值
	//in: x,y为点的世界坐标
	//out: 点(x,y)所在的subTerrainBlock的索引
	U8 getSubBlockIndex(F32 x, F32 y);
	//in: x,y为square坐标
	//out: square(x,y)所在的subTerrainBlock的索引
	U8 getSubBlockIndex(U32 x, U32 y);
	//in: index-子地块的索引
	//out: 返回子地块的偏移坐标(最小坐标)
	Point3F getSubBlockOffset(U8 index);
	//更新子块纹理
	U32 mTexNum;
	void updateBlendTexture();
	void updateTexture();
	void reloadTexture();//重载地形基础纹理
	void setReloadTexture(bool needReload){mNeedReloadTexture = needReload;}
	bool mNeedReloadTexture;

   bool mbEnableUse;
   Box3F mBounds;

   GBitmap *lightMap;
#pragma message(ENGINE(阳光颜色改变时，实时刷新地形的光度图))
	//记录rgb:该点bakedLightmap的rgb值，a:该点colorScale值，格式为：32bits
	//用途：当阳光颜色改变时，根据该数据刷新lightMap
  ColorF *bakedlightmapAndcolorScale;
  F32	*mShine;//记录每个像素使用太阳的哪个颜色计算光照,0则用mShadowColor, 1则使用mColor
  ColorF* mShadowMap;
	GFXTexHandle tex, tex2, tex3, tex4;//tex4(r-mShine, g-colorScale, b-mShadowMap)
	static bool first;

	void freshLightmapByShader(ColorF ambient, ColorF frontColor, ColorF backColor, bool bScene = false);//计算光照

	bool refreshLightmap();//更新光度图
   StringTableEntry *mMaterialFileName; ///< Array from the file.

   static void texManagerCallback( GFXTexCallbackCode code, void *userData );
   S32 mTexCallback;

   U8 *mBaseMaterialMap;
   Material *materialMap;

   // fixed point height values
   U16 *heightMap;
   U16 *flagMap;
   StringTableEntry mDetailTextureName;
   GFXTexHandle mDetailTextureHandle;

   StringTableEntry mTerrFileName;
   Vector<S32> mEmptySquareRuns;
   bool mHasRendered;

   S32 mMPMIndex[10];
   F32 mDetailDistance;
   F32 mDetailScale;
   F32 mDetailBrightness;

   S32 mVertexBuffer;
   Convex sTerrainConvexList;

	Vector<GFXTexHandle> mOpacityTexs;

   VectorPtr<TerrClipMapImageSource*> mOpacitySources;
   TerrClipMapImageSource *mLightmapSource;
   ClipMapBlenderCache *mClipBlender;
   ClipMap *mClipMap;

   enum 
   {
      LightmapUpdate    = BIT(0),
      HeightmapUpdate   = BIT(1),
      OpacityUpdate     = BIT(2),
      EmptyUpdate       = BIT(3)
   };

   Signal<U32,const Point2I& ,const Point2I&> mUpdateSignal;


   void updateOpacity();
   void addOpacitySources();

#ifdef NTJ_CLIENT
   static bool smLightingDone;
   static U32 smCurTerCRC;
#endif
   
  private:
   Resource<TerrainFile> mFile;
   GridSquare *gridMap[BlockShift+1];
   GridChunk *mChunkMap;
   U32   mCRC;

  public:
   TerrainBlock();
   ~TerrainBlock();

   void OnSerialize( CTinyStream& stream );
   void OnUnserialize( CTinyStream& stream );

   void buildChunkDeviance(S32 x, S32 y);
   void buildGridMap();
   U32 getCRC() { return(mCRC); }
   Resource<TerrainFile> getFile() { return mFile; };
   Resource<TerrainFile>& getFileRef() { return mFile; };
   bool onAdd();
   void onRemove();

   void refreshMaterialLists();
   void onEditorEnable();
   void onEditorDisable();

   void rebuildEmptyFlags();
   bool unpackEmptySquares();
   void packEmptySquares();

   void inspectPostApply();

   Material *getMaterial(U32 x, U32 y);
   GridSquare *findSquare(U32 level, Point2I pos);
   GridSquare *findSquare(U32 level, S32 x, S32 y);
   GridChunk *findChunk(Point2I pos);

   void calcBounds();

   void setHeight(const Point2I & pos, float height);
   void updateGrid(Point2I min, Point2I max);
   void updateGridMaterials(Point2I min, Point2I max);

   U16 getHeight(U32 x, U32 y); 

   U16 *getHeightAddress(U32 x, U32 y) 
   {
      return &heightMap[(x & BlockMask) + ((y & BlockMask) << BlockShift)]; 
   }
   
   void setBaseMaterial(U32 x, U32 y, U8 matGroup);

   U8 *getMaterialAlphaMap(U8 subTerr, U32 matIndex);	
   U8 *getMaterialAlphaMap(U32 matIndex);
   U8* getBaseMaterialAddress(U32 x, U32 y);
   U8  getBaseMaterial(U32 x, U32 y);
   U16 *getFlagMapPtr(S32 x, S32 y);

   void getMaterialAlpha(Point2I pos, U8 alphas[MaterialGroups]);
   void setMaterialAlpha(Point2I pos, const U8 alphas[MaterialGroups]);

	void setShadow(const Point2I & pos, ColorF shadow);
	ColorF		getShadow(const Point2I &pos);
#pragma message(ENGINE(添加函数，获得地形上指定点使用的所有纹理名称和透明度值(alpha>0)))
   //给定地形上一点，获取该点使用的所有纹理及对应的透明度
   //@param pos 地形上的网格点
   //@param alphas 纹理的透明度，其有效索引范围为[0, 返回值-1]
   //@param materials 纹理的文件名，其透明度在参数alphas的相应位置记录，其有效索引范围为[0, 返回值-1]
   //@return 返回该点所使用的纹理数目，若为0则表示未使用纹理
   S32 getMaterialAndAlpha(Point2I pos, Vector<U8>& alphas, Vector<std::string>& materials);

   // a more useful getHeight for the public...
   bool getHeight(const Point2F & pos, F32 * height);
   void getMinMaxHeight( F32* minHeight, F32* maxHeight ) const ;
   bool getNormal(const Point2F & pos, Point3F * normal, bool normalize = true);
   bool getNormalAndHeight(const Point2F & pos, Point3F * normal, F32 * height, bool normalize = true);

   // only the editor currently uses this method - should always be using a ray to collide with
   bool collideBox(const Point3F &start, const Point3F &end, RayInfo* info)
   {
      return(castRay(start,end,info));
   }
   
   S32 getMaterialAlphaIndex(const char *materialName);

  private:
   S32 mSquareSize;
   S32 mTexelsPerMeter;
   S32 mClipMapSizeLog2; 
   bool mTile;

  public:
   void setFile(Resource<TerrainFile> file);
   void createNewTerrainFile(StringTableEntry filename, U16 *hmap = NULL, Vector<StringTableEntry> *materials = NULL);
   bool save(const char* filename);

   static void flushCache();
   //void relight(const ColorF &lightColor, const ColorF &ambient, const Point3F &lightDir);

   S32 getSquareSize();
   void setSquareSize(S32 squareSize)
   {
      mSquareSize = squareSize;
   }
   bool isTiling();
   void setTiling(bool tiling)
   {
      mTile = tiling;
   }
   void setTexelsPerMeter(S32 value) {mTexelsPerMeter = value;}

   void refillClipmap();

   //--------------------------------------
   // SceneGraph functions...

   void doPrepRenderImage(SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState=false) {prepRenderImage(state,stateKey,startZone,modifyBaseZoneState);}
  protected:
   void setTransform     (const MatrixF &mat);
   bool prepRenderImage  ( SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState=false);
   void renderObject     ( SceneState *state, RenderInst *ri );

   //--------------------------------------
   // collision info
  private:
   S32 mHeightMin;
   S32 mHeightMax;

  public:
   void buildConvex(const Box3F& box,Convex* convex);
   bool buildPolyList(AbstractPolyList* polyList, const Box3F &box, const SphereF &sphere);
   bool castRay(const Point3F &start, const Point3F &end, RayInfo* info);
   bool castRayI(const Point3F &start, const Point3F &end, RayInfo* info, bool emptyCollide);
   bool castRayBlock(const Point3F &pStart, const Point3F &pEnd, Point2I blockPos, U32 level, F32 invDeltaX, F32 invDeltaY, F32 startT, F32 endT, RayInfo *info, bool);

  public:
   bool buildMaterialMap();
   bool destoryMaterialMap(S32 num);
   void buildMipMap();

   void setBaseMaterials(S32 argc, const char *argv[]);
   bool loadBaseMaterials();
   bool initMMXBlender();

   void postLight(Vector<TerrainBlock *> &terrBlocks);

   bool loadRenderResource();
   void freeRenderResource();

	void freshShadowTex();
	bool refreshLighttex();

	void setRenderSubTerrEdge(bool needRender){mNeedRenderSubTerrEdge = needRender;}

	ColorF mEdgeColor;
   // private helper
  private:
   bool        mCollideEmpty;
	
	bool mNeedRenderSubTerrEdge;
	void renderSubTerrEdge();
  public:
   DECLARE_CONOBJECT(TerrainBlock);
   static void initPersistFields();
   U64 packUpdate   (NetConnection *conn, U64 mask, BitStream *stream);
   void unpackUpdate(NetConnection *conn,           BitStream *stream);

   // 多地形信息
private:
	static U32 mMaxTerrainWidth;
	static U32 mMaxTerrainHeight;
	static TerrainBlock* mTerMap[TerrainBlock::TerrainSize][TerrainBlock::TerrainSize];
	static Box3F mTerrBox;
public:

	U32 mWidthIndex;
	U32 mHeightIndex;

	static void setTerMap(U32 x, U32 y, TerrainBlock*);
	static TerrainBlock* getTerMap(U32 x, U32 y);
	static void RemoveTerMap(U32 x, U32 y);
	static void clearTerMap();

	static void setMaxTerrainWidth(U32 width);
	static void setMaxTerrainHeight(U32 height);
	static U32 getMaxTerrainWidth() { return mMaxTerrainWidth; }
	static U32 getMaxTerrainHeight() { return mMaxTerrainHeight; }

	static void loadNaviGrid(StringTableEntry filename = NULL);
private:
	//set shader
	static GFXStateBlock* mSetShader1SB;
	static GFXStateBlock* mSetShader2SB;
	static GFXStateBlock* mClearSB;
	//cull mode
	static GFXStateBlock* mCullNoneSB;
	static GFXStateBlock* mCullCWSB;
	static GFXStateBlock* mCullCCWSB;
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



//--------------------------------------
class TerrainFile : public ResourceInstance
{
  public:
   // [tom, 5/16/2007] Bumped to version 4 as material filenames are now stored relative to the .ter
   // [joshe, 3/5/2008] Bumped to version 5 for edge terrains
   enum Constants {
      FILE_VERSION = 6,
      MATERIAL_GROUP_MASK = 0x7
   };

   TerrainFile();
   ~TerrainFile();
   U16 mHeightMap[TerrainBlock::BlockSize * TerrainBlock::BlockSize];
   U8  mBaseMaterialMap[TerrainBlock::BlockSize * TerrainBlock::BlockSize];
	ColorF  mShadowMap[TerrainBlock::BlockSize * TerrainBlock::BlockSize];
   GridSquare mGridMapBase[TerrainBlock::GridMapSize];
   GridSquare *mGridMap[TerrainBlock::BlockShift+1];
   GridChunk mChunkMap[TerrainBlock::ChunkSquareWidth * TerrainBlock::ChunkSquareWidth];
   U16 mFlagMap[TerrainBlock::FlagMapWidth * TerrainBlock::FlagMapWidth];
   char *mTextureScript;
   char *mHeightfieldScript;

   TerrainBlock::Material mMaterialMap[TerrainBlock::BlockSquareWidth * TerrainBlock::BlockSquareWidth];

   // DMMNOTE: This loads all the alpha maps, whether or not they are used.  Possible to
   //  restrict to only the used versions?
   StringTableEntry mMaterialFileName[TerrainBlock::MaterialGroups];
   U8*              mMaterialAlphaMap[TerrainBlock::MaterialGroups];
	U8* mTexAlpha[TerrainBlock::ChunkSize*TerrainBlock::ChunkSize][TerrainBlock::Max_Textures];
	U8 mTexIndex[TerrainBlock::ChunkSize*TerrainBlock::ChunkSize][TerrainBlock::Max_Texture_Layer];//纹理层
   U32 fileVersion;                 // The actual version of this file
   bool needsResaving;              // Indicates whether or not this TerrainFile needs resaving (for example if the materials are updated)
   StringTableEntry fileNamePath;   // The full path and name of the TerrainFile

   Resource<TerrainFile> mEdgeTerrainFiles[TerrainBlock::EDGE_MAX]; // Edge files

   bool save(const char *filename);
   void buildChunkDeviance(S32 x, S32 y);
   void buildGridMap();
   bool destoryMaterialMap(S32 num);
   void heightDevLine(U32 p1x, U32 p1y, U32 p2x, U32 p2y, U32 pmx, U32 pmy, U16 *devPtr);

   inline GridSquare *findSquare(U32 level, Point2I pos)
   {
      return mGridMap[level] + (pos.x >> level) + ((pos.y>>level) << (TerrainBlock::BlockShift - level));
   }

   U16 getHeight(U32 x, U32 y, bool tiling = true);

   inline TerrainBlock::Material *getMaterial(U32 x, U32 y)
   {
      return &mMaterialMap[(x & TerrainBlock::BlockMask) + ((y & TerrainBlock::BlockMask) << TerrainBlock::BlockShift)];
   }

   void setTextureScript(const char *script);
   void setHeightfieldScript(const char *script);
   const char *getTextureScript();
   const char *getHeightfieldScript();
};



//--------------------------------------------------------------------------
inline U16 *TerrainBlock::getFlagMapPtr(S32 x, S32 y)
{
   return flagMap + ((x >> 1) & TerrainBlock::FlagMapMask) +
                  ((y >> 1) & TerrainBlock::FlagMapMask) * TerrainBlock::FlagMapWidth;
}

inline GridSquare *TerrainBlock::findSquare(U32 level, S32 x, S32 y)
{
   return gridMap[level] + ((x  & TerrainBlock::BlockMask) >> level) + (((y & TerrainBlock::BlockMask) >> level) << (TerrainBlock::BlockShift - level));
}

inline GridSquare *TerrainBlock::findSquare(U32 level, Point2I pos)
{
   return gridMap[level] + (pos.x >> level) + ((pos.y>>level) << (BlockShift - level));
}

inline GridChunk *TerrainBlock::findChunk(Point2I pos)
{
   return mChunkMap + (pos.x >> ChunkDownShift) + ((pos.y>>ChunkDownShift) << ChunkShift);
}

inline TerrainBlock::Material *TerrainBlock::getMaterial(U32 x, U32 y)
{
   return materialMap + x + (y << BlockShift);
}

/*
inline TextureHandle TerrainBlock::getDetailTextureHandle()
{
   return mDetailTextureHandle;
}
*/

inline S32 TerrainBlock::getSquareSize()
{
   return mSquareSize;
}

inline U8 TerrainBlock::getBaseMaterial(U32 x, U32 y)
{
   return mBaseMaterialMap[(x & BlockMask) + ((y & BlockMask) << BlockShift)];
}

inline U8* TerrainBlock::getBaseMaterialAddress(U32 x, U32 y)
{
   return &mBaseMaterialMap[(x & BlockMask) + ((y & BlockMask) << BlockShift)];
}

inline U8 *TerrainBlock::getMaterialAlphaMap(U8 subTerr, U32 matIndex)
{
	if (mFile->mTexAlpha[subTerr][matIndex] == NULL) {
		mFile->mTexAlpha[subTerr][matIndex] = new U8[TerrainBlock::ChunkSquareWidth * TerrainBlock::ChunkSquareWidth];
		dMemset(mFile->mTexAlpha[subTerr][matIndex], 0, TerrainBlock::ChunkSquareWidth * TerrainBlock::ChunkSquareWidth);
	}

	return mFile->mTexAlpha[subTerr][matIndex];
}

inline U8* TerrainBlock::getMaterialAlphaMap(U32 matIndex)
{
   if (mFile->mMaterialAlphaMap[matIndex] == NULL) {
      mFile->mMaterialAlphaMap[matIndex] = new U8[TerrainBlock::BlockSize * TerrainBlock::BlockSize];
      dMemset(mFile->mMaterialAlphaMap[matIndex], 0, TerrainBlock::BlockSize * TerrainBlock::BlockSize);
   }

   return mFile->mMaterialAlphaMap[matIndex];
}

// 11.5 fixed point - gives us a height range from 0->2048 in 1/32 inc

inline F32 fixedToFloat(U16 val)
{
   return F32(val) * 0.03125f;
}

inline U16 floatToFixed(F32 val)
{
   return U16(val * 32.0);
}

extern ResourceInstance *constructTerrainFile(Stream &stream, ResourceObject *);

#endif
