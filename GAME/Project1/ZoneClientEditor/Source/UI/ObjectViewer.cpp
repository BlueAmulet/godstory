//------------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//------------------------------------------------------------------------------

#include "UI/ObjectViewer.h"
#include "math/mathUtils.h"
#include "renderInstance/renderInstMgr.h"
#include "sceneGraph/lightManager.h"
#include "sceneGraph/lightInfo.h"
#include "ts/TSShapeRepository.h"
#include "console/consoleTypes.h"
#include "gfx/primBuilder.h"
#include "materials/matInstance.h"

#include "sceneGraph/decalManager.h"
#include "lightingSystem/synapseGaming/sgDecalProjector.h"
#include "T3D/fx/particleEmitter.h"
#include "gui/buttons/guiButtonCtrl.h"
#include "terrain/terrData.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
#include "gfx/gfxFontRenderBatcher.h"

#include "util/aniThread.h"

//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
CObjectViewer* CObjectViewer::smObjectViewer = NULL;

//#define STATEBLOCK
//render collection
GFXStateBlock* CObjectViewer::mSetCollSB = NULL;
GFXStateBlock* CObjectViewer::mColorOpStoreSB = NULL;
GFXStateBlock* CObjectViewer::mColorOpSB = NULL;
GFXStateBlock* CObjectViewer::mFillSolidSB = NULL;
GFXStateBlock* CObjectViewer::mFillWireSB = NULL;
//render world
GFXStateBlock* CObjectViewer::mSetWorldSB = NULL;

//-----------------------------------------------------------------------------
// CObjectViewer::Model
//-----------------------------------------------------------------------------
bool CObjectViewer::mAltFlag = false;
bool CObjectViewer::mShowMesh = false;
bool CObjectViewer::mShowBounds = true;
bool CObjectViewer::mShowCollection = false;
bool CObjectViewer::mPlayAnimation = true;
F32  CObjectViewer::mAnimationScale = 1.0f;
bool CObjectViewer::mAnimationLoop = false;
bool CObjectViewer::mShowLightDir = true;
bool CObjectViewer::mlightEnable = false;
CObjectViewer::Model* CObjectViewer::mCurrentObject = NULL;

S32	CObjectViewer::mFillMode = 0;
S32 CObjectViewer::Model::shapeIdseed = 0;
Vector<char *> gCurrentSearchObject;			// 当前搜索对象
int gCurrentSearchStep = 0;
char NodeName[1024];

#ifdef NTJ_EDITOR
extern bool gPreviewMission;
extern ColorI gCanvasClearColor;
#endif

static VectorF axisVector[3] = {
	VectorF(1.0f,0.0f,0.0f),
	VectorF(0.0f,1.0f,0.0f),
	VectorF(0.0f,0.0f,1.0f)
};

static ColorI axisVectorColor[3] = {
	ColorI(255,0,0),
	ColorI(0,255,0),
	ColorI(0,0,255)
};

static Point3F BoxPnts[] = {
	Point3F(0.0f,0.0f,0.0f),
	Point3F(0.0f,0.0f,1.0f),
	Point3F(0.0f,1.0f,0.0f),
	Point3F(0.0f,1.0f,1.0f),
	Point3F(1.0f,0.0f,0.0f),
	Point3F(1.0f,0.0f,1.0f),
	Point3F(1.0f,1.0f,0.0f),
	Point3F(1.0f,1.0f,1.0f)
};

static U32 BoxVerts[][4] = {
	{7,6,4,5},     // +x
	{0,2,3,1},     // -x
	{7,3,2,6},     // +y
	{0,1,5,4},     // -y
	{7,5,1,3},     // +z
	{0,4,6,2}      // -z
};

static Point3F ConePnts[] = {
	Point3F(0.0f, 0.0f, 0.0f),
	Point3F(-1.0f, 0.0f, -0.25f),
	Point3F(-1.0f, -0.217f, -0.125f),
	Point3F(-1.0f, -0.217f, 0.125f),
	Point3F(-1.0f, 0.0f, 0.25f),
	Point3F(-1.0f, 0.217f, 0.125f),
	Point3F(-1.0f, 0.217f, -0.125f),
	Point3F(-1.0f, 0.0f, 0.0f)
};

static U32 ConeVerts[][3] = {
	{0, 2, 1},
	{0, 3, 2},
	{0, 4, 3},
	{0, 5, 4},
	{0, 6, 5},
	{0, 1, 6},
	{7, 1, 6}, // Base
	{7, 6, 5},
	{7, 5, 4},
	{7, 4, 3},
	{7, 3, 2},
	{7, 2, 1}
};

const char *trim(const char*str,char begin,char end)
{
	static char word[1024];
	int len = dStrlen(str);
	for(int i=len-1;i>=0;i--)
	{
		if(str[i]==end)
		{
			for(int j=i;j>=0;j--)
			{
				if(str[j]==begin)
				{
					int size = i-j-1;
					memcpy(word,&str[j+1],size);
					word[size]=0;
					return word;
				}
			}
		}
	}

	return NULL;
}

void renderArrows(F32 mAxisGizmoProjLen)
{
#ifdef STATEBLOCK
	AssertFatal(CObjectViewer::mColorOpStoreSB, "renderArrows -- CObjectViewer::mColorOpStoreSB cannot be NULL.");
	CObjectViewer::mColorOpStoreSB->capture();
	AssertFatal(CObjectViewer::mColorOpSB, "renderArrows -- CObjectViewer::mColorOpSB cannot be NULL.");
	CObjectViewer::mColorOpSB->apply();
#else
	U32 stage1ColorOp = GFX->getTextureStageState(0, GFXTSSColorOp);
	GFX->setTextureStageColorOp( 0, GFXTOPDisable );
#endif
	F32 tipScale = mAxisGizmoProjLen * 0.25;

	PrimBuild::begin( GFXTriangleList, 12*3*3 );

	S32 x, y, z;
	Point3F pnt;

	for(U32 axis = 0; axis < 3; ++axis)
	{
		PrimBuild::color( axisVectorColor[axis] );

		if(axis == 0)
		{
			x = 0;
			y = 1;
			z = 2;
		}
		else if(axis == 1)
		{
			x = 2;
			y = 0;
			z = 1;
		}
		else
		{
			x = 1;
			y = 2;
			z = 0;
		}

		for(U32 i = 0; i < sizeof(ConeVerts) / (sizeof(U32)*3); ++i)
		{
			Point3F& conePnt0 = ConePnts[ConeVerts[i][0]];
			pnt.set(conePnt0[x], conePnt0[y], conePnt0[z]);
			PrimBuild::vertex3fv(pnt * tipScale + axisVector[axis] * mAxisGizmoProjLen);

			Point3F& conePnt1 = ConePnts[ConeVerts[i][1]];
			pnt.set(conePnt1[x], conePnt1[y], conePnt1[z]);
			PrimBuild::vertex3fv(pnt * tipScale + axisVector[axis] * mAxisGizmoProjLen);

			Point3F& conePnt2 = ConePnts[ConeVerts[i][2]];
			pnt.set(conePnt2[x], conePnt2[y], conePnt2[z]);
			PrimBuild::vertex3fv(pnt * tipScale + axisVector[axis] * mAxisGizmoProjLen);
		}
	}

	PrimBuild::end();
#ifdef STATEBLOCK
	AssertFatal(CObjectViewer::mColorOpStoreSB, "renderArrows -- CObjectViewer::mColorOpStoreSB cannot be NULL.");
	CObjectViewer::mColorOpStoreSB->apply();
#else
	GFX->setTextureStageColorOp( 0, GFXTextureOp(stage1ColorOp) );
#endif
}

void DrawLine(Point3F &begin,Point3F &end,ColorI color,bool flag=false)
{
#ifdef STATEBLOCK
	AssertFatal(CObjectViewer::mColorOpStoreSB, "DrawLine -- CObjectViewer::mColorOpStoreSB cannot be NULL.");
	CObjectViewer::mColorOpStoreSB->capture();
	AssertFatal(CObjectViewer::mColorOpSB, "DrawLine -- CObjectViewer::mColorOpSB cannot be NULL.");
	CObjectViewer::mColorOpSB->apply();
#else
	U32 stage1ColorOp = GFX->getTextureStageState(0, GFXTSSColorOp);
	GFX->setTextureStageColorOp( 0, GFXTOPDisable );
#endif

	PrimBuild::begin( GFXLineList, 2 );
	PrimBuild::color( color );
	PrimBuild::vertex3fv( begin );
	PrimBuild::vertex3fv( end );
	PrimBuild::end();

	if(!flag)
		return;

	F32 mAxisGizmoProjLen = 1;
	F32 tipScale = mAxisGizmoProjLen * 0.25;
	PrimBuild::begin( GFXTriangleList, 12*3*3 );
	S32 x, y, z;
	Point3F pnt;
	U32 axis = 1;
		
	GFX->pushWorldMatrix();

	Point3F dir = end-begin;
	dir.normalize();
	MatrixF newcenter = MathUtils::createOrientFromDir(dir);
	newcenter.setPosition(begin);
	GFX->multWorld(newcenter);

	PrimBuild::color( ColorI(255,255,0) );
	if(axis == 0)
	{
		x = 0;
		y = 1;
		z = 2;
	}
	else if(axis == 1)
	{
		x = 2;
		y = 0;
		z = 1;
	}
	else
	{
		x = 1;
		y = 2;
		z = 0;
	}

	for(U32 i = 0; i < sizeof(ConeVerts) / (sizeof(U32)*3); ++i)
	{
		Point3F& conePnt0 = ConePnts[ConeVerts[i][0]];
		pnt.set(conePnt0[x], conePnt0[y], conePnt0[z]);
		PrimBuild::vertex3fv(pnt * tipScale + axisVector[axis] * mAxisGizmoProjLen);

		Point3F& conePnt1 = ConePnts[ConeVerts[i][1]];
		pnt.set(conePnt1[x], conePnt1[y], conePnt1[z]);
		PrimBuild::vertex3fv(pnt * tipScale + axisVector[axis] * mAxisGizmoProjLen);

		Point3F& conePnt2 = ConePnts[ConeVerts[i][2]];
		pnt.set(conePnt2[x], conePnt2[y], conePnt2[z]);
		PrimBuild::vertex3fv(pnt * tipScale + axisVector[axis] * mAxisGizmoProjLen);
	}

	PrimBuild::end();

	GFX->popWorldMatrix();
#ifdef STATEBLOCK
	AssertFatal(CObjectViewer::mColorOpStoreSB, "DrawLine -- CObjectViewer::mColorOpStoreSB cannot be NULL.");
	CObjectViewer::mColorOpStoreSB->apply();
#else
	GFX->setTextureStageColorOp( 0, GFXTextureOp(stage1ColorOp) );
#endif

}

