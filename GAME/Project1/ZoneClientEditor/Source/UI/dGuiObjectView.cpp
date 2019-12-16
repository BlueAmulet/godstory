//------------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//------------------------------------------------------------------------------

#include "UI/dGuiObjectView.h"
#include "renderInstance/renderInstMgr.h"
#include "sceneGraph/lightManager.h"
#include "sceneGraph/lightInfo.h"
#include "ts/TSShapeRepository.h"
#include "Gameplay/GameObjects/GameObject.h"
#include "console/consoleTypes.h"
#include "UI/dGuiMouseGamePlay.h"
#include "UI/dWorldEditor.h"
#include "math/mMatrix.h"
#include "math/mathUtils.h"
#include "Util/processMgr.h"
#include "Util/aniThread.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
#include "GamePlay/ClientGameplayState.h"

//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
////#define STATEBLOCK
GFXStateBlock* dGuiObjectView::mSetSB = NULL;
#define MAX_LINK 256
//-----------------------------------------------------------------------------
// dGuiObjectView::Model
//-----------------------------------------------------------------------------

dGuiObjectView::Model::Model():
shapeId(NULL),
mountNodeTarget(-1),
mountNodeSelf(-1),
action(GameObjectData::NullAnimation),
shapeInstance(NULL),
animThread(NULL),
target(NULL)
{
}

dGuiObjectView::Model::~Model()
{
	clear();
}

void dGuiObjectView::Model::clear()
{
	shapeId = NULL;
	mountNodeTarget = -1;
	mountNodeSelf = -1;
	action = GameObjectData::NullAnimation;
	SAFE_DELETE(shapeInstance);
	animThread = NULL;
	target = NULL;
	shape = NULL;
}

bool dGuiObjectView::Model::load(StringTableEntry id)
{
	clear();

	TSShapeInfo* pInfo = g_TSShapeRepository.InitializeTSShape(id, true,false,false,false);
	if(!pInfo || !pInfo->GetShape())
		return false;

	//if(!pInfo->m_TSShape->preloadMaterialList())
	//	return false;

	shape   = pInfo->GetShape();
	shapeId = pInfo->m_TSShapeId;
	shapeInstance = new TSShapeInstance(shape, true/*isClientObject()*/);
	//animThread = shapeInstance->addThread();

	return true;
}

bool dGuiObjectView::Model::isEmpty()
{
	return !(shapeId || shapeInstance);
}

void dGuiObjectView::Model::setAction(U32 act)
{
	if ((0 > act) || (act >= GameObjectData::NumTableActionAnims))
	{
		Con::warnf(avar("dGuiObjectView: The index %d is outside the permissible range. Please specify an animation index in the range [0, %d]", act, GameObjectData::NumTableActionAnims-1));
		return;
	}
	if(isEmpty())
		return;

	S32 seq = g_TSShapeRepository.GetSequence(shapeId,act);
	if(seq > -1)
	{
		if(!animThread)
			animThread = shapeInstance->addThread();
		if(animThread)
		{
			action = act;
			shapeInstance->setTimeScale(animThread,1.0f);
			//shapeInstance->transitionToSequence(animThread,seq,0, 0.25f, true);
			shapeInstance->setSequence(animThread,seq,0);
		}
	}
}

bool dGuiObjectView::Model::setSkin(StringTableEntry id,char skinTag)
{
	if(!load(id))
		return false;

	shapeInstance->reSkin(skinTag);
	return true;
}

bool dGuiObjectView::Model::setMountTarget(Model& tar, StringTableEntry tarPoint, StringTableEntry selfPoint)
{
	if(isEmpty() ||tar.isEmpty())
		return false;

	S32 tarNode = tarPoint ? tar.shape->findNode(tarPoint) : -1;
	S32 selfNode = selfPoint ? shape->findNode(selfPoint) : -1;

	// 自己可以没有链接点
	if(tarNode > -1)
	{
		target = &tar;
		mountNodeTarget = tarNode;
		mountNodeSelf = selfNode;
		return true;
	}
	return false;
}

void dGuiObjectView::Model::getMountMatrix(StringTableEntry linkPoint, MatrixF& mat)
{
	if(isEmpty() || !target || target->isEmpty())
		return;

	// linkPoint为空时取原目标链接点
	if(linkPoint)
		mountNodeTarget = target->shape->findNode(linkPoint);

	// 这里和游戏中有些不同，这里取得的坐标是target坐标系的，并不是世界坐标
	if(mountNodeTarget > -1)
	{
		mat = target->shapeInstance->getNodeTransforms()[mountNodeTarget];
		if(mountNodeSelf > -1)
		{
			MatrixF mountTrans = shapeInstance->getNodeTransforms()[mountNodeSelf];
			mat.mul(mountTrans.inverse());
		}
	}
}


//-----------------------------------------------------------------------------
// dGuiObjectView::Part
//-----------------------------------------------------------------------------
dGuiObjectView::Part::Part():
mountNodeTarget(-1),
target(NULL),
partNodeInst(NULL)
{
}

dGuiObjectView::Part::~Part()
{
    clear();
}

