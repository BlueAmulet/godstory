//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "console/consoleInternal.h"
//#include "Gameplay/GameObjects/MonsterObjectData.h"
//#include "Gameplay/GameObjects/MonsterObject.h"
#include "Gameplay/GameObjects/NpcObjectData.h"
#include "Gameplay/GameObjects/CollectionObject.h"
#include "ts/TSShapeRepository.h"
#include "T3D/fx/cameraFXMgr.h"
#include "sceneGraph/decalManager.h"
#include "lightingSystem/synapseGaming/sgDynamicRangeLighting.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "gfx/gfxFontRenderBatcher.h"
#include "util/powerStat.h"
#include "GamePlay/EditorGameplayState.h"
#include "Effects/ScreenFX.h"
#include "T3D/fx/cameraFXMgr.h"

//��ҶϿ���ͼ��������֪ͨ
ConsoleFunction(SptOnDisconnect,void,1,1,"SptOnDisconnect()")
{
	g_EditorGameplayState->onDisconnect();
}

ConsoleFunction(SptOnEndMission,void,1,1,"SptOnEndMission()")
{
	g_EditorGameplayState->onDisconnect();
}

//////////////////////////////////////////////////////////////////////////
// NPC
//////////////////////////////////////////////////////////////////////////
ConsoleFunction(GetTNpcID, S32, 2,2, "GetTNpcID(index);")
{
	int index = dAtoi(argv[1]);
	return g_NpcRepository.GetNpcIdByIndex(index);
}
ConsoleFunction(GetNpcDataName,const char*,2,2,"GetNpcDataName(NpcID);")
{
	int NpcID = dAtoi(argv[1]);
	NpcObjectData* obj = g_NpcRepository.GetNpcData(NpcID);
	if(obj)
		return obj->npcName;
	else
		return NULL;
}
//
ConsoleFunction(GetNpcNum, int , 1, 1, "GetNpcNum();")
{
	return  g_NpcRepository.GetNpcDataCount();
}

ConsoleFunction(GetCollectionObjectNum, int , 1, 1, "GetNpcNum();")
{
	return  gCollectionRepository.GetDataCount();
}

ConsoleFunction(GetCollectionObjectDataName,const char*,2,2,"GetCollectionObjectDataName(CollectionObjectID);")
{
	int CollectionObjectID = dAtoi(argv[1]);
	CollectionObjectData* obj = gCollectionRepository.GetCollectionObjectData(CollectionObjectID);
	if(obj)
		return obj->mCollectionName;
	else
		return NULL;
}

ConsoleFunction(GetTCollectionObjectID, S32, 2,2, "GetTCollectionObjectID(index);")
{
	int index = dAtoi(argv[1]);
	return gCollectionRepository.GetCollectionIdByIndex(index);
}

ConsoleFunction(addcamerafx, void , 9, 9, "addcamerafx(a1,a2,a3,a4,a5,a6,a7,a8);")
{
	F32 Duration = dAtof(argv[1]);                                    //��Ч��ά��ʱ��
	VectorF mFreq;
	mFreq.x = dAtof(argv[2]);                                         //Ƶ��
	mFreq.y = dAtof(argv[3]);
	mFreq.z = dAtof(argv[4]);
	VectorF StartAmp;
	StartAmp.x = dAtof(argv[5]);                                      //����
	StartAmp.y = dAtof(argv[6]);
	StartAmp.z = dAtof(argv[7]);
	F32 Falloff = dAtof(argv[8]);                                     //˥��ֵ

	return gCamFXMgr.addFX(Duration, mFreq, StartAmp, Falloff);
}

ConsoleFunction(setglow, void , 6, 6, "setglow(a1,a2,a3,a4,a5);")
{
	F32 a1 = dAtof(argv[1]);                                        //������ɫǿ�ȣ�0-10��Ĭ��Ϊ1
	F32 a2 = dAtof(argv[2]);                                        //������ɫǿ��(2����������΢��)��0-10��Ĭ��Ϊ1.5
	F32 a3 = dAtof(argv[3]);                                        //ԭ����ɫ������0-1��Ĭ��Ϊ1
	F32 a4 = dAtof(argv[4]);                                        //����ͼ��ɫ������0-1��Ĭ��Ϊ1
	F32 a5 = dAtof(argv[5]);                                        //�����ο��ķ�Χ����ֵԽС��ΧԽ��0-1��Ĭ��Ϊ0.25
	return sgDRLSurfaceChain::SetGlowConstants(a1, a2, a3, a4, a5);
}

ConsoleFunction(setcamerashake, void , 5, 5, "setcamerashake(a1,a2,a3,a4);")
{
	F32 a1 = dAtof(argv[1]);                                        //��ͷ�ζ��ٶ� 21.0f;
	F32 a2 = dAtof(argv[2]);                                        //��ͷ�ζ���Χ 0.25f;
	F32 a3 = dAtof(argv[3]);                                        //��ͷ�ζ���Χ����        2.2f;
    F32 a4 = dAtof(argv[4]);                                        //�ζ�ʱ�� 0.0f;
	return gCameraShakeEX.SetConstants(a1,a2,a3,a4);
}

ConsoleFunction(showResource, void , 1, 1, "showResource();")
{
	CStat::Instance()->Print(0);
	CommonFontManager::PrintAllFont();
}

ConsoleFunction(enableGlow, void , 8, 8, "enableGlow(a1,a2,a3,a4,a5,a6,a7);")
{
	sgLightManager::sgUseDRLHighDynamicRange = true;
	sgLightManager::sgAllowDynamicRangeLighting = true;
	sgLightManager::sgAllowDRLHighDynamicRange = true;
	sgLightManager::sgAllowDRLBloom = true;
	sgLightManager::sgAllowDRLToneMapping = true;

	sgLightManager::sgDRLTarget = dAtof(argv[1]);
	sgLightManager::sgDRLMax = dAtof(argv[2]);
	sgLightManager::sgDRLMin = dAtof(argv[3]);
	sgLightManager::sgDRLMultiplier = dAtof(argv[4]);
	sgLightManager::sgBloomCutOff = dAtof(argv[5]);
	sgLightManager::sgBloomAmount = dAtof(argv[6]);
	sgLightManager::sgBloomSeedAmount = dAtof(argv[7]);
}

ConsoleFunction(disableGlow, void , 1, 1, "disableGlow();")
{
	sgLightManager::sgUseDRLHighDynamicRange = false;
	sgLightManager::sgAllowDynamicRangeLighting = false;
	sgLightManager::sgAllowDRLHighDynamicRange = false;
	sgLightManager::sgAllowDRLBloom = false;
	sgLightManager::sgAllowDRLToneMapping = false;
}


ConsoleFunction(setScreenMask,void,2,2, "setScreenMask(a1);")
{
	g_ScreenFXMgr.setScreenMask(argv[1]);
}

ConsoleFunction(addScreenFx,void,5,5, "addScreenFx(a1,a2,a3,a4);")
{
	CScreenFX* temp = new CScreenFX(argv[1], dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4]));
	g_ScreenFXMgr.Add(temp);
}