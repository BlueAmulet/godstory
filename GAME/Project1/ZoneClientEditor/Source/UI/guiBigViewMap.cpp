//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include <d3dx9math.h>
#include "platform/platform.h"
#include "gfx/D3D9/gfxD3D9Device.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "gfx/primBuilder.h"
#include "gfx/gfxFontRenderBatcher.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gui/controls/guiPopUpCtrl.h"
#include "gui/controls/guiTreeViewCtrl.h"
#include "T3D/gameConnection.h"
#include "ui/dGuiMouseGamePlay.h"
#include "Gameplay/ai/NaviGrid/NavigationBlock.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/Team/ClientTeam.h"
#include "Gameplay/Data/RouteRepository.h"
#include "guiBigViewMap.h"

//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK
GFXStateBlock* GuiBigViewMap::mSetSB = NULL;

IMPLEMENT_CONOBJECT(GuiBigViewMap);
GuiBigViewMap::GuiBigViewMap(): mPlatformWindow(NULL),
                                mTextureRectOffset(0,0),
								mMouseFixedPos(0,0),
								mLocalMousePos(0,0)
{
	//m_MarkIconTexture   = NULL;
	//m_MarkIconFileName = StringTable->insert("");
}
GuiBigViewMap::~GuiBigViewMap()
{
	//m_MarkIconTexture.free();
}