void dGuiObjectView::Part::clear()
{
    mountNodeTarget = -1;
    target = NULL;
    if ( partNodeInst )
    {
        partNodeInst->deleteObject();
        partNodeInst = NULL;
    }
}

bool dGuiObjectView::Part::load(StringTableEntry _partNodeDataBlockName)
{
    if ( partNodeInst )
    {
        partNodeInst->deleteObject();
        partNodeInst = NULL;
    }

    ParticleEmitterNodeData* dataBlock = (ParticleEmitterNodeData*)Sim::findObject(_partNodeDataBlockName);
    if ( !dataBlock )
    {
        // 处于人物选择界面时, datablock 脚本尚未执行...这里手动执行下...
        // !!! 注意: 登入游戏后, datablock 还会执行一次, 重复执行的不知道会不会有错 !!!
        char filename[MAX_PATH];
        dStrcpy(filename, MAX_PATH, _partNodeDataBlockName);
        int len = dStrlen(filename);       
        filename[len-5] = 0;
        char filepath[MAX_PATH];
        dSprintf( filepath, MAX_PATH, "gameres/Data/datablocks/particles/%s.CS", filename );
        Con::executef( "exec", filepath );

        // 再读一次
        dataBlock = (ParticleEmitterNodeData*)Sim::findObject(_partNodeDataBlockName);
    }
    if ( !dataBlock )
        return false;

    partNodeInst = new ParticleEmitterNode();
    partNodeInst->setClientObject();
    partNodeInst->setDataBlock(dataBlock);
    if ( !(partNodeInst->registerObject()) )
    {
        delete partNodeInst;
        partNodeInst = NULL;
        return false;
    }    

    return true;
}


bool dGuiObjectView::Part::loadEx(StringTableEntry _partNodeDataBlockName)
{
    if ( partNodeInst )
    {
        partNodeInst->deleteObject();
        partNodeInst = NULL;
    }
    std::string temp = _partNodeDataBlockName;
    temp = temp + "_Node";


    ParticleEmitterNodeData* dataBlock = (ParticleEmitterNodeData*)Sim::findObject(temp.c_str());
    //if ( !dataBlock )
    //{
    //    // 处于人物选择界面时, datablock 脚本尚未执行...这里手动执行下...
    //    // !!! 注意: 登入游戏后, datablock 还会执行一次, 重复执行的不知道会不会有错 !!!
    //    char filename[MAX_PATH];
    //    dStrcpy(filename, MAX_PATH, _partNodeDataBlockName);
    //    int len = dStrlen(filename);          
    //    char filepath[MAX_PATH];
    //    dSprintf( filepath, MAX_PATH, "gameres/Data/datablocks/particles/%s.CS", filename );
    //    Con::executef( "exec", filepath );

    //    // 再读一次
    //    dataBlock = (ParticleEmitterNodeData*)Sim::findObject(_partNodeDataBlockName);
    //}
    if ( !dataBlock )
        return false;

    partNodeInst = new ParticleEmitterNode();
    partNodeInst->setClientObject();
    partNodeInst->setDataBlock(dataBlock);
    if ( !(partNodeInst->registerObject()) )
    {
        delete partNodeInst;
        partNodeInst = NULL;
        return false;
    }    

    return true;
}

bool dGuiObjectView::Part::isEmpty()
{
    return (!partNodeInst);
}

bool dGuiObjectView::Part::setMountTarget(Model* tar, StringTableEntry tarPoint)
{
    if(isEmpty() || tar->isEmpty())
        return false;

    S32 tarNode = tarPoint ? tar->shape->findNode(tarPoint) : -1;
    if(tarNode > -1)
    {
        target = tar;
        mountNodeTarget = tarNode;
        return true;
    }
    return false;
}


//-----------------------------------------------------------------------------
// dGuiObjectView
//-----------------------------------------------------------------------------

dGuiObjectView::dGuiObjectView():
	mMouseState(None),
	mLastMousePoint(0, 0),
	lastRenderTime(0),
	mPlayerRotZ(0),
	mAnimationSeq(GameObjectData::NullAnimation),
	mFakeSun(NULL)
{
	mActive = true;
    mlistLinkedtoScene.clear();
	// TODO: lots of hardcoded things in here
	mCameraMatrix.identity();
	//mCameraMatrix.setColumn(3, mCameraPos);

	bAutoLeftRotation = false;
	bAutoRightRotation = false;
}

dGuiObjectView::~dGuiObjectView()
{
	clear();
	SAFE_DELETE(mFakeSun);
}

void dGuiObjectView::initPersistFields()
{
	Parent::initPersistFields();

	addField("PlayerRotZ", TypeF32, Offset(mPlayerRotZ, dGuiObjectView));
	addField("AutoLeftRotation", TypeBool,Offset(bAutoLeftRotation,dGuiObjectView));
	addField("AutoRightRotation",TypeBool,Offset(bAutoRightRotation,dGuiObjectView));
}