void DrawAxis(MatrixF &mat,Point3F &center,F32 len)
{
	VectorF axis[3];

	mat.mulV(center);

	for(int i=0;i<3;i++)
	{
		mat.mulV(axisVector[i], &axis[i]);
		axis[i].normalize();
	}

#ifdef STATEBLOCK
	AssertFatal(CObjectViewer::mColorOpStoreSB, "DrawAxis -- CObjectViewer::mColorOpStoreSB cannot be NULL.");
	CObjectViewer::mColorOpStoreSB->capture();
	AssertFatal(CObjectViewer::mColorOpSB, "DrawAxis -- CObjectViewer::mColorOpSB cannot be NULL.");
	CObjectViewer::mColorOpSB->apply();
#else
	U32 stage1ColorOp = GFX->getTextureStageState(0, GFXTSSColorOp);
	GFX->setTextureStageColorOp( 0, GFXTOPDisable );
#endif
	PrimBuild::begin( GFXLineList, 6 );
	for(U32 i = 0; i < 3; i++)
	{
		PrimBuild::color( axisVectorColor[i]);
		PrimBuild::vertex3fv( center );
		PrimBuild::vertex3fv( center + axis[i] * len );
	}
	PrimBuild::end();
#ifdef STATEBLOCK
	AssertFatal(CObjectViewer::mColorOpStoreSB, "DrawAxis -- CObjectViewer::mColorOpStoreSB cannot be NULL.");
	CObjectViewer::mColorOpStoreSB->apply();
#else
	GFX->setTextureStageColorOp( 0, GFXTextureOp(stage1ColorOp) );
#endif
}

void DrawBox(Box3F &box,ColorI &color)
{
	Point3F projPnts[8];
	for(U32 i = 0; i < 8; i++)
	{
		projPnts[i].set(BoxPnts[i].x ? box.max.x : box.min.x,
			BoxPnts[i].y ? box.max.y : box.min.y,
			BoxPnts[i].z ? box.max.z : box.min.z);
	}

#ifdef STATEBLOCK
	AssertFatal(CObjectViewer::mColorOpStoreSB, "DrawBox -- CObjectViewer::mColorOpStoreSB cannot be NULL.");
	CObjectViewer::mColorOpStoreSB->capture();
	AssertFatal(CObjectViewer::mColorOpSB, "DrawBox -- CObjectViewer::mColorOpSB cannot be NULL.");
	CObjectViewer::mColorOpSB->apply();
#else
	U32 stage1ColorOp = GFX->getTextureStageState(0, GFXTSSColorOp);
	GFX->setTextureStageColorOp( 0, GFXTOPDisable );
#endif
	// do the box
	for(U32 j = 0; j < 6; j++)
	{
		PrimBuild::begin( GFXLineStrip, 4 );
		for(U32 k = 0; k < 4; k++)
		{
			PrimBuild::color( color);
			PrimBuild::vertex3fv( projPnts[BoxVerts[j][k]] );
		}
		PrimBuild::end();
	}
#ifdef STATEBLOCK
	AssertFatal(CObjectViewer::mColorOpStoreSB, "DrawBox -- CObjectViewer::mColorOpStoreSB cannot be NULL.");
	CObjectViewer::mColorOpStoreSB->apply();
#else
	GFX->setTextureStageColorOp( 0, GFXTextureOp(stage1ColorOp) );
#endif
}

Box3F BuildBox(Point3F center,F32 len)
{
	Box3F box;
	Point3F ext(len,len,len);

	box.min = center - ext;
	box.max = center + ext;
	return box;
}

CObjectViewer::Model::Model():
mountNodeParent(-1),
mountNodeSelf(-1),
curNode(-1),
action(GameObjectData::NullAnimation),
shapeInstance(NULL),
animThread(NULL),
parent(NULL),
curMesh(-1)
{
	shapeId = shapeIdseed++;
	actionFileName[0] = 0;
}

CObjectViewer::Model::~Model()
{
	Vector<Model*> temp = child;

	for(int i=0;i<temp.size();i++)
	{
		delete temp[i];
	}

	if(parent)
		parent->deleteMountChild(this);

	clear();
}

void CObjectViewer::Model::clear()
{
	mountNodeParent = -1;
	mountNodeSelf = -1;
	action = GameObjectData::NullAnimation;
	SAFE_DELETE(shapeInstance);
	animThread = NULL;
	parent = NULL;
	skinNameHandle.setFromIndex(0);
	shape = NULL;
}

void CObjectViewer::Model::rebuildMaterials()
{
    return;

	if(shapeInstance)
	{
		TSMaterialList* pMaterials = shapeInstance->getMaterialList();
		for(int j=0;j<pMaterials->getMaterialCount();j++)
		{
			MatInstance *pInst = pMaterials->getMaterialInst(j);
			if(pInst)
			{
				Material *pMaterial = pInst->getMaterial();
				if(pMaterial && !pInst->isSelfIlluminating())
				{
					if(CObjectViewer::mlightEnable)
						pMaterial->emissive[0] = 0;
					else
						pMaterial->emissive[0] = 1;

					pInst->reInit();
				}
			}
		}
	}

	for(int i=0;i<child.size();i++)
	{
		child[i]->rebuildMaterials();
	}
}

bool CObjectViewer::Model::loadShape(StringTableEntry filename)
{
	clear();

    bool computeCRC = false;
#ifdef NTJ_EDITOR
    if ( gPreviewMission )
        computeCRC = true;
#endif
    shape = ResourceManager->load(filename, computeCRC);
	if(!shape)
		return false;

	shapeInstance = new TSShapeInstance(shape, true);

	for (int i = 0; i < shape->details.size(); i++)
	{
		char* name = (char*)shape->names[shape->details[i].nameIndex];

		if (dStrstr((const char*)dStrlwr(name), "collision-"))
		{
			CollisionDetails.push_back(i);
		}
		else if(!dStrnicmp(name,"detail",6))
		{
			MeshDetails.push_back(i);
		}
	}

	MatrixF mat(true);
	VectorF v;

	feature.setSize(MeshDetails.size());

	for(int i=0;i<MeshDetails.size();i++)
	{
		shapeInstance->getFeatures(mat,v,&feature[i],MeshDetails[i]);
	}

	rebuildMaterials();

	S32 sequence = shape->findSequence("ambient");
	if(sequence != -1)
	{
		animThread= shapeInstance->addThread();
		shapeInstance->setPos(animThread, 0);
		shapeInstance->setTimeScale(animThread, mAnimationScale);
		shapeInstance->setSequence(animThread, sequence, 0);
	}
	return true;
}

bool CObjectViewer::Model::mountShape(StringTableEntry modelName,StringTableEntry parentPoint,StringTableEntry childPoint)
{
	Model *pObject = new Model;
	if(pObject->loadShape(modelName))
	{
		if(pObject->setMountParent(this,parentPoint,childPoint))
			return true;
	}

	delete pObject;
	return false;
}

void CObjectViewer::Model::selectNode(StringTableEntry node)
{
	S32 inode = shape->findNode(node);
	if(inode!=-1)
		curNode = inode;
}

void CObjectViewer::Model::selectMesh(S32 i)
{
	curMesh = i;
}

bool CObjectViewer::Model::isEmpty()
{
	return !(shapeInstance);
}

void CObjectViewer::Model::clearAction()
{
	if(!shape)
		return;
	shape->sequences.clear();
	if(shapeInstance->threadCount())
		shapeInstance->destroyThread(shapeInstance->getThread(0));

	animThread = NULL;
	actionFileName[0]=0;
}

void CObjectViewer::Model::loadAction(StringTableEntry filename)
{
    if(!shape)
        return;

    Stream* stream;
    stream = ResourceManager->openStream(filename);
    if (!stream)
        return;

    clearAction();

    if (!shape->importSequences(stream) || stream->getStatus() != Stream::Ok)
    {			
        ResourceManager->closeStream(stream);
        return;
    }
    ResourceManager->closeStream(stream);

    animThread = shapeInstance->addThread();
    if(animThread)
    {
        shapeInstance->setTimeScale(animThread,mAnimationScale);
        shapeInstance->setSequence(animThread,0,0);
        dStrcpy(actionFileName, sizeof(actionFileName), filename);
    }
}

void CObjectViewer::Model::transitionAction(StringTableEntry filename)
{
    if(!shape || !shapeInstance)
        return;       

    F32 prevPos = animThread ? shapeInstance->getPos(animThread) : 0.0f;
    char prevFilename[256];
    dStrcpy(prevFilename, 256, actionFileName);
    clearAction();

    bool hasPrevSeq = false;
    Stream* stream = ResourceManager->openStream(prevFilename);
    if (stream)
    {
        if (shape->importSequences(stream) && stream->getStatus() == Stream::Ok)
            hasPrevSeq = true;

        ResourceManager->closeStream(stream);
    }

    bool hasCurrSeq = false;
    stream = ResourceManager->openStream(filename);
    if (stream)
    {
        if (shape->importSequences(stream) && stream->getStatus() == Stream::Ok)
            hasCurrSeq = true;

        ResourceManager->closeStream(stream);
    }

    animThread = shapeInstance->addThread();
    if (animThread && hasCurrSeq)
    {
        shapeInstance->setTimeScale(animThread,mAnimationScale);

        if (!hasPrevSeq) 
            shapeInstance->setSequence(animThread,0,0);        
        else
        {
            shapeInstance->setSequence(animThread,0,0);
            shapeInstance->setPos(animThread, prevPos);

            shapeInstance->transitionToSequence(animThread,1,0, 0.1f, true);
        }
    }

    dStrcpy(actionFileName, sizeof(actionFileName), filename);
}

bool CObjectViewer::Model::addMountChild(Model* childModel)
{
	if(isEmpty() || !childModel || childModel->isEmpty())
		return false;

	child.push_back(childModel);
	return true;
}

bool CObjectViewer::Model::deleteMountChild(Model* childModel)
{
	if(isEmpty() || !childModel || childModel->isEmpty())
		return false;

	Vector<Model*>::iterator it = child.begin();
	while(it!=child.end())
	{
		if(*it == childModel)
		{
			child.erase(it);
			break;
		}
		it++;
	}

	return true;
}