void GuiBigViewMap::onRender(Point2I offset, const RectI &updateRect)
{
	if(m_TerrainTexture.isNull())
		return;

	Player* control_Player = g_ClientGameplayState->GetControlPlayer();
	if(NULL == control_Player)
		return;

	char buff[COMMON_STRING_LENGTH] = {0,};
	bool bDrawName = false;
	ColorI color = mProfile->mFontColor;
	Point2I mousePos = mLocalMousePos + offset; 

	//画地形
	RectF terrMapRect(offset.x,offset.y,mMapExtent.x,mMapExtent.y);
	GFX->getDrawUtil()->clearBitmapModulation();
	GFX->getDrawUtil()->drawBitmapStretch(m_TerrainTexture, terrMapRect);

	//绘当前地图信息
	if(mCurrentMapId == mSelectMapId)
	{
		Point3F playerPos3D = control_Player->getPosition();
		m_TerrainPlayer = getTerrainPlayerScale(mTerrPos3D,playerPos3D);
		if(!m_TextureRect.contains(m_TerrainPlayer))
			return;		// 超出地图

		Point2F playerPos = convert3DposToMap2D(mCurrentMapId,playerPos3D);
		playerPos.x += offset.x;
		playerPos.y += offset.y;

		Point2F tempPos;
		Point3F tempVerts[4];
		Point2I extent = getExtent();

		//画寻径路径
		drawPath(terrMapRect);
		//画自己
		Point3F playerSelf_verts[4];
		Point2F playerSelf_texCoord[4];
		playerSelf_texCoord[0].set(0,0);
		playerSelf_texCoord[1].set(0,1);
		playerSelf_texCoord[2].set(1,1);
		playerSelf_texCoord[3].set(1,0);
		Point3F roation = control_Player->getRotation();
		roation.z=-roation.z;
		roation.z-=M_2PI_F/4; 
		MatrixF  rotMatrix(roation);
		tempPos.set(m_SelfIconTexture.getWidth()/2,m_SelfIconTexture.getHeight()/2);
		playerSelf_verts[0].set(-tempPos.x,tempPos.y,0.0f);
		playerSelf_verts[1].set(-tempPos.x,-tempPos.y,0.0f);
		playerSelf_verts[2].set(tempPos.x,-tempPos.y,0.0f);
		playerSelf_verts[3].set(tempPos.x,tempPos.y,0.0f);

		for(U32 i=0;i<4;i++)
			rotMatrix.mulP(playerSelf_verts[i]);

		for (U32 i =0; i<4;i++)
			playerSelf_verts[i].set(playerSelf_verts[i].x+playerPos.x,playerSelf_verts[i].y+playerPos.y,playerSelf_verts[i].z);


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
			camera_Verts[i].set(tempVerts[i].x+playerPos.x,tempVerts[i].y+playerPos.y);

		//显示怪物,NPC 玩家
		if (m_DisplayAll)
		{
			typedef stdext::hash_map<U32,GameObject*> ObjMap;
			ObjMap::iterator itr = g_ClientGameplayState->getObjectList().begin();
			for (; itr != g_ClientGameplayState->getObjectList().end(); ++itr)
			{
				GameObject* pObj = itr->second;
				if(NULL == pObj || pObj->getVisibility() < 0.2f || pObj == (GameObject*)control_Player)
					continue;

				Point2F tempPos = convert3DposToMap2D(mCurrentMapId,pObj->getPosition());
				RectI destRect(tempPos.x+offset.x,tempPos.y+offset.y,0,0);

				GFX->getDrawUtil()->clearBitmapModulation();

				if(pObj->getGameObjectMask() & NpcObjectType )
				{
					if (!pObj->isCombative())
					{
						if (m_DisplayNpc && m_NpcIconTexture.isValid())
							drawBitmap(m_NpcIconTexture,destRect);

					}
					else if (m_DisplayMonsters && m_MonsterIconTexture.isValid())
						drawBitmap(m_MonsterIconTexture,destRect);

				}
				if(pObj->getGameObjectMask() & PlayerObjectType && m_DisplayPlayers)
				{
					Player* playerObj=dynamic_cast<Player*>(pObj);
					if(playerObj && playerObj->getPlayerID() !=control_Player->getPlayerID())
					{
						if (control_Player->getTeamInfo().m_bInTeam && CLIENT_TEAM->IsTeammate(playerObj->getPlayerID()) 
							&& m_TeammateIconTexture.isValid())
							drawBitmap(m_TeammateIconTexture,destRect);

						else if(m_PlayerIconTexture.isValid())
							drawBitmap(m_PlayerIconTexture,destRect);
					}
				}

				//绘名字
				if(destRect.pointInRect(mousePos))
				{
					bDrawName = true;
					renderText(mousePos,pObj->getObjectName(),color);
				}
			}
		}
		GFX->setBaseRenderState();
#ifdef STATEBLOCK
		AssertFatal(mSetSB, "GuiBigViewMap::onRender -- mSetSB cannot be NULL.");
		mSetSB->apply();
#else
		GFX->setCullMode( GFXCullNone );
		GFX->setLightingEnable( false );
		GFX->setAlphaBlendEnable( true );
		GFX->setSrcBlend( GFXBlendSrcAlpha );
		GFX->setDestBlend( GFXBlendInvSrcAlpha );
		GFX->setTextureStageColorOp( 0, GFXTOPModulate );
		GFX->setTextureStageColorOp( 1, GFXTOPDisable );
#endif
		GFX->setTexture( 0, m_TerrainTexture );
		GFX->setTexture( 1, NULL);
		ColorI colorStore = PrimBuild::getColor();
		PrimBuild::color3i(255, 255, 255);
		//摄像机	      
		if (m_CameraImageTexture.isValid())
		{
			GFX->setTexture(0,m_CameraImageTexture);
			PrimBuild::begin(GFXTriangleFan,4);
			for (S32 i=0; i<4;i++)
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
	}

	//绘鼠标指向坐标
	if(!mMouseFixedPos.isZero() && !bDrawName)
	{
		dSprintf(buff,sizeof(buff),"%d,%d",mMouseFixedPos.x,mMouseFixedPos.y);
		renderText(mousePos,buff,mProfile->mFontColor);
	}

	renderChildControls(offset, updateRect);
}

void GuiBigViewMap::onMouseDown(const GuiEvent &event)
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(NULL == pPlayer)
		return;

	Point2I local_Position = globalToLocalCoord(event.mousePoint);
	if (m_PathAvailableRect.pointInRect(local_Position))
	{
		Point3F destPos = convertMapPosTo3D(mSelectMapId,local_Position);
		if(mCurrentMapId != mSelectMapId)
			moveToPos3D(mSelectMapId,destPos,mMouseFixedPos);
		else
			g_ClientGameplayState->setDarwPath(true);

		pPlayer->SetTarget(destPos);
	}
}

void GuiBigViewMap::onMouseLeave(const GuiEvent &event)
{
	mLocalMousePos.set(0,0);
	mMouseFixedPos.set(0,0);
}

void GuiBigViewMap::onMouseMove(const GuiEvent &event)
{
	mLocalMousePos = globalToLocalCoord(event.mousePoint);
	if (m_PathAvailableRect.pointInRect(mLocalMousePos))
		mMouseFixedPos =  map2DToFixed2D(Point2F(mLocalMousePos.x,mLocalMousePos.y));
	else
		mMouseFixedPos.set(0,0);
}