void dGuiObjectView::clear()
{
   
	mScene.clear();
	mRide.clear();
    mPlayer.clear();
    mPet.clear();
    for (std::list<AttachedModel*>::iterator it = mlistLinkedtoScene.begin(); it != mlistLinkedtoScene.end(); it++)
    {
        (*it)->pAttachedModel->clear();
        SAFE_DELETE((*it)->pAttachedModel);
        SAFE_DELETE((*it));
    } 
    mlistLinkedtoScene.clear();
    for (std::list<Part*>::iterator it = mlistLinkedtoScene_part.begin(); it != mlistLinkedtoScene_part.end(); it++)
    {
        (*it)->clear();        
        SAFE_DELETE((*it));
    } 
    mlistLinkedtoScene_part.clear();
    for (S32 i=0; i<MaxMounted; i++)
        mMounted[i].clear();

    clearAllEffc();
	//SAFE_DELETE(mFakeSun);
}

bool dGuiObjectView::onWake()
{
   
	if (!Parent::onWake())
	{
		return(false);
	}

	LightManager* lm = gClientSceneGraph->getLightManager();
	if (!mFakeSun)
	{
		mFakeSun = lm->createLightInfo();   
	}
    mFakeSun->mColor.set(0.0f,0.0f,0.0f);
    mFakeSun->mAmbient.set(1.1f, 1.1f, 1.1f);
    mFakeSun->mDirection.set(0.0f, 0.0f, 0.0f);
    mFakeSun->mBackColor.set(0.0f, 0.0f, 0.0f); // 打上背光
   	return(true);
}

void dGuiObjectView::onSleep()
{
    clearAllEffc();
    Parent::onSleep();
}

void dGuiObjectView::onMouseDown(const GuiEvent &event)
{
	if (!mActive || !mVisible || !mAwake)
	{
		return;
	}

	mMouseState = Rotating;
	mLastMousePoint = event.mousePoint;
	mouseLock();
}

void dGuiObjectView::onMouseUp(const GuiEvent &event)
{
	mouseUnlock();
	mMouseState = None;
}

void dGuiObjectView::onMouseDragged(const GuiEvent &event)
{
	if (mMouseState != Rotating)
	{
		return;
	}

	Point2I delta = event.mousePoint - mLastMousePoint;
	mLastMousePoint = event.mousePoint;

	mPlayerRotZ -= (delta.x * 0.01f);
}

void dGuiObjectView::onRightMouseDown(const GuiEvent &event)
{
	mMouseState = Zooming;
	mLastMousePoint = event.mousePoint;
	mouseLock();
}

void dGuiObjectView::onRightMouseUp(const GuiEvent &event)
{
	mouseUnlock();
	mMouseState = None;
}

void dGuiObjectView::onRightMouseDragged(const GuiEvent &event)
{
	if (mMouseState != Zooming)
	{
		return;
	}

	S32 delta = event.mousePoint.y - mLastMousePoint.y;
	mLastMousePoint = event.mousePoint;
}

void dGuiObjectView::setPlayerAnimation(S32 index)
{
	if ((0 > index) || (index >= GameObjectData::NumTableActionAnims))
	{
		Con::warnf(avar("dGuiObjectView: The index %d is outside the permissible range. Please specify an animation index in the range [0, %d]", index, GameObjectData::NumTableActionAnims-1));
		return;
	}
	mPlayer.setAction(index);
	for (S32 i=0; i<MaxMounted; i++)
	{
		mMounted[i].setAction(index);
	}
}

