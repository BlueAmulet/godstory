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

//�߼���ǵ������(��;)
enum LogicMarkNodeType{
	LMNT_STALL,//���ڰ�̯
	
	COUNT
};
//Ray: ��Ϊ����õĶ��������߼������б�Ƿ�Χ���ڱ༭������Ⱦ���η�����ڣ��ڿͻ��˲��ɼ�

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


//���������������ͼ������TSLogicMarkNode���� 
//����������TSLogicMarkNode�ڿͻ��˺ͷ��������ü��뵽�����У����������߼���findobjectʱ�Ĵ�����
class LogicMarkNodeManager
{
	stdext::hash_map<U8, stdext::hash_set<TSLogicMarkNode*>> mMap;//<��ǵ����ͣ���ǵ㼯��>
public:
	static Point3F base;//getClosestMarkNode�����������Ĳ�����absLess������ʹ��
	bool getClosestMarkNode(Point3F pt, LogicMarkNodeType type, Point3F& findPos);  //����������ڸ�����һ����������㣬����һ������õ��TSLogicMarkNode�����ĵ���������
	void addNode(TSLogicMarkNode * node);
	void removeNode(TSLogicMarkNode * node);
};
extern LogicMarkNodeManager* gClientLogicMarkNodeManager;
extern LogicMarkNodeManager* gSeverLogicMarkNodeManager;
#endif // _TSLOGICMARKNODE_H_