bool CObjectViewer::Model::setMountParent(Model* parentModel, StringTableEntry parentPoint, StringTableEntry selfPoint)
{
	if(isEmpty() || !parentModel || parentModel->isEmpty())
		return false;

	S32 parentNode = parentPoint ? parentModel->shape->findNode(parentPoint) : -1;
	S32 selfNode = selfPoint ? shape->findNode(selfPoint) : -1;

	// 自己可以没有链接点
	if(parentNode > -1)
	{
		parent = parentModel;
		mountNodeParent = parentNode;
		mountNodeSelf = selfNode;

		parentModel->addMountChild(this);
		return true;
	}
	return false;
}

void CObjectViewer::Model::getMountMatrix(StringTableEntry linkPoint, MatrixF& mat)
{
	if(isEmpty() || !parent || parent->isEmpty())
		return;

	// linkPoint为空时取原目标链接点
	if(linkPoint)
		mountNodeParent = parent->shape->findNode(linkPoint);

	// 这里和游戏中有些不同，这里取得的坐标是parent坐标系的，并不是世界坐标
	if(mountNodeParent > -1)
	{
		mat = parent->shapeInstance->mNodeTransforms[mountNodeParent];
		if(mountNodeSelf > -1)
		{
			MatrixF mountTrans = shapeInstance->mNodeTransforms[mountNodeSelf];
			mat.mul(mountTrans.inverse());
		}
	}
}

void CObjectViewer::Model::getWorldMX(MatrixF &mat)
{
	if(parent)
	{
		parent->getWorldMX(mat);

		MatrixF parentTrans = parent->shapeInstance->mNodeTransforms[mountNodeParent];
		if(mountNodeSelf > -1)
		{
			MatrixF mountTrans = shapeInstance->mNodeTransforms[mountNodeSelf];
			parentTrans.mul(mountTrans.inverse());
		}

		mat = mat * parentTrans;
	}
	else
	{
		mat.identity();
	}
}

void CObjectViewer::Model::getNodeMX(S32 mountNode,MatrixF &mat)
{
	getWorldMX(mat);

	if(mountNode > -1)
	{
		MatrixF mountTrans = shapeInstance->mNodeTransforms[mountNode];
		mat.mul(mountTrans);
	}
}

CObjectViewer::Model *CObjectViewer::Model::selectShape(S32 sId)
{
	if(shapeId==sId)
		return this;

	CObjectViewer::Model *temp;
	for(int i=0;i<child.size();i++)
	{
		temp = child[i]->selectShape(sId);
		if(temp)
			return temp;
	}

	return NULL;
}

StringTableEntry CObjectViewer::Model::getActionFileName()
{
	return actionFileName;
}

StringTableEntry CObjectViewer::Model::getShapeFileName()
{
	if(!shape)
		return "";

	static char msg[1024];
	const char *temp = dStrstr(shape.getFilePath(),"gameres");
	dSprintf(msg,1024,"%s/%s",temp,shape.getFileName());
	return msg;
}

int CObjectViewer::Model::getTextureCount()
{
	return shape->materialList->mMaterials.size();
}

StringTableEntry CObjectViewer::Model::getTextureName(int i)
{
	GFXTexHandle &texHandle = shape->materialList->mMaterials[i];
	if(texHandle.isNull())
		return "";

	int len = dStrlen(texHandle->mTextureFileName);
	int pos;
	for(pos=len-1;pos>=0;pos--)
	{
		if(shape->materialList->mMaterials[i]->mTextureFileName[pos] == '/')
		{
			pos++;
			break;
		}
	}

	return &shape->materialList->mMaterials[i]->mTextureFileName[pos];
}

int CObjectViewer::Model::getMeshFaceCount(int i)
{
	return feature[i].mFaceList.size();
}

int CObjectViewer::Model::getCollisionCount()
{
	return CollisionDetails.size();
}

int CObjectViewer::Model::getMeshCount(int i)
{
	S32 dl, ss, od;
	S32 meshCount=0;

	const TSDetail * detail = NULL;
	dl = MeshDetails[i];
	detail = &shape->details[dl];
	ss = detail->subShapeNum;
	od = detail->objectDetailNum;

	S32 start = shape->subShapeFirstObject[ss];
	S32 end   = shape->subShapeFirstObject[ss] + shape->subShapeNumObjects[ss];
	for (i=start; i<end; i++)
	{
		TSShapeInstance::MeshObjectInstance *Mesh = &shapeInstance->mMeshObjects[i];
		if(Mesh->getMesh(od))
		{
			meshCount++;
		}
	}

	return meshCount;
}

int CObjectViewer::Model::getDetailCount()
{
	return MeshDetails.size();
}

int CObjectViewer::Model::getNodeCount()
{
	return shape->nodes.size();
}

StringTableEntry CObjectViewer::Model::getNodeName(int i)
{
	int idx = shape->nodes[i].nameIndex;
	dSprintf(NodeName,1024,"[%s]",shape->names[idx]);
	return NodeName;
}

StringTableEntry CObjectViewer::Model::getMountNodeName()
{
	if(mountNodeSelf==-1)
		return "";

	int idx = shape->nodes[mountNodeSelf].nameIndex;

	Vector<const char *> &vt = shape->names;
	return shape->names[idx];
}

StringTableEntry CObjectViewer::Model::getParentNodeName()
{
	if(mountNodeParent==-1)
		return "";

	int idx = parent->shape->nodes[mountNodeParent].nameIndex;

	Vector<const char *> &vt = parent->shape->names;
	return parent->shape->names[idx];
}

void CObjectViewer::Model::renderCollection()
{
	S32 dl, ss, od;
	S32 meshCount=0;
	const TSDetail * detail = NULL;


#ifdef STATEBLOCK
	AssertFatal(mColorOpStoreSB, "CObjectViewer::Model::renderCollection -- mColorOpStoreSB cannot be NULL.");
	mColorOpStoreSB->capture();
	AssertFatal(mSetCollSB, "CObjectViewer::Model::renderCollection -- mSetCollSB cannot be NULL.");
	mSetCollSB->apply();
#else
	U32 stage1ColorOp = GFX->getTextureStageState(0, GFXTSSColorOp);
	GFX->setTextureStageColorOp( 0, GFXTOPModulateInvColorAddAlpha );

	GFX->setFillMode((GFXFillMode)GFXFillWireframe);
#endif
	for(int j=0;j<CollisionDetails.size();j++)
	{
		dl = CollisionDetails[j];
		detail = &shape->details[dl];
		ss = detail->subShapeNum;
		od = detail->objectDetailNum;

		S32 start = shape->subShapeFirstObject[ss];
		S32 end   = shape->subShapeFirstObject[ss] + shape->subShapeNumObjects[ss];
		for (int i=start; i<end; i++)
		{
			const TSObject* obj = &shape->objects[i];

			if (obj->numMeshes && od < obj->numMeshes) 
			{
				TSMesh* mesh = shape->meshes[obj->startMeshIndex + od];
				if (mesh)
				{
					GFX->pushWorldMatrix();
					if(obj->nodeIndex!=-1)
					{
						GFX->multWorld(shapeInstance->mNodeTransforms[obj->nodeIndex]);
					}

					mesh->render();
					GFX->popWorldMatrix();
				}	
			}
		}
	}

#ifdef STATEBLOCK
	if(mFillMode!=1)
	{
		AssertFatal(mFillSolidSB, "CObjectViewer::Model::renderCollection -- mFillSolidSB cannot be NULL.");
		mFillSolidSB->apply();
	}
	//AssertFatal(mColorOpStoreSB, "CObjectViewer::Model::renderCollection -- mColorOpStoreSB cannot be NULL.");
	mColorOpStoreSB->apply();

#else
	if(mFillMode!=1)
		GFX->setFillMode((GFXFillMode)GFXFillSolid);

	GFX->setTextureStageColorOp( 0,(GFXTextureOp) stage1ColorOp );
#endif

}

void CObjectViewer::Model::renderNode()
{
	int idx,parent;
	Box3F box;
	Point3F center(0,0,0);
	ColorI R(255,0,0),R2(100,0,0),G(0,255,0),Y(255,255,0),W(255,255,255);
	MatrixF total;
	for(int i=0;i<shape->nodes.size();i++)
	{
		TSShape::Node &node = shape->nodes[i];
		idx = node.nameIndex;

		MatrixF mat = shapeInstance->mNodeTransforms[i];
		GFX->pushWorldMatrix();
		GFX->multWorld(mat);
		
		if(curNode == i)
		{
			box = BuildBox(center,0.01);
			DrawBox(box,W);
		}
		else
		if(!dStrnicmp(shape->names[idx],"Bip",3))
		{
			box = BuildBox(center,0.01);
			DrawBox(box,R);
		}
		else if(!dStrnicmp(shape->names[idx],"bone",4))
		{
			box = BuildBox(center,0.01);
			DrawBox(box,R2);
		}
		else if(!dStrnicmp(shape->names[idx],"dummy",4))
		{
			box = BuildBox(center,0.01);
			DrawBox(box,G);
		}
		else if(!dStrnicmp(shape->names[idx],"link",4))
		{
			box = BuildBox(center,0.01);
			DrawBox(box,Y);
		}

		GFX->popWorldMatrix();
	}

	for(int i=0;i<shape->nodes.size();i++)
	{
		TSShape::Node &node = shape->nodes[i];
		idx = node.nameIndex;
		parent = node.parentIndex;

		if(parent==-1)
			continue;

		MatrixF mat = shapeInstance->mNodeTransforms[i];
		MatrixF Parentmat = shapeInstance->mNodeTransforms[parent];

		Point3F begin = mat.getPosition();
		Point3F end = Parentmat.getPosition();
		DrawLine(begin,end,ColorI(255,255,255));
	}
}