void dGuiObjectView::setScene(const char* modelName)
{    
    //清除主场景附属模型列表
    for (std::list<AttachedModel*>::iterator it = mlistLinkedtoScene.begin(); it != mlistLinkedtoScene.end(); it++)
    {
        (*it)->pAttachedModel->clear();
        SAFE_DELETE((*it)->pAttachedModel);
        SAFE_DELETE((*it));
    } 
    mlistLinkedtoScene.clear();
    //清除主场景附属粒子列表
    for (std::list<Part*>::iterator it = mlistLinkedtoScene_part.begin(); it != mlistLinkedtoScene_part.end(); it++)
    {
        (*it)->clear();        
        SAFE_DELETE((*it));
    } 
    mlistLinkedtoScene_part.clear();
   
    if(!mScene.load(modelName))
        return;

    // 场景可能带有固定的动画，直接播放
    S32 sequence = mScene.shape->findSequence("ambient");
    if(sequence != -1)
    {
		mScene.animThread = mScene.shapeInstance->addThread();
		mScene.shapeInstance->setPos(mScene.animThread, 0);
		mScene.shapeInstance->setTimeScale(mScene.animThread, 1.0f);
		mScene.shapeInstance->setSequence(mScene.animThread, sequence, 0);
	}

    char* tempNodeNameSet[15];
    int CountNum = 0;
    //加载主场景附属模型
    getNodeName_withPrefix("mod_" ,tempNodeNameSet ,CountNum);
    for (int i =  0; i< CountNum; ++i)
    {
        AttachedModel *pTempAttachedModel = new AttachedModel;
        pTempAttachedModel->pAttachedModel = new Model;
        /* if ( dStrcmp(tempNodeNameSet[i] , "mod_water") == 0)
         continue;*/
        if(!pTempAttachedModel->pAttachedModel->load(tempNodeNameSet[i]))
        {
            SAFE_DELETE(pTempAttachedModel->pAttachedModel);
            SAFE_DELETE(pTempAttachedModel);
            continue;
        }
                
        S32 tempNode = mScene.shape->findNode(tempNodeNameSet[i]);
        if(tempNode >= 0)
        {
            pTempAttachedModel->ModelMatrix = mScene.shapeInstance->getNodeTransforms()[tempNode];
        }
        S32 seq = pTempAttachedModel->pAttachedModel->shape->findSequence("ambient");
        if(seq != -1)
        {
            pTempAttachedModel->pAttachedModel->animThread = pTempAttachedModel->pAttachedModel->shapeInstance->addThread();
            pTempAttachedModel->pAttachedModel->shapeInstance->setPos(pTempAttachedModel->pAttachedModel->animThread, 0);
            pTempAttachedModel->pAttachedModel->shapeInstance->setTimeScale(pTempAttachedModel->pAttachedModel->animThread, 1.0f);
            pTempAttachedModel->pAttachedModel->shapeInstance->setSequence(pTempAttachedModel->pAttachedModel->animThread, seq, 0);
        }
        mlistLinkedtoScene.push_back(pTempAttachedModel);
    }

    //加载主场景附属粒子
    getNodeName_withPrefix("part_" ,tempNodeNameSet ,CountNum);
    for (int i =  0; i< CountNum; ++i)
    {
        Part *pTempAttachedParticle = new Part;        
        if(!pTempAttachedParticle->loadEx(tempNodeNameSet[i]))
        {           
            SAFE_DELETE(pTempAttachedParticle);
            continue;
        }
       pTempAttachedParticle->setMountTarget(&mScene, tempNodeNameSet[i]);       
       mlistLinkedtoScene_part.push_back(pTempAttachedParticle);
    }

    // 设置人物位置
    mPlayer.getMountMatrix("linkPoint", mPlayerMatrix); 

    mCameraMatrix.identity();
    char mountNodeName[64];
    dSprintf(mountNodeName, sizeof(mountNodeName), "linkCameraPoint");
    S32 moundNode = mScene.shape->findNode(mountNodeName);
    if(moundNode >= 0)      
    {
        mCameraMatrix = mScene.shapeInstance->getNodeTransforms()[moundNode];      
    }    

    // the first time recording
    lastRenderTime = Platform::getVirtualMilliseconds();
}

void dGuiObjectView::setPlayer(const char * modelName, char skinTag /* = 0 */)
{
	// 没有得到玩家模型，清除相关信息
	if(!mPlayer.setSkin(modelName, skinTag))
	{
		mRide.clear();
		mPlayer.clear();
		mPet.clear();
		for (S32 i=0; i<MaxMounted; i++)
			mMounted[i].clear();
		return;
	}
	Point3F PlayerPos(0,0,0);
	if(mScene.isEmpty())
	{
		// 场景为空的情况下
		mPlayerMatrix.identity();
		mPlayer.shape->bounds.getCenter(&PlayerPos);
		PlayerPos.z = mPlayer.shape->bounds.len_z() * 0.56f;
		mPlayerMatrix.mulP(PlayerPos);

		Point3F dir,camPos;
		mPlayerMatrix.getColumn(1,&dir);
		dir.z += 0.25f;
		dir.normalize();
		Point3F tempDir = -dir;
		mCameraMatrix = MathUtils::createOrientFromDir(tempDir);
		//F32 max = getMax(getMax(mPlayer.shape->bounds.len_x(), mPlayer.shape->bounds.len_y()), mPlayer.shape->bounds.len_z()) + mPlayer.shape->bounds.len_y()*0.5f;
		F32 max = (mPlayer.shape->bounds.max - mPlayer.shape->bounds.min).len();
		mCameraMatrix.setPosition(PlayerPos+dir*1.6f*max);
	}
	else
	{
		mPlayer.setMountTarget(mScene, "linkPoint", NULL);
		// 设置人物位置
		mPlayer.getMountMatrix("linkPoint", mPlayerMatrix);
		mPlayer.shape->bounds.getCenter(&PlayerPos);
		PlayerPos.z = 1.1f;
		mPlayerMatrix.mulP(PlayerPos);

		//从人物设置摄像机，因为美术给的人物LINK点是歪的。 -.-!!
// 		mCameraMatrix.identity();
// 		char mountNodeName[64];
// 		dSprintf(mountNodeName, sizeof(mountNodeName), "linkCameraPoint");
// 		S32 moundNode = mScene.shape->findNode(mountNodeName);
// 		if(moundNode >= 0)
// 		{
// 			MatrixF tmp = mScene.shapeInstance->getNodeTransforms()[moundNode];
// 
// 			int len = (tmp.getPosition() - PlayerPos).len();
// 
// 			Point3F dir,camPos;
// 			mPlayerMatrix.getColumn(1,&dir);
// 			dir.z = 0;
// 			dir.normalize();
// 			mCameraMatrix = MathUtils::createOrientFromDir(-dir);
// 			mCameraMatrix.setPosition(PlayerPos+dir*len*3.2f);
// 		}
	}

    // the first time recording
	lastRenderTime = Platform::getVirtualMilliseconds();
}

