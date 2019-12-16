//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _TSLOGICMARKNODE_H_
#define _TSLOGICMARKNODE_H_

#ifndef _RENDERABLESCENEOBJECT_H
#include "sceneGraph/RenderableSceneObject.h"
#endif
#ifndef _RESMANAGER_H_
#include "core/resManager.h"
#endif

//逻辑标记点的类型(用途)
enum LogicMarkNodeType{
	LMNT_STALL,//用于摆摊
	
	COUNT
};
//Ray: 作为标记用的对象，用于逻辑处理中标记范围。在编辑器中渲染矩形方便调节，在客户端不可见

//--------------------------------------------------------------------------
class TSLogicMarkNode : public RenderableSceneObject
{
   typedef RenderableSceneObject Parent;
   LogicMarkNodeType mType;
protected:
   bool onAdd();
   void onRemove();

public:
  void setTransform     ( const MatrixF &mat);

   // Rendering
protected:
   bool prepRenderImage  ( SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState=false);
	void renderObject( SceneState *state, RenderInst *ri );

public:
   TSLogicMarkNode();
   ~TSLogicMarkNode();

	U64  packUpdate  (NetConnection *conn, U64 mask, BitStream* stream);
	void unpackUpdate(NetConnection *conn,           BitStream* stream);
	static void initPersistFields();

	void OnSerialize( CTinyStream& stream );
	void OnUnserialize( CTinyStream& stream );

	void setType(LogicMarkNodeType type);
	LogicMarkNodeType getType() const{return mType;}
   DECLARE_CONOBJECT(TSLogicMarkNode);
};


//这个类用来管理本地图上所有TSLogicMarkNode对象。 
//这样可以让TSLogicMarkNode在客户端和服务器不用加入到场景中，减少其他逻辑在findobject时的处理量
class LogicMarkNodeManager
{
	stdext::hash_map<U8, stdext::hash_set<TSLogicMarkNode*>> mMap;//<标记点类型，标记点集合>
public:
	static Point3F base;//getClosestMarkNode函数传进来的参数，absLess函数中使用
	bool getClosestMarkNode(Point3F pt, LogicMarkNodeType type, Point3F& findPos);  //这个函数对于给定的一个世界坐标点，返回一个最靠近该点的TSLogicMarkNode的中心点世界坐标
	void addNode(TSLogicMarkNode * node);
	void removeNode(TSLogicMarkNode * node);
};
extern LogicMarkNodeManager* gClientLogicMarkNodeManager;
extern LogicMarkNodeManager* gSeverLogicMarkNodeManager;
#endif // _TSLOGICMARKNODE_H_

