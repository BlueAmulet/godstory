#pragma once

//======================================================================
// ���Թ��ͶӰ
//======================================================================
#ifndef _SCENEOBJECT_H_
#include "sceneGraph/sceneObject.h"
#endif

#include "collision/depthSortList.h"
#include "math/mSphere.h"
#include "gfx/gfxVertexBuffer.h"

class vectorProjector
{
	static MatrixF mLightToWorld;
	static MatrixF mWorldToLight;

	static Box3F mProjectBox;
	static SphereF mProjectSphere;
	static Point3F mProjectPoly[4];
	static DepthSortList smDepthSortList;
	static DepthSortList sTemp;

	void		setLightMatrices(const Point3F & lightDir, const Point3F & pos, MatrixF* pLightToWorld);
	static void	collisionCallback(SceneObject * obj, void* thisPtr);

	static void	transparencyCallback(SceneObject * obj, void* thisPtr);

public:
	void		buildPartition( const Point3F & p,				// ��Դ
								const Point3F & lightDir,		// ����
								F32 radius,						// �뾶
								F32 projectLen,					// ����
								U32 mask,
								Vector<DepthSortList::Poly>& mPartition,
								Vector<Point3F>& mPartitionVerts,
								MatrixF* pLightToWorld = NULL,
								bool bTransparency = false);			// �黯
};

extern vectorProjector gVectorProjector;
