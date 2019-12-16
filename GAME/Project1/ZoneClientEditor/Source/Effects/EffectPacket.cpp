//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "core/bitStream.h"
#include "math/mathUtils.h"
#include "T3D/gameConnectionEvents.h"
#include "Gameplay/Data/readDataFile.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "GamePlay/GameEvents/GameNetEvents.h"
#include "GamePlay/ClientGameplayState.h"
#include "T3D/fx/particleEmitterNode.h"
#include "T3D/fx/particleEmitter.h"
#include "lightingSystem/synapseGaming/sgDecalProjector.h"
#include "lightingSystem/synapseGaming/sgLightObject.h"
#include "effects/EffectPacket.h"
#include "T3D/sfxEmitter.h"
#include "Ts/tsSimpleShape.h"
#include "Effects/SplineObject.h"
#include "T3D/gameFunctions.h"

#ifdef NTJ_CLIENT
#include "UI/dGuiMouseGamePlay.h"
#endif

EffectPacketContainer g_EffectPacketContainer;
EffectDataRepository  g_EffectDataRepository;

U32 EffectPacketContainer::iDseed = 0;

//==========================================================================================================================================
//==========================================================================================================================================
//==========================================================================================================================================
EffectPacketItemData::EffectPacketItemData()
{
	objects.clear();
}

EffectPacketItemData::~EffectPacketItemData()
{
	for(int i=0;i<objects.size();i++)
	{
		delete objects[i];
	}
	objects.clear();
}

EffectDataRepository::EffectDataRepository()
{
	mData.clear();
}

EffectDataRepository::~EffectDataRepository()
{
	Clear();
}

void EffectDataRepository::Clear()
{
	EffectPacketItemData_Hash::iterator it = mData.begin();
	for(;it!=mData.end();it++)
	{
		delete it->second;
	}
	mData.clear();
}

bool EffectDataRepository::Write()
{
	return true;
}

bool EffectDataRepository::Read()
{
	CDataFile file;
	RData tempData;
	char filename[1024];

	// 读取BUFF数据
	Platform::makeFullPathName(GAME_EFFECTDATA_FILE, filename, sizeof(filename));
	file.ReadDataInit();
	AssertRelease(file.readDataFile(filename), "can NOT read file : EffectRepository.dat!");
	for(int i = 0 ; i < file.RecordNum ; ++ i)
	{
		EffectPacketItemData* pData = NULL;

		// 特效包ID
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S32, "EffectRepository.dat::Read !");

		EffectPacketItemData_Hash::iterator it = mData.find(tempData.m_S32);
		if(it!=mData.end())
			pData = it->second;

		if(!pData)
		{
			pData = new EffectPacketItemData;
			pData->PacketId = tempData.m_S32;
			mData.insert(EffectPacketItemData_Hash::value_type(pData->PacketId, pData));
		}

		// 特效包持续时间
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S32, "EffectRepository.dat::Read !");
		pData->duration = tempData.m_S32;

		// 特效对象
		EffectObjectData* pObject = new EffectObjectData;
		pData->objects.push_back(pObject);

		// 特效类型
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_enum8, "EffectRepository.dat::Read !");
		pObject->type = (EPO_Type)tempData.m_Enum8;

		// 本地ID
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S8, "EffectRepository.dat::Read !");
		pObject->objectId = tempData.m_S8;

		// 缩放
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "EffectRepository.dat::Read !");
		pObject->scale.x = tempData.m_F32;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "EffectRepository.dat::Read !");
		pObject->scale.y = tempData.m_F32;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "EffectRepository.dat::Read !");
		pObject->scale.z = tempData.m_F32;

		// 坐标偏移量
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "EffectRepository.dat::Read !");
		pObject->offset.x = tempData.m_F32;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "EffectRepository.dat::Read !");
		pObject->offset.y = tempData.m_F32;

		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "EffectRepository.dat::Read !");
		pObject->offset.z = tempData.m_F32;

		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "EffectRepository.dat::Read !");
		pObject->stateMask = tempData.m_U32;

		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "EffectRepository.dat::Read !");
		pObject->flags = tempData.m_U32;

		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "EffectRepository.dat::Read !");
		pObject->velocity = tempData.m_F32;

		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S32, "EffectRepository.dat::Read !");
		pObject->traceTime = tempData.m_S32;

		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S8, "EffectRepository.dat::Read !");
		pObject->link.parentId = tempData.m_S8;

		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S8, "EffectRepository.dat::Read !");
		pObject->link.mountSlotType = tempData.m_S8;

		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "EffectRepository.dat::Read !");
		pObject->link.parentPoint = StringTable->insert(tempData.m_string);

		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "EffectRepository.dat::Read !");
		pObject->link.linkPoint = StringTable->insert(tempData.m_string);

		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "EffectRepository.dat::Read !");
		pObject->DataBlockId = tempData.m_U32;

		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "EffectRepository.dat::Read !");
		pObject->DataBlockName = StringTable->insert(tempData.m_string);

		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S32, "EffectRepository.dat::Read !");
		pObject->duration = tempData.m_S32;

		// 
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_S32, "EffectRepository.dat::Read !");
		pObject->delay = tempData.m_S32;
	}
/*
	EffectPacketItemData *pItem = new EffectPacketItemData;
	pItem->PacketId = 1;
	pItem->duration = TM_IGNORE;

	EffectObjectData *pObj;

	//假定的一个DECAL-----------------------------------------------------------
	pObj = new EffectObjectData;
	pObj->objectId = 1;
	pObj->type = EPO_Decal;
	pObj->scale = Point3F(1,1,1);

	pObj->flags							= EffectObjectData::StateMask_EnableA | EffectObjectData::StateMask_VisibleA;
	pObj->velocity						= 0.0f;
	pObj->traceTime						= 0.0f;
	pObj->startTime						= 0;
	pObj->duration						= TM_INFINITE;  //5000
	pObj->DataBlockName					= StringTable->insert("testDecal");
	pObj->isBindObject					= true;
	pObj->link.parentId					= -1;	//空
	pObj->link.mountSlotType			= -1;	//身体
	pObj->link.parentPoint				= 14;
	pObj->link.linkPoint				= -1;

	pItem->objects.push_back(pObj);
/*
	//假定的一个emmiter--------------------------------------------------------
	pObj = new EffectObjectData;
	pObj->objectId = 2;
	pObj->type = EPO_Particle;
	pObj->scale = Point3F(1,1,1);
	pObj->local = MatrixF(true);

	pObj->status.increment();
	pObj->status[0].enableStatus					= true;
	pObj->status[0].isVisible						= true;
	pObj->status[0].isTraceTarget					= false;
	pObj->status[0].velocity						= 0.0f;
	pObj->status[0].acceleration					= 0.0f;
	pObj->status[0].traceTime						= 0.0f;
	
	pObj->status[0].startTime						= 0;
	pObj->status[0].duration						= TM_INFINITE;

	pObj->status[0].enableData						= true;
	pObj->status[0].DataBlockName = StringTable->insert("SmokeEmitter");

	pObj->status[0].enableBindInfo		= true;
	pObj->status[0].isBindObject		= true;
	pObj->status[0].link.parentId		= -1;	//空
	pObj->status[0].link.mountSlotType	= -1;	//身体
	pObj->status[0].link.parentPoint	= 6;
	pObj->status[0].link.linkPoint		= -1;  //忽略
	
	//pObj->status[1].enableStatus					= false;
	pItem->objects.push_back(pObj);
//*/
/*
	//假定的一个shape--------------------------------------------------------
	pObj = new EffectObjectData;
	pObj->objectId = 1;
	pObj->type = EPO_Shape;
	pObj->scale = Point3F(0.2f,0.2f,0.2f);

	pObj->flags					= EffectObjectData::StateMask_EnableA | EffectObjectData::StateMask_VisibleA |
									EffectObjectData::StateMask_EnableB | EffectObjectData::StateMask_VisibleB | EffectObjectData::StateMask_TraceB |
									EffectObjectData::StateMask_EnableC | EffectObjectData::StateMask_VisibleC;
	pObj->velocity						= 5.0f;
	pObj->traceTime						= 0.0f;
	pObj->startTime						= 0;
	pObj->duration						= TM_INFINITE;
	pObj->DataBlockName					= StringTable->insert("~/data/shapes/effect/chuansong.mod");
	pObj->isBindObject					= true;
	pObj->link.parentId					= -1;	//空
	pObj->link.mountSlotType			= -1;	//身体
	pObj->link.parentPoint				= 5;
	pObj->link.linkPoint				= 1;


	pItem->objects.push_back(pObj);
	//*/
