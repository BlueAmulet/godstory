//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "ts/tsLogicMarkNode.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/sceneGraph.h"
#include "math/mathIO.h"
#include "console/consoleTypes.h"
#include "gfx/gfxDevice.h"
#include "gfx/gfxDrawUtil.h"
#include "renderInstance/renderInstMgr.h"
#include "core/bitStream.h"
#include <algorithm>
IMPLEMENT_CO_NETOBJECT_V1(TSLogicMarkNode);

DECLARE_SERIALIBLE(TSLogicMarkNode);

//--------------------------------------------------------------------------
TSLogicMarkNode::TSLogicMarkNode()
{
   mNetFlags.set(Ghostable | ScopeAlways);

   mTypeMask |= StaticObjectType | StaticRenderedObjectType | MarkerObjectType;
	mType = LMNT_STALL;
}

TSLogicMarkNode::~TSLogicMarkNode()
{
}

//--------------------------------------------------------------------------
bool TSLogicMarkNode::onAdd()
{
   PROFILE_SCOPE(TSLogicMarkNode_onAdd);
   if(!Parent::onAdd())
      return false;

   //mObjBox = mShape->bounds;
	mObjBox.min.set(-1, -1, -1);
	mObjBox.max.set( 1,  1,  1);
	mWorldBox.min.set(-1, -1, -1);
	mWorldBox.max.set( 1,  1,  1);
   resetWorldBox();
   setRenderTransform(mObjToWorld);
#ifdef NTJ_EDITOR
   //编辑器加入场景用于显示
   addToScene();
#endif

   //加入标记对象管理器中
	if (isClientObject())
	{
		gClientLogicMarkNodeManager->addNode(this);
	} 
	else
	{
		gSeverLogicMarkNodeManager->addNode(this);
	}
   return true;
}

void TSLogicMarkNode::onRemove()
{
#ifdef NTJ_EDITOR
   removeFromScene();
#endif

	if (isClientObject())
	{
		gClientLogicMarkNodeManager->removeNode(this);
	} 
	else
	{
		gSeverLogicMarkNodeManager->removeNode(this);
	}
   Parent::onRemove();
}

//--------------------------------------------------------------------------
bool TSLogicMarkNode::prepRenderImage(SceneState* state, const U32 stateKey,
                                       const U32 /*startZone*/, const bool /*modifyBaseState*/)
{
   if (isLastState(state, stateKey))
      return false;
   setLastState(state, stateKey);

   if(gClientSceneGraph->isReflectPass())
	   return false;

#ifdef NTJ_EDITOR
   //编辑器中需要渲染
	//if (state->isObjectRendered(this))
	{
		RenderInst *ri = gRenderInstManager.allocInst();
		ri->obj = this;
		ri->state = state;
		ri->type = RenderInstManager::RIT_Decal;//暂时用这个类型
		gRenderInstManager.addInst(ri);
		return true;
	}
#endif

   return false;
}


void TSLogicMarkNode::setTransform(const MatrixF & mat)
{
   Parent::setTransform(mat);
   setRenderTransform(mat);
}

void TSLogicMarkNode::renderObject( SceneState *state, RenderInst *ri )
{
   MatrixF proj = GFX->getProjectionMatrix();
   RectI viewport = GFX->getViewport();
   MatrixF world = GFX->getWorldMatrix();
   GFX->pushWorldMatrix();

	Box3F box = mRenderWorldBox;
	Point3F size = (box.max - box.min) * 0.5;
	Point3F center = (box.min + box.max) * 0.5;
	GFX->getDrawUtil()->drawWireCube(size, center, ColorI(0, 0, 255));
   GFX->popWorldMatrix();
   GFX->setProjectionMatrix( proj );
   GFX->setViewport( viewport );

}

U64 TSLogicMarkNode::packUpdate( NetConnection *conn, U64 mask, BitStream* stream )
{
	U64 retMask = Parent::packUpdate(conn, mask, stream);

	// Note that we don't really care about efficiency here, since this is an
	//  edit-only ghost...
	stream->writeAffineTransform(mObjToWorld);
	mathWrite(*stream, mObjScale);
	stream->writeInt(mType, 8);
	return retMask;
}