void CObjectViewer::Model::render(F32 dt)
{
	Model *pWalk = NULL;

	GFX->pushWorldMatrix();
	if(mountNodeParent!=-1)
	{
		MatrixF mat = parent->shapeInstance->mNodeTransforms[mountNodeParent];
		GFX->multWorld(mat);
	}
	if(mountNodeSelf!=-1)
	{
		MatrixF mat = shapeInstance->mNodeTransforms[mountNodeSelf];
		mat.inverse();
		GFX->multWorld(mat);
	}

	if(curNode!=-1)
	{
		GFX->pushWorldMatrix();
		MatrixF mat = shapeInstance->mNodeTransforms[curNode];
		GFX->multWorld(mat);
		MatrixF idt(true);
		DrawAxis(idt,Point3F(0,0,0),0.5);
		renderArrows(0.5);
		GFX->popWorldMatrix();
	}

	if(CObjectViewer::mShowBounds)
	{
		if(CObjectViewer::mCurrentObject == this)
        {
            Point3F tempPosWorld,tempPosProj;
            Point2I tempScreenPos;
            MatrixF tempWorldMX;
            char    tempChar[128];

            ColorI color(255,255,0),color2(255, 0 ,0);
            DrawBox(shape->bounds,color); 
            //画最大值点和最小值点坐标
            CommonFontEX* tempFont = CommonFontManager::GetFont("微软雅黑" , 24);
            CObjectViewer::mCurrentObject->getWorldMX(tempWorldMX);
            tempPosWorld = tempWorldMX.getPosition();
            Point3F maxPos = tempPosWorld + shape->bounds.max;
            Point3F minPos = tempPosWorld + shape->bounds.min;

            Point3F Delta = Point3F(0.01f , 0.01f , 0.01f);
            Box3F redSpotmax(maxPos - Delta , maxPos + Delta);
            DrawBox(redSpotmax,color2); 
            Box3F redSpotmin(minPos - Delta , minPos + Delta);
            DrawBox(redSpotmin,color2);

            MathUtils::projectWorldToScreen(maxPos,tempPosProj,GFX->getViewport(),GFX->getWorldMatrix(),GFX->getProjectionMatrix());
            tempScreenPos.x = tempPosProj.x;
            tempScreenPos.y = tempPosProj.y;
            sprintf_s(tempChar , "(%.3f , %.3f , %.3f)" , shape->bounds.max.x, shape->bounds.max.y ,  shape->bounds.max.z);            
            GFX->getDrawUtil()->drawText(tempFont, tempScreenPos, tempChar, NULL);

            MathUtils::projectWorldToScreen(minPos,tempPosProj,GFX->getViewport(),GFX->getWorldMatrix(),GFX->getProjectionMatrix());
            tempScreenPos.x = tempPosProj.x;
            tempScreenPos.y = tempPosProj.y;
            sprintf_s(tempChar , "(%.3f , %.3f , %.3f)" , shape->bounds.min.x, shape->bounds.min.y ,  shape->bounds.min.z);            
            GFX->getDrawUtil()->drawText(tempFont, tempScreenPos, tempChar, NULL);
		}
	}

	GFX->pushWorldMatrix();
	// animate and render in a run pose
	if(animThread && CObjectViewer::mPlayAnimation)
	{
		F32 pos = shapeInstance->getPos(animThread);
		if(pos==1.0f && CObjectViewer::mAnimationLoop)
        {
			shapeInstance->setPos(animThread,0.0f);
            if ( smObjectViewer->mBasePart && smObjectViewer->mBasePart->parent == this )
                smObjectViewer->mBasePart->reset = true;
            for ( int i = 0; i < smObjectViewer->mLinkParts.size(); i++ )
            {
                if ( smObjectViewer->mLinkParts[i] && smObjectViewer->mLinkParts[i]->parent == this )
                    smObjectViewer->mLinkParts[i]->reset = true;
            }
        }
		else
			shapeInstance->advanceTime( dt/1000.f, animThread );
		shapeInstance->animate();
	}

	if(CObjectViewer::mFillMode!=2)
		shapeInstance->render();
	else
	{
		renderNode();
	}
	GFX->popWorldMatrix();

	GFX->pushWorldMatrix();
	if(CObjectViewer::mShowCollection)
	{
		renderCollection();
	}
	GFX->popWorldMatrix();


	for(int i=0;i<child.size();i++)
	{
		GFX->pushWorldMatrix();
		pWalk = child[i];
		pWalk->render(dt);
		GFX->popWorldMatrix();
	}

	GFX->popWorldMatrix();
}

void CObjectViewer::Model::fillItemMsg(StringTableEntry msg)
{
	int size;
	if(msg)
		size = dStrlen(msg) + dStrlen(shape->mSourceResource->name) + 10;
	else 
		size =  dStrlen(shape->mSourceResource->name) + 10;
	char *pBuff = new char[size];
	if(msg)
		dSprintf(pBuff,size,"%s/%s(%d)",msg,shape->mSourceResource->name,shapeId);
	else
		dSprintf(pBuff,size,"%s(%d)",shape->mSourceResource->name,shapeId);

	gCurrentSearchObject.push_back(pBuff);

	for(int i=0;i<child.size();i++)
	{
		child[i]->fillItemMsg(pBuff);
	}
}

void CObjectViewer::Model::fillMeshMsg()
{
	S32 dl, ss, od;
	S32 start,end;
	char msg[1024];
	S32 size;
	char *pBuff;

	for( int i=0;i<MeshDetails.size();i++)
	{
		const TSDetail * detail = NULL;
		dl = MeshDetails[i];
		detail = &shape->details[dl];
		ss = detail->subShapeNum;
		od = detail->objectDetailNum;

		start = shape->subShapeFirstObject[ss];
		end   = shape->subShapeFirstTranslucentObject[ss];
		for (int j=start; j<end; j++)
		{
			TSShapeInstance::MeshObjectInstance *Mesh = &shapeInstance->mMeshObjects[j];
			TSMesh *tsMesh = Mesh->getMesh(od);
			if(tsMesh)
			{
				dSprintf(msg,1024,"Detail-%d(共%d面)/Mesh-%d(%d次实体DP)",i,feature[i].mFaceList.size(),j,tsMesh->primitives.size());
				size = dStrlen(msg)+10;
				pBuff = new char[size];
				dStrcpy(pBuff, size, msg);
				gCurrentSearchObject.push_back(pBuff);
			}
		}

		start = shape->subShapeFirstTranslucentObject[ss];
		end   = shape->subShapeFirstObject[ss] + shape->subShapeNumObjects[ss];
		for (int j=start; j<end; j++)
		{
			TSShapeInstance::MeshObjectInstance *Mesh = &shapeInstance->mMeshObjects[j];
			TSMesh *tsMesh = Mesh->getMesh(od);
			if(tsMesh)
			{
				dSprintf(msg,1024,"Detail-%d(共%d面)/Mesh-%d(%d次透明DP)",i,feature[i].mFaceList.size(),j,tsMesh->primitives.size());
				size = dStrlen(msg)+10;
				pBuff = new char[size];
				dStrcpy(pBuff, size, msg);
				gCurrentSearchObject.push_back(pBuff);
			}
		}
	}
}

S32 FN_CDECL cmpStrFunc(const void* p1, const void* p2)
{
	const char* mse1 = (const char*) p1;
	const char* mse2 = (const char*) p2;

	int size1 = dStrlen(mse1);
	int size2 = dStrlen(mse2);
	for(int i=0;(i<size1&&i<size2);i++)
	{
		int val = mse2[i]-mse1[i];
		if(val) return val;
	}

	return size2-size1;
} 

void CObjectViewer::Model::fillNodeMsg()
{
	S32 size;
	char *pBuff;
	int Num = shape->nodes.size();
	for(int i=0;i<Num;i++)
	{
		int idx = shape->nodes[i].nameIndex;
		dSprintf(NodeName,1024,"%s",shape->names[idx]);

		size = dStrlen(NodeName)+10;
		pBuff = new char[size];
		dStrcpy(pBuff, size, NodeName);
		gCurrentSearchObject.push_back(pBuff);
	}

	for(int i=0;i<gCurrentSearchObject.size();i++)
	{
		for(int j=0;j<gCurrentSearchObject.size()-1;j++)
		{
			if(cmpStrFunc(gCurrentSearchObject[j],gCurrentSearchObject[j+1])<0)
			{
				char *tmp = gCurrentSearchObject[j];
				gCurrentSearchObject[j] = gCurrentSearchObject[j+1];
				gCurrentSearchObject[j+1] = tmp;
			}
		}
	}
}

void CObjectViewer::Model::setTimeScale(F32 scale)
{
	if(shapeInstance && animThread)
		shapeInstance->setTimeScale(animThread,scale);

	for(int i=0;i<child.size();i++)
	{
		child[i]->setTimeScale(scale);
	}
}

CObjectViewer::Part::Part()
{
    name[0] = 0;
    part = NULL;
    parent = NULL;
    parentPointName[0] = 0;
    mountNodeParent = -1;
    reset = false;
}

CObjectViewer::Part::~Part()
{
    if ( part )
        part->deleteObject();
}

bool CObjectViewer::Part::loadPart(StringTableEntry _name)
{
    if ( part )
        part->deleteObject();

    dStrcpy( name, 256, _name );

    part = new ParticleEmitterNode();
    part->setClientObject();
	part->useClosestPoint(true);

    char dbName[512];
    dStrcpy( dbName, 512, _name );    
    dStrcat( dbName, 512, "_Node" );
    ParticleEmitterNodeData* dataBlock = NULL;
    Sim::findObject( dbName, dataBlock );
	 AssertWarn(dataBlock!=NULL, dbName);
    part->onNewDataBlock(dataBlock);   

    if ( !part->registerObject() )
    {
        delete part;
        part = NULL;
        return false;
    }

    return true;
}

bool CObjectViewer::Part::setMountParent(Model* parentModel, StringTableEntry parentPoint)
{
    if ( parentModel && parentModel->shape )
        parent = parentModel;

    if ( parentPoint )
        dStrcpy( parentPointName, 32, parentPoint );

    if ( parent && parentPointName[0] )
        mountNodeParent = parent->shape->findNode(parentPointName); 

    return true;
}

void CObjectViewer::Part::setTime(F32 time)
{
    part->setTime(time);
}

void CObjectViewer::Part::setTimeScale(F32 scale)
{
    part->setTimeScale(scale);
}

void CObjectViewer::Part::render(CObjectViewer *pRoot,SceneState *pSceneState)
{
	const Point3F& camPos = pSceneState->getCameraPosition();

	GFX->pushWorldMatrix();

	GFX->setWorldMatrix(gClientSceneGraph->getLastWorld2ViewMX());  //Ray: 只传入视矩阵

	MatrixF baseMat(true);
	if(parent && parent->shapeInstance && mountNodeParent!=-1)
		parent->getNodeMX(mountNodeParent,baseMat);
	part->setTransform( pRoot->mWorldMatrix * baseMat);  //传入世界矩阵

	part->render(camPos);
	GFX->popWorldMatrix();

    if(CObjectViewer::mFillMode==2)
    {
        GFX->pushWorldMatrix();
        GFX->multWorld( baseMat );

        Box3F box = BuildBox(Point3F(0.0f, 0.0f, 0.0f),0.05f);
        ColorI color(0,0,255);
        DrawBox(box,color);

        GFX->popWorldMatrix();
    }

	//Ray: 需要增加统计所有粒子发射器objBox的最大范围，并且绘制出来
	//if(CObjectViewer::mShowBounds)
	//{
	//	ColorI color(255,255,0);
	//	DrawBox(part->getMaxBounds(),color);
	//}
}

