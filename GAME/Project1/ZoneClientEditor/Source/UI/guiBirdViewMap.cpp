//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "gfx/D3D9/gfxD3D9Device.h"
#include "T3D/gameConnection.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "gfx/primBuilder.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/ai/NaviGrid/NavigationBlock.h"
#include "ui/dGuiMouseGamePlay.h"
#include "Gameplay/Team/ClientTeam.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "guiBirdViewMap.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK
GFXStateBlock* guiBirdViewMap::mSetSB = NULL;
GFXStateBlock* guiBirdViewMap::mSetStencilSB = NULL;
GFXStateBlock* guiBirdViewMap::mStencilFalseSB = NULL;
GFXStateBlock* guiBirdViewMap::mClearSB = NULL;

IMPLEMENT_CONOBJECT(guiBirdViewMap);

guiBirdViewMap::guiBirdViewMap(void): m_Scale(1),
                                      m_MaxScale(1.3f),
                                      m_MinScale(0.7f),
                                      mRadius(0),
									  mPosTextRect(0,0,0,0),
                                      m_UpdateInterval(1000),
                                      m_LastUpdateTime(0),
                                      m_CurrentTime(0),
                                      m_CoverIconTexture(NULL)									 
{
	m_CoverIconFileName     = StringTable->insert("");
	memset(&localTime,0,sizeof(Platform::LocalTime));
}
guiBirdViewMap::~guiBirdViewMap()
{
	m_CoverIconTexture.free();
}

void guiBirdViewMap::onPreRender()
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(NULL == pPlayer)
		return;

	Parent::onPreRender();

	if(updateData())
	{ 
		Platform::LocalTime currentTime;
		g_ClientGameplayState->getGameFormatTime(currentTime);
		if(currentTime.min != localTime.min)
		{
			localTime = currentTime;
			Con::executef("UpdateMapTime",avar("%02d:%02d",localTime.hour,localTime.min));
		}
	}

	if (g_ClientGameplayState->isStartNavigation() &&  pPlayer->m_pAI->GetAIPath()->GetPathList().size() >0)
	{
		static GuiControl* ctrl = dynamic_cast<GuiControl*>(Sim::findObject("SeekLoadDialogWnd"));
		if(ctrl && !ctrl->isVisible())
		{
			Point2I pos2D = map2DToFixed2D(Point2F(m_DestPos.x,m_DestPos.y));
			if(pos2D.isZero())
				return;

			Con::executef("OpenFindPathDialog",
				Con::getIntArg(mCurrentMapId),
				Con::getIntArg(pos2D.x),
				Con::getIntArg(pos2D.y));
		}
	}
}

