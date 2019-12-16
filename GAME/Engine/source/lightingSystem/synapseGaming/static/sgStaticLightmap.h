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

		Point3F     pos;  //�����Ӧ�ĵ�λ��
		size_t      face; //��Ӧ����
		VectorF		normal; //�淨��

		ColorF clr;       //������ɫ
		bool  isValid;    //�Ƿ���Ч
	};
public:
	CLightmap()
	{
		mInfos = NULL;
		Clear();
	}

	~CLightmap()	{		Clear();	}
	//��������
	void Reset(void){		Clear();	}

	//���������ͼ����
	void Initialize(TSShapeInstance *pShapeInstance,const MatrixF& obj2world,U32 Width,U32 Height);

	//���������Ӧ�Ĺ�����Ϣ
	CLightmapInfo* GetLightAt(size_t u,size_t v);

	//������������
	//GBitmap* GetLightmap(void);
private:
	void Clear(void);

	U32 mWidth;               //������ͼ��С
	U32 mHeight;
	CLightmapInfo* mInfos; //������������Ԫ�صĹ�����Ϣ

	TSShapeInstance* mShapeInstance;
};

#endif