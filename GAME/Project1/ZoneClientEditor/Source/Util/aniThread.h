#ifndef ANI_THREAD_H
#define ANI_THREAD_H

#include "core/tVector.h"

class TSShapeInstance;
class SimObject;
class TSThread;
struct stThreadParam;

enum callType
{
	animate = 0,
	//setNodeAnimationState,
	//transitionToSequence,
	//setSequence,
};

class CMultThreadWorkMgr
{
	Vector<TSShapeInstance *> mDeleteingShapeInstance;
	Vector<stThreadParam *> mAnimateShapeInstance;
	Vector<SimObject*> mDeleteingObjects;
	Vector<stThreadParam *> mProcessList;

	bool					mZombifyFlag;
	bool					mEnable;
	bool					mBegin;
	bool					mStepOne;

public:
	static void init();
	static void shutdown();

	static void aniCallback(void* param);

	CMultThreadWorkMgr();
	~CMultThreadWorkMgr();

	void zombify(bool flag) {mZombifyFlag = flag;}
	bool isZombify(){return mZombifyFlag;}

	void enable(bool flag) {mEnable = flag&&!mZombifyFlag;}
	bool isEnable() {return mEnable;}

	void setBegin(bool flag) {mBegin=flag;}
	bool isBegin() {return mBegin;}

	void addInDeleteingShapeInsList(TSShapeInstance *obj){mDeleteingShapeInstance.push_back(obj);}
	void addInDeleteingObjectInsList(SimObject *obj){mDeleteingObjects.push_back(obj);}
	
	void addInAnimateShapeInsList(TSShapeInstance *obj);																//Animate
	void addInProcessList(stThreadParam *obj);

	void clear();
	void beginProcess();
	void tryProcess(bool bEnd=false);
	void endProcess(bool clear=true);
};

extern CMultThreadWorkMgr *g_MultThreadWorkMgr;

#endif