//-----------------------------------------------------------------------------
// CObjectViewer
//-----------------------------------------------------------------------------
CObjectViewer::CObjectViewer():
	mMouseState(None),
	mLastMousePoint(0, 0),
	lastRenderTime(0),
	mWorldPatch(0),
	mWorldRow(0),
	mCameraZoom(0),
	mWorldMoveX(0),
	mWorldMoveZ(0),
	mLightRotX(0),
	mLightRotZ(0),
	mFakeSun(NULL)
{
	mActive = true;

	// TODO: lots of hardcoded things in here
	mCameraMatrix.identity();
	mWorldMatrix.identity();

	mLightDir.set(0.f, 0.707f, -0.707f);
	mLightColor.set(0.0f, 0.0f, 0.0f);
	mAmbientColor.set(1.0f, 1.0f, 1.0f);

	mLightDir.normalize();

	mBaseObject = NULL;
	mCurrentObject = NULL;
	//pTerrain = NULL;

    mBasePart = NULL;
    mLinkParts.clear();

	resetCamera();

    smObjectViewer = this;
}

CObjectViewer::~CObjectViewer()
{
    if ( mBasePart )
        delete mBasePart;
    mBasePart = NULL;

    for ( int i = 0; i < mLinkParts.size(); i++ )
        delete mLinkParts[i];
    mLinkParts.clear();

	clearModel();
	clearOther();

	SAFE_DELETE(mFakeSun);
}

void CObjectViewer::initPersistFields()
{
	Parent::initPersistFields();
}

void CObjectViewer::clearModel()
{
	if(mBaseObject == mCurrentObject)
	{
		mCurrentObject = NULL;
	}

	SAFE_DELETE(mBaseObject);
}

void CObjectViewer::clearOther()
{
	//for(int i=0;i<mParticleEmitter.size();i++)
	//{
	//	if(mParticleEmitter[i])
	//	{
	//		mParticleEmitter[i]->deleteObject();
	//	}
	//}
	//mParticleEmitter.clear();

	//for(int i=0;i<mDecalList.size();i++)
	//{
	//	if(mDecalList[i])
	//	{
	//		mDecalList[i]->deleteObject();
	//	}
	//}
	//mDecalList.clear();

	//if(pTerrain)
	//{
	//	pTerrain->deleteObject();
	//	pTerrain = NULL;
	//}
}

bool CObjectViewer::onWake()
{
	if (!Parent::onWake())
		return(false);

	LightManager* lm = gClientSceneGraph->getLightManager();
	if (!mFakeSun)
	{
		mFakeSun = lm->createLightInfo();   
	}
	mFakeSun->mColor = mLightColor;
	mFakeSun->mAmbient = mAmbientColor;
	mFakeSun->mDirection = mLightDir;
	mWorldMatrix.mulV(mFakeSun->mDirection);
	mFakeSun->mDirection.normalize();

	//pTerrain = new TerrainBlock;
	//pTerrain->setPosition(Point3F(-256,-256,-6.25));
	//pTerrain->mTerrFileName = StringTable->insert("gameres/data/missions/newMission.ter");
	//pTerrain->setSquareSize(2);
	//pTerrain->setTiling(false);
	//pTerrain->setTexelsPerMeter(32);
	//pTerrain->setClientObject();
	//if(!pTerrain->registerObject())
	//{
	//	SAFE_DELETE(pTerrain);
	//}

	//sgDecalProjector *pDecal = new sgDecalProjector;
	//pDecal->setClientObject();
	//ProjectDecalData* pData = new ProjectDecalData;
	//if(pData)
	//{
	//	pData->lifeSpan		= 0;
	//	pData->mInterval	= 0;
	//	pData->mRot			= 0;
	//	pData->mTextureName = StringTable->insert("~/data/environments/decal/Arrow");  //StringTable->insert("~/data/environments/decal/dianditubiao1");

	//	pData->mDiameter	= 10;
	//	pData->mDiameterScale		= 0;
	//	pData->mMaxDiameter	= 0;
	//	pData->mAlpha		= 0;
	//	pData->mAlphaScale  = 0;						//alpha变换速度
	//	pData->mMaxAlpha    = 0;							//最大alpha
	//	pData->mLoop		= false;
	//	pData->mLoopType	= 1;

	//	pDecal->setDataBlock( (GameBaseData*)pData );
	//	MatrixF mat(true);
	//	mat.setPosition(Point3F(0,0,0));
	//	pDecal->setScale(Point3F(1,1,1));
	//	if(!pDecal->registerObject())
	//	{
	//		delete pDecal;
	//	}
	//	else
	//	{
	//		pDecal->updateProject(mat);
	//	}

	//	mDecalList.push_back(pDecal);
	//}

	return(true);
}

void CObjectViewer::onMouseDown(const GuiEvent &event)
{
	if (!mActive || !mVisible || !mAwake)
		return;

	mMouseState = MouseDown;
	mLastMousePoint = event.mousePoint;
	mouseLock();
}

void CObjectViewer::onMouseUp(const GuiEvent &event)
{
	mouseUnlock();
	mMouseState = None;
}

void CObjectViewer::onMouseDragged(const GuiEvent &event)
{
	if (mMouseState != MouseDown)
		return;

	Point2I delta = event.mousePoint - mLastMousePoint;
	mLastMousePoint = event.mousePoint;

	mWorldPatch += (delta.y * 0.01f);
	mWorldRow -= (delta.x * 0.01f);
}

void CObjectViewer::onRightMouseDown(const GuiEvent &event)
{
	mMouseState = RightMouseDown;
	mLastMousePoint = event.mousePoint;
	mouseLock();
}

void CObjectViewer::onRightMouseUp(const GuiEvent &event)
{
	mouseUnlock();
	mMouseState = None;
}

void CObjectViewer::onRightMouseDragged(const GuiEvent &event)
{
	if (mMouseState != RightMouseDown)
		return;

	Point2I delta = event.mousePoint - mLastMousePoint;
	mLastMousePoint = event.mousePoint;

	mLightRotX += (delta.y * 0.01f);
	mLightRotZ -= (delta.x * 0.01f);
}

bool CObjectViewer::onMouseWheelUp(const GuiEvent& event)
{
	if (mMouseState != None)
		return false;

	mCameraZoom+=0.5;
	return true;
}

bool CObjectViewer::onMouseWheelDown(const GuiEvent& event)
{
	if (mMouseState != None)
		return false;

	mCameraZoom-=0.5;
	return true;
}

void CObjectViewer::onMiddleMouseDown(const GuiEvent &event)
{
	if (!mActive || !mVisible || !mAwake)
		return;

	mMouseState = MiddleMouseDown;
	mLastMousePoint = event.mousePoint;
	mouseLock();
}

void CObjectViewer::onMiddleMouseUp(const GuiEvent &event)
{
	mouseUnlock();
	mMouseState = None;
}

void CObjectViewer::onMiddleMouseDragged(const GuiEvent &event)
{
	if (mMouseState != MiddleMouseDown)
		return;

	Point2I delta = event.mousePoint - mLastMousePoint;
	mLastMousePoint = event.mousePoint;

	if(mAltFlag)
	{
		mWorldPatch -= (delta.y * 0.01f);
		mWorldRow -= (delta.x * 0.01f);
	}
	else
	{
		mWorldMoveZ +=delta.y * 0.01f;
		mWorldMoveX -=delta.x * 0.01f;
	}
}

bool CObjectViewer::onKeyDown(const GuiEvent &event)
{
	if(event.keyCode == KEY_ALT)
		mAltFlag = true;

	return true;
}

bool CObjectViewer::onKeyUp(const GuiEvent &event)
{
	if(event.keyCode == KEY_ALT)
		mAltFlag = false;

	return true;
}

void CObjectViewer::deleteCurrentShape()
{
	if(mCurrentObject)
	{
		if(mCurrentObject!=mBaseObject)
		{
			Model *temp = mCurrentObject->parent;
			SAFE_DELETE(mCurrentObject);
			mCurrentObject = temp;
		}
		else
		{
			SAFE_DELETE(mCurrentObject);
			mBaseObject = NULL;
		}
	}
}

void CObjectViewer::selectCurrentShape(int shapeId)
{
	Model *ptemp;
	if(mBaseObject)
	{
		ptemp = mBaseObject->selectShape(shapeId);
		if(ptemp)
			mCurrentObject = ptemp;
	}
}

void CObjectViewer::clearCurrentAction()
{
	if(mCurrentObject)
		mCurrentObject->clearAction();
}

void CObjectViewer::loadCurrentAction(StringTableEntry fileName)
{
	if(mCurrentObject)
    {
        // 下面直接改为平滑过渡切换动作( 更好的改法是在界面上做个勾选框, 在下面对应选择直接切换动作还是过渡切换动作 )
        //mCurrentObject->loadAction(fileName);
        mCurrentObject->transitionAction(fileName);
    }
}

void CObjectViewer::mountShapeToCurrentObject(StringTableEntry modelName,StringTableEntry parentPoint,StringTableEntry childPoint)
{
	if(mCurrentObject)
	{
		mCurrentObject->mountShape(modelName,parentPoint,childPoint);
	}
}

void CObjectViewer::loadBaseShape(StringTableEntry modelName)
{
	clearModel();

	mBaseObject = new Model;
	if ( !mBaseObject->loadShape(modelName) )
    {
        delete mBaseObject;
        mBaseObject = NULL;
    }
	mCurrentObject = mBaseObject;

    if ( mBasePart )
        mBasePart->setMountParent( mCurrentObject );

    for ( int i = 0; i < mLinkParts.size(); i++ )
    {
        if ( mLinkParts[i] )
            mLinkParts[i]->setMountParent( mCurrentObject );
    }

    if ( !gPreviewMission )
        resetCamera();
}

void CObjectViewer::selectNode(StringTableEntry node)
{
	if(mCurrentObject)
	{
		mCurrentObject->selectNode(node);
	}
}

void CObjectViewer::selectMesh(int i)
{
	if(mCurrentObject)
	{
		mCurrentObject->selectMesh(i);
	}
}

StringTableEntry CObjectViewer::getCurrentActionFileName()
{
	if(mCurrentObject)
		return mCurrentObject->getActionFileName();
	return "";
}

