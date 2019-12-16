//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc. 
//-----------------------------------------------------------------------------
#include <d3dx9math.h>
#include "console/console.h"
#include "console/consoleTypes.h"
#include "gfx/D3D9/gfxD3D9Device.h"
#include "terrain/terrData.h"
#include "Gameplay/ai/NaviGrid/NavigationBlock.h"
#include "Gameplay/ClientGameplayState.h"
#include "ui/guiViewMapCtrl.h"

IMPLEMENT_CONOBJECT(guiViewMapCtrl);

guiViewMapCtrl g_ViewMap;

const Point2I guiViewMapCtrl::mMapExtent(1024,768);
const RectI guiViewMapCtrl::m_PathAvailableRect(100,50,640,640);
const RectF guiViewMapCtrl::m_TextureAvailableRect(0,0,0.97656f,0.91145f); // 1000/1024,700/768

bool guiViewMapCtrl::m_DisplayMonsters(true);
bool guiViewMapCtrl::m_DisplayNpc(true);
bool guiViewMapCtrl::m_DisplayPlayers(true);
bool guiViewMapCtrl::m_DisplayAll(true);

guiViewMapCtrl::guiViewMapCtrl() : m_TerrainTexture(NULL),
								   m_TextureRect(0,0,1,1),
								   m_TerrainPlayer(0.5f,0.5f),
								   m_DestPos(0,0),
								   mTerrPos3D(0,0),
								   mCurrentMapId(0),
								   mSelectMapId(0),
								   m_SelfIconTexture(NULL),
								   m_PlayerIconTexture(NULL),
								   m_TeammateIconTexture(NULL),
								   m_CameraImageTexture(NULL),
								   m_NpcIconTexture(NULL),
								   m_MonsterIconTexture(NULL),
								   m_DestIconTexture(NULL),
								   m_TargetCover(NULL)
{
	m_SelfIconFileName     = StringTable->insert("");
	m_PlayerIconFileName   = StringTable->insert("");
	m_TeammateIconFileName = StringTable->insert("");
	m_CameraImageFileName  = StringTable->insert("");
	m_NpcIconFileName      = StringTable->insert("");
	m_MonsterIconFileName  = StringTable->insert("");
	m_DestIconFileName     = StringTable->insert("");

}

guiViewMapCtrl::~guiViewMapCtrl()
{
	m_TerrainTexture.free();
	m_SelfIconTexture.free();
	m_PlayerIconTexture.free();
	m_TeammateIconTexture.free();
	m_CameraImageTexture.free();
	m_NpcIconTexture.free();
	m_MonsterIconTexture.free();
	m_DestIconTexture.free();
}

void guiViewMapCtrl::onPreRender()
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(NULL == pPlayer)
		return;

	// 记录寻径路径
	if(g_ClientGameplayState->getDarwPath() && pPlayer->m_pAI && !pPlayer->m_pAI->IsNewMove() && pPlayer->m_pAI->GetAIPath())
	{
		Point3F srcPos = pPlayer->getPosition();
		g_ClientGameplayState->setPath(pPlayer->m_pAI->GetAIPath()->GetPathList());
		g_ClientGameplayState->getPathList().push_front(srcPos);
		g_ClientGameplayState->setDarwPath(false);
	}	
}


void guiViewMapCtrl::initPersistFields()
{
	Parent::initPersistFields();
	addGroup("guiViewMapCtrl");

	addField("SelfIconFileName",          TypeString,        Offset(m_SelfIconFileName,            guiViewMapCtrl));
	addField("PlayerIconFileName",        TypeString,        Offset(m_PlayerIconFileName,          guiViewMapCtrl));
	addField("TeammateIconFileName",      TypeString,        Offset(m_TeammateIconFileName,        guiViewMapCtrl));
	addField("CameraImageFileName",       TypeString,        Offset(m_CameraImageFileName,         guiViewMapCtrl));
	addField("NpcIconFileName",           TypeString,        Offset(m_NpcIconFileName,             guiViewMapCtrl));
	addField("MonsterIconFileName",       TypeString,        Offset(m_MonsterIconFileName,         guiViewMapCtrl));
	addField("DestIconFileName",          TypeString,        Offset(m_DestIconFileName,            guiViewMapCtrl));
	
	endGroup("guiViewMapCtrl");
}