void dGuiObjectView::setMounted(U32 slot, const char * modelName, StringTableEntry tarPoint, StringTableEntry selfPoint,char skinTag)
{
	if(slot < 0 || slot >= MaxMounted)
		return;
	// 没有主角无法链接
	if(mPlayer.isEmpty())
		return;

	mMounted[slot].setSkin(modelName, skinTag);
	mMounted[slot].setMountTarget(mPlayer, tarPoint, selfPoint);
	if(mPlayer.action != GameObjectData::NullAnimation)
		mMounted[slot].setAction(mPlayer.action);
}

void dGuiObjectView::getMountedTransform(U32 slot, MatrixF *mat)
{
	if(slot < 0 || slot >= MaxMounted)
		return;
	if(mMounted[slot].isEmpty())
		return;

	MatrixF mountMat;
	mMounted[slot].getMountMatrix(NULL, mountMat);
	mat->mul(mPlayerMatrix,mountMat);
}

void dGuiObjectView::setEffcModelMounted(const char* modelName, Model& tarModel, StringTableEntry tarPoint, StringTableEntry selfPoint, char skinTag)
{
    Model* model = new Model();
    model->setSkin(modelName, skinTag);
    model->setMountTarget(tarModel, tarPoint, selfPoint);
    mEffcModel.push_back(model);
}

void dGuiObjectView::getEffcModelMountedTransform(Model* effcModel, MatrixF *_mat)
{
    if ( !_mat )
        return;

    MatrixF mat(true);   

    Model* curModel = effcModel;
    Model* curTarget = curModel ? curModel->target : NULL;
    while (curModel && curTarget)
    {
        if (curModel == &mPlayer) // !!! 注意: 现有代码中 mPlayer 的特殊性硬性打乱了层次结构...碰到 mPlayer 这里就直接对应的硬性打断, 如果改了这个打断就不需要了!!!
        {
            mat = mPlayerMatrix * mat;
            break;
        }

        if(curModel->mountNodeTarget > -1)
        {
            mat = curTarget->shapeInstance->getNodeTransforms()[curModel->mountNodeTarget] * mat;
            if(curModel->mountNodeSelf > -1)
            {
                MatrixF mountTrans = curModel->shapeInstance->getNodeTransforms()[curModel->mountNodeSelf];
                mat.mul(mountTrans.inverse());
            }
        }     

        curModel= curTarget;
        curTarget = curModel ? curModel->target : NULL;
    }

    *_mat = mat;
}

void dGuiObjectView::setEffcModelAnimation(Model* effcModel, S32 index)
{
    if ((0 > index) || (index >= GameObjectData::NumTableActionAnims))
    {
        Con::warnf(avar("dGuiObjectView: The index %d is outside the permissible range. Please specify an animation index in the range [0, %d]", index, GameObjectData::NumTableActionAnims-1));
        return;
    }
    effcModel->setAction(index);
}

void dGuiObjectView::setEffcPartMounted(const char* _partNodeDataBlockName, Model* tarModel, StringTableEntry tarPoint)
{
    Part* part = new Part();
    part->load(_partNodeDataBlockName);
    part->setMountTarget(tarModel, tarPoint);
    mEffcPart.push_back(part);
}

void dGuiObjectView::getEffcPartMountedTransform(Part* effcPart, MatrixF *_mat)
{
    if ( effcPart->isEmpty() || !_mat )
        return;
   
    MatrixF modelMountedTransform(true);
    getEffcModelMountedTransform(effcPart->target, &modelMountedTransform);

    MatrixF partMountedTransform(true);
    if (effcPart->target && effcPart->target->shapeInstance && effcPart->mountNodeTarget!=-1)
        partMountedTransform = effcPart->target->shapeInstance->getNodeTransforms()[effcPart->mountNodeTarget] * partMountedTransform;

    *_mat = modelMountedTransform * partMountedTransform;
}

void dGuiObjectView::clearAllEffc()
{
    for (S32 i=0; i<mEffcModel.size(); i++)
        delete mEffcModel[i];
    mEffcModel.clear();

    for (S32 i=0; i<mEffcPart.size(); i++)
        delete mEffcPart[i];
    mEffcPart.clear();

	 for (U32 i=0; i<mEP.size(); i++)
	 {
		 for (U32 j=0; j<mEP[i].size(); j++)
		 {
			 SAFE_DELETE(mEP[i][j]);
		 }
	 }
	 mEP.clear();

	 for (U32 i=0; i<mEM.size(); i++)
	 {
		 for (U32 j=0; j<mEM.size(); j++)
		 {
			 SAFE_DELETE(mEM[i][j]);
		 }
	 }
	 mEM.clear();
}