StringTableEntry CObjectViewer::getCurrentShapeFileName()
{
	if(mCurrentObject)
		return mCurrentObject->getShapeFileName();
	return "";
}

int CObjectViewer::getCurrentShapeTextureCount()
{
	if(mCurrentObject)
		return mCurrentObject->getTextureCount();
	return 0;
}

StringTableEntry CObjectViewer::getCurrentShapeTextureName(int i)
{
	if(mCurrentObject)
		return mCurrentObject->getTextureName(i);
	return "";
}

int CObjectViewer::getCurrentShapeCollisionCount()
{
	if(mCurrentObject)
		return mCurrentObject->getCollisionCount();
	return 0;
}

int CObjectViewer::getCurrentShapeDetailCount()
{
	if(mCurrentObject)
		return mCurrentObject->getDetailCount();
	return 0;
}

int CObjectViewer::getCurrentShapeMeshCount(int i)
{
	if(mCurrentObject)
		return mCurrentObject->getMeshCount(i);
	return 0;
}

int CObjectViewer::getCurrentShapeNodeCount()
{
	if(mCurrentObject)
		return mCurrentObject->getNodeCount();
	return 0;
}

int CObjectViewer::getCurrentShapeMeshFaceCount(int i)
{
	if(mCurrentObject)
		return mCurrentObject->getMeshFaceCount(i);
	return 0;
}

StringTableEntry CObjectViewer::getCurrentShapeNodeName(int i)
{
	if(mCurrentObject)
		return mCurrentObject->getNodeName(i);
	return "";
}

StringTableEntry CObjectViewer::getCurrentShapeMountNodeName()
{
	if(mCurrentObject)
		return mCurrentObject->getMountNodeName();
	return "";
}

StringTableEntry CObjectViewer::getCurrentShapeParentNodeName()
{
	if(mCurrentObject)
		return mCurrentObject->getParentNodeName();
	return "";
}

void CObjectViewer::LinkCamera()
{
    if(mBaseObject)
    {
        mCameraMatrix.identity();
        char mountNodeName[64];
        dSprintf(mountNodeName, sizeof(mountNodeName), "linkCameraPoint");
        S32 moundNode = mBaseObject->shape->findNode(mountNodeName);
        if(moundNode >= 0)      
        {
            mCameraMatrix = mBaseObject->shapeInstance->getNodeTransforms()[moundNode];      
        }    
        lastRenderTime = Platform::getVirtualMilliseconds();
    }    
    return;
}

void CObjectViewer::resetCamera()
{
    mWorldMatrix.identity();

	Point3F dir(0,-1,0);

	if(mBaseObject)
	{
		Point3F center;
		mBaseObject->shape->bounds.getCenter(&center);
		F32 len = (mBaseObject->shape->bounds.max-mBaseObject->shape->bounds.min).len();
		len *=2.5;

		mCameraMatrix = MathUtils::createOrientFromDir(dir);
		Point3F camera = center - (dir*len);
		mCameraMatrix.setColumn(3,camera);
	}
	//else if(mBasePart)
	//{
	//	Point3F center;
	//	mBasePart->part->getObjBox().getCenter(&center);
	//	F32 len = (mBasePart->part->getObjBox().max-mBasePart->part->getObjBox().min).len();
	//	len *=2.5;

	//	mCameraMatrix = MathUtils::createOrientFromDir(dir);
	//	Point3F camera = center - (dir*len);
	//	mCameraMatrix.setColumn(3,camera);
	//}
	else
	{
		mCameraMatrix = MathUtils::createOrientFromDir(dir);

		Point3F camera = dir*-10;
		mCameraMatrix.setColumn(3,camera);
	}

	lastRenderTime = Platform::getVirtualMilliseconds();
	mWorldMoveZ = 0;
	mWorldMoveX = 0;
	mWorldPatch = 0;
	mWorldRow	= 0;
}

void CObjectViewer::UpdateData()
{
	MatrixF zRot;

	//if(mBaseObject && !mBaseObject->isEmpty())
	{
		Point3F center(0,0,0);
		
		if(mBaseObject && !mBaseObject->isEmpty())
			mBaseObject->shape->bounds.getCenter(&center);

		mWorldMatrix.identity();
		mWorldMatrix.setPosition(center);
		zRot.set(EulerF(mWorldPatch, 0.0f, mWorldRow));
		mWorldMatrix.mul(zRot);
		center.neg();
		mWorldMatrix.mulP(center);

		center.x += mWorldMoveX;
		center.z -= mWorldMoveZ;

		mWorldMatrix.setPosition(center);

		MatrixF idn(true);
		zRot.set(EulerF(mLightRotX,0,mLightRotZ));
		idn.mul(zRot);
		idn.mulV(mLightDir);
		mLightDir.normalize();
		
		if(mFakeSun)
		{
			mFakeSun->mDirection = mLightDir;
			mWorldMatrix.mulV(mFakeSun->mDirection);
			mFakeSun->mDirection.normalize();

			mFakeSun->mAmbient = mAmbientColor;
			mFakeSun->mColor = mLightColor;
		}
	}

	Point3F dir;
	mCameraMatrix.getColumn(1,&dir);
	dir.normalize();
	Point3F camera = mCameraMatrix.getPosition();
	camera +=  dir*mCameraZoom;
	mCameraMatrix.setPosition(camera);
	
	mCameraZoom = 0;
	mLightRotX = 0;
	mLightRotZ = 0;
}

bool CObjectViewer::processCameraQuery(CameraQuery* query)
{
	UpdateData();

	query->farPlane = 2100.0f;
	query->nearPlane = 0.1f;

	query->fov = mDegToRad(30.0f);
	query->cameraMatrix = mCameraMatrix;

	gClientSceneGraph->setLastView2WorldMX(mCameraMatrix);

	return true;
}

void CObjectViewer::setFillMode(int mode)
{
	mFillMode = mode;
}

SceneState * CObjectViewer::createSceneState()
{
	MatrixF modelview(true);
	MatrixF mv;
	Point3F cp;
	modelview = GFX->getWorldMatrix();
	mv = modelview;
	mv.inverse();
	mv.getColumn(3, &cp);
	gRenderInstManager.setCamPos( cp );


	// Set up the base SceneState.
	F32 left, right, top, bottom, nearPlane, farPlane;
	RectI viewport;

	GFX->getFrustum( &left, &right, &bottom, &top, &nearPlane, &farPlane );
	viewport = GFX->getViewport();

	SceneState *pSceneState = new SceneState(NULL,
		0,
		left, right,
		bottom, top,
		nearPlane,
		1000,
		viewport,
		cp,
		modelview,
		1000,
		1000,
		ColorI(0,0,0),
		0,
		NULL,
		1000);
	pSceneState->enableTerrainOverride();
	return pSceneState;
}
void CObjectViewer::renderWorld(const RectI &updateRect)
{
	if(g_MultThreadWorkMgr)
		g_MultThreadWorkMgr->enable(false);

	SceneState *pSceneState = createSceneState();

	GFX->pushWorldMatrix();
	GFX->multWorld(mWorldMatrix);

	S32 time = Platform::getVirtualMilliseconds();
	S32 dt = time - lastRenderTime;
	lastRenderTime = time;

	gClientSceneGraph->buildFogTexture(NULL);

#ifdef STATEBLOCK
	AssertFatal(mSetWorldSB, "CObjectViewer::renderWorld -- mSetWorldSB cannot be NULL.");
	mSetWorldSB->apply();
#else
	GFX->setZEnable(true);
	GFX->setZWriteEnable(true);
	GFX->setZFunc(GFXCmpLessEqual);
	GFX->setCullMode(GFXCullNone);
#endif

	renderArrows(0.5);

	MatrixF idt(true);
	DrawAxis(idt,Point3F(0,0,0),100);

	if(mlightEnable)
	{
		LightManager* lm = gClientSceneGraph->getLightManager();
		lm->setSpecialLight(LightManager::slSunLightType, mFakeSun);
	}

	if (mBaseObject && mBaseObject->shapeInstance)
	{

#ifdef STATEBLOCK
		if (mFillMode==1)
		{
			AssertFatal(mFillWireSB, "CObjectViewer::renderWorld -- mFillWireSB cannot be NULL.");
			mFillWireSB->apply();
		}
#else
		if(mFillMode==1)
			GFX->setFillMode((GFXFillMode)GFXFillWireframe);
#endif

		if(mlightEnable)
		{
			if(CObjectViewer::mShowLightDir)
			{
				Point3F center;
				mBaseObject->shape->bounds.getCenter(&center);
				F32 len = (mBaseObject->shape->bounds.min-center).len();
				VectorF line = mLightDir;
				line *= len*2;
				
				DrawLine(center+line,center-line,ColorI(255,255,0),true);
			}
		}

		MatrixF meshCamTrans(true);
		TSMesh::setCamTrans(meshCamTrans);
		TSMesh::setSceneState( pSceneState );
		TSMesh::setObject( NULL );

		GFX->pushWorldMatrix();

		mBaseObject->render(dt);
		GFX->popWorldMatrix();
#ifdef STATEBLOCK
		AssertFatal(mFillSolidSB, "CObjectViewer::renderWorld -- mFillSolidSB cannot be NULL.");
		mFillSolidSB->apply();
#else
		GFX->setFillMode((GFXFillMode)GFXFillSolid);
#endif

	}

    if ( mBasePart )
    {
        mBasePart->render(this,pSceneState);
        if ( mBasePart->reset )
        {
            mBasePart->setTime(0.0f);
            mBasePart->reset = false;
        }
    }
    for ( int i = 0; i < mLinkParts.size(); i++ )
    {
        mLinkParts[i]->render(this,pSceneState);
        if ( mLinkParts[i]->reset )
        {
            mLinkParts[i]->setTime(0.0f);
            mLinkParts[i]->reset = false;
        }
    }
    animBgColor(dt);

	//if(gProjectDecalManager)
	//{
	//	gProjectDecalManager->doPrepRender(pSceneState,lastRenderTime,0,false);
	//}

	//for(int i=0;i<mParticleEmitter.size();i++)
	//{
	//	mParticleEmitter[i]->doPrepRender(mCameraMatrix.getPosition());
	//}

	//if(pTerrain)
	//{
	//	//pTerrain->doPrepRenderImage(pSceneState,lastRenderTime,0,false);
	//}
	
	gRenderInstManager.sort();
	gRenderInstManager.render();
	gRenderInstManager.clear();

	SAFE_DELETE(pSceneState);

	GFX->popWorldMatrix();
}