/*
	//假定的一个light--------------------------------------------------------
	pObj = new EffectObjectData;
	pObj->objectId = 4;
	pObj->type = EPO_Light;
	pObj->scale = Point3F(1,1,1);

	pObj->flags					= EffectObjectData::StateMask_EnableA | EffectObjectData::StateMask_VisibleA |
									EffectObjectData::StateMask_EnableB | EffectObjectData::StateMask_VisibleB |
									EffectObjectData::StateMask_EnableC | EffectObjectData::StateMask_VisibleC;
	pObj->velocity						= 0.0f;
	pObj->traceTime						= 0.0f;
	pObj->duration						= TM_INFINITE; //15000;
	pObj->DataBlockName					= StringTable->insert("defaultLight");
	pObj->isBindObject					= true;
	pObj->link.parentId					= 1;	//objectId=3
	pObj->link.mountSlotType			= -1;	//身体
	pObj->link.parentPoint				= 1;	//shape的link01point上
	pObj->link.linkPoint				= -1;

	pItem->objects.push_back(pObj);

	mData[pItem->PacketId] = pItem;
/
	//自定义data
	ProjectDecalData* pData = new ProjectDecalData;
	pData->mDiameter		= 2;
	pData->lifeSpan		= 0;
	pData->mInterval	= 0;
	pData->mRot			= 0;
	pData->mTextureName = StringTable->insert("~/data/environments/decal/dianditubiao1");//Arrow dianditubiao1

	pData->mDiameterScale= 0.003f;
	pData->mMaxDiameter	= 5;

	pData->mAlpha		= 1;
	pData->mAlphaScale	= 0.001;
	pData->mMaxAlpha	= 0;

	pData->mLoop		= true;
	pData->mLoopTimeStep= 1000;
	pData->mLoopType	= 0;
	pData->assignName("testDecalData");
	pData->registerObject();

	*/
	return true;
}

const EffectPacketItemData *EffectDataRepository::getEffectPacketItemData(EP_ITEM id)
{
	EffectPacketItemData_Hash::iterator it = mData.find(id);
	if(it!=mData.end())
	{
		return it->second;
	}

	return NULL;
}


//==========================================================================================================================================
//==========================================================================================================================================
//==========================================================================================================================================

EffectObject::EffectObject()
{
	inited = false;
	curStatus = 0;
	lastTime = 0;
	statusStartTime = 0;
	lastRenderTime = 0;
	currentPos.set(0,0,0);
	targetPos.set(0,0,0);
	Id = 0;
	currentVelocity = 8;
	currentTraceTime = 0;
	m_pMainObj = NULL;
	m_pData = NULL;
	m_pItem = NULL;
	childs.clear();
	parent = NULL;
}

EffectObject::~EffectObject()
{
	for(int i=0;i<childs.size();i++)
		delete childs[i];
	
	unregisterEffectObject();
}

bool EffectObject::registerEffectObject()
{
	//已经初始化了
	if(inited)
		return true;

	inited = true;
	m_pMainObj = NULL;
	switch(m_pData->type)
	{
	case EPO_Particle:		//粒子
		createParticle();
		break;
	case EPO_Shape:			//模型
		createShape();
		break;
	case EPO_Sound:			//声音
		createSound();
		break;
	case EPO_Light:			//光源
		createLight();
		break;
	case EPO_Decal:			//贴花
		createDecal();
		break;
	case EPO_Screen:		//屏幕
		createScreen();
		break;
	case EPO_Camera:		//镜头
		createCamera();
		break;
	case EPO_Spline:		//曲线
		createSpline();
#ifdef NTJ_CLIENT
		break;
#else
		return true;
#endif
	case EPO_Electricity:	//闪电
		createElectricity();
		break;
	case EPO_FluidLight:	//流光
		createFluidLight();
		break;
	default:
		return false;
	}

	if(m_pMainObj)
		m_pMainObj->setGhostable(false);
	return (bool)m_pMainObj;
}

bool EffectObject::unregisterEffectObject()
{
	switch(m_pData->type)
	{
	case EPO_Particle:		//粒子
		return deleteParticle();
	case EPO_Shape:			//模型
		return deleteShape();
	case EPO_Sound:			//声音
		return deleteSound();
	case EPO_Light:			//光源
		return deleteLight();
	case EPO_Decal:			//贴花
		return deleteDecal();
	case EPO_Screen:		//屏幕
		return deleteDecal();
	case EPO_Camera:		//镜头
		return deleteDecal();
	case EPO_Spline:		//曲线
		return deleteSpline();
	case EPO_Electricity:	//闪电
		return deleteElectricity();
	case EPO_FluidLight:	//流光
		return deleteFluidLight();
	default:
		return false;
	}

	return true;
}

bool EffectObject::createParticle()
{
	ParticleEmitterNode *pParticle = new ParticleEmitterNode;
	pParticle->setDataBlock( (GameBaseData*) Sim::findObject(m_pData->DataBlockName));
	if(pParticle->getDataBlock())
	{
		MatrixF mat(true);
		if(!m_pData->isFlags(EffectObjectData::Flags_Bind))
		{
			mat.mul(m_pItem->sourceMat);
			pParticle->setTransform(mat);
		}

		pParticle->setScale(m_pData->scale);
		if(m_pItem && m_pItem->source)
			pParticle->setLayerID(m_pItem->source->getLayerID());
		if(!pParticle->registerObject())
		{
			delete pParticle;
			return false;
		}

		Id = pParticle->getId();		//记录对象ID
	}
	else
	{
		delete pParticle;
		return false;
	}

	pParticle->useClosestPoint(true);
	addToMissionGroup(pParticle);
	m_pMainObj = pParticle;
	return true;
}

bool EffectObject::deleteParticle()
{
	if(m_pMainObj)
		m_pMainObj->deleteObject();

	return true;
}

bool EffectObject::createSound()
{
	SFXEmitter *pSound = new SFXEmitter;
	pSound->setSoundId(m_pData->DataBlockName);

	MatrixF mat(true);
	if(!m_pData->isFlags(EffectObjectData::Flags_Bind))
	{
		mat.mul(m_pItem->sourceMat);
		pSound->setTransform(mat);
	}

	pSound->setScale(Point3F(1,1,1));

	if(m_pItem && m_pItem->source)
		pSound->setLayerID(m_pItem->source->getLayerID());
	if(!pSound->registerObject())
	{
		delete pSound;
		return false;
	}

	Id = pSound->getId();		//记录对象ID
	addToMissionGroup(pSound);
	m_pMainObj = pSound;
	return true;
}

