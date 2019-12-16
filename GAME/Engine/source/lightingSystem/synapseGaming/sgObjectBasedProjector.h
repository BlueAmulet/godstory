//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#ifndef _SGOBJECTBASEDPROJECTOR_H_
#define _SGOBJECTBASEDPROJECTOR_H_

#include "collision/depthSortList.h"
#include "materials/shaderData.h"
#include "sceneGraph/sceneObject.h"
#include "ts/tsShapeInstance.h"
#include "lightingSystem/common/shadowBase.h"

class sgLightInfo;
class RenderableSceneObject;

class sgShadowSharedZBuffer
{
private:
	static Point2I sgSize;
	static GFXTexHandle sgZBuffer;
public:
	static GFXTexHandle &sgGetZBuffer();
	static void sgPrepZBuffer(const Point2I &size);
	static void sgClear();
};

class sgShadowProjector : public ShadowBase
{
public:
   Box3F sgBoundingBox;
   RenderableSceneObject *sgParentObject;
	class sgShadow
	{
	private:
		S32 sgCurrentLOD;
	public:
		GFXTexHandle sgShadowTexture;
		GFXFormat sgShadowTextureFormat;
		sgShadow()
		{
			sgCurrentLOD = -1;
			//sgShadowTextureFormat = 0;
			sgShadowTexture = NULL;
		}
		~sgShadow() {sgRelease();}
		bool sgSetLOD(S32 lod, Point2I size);
		S32 sgGetLOD() {return sgCurrentLOD;}
		void sgRelease();
	};

	bool sgEnable;
	bool sgCanMove;
	bool sgCanRTT;
	bool sgCanSelfShadow;
	U32 sgRequestedShadowSize;
	U32 sgFrameSkip;
	F32 sgMaxVisibleDistance;
	F32 sgProjectionDistance;
	F32 sgSphereAdjust;
	F32 sgBias;
	bool sgDTSShadow;
	F32 sgIntensity;
	sgLightInfo *sgLight;
	Point3F sgLightVector;
	MatrixF sgLightSpaceY;
	MatrixF sgLightToWorldY;
	MatrixF sgWorldToLightY;
	MatrixF sgLightProjToLightY;
	MatrixF sgWorldToLightProjY;
	Point4F sgProjectionInfo;

	F32 sgAttn;
	bool sgAllowlodselfshadowing;
	F32 sgScaledcamdist;
	enum
	{
		sgspLastSelfShadowLOD = 3,
		sgspMaxLOD = 5
	};

	// shadow lod textures...
	Point2I sgShadowLODSize[sgspMaxLOD];
	// current texture, cached to avoid texture switch and re-RTT on lod switch...
	sgShadow sgShadowLODObject;
	ShaderData *sgShadowBuilderShader;
	ShaderData *sgShadowShader;
	ShaderData *sgShadowShaderFastPartition;
	ShaderData *sgShadowBuilderShader_2_0;
	ShaderData *sgShadowBuilderShader_1_1;
	ShaderData *sgShadowBuilderShader_vt;
	ShaderData *sgShadowShaderHigh_2_0;
	ShaderData *sgShadowShader_2_0;
	ShaderData *sgShadowShader_1_1;
	TSShapeInstance *sgShapeInstance;
	
	Vector<SceneObject *> sgShadowReceivers;
	Vector<DepthSortList::Poly> sgShadowPolys;
	Vector<Point3F> sgShadowPoints;
	GFXVertexBufferHandle<GFXVertexPN> sgShadowBuffer;
	GFXPrimitiveBufferHandle mShadowPBuffer;
	size_t mPBTriangleNum;
   GFXTextureTargetRef mShadowBufferTarget;

	Point3F testRenderPoints[8];
	Point3F testRenderPointsWorld[8];

	enum sgShadowOp
	{
		shadow_gen,
		shadow_save,
		shadow_set,
	};

	enum sgShaderModel
	{
		sgsm_1_1,
		sgsm_2_0,
		sgsmHigh_2_0,
	};
	sgShaderModel sgCalculateShaderModel();

	bool sgFirstMove;
	bool sgFirstRTT;
	sgShaderModel sgCachedShaderModel;
	Point3F sgCachedParentPos;
	U32 sgLastFrame;
	U32 sgCachedTextureDetailSize;
	U32 sgCachedParentTransformHash;
	U32 sgPreviousShadowTime;
	VectorF sgPreviousShadowLightingVector;
	bool sgShadowTypeDirty;
	bool sgAllowSelfShadowing() {return sgCachedShaderModel != sgsm_1_1;}
	void sgSetupShadowType(sgShadowOp flag=shadow_gen);
	U32 sgGetShadowSize()
	{
		U32 size = sgRequestedShadowSize >> sgCachedTextureDetailSize;
		size = getMax(size, U32(64));
		return size;
	}
	void sgGetVariables();
	void sgGetLightSpaceBasedOnY();
   MatrixF sgGetAdjustedParentWorldTransform();
	virtual void sgCalculateBoundingBox(sgShadowOp flag=shadow_gen);
	void sgDebugRenderProjectionVolume();
	void sgRenderShape(TSShapeInstance *shapeinst,
		const MatrixF &trans1, S32 vertconstindex1,
		const MatrixF &trans2, S32 vertconstindex2);
	void sgRenderShadowBuffer();
	Point3F sgGetCompositeShadowLightDirection();
	void sgClear(bool texOnly=false);

	template<class T> void sgDirtySync(T &dst, T src)
	{
		if(dst == src)
			return;
		dst = src;
		sgShadowTypeDirty = true;
	}

   S32 mTexCallbackHandle;
   static void texCB( GFXTexCallbackCode code, void *userData );

public:
	F32 sgProjectionScale;
	Box3F sgShadowBox;
	SphereF sgShadowSphere;
	Point3F sgShadowPoly[4];
	DepthSortList sgPolyGrinder;

public:
	U32 sgLastRenderTime;

	sgShadowProjector(SceneObject *parentobject,
		LightInfo *light, TSShapeInstance *shapeinstance);
	~sgShadowProjector();
	void render(S32 key);
	static void collisionCallback(SceneObject *obj, void *shadow);
	static void dtsCallback(SceneObject *obj, void *shadow);
   bool shouldRender(F32 camDist);
   void preRender(F32 camDist);
   U32 getLastRenderTime() { return sgLastRenderTime; }
   void resetShapeInst(TSShapeInstance* shapeInst) { sgShapeInstance = shapeInst;}
   void saveDynamicShadowData(Stream *stream);
   void setDynamicShadowData(void *,void *);
   void freeResource();
   void setRenderStatus(S32);
   void clearRenderStatus(S32);
private:
	static GFXStateBlock* mSetOneSB;
	static GFXStateBlock* mSetTwoSB;
	static GFXStateBlock* mClearStatusSB;
	static GFXStateBlock* mSetAllowFastSB;
	static GFXStateBlock* mSetSB;
	static GFXStateBlock* mClearSB;
	static GFXStateBlock* mSetDebugSB;
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


#endif//_SGOBJECTBASEDPROJECTOR_H_