bool dGuiObjectView::processCameraQuery(CameraQuery* query)
{
	query->farPlane = 2100.0f;
	query->nearPlane = query->farPlane / 5000.0f;
//#ifdef NTJ_EDITOR
//	query->fov = mDegToRad(dWorldEditor::GetCameraFov());
//#endif
//#ifdef NTJ_CLIENT
//	query->fov = mDegToRad(dGuiMouseGamePlay::GetCameraFov());
//#endif
	query->fov = mDegToRad(30.0f);
	query->cameraMatrix = mCameraMatrix;

	gClientSceneGraph->setLastView2WorldMX(mCameraMatrix);
	return true;
}

void dGuiObjectView::onMouseEnter(const GuiEvent & event)
{
	Con::executef(this, "onMouseEnter");
}

void dGuiObjectView::onMouseLeave(const GuiEvent & event)
{
	Con::executef(this, "onMouseLeave");
}


void dGuiObjectView::renderWorld(const RectI &updateRect)
{
	bool bInGameing = false;

#ifdef NTJ_CLIENT
	if(g_ClientGameplayState->GetControlObject())
		bInGameing = true;
#endif

#ifdef USE_MULTITHREAD_ANIMATE
	if(!bInGameing)
		g_MultThreadWorkMgr->endProcess();

	g_MultThreadWorkMgr->enable(false);
#endif

	if ((! mPlayer.shapeInstance) && (! mScene.shapeInstance))
	{
		// nothing to render, punt
		//mPlayerRotZ = 0;
		return;
	}
   

	S32 time = Platform::getVirtualMilliseconds();
	S32 dt = mClamp(time - lastRenderTime, 0, 1000);
	lastRenderTime = time;

	LightManager* lm = gClientSceneGraph->getLightManager();
	lm->setSpecialLight(LightManager::slSunLightType, mFakeSun);

#ifdef NTJ_CLIENT
	if(!bInGameing)
		gClientSceneGraph->buildFogTexture(NULL);
#endif

#ifdef STATEBLOCK
	AssertFatal(mSetSB, "dGuiObjectView::renderWorld -- mSetSB cannot be NULL.");
	mSetSB->apply();
#else
	GFX->setZEnable(true);
	GFX->setZWriteEnable(true);
	GFX->setZFunc(GFXCmpLessEqual);
	GFX->setCullMode(GFXCullNone);
#endif


	MatrixF meshCamTrans(true);
	TSMesh::setRefract(false);
	TSMesh::setCamTrans(meshCamTrans);
	TSMesh::setSceneState( NULL );
	TSMesh::setObject( NULL );
	TSMesh::setOverrideFade(GFX->getDrawUtil()->getLockedAlpha() / 255.0f);

    if (mScene.shapeInstance)
    {
        // animate and render in a run pose
        F32 fdt = dt;
        if(mScene.animThread)
            mScene.shapeInstance->advanceTime( fdt/1000.f, mScene.animThread );
        mScene.shapeInstance->animate();

		TSMesh::setFieldMesh(true);
        mScene.shapeInstance->render();
		TSMesh::setFieldMesh(false);
    }
  //主场景附属模型
    for (std::list<AttachedModel*>::iterator it =  mlistLinkedtoScene.begin() ; it != mlistLinkedtoScene.end() ; ++it)
    {
        GFX->pushWorldMatrix();
        GFX->multWorld( (*it)->ModelMatrix );

        // animate and render in a run pose
       if((*it)->pAttachedModel->animThread)
        {
            F32 fdt = dt;
            (*it)->pAttachedModel->shapeInstance->advanceTime( fdt/1000.f, (*it)->pAttachedModel->animThread );
            (*it)->pAttachedModel->shapeInstance->animate();
        }
        (*it)->pAttachedModel->shapeInstance->render();
        GFX->popWorldMatrix();
    }
   //主场景附属粒子

    for (std::list<Part*>::iterator it =  mlistLinkedtoScene_part.begin() ; it != mlistLinkedtoScene_part.end() ; ++it)
    {
        MatrixF mat;
        getEffcPartMountedTransform((*it),&mat);
        (*it)->partNodeInst->setTransform(mat);
        (*it)->partNodeInst->render(gClientSceneGraph->getBaseCameraPosition());
    }
	
	if (mPlayer.shapeInstance)
	{
		MatrixF zRot;

	  //在按钮按下时自动旋转
		if (bAutoLeftRotation)
          mPlayerRotZ +=0.08f;
		if (bAutoRightRotation)
		  mPlayerRotZ -=0.08f;

		mPlayerMatrix.set(EulerF(0.0f, 0.0f, mPlayerRotZ), mPlayerMatrix.getPosition());

		GFX->pushWorldMatrix();
		GFX->multWorld( mPlayerMatrix );

		// animate and render in a run pose
		if(mPlayer.action != GameObjectData::NullAnimation)
		{
			F32 fdt = dt;
			mPlayer.shapeInstance->advanceTime( fdt/1000.f, mPlayer.animThread );
			mPlayer.shapeInstance->animate();
		}
		mPlayer.shapeInstance->render();
		GFX->popWorldMatrix();
	}

	for (S32 i=0; i<MaxMounted; i++)
	{
		if (!mMounted[i].isEmpty())
		{
			// render a weapon
			MatrixF mat;
			getMountedTransform(i, &mat);

			GFX->pushWorldMatrix();
			GFX->multWorld( mat );

			if(mMounted[i].action != GameObjectData::NullAnimation)
			{
				F32 fdt = dt;
				mMounted[i].shapeInstance->advanceTime( fdt/1000.f, mMounted[i].animThread );
				mMounted[i].shapeInstance->animate();
			}
			mMounted[i].shapeInstance->render();

			GFX->popWorldMatrix();
		}
	}

    // 特效模型、粒子
    for (S32 i=0; i<mEffcModel.size(); i++)
    {
        if (!mEffcModel[i]->isEmpty())
        {
            MatrixF mat;
            getEffcModelMountedTransform(mEffcModel[i], &mat);

            GFX->pushWorldMatrix();
            GFX->multWorld( mat );

            if(mEffcModel[i]->action != GameObjectData::NullAnimation)
            {
                F32 fdt = dt;
                mEffcModel[i]->shapeInstance->advanceTime(fdt/1000.f, mEffcModel[i]->animThread);
                mEffcModel[i]->shapeInstance->animate();
            }
            mEffcModel[i]->shapeInstance->render();

            GFX->popWorldMatrix();
        }
    }

    for ( int i = 0; i < mEffcPart.size(); i++ )
    {
        if (!mEffcPart[i]->isEmpty())
        {
            MatrixF mat;
            getEffcPartMountedTransform(mEffcPart[i], &mat);
            mEffcPart[i]->partNodeInst->setTransform(mat);

            mEffcPart[i]->partNodeInst->render(gClientSceneGraph->getBaseCameraPosition());
        }
    }

	 {//特效模型，特效粒子渲染
		 Model* model = NULL;
		for (U32 i=0; i<mEM.size(); i++)
		{
			for (U32 j=0; j<mEM[i].size(); j++)
			{
				model = mEM[i][j];
				if (model != NULL && !model->isEmpty())
				{
					MatrixF mat;
					getEffcModelMountedTransform(model, &mat);

					GFX->pushWorldMatrix();
					GFX->multWorld( mat );

					if(model->action != GameObjectData::NullAnimation)
					{
						F32 fdt = dt;
						model->shapeInstance->advanceTime(fdt/1000.f, model->animThread);
						model->shapeInstance->animate();
					}
					model->shapeInstance->render();

					GFX->popWorldMatrix();
				}
			}
		}

		Part* part = NULL;
		for (U32 i=0; i<mEP.size(); i++)
		{
			for (U32 j=0; j<mEP[i].size(); j++)
			{
				part = mEP[i][j];
				if (part!=NULL && !part->isEmpty())
				{
					MatrixF mat;
					getEffcPartMountedTransform(part, &mat);
					part->partNodeInst->setTransform(mat);

					part->partNodeInst->render(gClientSceneGraph->getBaseCameraPosition());
				}
			}
		}
	 }

	gRenderInstManager.sort();
	gRenderInstManager.render();
	gRenderInstManager.clear();

#ifdef USE_MULTITHREAD_ANIMATE
#ifdef NTJ_CLIENT//编辑器不要
	if(g_ClientGameplayState->GetControlPlayer())
#endif
		g_MultThreadWorkMgr->enable(true);
#endif
}