bool EffectObject::deleteSound()
{
	if(m_pMainObj)
		m_pMainObj->deleteObject();
	return true;
}

bool EffectObject::createLight()
{
	sgLightObject *pLight = new sgLightObject;
	pLight->setDataBlock( (GameBaseData*) Sim::findObject(m_pData->DataBlockName));
	if(pLight->getDataBlock())
	{
		MatrixF mat(true);
		if(!m_pData->isFlags(EffectObjectData::Flags_Bind))
		{
			mat.mul(m_pItem->sourceMat);
			pLight->setTransform(mat);
		}
		pLight->setScale(m_pData->scale);
		if(m_pItem && m_pItem->source)
			pLight->setLayerID(m_pItem->source->getLayerID());
		if(!pLight->registerObject())
		{
			delete pLight;
			return false;
		}

		Id = pLight->getId();		//记录对象ID
	}
	else
	{
		delete pLight;
		return false;
	}

	addToMissionGroup(pLight);
	m_pMainObj = pLight;
	return true;
}

bool EffectObject::deleteLight()
{
	if(m_pMainObj)
		m_pMainObj->deleteObject();
	return true;
}

bool EffectObject::createDecal()
{
	bool userNew = false;
	sgDecalProjector *pDecal = new sgDecalProjector;
	ProjectDecalData* pData = (ProjectDecalData*) Sim::findObject(m_pData->DataBlockName);	//脚本data
	if(!pData)
		pData = (ProjectDecalData*) Sim::findObject("testDecalData");	//脚本data

	if(pData)
	{
		pDecal->setDataBlock( (GameBaseData*)pData );

		MatrixF mat(true);
		if(!m_pData->isFlags(EffectObjectData::Flags_Bind))
		{
			mat.mul(m_pItem->sourceMat);
		}
		else
		{
			if(m_pItem->source)
			{
				mat = m_pItem->source->getTransform();
			}
		}

		if(m_pItem && m_pItem->source)
			pDecal->setLayerID(m_pItem->source->getLayerID());
		if(!pDecal->registerObject())
		{
			delete pDecal;
			return false;
		}

		pDecal->updateProject(mat);
		pDecal->setScale(m_pData->scale);
		Id = pDecal->getDecalId();		//记录Decal的全局ID
	}
	else
	{
		delete pDecal;
		return false;
	}

	addToMissionGroup(pDecal);
	m_pMainObj = pDecal;
	return true;
}

bool EffectObject::deleteDecal()
{
	gProjectDecalManager->ageDecal(Id);

	if(m_pMainObj)
		m_pMainObj->deleteObject();
	return true;
}

bool EffectObject::createScreen()
{
#ifdef NTJ_SERVER
#endif
#ifdef NTJ_CLIENT
#endif
	return true;
}

bool EffectObject::deleteScreen()
{
	return true;
}

bool EffectObject::createCamera()
{
#ifdef NTJ_SERVER
#endif
#ifdef NTJ_CLIENT
#endif
	return true;
}

bool EffectObject::deleteCamera()
{
	return true;
}

bool EffectObject::createShape()
{
	TSSimpleShape *pShape = new TSSimpleShape;
	pShape->setShapeName(m_pData->DataBlockName);

	MatrixF mat(true);
	if(!m_pData->isFlags(EffectObjectData::Flags_Bind))
	{
		mat.mul(m_pItem->sourceMat);
		pShape->setTransform(mat);
	}

	pShape->setScale(m_pData->scale);
	//EffectObjectData*  pData = (EffectObjectData*)m_pData;
	pShape->setFade(RenderableSceneObject::Fade_AddRemove, !m_pData->isStateMask(EffectObjectData::StateMask_VisibleA, curStatus), false);

	if(m_pItem && m_pItem->source)
		pShape->setLayerID(m_pItem->source->getLayerID());
	if(!pShape->registerObject())
	{
		delete pShape;
		return false;
	}

	Id = pShape->getId();

	addToMissionGroup(pShape);
	m_pMainObj = pShape;
	return true;
}

bool EffectObject::deleteShape()
{
	if(m_pMainObj)
		m_pMainObj->deleteObject();
	return true;
}

bool EffectObject::createSpline()
{
#if 0
//#ifdef NTJ_CLIENT
	SplineObject *pSpline = new SplineObject;

	GameBase* pSrc = dynamic_cast<GameBase*>(Sim::findObject(m_pItem->sourceObjectId));
	GameBase* pTgt = dynamic_cast<GameBase*>(Sim::findObject(m_pItem->targetObjectId));
	if(!pSpline->Create(pSrc, pTgt, TM_IGNORE) || !pSpline->registerObject())
	{
		delete pSpline;
		return false;
	}

	MatrixF mat(true);
	if(!m_pData->isBindObject)
	{
		mat.mul(m_pItem->sourceMat);
		pSpline->setTransform(mat);
	}

	//pShape->setScale(m_pData->scale);
	pSpline->setFade(m_pData->isFlags(EffectObjectData::StateMask_VisibleA, curStatus));

	Id = pSpline->getId();
	addToMissionGroup(pSpline);
	m_pMainObj = pSpline;
#endif
	return true;
}

bool EffectObject::deleteSpline()
{
	if(m_pMainObj)
		m_pMainObj->deleteObject();
	return true;
}

bool EffectObject::createElectricity()
{
	ElectricityData* pData = (ElectricityData*) Sim::findObject(m_pData->DataBlockName);	//脚本data
	if (!pData)
		return false;
	SceneObject* pSrc = m_pItem->source;
	SceneObject* pTgt = m_pItem->target;
	// 目标和源缺一不可
	if(!pSrc || !pTgt)
		return false;

	Electricity* pElect = new Electricity();
	pElect->setLayerID(pSrc->getLayerID());
	pElect->setData(pData);
	pElect->setCarrier(pSrc, pTgt);
	if(!pElect->registerObject())
	{
		delete pElect;
		return false;
	}

	Id = pElect->getId();

	addToMissionGroup(pElect);
	m_pMainObj = pElect;
	return true;
}

bool EffectObject::deleteElectricity()
{
	if(m_pMainObj)
		m_pMainObj->deleteObject();
	return true;
}

bool EffectObject::createFluidLight()
{
#ifdef NTJ_SERVER
	if(m_pItem && m_pItem->source && (m_pItem->source->getType() & GameObjectType))
	{
		GameObject* pObj = (GameObject*)(m_pItem->source.getObject());
		pObj->addFluidLight(1);
	}
#endif
	return true;
}

bool EffectObject::deleteFluidLight()
{
#ifdef NTJ_SERVER
	if(m_pItem && m_pItem->source && (m_pItem->source->getType() & GameObjectType))
	{
		GameObject* pObj = (GameObject*)(m_pItem->source.getObject());
		pObj->addFluidLight(-1);
	}
#endif
	return true;
}

