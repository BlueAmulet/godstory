#ifndef __SGSTATICLIGHTMAP_H
#define __SGSTATICLIGHTMAP_H

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

#include "core/color.h"
#include "collision/convex.h"

/************************************************************************/
/* ShapeInfo
/************************************************************************/
class ConvexFeature;
class ConvexFeatureEx : public ConvexFeature
{
	enum PlaneProjed
	{
		PLANE_UNKNOWN,
		PLANE_YZ,
		PLANE_XZ,
		PLANE_XY,
	};

	Vector<PlaneProjed> mProjedPlaneList;

public:
	void Initialize();
	PlaneProjed GetProjPlane(S32 FaceNum);
	bool IsInFace(S32 FaceNum,F32 lmU,F32 lmV);
	bool TexturePosToWorldPos(S32 FaceNum,F32 lmU,F32 lmV,Point3F &pos);
};

/************************************************************************/
/* LightMapping
/************************************************************************/
class GBitmap;
class TSShapeInstance;
class LightInfo;
class CLightmap
{
public:
	struct CLightmapInfo
	{
		CLightmapInfo(void) : isValid(false) {}

		Point3F     pos;  //纹理对应的点位置
		size_t      face; //对应的面
		VectorF		normal; //面法线

		ColorF clr;       //像素颜色
		bool  isValid;    //是否有效
	};
public:
	CLightmap()
	{
		mInfos = NULL;
		Clear();
	}

	~CLightmap()	{		Clear();	}
	//重设数据
	void Reset(void){		Clear();	}

	//计算光照贴图数据
	void Initialize(TSShapeInstance *pShapeInstance,const MatrixF& obj2world,U32 Width,U32 Height);

	//返回纹理对应的光照信息
	CLightmapInfo* GetLightAt(size_t u,size_t v);

	//产生光照纹理
	//GBitmap* GetLightmap(void);
private:
	void Clear(void);

	U32 mWidth;               //光照贴图大小
	U32 mHeight;
	CLightmapInfo* mInfos; //保存所有纹理元素的光照信息

	TSShapeInstance* mShapeInstance;
};

#endif