void dGuiObjectView::setOrbitDistance(F32 distance)
{
	// Make sure the orbit distance is within the acceptable range
	//mOrbitDist = mClampF(distance, mMinOrbitDist, mMaxOrbitDist);
}


void dGuiObjectView::getNodeName_withPrefix(const char * prefix,char** NodeNameSet , int &count)
{
    int nCount = 0;
    count = 0;
    S32 prefixLength = dStrlen(prefix);

    for (S32 i=0; i<mScene.shape->names.size(); i++)
    {
        std::string strNodeName = mScene.shape->names[i];
        strNodeName.resize(prefixLength);
        if (strNodeName == prefix)
        {
            NodeNameSet[nCount++] = const_cast<char *>(mScene.shape->names[i]);
            count++;
        }
    }
}

//-----------------------------------------------------------------------------
// Console stuff (dGuiObjectView)
//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(dGuiObjectView);

void dGuiObjectView::addEffectMounted( EP_ITEM id )
{
	const EffectPacketItemData* pData = g_EffectDataRepository.getEffectPacketItemData(id);
	if (pData == NULL)
	{
		return;
	}

	EffectObjectData* pEod = NULL;
	for (U32 i=0; i<pData->objects.size(); i++)
	{
		pEod = pData->objects[i];
		if (pEod == NULL)
		{
			continue;
		}

		switch (pEod->type)
		{
		case EPO_Particle://粒子特效
			{
				Mit mit;
				if (mEM.find(pData->PacketId)!=mEM.end() 
					&& (mit=mEM[pData->PacketId].find(pEod->objectId))!=mEM[pData->PacketId].end()
					&& mit->second!=NULL)
				{//粒子链接到特效模型
					Part *part = new Part;
					part->load(pEod->DataBlockName);
					part->setMountTarget(mit->second, pEod->link.parentPoint);
					//注意：没有对应粒子，则创建一个。否则，则直接覆盖
					mEP[pData->PacketId][pEod->objectId] = part;
				}
				else
				{
					Part *part = new Part;
					part->load(pEod->DataBlockName);
					if (pEod->link.mountSlotType < 0)
					{//粒子链接到人物身体
						part->setMountTarget(&mPlayer, pEod->link.parentPoint);
						//注意：没有对应粒子，则创建一个。否则，则直接覆盖
						mEP[pData->PacketId][pEod->objectId] = part;
					} 
					else if (pEod->link.mountSlotType<MaxMounted)
					{//粒子链接到装备
						part->setMountTarget(&mPlayer, pEod->link.parentPoint);
						//注意：没有对应粒子，则创建一个。否则，则直接覆盖
						mEP[pData->PacketId][pEod->objectId] = part;
					}
					else
					{//出错
						delete part;
						part = NULL;
						AssertWarn(false, "没有找到特效粒子对象对应的链接模型");
					}
				}
			}
			break;
		case EPO_Shape://模型特效
			{
				Mit mit;
				if (mEM.find(pData->PacketId)!=mEM.end() 
					&& (mit=mEM[pData->PacketId].find(pEod->link.parentId))!=mEM[pData->PacketId].end()
					&& mit->second!=NULL)
				{//模型链接到特效模型
					Model *model = new Model;
					model->setSkin(pEod->DataBlockName, 0);
					model->setMountTarget(*mit->second, pEod->link.parentPoint, pEod->link.linkPoint);
					mEM[pData->PacketId][pEod->objectId] = model;
				}
				else
				{
					Model *model = new Model;
					model->setSkin(pEod->DataBlockName, 0);
					if (pEod->link.mountSlotType < 0)
					{//模型链接到人物身体
						model->setMountTarget(mPlayer, pEod->link.parentPoint, pEod->link.linkPoint);
						//注意：没有对应模型，则创建一个。否则，则直接覆盖
						mEM[pData->PacketId][pEod->objectId] = model;
					} 
					else if (pEod->link.mountSlotType<MaxMounted)
					{//模型链接到装备
						model->setMountTarget(mMounted[pEod->link.mountSlotType], pEod->link.parentPoint, pEod->link.linkPoint);
						//注意：没有对应模型，则创建一个。否则，则直接覆盖
						mEM[pData->PacketId][pEod->objectId] = model;
					}
					else
					{//出错
						delete model;
						model = NULL;
						AssertWarn(false, "没有找到特效粒子对象对应的链接模型");
					}

				}
			}
			break;
		default:
			break;
		}
	}
}
ConsoleMethod(dGuiObjectView, setScene, void, 3, 3,
			  "(string shapeName)\n"
			  "Sets the model to be displayed in this control\n\n"
			  "\\param shapeName Name of the model to display.\n")
{
	argc;
	object->setScene(argv[2]);
}