bool guiViewMapCtrl::onWake()
{
	if(!Parent::onWake())
		return false;

	char szViewMap_File[256] = {0,};
	S32 iMapId = convertZoneIdToMapId(g_ClientGameplayState->getCurrentZoneId());
	if(!isValidMapId(iMapId))
		return true;

	viewMapData* pData = g_viewMapRepository.getViewMapData(iMapId);
	if(pData)
	{
		m_TextureRect = pData->getProjectRect();
		pData->getTerrainPos(mTerrPos3D);
		mCurrentMapId =  iMapId;
		mSelectMapId = mCurrentMapId;

		initTexture();
		dSprintf(szViewMap_File,sizeof(szViewMap_File),GUIMAPIMAGE,pData->getMapFileName());
		m_TerrainTexture.set(szViewMap_File,&GFXDefaultGUIProfile);

		if(m_TerrainTexture.isValid() && m_TargetCover.isNull())
			m_TargetCover.set(mMapExtent.x,mMapExtent.y,GFXFormat_32BIT,&GFXDefaultRenderTargetProfile);
	}

	return true;
}

void guiViewMapCtrl::onSleep()
{
	m_TerrainTexture.free();
	m_SelfIconTexture.free();
	m_PlayerIconTexture.free();
	m_TeammateIconTexture.free();
	m_CameraImageTexture.free();
	m_NpcIconTexture.free();
	m_MonsterIconTexture.free();
	m_DestIconTexture.free();
	Parent::onSleep();
}

S32 guiViewMapCtrl::convertZoneIdToMapId(const S32 zoneId)
{
	S32 mapId = zoneId;

	if(mapId > MAX_ZONEID)
		mapId /= MAX_COPYID;

	return mapId;
}

bool guiViewMapCtrl::isValidMapId(const S32 mapId)
{
	if(MIN_ZONEID <= mapId && mapId <= MAX_ZONEID)
		return true;

	AssertFatal(false,"Invalid mapId")
	return false;
}

void guiViewMapCtrl::setDisplayAll(bool value)
{
	if(value)
	{
		m_DisplayAll = true;
		m_DisplayPlayers = true;
		m_DisplayNpc = true;
		m_DisplayMonsters = true;
	}
	else
	{
		m_DisplayAll = false;
		m_DisplayPlayers = false;
		m_DisplayNpc = false;
		m_DisplayMonsters = false;
	}
}

void guiViewMapCtrl::setDisplayPlayer(bool value)
{
	if(value)
	{
		m_DisplayAll = true;
		m_DisplayPlayers = true;
		m_DisplayNpc = false;
		m_DisplayMonsters = false;
	}
}

void guiViewMapCtrl::setDisplayNpc(bool value)
{
	if(value)
	{
		m_DisplayAll = true;
		m_DisplayPlayers = false;
		m_DisplayNpc = true;
		m_DisplayMonsters = false;
	}
}

void guiViewMapCtrl::setDisplayMonster(bool value)
{
	if(value)
	{
		m_DisplayAll = true;
		m_DisplayPlayers = false;
		m_DisplayNpc = false;
		m_DisplayMonsters = true;
	}
}

void guiViewMapCtrl::getMapTextureUV(const RectF &rect,Point2F& pos)
{
	pos.x = (pos.x - rect.point.x)/ rect.extent.x;
	pos.y = (pos.y - rect.point.y)/ rect.extent.y;

	pos.x = (pos.x - m_TextureAvailableRect.point.x) * m_TextureAvailableRect.extent.x;
	pos.y = (pos.y - m_TextureAvailableRect.point.y) * m_TextureAvailableRect.extent.y;
}

Point2F guiViewMapCtrl::getTerrainPlayerScale(const Point2F &terrPos3D,const Point3F &pos3D)
{
	Point2F playerPos(0,0);
	playerPos.x = (pos3D.x - terrPos3D.x ) / MAP_WIDTH;
	//由于坐标系不同，需改变Y轴坐标 m_TerrainPlayer现在保存的是在整个terr中的坐标（0～1）
	playerPos.y = 1 - (pos3D.y - terrPos3D.y ) / MAP_WIDTH;
	return playerPos;
}

