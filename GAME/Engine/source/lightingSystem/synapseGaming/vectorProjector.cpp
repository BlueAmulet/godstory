
#include "lightingSystem/synapseGaming/vectorProjector.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/lightingInterfaces.h"
#include "sceneGraph/lightManager.h"
#include "t3d/tsStatic.h"


MatrixF			vectorProjector::mLightToWorld;
MatrixF			vectorProjector::mWorldToLight;
Box3F			vectorProjector::mProjectBox;
SphereF			vectorProjector::mProjectSphere;
Point3F			vectorProjector::mProjectPoly[4];
DepthSortList	vectorProjector::smDepthSortList;
DepthSortList	vectorProjector::sTemp;


void vectorProjector::collisionCallback(SceneObject * obj, void* thisPtr)
{
	// <Edit> [7/20/2009 joy] 不接受阴影的物体同样不接受投影
	if(obj->getTypeMask() & StaticTSObjectType)
	{
		TSStatic *tsstatic = dynamic_cast<TSStatic *>(obj);
		if(tsstatic && !tsstatic->receiveShadow())
			return;
	}
	// only interiors clip...
	// <Edit> [3/30/2009 joy] 都进行裁剪
	//ClippedPolyList::allowClipping = (obj->getTypeMask() & gClientSceneGraph->getLightManager()->getSceneLightingInterface()->mClippingMask) != 0;
	obj->buildPolyList(&smDepthSortList,mProjectBox,mProjectSphere);
	//ClippedPolyList::allowClipping = true;
}

void vectorProjector::transparencyCallback(SceneObject * obj, void* thisPtr)
{
	//暂时没启用

	if(obj->getTypeMask() & TerrainObjectType)
	{
		//ClippedPolyList::allowClipping = (obj->getTypeMask() & gClientSceneGraph->getLightManager()->getSceneLightingInterface()->mClippingMask) != 0;
		obj->buildPolyList(&smDepthSortList,mProjectBox,mProjectSphere);
		//ClippedPolyList::allowClipping = true;
	}
	else if(obj->getTypeMask() & StaticRenderedObjectType)
	{
		if(TSStatic* pstatic = dynamic_cast<TSStatic*>(obj))
		{
			sTemp.mPolyList.clear();
			obj->buildPolyList(&sTemp,mProjectBox,mProjectSphere);
			pstatic->setTransparency(bool(sTemp.mPolyList.size()));
		}
	}
}

void vectorProjector::setLightMatrices(const Point3F & lightDir, const Point3F & pos, MatrixF* pLightToWorld)
{
	AssertFatal(mDot(lightDir,lightDir)>0.0001f,"vectorProjector::setLightDir: light direction must be a non-zero vector.");

	// construct light matrix
	Point3F x,z;
	if (mFabs(lightDir.z)>0.001f)
	{
		// mCross(Point3F(1,0,0),lightDir,&z);
		z.x = 0.0f;
		z.y =  lightDir.z;
		z.z = -lightDir.y;
		z.normalize();
		mCross(lightDir,z,&x);
	}
	else
	{
		mCross(lightDir,Point3F(0,0,1),&x);
		x.normalize();
		mCross(x,lightDir,&z);
	}

	mLightToWorld.identity();
	mLightToWorld.setColumn(0,x);
	mLightToWorld.setColumn(1,lightDir);
	mLightToWorld.setColumn(2,z);
	mLightToWorld.setColumn(3,pos);

	mWorldToLight = mLightToWorld;
	mWorldToLight.inverse();

	if(pLightToWorld)
		*pLightToWorld = mLightToWorld;
}

void vectorProjector::buildPartition(const Point3F & p,
									 const Point3F & lightDir,
									 F32 radius,
									 F32 projectLen,
									 U32 mask,
									 Vector<DepthSortList::Poly>& rPartition,
									 Vector<Point3F>& rPartitionVerts,
									 MatrixF* pLightToWorld,
									 bool bTransparency)
{
	setLightMatrices(lightDir,p,pLightToWorld);

	Point3F extent(2.0f*radius,projectLen,2.0f*radius);
	smDepthSortList.clear();
	smDepthSortList.set(mWorldToLight,extent);
	smDepthSortList.setInterestNormal(lightDir);
	if(bTransparency)
	{
		sTemp.clear();
		sTemp.set(mWorldToLight,extent);
		sTemp.setInterestNormal(lightDir);
	}

	if (projectLen<1.0f)
		// no point in even this short of a shadow...
		projectLen = 1.0f;

	// build world space box and sphere around shadow

	Point3F x,y,z;
	mLightToWorld.getColumn(0,&x);
	mLightToWorld.getColumn(1,&y);
	mLightToWorld.getColumn(2,&z);
	x *= radius;
	y *= projectLen;
	z *= radius;
	mProjectBox.max.set(mFabs(x.x)+mFabs(y.x)+mFabs(z.x),
		mFabs(x.y)+mFabs(y.y)+mFabs(z.y),
		mFabs(x.z)+mFabs(y.z)+mFabs(z.z));
	y *= 0.5f;
	mProjectSphere.radius = mProjectBox.max.len();
	mProjectSphere.center = p + y;
	mProjectBox.min  = y + p - mProjectBox.max;
	mProjectBox.max += y + p;

	if(bTransparency)
		gClientContainer.findObjects(mProjectBox,mask,vectorProjector::transparencyCallback,this);
	else
		gClientContainer.findObjects(mProjectBox,mask,vectorProjector::collisionCallback,this);

	// setup rPartition list
	mProjectPoly[0].set(-radius,0, -radius);
	mProjectPoly[1].set(-radius,0, radius);
	mProjectPoly[2].set(radius,0, radius);
	mProjectPoly[3].set(radius,0, -radius);

	rPartition.clear();
	rPartitionVerts.clear();
	smDepthSortList.depthPartition(mProjectPoly,4,rPartition,rPartitionVerts);
}

vectorProjector gVectorProjector;