bool GuiBigViewMap::onAdd()
{
	if(!Parent::onAdd())
		return false;

	mPlatformWindow = WindowManager->getFirstWindow();
	mPlatformWindow->resizeEvent.notify(this,&GuiBigViewMap::handleResize);
	return true;
}

void GuiBigViewMap::handleResize(DeviceId did, S32 width, S32 height)
{
	//因缩放按指定比例缩放,如(800*600,1024*768等),所以不考虑其它情况
	if(width < 1024 && height < 768)
		Con::executef("ResizeMap",Con::getIntArg(1));
	else
		Con::executef("ResizeMap",Con::getIntArg(0));
}

bool GuiBigViewMap::onWake()
{
	if(!Parent::onWake())
		return false;

	viewMapData* pData =  g_viewMapRepository.getViewMapData(mSelectMapId);
	if(pData)
	{
		initTexture();
		mTextureRectOffset = pData->getProjectOffset();
	}

	return true;
}

void GuiBigViewMap::onSleep()
{
	Parent::onSleep();
	//m_MarkIconTexture.free();
	
}

void GuiBigViewMap::initTexture()
{
	Parent::initTexture();
	//if (m_MarkIconFileName)
	//	m_MarkIconTexture.set(m_MarkIconFileName,&GFXDefaultGUIProfile);
}

void GuiBigViewMap::setSelectMap(S32 mapId)
{
	viewMapData* pData =  g_viewMapRepository.getViewMapData(mapId);
	if(NULL == pData)
		return;

	m_TextureRect = pData->getProjectRect();
	pData->getTerrainPos(mTerrPos3D);
	mSelectMapId =  mapId;
	mTextureRectOffset = pData->getProjectOffset();

	char szViewMap_File[256] = {0,};
	dSprintf(szViewMap_File,sizeof(szViewMap_File),GUIMAPIMAGE,pData->getMapFileName());
	m_TerrainTexture.set(szViewMap_File,&GFXDefaultGUIProfile);
}

Point3F GuiBigViewMap::convertMapPosTo3D(S32 mapId,Point2I pos2D)
{
	Point3F pos3D(0,0,0);
	RectF rect;
	Point2F offset;

	if(mapId != mSelectMapId)
	{
		viewMapData* pData = g_viewMapRepository.getViewMapData(mapId);
		if(NULL == pData)
			return pos3D;

		rect   = pData->getProjectRect();
		offset = pData->getProjectOffset();
	}
	else
	{
		rect   = m_TextureRect;
		offset = mTextureRectOffset;
	}
	

	Point2I extent = getExtent();
	F32 mTexU= (F32)pos2D.x / (F32)extent.x;
	F32 mTexV= (F32)pos2D.y / (F32)extent.y;
	//地形纹理(0~1)，按坐标轴第4象限转换(X,-Y),也就是屏幕坐.注:左上角为(0,0)。
	pos3D.x = (offset.x + (rect.point.x + mTexU * rect.extent.x)) * MAP_WIDTH;
	pos3D.y = (offset.y - (rect.point.y + mTexV * rect.extent.y)) * MAP_WIDTH;
	pos3D   = g_NavigationManager->SlerpPosition(pos3D);
	return pos3D;
}

void GuiBigViewMap::moveToPos2D(S32 mapId,Point2I DestPos)
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(pPlayer)
	{
		g_ClientGameplayState->setDarwPath(true);
		Point2I tempPos; //因寻径在(0,0,1024,768贴图)上,所以此处不需加控件偏移
		tempPos.x = DestPos.x / POS2D_WIDTH * m_PathAvailableRect.extent.x + m_PathAvailableRect.point.x;
		tempPos.y = DestPos.y / POS2D_WIDTH * m_PathAvailableRect.extent.y + m_PathAvailableRect.point.y;
		Point3F destPos3D = convertMapPosTo3D(mapId,tempPos);
		moveToPos3D(mapId,destPos3D,DestPos);
		pPlayer->SetTarget(destPos3D);
	}
}

void GuiBigViewMap::moveToPos3D(S32 mapId,Point3F &pos3D,Point2I pos2D)
{
	//899999999大地图跨服专用ID
	RouteData* pData =  g_RouteRepository.getRouteData(899999999);
	if(NULL == pData)
	{
		AssertFatal(false,"Cannt find routeId 899999999");
		return;
	}
	char buff[32] = {0,};
	dSprintf(buff,sizeof(buff),"8%d0100",mapId);
	pData->mMapID = dAtoi(buff);
	pData->mPositionVec[0] = pos3D;

	dSprintf(buff,sizeof(buff),"%d,%d",pos2D.x,pos2D.y);
	pData->mTextInfo = StringTable->insert(buff);
	pos3D = getPath_Position(pData->mRouteID);
}