void CObjectViewer::beginItemSearch()
{
	for(int i=0;i<gCurrentSearchObject.size();i++)
		delete [] gCurrentSearchObject[i];
	gCurrentSearchObject.clear();
	gCurrentSearchStep = 0;
	
	if(mBaseObject)
		mBaseObject->fillItemMsg(NULL);
}

void CObjectViewer::baseNodeSearch()
{
	for(int i=0;i<gCurrentSearchObject.size();i++)
		delete [] gCurrentSearchObject[i];
	gCurrentSearchObject.clear();
	gCurrentSearchStep = 0;

	if(mBaseObject)
		mBaseObject->fillNodeMsg();
}

void CObjectViewer::currentNodeSearch()
{
	for(int i=0;i<gCurrentSearchObject.size();i++)
		delete [] gCurrentSearchObject[i];
	gCurrentSearchObject.clear();
	gCurrentSearchStep = 0;

	if(mCurrentObject)
		mCurrentObject->fillNodeMsg();
}

void CObjectViewer::beginMeshSearch()
{
	for(int i=0;i<gCurrentSearchObject.size();i++)
		delete [] gCurrentSearchObject[i];
	gCurrentSearchObject.clear();
	gCurrentSearchStep = 0;

	if(mCurrentObject)
		mCurrentObject->fillMeshMsg();
}

StringTableEntry CObjectViewer::getNext()
{
	if(gCurrentSearchStep<gCurrentSearchObject.size())
	{
		StringTableEntry pChar = gCurrentSearchObject[gCurrentSearchStep++];
		return pChar;
	}
	else
		return "";
}

void CObjectViewer::rebuildMaterials()
{
	if (mBaseObject)
		mBaseObject->rebuildMaterials();
}

void CObjectViewer::setLightColor(F32 r,F32 g,F32 b)
{
	mLightColor.set(r,g,b,1);
}

void CObjectViewer::setAmbientColor(F32 r,F32 g,F32 b)
{
	mAmbientColor.set(r,g,b,1);
}

const char *CObjectViewer::getLightColor()
{
	static char msg[256];
	dSprintf(msg,256,"%.2f %.2f %.2f",mLightColor.red,mLightColor.green,mLightColor.blue);
	return msg;
}

const char *CObjectViewer::getAmbientColor()
{
	static char msg[256];
	dSprintf(msg,256,"%.2f %.2f %.2f",mAmbientColor.red,mAmbientColor.green,mAmbientColor.blue);
	return msg;
}

const char *CObjectViewer::getLightDir()
{
	static char msg[256];
	dSprintf(msg,256,"%.2f %.2f %.2f",mLightDir.x,mLightDir.y,mLightDir.z);
	return msg;
}

void CObjectViewer::setTimeScale(F32 scale)
{
	mAnimationScale = scale;
	if (mBaseObject)
		mBaseObject->setTimeScale(scale);

    if ( mBasePart )
        mBasePart->setTimeScale(scale);
    for ( int i = 0; i < mLinkParts.size(); i++ )
        mLinkParts[i]->setTimeScale(scale);
}

F32 CObjectViewer::getTimeScale()
{
	return mAnimationScale;
}


void CObjectViewer::loadBasePart(StringTableEntry filename)
{
    char _name[MAX_PATH];    
    dStrcpy( _name, MAX_PATH, filename );
    char* _pname = _name;
    while (*_pname){ if (*_pname == '\\') *_pname = '/'; _pname++; }

    char *baseName = dStrrchr(_name, '/');
    if(!baseName)
        baseName = _name;
    else
        baseName++;

    char *ext = dStrrchr(baseName, '.');
    if(ext)
        *ext = 0;

    deleteCurrentShape();

    for ( int i = 0; i < mLinkParts.size(); i++ )
        delete mLinkParts[i];
    mLinkParts.clear();

    if ( mBasePart )
    {
        if ( dStricmp( mBasePart->name, baseName ) != 0 )
        {
            delete mBasePart;
            mBasePart = new Part();
            if ( !mBasePart->loadPart(baseName) )
            {
                delete mBasePart;
                mBasePart = NULL;
            }
        }
        else
        {
            delete mBasePart;
            mBasePart = NULL;
        }        
    }
    else
    {
        mBasePart = new Part();
        if ( !mBasePart->loadPart(baseName) )
        {
            delete mBasePart;
            mBasePart = NULL;
        }
    }

    if ( !gPreviewMission )
        resetCamera();
}

void CObjectViewer::mountPartToCurrentObject(StringTableEntry filename)
{
    char _name[MAX_PATH];    
    dStrcpy( _name, MAX_PATH, filename );
    char* _pname = _name;
    while (*_pname){ if (*_pname == '\\') *_pname = '/'; _pname++; }

    char *baseName = dStrrchr(_name, '/');
    if(!baseName)
        baseName = _name;
    else
        baseName++;

    char *ext = dStrrchr(baseName, '.');
    if(ext)
        *ext = 0;

    if ( mBasePart )
        delete mBasePart;
    mBasePart = NULL;
    
    Part* linkPart = NULL;
    for ( int i = 0; i < mLinkParts.size(); i++ )
    {
        if ( dStricmp( mLinkParts[i]->name, baseName ) == 0  )
        {
            linkPart = mLinkParts[i];
            mLinkParts.erase(i);
            break;
        }
    }

    if ( linkPart )
        delete linkPart;
    else
    {
        linkPart = new Part();
        if ( !linkPart->loadPart(baseName) )
        {
            delete linkPart;
            return;
        }
        mLinkParts.push_back(linkPart);

        int len = dStrlen( baseName );
        if ( len > 8)
		{
			int _Count = 0;
			while(_Count<4 && *baseName)
			{
				if(*baseName == '_')
					_Count++;

				baseName++;
			}

			if(_Count==4)
			{
				_pname = baseName;
				while(*_pname)
				{
					if(*_pname == '_')
						*_pname = ' ';
				}
			}
		}

		linkPart->setMountParent(mCurrentObject, baseName);
    }    
}

void CObjectViewer::mountPartToCurrentObjectByLink(StringTableEntry filename, StringTableEntry linkPoint )
{
	char _name[MAX_PATH];    
	dStrcpy( _name, MAX_PATH, filename );
	char* _pname = _name;
	while (*_pname){ if (*_pname == '\\') *_pname = '/'; _pname++; }

	char *baseName = dStrrchr(_name, '/');
	if(!baseName)
		baseName = _name;
	else
		baseName++;

	char *ext = dStrrchr(baseName, '.');
	if(ext)
		*ext = 0;

	if ( mBasePart )
		delete mBasePart;
	mBasePart = NULL;

	Part* linkPart = NULL;
	for ( int i = 0; i < mLinkParts.size(); i++ )
	{
		if ( dStricmp( mLinkParts[i]->name, baseName ) == 0  )
		{
			linkPart = mLinkParts[i];
			mLinkParts.erase(i);
			break;
		}
	}

	if ( linkPart )
		delete linkPart;
	else
	{
		linkPart = new Part();
		if ( !linkPart->loadPart(baseName) )
		{
			delete linkPart;
			return;
		}
		mLinkParts.push_back(linkPart);
		linkPart->setMountParent(mCurrentObject, linkPoint);
	}
}

void CObjectViewer::clearAllParts()
{    
    if ( mBasePart )
    {
        char dbName[512];
        dStrcpy( dbName, 512, mBasePart->name );    
        dStrcat( dbName, 512, "_Node" );
        ParticleEmitterNodeData* dataBlock = NULL;
        Sim::findObject( dbName, dataBlock );
        if ( dataBlock )
            dataBlock->deleteObject();

        delete mBasePart;
        mBasePart = NULL;
    }

    for ( int i = 0; i < mLinkParts.size(); i++ )
    {
        char dbName[512];
        dStrcpy( dbName, 512, mLinkParts[i]->name );    
        dStrcat( dbName, 512, "_Node" );
        ParticleEmitterNodeData* dataBlock = NULL;
        Sim::findObject( dbName, dataBlock );
        if ( dataBlock )
            dataBlock->deleteObject();

        delete mLinkParts[i];
    }
    mLinkParts.clear();    
}

void CObjectViewer::animBgColor(S32 dt)
{
    static GuiButtonCtrl* sBtn = NULL;
    if ( !sBtn )
        Sim::findObject( "AnimBgColor", sBtn );
    if ( !sBtn || !sBtn->IsStateOn() )
        return;

    static F32 sElapsed = 0.0f;    
    sElapsed += dt * 0.001f;
    S32 changed = sElapsed * 30.0f;    
    if ( changed )
    {        
        static Point3I dir( 1, 1, 1 );

        Point3I clr;
        clr.x = gCanvasClearColor.red + changed * dir.x;
        clr.y = gCanvasClearColor.green + changed * dir.y;
        clr.z = gCanvasClearColor.blue + changed * dir.z;

        if ( clr.x > 255 ){ clr.x = 255; dir.x = -1; }
        else if ( clr.x < 0 ){ clr.x = 0; dir.x = 1; }

        if ( clr.y > 255 ){ clr.y = 255; dir.y = -1; }
        else if ( clr.y < 0 ){ clr.y = 0; dir.y = 1; }

        if ( clr.z > 255 ){ clr.z = 255; dir.z = -1; }
        else if ( clr.z < 0 ){ clr.z = 0; dir.z = 1; }

        gCanvasClearColor.set( clr.x, clr.y, clr.z );

        sElapsed = 0.0f;
    }
}


void CObjectViewer::resetStateBlock()
{
	//mSetCollSB
	GFX->beginStateBlock();
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulateInvColorAddAlpha);
	GFX->setRenderState(GFXRSFillMode, GFXFillWireframe);
	GFX->endStateBlock(mSetCollSB);

	//mColorOpStoreSB
	GFX->beginStateBlock();
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPDisable);	
	GFX->endStateBlock(mColorOpStoreSB);

	//mColorOpSB
	GFX->beginStateBlock();
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPDisable);	
	GFX->endStateBlock(mColorOpSB);

	//mFillSolidSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSFillMode, GFXFillSolid);
	GFX->endStateBlock(mFillSolidSB);

	//mFillWireSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSFillMode, GFXFillWireframe);
	GFX->endStateBlock(mFillWireSB);

	//mSetWorldSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->setRenderState(GFXRSZFunc, GFXCmpLessEqual);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->endStateBlock(mSetWorldSB);
}