ConsoleMethod(dGuiObjectView, setPlayer, void, 3, 3,
			  "(string shapeName)\n"
			  "Sets the model to be displayed in this control\n\n"
			  "\\param shapeName Name of the model to display.\n")
{
	argc;
	object->setPlayer(argv[2]);
}

ConsoleMethod(dGuiObjectView, setSeq, void, 3, 3,
			  "(int index)\n"
			  "Sets the animation to play for the viewed object.\n\n"
			  "\\param index The index of the animation to play.")
{
	argc;
	object->setPlayerAnimation(dAtoi(argv[2]));
}

ConsoleMethod(dGuiObjectView, setMounted, void, 6, 7,"")
{
	U32 ssId = dAtoi(argv[3]);
	StringTableEntry pShapeName = argv[3];
	if(ssId)
	{
		const ShapesSet* pSS = g_ShapesSetRepository.GetSet(ssId);
		S32 slot = mClamp(dAtoi(argv[2]), 0, dGuiObjectView::MaxMounted);
		if(pSS && pSS->subImages[slot].shapeName && pSS->subImages[slot].shapeName[0])
			pShapeName = pSS->subImages[slot].shapeName;
	}
	if(argc == 6)
		object->setMounted(dAtoi(argv[2]), pShapeName, argv[4], argv[5]);
	else
		object->setMounted(dAtoi(argv[2]), pShapeName, argv[4], argv[5], *((char*)argv[6]));
}

ConsoleMethod(dGuiObjectView, setOrbitDistance, void, 3, 3,
			  "(float distance)\n"
			  "Sets the distance at which the camera orbits the object. Clamped to the acceptable range defined in the class by min and max orbit distances.\n\n"
			  "\\param distance The distance to set the orbit to (will be clamped).")
{
	argc;
	object->setOrbitDistance(dAtof(argv[2]));
}

ConsoleMethod(dGuiObjectView, clearPlayer, void, 2, 2,"clear the objectView")
{
	argc;
	object->setPlayer(NULL);
}

void dGuiObjectView::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->setRenderState(GFXRSZFunc, GFXCmpLessEqual);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->endStateBlock(mSetSB);
}


void dGuiObjectView::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}
}

void dGuiObjectView::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		resetStateBlock();
	}


}

void dGuiObjectView::shutdown()
{
	SAFE_DELETE(mSetSB);
}