void TSLogicMarkNode::unpackUpdate( NetConnection *conn, BitStream* stream )
{
	Parent::unpackUpdate(conn, stream);
	MatrixF temp;
	Point3F tempScale;
	// Transform
	stream->readAffineTransform(&temp);
	mathRead(*stream, &tempScale);
	LogicMarkNodeType type = (LogicMarkNodeType)stream->readInt(8);
	setScale(tempScale);
	setTransform(temp);
	setType(type);
}

void TSLogicMarkNode::initPersistFields()
{
	Parent::initPersistFields();
	addGroup("misc"); // MM: Added group header.
	addField("type", TypeS8 , Offset(mType, TSLogicMarkNode));
	endGroup("misc"); // MM: Added group footer.
}

void TSLogicMarkNode::setType( LogicMarkNodeType type )
{
	if (isClientObject())
	{
		gClientLogicMarkNodeManager->removeNode(this);
		mType = type;
		gClientLogicMarkNodeManager->addNode(this);
	}
	else
	{
		gSeverLogicMarkNodeManager->removeNode(this);
		mType = type;
		gSeverLogicMarkNodeManager->addNode(this);
	}
}

void TSLogicMarkNode::OnSerialize( CTinyStream& stream )
{
	__super::OnSerialize( stream );
	stream << mType;
}

void TSLogicMarkNode::OnUnserialize( CTinyStream& stream )
{
	__super::OnUnserialize( stream );
	stream >> mType;
}

//---------------------------------------------------------------------------
//LogicMarkNodeManager
//
//---------------------------------------------------------------------------

LogicMarkNodeManager* gClientLogicMarkNodeManager = NULL;
LogicMarkNodeManager* gSeverLogicMarkNodeManager = NULL;

Point3F LogicMarkNodeManager::base;

bool absLess(SimObject* elem1, SimObject* elem2)
{
#ifdef DEBUG_ABSLESS
	F32 len = (LogicMarkNodeManager::base-((TSLogicMarkNode*)elem1)->getPosition()).len();
	F32 len2 = (LogicMarkNodeManager::base-((TSLogicMarkNode*)elem2)->getPosition()).len();
	Con::printf("len %f < %f len2", len, len2);
#define pos LogicMarkNodeManager::base
#define posi ((TSLogicMarkNode*)elem1)->getPosition()
	Con::printf("pos(%f, %f, %f)-->posi(%f, %f, %f", pos.x, pos.y, pos.z, posi.x, posi.y, posi.z);
#undef posi
#define posi ((TSLogicMarkNode*)elem2)->getPosition()
	Con::printf("pos(%f, %f, %f)-->posi(%f, %f, %f", pos.x, pos.y, pos.z, posi.x, posi.y, posi.z);
#undef pos
#undef posi
	return len < len2;
#endif
	return (LogicMarkNodeManager::base-((TSLogicMarkNode*)elem1)->getPosition()).lenSquared() < (LogicMarkNodeManager::base-((TSLogicMarkNode*)elem2)->getPosition()).lenSquared();
}

bool LogicMarkNodeManager::getClosestMarkNode(Point3F pt, LogicMarkNodeType type, Point3F& findPos)
{
	//AssertFatal(mMap.size(), "LogicMarkNodeManager::getClosestMarkNode--mMap为空。");
	if(mMap.size() == 0)
		return false;
	stdext::hash_set<TSLogicMarkNode*> &logicSet = mMap[type];
	AssertFatal(logicSet.size(), "LogicMarkNodeManager::getClosestMarkNode--logicSet为空。");

	base = pt;//absLess中用
	findPos = pt;
	stdext::hash_set<TSLogicMarkNode*>::iterator it;
	it = std::min_element(logicSet.begin(), logicSet.end(), absLess);
	if (it != logicSet.end())
	{
		findPos = ((TSLogicMarkNode*)*it)->getPosition();
		return true;
	}
	return false;
}

void LogicMarkNodeManager::addNode( TSLogicMarkNode * node)
{
	AssertFatal(node!=NULL, "LogicMarkNodeManager::addNode--node为空。");
	mMap[node->getType()].insert(node);
}

void LogicMarkNodeManager::removeNode( TSLogicMarkNode * node )
{
	AssertFatal(node!=NULL, "LogicMarkNodeManager::removeNode--node为空。");
	mMap[node->getType()].erase(node);
}