F32 EffectObject::setFade(bool val, bool fading /* = false */, F32 fadeTime /* = 1000.0f */, F32 fadeDelay /* = 0.0f */)
{
	if(!m_pMainObj)
		return fadeTime;

	F32 ft = fadeTime;
	//强制遵循data的淡入淡出规则
	if(m_pData->isFlags(EffectObjectData::Flags_FollowDataFade))
		fading = val ? m_pData->isStateMask(EffectObjectData::StateMask_FadeInA, curStatus) : m_pData->isStateMask(EffectObjectData::StateMask_FadeOutA, curStatus);

	switch(m_pData->type)
	{
	case EPO_Particle:		//粒子
		((ParticleEmitterNode*)m_pMainObj)->setPause(!val);
		break;
	case EPO_Shape:			//模型
		if(m_pData->isFlags(EffectObjectData::Flags_InvisibleCycle) && !val)
		{
			//设置一个周期后隐藏
			ft = getMax(((TSSimpleShape*)m_pMainObj)->setInvisibleCycle(1) * 1000.0f, fadeTime);
		}
		else
			((TSSimpleShape*)m_pMainObj)->setFade(RenderableSceneObject::Fade_AddRemove, !val, fading, fadeTime, fadeDelay);
		break;
	case EPO_Sound:			//声音
		break;
	case EPO_Light:			//光源
		break;
	case EPO_Decal:			//贴花
		break;
	}

	EffectObject *pObject;
	for(int i=childs.size()-1;i>=0;i--)
	{
		pObject = childs[i];
		ft = getMax(pObject->setFade(val, fading, fadeTime, fadeDelay), ft);
	}
	return ft;
}

void EffectObject::setStatus(U8 status)
{
	if(curStatus >= status || curStatus>=EffectPacketItem::MaxStatus)
		return;

	curStatus = status;

	statusStartTime = lastTime;

	setFade(m_pData->isStateMask(EffectObjectData::StateMask_VisibleA, curStatus),
		m_pData->isStateMask(EffectObjectData::StateMask_VisibleA, curStatus) ? m_pData->isStateMask(EffectObjectData::StateMask_FadeInA, curStatus) : m_pData->isStateMask(EffectObjectData::StateMask_FadeOutA, curStatus));
	if(!m_pData->isStateMask(EffectObjectData::StateMask_EnableA, curStatus))
		return;

	if(m_pData->isStateMask(EffectObjectData::StateMask_TraceA, curStatus))
	{
		currentVelocity = m_pData->velocity;	//初始化追踪速度
	}
}

void EffectObject::prepRenderImage(const U32 stateKey)
{
	if(!inited || !m_pData->isStateMask(EffectObjectData::StateMask_EnableA, curStatus))
		return;

	if(m_pData->isStateMask(EffectObjectData::StateMask_TraceA, curStatus))
	{
		// 检查初始位置
		if(currentPos.isZero())
		{
			//绑定推进
			MatrixF childMX = getLinkMaterix();
			MatrixF parentMX(true);
			const _objectInfo *pLink = getLinkInfo();
			if(parent)
			{
				TSSimpleShape *pObj = (TSSimpleShape *)parent->m_pMainObj;
				if(pObj)
					pObj->getRenderNodeTransform(StringTable->insert(pLink->parentPoint),&parentMX);
			}
			else
			{
				GameBase *pObj = m_pItem->source;
				if(pObj)
				{
					if(pObj->getType() & GameObjectType)
					{
						GameObject* pgo = (GameObject*)pObj;
						if(pLink->mountSlotType==-1)
							pgo->getRenderNodeTransform(StringTable->insert(pLink->parentPoint),&parentMX);
						else
							pgo->getRenderImageTransform(pLink->mountSlotType,StringTable->insert(pLink->parentPoint),&parentMX);
					}
					else
						parentMX = pObj->getTransform();
				}
			}

			childMX.inverse();
			parentMX.mul(childMX);
			setMatrix(parentMX);

			currentPos = parentMX.getPosition();
			lastRenderTime = Platform::getVirtualMilliseconds();
		}

		if(m_pItem->target)
		{
			targetPos = getObjPosition(m_pItem->target);
		}

		bool reached = true;
		Point3F diff = targetPos-currentPos;			
		F32 len = diff.len();
		diff.normalize();
		MatrixF new_mat;
		new_mat = MathUtils::createOrientFromDir(diff);
		if(len>0.01)
		{
			currentPos += currentVelocity  * diff * (getMin(Platform::getVirtualMilliseconds() - lastRenderTime, (SimTime)1024)/1000.0f);
			Point3F nextDiff = targetPos - currentPos;
			if (mDot(diff, nextDiff) >= 0)
			{
				reached = false;
			}
		}

		if(reached)
			currentPos = targetPos;

		new_mat.setPosition(currentPos);
		setMatrix(new_mat);

		// 到达目标
		if(reached)
			return;
	}
	else if(m_pData->isFlags(EffectObjectData::Flags_Bind))
	{
		//绑定推进
		MatrixF childMX = getLinkMaterix();
		MatrixF parentMX(true);
		//char msg[64];
		const _objectInfo *pLink = getLinkInfo();
		//dSprintf(msg,64,"link%02dpoint",pLink->parentPoint);

		if(parent)
		{
			TSSimpleShape *pObj = (TSSimpleShape *)parent->m_pMainObj;
			if(pObj)
				pObj->getRenderNodeTransform(StringTable->insert(pLink->parentPoint),&parentMX);
		}
		else
		{
			GameBase *pObj = m_pItem->source;
			if(pObj)
			{
				if(pObj->getType() & GameObjectType)
				{
					GameObject* pgo = (GameObject*)pObj;
					if(pLink->mountSlotType==-1)
						pgo->getRenderNodeTransform(StringTable->insert(pLink->parentPoint),&parentMX);
					else
						pgo->getRenderImageTransform(pLink->mountSlotType,StringTable->insert(pLink->parentPoint),&parentMX);
				}
				else
					parentMX = pObj->getTransform();
			}
		}

		childMX.inverse();
		parentMX.mul(childMX);
		setMatrix(parentMX);

		currentPos = parentMX.getPosition();
	}
	// 不绑定到对象的特效无需更新位置

	for(int i=childs.size()-1;i>=0;i--)
	{
		childs[i]->prepRenderImage(stateKey);
	}
	lastRenderTime = Platform::getVirtualMilliseconds();
}

bool EffectObject::advanceTime(S32 dt)
{
	updateVisibility();
	lastTime += dt;
	if( delayTime > 0)
	{
		delayTime -= dt;
		if(delayTime <= 0)
			statusStartTime = Platform::getVirtualMilliseconds();
	}
	if(m_pData->duration!=TM_IGNORE && m_pData->duration!=TM_INFINITE && delayTime <= 0)		//对象自己推进或者是永久对象
	{
		if(lastTime >= /*statusStartTime + */m_pData->duration )
			return false;
	}
	if(!inited && delayTime <= 0)
	{
		if(!registerEffectObject())
			return false;
		GameBase* po = m_pItem->source;
		if(m_pMainObj)
			InitTransform(m_pMainObj->getTransform(), (po ? po->getTransform() : m_pMainObj->getTransform()));
	}
	if(!inited)
		return true;

#ifdef NTJ_CLIENT
	if(m_pData->type == EPO_Spline && m_pMainObj)
	{
		if(!((SplineObject*)m_pMainObj)->AdvanceTime(dt))
			return false;
	}
#endif
	for(int i=childs.size()-1;i>=0;i--)
	{
		if(!childs[i]->advanceTime(dt))
		{
			//删除这个特效对象
			delete childs[i];
			childs.erase(i);
		}
	}

	return true;
}

bool EffectObject::Init(const EffectPacketItem *pItem,const EffectObjectData *pObjData)
{
	if(!pItem || !pObjData)
	{
		AssertFatal(false, "EffectObject::Init() - input error!!");
		Con::errorf("EffectObject::Init() - input error!!");
		return false;
	}
	inited			= false;
	lastTime		= 0;
	statusStartTime = 0;
	m_pItem			= pItem;
	m_pMainObj		= NULL;
	m_pData			= pObjData;
	curStatus		= 0;

	//下面两个坐标如果是绑定点或者是有追踪目标的话会随时改变
	targetPos		= pItem->targetPos;

	currentVelocity = m_pData->velocity;
	currentTraceTime = 0.0f;

	return ((delayTime <= 0) ? registerEffectObject() : true);
}