void GuiBigViewMap::initPersistFields()
{
	Parent::initPersistFields();
	addGroup("GuiBigViewMap");
	//addField("MarkIconFileName",    TypeString,    Offset(m_MarkIconFileName,     GuiBigViewMap));

	endGroup("GuiBigViewMap");
}

void GuiBigViewMap::renderText(Point2I offset,const char* text,ColorI &color)
{
	if(dStrcmp(text,"") == 0)
		return;

	offset.x += 10;
	offset.y -= 20;
	
	Point2I extent(4,4);
	Point2I start(2,2);
	CommonFontEX* pFont = mProfile->mFont;
	//文字是否描边
	if(mProfile->mFontOutline != ColorI(0,0,0))
	{
		extent.x += pFont->getStrWidthOutline((UTF8*)text);
		extent.y += pFont->getHeightOutline();
	}
	else
	{
		extent.x += pFont->getStrWidth((UTF8*)text);
		extent.y += pFont->getHeight();
	}
	
	RectI rect(offset,extent);
	if (mProfile->mBorder)
		renderBorder(rect, mProfile);

	if(mProfile->mFontOutline != ColorI(0,0,0))
		GFX->getDrawUtil()->drawTextOutline(pFont, start + offset, text, &color,true,mProfile->mFontOutline);
	else
		 GFX->getDrawUtil()->drawText( pFont, start + offset, text, &color );
}

ConsoleMethod(GuiBigViewMap,updateBigMapUI,void,2,3,"obj.updateBigMapUI(%name)")
{
	static GuiPopUpMenuCtrl* ctrlPopUpMenu = dynamic_cast<GuiPopUpMenuCtrl*>(Sim::findObject("BigMapMenu_DisplayMainland"));
	static GuiPopUpMenuCtrl* ctrlPopUpMenu1 = dynamic_cast<GuiPopUpMenuCtrl*>(Sim::findObject("BigMapMenu_DisplayScene"));
	if(NULL ==  ctrlPopUpMenu || NULL == ctrlPopUpMenu1)
		return;

	ctrlPopUpMenu1->clear();
	viewMapRepository::viewMap_Map& mapData = g_viewMapRepository.getData();
	viewMapRepository::viewMap_Map::iterator it =  mapData.begin();

	if(argc == 2)
	{
		S32 selectMapId = object->getSelectMapId();
		viewMapData* pSelectData = g_viewMapRepository.getViewMapData(selectMapId);
		if(NULL == pSelectData)
			return;

		for(; it != mapData.end(); it++)
		{
			viewMapData* pData = it->second;
			if(NULL ==  pData || dStrcmp(pSelectData->getMapArea(),pData->getMapArea()))
				continue;

			ctrlPopUpMenu1->addEntry(pData->getMapName(),pData->getMapId());
		}

		S32 itemId = ctrlPopUpMenu->findText(pSelectData->getMapArea());
		if(itemId == -1)
			return;

		ctrlPopUpMenu1->sortID();
		ctrlPopUpMenu->setSelected(itemId,false);
		ctrlPopUpMenu1->setSelected(selectMapId,false);
		Con::executef(object,"queryBigMapInfo","0");
	}

	if(argc == 3)
	{
		for(; it != mapData.end(); it++)
		{
			viewMapData* pData = it->second;
			if(NULL ==  pData || dStrcmp(argv[2],pData->getMapArea()))
				continue;

			ctrlPopUpMenu1->addEntry(pData->getMapName(),pData->getMapId());
		}

		ctrlPopUpMenu1->sortID();
		ctrlPopUpMenu1->onAction();
	}	
}

ConsoleMethod(GuiBigViewMap,moveToPos2D,void,4,5,"obj.moveToPos2D(%point.x,%point.y,%mapId)")
{

	Point2I destPos;
	destPos.x = dAtoi(argv[2]);
	destPos.y = dAtoi(argv[3]);
	if(destPos.x<0 || destPos.x>100 || destPos.y<0 || destPos.y>100)
		return;

	if(argc == 4)
		object->moveToPos2D(object->getSelectMapId(),destPos);
	else
		object->moveToPos2D(dAtoi(argv[4]),destPos);
}