void guiBirdViewMap::onRender(Point2I offset, const RectI &updateRect)
{
	if(m_TerrainTexture.isNull() || m_CoverIconTexture.isNull())
		return;

	Player* control_Player = g_ClientGameplayState->GetControlPlayer();
	if(!control_Player)
		return;

	m_TerrainPlayer = getTerrainPlayerScale(mTerrPos3D,control_Player->getPosition());
	if(!m_TextureRect.contains(m_TerrainPlayer))
		return;		// 超出地图


	F32 delRatX = mRadius + offset.x;
	F32 delRatY = mRadius + offset.y;

#ifdef STATEBLOCK
	AssertFatal(mSetSB, "guiBirdViewMap::onRender -- mSetSB cannot be NULL.");
	mSetSB->apply();
#else
	GFX->setAlphaTestEnable(true);
	GFX->setAlphaRef(1);
	GFX->setAlphaFunc(GFXCmpGreaterEqual);
	GFX->setStencilEnable(true);
	GFX->setStencilRef(100);
	GFX->setStencilPassOp( GFXStencilOpReplace);
	GFX->setStencilFunc(GFXCmpAlways);
	GFX->setTextureStageMipFilter( 0, GFXTextureFilterLinear );
	GFX->setTextureStageMinFilter( 0, GFXTextureFilterLinear );
	GFX->setTextureStageMagFilter( 0, GFXTextureFilterLinear );
#endif


	RectI coverRect;
	coverRect.set(offset.x,offset.y,m_CoverIconTexture.getWidth(),m_CoverIconTexture.getHeight());
	GFX->getDrawUtil()->clearBitmapModulation();
	GFX->getDrawUtil()->drawBitmapStretch(m_CoverIconTexture,coverRect);

	Point2F mapPos2D = convert3DposToMap2D(mCurrentMapId,control_Player->getPosition());
	Point2F playerPos = mapPos2D * m_Scale;
	F32 screenLeft = delRatX - playerPos.x;
	F32 screenTop  = delRatY - playerPos.y;
	RectF terrMapRect(screenLeft,screenTop,mMapExtent.x*m_Scale,mMapExtent.y*m_Scale);
#ifdef STATEBLOCK
	AssertFatal(mSetStencilSB, "guiBirdViewMap::onRender -- mSetStencilSB cannot be NULL.");
	mSetStencilSB->apply();
#else
	GFX->setStencilFunc(GFXCmpEqual);
	GFX->setStencilRef(100);
#endif

	//画地形
	GFX->getDrawUtil()->clearBitmapModulation();
	GFX->getDrawUtil()->drawBitmapStretch(m_TerrainTexture, terrMapRect);
	//画寻径路径
	drawPath(terrMapRect);

#ifdef STATEBLOCK
	AssertFatal(mStencilFalseSB, "guiBirdViewMap::onRender -- mStencilFalseSB cannot be NULL.");
	mStencilFalseSB->apply();
#else
	GFX->setStencilEnable(false);
	GFX->setTextureStageMagFilter(0, GFXTextureFilterPoint);
	GFX->setTextureStageMinFilter(0, GFXTextureFilterPoint);
	GFX->setTextureStageMipFilter(0, GFXTextureFilterNone);
#endif


	Point3F tempVerts[4];
	Point2F tempPos;

	//画自己
	Point2F playerSelf_verts[4];
	Point2F playerSelf_texCoord[4];
	Point3F roation = control_Player->getRotation();
	roation.z = -roation.z;
	roation.z -= M_2PI_F/4; 
	MatrixF  rotMatrix(roation);
	playerSelf_texCoord[0].set(0,0);
	playerSelf_texCoord[1].set(0,1);
	playerSelf_texCoord[2].set(1,1);
	playerSelf_texCoord[3].set(1,0);
	tempPos.set(m_SelfIconTexture.getWidth() * 0.5f,m_SelfIconTexture.getHeight() * 0.5f);
	tempVerts[0].set(-tempPos.x,tempPos.y,0.0f);
	tempVerts[1].set(-tempPos.x,-tempPos.y,0.0f);
	tempVerts[2].set(tempPos.x,-tempPos.y,0.0f);
	tempVerts[3].set(tempPos.x,tempPos.y,0.0f);
	for(U32 i=0;i<4;i++)
		rotMatrix.mulP(tempVerts[i]);

	for (U32 i=0; i<4; i++)
		playerSelf_verts[i].set(tempVerts[i].x+delRatX,tempVerts[i].y+delRatY);

	//画摄像机视野
	Point2F camera_TexCoord[4];
	Point2F camera_Verts[4];
	EulerF camera_roation(0.0f,0.0f,0.0f);
	camera_roation.z = control_Player->getRotation().z+g_UIMouseGamePlay->GetCameraYaw();
	camera_roation.z = - camera_roation.z;
	camera_roation.z += M_2PI_F/4;
	MatrixF camera_RotMat(camera_roation);
	camera_TexCoord[0].set(0,0);
	camera_TexCoord[1].set(0,1);
	camera_TexCoord[2].set(1,1);
	camera_TexCoord[3].set(1,0);
	tempPos.set(m_CameraImageTexture.getWidth() * 0.5f,m_CameraImageTexture.getHeight() * 0.5f);
	tempVerts[0].set(-tempPos.x,tempPos.y,0.0f);
	tempVerts[1].set(-tempPos.x,-tempPos.y,0.0f);
	tempVerts[2].set(tempPos.x,-tempPos.y,0.0f);
	tempVerts[3].set(tempPos.x,tempPos.y,0.0f);
	for(U32 i=0; i<4; i++)
		camera_RotMat.mulP(tempVerts[i]);

	for(U32 i=0; i<4; i++)
		camera_Verts[i].set(tempVerts[i].x+delRatX,tempVerts[i].y+delRatY);

	if (m_DisplayAll)
	{
		//显示怪物,NPC 玩家
		typedef stdext::hash_map<U32,GameObject*> ObjMap;
		ObjMap::iterator itr = g_ClientGameplayState->getObjectList().begin();
		for (; itr != g_ClientGameplayState->getObjectList().end(); ++itr)
		{
			GameObject* pObj = itr->second;
			if(NULL == pObj || pObj->getVisibility() < 0.2f || pObj == (GameObject*)control_Player)
				continue;

			//因该绘制方法是绘制到小地图圆内,所以不能用下方法二,如果绘制到目标贴图则可以使用.

			//方法一
			Point2F objPos2F = getTerrainPlayerScale(mTerrPos3D,pObj->getPosition());
			// x,y任意取值计算. 映射到小地图
			Point2F distancePos = (objPos2F - m_TerrainPlayer) / m_TextureRect.extent.y * m_TextureAvailableRect.extent.y * m_TerrainTexture.getHeight() * m_Scale;
			// 当前对象与自己的间距
			F32 distance = distancePos.len();
			RectI destRect(distancePos.x + delRatX, distancePos.y +delRatY,0,0);

			if(pObj->getGameObjectMask() & NpcObjectType)
			{
				NpcObject* Npc = dynamic_cast<NpcObject*>(pObj);
				if(Npc)
				{
					if(!Npc->isCombative())
					{
						if (m_DisplayNpc && m_NpcIconTexture .isValid()
							&& distance < (mRadius - m_NpcIconTexture.getWidth()*0.5f))
							drawBitmap(m_NpcIconTexture,destRect);
					}
					else if (m_DisplayMonsters && m_MonsterIconTexture.isValid()
						&& distance < (mRadius - m_MonsterIconTexture.getWidth()*0.5f))
						drawBitmap(m_MonsterIconTexture,destRect);
				}
			}

			if((pObj->getGameObjectMask() & PlayerObjectType) && m_DisplayPlayers)
			{
				Player* player=dynamic_cast<Player*>(pObj);
				if(player && control_Player->getPlayerID() != player->getPlayerID())
				{
					if (control_Player->getTeamInfo().m_bInTeam && CLIENT_TEAM->IsTeammate(player->getPlayerID()) 
						&& m_TeammateIconTexture.isValid() && distance < (mRadius - m_TeammateIconTexture.getWidth()*0.5f))
						drawBitmap(m_TeammateIconTexture,destRect);

					else if(m_PlayerIconTexture.isValid() && distance < (mRadius - m_TeammateIconTexture.getWidth()*0.5f))
						drawBitmap(m_PlayerIconTexture,destRect);

				}
			}
		}
	}

	GFX->setBaseRenderState();
#ifdef STATEBLOCK
	AssertFatal(mClearSB, "guiBirdViewMap::onRender -- mClearSB cannot be NULL.");
	mClearSB->apply();
#else
	GFX->setCullMode( GFXCullNone );
	GFX->setLightingEnable( false );
	GFX->setAlphaBlendEnable( true );
	GFX->setSrcBlend( GFXBlendSrcAlpha );
	GFX->setDestBlend( GFXBlendInvSrcAlpha );
	GFX->setTextureStageColorOp( 0, GFXTOPModulate );
	GFX->setTextureStageColorOp( 1, GFXTOPDisable );
	GFX->setTextureStageAddressModeU(0,GFXAddressBorder);
	GFX->setTextureStageAddressModeV(0,GFXAddressBorder);
#endif

	ColorI colorStore = PrimBuild::getColor();
	PrimBuild::color3i(255, 255, 255);
	//摄像机
	if(m_CameraImageTexture.isValid())
	{
		GFX->setTexture(0,m_CameraImageTexture);
		PrimBuild::begin(GFXTriangleFan,4);
		for (U32 i=0; i<4; i++)
		{
			PrimBuild::texCoord2f(camera_TexCoord[i].x,camera_TexCoord[i].y);
			PrimBuild::vertex2f(camera_Verts[i].x,camera_Verts[i].y);
		}
		PrimBuild::end();
	}
	//自己
	if (m_SelfIconTexture.isValid())
	{
		GFX->setTexture( 0,m_SelfIconTexture );
		PrimBuild::begin(GFXTriangleFan,4);
		for (S32 i=0; i<4; i++)
		{
			PrimBuild::texCoord2f(playerSelf_texCoord[i].x, playerSelf_texCoord[i].y);
			PrimBuild::vertex2f(playerSelf_verts[i].x, playerSelf_verts[i].y);
		}
		PrimBuild::end();
	}
	PrimBuild::color(colorStore);

	//玩家自身坐标(0~100)
	Point2I fixedPos2D = map2DToFixed2D(mapPos2D);
	if(fixedPos2D.isZero())
		return;

	char szTextPos[32] = {0,};
	dSprintf(szTextPos,sizeof(szTextPos),"[%d,%d]",fixedPos2D.x,fixedPos2D.y);
	renderJustifiedText(offset + mPosTextRect.point,mPosTextRect.extent,szTextPos);
}