void EffectObject::InitTransform(const MatrixF &SourceMat, const MatrixF &Objmat)
{
	if(!inited || !m_pMainObj || !m_pMainObj->getId())
		return;
	// 确定偏移量
	static MatrixF mat;
	static Point3F offset, start_point, end_point;
	offset = m_pData->offset;
	if(m_pData->isFlags(EffectObjectData::Flags_RandomOffset) && !offset.isZero())
	{
		F32 r = offset.len();
		F32 rd = mClampF(0.2f, 0.5f/r, 1.0f/r);
		F32 rd_x = fmod((m_pMainObj->getId()+Platform::getVirtualMilliseconds()) * 0.179456f, rd*2.0f) - rd;
		F32 rd_y = fmod(rd_x * 2.179456f, rd*2.0f) - rd;
		rd_x = r * rd_x;
		rd_y = r * rd_y;
		offset.x += rd_x;
		offset.y += rd_y;
	}
	// 初始化位置
	if(!m_pData->isFlags(EffectObjectData::Flags_Bind))
	{
		mat = SourceMat;
		mat.setPosition(SourceMat.getPosition() + offset);
#ifdef NTJ_CLIENT
		if(m_pData->isFlags(EffectObjectData::Flags_Floor))
		{
			start_point = mat.getPosition();
			end_point = mat.getPosition();
			start_point.z += 4;
			end_point.z -= 4;
			RayInfo ri;
			bool hit = gClientContainer.castRay(start_point,end_point,dGuiMouseGamePlay::m_MouseFloorCollisionMask,&ri);
			if(hit)
			{
				ri.point.z += 0.2f * mClampF(m_pMainObj->getObjBox().len_x(), 0.5f, 2.0f);
				mat.setPosition(ri.point);
			}
		}
#endif
	}
	else if(m_pData->link.parentId == -1)
		mat = Objmat;
	setMatrix(mat);
}

Point3F& EffectObject::getObjPosition(GameBase *pObj) 
{
	static Point3F pos;
	pObj->getWorldBox().getCenter(&pos);
	return pos;
}

MatrixF EffectObject::getLinkMaterix()
{
	MatrixF mat(true);
	if(m_pData->isFlags(EffectObjectData::Flags_Bind) && m_pMainObj)
	{
		switch(m_pData->type)
		{
		case EPO_Shape:
			{
				TSSimpleShape *pShape = (TSSimpleShape *)m_pMainObj;
				if(pShape)
				{
					pShape->getNodeMatrix(m_pData->link.linkPoint,&mat);
				}
			}
			break;
		case EPO_Particle:
			{
				//Ray: 粒子的ObjBox会依据粒子范围随时变化
				return mat;
			}
			break;
		default:
			{
				Point3F center;
				m_pMainObj->getObjBox().getCenter(&center);
				mat.setColumn(3,center);
				//mat = mainObj->getTransform();
			}
			break;
		}
	}

	return mat;
}

void EffectObject::setMatrix(MatrixF &mat)
{
	if(m_pData->type == EPO_Decal)
	{
		Point3F pos = mat.getPosition();
		// <Edit> [8/4/2009 joy] 从下往下投影
		pos.z += 2;
		mat.setPosition(pos);
		sgDecalProjector *pDecal = (sgDecalProjector *)m_pMainObj;
		pDecal->updateProject(mat);
	}
	else
	{
		m_pMainObj->setTransform(mat);
		/*if(m_pData->type == EPO_Particle)
		{
			MatrixF rot = mat;
			rot.setPosition(Point3F(0,0,0));
			ParticleEmitterNode *pEmitter = (ParticleEmitterNode *)m_pMainObj;
			pEmitter->setRotationMX(rot);
		}*/
	}
}

void EffectObject::updateVisibility()
{
	if(!m_pItem || !m_pMainObj)
		return;
	switch(m_pData->type)
	{
	case EPO_Particle:
	case EPO_Shape:
	case EPO_Decal:
		{
			((RenderableSceneObject*)m_pMainObj)->setVisibility(m_pItem->visibility);
		}
		break;
	}
}

void EffectObject::setAnimationPos(F32 pos)
{
	if(!m_pMainObj)
		return;
	if(m_pData->type == EPO_Shape)
	{
		((TSSimpleShape*)m_pMainObj)->setAnimationPos(pos);
	}
}

