//Ray: 标准的光照图生成方式
#include "lightingSystem/synapseGaming/static/sgStaticLightmap.h"
#include "gfx/gBitmap.h"
#include "ts/tsShapeInstance.h"
#include "ts/tsMesh.h"

const double FLOAT_EPSILON = (1e-4);

/************************************************************************/
/* ShapeInfo
/************************************************************************/
void ConvexFeatureEx::Initialize()
{
	for(int i=0;i<mFaceList.size();i++)
		mProjedPlaneList.push_back(PLANE_UNKNOWN);
}

bool ConvexFeatureEx::IsInFace(S32 iFace,F32 lmU,F32 lmV)
{
	float u0 = mUVList[mFaceList[iFace].vertex[0]].x;
	float v0 = mUVList[mFaceList[iFace].vertex[0]].y;

	float u1 = mUVList[mFaceList[iFace].vertex[1]].x;
	float v1 = mUVList[mFaceList[iFace].vertex[1]].y;

	float u2 = mUVList[mFaceList[iFace].vertex[2]].x;
	float v2 = mUVList[mFaceList[iFace].vertex[2]].y;

	/*
		p0

		d
	p1       p2
	*/
	float area[3] = {0};

	float x0 = lmU - u0;
	float y0 = lmV - v0;

	float x1 = u1 - u0;
	float y1 = v1 - v0;

	area[0] = x0 * y1 - y0 * x1;

	if (fabs(area[0]) < FLOAT_EPSILON)
		return true;

	x0 = lmU - u1;
	y0 = lmV - v1;

	x1 = u2 - u1;
	y1 = v2 - v1;

	area[1] = x0 * y1 - y0 * x1;

	if (mFabs(area[1]) < FLOAT_EPSILON)
		return true;

	if (area[0] * area[1] < 0)
		return false;

	x0 = lmU - u2;
	y0 = lmV - v2;

	x1 = u0 - u2;
	y1 = v0 - v2;

	area[2] = x0 * y1 - y0 * x1;

	if (mFabs(area[2]) < FLOAT_EPSILON)
		return true;

	if (area[0] * area[2] < 0 || area[1] * area[2] < 0)
		return false;

	return true;
}

ConvexFeatureEx::PlaneProjed ConvexFeatureEx::GetProjPlane(S32 iFace)
{
	if (PLANE_UNKNOWN != mProjedPlaneList[iFace])
		return mProjedPlaneList[iFace];

	//计算平面的法线，忽略分项绝对值最大的轴
	Point3F p0 = mVertexList[mFaceList[iFace].vertex[0]];
	Point3F p1 = mVertexList[mFaceList[iFace].vertex[1]];
	Point3F p2 = mVertexList[mFaceList[iFace].vertex[2]];

	Point3F side0 = p1 - p0;
	Point3F side1 = p2 - p0;

	side0 = mCross(side0,side1);
	mFaceList[iFace].normal = side0; //保存平面的法线
	side0.x = mFabs(side0.x);
	side0.y = mFabs(side0.y);
	side0.z = mFabs(side0.z);

	float max_val = getMax(getMax(side0.x,side0.y),side0.z);

	if (side0.x == max_val)
		mProjedPlaneList[iFace] = PLANE_YZ;

	if (side0.y == max_val)
		mProjedPlaneList[iFace] = PLANE_XZ;

	if (side0.z == max_val)
		mProjedPlaneList[iFace] = PLANE_XY;

	return mProjedPlaneList[iFace];
}