ConsoleMethod(GuiBigViewMap,setSelectBigMap,void,3,3,"object.setSelectBigMap(%mapId)")
{
	object->setSelectMap(dAtoi(argv[2]));
}

ConsoleMethod(GuiBigViewMap,resetSelectMap,void,2,2,"obj.resetSelectMap()")
{
	if(object->getCurrentMapId() != object->getSelectMapId())
	{
		object->setSelectMap(object->getCurrentMapId());
		Con::executef(object,"clearQueryInfo");
	}
}

ConsoleMethod(GuiBigViewMap,queryBigMapInfo,void,3,3,"object.queryBigMapInfo(%type)")
{
	U8 itype = dAtoi(argv[2]);
	static GuiTreeViewCtrl* ctrlTree = dynamic_cast<GuiTreeViewCtrl*>(Sim::findObject("BigViewMapTree"));
	static GuiPopUpMenuCtrl* ctrlPopUpMenu = dynamic_cast<GuiPopUpMenuCtrl*>(Sim::findObject("BigMapMenu_SeekLoadList"));
	if(NULL == ctrlTree || NULL == ctrlPopUpMenu)
		return;

	if(object->getCurrentMapId() == object->getSelectMapId() && itype == 0 && ctrlTree->getItemCount() >0 )
		return;

	//清除原有的内容
	ctrlTree->clearItem();
	if(itype == 0)
	{
		ctrlPopUpMenu->closePopUp();
		ctrlPopUpMenu->clear();
		ctrlPopUpMenu->addEntry("全部显示",0);
	}

	RouteData* pData = NULL;
	char szNodeId[8] = {0,};
	S32 mapId = object->getSelectMapId();
	RouteRepository::RouteMap& pTabData =  g_RouteRepository.getData();
	RouteRepository::RouteMap::iterator it =  pTabData.begin();
	for (; it != pTabData.end(); it++)
	{
		pData =  it->second;
		if(NULL == pData || pData->mCategoryType == 0 || mapId != g_RouteRepository.getMapID(pData->mMapID))
			continue;

		if(itype ==0 || itype == pData->mCategoryType)
		{
			dSprintf(szNodeId,sizeof(szNodeId),"%d",pData->mCategoryType);
			S32 subId = ctrlTree->findMissionTreeNode(szNodeId);
			if(subId == 0)
			{
				if(itype == 0)
				{
					ctrlPopUpMenu->addEntry(pData->getCategoryName(),pData->mCategoryType);
					ctrlPopUpMenu->sortID();
				}

				char szBuff[128] = {0,};
				dSprintf(szBuff,sizeof(szBuff),"<t c='0x000000ff' f='宋体' n='14' b='1'> %s </t>",pData->getCategoryName());
				ctrlTree->insertItem(0,szBuff,"","",0,1,1,szNodeId);
			}

			subId = ctrlTree->findMissionTreeNode(szNodeId);
			if(subId)
			{
				char szText[256]={0,};
				char szCommand[64]={0,}; 
				dSprintf(szCommand,sizeof(szCommand),"getPath_Position(%d);",pData->mRouteID);
				dMemcpy(szText,pData->mTextInfo,sizeof(szText));

				S32 npcId = pData->mTargetID;
				NpcObjectData* pNpcData = g_NpcRepository.GetNpcData(npcId);
				if(pNpcData && dStrcmp(pNpcData->npcTitle,""))
					dSprintf(szText,sizeof(szText),"<t c='0x000000ff' f='宋体' n='12'> %s%s%s </t>",pNpcData->npcTitle,"  ",pData->mTextInfo);
				else
					dSprintf(szText,sizeof(szText),"<t c='0x000000ff' f='宋体' n='12'> %s </t>",pData->mTextInfo);

				ctrlTree->insertItem(subId,szText,szCommand,"",0,1,1,pData->mTextInfo);
			}
		}
	}
	ctrlPopUpMenu->setSelected(itype,false);
}

void GuiBigViewMap::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPModulate );
	GFX->setTextureStageState( 1, GFXTSSColorOp, GFXTOPDisable );
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->endStateBlock(mSetSB);
}


void GuiBigViewMap::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}
}

void GuiBigViewMap::init()
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

void GuiBigViewMap::shutdown()
{
	SAFE_DELETE(mSetSB);
}