void guiBirdViewMap::onMouseDown(const GuiEvent &event)
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if (NULL == player)
		return;

	Point2I local_Position = globalToLocalCoord(event.mousePoint);
	//圆内有效距离
	F32 distance = mSqrt(mPow(local_Position.x - mRadius,2)+mPow(local_Position.y - mRadius,2));
	if ( distance < mRadius)
	{
		g_ClientGameplayState->setDarwPath(true);
		Point3F destPos = convertMapPosTo3D(mCurrentMapId,local_Position);
		player->SetTarget(destPos);	
	}
}

bool guiBirdViewMap::onWake()
{
	if(!Parent::onWake())
		return false;

	initTexture();
	if(mRadius == 0)
	{
		Point2I extent = getExtent();
		mRadius = getMin(extent.x,extent.y) * 0.5f;
	}
	return true; 
}

void guiBirdViewMap::onSleep()
{
	Parent::onSleep();
	m_CoverIconTexture.free();
	m_TargetCover.free();
}

void guiBirdViewMap::initTexture()
{
	Parent::initTexture();

	if(m_CoverIconFileName && m_CoverIconFileName[0])
		m_CoverIconTexture.set(m_CoverIconFileName,&GFXDefaultGUIProfile);
}

bool guiBirdViewMap::updateData()
{
	m_CurrentTime = Platform::getVirtualMilliseconds();
	if(m_CurrentTime - m_LastUpdateTime < m_UpdateInterval)
		return false;

	m_LastUpdateTime = m_CurrentTime;
	return true;
}