bool ConvexFeatureEx::TexturePosToWorldPos(S32 iFace,F32 lmU,F32 lmV,Point3F &rslt)
{
	Point2F a;
	Point2F b;
	Point2F c;

	Point3F p0 = mVertexList[mFaceList[iFace].vertex[0]];
	Point3F p1 = mVertexList[mFaceList[iFace].vertex[1]];
	Point3F p2 = mVertexList[mFaceList[iFace].vertex[2]];

	//找出计算坐标的最大投影平面
	PlaneProjed projPlane = GetProjPlane(iFace);

	if (PLANE_UNKNOWN == projPlane)
	{
		AssertFatal(0,"no projected plane");
		return false;
	}

	if (PLANE_YZ == projPlane)
	{
		a = Point2F(p0.y,p0.z);
		b = Point2F(p1.y,p1.z);
		c = Point2F(p2.y,p2.z);
	}

	if (PLANE_XZ == projPlane)
	{
		a = Point2F(p0.x,p0.z);
		b = Point2F(p1.x,p1.z);
		c = Point2F(p2.x,p2.z);
	}

	if (PLANE_XY == projPlane)
	{
		a = Point2F(p0.x,p0.y);
		b = Point2F(p1.x,p1.y);
		c = Point2F(p2.x,p2.y);
	}

	//3个顶点的纹理坐标
	float u0 = mUVList[mFaceList[iFace].vertex[0]].x;
	float v0 = mUVList[mFaceList[iFace].vertex[0]].y;

	float u1 = mUVList[mFaceList[iFace].vertex[1]].x;
	float v1 = mUVList[mFaceList[iFace].vertex[1]].y;

	float u2 = mUVList[mFaceList[iFace].vertex[2]].x;
	float v2 = mUVList[mFaceList[iFace].vertex[2]].y;

	//计算du dv
	float du = lmU - u0;
	float dv = lmV - v0;

	//顶点位置在u,v方向上变换率
	float x0 = a.x;
	float y0 = a.y;

	float x1 = b.x;
	float y1 = b.y;

	float x2 = c.x;
	float y2 = c.y;

	float denominator = (v0-v2) * (u1-u2) - (v1-v2) * (u0-u2);

	if (mFabs(denominator) < FLOAT_EPSILON)
		return false;

	float dpdu = ((x1-x2) * (v0-v2) - (x0-x2) * (v1-v2))/denominator;
	float dpdv = ((x1-x2) * (u0-u2) - (x0-x2) * (u1-u2))/-denominator;
	float dqdu = ((y1-y2) * (v0-v2) - (y0-y2) * (v1-v2))/denominator;
	float dqdv = ((y1-y2) * (u0-u2) - (y0-y2) * (u1-u2))/-denominator;

	float s = (x0) + (dpdu * du) + (dpdv * dv);
	float t = (y0) + (dqdu * du) + (dqdv * dv);

	//根据3角形的平面方程，计算另外1个点的坐标
	Point3F& normal = mFaceList[iFace].normal;
	float d = -mDot(normal,p0);

	if (PLANE_YZ == projPlane) 
	{
		rslt.y = s;
		rslt.z = t;
		rslt.x = -(normal.y * rslt.y + normal.z * rslt.z + d) / normal.x;
	}

	if (PLANE_XZ == projPlane)
	{
		rslt.x = s;
		rslt.z = t;
		rslt.y = -(normal.x * rslt.x + normal.z * rslt.z + d) / normal.y;
	}

	if (PLANE_XY == projPlane)
	{
		rslt.x = s;
		rslt.y = t;
		rslt.z = -(normal.x * rslt.x + normal.y * rslt.y + d) / normal.z;
	}

	return true;
}

/************************************************************************/
/* LightMapping
/************************************************************************/
void CLightmap::Clear(void)
{
	if (NULL != mInfos)
		delete []mInfos;

	mInfos = NULL;
}