bool EffectObject::updatePos(S32 dt)
{
	if(!inited || !m_pData->isStateMask(EffectObjectData::StateMask_EnableA, curStatus))
		return true;

	if(m_pData->isStateMask(EffectObjectData::StateMask_TraceA, curStatus))
	{
		//追踪推进
		currentTraceTime += dt;
		if(m_pData->traceTime>0 && (currentTraceTime > m_pData->traceTime))
			return false;	//超过时间

		// 追踪到目的地了
		if(targetPos.equal(currentPos))
			return false;
	}

	EffectObject *pObject;
	for(int i=childs.size()-1;i>=0;i--)
	{
		pObject = childs[i];
		if(!pObject->updatePos(dt))
		{
			//删除这个特效对象
			delete pObject;
			childs.erase(i);
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////EffectPacketItem/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////EffectPacketItem/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////EffectPacketItem/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////EffectPacketItem/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////EffectPacketItem/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////EffectPacketItem/////////////////////////////////////////////////////////////////////////////////

bool EffectPacketItem::addEffectObject(EffectObject *pObj)
{
	if(pObj->m_pData->isFlags(EffectObjectData::Flags_Bind) && pObj->m_pData->link.parentId!=-1)
	{
		for(int i=0;i<objects.size();i++)
		{
			if(objects[i]->m_pData->objectId == pObj->m_pData->link.parentId)
			{
				pObj->parent = objects[i];
				objects[i]->childs.push_back(pObj);
				return true;
			}
		}

		Con::printf("EffectPacketItem::addEffectObject error : %d !", PacketId, pObj->m_pData->objectId);
		return false;
	}
	else
	{
		pObj->parent = NULL;
		objects.push_back(pObj);
	}

	return true;
}	

void EffectPacketItem::setStatus(U8 status)
{
	EffectObject *pObject;
	for(int i=objects.size()-1;i>=0;i--)
	{
		pObject = objects[i];
		AssertFatal(pObject, "EffectPacketItem :: Object is NULL!");
		if(pObject)
			pObject->setStatus(status);
	}
}

F32 EffectPacketItem::setFade(bool val, bool fading /* = true */, F32 fadeTime /* = 1000.0f */, F32 fadeDelay /* = 0.0f */)
{
	F32 ft = 0;
	EffectObject *pObject;
	for(int i=objects.size()-1;i>=0;i--)
	{
		pObject = objects[i];
		AssertFatal(pObject, "EffectPacketItem :: Object is NULL!");
		if(pObject)
			ft = getMax(pObject->setFade(val,  fading, fadeTime, fadeDelay), fadeTime);
	}
	return ft;
}

void EffectPacketItem::prepRenderImage(const U32 stateKey)
{
	if(deleteMask)
		return ;

	if(duration!=TM_IGNORE && duration!=TM_INFINITE)
	{
		if(lastTime >= startTime+duration)
		{
			return ;
		}
	}
	// 特效包跟着源
	GameBase *pObj = source;
	if(!pObj && isBind)
		return ;

	EffectObject *pObject;
	for(int i=objects.size()-1 ;i>=0;i--)
	{
		pObject = objects[i];
		pObject->prepRenderImage(stateKey);
	}
}

bool EffectPacketItem::advanceTime(S32 dt)
{
	if(deleteMask)
		return false;
	lastTime += dt;

	if(duration!=TM_IGNORE && duration!=TM_INFINITE)
	{
		if(lastTime >= startTime+duration)
		{
			return false;
		}
	}
	// 特效包跟着源
	GameBase *pObj = source;
	if(!pObj && isBind)
		return false;
	if(pObj && isBind)
		visibility = pObj->getVisibility();

#ifdef NTJ_SERVER
	// 更新网络对象的位置
	EPItem *pEPItem = netEP;
	if(pEPItem && pObj)
	{
		pEPItem->setPosition(pObj->getPosition());
	}
#endif

	EffectObject *pObject;

	if(duration!=TM_INFINITE)				//永久存在的不用继续推进时间了，这个ITEM下的所有对象都必须是永久的，并且是在第一时间就产生对象的
	{
		for(int i=objects.size()-1 ;i>=0;i--)
		{
			pObject = objects[i];
			if(!pObject->advanceTime(dt))
			{
				//删除这个特效对象
				delete pObject;
				objects.erase(i);
			}
		}
	}

	if(!objects.size())
		return false;

	bool traceDone = false;
	bool allTraceDone = true;
	for(int i=objects.size()-1;i>=0;i--)
	{
		pObject = objects[i];
		if(!pObject->updatePos(dt))
		{
			//删除这个特效对象
			delete pObject;
			objects.erase(i);
			traceDone = true;
		}
		else if(pObject->m_pData && pObject->m_pData->isStateMask(EffectObjectData::StateMask_TraceA, curStatus))
			allTraceDone = false;
	}
	// 最后一个追踪特效被删除后，整个特效包也应该删除
	deleteMask |= (traceDone && allTraceDone);

	if(!objects.size())
		return false;

	return true;
}

EffectPacketItem::EffectPacketItem()
{
	notify=true;
	deleteMask=false;
	isBind = true;
	curStatus=0;
	itemId=0;
	PacketId = 0;
	startTime = 0;
	duration = 0;
	lastTime = 0;
	sourceMat.identity();
	source = 0;
	target = 0;
	netEP = 0;
	visibility = 1.0f;
	targetPos.set(0,0,0);
	objects.clear();
	addToContainer = false;

	// 直接获得管理ID
	itemId = EffectPacketContainer::getId();
}

EffectPacketItem::~EffectPacketItem()
{
	for(int i=0;i<objects.size();i++)
	{
		delete objects[i];
	}
	objects.clear();

	GameBase *pObj = source;
	if(pObj)
	{
		if(pObj->getType() & GameObjectType)
			((GameObject*)pObj)->GetSpell().DeleteSpellEffect(itemId);
		if(notify)
			pObj->deleteEffectItem(itemId);
	}

	EPItem *pEPItem = netEP;
	if(pEPItem)
		pEPItem->deleteObject();
}

/////////////////////////////////////////////////////////////////////////////////EffectPacketContainer/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////EffectPacketContainer/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////EffectPacketContainer/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////EffectPacketContainer/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////EffectPacketContainer/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////EffectPacketContainer/////////////////////////////////////////////////////////////////////////////////
//特效包对象Id生成器
U32 EffectPacketContainer::getId()
{
	if(++iDseed == 0)
		++iDseed;
	return iDseed;
}

U32 EffectPacketContainer::addEffectPacket(EP_ITEM PacketId,GameBase *pSource,const MatrixF &SourceMat,GameBase *pTarget,Point3F &TargetPos,F32 duration /* = TM_UNKNOW */)
{
	EffectPacketItem *pItem = new EffectPacketItem;

	return addEffectPacket(pItem,PacketId,pSource,SourceMat,pTarget,TargetPos,duration);
}

U32 EffectPacketContainer::addEffectPacket(EffectPacketItem* pItem,EP_ITEM PacketId,GameBase *pSource,const MatrixF &SourceMat,GameBase *pTarget,Point3F &TargetPos,F32 duration /* = TM_UNKNOW */)
{
	static MatrixF mat;
	static Point3F offset;
	if(!pItem || !pSource)
		return 0;

	if(pItem->addToContainer)
		return pItem->itemId;

	if(PacketId == EP_Wound_Player && pTarget)
		if(pTarget->getType() & GameObjectType)
			if(!(((GameObject*)pTarget)->getGameObjectMask() & PlayerObjectType))
				PacketId = EP_Wound_Other;

	items[pItem->itemId] = pItem;
	const EffectPacketItemData *pItemData = g_EffectDataRepository.getEffectPacketItemData(PacketId);
	if(!pItemData)
	{
		pItem->setDelete();
		return 0;
	}

	if(duration==TM_UNKNOW)
	{
		duration = pItemData->duration;
	}

	// 必须要有Source
	pItem->source = pSource;
	pItem->target = pTarget;
	pItem->targetPos	= TargetPos;
	pItem->PacketId		= PacketId;
	pItem->startTime	= Platform::getVirtualMilliseconds();
	pItem->duration		= duration;
	pItem->lastTime		= Platform::getVirtualMilliseconds();
	pItem->sourceMat	= SourceMat;

	bool success = false;
	for(int i=0;i<pItemData->objects.size();i++)
	{
		// 产生对象并取得data
		EffectObject *pObj	= new EffectObject;
		const EffectObjectData *pData = pItemData->objects[i];

		// 如果是延时播放的
		if(pData->isFlags(EffectObjectData::Flags_RandomDelay))
			pObj->delayTime = pData->delay * fmod((i+(0xFF & Platform::getVirtualMilliseconds())) * 0.7654321f, 1.0f);
		else
			pObj->delayTime = pData->delay;

		// 添加到列表
		if(pObj->Init(pItem,pData) && pItem->addEffectObject(pObj))
		{
			success = true;
			pObj->InitTransform(SourceMat, pSource->getTransform());
		}
		else
		{
			//初始化失败
			delete pObj;
		}
	}

#ifdef NTJ_SERVER
	EPItem* pNet = new EPItem(pItem->PacketId, pItem->source ? pItem->source->getId() : 0, pItem->target ? pItem->target->getId() : 0, 0);
	if(!pNet)
		success = false;

	if(!pNet->registerObject())
	{
		pNet->deleteObject();
		success = false;
	}
	else
	{
		pNet->setTransform(pSource->getTransform());
		pItem->netEP = pNet;
	}
#endif

	if(!success)
	{
		pItem->setDelete();
		return 0;
	}

	pItem->addToContainer = true;

	if(pSource)
		pSource->addEffectItem(pItem->itemId);
	return pItem->itemId;
}

bool EffectPacketContainer::removeEffectPackets(EP_ITEM PacketId)
{
	bool ret = false;
	EffectPacketItem_Hash::iterator it = items.begin();
	for(;it!=items.end();)
	{
		if(it->second->PacketId == PacketId)
		{
			it->second->setDelete();
			ret = true;
		}
		else
			it++;
	}
	return ret;
}

bool EffectPacketContainer::removeEffectPacket(U32 itemId,bool notify/* =true */, U32 time /* = 0 */)
{
	EffectPacketItem_Hash::iterator it = items.find(itemId);
	if(it!=items.end())
	{
		if(time > 0)
		{
			time = it->second->setFade(false, true, time);
			it->second->resetTime(Platform::getVirtualMilliseconds(), time);
			it->second->setNotify(notify);
			it->second->setDelete(false);
			it->second->isBind = false;
			return true;
		}
		it->second->setNotify(notify);
		it->second->setDelete();
		return true;
	}
	return false;
}

bool EffectPacketContainer::removeFromMap(U32 itemId)
{
	EffectPacketItem_Hash::iterator it = items.find(itemId);
	if(it!=items.end())
	{
		items.erase(it);
		return true;
	}
	return false;
}

void EffectPacketContainer::removeFromScene()
{
	EffectPacketItem_Hash::iterator it = items.begin();
	for(;it!=items.end();)
	{
		SAFE_DELETE(it->second);
		it++;
	}
	items.clear();
}

bool EffectPacketContainer::setEffectPacketStatus(U32 itemId,U8 status)
{
#ifdef NTJ_CLIENT
	EffectPacketItem_Hash::iterator it = items.find(itemId);
	if(it!=items.end())
	{
		it->second->setStatus(status);
		return true;
	}
#endif
	return false;
}

bool EffectPacketContainer::setEffectPacketsStatus(EP_ITEM PacketId,U8 status)
{
#ifdef NTJ_CLIENT
	EffectPacketItem_Hash::iterator it = items.begin();
	for(;it!=items.end(); it++)
	{
		if(it->second->PacketId == PacketId)
			it->second->setStatus(status);
	}
#endif
	return true;
}

//EffectPacketItem *EffectPacketContainer::getEffectItem(U32 id)
//{
//	EffectPacketItem_Hash::iterator it = items.find(id);
//	if(it!=items.end())
//	{
//		return it->second;
//	}
//	return NULL;
//}

void EffectPacketContainer::refreshDecal(U32 id, F32* rad, Point3F* pos, ColorI* col)
{
	EffectPacketItem_Hash::iterator it = items.find(id);
	if(it == items.end() || !it->second)
		return;

	for(S32 i=it->second->objects.size()-1 ;i>=0;i--)
	{
		EffectObject *pObject = it->second->objects[i];
		if(pObject->m_pData && pObject->m_pData->type == EPO_Decal)
		{
			ProjectDecalInstance* pDecal = gProjectDecalManager->getDecal(((sgDecalProjector*)pObject->getObj())->getDecalId());
			if(pDecal)
			{
				if(rad)
				{
					pDecal->setProjectLen(mClampF(*rad * 2.0f, 1.0f, 15.0f));
					pDecal->setRadius(mClampF(*rad, 0.5f, 12.0f));
				}
				if(pos && !pObject->m_pData->isFlags(EffectObjectData::Flags_Bind))
					pDecal->setPos(*pos);
				if(col)
					pDecal->setColor(*col);
			}
		}
	}
}

void EffectPacketContainer::prepRenderImage(const U32 stateKey)
{
	EffectPacketItem *pItem;
	EffectPacketItem_Hash::iterator it = items.begin();
	for(;it!=items.end();)
	{
		pItem = it->second;
		pItem->prepRenderImage(stateKey);
		it++;
	}
}

void EffectPacketContainer::advanceTime(S32 dt)
{
	EffectPacketItem *pItem;
	EffectPacketItem_Hash::iterator it = items.begin();
	for(;it!=items.end();)
	{
		pItem = it->second;
		if(!pItem->advanceTime(dt))
		{
			delete pItem;
			items.erase(it++);
		}
		else
			it++;
	}
}

void EffectPacketContainer::hideEffect(U32 id, bool hide)
{
	EffectPacketItem_Hash::iterator it = items.find(id);
	if(it != items.end() && it->second)
	{
		EffectPacketItem *pItem = it->second;
		EffectObject *pObj;
		for (S32 i=pItem->objects.size()-1; i>=0; --i)
		{
			pObj = pItem->objects[i];
			if(pObj->m_pMainObj)
				((RenderableSceneObject*)pObj->m_pMainObj)->setFade(RenderableSceneObject::Fade_Hide, hide, false, 0, 0);
		}
	}
}

IMPLEMENT_CO_NETOBJECT_V1(EPItem);
EPItem::~EPItem()
{
	g_EffectPacketContainer.removeEffectPacket(itemId);
}

bool EPItem::onAdd()
{
	if(!Parent::onAdd())
		return false;

	addToScene();
	return true;
}

void EPItem::onRemove()
{
	removeFromScene();
	Parent::onRemove();
}

U64  EPItem::packUpdate(NetConnection *conn, U64 mask, BitStream *stream)
{
	U64 retMask = Parent::packUpdate(conn, mask, stream);

	if(stream->writeFlag(mask & InitialUpdateMask))
	{
		stream->writeInt(PacketId, Base::Bit32);
		stream->writeInt(curStatus, Base::Bit8);
		GameObject* pObj = (GameObject*)Sim::findObject(sourceObjectId);
		stream->writeInt(pObj ? conn->getGhostIndex(pObj) : 0, NetConnection::GhostIdBitSize);
		pObj = (GameObject*)Sim::findObject(targetObjectId);
		stream->writeInt(pObj ? conn->getGhostIndex(pObj) : 0, NetConnection::GhostIdBitSize);
	}

	return retMask;
}

void EPItem::unpackUpdate(NetConnection *conn,BitStream *stream)
{
	Parent::unpackUpdate(conn, stream);

	if(stream->readFlag())	// InitialUpdateMask
	{
		PacketId = stream->readInt(Base::Bit32);
		curStatus = stream->readInt(Base::Bit8);
		GameObject* SrcObj = dynamic_cast<GameObject*>(conn->resolveGhost(stream->readInt(NetConnection::GhostIdBitSize)));
		sourceObjectId = SrcObj ? SrcObj->getId() : 0;
		GameObject* TgtObj = dynamic_cast<GameObject*>(conn->resolveGhost(stream->readInt(NetConnection::GhostIdBitSize)));
		targetObjectId = TgtObj ? TgtObj->getId() : 0;

		if(SrcObj)
		{
			Point3F pos = SrcObj->getPosition();
			g_EffectPacketContainer.addEffectPacket(PacketId, SrcObj, SrcObj->getTransform(), TgtObj, pos,TM_UNKNOW);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ConsoleMethod(GameBase,addpacket, S32 , 3, 3, "obj.addpacket(%id)")
{
	MatrixF mat(true);
	Point3F pos = object->getPosition();
	return g_EffectPacketContainer.addEffectPacket(dAtoi(argv[2]),object,mat,NULL,pos);
}


#ifdef NTJ_CLIENT 
//测试脚本，需要全部删除
ConsoleFunction(addpacket, bool , 2, 2, "addpacket(x,y,z)" )
{
	std::string st1;

	MatrixF mat(true);
	GameBase *pSource = (GameBase *)g_ClientGameplayState->GetControlPlayer();
	mat.setPosition(pSource->getPosition());

	//Point3F pos(atof(argv[1]),atof(argv[2]),atof(argv[3]));
	Point3F pos = pSource->getPosition();
	pos.x += 2;

	U32 id = g_EffectPacketContainer.addEffectPacket(dAtoi(argv[1]),pSource,mat,NULL,pos);
	Con::printf("EffectItem = %d",id);
	return true;
}

ConsoleFunction(removepacket, bool , 2, 2, "removepacket(id)" )
{
	return g_EffectPacketContainer.removeEffectPacket(atoi(argv[1]));
}

ConsoleFunction(setpacketstatus, bool , 3, 3, "setpacketstatus(id,status)" )
{
	return g_EffectPacketContainer.setEffectPacketStatus(atoi(argv[1]),atoi(argv[2]));
}

ConsoleFunction(createEmmiter, bool , 1, 1, "createEmmiter()" )
{
	ParticleEmitterNode *pParticle = new ParticleEmitterNode;
	pParticle->assignName("testEmmiter");
	pParticle->setDataBlock( (GameBaseData*) Sim::findObject("eff_s_003_e_Node"));
	if(pParticle->getDataBlock())
	{
		if(!pParticle->getDataBlock()->isProperlyAdded())
			if(pParticle->getDataBlock()->registerObject())
				return false;
		MatrixF mat(true);
		Point3F pos;
		GameConnection* conn = GameConnection::getConnectionToServer();
		Player *pPlayer = (Player*)conn->getControlObject();
		pos = pPlayer->getPosition();
		mat.setColumn(3,pos);
		pParticle->setTransform(mat);
		pParticle->setScale(Point3F(1,1,1));
		if(!pParticle->registerObject())
		{
			delete pParticle;
			return false;
		}
	}
	else
	{
		delete pParticle;
		return false;
	}

	addToMissionGroup(pParticle);

	return true;
}

ConsoleFunction(removeEmmiter, bool , 1, 1, "removeEmmiter()" )
{
	SimObject *pObj = Sim::findObject("testEmmiter");
	if(pObj)
		pObj->deleteObject();

	return true;
}

ConsoleFunction(createDecal, bool , 1, 1, "createDecal()" )
{
	sgDecalProjector *pDecal = new sgDecalProjector;
	pDecal->assignName("testDecal");
	pDecal->setDataBlock( (GameBaseData*) Sim::findObject("Arrow"));

	MatrixF mat(true);
	Point3F pos;
	GameConnection* conn = GameConnection::getConnectionToServer();
	Player *pPlayer = (Player*)conn->getControlObject();
	pos = pPlayer->getPosition();
	pos.z-=10;
	mat.setColumn(3,pos);
	pDecal->setTransform(mat);
	pDecal->setScale(Point3F(1,1,1));

	if(pDecal->getDataBlock())
	{
		if(!pDecal->registerObject())
		{
			delete pDecal;
			return false;
		}

		pDecal->triggerTerrainChange((SceneObject *)gClientSceneGraph->getCurrentTerrain());
	}
	else
	{
		delete pDecal;
		return false;
	}

	addToMissionGroup(pDecal);
	return true;
}

ConsoleFunction(removeDecal, bool , 1, 1, "removeDecal()" )
{
	SimObject *pObj = Sim::findObject("testDecal");
	if(pObj)
	{
		sgDecalProjector *pDecalProjector = (sgDecalProjector *)pObj;
		gProjectDecalManager->ageDecal(pDecalProjector->getDecalId());
		pObj->deleteObject();
	}

	return true;
}

ConsoleFunction(createLight, bool , 1, 1, "createLight()" )
{
	sgLightObject *pLight = new sgLightObject;
	pLight->assignName("testLight");
	pLight->setDataBlock( (GameBaseData*) Sim::findObject("defaultLight"));

	MatrixF mat(true);
	Point3F pos;
	GameConnection* conn = GameConnection::getConnectionToServer();
	Player *pPlayer = (Player*)conn->getControlObject();
	pos = pPlayer->getPosition();
	pos.z+=1;
	mat.setColumn(3,pos);
	pLight->setTransform(mat);
	pLight->setScale(Point3F(1,1,1));

	if(pLight->getDataBlock())
	{
		if(!pLight->registerObject())
		{
			delete pLight;
			return false;
		}
	}
	else
	{
		delete pLight;
		return false;
	}

	addToMissionGroup(pLight);
	return true;
}

ConsoleFunction(removeLight, bool , 1, 1, "removeLight()" )
{
	SimObject *pObj = Sim::findObject("testLight");
	if(pObj)
		pObj->deleteObject();

	return true;
}

ConsoleFunction(createSound, bool , 1, 1, "createSound()" )
{
	SFXEmitter *pSound = new SFXEmitter;
	pSound->assignName("testSound");
	pSound->setSoundId(StringTable->insert("A_man_003"));

	MatrixF mat(true);
	Point3F pos;
	GameConnection* conn = GameConnection::getConnectionToServer();
	Player *pPlayer = (Player*)conn->getControlObject();
	pos = pPlayer->getPosition();
	pos.z+=1;
	mat.setColumn(3,pos);
	pSound->setTransform(mat);
	pSound->setScale(Point3F(1,1,1));

	if(!pSound->registerObject())
	{
		delete pSound;
		return false;
	}

	addToMissionGroup(pSound);
	return true;
}

ConsoleFunction(removeSound, bool , 1, 1, "removeSound()" )
{
	SimObject *pObj = Sim::findObject("testSound");
	if(pObj)
		pObj->deleteObject();

	return true;
}

ConsoleFunction(createShape, bool , 1, 1, "createShape()" )
{
	TSSimpleShape *pShape = new TSSimpleShape;
	pShape->assignName("testShape");
	pShape->setShapeName(StringTable->insert("~/data/shapes/effect/chuansong.mod"));

	MatrixF mat(true);
	Point3F pos;
	GameConnection* conn = GameConnection::getConnectionToServer();
	Player *pPlayer = (Player*)conn->getControlObject();
	pos = pPlayer->getPosition();
	pos.z+=1;
	mat.setColumn(3,pos);
	pShape->setTransform(mat);
	pShape->setScale(Point3F(1,1,1));

	if(!pShape->registerObject())
	{
		delete pShape;
		return false;
	}

	addToMissionGroup(pShape);
	return true;
}
   
ConsoleFunction(removeShape, bool , 1, 1, "removeShape()" )
{
	SimObject *pObj = Sim::findObject("testShape");
	if(pObj)
		pObj->deleteObject();

	return true;
}

ConsoleFunction(createSpline, bool , 3, 3, "createSpline(%src, %tgt)" )
{
#if 0
//#ifdef NTJ_CLIENT
	SplineObject *pSpline = new SplineObject;

	GameBase* pSrc = dynamic_cast<GameBase*>(Sim::findObject(dAtoi(argv[1])));
	GameBase* pTgt = dynamic_cast<GameBase*>(Sim::findObject(dAtoi(argv[2])));
	if(!pSpline->Create(pSrc, pTgt, TM_IGNORE) || !pSpline->registerObject())
	{
		delete pSpline;
		return false;
	}

	pSpline->setFade(true);

	addToMissionGroup(pSpline);
	pSpline->assignName("testSpline");
#endif
	return true;
}

ConsoleFunction(removeSpline, bool , 1, 1, "removeSpline()" )
{
	SimObject *pObj = Sim::findObject("testSpline");
	if(pObj)
		pObj->deleteObject();

	return true;
}

ConsoleFunction(createElectricity, bool , 1, 1, "createElectricity()" )
{
	ElectricityData* pData = (ElectricityData*) Sim::findObject("ElectricityData_A");	//脚本data
	if (!pData)
		return false;
	SceneObject* pSrc = g_ClientGameplayState->GetControlPlayer();
	SceneObject* pTgt = pSrc ? g_ClientGameplayState->GetControlPlayer()->GetTarget() : NULL;
	// 目标和源缺一不可
	if(!pSrc || !pTgt)
		return false;

	Electricity* pElect = new Electricity();
	pElect->assignName("testElectricity");
	pElect->setLayerID(pSrc->getLayerID());
	pElect->setData(pData);
	pElect->setCarrier(pSrc, pTgt);
	if(!pElect->registerObject())
	{
		delete pElect;
		return false;
	}

	addToMissionGroup(pElect);
	return true;
}

ConsoleFunction(removeElectricity, bool , 1, 1, "removeElectricity()" )
{
	SimObject *pObj = Sim::findObject("testElectricity");
	if(pObj)
		pObj->deleteObject();

	return true;
}


#endif