void guiBirdViewMap::initPersistFields()
{
	Parent::initPersistFields();
	addGroup("guiBirdViewMap");
	addField("Scale",                   TypeF32,                 Offset(m_Scale,                guiBirdViewMap));
	addField("MaxScale",                TypeF32,                 Offset(m_MaxScale,             guiBirdViewMap));
	addField("MinScale",                TypeF32,                 Offset(m_MinScale,             guiBirdViewMap));
	addField("UpdateInterval",          TypeS32,                 Offset(m_UpdateInterval,       guiBirdViewMap));
	addField("MyselfPosTextRect",       TypeRectI,               Offset(mPosTextRect,           guiBirdViewMap));
	addField("CoverIconFileName",       TypeString,              Offset(m_CoverIconFileName,    guiBirdViewMap));
	endGroup("guiBirdViewMap");
}

Point3F guiBirdViewMap::convertMapPosTo3D(S32 mapId,Point2I pos2D)
{
	Point3F pos3D(0,0,0);
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(player)
	{
		F32 mTexU = (F32)pos2D.x / (mRadius * 2);
		F32 mTexV = (F32)pos2D.y / (mRadius * 2);

		//X,Y可以任意取一值计算.注:圆在图片所占的百分比,换算到地形中的百分比
		F32 unitsValue = (mRadius * 2)/(m_TextureAvailableRect.extent.y*m_TerrainTexture.getHeight()) * (m_TextureRect.extent.y * MAP_WIDTH) / m_Scale;
		pos3D.x = (mTexU-0.5) * unitsValue + player->getPosition().x;
		pos3D.y = (0.5-mTexV) * unitsValue  + player->getPosition().y;
		pos3D   = g_NavigationManager->SlerpPosition(pos3D);
	}
	return pos3D;
}

void guiBirdViewMap::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSAlphaRef, 1);
	GFX->setRenderState(GFXRSAlphaFunc, GFXCmpGreaterEqual);
	GFX->setRenderState(GFXRSStencilEnable, true);
	GFX->setRenderState(GFXRSStencilRef, 100);
	GFX->setRenderState(GFXRSStencilPass, GFXStencilOpReplace);
	GFX->setRenderState(GFXRSStencilFunc, GFXCmpAlways);
	GFX->setSamplerState(0, GFXSAMPMagFilter, GFXTextureFilterLinear);
	GFX->setSamplerState(0, GFXSAMPMinFilter, GFXTextureFilterLinear);
	GFX->setSamplerState(0, GFXSAMPMipFilter, GFXTextureFilterLinear);
	GFX->endStateBlock(mSetSB);

	//mSetStencilSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSStencilRef, 100);
	GFX->setRenderState(GFXRSStencilFunc, GFXCmpEqual);
	GFX->endStateBlock(mSetStencilSB);

	//mStencilFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSStencilEnable, false);
	GFX->endStateBlock(mStencilFalseSB);

	//mClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressBorder);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressBorder);
	GFX->endStateBlock(mClearSB);
}


void guiBirdViewMap::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}

	if (mSetStencilSB)
	{
		mSetStencilSB->release();
	}

	if (mStencilFalseSB)
	{
		mStencilFalseSB->release();
	}

	if (mClearSB)
	{
		mClearSB->release();
	}
}

void guiBirdViewMap::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		mSetStencilSB = new GFXD3D9StateBlock;
		mStencilFalseSB = new GFXD3D9StateBlock;
		mClearSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}

void guiBirdViewMap::shutdown()
{
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mSetStencilSB);
	SAFE_DELETE(mStencilFalseSB);
	SAFE_DELETE(mClearSB);
}