void guiViewMapCtrl::initTexture()
{
	if(m_SelfIconFileName && m_SelfIconFileName[0] && m_SelfIconTexture.isNull())
		m_SelfIconTexture.set(m_SelfIconFileName,&GFXDefaultGUIProfile);

	if(m_PlayerIconFileName && m_PlayerIconFileName[0] && m_PlayerIconTexture.isNull())
		m_PlayerIconTexture.set(m_PlayerIconFileName,&GFXDefaultGUIProfile);

	if(m_TeammateIconFileName && m_TeammateIconFileName[0] && m_TeammateIconTexture.isNull())
		m_TeammateIconTexture.set(m_TeammateIconFileName,&GFXDefaultGUIProfile);
	
	if(m_CameraImageFileName && m_CameraImageFileName[0] && m_CameraImageTexture.isNull())
		m_CameraImageTexture.set(m_CameraImageFileName,&GFXDefaultGUIProfile);

	if(m_NpcIconFileName && m_NpcIconFileName[0] && m_NpcIconTexture.isNull())
		m_NpcIconTexture.set(m_NpcIconFileName,&GFXDefaultGUIProfile);

	if(m_MonsterIconFileName && m_MonsterIconFileName[0] && m_MonsterIconTexture.isNull())
		m_MonsterIconTexture.set(m_MonsterIconFileName,&GFXDefaultGUIProfile);

	if(m_DestIconFileName && m_DestIconFileName[0] && m_DestIconTexture.isNull())
		m_DestIconTexture.set(m_DestIconFileName,&GFXDefaultGUIProfile);
}

void guiViewMapCtrl::drawPath(const RectF &drawRect)
{
	if(mCurrentMapId != mSelectMapId)
		return;

	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(NULL == pPlayer)
		return;

	if (g_ClientGameplayState->isShowSearchPath() && pPlayer->m_pAI->GetAIPath()->GetPathList().size()>0
		&& g_ClientGameplayState->getPathList().size()>0)
	{
		LPDIRECT3DDEVICE9 D3DDevice = dynamic_cast<GFXD3D9Device *>(GFX)->getDevice();
		if(D3DDevice && (!g_ClientGameplayState->getDarwPath() || !g_ClientGameplayState->isStartNavigation()))
		{		
			LPD3DXLINE   pLine;
			D3DXCreateLine(D3DDevice,   &pLine);
			U32 sizeNum = g_ClientGameplayState->getPathList().size();
			FrameAllocatorMarker alloc;
			D3DXVECTOR2* pv = (D3DXVECTOR2*)alloc.alloc( sizeof( D3DXVECTOR2 ) * sizeNum );
			std::list<Point3F>::iterator itr = g_ClientGameplayState->getPathList().begin();

			Point2F tempPos(0,0);
			for (U32 i = 0; itr != g_ClientGameplayState->getPathList().end(); ++itr,++i)
			{
				tempPos = convert3DposToMap2D(mCurrentMapId,(Point3F)(*itr));
				pv[i].x = tempPos.x;
				pv[i].y = tempPos.y;
			}
			RectI tempRect(tempPos.x,tempPos.y,0,0);

			pLine->SetWidth(4);
			pLine->SetAntialias(true);
			pLine->SetPatternScale(0.5f);
			pLine->SetGLLines(TRUE);
			pLine->SetPattern(0x00ff00ff);

			GFX->pushActiveRenderTarget();
			GFXTextureTargetRef mTarget = GFX->allocRenderToTextureTarget(); 
			mTarget->attachTexture(GFXTextureTarget::Color0, m_TargetCover);
			GFX->setActiveRenderTarget( mTarget );
			GFX->clear( GFXClearTarget, ColorI( 0, 0, 0, 0 ), 1.0f, 0 );

            GFX->setZEnable(false);
            GFX->setZWriteEnable(false);

			RectI clipRect = GFX->getClipRect();	
			GFX->setClipRect( RectI(0,0,mMapExtent.x,mMapExtent.y));

			pLine->Begin();
			pLine->Draw(pv, sizeNum, D3DCOLOR_XRGB(255, 255, 255));
			pLine->End();

			// 寻路终点
			drawBitmap(m_DestIconTexture,tempRect);
			m_DestPos = tempRect.point;

            GFX->setZEnable(true);
            GFX->setZWriteEnable(true);
			GFX->popActiveRenderTarget();
			GFX->setClipRect( clipRect );
			pLine->Release();	

			g_ClientGameplayState->setStartNavigation(true);
		}
		GFX->getDrawUtil()->clearBitmapModulation();
		GFX->getDrawUtil()->drawBitmapStretch(m_TargetCover,drawRect);
	}
	else if(g_ClientGameplayState->getPathList().size()>0)
	{
		g_ClientGameplayState->clearPath();
	}
}