void CObjectViewer::releaseStateBlock()
{
	if (mSetCollSB)
	{
		mSetCollSB->release();
	}

	if (mColorOpStoreSB)
	{
		mColorOpStoreSB->release();
	}

	if (mColorOpSB)
	{
		mColorOpSB->release();
	}

	if (mFillSolidSB)
	{
		mFillSolidSB->release();
	}

	if (mFillWireSB)
	{
		mFillWireSB->release();
	}

	if (mSetWorldSB)
	{
		mSetWorldSB->release();
	}
}

void CObjectViewer::init()
{
	if (mSetCollSB == NULL)
	{
		mSetCollSB = new GFXD3D9StateBlock;
		mSetCollSB->registerResourceWithDevice(GFX);
		mSetCollSB->mZombify = &releaseStateBlock;
		mSetCollSB->mResurrect = &resetStateBlock;

		mColorOpStoreSB = new GFXD3D9StateBlock;
		mColorOpSB = new GFXD3D9StateBlock;
		mFillSolidSB = new GFXD3D9StateBlock;
		mFillWireSB = new GFXD3D9StateBlock;
		mSetWorldSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}

void CObjectViewer::shutdown()
{
	SAFE_DELETE(mSetCollSB);
	SAFE_DELETE(mColorOpStoreSB);
	SAFE_DELETE(mColorOpSB);
	SAFE_DELETE(mFillSolidSB);
	SAFE_DELETE(mFillWireSB);
	SAFE_DELETE(mSetWorldSB);
}

//-----------------------------------------------------------------------------
// Console stuff (CObjectViewer)
//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(CObjectViewer);

void CObjectViewer::clearParts()
{
	SAFE_DELETE(mBasePart);

	Part* linkPart = NULL;
	for ( int i = 0; i < mLinkParts.size(); i++ )
	{
		SAFE_DELETE(mLinkParts[i]);
	}
	mLinkParts.clear();
}
ConsoleMethod(CObjectViewer, loadBaseShape, void, 3, 3, "(string shapeFile)")
{
	object->loadBaseShape(argv[2]);
}

ConsoleMethod(CObjectViewer, selectCurrentShape, void, 3, 3, "(string shapeStr)")
{
	const char * pChar = trim(argv[2],'(',')');
	if(pChar)
		object->selectCurrentShape(dAtoi(pChar));
}

ConsoleMethod(CObjectViewer, loadCurrentAction, void, 3, 3, "(string ActionFile)")
{
	object->loadCurrentAction(argv[2]);
}

ConsoleMethod(CObjectViewer, getCurrentActionFileName, const char *, 2, 2, "()")
{
	return object->getCurrentActionFileName();
}

ConsoleMethod(CObjectViewer, getCurrentShapeFileName, const char *, 2, 2, "()")
{
	return object->getCurrentShapeFileName();
}


ConsoleMethod(CObjectViewer, clearCurrentAction, void, 2, 2, "()")
{
	return object->clearCurrentAction();
}

ConsoleMethod(CObjectViewer, mountShapeToCurrentObject, void, 5, 5,  "(string shapeName,string parentPoint,string selfPoint)")
{
	object->mountShapeToCurrentObject(argv[2],argv[3],argv[4]);
}

ConsoleMethod(CObjectViewer, deleteCurrentShape, void, 2, 2, "deleteCurrentShape()")
{
	object->deleteCurrentShape();
}

ConsoleMethod(CObjectViewer, resetCamera, void, 2, 2, "resetCamera()")
{
	object->resetCamera();
}

ConsoleMethod(CObjectViewer, LinkCamera, void, 2, 2, "LinkCamera()")
{
    object->resetCamera();
    object->LinkCamera();
}

ConsoleMethod(CObjectViewer, selectNode, void, 3, 3, "selectNode(string nodeName)")
{
	const char * pChar = trim(argv[2],'[',']');
	if(pChar)
		object->selectNode(pChar);
}

ConsoleMethod(CObjectViewer, getCurrentShapeNodeCount, S32, 2, 2, "getCurrentShapeNodeCount()")
{
	return object->getCurrentShapeNodeCount();
}

ConsoleMethod(CObjectViewer, getCurrentShapeNodeName, const char *, 3, 3, "getCurrentShapeNodeName(int i)")
{
	return object->getCurrentShapeNodeName(dAtoi(argv[2]));
}

ConsoleMethod(CObjectViewer, getCurrentShapeMountNodeName, const char *, 2, 2, "getCurrentShapeMountNodeName()")
{
	return object->getCurrentShapeMountNodeName();
}

ConsoleMethod(CObjectViewer, getCurrentShapeParentNodeName, const char *, 2, 2, "getCurrentShapeParentNodeName()")
{
	return object->getCurrentShapeParentNodeName();
}

ConsoleMethod(CObjectViewer, getCurrentShapeCollisionCount, S32, 2, 2, "getCurrentShapeCollisionCount()")
{
	return object->getCurrentShapeCollisionCount();
}

ConsoleMethod(CObjectViewer, getCurrentShapeDetailCount, S32, 2, 2, "getCurrentShapeDetailCount()")
{
	return object->getCurrentShapeDetailCount();
}

ConsoleMethod(CObjectViewer, getCurrentShapeMeshCount, S32, 3, 3, "getCurrentShapeMeshCount(int i)")
{
	return object->getCurrentShapeMeshCount(dAtoi(argv[2]));
}

ConsoleMethod(CObjectViewer, getCurrentShapeMeshFaceCount, S32, 3, 3, "getCurrentShapeMeshFaceCount(int i)")
{
	return object->getCurrentShapeMeshFaceCount(dAtoi(argv[2]));
}

ConsoleMethod(CObjectViewer, getCurrentShapeTextureCount, S32, 2, 2, "getCurrentShapeTextureCount()")
{
	return object->getCurrentShapeTextureCount();
}

ConsoleMethod(CObjectViewer, getCurrentShapeTextureName, const char*, 3, 3, "getCurrentShapeTextureName(int i)")
{
	return object->getCurrentShapeTextureName(dAtoi(argv[2]));
}


ConsoleMethod(CObjectViewer, setFillMode, void, 3, 3, "setFillMode(int mode)")
{
	object->setFillMode(dAtoi(argv[2]));
}

ConsoleMethod(CObjectViewer, showBounds, void, 3, 3, "showBounds(bool flag)")
{
	CObjectViewer::mShowBounds = dAtoi(argv[2])!=0;
}

ConsoleMethod(CObjectViewer, showCollection, void, 3, 3, "showCollection(bool flag)")
{
	CObjectViewer::mShowCollection = dAtoi(argv[2])!=0;
}

ConsoleMethod(CObjectViewer, beginItemSearch, void , 2, 2, "beginItemSearch()")
{
	return object->beginItemSearch();
}

ConsoleMethod(CObjectViewer, getNext, const char* , 2, 2, "getNext()")
{
	return object->getNext();
}

ConsoleMethod(CObjectViewer, beginMeshSearch, void , 2, 2, "beginMeshSearch()")
{
	return object->beginMeshSearch();
}

ConsoleMethod(CObjectViewer, baseNodeSearch, void , 2, 2, "baseNodeSearch()")
{
	return object->baseNodeSearch();
}

ConsoleMethod(CObjectViewer, currentNodeSearch, void , 2, 2, "currentNodeSearch()")
{
	return object->currentNodeSearch();
}

ConsoleMethod(CObjectViewer, playAnimation, void , 3, 3, "playAnimation(bool flag)")
{
	CObjectViewer::mPlayAnimation = dAtoi(argv[2])!=0;
}

ConsoleMethod(CObjectViewer, showLightDir, void , 3, 3, "showLightDir(bool flag)")
{
	CObjectViewer::mShowLightDir = dAtoi(argv[2])!=0;
}

ConsoleMethod(CObjectViewer, lightEnable, void , 3, 3, "lightEnable(bool flag)")
{
	CObjectViewer::mlightEnable = dAtoi(argv[2])!=0;
	object->rebuildMaterials();
}

ConsoleMethod(CObjectViewer, setLightColor, void , 5, 5, "setLightColor(F32 Red,F32 Gree,F32 Blue)")
{
	object->setLightColor(dAtof(argv[2]),dAtof(argv[3]),dAtof(argv[4]));
}

ConsoleMethod(CObjectViewer, setAmbientColor, void , 5, 5, "setAmbientColor(F32 Red,F32 Gree,F32 Blue)")
{
	object->setAmbientColor(dAtof(argv[2]),dAtof(argv[3]),dAtof(argv[4]));
}

ConsoleMethod(CObjectViewer, getLightColor, const char* , 2, 2, "getLightColor()")
{
	return object->getLightColor();
}

ConsoleMethod(CObjectViewer, getAmbientColor, const char* , 2, 2, "getAmbientColor()")
{
	return object->getAmbientColor();
}

ConsoleMethod(CObjectViewer, getLightDir, const char* , 2, 2, "getLightDir()")
{
	return object->getLightDir();
}

ConsoleMethod(CObjectViewer, setTimeScale, void , 3, 3, "setTimeScale(F32 scale)")
{
	object->setTimeScale(dAtof(argv[2]));
}

ConsoleMethod(CObjectViewer, getTimeScale, F32  , 2, 2, "setTimeScale()")
{
	return object->getTimeScale();
}

ConsoleMethod(CObjectViewer, loopAnimation, void , 3, 3, "loopAnimation(bool flag)")
{
	CObjectViewer::mAnimationLoop = dAtoi(argv[2])!=0;
}

ConsoleMethod(CObjectViewer, onQuit, void  , 2, 2, "onQuit()")
{
	object->clearModel();
	object->clearOther();
}


ConsoleMethod(CObjectViewer, loadBasePart, void, 3, 3, "(string filename)")
{
    object->loadBasePart(argv[2]);
}

ConsoleMethod(CObjectViewer, mountPartToCurrentObject, void, 3, 3,  "(string filename)")
{
    object->mountPartToCurrentObject(argv[2]);
}

ConsoleMethod(CObjectViewer, mountPartToCurrentObjectByLink, void, 4, 4,  "(string filename, string linkPoint)")
{
	object->mountPartToCurrentObjectByLink(argv[2], argv[3]);
}

ConsoleMethod(CObjectViewer, clearAllParts, void, 2, 2,  "()")
{
    object->clearAllParts();
}

ConsoleMethod(CObjectViewer, clearParts, void, 2, 2,  "()")
{
	object->clearParts();
}

ConsoleFunction(setBgColor, void , 4, 4, "setBgColor(F32 Red,F32 Gree,F32 Blue)")
{
    gCanvasClearColor = ColorF(dAtof(argv[1]),dAtof(argv[2]),dAtof(argv[3]));
}