//计算光照贴图数据
void CLightmap::Initialize(TSShapeInstance *pShapeInstance,const MatrixF& obj2world,U32 Width,U32 Height)
{
	mShapeInstance = pShapeInstance;

	mWidth = Width;
	mHeight = Height;

	//分配Width * Height大小的数据
	mInfos = new CLightmapInfo[mWidth * mHeight];

	ConvexFeatureEx cf;
	Point3F n(0, 0, 1);
	U32 surfaceKey = 0;

	S32 dl =0;

	TSShape *pShape = mShapeInstance->getShape();
	const TSDetail * detail = &pShape->details[dl];
	S32 ss = detail->subShapeNum;
	S32 od = detail->objectDetailNum;

	// set up static data
	mShapeInstance->setStatics(dl);

	S32 start = pShape->subShapeFirstObject[ss];
	S32 end   = pShape->subShapeNumObjects[ss] + start;

	if(start>=end)
		return;

	MatrixF mat = obj2world;
	MatrixF * previousMat = NULL;
	previousMat = mShapeInstance->mMeshObjects[start].getTransform();
	if(previousMat!=NULL)
		mat.mul(*previousMat);

	for (S32 i=start; i<end; i++)
	{
		TSShapeInstance::MeshObjectInstance * meshIns = &mShapeInstance->mMeshObjects[i];
		if (od >= meshIns->object->numMeshes)
			continue;

		TSMesh *mesh = meshIns->getMesh(od);
		if(mesh)
		{
			if (meshIns->getTransform() != previousMat)
			{
				previousMat = meshIns->getTransform();
				if(previousMat!=NULL)
				{
					mat = obj2world;
					mat.mul(*previousMat);
				}
			}

			mesh->getFeatures(0, mat, n, &cf, surfaceKey);
		}
	}
	cf.Initialize();

	//计算每个纹理坐标对应的信息
	for (size_t v = 0; v < mHeight; v++)
	{
		for (size_t u = 0; u < mWidth; u++)
		{
			//计算纹理坐标
			float texU = (float(u) + 0.5f) / float(mWidth);
			float texV = (float(v) + 0.5f) / float(mHeight);

			//遍历每个面，查看纹理坐标是否在面内
			for (S32 iFace = 0; iFace < cf.mFaceList.size(); iFace++)
			{
				if (!cf.IsInFace(iFace,texU,texV))
					continue;

				//计算当前纹理对应的坐标
				Point3F pos;

				if (!cf.TexturePosToWorldPos(iFace,texU,texV,pos))
					continue;

				mInfos[v * mWidth + u].isValid = true;
				mInfos[v * mWidth + u].face    = iFace;
				mInfos[v * mWidth + u].normal  = cf.mFaceList[iFace].normal;
				mInfos[v * mWidth + u].pos     = pos;
			}
		}
	}
}

//返回纹理对应的光照信息
CLightmap::CLightmapInfo* CLightmap::GetLightAt(U32 lmU,U32 lmV)
{
	if (NULL == mInfos)
		return NULL;

	return mInfos + lmV * mWidth + lmU;
}

////产生光照纹理
//GBitmap* CLightmap::GetLightmap(void)
//{
//	if (NULL == mInfos)
//		return NULL;
//
//	GBitmap *pLightMap = new GBitmap(mWidth ,mHeight);
//	U8 *pData = pLightMap->getWritableBits();
//
//	for (size_t v = 0; v < mHeight; v++)
//	{
//		for (size_t u = 0; u < mWidth; u++)
//		{
//			CLightmapInfo *pInfo = GetLightAt(u,v);
//			if(pInfo)
//				continue;
//
//			ColorF clr = pInfo->clr;
//			//把[-1,1]转为[0,1]
//			//clr = clr * 0.5f + ColorF(0.5f,0.5f,0.5f,0.5f);
//			//转为颜色值
//			clr *= 255.0f;
//
//			//存储到相应的cubemap
//
//			pData[0] = (U8)getMin(clr.red	,255.0f); //r
//			pData[1] = (U8)getMin(clr.green	,255.0f); //g
//			pData[2] = (U8)getMin(clr.blue	,255.0f); //b
//			pData[3] = (U8)getMin(clr.alpha	,255.0f); //a
//
//			pData += 4;
//		}
//	}
//
//	return pLightMap;
//}