void guiViewMapCtrl::drawBitmap(GFXTextureObject*texture,RectI &destRect)
{
	AssertFatal( texture != 0, "No texture specified for drawBitmapStretch()" );

	Point2I extent(texture->getWidth(),texture->getHeight());
	
	destRect.point.x  -= extent.x * 0.5f;
	destRect.point.y  -= extent.x * 0.5f; 
	destRect.extent.x = extent.x;
	destRect.extent.y = extent.y;

	GFX->getDrawUtil()->clearBitmapModulation();
	GFX->getDrawUtil()->drawBitmapStretch(texture,destRect);
}

Point2F guiViewMapCtrl::convert3DposToMap2D(S32 mapId,const Point3F &pos3D)
{
	Point2F mapPos(0,0); 
	viewMapData* pData = g_viewMapRepository.getViewMapData(mapId);
	if(NULL == pData)
		return mapPos;

	Point2F terrPos3D;
	RectF textureRect;
	pData->getTerrainPos(terrPos3D);
	textureRect = pData->getProjectRect();
	mapPos = getTerrainPlayerScale(terrPos3D,pos3D);

	if(m_TextureRect.contains(mapPos))
	{
		getMapTextureUV(textureRect,mapPos);
		mapPos.x *= mMapExtent.x;
		mapPos.y *= mMapExtent.y;
	}
	
	return mapPos;
}

Point2I guiViewMapCtrl::pos3DTo2D(S32 mapId,const Point3F &pos3D)
{
	Point2F pos = convert3DposToMap2D(mapId,pos3D);
	if(pos.isZero())
		return Point2I(pos.x,pos.y);
	else
		return map2DToFixed2D(pos);
}

Point2I guiViewMapCtrl::map2DToFixed2D(const Point2F &map2D)
{
	Point2I pos(0,0);
	if(!map2D.isZero())
	{
		pos.x = mCeil((F32)(map2D.x - m_PathAvailableRect.point.x) / m_PathAvailableRect.extent.x * POS2D_WIDTH);
		pos.y = mCeil((F32)(map2D.y - m_PathAvailableRect.point.y) / m_PathAvailableRect.extent.y * POS2D_WIDTH);
	}

	return pos;
}


ConsoleFunction(getPlayrPos3DTo2D,const char*,1,1,"getPlayrPos3DTo2D()")
{
	char* retBuffer =Con::getReturnBuffer(64);
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(pPlayer)
	{
		S32 mapId = g_ViewMap.convertZoneIdToMapId(g_ClientGameplayState->getCurrentZoneId());
		if(g_ViewMap.isValidMapId(mapId))
		{
			const Point2I  pos = g_ViewMap.pos3DTo2D(mapId,pPlayer->getPosition());
			if(!pos.isZero())
			{
				dSprintf(retBuffer, 64, "%d %d", pos.x, pos.y);
				return retBuffer;
			}
		}
	}
	return "";
}

ConsoleFunction(Pos3DTo2D,const char*,4,4,"Pos3DTo2D(%zoneId,%posX,%posY)")
{
	char* retBuffer =Con::getReturnBuffer(64);
	S32 mapId = g_ViewMap.convertZoneIdToMapId(dAtoi(argv[1]));
	Point3F pos3D(dAtof(argv[2]),dAtof(argv[3]),0);

	if(g_ViewMap.isValidMapId(mapId))
	{
		const Point2I  pos = g_ViewMap.pos3DTo2D(mapId,pos3D);
		if(!pos.isZero())
		{
			dSprintf(retBuffer, 64, "%d %d", pos.x, pos.y);
			return retBuffer;
		}
	}

	return "";
}

ConsoleFunction(viewMapSetDisplayAll,void,2,2,"viewMapSetDisplayAll(%bool)")
{
	g_ViewMap.setDisplayAll(dAtob(argv[1]));
}

ConsoleFunction(viewMapSetDisplayPlayer,void,2,2,"viewMapSetDisplayPlayer(%bool)")
{
	g_ViewMap.setDisplayPlayer(dAtob(argv[1]));
}

ConsoleFunction(viewMapSetDisplayNpc,void,2,2,"viewMapSetDisplayNpc(%bool)")
{
	g_ViewMap.setDisplayNpc(dAtob(argv[1]));
}

ConsoleFunction(viewMapSetDisplayMonster,void,2,2,"viewMapSetDisplayMonster(%bool)")
{
	g_ViewMap.setDisplayMonster(dAtob(argv[1]));
}