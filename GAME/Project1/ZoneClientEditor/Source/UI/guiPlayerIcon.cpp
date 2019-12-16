//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "console/consoleTypes.h"
#include "T3D/gameConnection.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/Team/ClientTeam.h"
#include "Gameplay/ClientPlayerManager.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/GameObjects/PetObject.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/GameObjects/GameObject.h"
#include "UI/guiPlayerIcon.h"

GuiPlayerIconPopUpMenu* GuiIconBase::m_pPlayerIconPopUpMenu;
//==================================================================================================================================
//   GuiIconBaseClase
//==================================================================================================================================
IMPLEMENT_CONOBJECT(GuiIconBase);
GuiIconBase::GuiIconBase()
{
    m_BackgroundBoundary.set(0,0,1,1);
    m_BarBackgroundBoundary.set(0,0,1,1);
    m_IconBoundary.set(0,0,1,1);
    m_NameBoundary.set(0,0,1,1);
    m_LevelBoundary.set(0,0,1,1);
    m_LevelBackBoundary.set(0,0,1,1);
    m_HpBarBoundary.set(0,0,1,1);
    m_MpBarBoundary.set(0,0,1,1);   
    m_CaptainBoundary.set(0,0,1,1);    

    m_BackgroundFileName = StringTable->insert("");
    m_BackgroundTexture = NULL;
    m_BarBackgroundFileName = StringTable->insert("");
    m_BarBackgroundTexture = NULL;   
    m_IconFileName[0] = '\0';   
    m_IconTexture = NULL;
    m_HpBarFileName = StringTable->insert("");
    m_HpBarTextrue = NULL;
    m_MpBarFileName = StringTable->insert("");
    m_MpBarTexture = NULL;   
    m_CaptainFileName = StringTable->insert("");
    m_CaptainTexture = NULL;    
 
    m_LevelBackFileName = StringTable->insert("");
    m_LevelBackTexture = NULL;

    m_NameColor.set(255,255,255,255);
    m_NameInfo[0] = '\0';

    m_HpInfo[0] = '\0';
    m_MpInfo[0] = '\0';    
    m_LevelInfo = 50;

    m_IsShowInfo = false;
    m_HpPercentum = 0;
    m_MpPercentum = 0;    

    m_MouseOnHp = true;
    m_MouseOnMp = true;
    m_MouseOnPp = true;
    m_IsShowTextInfo = true;
    m_IsUseMouseMethod = false;

    mMustPrerender = true;
}

void GuiIconBase::initPersistFields()
{
    Parent::initPersistFields();

    addField("BackgroundBoundary",    TypeRectI,    Offset(m_BackgroundBoundary,     GuiIconBase));
    addField("BarBackgroundBoundary", TypeRectI,    Offset(m_BarBackgroundBoundary,  GuiIconBase));
    addField("IconBoundary",          TypeRectI,    Offset(m_IconBoundary,           GuiIconBase));
    addField("NameBoundary",          TypeRectI,    Offset(m_NameBoundary,           GuiIconBase));
    addField("LevelBoundary",         TypeRectI,    Offset(m_LevelBoundary,          GuiIconBase));
    addField("LevelBackBoundary",     TypeRectI,    Offset(m_LevelBackBoundary,      GuiIconBase));
    addField("HpBarBoundary",         TypeRectI,    Offset(m_HpBarBoundary,          GuiIconBase));
    addField("MpBarBoundary",         TypeRectI,    Offset(m_MpBarBoundary,          GuiIconBase));  
    addField("CaptainBoundary",       TypeRectI,    Offset(m_CaptainBoundary,        GuiIconBase));    

    addField("BackgroundFileName",    TypeString,   Offset(m_BackgroundFileName,     GuiIconBase));
    addField("BarBackgroundFileName", TypeString,   Offset(m_BarBackgroundFileName,  GuiIconBase));
   
    addField("HpBarFileName",         TypeString,   Offset(m_HpBarFileName,          GuiIconBase));
    addField("MpBarFileName",         TypeString,   Offset(m_MpBarFileName,          GuiIconBase));    
    addField("CaptainFileName",       TypeString,   Offset(m_CaptainFileName,        GuiIconBase));  
    addField("LevelBackFileName",     TypeString,   Offset(m_LevelBackFileName,      GuiIconBase));
}

void GuiIconBase::IsShowTextInfo(S32 isShow)
{
    m_IsShowTextInfo = isShow;
}
void GuiIconBase::setCountType(S32 countType)
{
    m_CountType = countType;
}

void GuiIconBase::IsUseMouseMethod(S32 isUse)
{
    if (isUse == 1)
    {
        m_MouseOnHp = false;
        m_MouseOnMp = false;
    }
    else
    {
        m_MouseOnHp = true;
        m_MouseOnMp = true;
    }
    m_IsUseMouseMethod = isUse;
}

void GuiIconBase::onPreRender()
{
    m_DirtyFlag = updateData();
    if(!m_DirtyFlag)
        Parent::onPreRender();
}

bool GuiIconBase::onWake()
{
    if (!Parent::onWake())
        return false;

    setActive(true);
    setProfileProt(this,"GuiPlayerIconProfile");

    return true;
}

void GuiIconBase::onSleep()
{
    Parent::onSleep();
}

void GuiIconBase::onRenderExt(Point2I offset, const RectI &updateRect)
{
    if (m_IsShowInfo && isVisible())
    {    
        GFX->getDrawUtil()->clearBitmapModulation();
        if (!m_BarBackgroundTexture.isNull())
        {           
            RectI rect(offset + m_BarBackgroundBoundary.point,m_BarBackgroundBoundary.extent);
            GFX->getDrawUtil()->drawBitmapStretch(m_BarBackgroundTexture,rect);
        }		

        if (!m_BackgroundTexture.isNull())
        {
            
            RectI rect(offset + m_BackgroundBoundary.point,m_BackgroundBoundary.extent);
            GFX->getDrawUtil()->drawBitmapStretch(m_BackgroundTexture,rect);
        }

        if (!m_IconTexture.isNull())
        {           
            RectI rect(offset + m_IconBoundary.point,m_IconBoundary.extent);
            GFX->getDrawUtil()->drawBitmapStretch(m_IconTexture,rect);
        }

        if (!m_HpBarTextrue.isNull())
        {          
            RectI rect(offset + m_HpBarBoundary.point,m_HpBarBoundary.extent);
            rect.extent.x = rect.extent.x * m_HpPercentum;
            GFX->getDrawUtil()->drawBitmapStretch(m_HpBarTextrue,rect);
        }

        if (!m_MpBarTexture.isNull())
        {           
            RectI rect(offset + m_MpBarBoundary.point,m_MpBarBoundary.extent);
            rect.extent.x = rect.extent.x * m_MpPercentum;
            GFX->getDrawUtil()->drawBitmapStretch(m_MpBarTexture,rect);
        }
        if (m_HpInfo[0])
        {          
            GFX->getDrawUtil()->setBitmapModulation(m_NameColor);
            renderJustifiedText(offset + m_HpBarBoundary.point - Point2I(0,1), m_HpBarBoundary.extent, (char*)m_HpInfo);
        }

        if (m_MpInfo[0])
        {           
            GFX->getDrawUtil()->setBitmapModulation(m_NameColor);
            renderJustifiedText(offset + m_MpBarBoundary.point - Point2I(0,1), m_MpBarBoundary.extent, (char*)m_MpInfo);
        }
        if (m_NameInfo[0])
        {            
            GFX->getDrawUtil()->setBitmapModulation(m_NameColor);
            renderJustifiedText(offset + m_NameBoundary.point, m_NameBoundary.extent, (char*)m_NameInfo);
        }

        if (!m_LevelBackTexture.isNull())
        {           
            RectI rect(offset + m_LevelBackBoundary.point,m_LevelBackBoundary.extent);
            GFX->getDrawUtil()->drawBitmapStretch(m_LevelBackTexture,rect);
        }

        if (m_LevelInfo)
        {            
            renderJustifiedText(offset + m_LevelBoundary.point, m_LevelBoundary.extent, avar("%d",m_LevelInfo));
        }

        if (!m_CaptainTexture.isNull())
        {           
            RectI rect(offset + m_CaptainBoundary.point,m_CaptainBoundary.extent);
            GFX->getDrawUtil()->drawBitmapStretch(m_CaptainTexture,rect);
        }
        //renderChildControls(offset,updateRect);
    }
}


//==================================================================================================================================
//   GuiPlayerIcon
//==================================================================================================================================

IMPLEMENT_CONOBJECT(GuiPlayerIcon);

const char HEAD_ICON_PATH[64] = "gameres/data/icon/head/";

GuiPlayerIcon::GuiPlayerIcon()
{
    m_BileBarFileName = StringTable->insert("");
	m_BileBarTexture = NULL;
	
	dMemset(m_FamilyFileName,		0, sizeof(m_FamilyFileName));
	m_FamilyTexture = NULL;	
	m_PpInfo[0] = '\0';	
	m_BilePercentum = 0;
	m_TempFamily = -1;	
}

void GuiPlayerIcon::initPersistFields()
{
	Parent::initPersistFields();

	addField("BileBarBoundary",       TypeRectI,    Offset(m_BileBarBoundary,        GuiPlayerIcon));	
	addField("FamilyBoundary",        TypeRectI,    Offset(m_FamilyBoundary,         GuiPlayerIcon));	
	addField("BileBarFileName",       TypeString,   Offset(m_BileBarFileName,        GuiPlayerIcon));	
	addField("FamilyFileName",        TypeString,   Offset(m_FamilyFileName,         GuiPlayerIcon), 9);
}


bool GuiPlayerIcon::updateData()
{
	Player* myself = g_ClientGameplayState->GetControlPlayer();

	if (!myself)
		return false;

	GameObject* pObject = dynamic_cast<GameObject*>(myself);

	if (!pObject)
		return false;

	bool dirty = m_DirtyFlag;

	if (m_BarBackgroundTexture.isNull() && m_BarBackgroundFileName[0] != '\0')
	{
		m_BarBackgroundTexture = GFXTexHandle(m_BarBackgroundFileName,&GFXDefaultStaticDiffuseProfile,true);
		dirty = true;
	}

	if (m_BackgroundTexture.isNull() && m_BackgroundFileName[0] != '\0')
	{
		m_BackgroundTexture = GFXTexHandle(m_BackgroundFileName,&GFXDefaultStaticDiffuseProfile,true);
		dirty = true;
	}

	if (pObject->getSex() != m_TempSex)
	{
		m_TempSex = pObject->getSex();
		setHeadIcon(m_TempSex,myself->mFace,m_IconFileName);
		m_IconTexture = GFXTexHandle(m_IconFileName,&GFXDefaultStaticDiffuseProfile,true);
		dirty = true;
	}

	if (m_HpBarTextrue.isNull() && m_HpBarFileName[0] != '\0')
	{
		m_HpBarTextrue = GFXTexHandle(m_HpBarFileName,&GFXDefaultStaticDiffuseProfile,true);
		dirty = true;
	}


	if (m_MpBarTexture.isNull() && m_MpBarFileName[0] != '\0')
	{
		m_MpBarTexture = GFXTexHandle(m_MpBarFileName,&GFXDefaultStaticDiffuseProfile,true);
		dirty = true;
	}


	if (m_BileBarTexture.isNull() && m_BileBarFileName[0] != '\0')
	{
		m_BileBarTexture = GFXTexHandle(m_BileBarFileName,&GFXDefaultStaticDiffuseProfile,true);
		dirty = true;
	}

	if (m_LevelBackTexture.isNull() && m_LevelBackFileName[0] != '\0')
	{
		m_LevelBackTexture = GFXTexHandle(m_LevelBackFileName,&GFXDefaultStaticDiffuseProfile,true);
		dirty = true;
	}

	if (CLIENT_TEAM->HasTeam())
	{
		if (CLIENT_TEAM->GetCaption() == g_ClientGameplayState->getSelectedPlayerId())
		{
			if (!m_CaptainTexture)
			{
				if (m_CaptainFileName[0] != '\0')
				{
					m_CaptainTexture = GFXTexHandle(m_CaptainFileName,&GFXDefaultStaticDiffuseProfile,true);
					dirty = true;
				}
			}
		}
		else
		{
			if(m_CaptainTexture)
				dirty = true;

			m_CaptainTexture = NULL;
		}
	}
	else
	{
		if(m_CaptainTexture)
			dirty = true;
		m_CaptainTexture = NULL;
	}
	if (pObject->getFamily() != m_TempFamily)
	{
		m_TempFamily = pObject->getFamily();
		Con::executef("SetPlayerInfo_Family",Con::getIntArg(m_TempFamily));
		if (m_FamilyFileName[m_TempFamily])
		{
			m_FamilyTexture = GFXTexHandle(m_FamilyFileName[m_TempFamily],&GFXDefaultStaticDiffuseProfile,true);
			dirty = true;
		}
	}
	//if (!m_FamilyTexture)
	//{
	//	if (m_FamilyFileName[0] != '\0')
	//	{
	//		m_FamilyTexture = GFXTexHandle(m_FamilyFileName,&GFXDefaultStaticDiffuseProfile,true);
	//		dirty = true;
	//	}
	//}

	if (pObject->GetStats().HP <= pObject->GetStats().MaxHP)
	{
		F32 HpPercentum = float(pObject->GetStats().HP)/pObject->GetStats().MaxHP;
		if (HpPercentum < 0)
			HpPercentum = 0;
		else if (HpPercentum > 1)
			HpPercentum = 1;
		if(m_HpPercentum != HpPercentum)
		{
			m_HpPercentum = HpPercentum;
			dirty = true;
		}
	}
	if (pObject->GetStats().MP <= pObject->GetStats().MaxMP)
	{
		F32 MpPercentum = float(pObject->GetStats().MP)/pObject->GetStats().MaxMP;
		if (MpPercentum < 0)
			MpPercentum = 0;
		else if (MpPercentum > 1)
			MpPercentum = 1;
		if(m_MpPercentum != MpPercentum)
		{
			m_MpPercentum = MpPercentum;
			dirty = true;
		}
	}
	if (pObject->GetStats().PP <= pObject->GetStats().MaxPP)
	{
		F32 BilePercentum = float(pObject->GetStats().PP)/pObject->GetStats().MaxPP;
		if (BilePercentum < 0)
			BilePercentum = 0;
		else if (BilePercentum > 1)
			BilePercentum = 1;
		if(m_BilePercentum != BilePercentum)
		{
			m_BilePercentum = BilePercentum;
			dirty = true;
		}
	}

	char NameInfo[64] = {'\0'};
	dSprintf(NameInfo,sizeof(NameInfo),"%s",myself->getPlayerName());
	if (dStrcmp(m_NameInfo,NameInfo))
	{
		dStrcpy(m_NameInfo,sizeof(m_NameInfo),NameInfo);
		Con::executef("SetPlayerInfo_Name",m_NameInfo);
		dirty = true;
	}
	U32 LevelInfo = myself->getLevel();
	if(LevelInfo != m_LevelInfo)
	{
		m_LevelInfo = LevelInfo;
		Con::executef("SetPlayerInfo_Level",Con::getIntArg(m_LevelInfo));
		dirty = true;
	}

	if (m_IsShowTextInfo)
	{
		if(dirty)
		{
			switch (m_CountType)
			{
			case 0:
				{
					if (m_MouseOnHp)
						dSprintf(m_HpInfo,sizeof(m_HpInfo),"%d/%d",pObject->GetStats().HP,pObject->GetStats().MaxHP);
					else
						m_HpInfo[0] = '\0';

					if (m_MouseOnMp)
						dSprintf(m_MpInfo,sizeof(m_MpInfo),"%d/%d",pObject->GetStats().MP,pObject->GetStats().MaxMP);
					else
						m_MpInfo[0] = '\0';

					if (m_MouseOnPp)
						dSprintf(m_PpInfo,sizeof(m_PpInfo),"%d/%d",pObject->GetStats().PP,pObject->GetStats().MaxPP);
					else
						m_PpInfo[0] = '\0';			
				}
				break;
			case 1:
				{
					if (m_MouseOnHp)
						dSprintf(m_HpInfo,sizeof(m_HpInfo),"%d",pObject->GetStats().HP);
					else
						m_HpInfo[0] = '\0';

					if (m_MouseOnMp)
						dSprintf(m_MpInfo,sizeof(m_MpInfo),"%d",pObject->GetStats().MP);
					else
						m_MpInfo[0] = '\0';

					if (m_MouseOnPp)
						dSprintf(m_PpInfo,sizeof(m_PpInfo),"%d",pObject->GetStats().PP);
					else
						m_PpInfo[0] = '\0';
				}
				break;
			case 2:
				{
					if (m_MouseOnHp)
						dSprintf(m_HpInfo,sizeof(m_HpInfo),"%d%s",(int)(m_HpPercentum * 100),"%");
					else
						m_HpInfo[0] = '\0';

					if (m_MouseOnMp)
						dSprintf(m_MpInfo,sizeof(m_MpInfo),"%d%s",(int)(m_MpPercentum * 100),"%");
					else
						m_MpInfo[0] = '\0';

					if (m_MouseOnPp)
						dSprintf(m_PpInfo,sizeof(m_PpInfo),"%d%s",(int)(m_BilePercentum * 100),"%");
					else
						m_PpInfo[0] = '\0';
				}
				break;
			default:;
			}
		}
	}
	else
	{
		m_HpInfo[0] = '\0';
		m_MpInfo[0] = '\0';
		m_PpInfo[0] = '\0';
	}

	m_IsShowInfo = true;
	return dirty;
}
void GuiPlayerIcon::onRenderExt(Point2I offset, const RectI &updateRect)
{
	if (m_IsShowInfo && isVisible())
	{        
        Parent::onRenderExt(offset,updateRect);       

        if (!m_BileBarTexture.isNull())
        {
            GFX->getDrawUtil()->clearBitmapModulation();
            RectI rect(offset + m_BileBarBoundary.point,m_BileBarBoundary.extent);
            rect.extent.x = rect.extent.x * m_BilePercentum;
            GFX->getDrawUtil()->drawBitmapStretch(m_BileBarTexture,rect);
        }			
// 		if (m_PpInfo[0])
// 		{
// 			GFX->getDrawUtil()->clearBitmapModulation();
// 			GFX->getDrawUtil()->setBitmapModulation(m_NameColor);
// 			renderJustifiedText(offset + m_BileBarBoundary.point - Point2I(0,1), m_BileBarBoundary.extent, (char*)m_PpInfo);
// 		}	

		if (!m_FamilyTexture.isNull())
		{
			GFX->getDrawUtil()->clearBitmapModulation();
			RectI rect(offset + m_FamilyBoundary.point,m_FamilyBoundary.extent);
			GFX->getDrawUtil()->drawBitmapStretch(m_FamilyTexture,rect);
		}

		renderChildControls(offset,updateRect);
	}
}

void GuiPlayerIcon::onMouseDown(const GuiEvent &event)
{
	Player* myself = g_ClientGameplayState->GetControlPlayer();
	if(!myself)
		return;

	g_ClientGameplayState->GetControlPlayer()->SetTarget(myself);
	setUpdate();
}

void GuiPlayerIcon::onMouseMove(const GuiEvent &event)
{
	if (m_IsShowTextInfo)
	{
		if (m_IsUseMouseMethod)
		{
			Point2I local_position = globalToLocalCoord(event.mousePoint);
			if (m_HpBarBoundary.pointInRect(local_position))
				m_MouseOnHp = true;
			else
				m_MouseOnHp = false;
			if (m_MpBarBoundary.pointInRect(local_position))
				m_MouseOnMp = true;
			else
				m_MouseOnMp = false;
			if (m_BileBarBoundary.pointInRect(local_position))
				m_MouseOnPp = true;
			else
				m_MouseOnPp = false;
		}
		else
		{
			m_MouseOnHp = true;
			m_MouseOnMp = true;
			m_MouseOnPp = true;
		}

		setUpdate();
	}
}

void GuiPlayerIcon::selectSelfIcon()
{
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(!conn)
		return;

	Player* control_player = dynamic_cast<Player*>(conn->getControlObject());
	if(!control_player)
		return;

	control_player->SetTarget(control_player,false);

}

void GuiPlayerIcon::onRightMouseDown( const GuiEvent &event )
{
	Point2I local_position = globalToLocalCoord(event.mousePoint);
	if (m_BackgroundBoundary.pointInRect(local_position))
    {
        GuiIconBase::m_pPlayerIconPopUpMenu = (GuiPlayerIconPopUpMenu *)Sim::findObject("PopUpMenuWndCtrl");
        getRoot()->pushDialogControl( m_pPlayerIconPopUpMenu , 99 );
        Con::executef("OpenMouseRightDownWnd","0",Con::getIntArg(event.mousePoint.x),Con::getIntArg(event.mousePoint.y));
    }
		
}

void GuiPlayerIcon::setHeadIcon(S32 objSex, S32 objFace,char* iconFileName)
{
	char headIcon[32] = "\0";
	switch (objFace)
	{
	case 0:
		{
			if (objSex == 1)
				dStrcpy(headIcon, sizeof(headIcon), "GUIWindow02_1_021");
			else
				dStrcpy(headIcon, sizeof(headIcon), "GUIWindow02_1_022");
		}
		break;
	case 1:
		{
			if (objSex == 1)
				dStrcpy(headIcon, sizeof(headIcon), "GUIWindow02_1_021");
			else
				dStrcpy(headIcon, sizeof(headIcon), "GUIWindow02_1_022");
		}
		break;
	case 2:
		{
			if (objSex == 1)
				dStrcpy(headIcon, sizeof(headIcon), "GUIWindow02_1_021");
			else
				dStrcpy(headIcon, sizeof(headIcon), "GUIWindow02_1_022");
		}
		break;
	case 3:
		{
			if (objSex == 1)
				dStrcpy(headIcon, sizeof(headIcon), "GUIWindow02_1_021");
			else
				dStrcpy(headIcon, sizeof(headIcon), "GUIWindow02_1_022");
		}
		break;
	case 4:
		{
			if (objSex == 1)
				dStrcpy(headIcon, sizeof(headIcon), "GUIWindow02_1_021");
			else
				dStrcpy(headIcon, sizeof(headIcon), "GUIWindow02_1_022");
		}
		break;
	case 5:
		{
			if (objSex == 1)
				dStrcpy(headIcon, sizeof(headIcon), "GUIWindow02_1_021");
			else
				dStrcpy(headIcon, sizeof(headIcon), "GUIWindow02_1_022");
		}
		break;
	default:
		break;
	}
	//dStrcat(iconFileName,128,HEAD_ICON_PATH);
	//dStrcat(iconFileName,128,headIcon); 
	dSprintf(iconFileName,128,"%s%s",HEAD_ICON_PATH,headIcon);
}
//==================================================================================================================================
//   GuiObjectIcon
//==================================================================================================================================

IMPLEMENT_CONOBJECT(GuiObjectIcon);

GuiObjectIcon::GuiObjectIcon()
{
    m_FamilyBoundary.set(0,0,1,1);

	m_IngroupFileName = StringTable->insert("");
	m_IngroupTexture = NULL;

	m_CountType = 1;
	m_ObjectType = 1;

	m_ShowTarget = false;
	m_ShowPet = false;

	pObject = NULL;
	pPlayer = NULL;

	mMustPrerender = true;

	m_TempIcon[0] = '\0';
}

void GuiObjectIcon::initPersistFields()
{
	Parent::initPersistFields();

	addField("IngroupBoundary",       TypeRectI,    Offset(m_FamilyBoundary,        GuiObjectIcon));
	addField("IngroupFileNmae",       TypeString,   Offset(m_IngroupFileName,        GuiObjectIcon));
	addField("ObjectType",            TypeS32,      Offset(m_ObjectType,             GuiObjectIcon));
}

bool GuiObjectIcon::updateData()
{
	bool dirty = m_DirtyFlag;

	pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;

	if (m_ObjectType == 1)
	{
		pObject = dynamic_cast<GameObject*>(pPlayer->GetTarget());

		if (!pObject)
		{
			if(m_ShowTarget)
			{
				m_ShowTarget = false;
				Con::executef("ShowTargetPlayerWnd","0");
				dirty = true;
			}
			return false;
		}
		else
		{
			if (pObject->getGameObjectMask() & PlayerObjectType)
			{
				if (pPlayer->getId() == pObject->getId())
					Con::executef("ClosePlayerCommand");
				else
					Con::executef("OpenPlayerCommand");
				GuiPlayerIcon::setHeadIcon(pObject->getSex(), pPlayer->mFace, m_IconFileName);
			}
			else if (pObject->getGameObjectMask() & PetObjectType)
			{
				Con::executef("ClosePlayerCommand");
				PetObjectData* petData = g_PetRepository.GetPetData(pObject->getDataID());
				char petIcon[32] = "\0";
				dStrcpy(petIcon,sizeof(petIcon),petData->petIcon);
				dSprintf(m_IconFileName,sizeof(m_IconFileName),"%s%s",HEAD_ICON_PATH,petIcon);
			}
			else  
			{
				Con::executef("ClosePlayerCommand");
				NpcObjectData* npcData = g_NpcRepository.GetNpcData(pObject->getDataID());
				char objectIcon[32] = "\0";
				dStrcpy(objectIcon,sizeof(objectIcon),npcData->icon);
				dSprintf(m_IconFileName,sizeof(m_IconFileName),"%s%s",HEAD_ICON_PATH,objectIcon);
			}
			if(!m_ShowTarget)
			{
				m_ShowTarget = true;
				Con::executef("ShowTargetPlayerWnd","1");
				dirty = true;
			}
		}
	}
	else if (m_ObjectType == 2)
	{		
		pObject = dynamic_cast<GameObject*>(pPlayer->getPet(0));
		if (!pObject)
		{
			Con::executef("ShowPetWnd","0");
			dirty = true;
			return false;
		}
		else
		{			
			PetObject *pPetObject = dynamic_cast<PetObject *>(pObject);
			Player *pPlayer = pPetObject->getMaster();
			if (!pPlayer)
				return false;
			PetTable &petTable = (PetTable &)pPlayer->getPetTable();
			stPetInfo *pPetInfo = (stPetInfo *)petTable.getPetInfo(pPetObject->getPetSlot());
			if (!pPetInfo)
				return false;
			PetObjectData* petData = g_PetRepository.GetPetData(pPetInfo->petDataId);
			if (!petData)
				return false;
			dSprintf(m_IconFileName,sizeof(m_IconFileName),"%s%s", HEAD_ICON_PATH, petData->petIcon); 

            S32 nPetIconStatus = Con::getIntVariable("$PetIconGui_Status");
            if (nPetIconStatus == 2)
            {
                Con::executef("ShowPetWnd","1");
			    dirty = true;
            }
		}
	}

	if (m_BarBackgroundTexture.isNull() && m_BarBackgroundFileName[0] != '\0')
	{
		m_BarBackgroundTexture = GFXTexHandle(m_BarBackgroundFileName,&GFXDefaultStaticDiffuseProfile,true);
		dirty = true;
	}

	if (m_BackgroundTexture.isNull() && m_BackgroundFileName[0] != '\0')
	{
		m_BackgroundTexture = GFXTexHandle(m_BackgroundFileName,&GFXDefaultStaticDiffuseProfile,true);
		dirty = true;
	}

	if (dStrcmp((char*)m_IconFileName,(char*)m_TempIcon))
	{
		dStrcpy(m_TempIcon,sizeof(m_TempIcon),m_IconFileName);
		m_IconTexture = GFXTexHandle(m_TempIcon,&GFXDefaultStaticDiffuseProfile,true);
		dirty = true; 
	}

	if (m_HpBarTextrue.isNull() && m_HpBarFileName[0] != '\0')
	{
		m_HpBarTextrue = GFXTexHandle(m_HpBarFileName,&GFXDefaultStaticDiffuseProfile,true);
		dirty = true;
	}

	if (m_MpBarTexture.isNull() && m_MpBarFileName[0] != '\0')
	{
		m_MpBarTexture = GFXTexHandle(m_MpBarFileName,&GFXDefaultStaticDiffuseProfile,true);
		dirty = true;
	}

	if (m_CaptainTexture.isNull() && m_CaptainFileName[0] != '\0')
	{
		m_CaptainTexture = GFXTexHandle(m_CaptainFileName,&GFXDefaultStaticDiffuseProfile,true);
		dirty = true;
	}

	if (m_IngroupTexture.isNull() && m_IngroupFileName[0] != '\0')
	{
		m_IngroupTexture = GFXTexHandle(m_IngroupFileName,&GFXDefaultStaticDiffuseProfile,true);
		dirty = true;
	}

	if (m_LevelBackTexture.isNull() && m_LevelBackFileName[0] != '\0')
	{
		m_LevelBackTexture = GFXTexHandle(m_LevelBackFileName,&GFXDefaultStaticDiffuseProfile,true);
		dirty = true;
	}

	if (pObject->GetStats().HP <= pObject->GetStats().MaxHP)
	{
		F32 HpPercentum = float(pObject->GetStats().HP)/pObject->GetStats().MaxHP;
		if (HpPercentum < 0)
			HpPercentum = 0;
		else if (HpPercentum > 1)
			HpPercentum = 1;
		if(m_HpPercentum != HpPercentum )
		{
			m_HpPercentum = HpPercentum;
			dirty = true;
		}
	}
	if (pObject->GetStats().MP <= pObject->GetStats().MaxMP)
	{
		F32 MpPercentum = float(pObject->GetStats().MP)/pObject->GetStats().MaxMP;
		if (MpPercentum < 0)
			MpPercentum = 0;
		else if (MpPercentum > 1)
			MpPercentum = 1;
		if(m_MpPercentum != MpPercentum)
		{
			m_MpPercentum = MpPercentum;
			dirty = true;
		}
	}

	dSprintf(m_NameInfo,sizeof(m_NameInfo),"%s",pObject->getObjectName());

	U32 LevelInfo = pObject->getLevel();
	if(m_LevelInfo!=LevelInfo)
	{
		m_LevelInfo = LevelInfo;
		dirty = true;
	}

	if (m_IsShowTextInfo)
	{
		if(dirty)
		{
			switch (m_CountType)
			{
			case 0:
				{
					if (m_MouseOnHp)
						dSprintf(m_HpInfo,sizeof(m_HpInfo),"%d/%d",pObject->GetStats().HP,pObject->GetStats().MaxHP);
					else
						m_HpInfo[0] = '\0';
	
					if (m_MouseOnMp)
						dSprintf(m_MpInfo,sizeof(m_MpInfo),"%d/%d",pObject->GetStats().MP,pObject->GetStats().MaxMP);
					else
						m_MpInfo[0] = '\0';		
				}
				break;
			case 1:
				{
					if (m_MouseOnHp)
						//dSprintf(m_HpInfo,sizeof(m_HpInfo),"%d",pObject->GetStats().HP);
						dSprintf(m_HpInfo,sizeof(m_HpInfo),"%d/%d",pObject->GetStats().HP, pObject->GetStats().MaxHP);
					else
						m_HpInfo[0] = '\0';
					
					if (m_MouseOnMp)
						//dSprintf(m_MpInfo,sizeof(m_MpInfo),"%d",pObject->GetStats().MP);
						dSprintf(m_MpInfo,sizeof(m_MpInfo),"%d/%d",pObject->GetStats().MP,pObject->GetStats().MaxMP);
					else
						m_MpInfo[0] = '\0';
				}
				break;
			case 2:
				{
					if (m_MouseOnHp)
						dSprintf(m_HpInfo,sizeof(m_HpInfo),"%d%s",(int)(m_HpPercentum * 100),"%");
					else
						m_HpInfo[0] = '\0';
					
					if (m_MouseOnMp)
						dSprintf(m_MpInfo,sizeof(m_MpInfo),"%d%s",(int)(m_MpPercentum * 100),"%");
					else
						m_MpInfo[0] = '\0';
				}
				break;
			default:;
			}
		}
	}
	else
	{
		m_HpInfo[0] = '\0';
		m_MpInfo[0] = '\0';
	}

	m_IsShowInfo = true;
	return dirty;
}


void GuiObjectIcon::onRenderExt(Point2I offset, const RectI &updateRect)
{
	if (m_IsShowInfo && isVisible())
    {
        Parent::onRenderExt(offset,updateRect); 

        if (!m_IngroupTexture.isNull())
        {
            GFX->getDrawUtil()->clearBitmapModulation();
            RectI rect(offset + m_FamilyBoundary.point,m_FamilyBoundary.extent);
            GFX->getDrawUtil()->drawBitmapStretch(m_IngroupTexture,rect);
        }

		renderChildControls(offset,updateRect);
	}
}

void GuiObjectIcon::onMouseMove(const GuiEvent &event)
{
	if (m_IsShowTextInfo)
	{
		if (m_IsUseMouseMethod)
		{
			Point2I local_position = globalToLocalCoord(event.mousePoint);
			if (m_HpBarBoundary.pointInRect(local_position))
				m_MouseOnHp = true;
			else
				m_MouseOnHp = false;
			if (m_MpBarBoundary.pointInRect(local_position))
				m_MouseOnMp = true;
			else
				m_MouseOnMp = false;
		}
		else
		{
			m_MouseOnHp = true;
			m_MouseOnMp = true;
		}

		m_DirtyFlag = true;
	}
}


void GuiObjectIcon::setTargetNull()
{
	g_ClientGameplayState->GetControlPlayer()->SetTarget(NULL);
}

void GuiObjectIcon::onRightMouseDown( const GuiEvent &event )
{
	Point2I local_position = globalToLocalCoord(event.mousePoint);
	if (m_BackgroundBoundary.pointInRect(local_position))
	{
        GuiIconBase::m_pPlayerIconPopUpMenu = (GuiPlayerIconPopUpMenu *)Sim::findObject("PopUpMenuWndCtrl");
        getRoot()->pushDialogControl( m_pPlayerIconPopUpMenu , 99 );
		if (pObject->getGameObjectMask() & PlayerObjectType && pPlayer->getId() != pObject->getId())
			Con::executef("OpenMouseRightDownWnd","1",Con::getIntArg(event.mousePoint.x),Con::getIntArg(event.mousePoint.y));
		else if (pObject->getGameObjectMask() & NpcObjectType && pObject->isCombative())
			Con::executef("OpenMouseRightDownWnd","2",Con::getIntArg(event.mousePoint.x),Con::getIntArg(event.mousePoint.y));
		else if (pObject->getGameObjectMask() & PetObjectType && pPlayer->getPet(0)->getId() != pObject->getId())
			Con::executef("OpenMouseRightDownWnd","3",Con::getIntArg(event.mousePoint.x),Con::getIntArg(event.mousePoint.y));
		else if (pObject->getGameObjectMask() & PetObjectType && pPlayer->getPet(0)->getId() == pObject->getId())
			Con::executef("OpenMouseRightDownWnd","4",Con::getIntArg(event.mousePoint.x),Con::getIntArg(event.mousePoint.y));
	}
}
//==================================================================================================================================
//   GuiTeammateIcon
//==================================================================================================================================

IMPLEMENT_CONOBJECT(GuiTeammateIcon);

GuiTeammateIcon::GuiTeammateIcon()
{
	for ( int i = 0; i < 5; i++)
	{
		m_TeammateBoundary[i].set(0,0,1,1);
		m_NameInfo[i][0] = '\0';
		m_HpInfo[i][0] = '\0';
		m_MpInfo[i][0] = '\0';
		m_LevelInfo[i] = 50;
		m_MouseOnHp[i+1] = true;
		m_MouseOnMp[i+1] = true;
	}
	m_CountType = 2;
	m_TempSex = 0;

	m_IsShowTextInfo = false;
}

void GuiTeammateIcon::initPersistFields()
{
	Parent::initPersistFields();

	addField("TeammateBoundary",	  TypeRectI,    Offset(m_TeammateBoundary,	     GuiTeammateIcon), 5);
}

bool GuiTeammateIcon::updateData()
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return false;

	bool dirty = m_DirtyFlag;

	if (CLIENT_TEAM->HasTeam())
	{
		for (int i = 1; i <= CLIENT_TEAM->GetPlayerList()->size()-1; i++)
		{
			Player* pTeammate = PLAYER_MGR->GetLocalPlayer(CLIENT_TEAM->GetPlayerByIndex(i-1,0));

			dSprintf(m_NameInfo[i-1],64,"%s",PLAYER_MGR->getPlayerName(CLIENT_TEAM->GetPlayerByIndex(i-1,0)).c_str());

			if (!m_BarBackgroundTexture)
			{
				if (m_BarBackgroundFileName[0] != '\0')
				{
					m_BarBackgroundTexture = GFXTexHandle(m_BarBackgroundFileName,&GFXDefaultStaticDiffuseProfile,true);
					dirty = true;
				}
			}

			if (m_BackgroundTexture.isNull() && m_BackgroundFileName[0] != '\0')
			{
				m_BackgroundTexture = GFXTexHandle(m_BackgroundFileName,&GFXDefaultStaticDiffuseProfile,true);
				dirty = true;
			}

			//if (!m_IconTexture)
			//{
			//	if (m_IconFileName[0] != '\0')
			//	{
			//		m_IconTexture = GFXTexHandle(m_IconFileName,&GFXDefaultStaticDiffuseProfile,true);
			//		dirty = true;
			//	}
			//}

			if (m_HpBarTextrue.isNull() && m_HpBarFileName[0] != '\0')
			{
				m_HpBarTextrue = GFXTexHandle(m_HpBarFileName,&GFXDefaultStaticDiffuseProfile,true);
				dirty = true;
			}

			if (m_MpBarTexture.isNull() && m_MpBarFileName[0] != '\0')
			{
				m_MpBarTexture = GFXTexHandle(m_MpBarFileName,&GFXDefaultStaticDiffuseProfile,true);
				dirty = true;
			}

			if (m_LevelBackTexture.isNull() && m_LevelBackFileName[0] != '\0')
			{
				m_LevelBackTexture = GFXTexHandle(m_LevelBackFileName,&GFXDefaultStaticDiffuseProfile,true);
				dirty = true;
			}

			if (CLIENT_TEAM->HasTeam())
			{
				if (CLIENT_TEAM->GetCaption() != g_ClientGameplayState->getSelectedPlayerId())
				{
					if (m_CaptainTexture.isNull() && m_CaptainFileName[0] != '\0')
					{
						m_CaptainTexture = GFXTexHandle(m_CaptainFileName,&GFXDefaultStaticDiffuseProfile,true);
						dirty = true;
					}
				}
				else if (!m_CaptainTexture.isNull())
				{
					m_CaptainTexture.free();
					dirty = true;
				}
			}
			else if (!m_CaptainTexture.isNull())
			{
				m_CaptainTexture.free();
				dirty = true;
			}

			if (!pTeammate)
				return false;

			GameObject* pObject = dynamic_cast<GameObject*>(pTeammate);
			if (!pObject)
				return false;

			if (pObject->getSex() != m_TempSex)
			{
				m_TempSex = pObject->getSex();
				GuiPlayerIcon::setHeadIcon(m_TempSex,pTeammate->mFace,m_IconFileName);
				m_IconTexture = GFXTexHandle(m_IconFileName,&GFXDefaultStaticDiffuseProfile,true);
				dirty = true;
			}

			if (pObject->GetStats().HP <= pObject->GetStats().MaxHP)
			{
				F32 HpPercentum = float(pObject->GetStats().HP)/pObject->GetStats().MaxHP;
				if (HpPercentum < 0)
					HpPercentum = 0;
				else if (HpPercentum > 1)
					HpPercentum = 1;
				if(HpPercentum != m_HpPercentum[i-1])
				{
					m_HpPercentum[i-1] = HpPercentum;
					dirty = true;
				} 
			}
			if (pObject->GetStats().MP <= pObject->GetStats().MaxMP)
			{
				F32 MpPercentum = float(pObject->GetStats().MP)/pObject->GetStats().MaxMP;
				if (MpPercentum < 0)
					MpPercentum = 0;
				else if (MpPercentum > 1)
					MpPercentum = 1;
				if(MpPercentum != m_MpPercentum[i-1])
				{
					m_MpPercentum[i-1] = MpPercentum;
					dirty = true;
				} 
			}

			U32 LevelInfo = pTeammate->getLevel();
			if(m_LevelInfo[i-1] != LevelInfo)
			{
				m_LevelInfo[i-1] = LevelInfo;
				dirty = true;
			}

			if (m_IsShowTextInfo)
			{
				if(dirty)
				{
					switch (m_CountType)
					{
					case 0:
						{
							if (m_MouseOnHp[i-1])
								dSprintf(m_HpInfo[i-1],sizeof(m_HpInfo[i-1]),"%d/%d",pObject->GetStats().HP,pObject->GetStats().MaxHP);
							else
								m_HpInfo[i-1][0] = '\0';
							
							if (m_MouseOnMp[i-1])
								dSprintf(m_MpInfo[i-1],sizeof(m_MpInfo[i-1]),"%d/%d",pObject->GetStats().MP,pObject->GetStats().MaxMP);
							else
								m_MpInfo[i-1][0] = '\0';		
						}
						break;
					case 1:
						{
							if (m_MouseOnHp[i-1])
								dSprintf(m_HpInfo[i-1],sizeof(m_HpInfo[i-1]),"%d",pObject->GetStats().HP);
							else
								m_HpInfo[i-1][0] = '\0';
							
							if (m_MouseOnMp[i])
								dSprintf(m_MpInfo[i-1],sizeof(m_MpInfo[i-1]),"%d",pObject->GetStats().MP);
							else
								m_MpInfo[i-1][0] = '\0';
						}
						break;
					case 2:
						{
							if (m_MouseOnHp[i-1])
								dSprintf(m_HpInfo[i-1],sizeof(m_HpInfo[i-1]),"%d%s",(int)(m_HpPercentum[i-1] * 100),"%");
							else
								m_HpInfo[i-1][0] = '\0';
							
							if (m_MouseOnMp[i-1])
								dSprintf(m_MpInfo[i-1],sizeof(m_MpInfo[i-1]),"%d%s",(int)(m_MpPercentum[i-1] * 100),"%");
							else
								m_MpInfo[i-1][0] = '\0';
						}
						break;
					default:;
					}
				}
			}
			else
			{
				m_HpInfo[i-1][0] = '\0';
				m_MpInfo[i-1][0] = '\0';
			}

		}
		//m_IsShowInfo = true;
	}
	return dirty;
}

void GuiTeammateIcon::onRender(Point2I offset, const RectI &updateRect)
{
	if (/*m_IsShowInfo && */isVisible())
	{
        int size = CLIENT_TEAM->GetPlayerList()->size();
        if (size == 0)
            return;

        for (int i = 1; i <= size-1; i++)
        {
            if (!m_BackgroundTexture.isNull())
            {
                GFX->getDrawUtil()->clearBitmapModulation();
                RectI rect(offset + m_BackgroundBoundary.point + m_TeammateBoundary[i-1].point,m_BackgroundBoundary.extent);
                GFX->getDrawUtil()->drawBitmapStretch(m_BackgroundTexture,rect);
            }

            if (!m_IconTexture.isNull())
            {
                GFX->getDrawUtil()->clearBitmapModulation();
                RectI rect(offset + m_IconBoundary.point + m_TeammateBoundary[i-1].point,m_IconBoundary.extent);
                GFX->getDrawUtil()->drawBitmapStretch(m_IconTexture,rect);
            }

			if (!m_HpBarTextrue.isNull())
			{
				GFX->getDrawUtil()->clearBitmapModulation();
				RectI rect(offset + m_HpBarBoundary.point + m_TeammateBoundary[i-1].point,m_HpBarBoundary.extent);
				rect.extent.x = rect.extent.x * m_HpPercentum[i-1];
				GFX->getDrawUtil()->drawBitmapStretch(m_HpBarTextrue,rect);
			}

			if (!m_MpBarTexture.isNull())
			{
				GFX->getDrawUtil()->clearBitmapModulation();
				RectI rect(offset + m_MpBarBoundary.point + m_TeammateBoundary[i-1].point,m_MpBarBoundary.extent);
				rect.extent.x = rect.extent.x * m_MpPercentum[i-1];
				GFX->getDrawUtil()->drawBitmapStretch(m_MpBarTexture,rect);
			}			

			/*if (m_HpInfo[i-1][0])
			{
				GFX->getDrawUtil()->clearBitmapModulation();
				GFX->getDrawUtil()->setBitmapModulation(m_NameColor);
				renderJustifiedText(offset + m_HpBarBoundary.point - Point2I(0,1) + m_TeammateBoundary[i-1].point, m_HpBarBoundary.extent, (char*)m_HpInfo[i-1]);
			}

			if (m_MpInfo[i-1][0])
			{
				GFX->getDrawUtil()->clearBitmapModulation();
				GFX->getDrawUtil()->setBitmapModulation(m_NameColor);
				renderJustifiedText(offset + m_MpBarBoundary.point + m_TeammateBoundary[i-1].point, m_MpBarBoundary.extent, (char*)m_MpInfo[i-1]);
			}*/

			if (m_NameInfo[i-1][0])
			{
				GFX->getDrawUtil()->clearBitmapModulation();
				GFX->getDrawUtil()->setBitmapModulation( ColorI(255,255,255,255 ));
				renderJustifiedText(offset + m_NameBoundary.point + m_TeammateBoundary[i-1].point, m_NameBoundary.extent, (char*)m_NameInfo[i-1]);             
			}			

			if (!m_LevelBackTexture.isNull())
			{
				GFX->getDrawUtil()->clearBitmapModulation();
				RectI rect(offset + m_LevelBackBoundary.point + m_TeammateBoundary[i-1].point,m_LevelBackBoundary.extent);
				GFX->getDrawUtil()->drawBitmapStretch(m_LevelBackTexture,rect);
			}

			if (m_LevelInfo)
			{
				GFX->getDrawUtil()->clearBitmapModulation();
				GFX->getDrawUtil()->setBitmapModulation(ColorI(255,255,255,255));
				renderJustifiedText(offset + m_LevelBoundary.point + m_TeammateBoundary[i-1].point, m_LevelBoundary.extent, avar("%d",m_LevelInfo[i-1]));
			}
			if (!m_CaptainTexture.isNull())
			{
				GFX->getDrawUtil()->clearBitmapModulation();
				RectI rect(offset + m_CaptainBoundary.point,m_CaptainBoundary.extent);
				GFX->getDrawUtil()->drawBitmapStretch(m_CaptainTexture,rect);
			}
		}
	}

	renderChildControls(offset,updateRect);
}

void GuiTeammateIcon::onMouseDown(const GuiEvent &event)
{
	Point2I local_position = globalToLocalCoord(event.mousePoint);
	for (int i = 1; i <= CLIENT_TEAM->GetPlayerList()->size() - 1; i++)
	{
		Player* pTeammate = PLAYER_MGR->GetLocalPlayer(CLIENT_TEAM->GetPlayerByIndex(i-1,0));
		if (m_TeammateBoundary[i-1].pointInRect(local_position))
			g_ClientGameplayState->GetControlPlayer()->SetTarget(pTeammate);
	}
	setUpdate();
}

void GuiTeammateIcon::onMouseMove(const GuiEvent &event)
{
	if (m_IsShowTextInfo)
	{
		if (m_IsUseMouseMethod)
		{
			Point2I local_position = globalToLocalCoord(event.mousePoint);

			for (int i = 1; i <= CLIENT_TEAM->GetPlayerList()->size()-1; i++)
			{
				hpBoundary[i-1] = RectI(m_HpBarBoundary.point + m_TeammateBoundary[i-1].point,m_HpBarBoundary.extent);
				if (hpBoundary[i-1].pointInRect(local_position))
					m_MouseOnHp[i-1] = true;
				else
					m_MouseOnHp[i-1] = false;
				mpBoundary[i-1] = RectI(m_MpBarBoundary.point + m_TeammateBoundary[i-1].point,m_MpBarBoundary.extent);
				if (mpBoundary[i-1].pointInRect(local_position))
					m_MouseOnMp[i-1] = true;
				else
					m_MouseOnMp[i-1] = false;
			}
		}
		else
		{
			for (int i = 1; i <= 5; i++)
			{
				m_MouseOnHp[i-1] = true;
				m_MouseOnMp[i-1] = true;
			}
		}
		setUpdate();
	}
}

void GuiTeammateIcon::IsUseMouseMethod(S32 isUse)
{
	if (isUse == 1)
	{
		for (int i = 1; i <= 5; i++)
		{
			m_MouseOnHp[i-1] = true;
			m_MouseOnMp[i-1] = true;
		}
	}
	else
	{
		for (int i = 1; i <= 5; i++)
		{
			m_MouseOnHp[i-1] = false;
			m_MouseOnMp[i-1] = false;
		}
	}
	m_IsUseMouseMethod = isUse;
}

void GuiTeammateIcon::selectTeamIcon(U32 index)
{
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(!conn)
		return;

	Player* control_player = dynamic_cast<Player*>(conn->getControlObject());
	if(!control_player)
		return;

	if(index > CLIENT_TEAM->GetPlayerList()->size())
		return;

	S32 playerID = CLIENT_TEAM->GetPlayerByIndex(index,false);
	if(playerID == 0)
		return;

	Player* player = PLAYER_MGR->GetLocalPlayer(playerID);
	if(!player)
		return;

	control_player->SetTarget(player,false);
}

//------------------------------------------ICONbase---------------------------------------------
ConsoleMethod( GuiIconBase, setCountType, void , 3, 3, "obj.setCountType( type )")
{
    object->setCountType(dAtoi(argv[2]));
}

ConsoleMethod( GuiIconBase, IsShowTextInfo, void , 3, 3, "obj.IsShowTextInfo( isshow )")
{
    object->IsShowTextInfo(dAtoi(argv[2]));
}

ConsoleMethod( GuiIconBase, IsUseMouseMethod, void , 3, 3, "obj.IsUseMouseMethod( isshow )")
{
    object->IsUseMouseMethod(dAtoi(argv[2]));
}

//------------------------------------------Player---------------------------------------------

ConsoleFunction(selectSelfIcon,void,1,1,"selectSelfIcon()")
{
	argc,argv;
	GuiPlayerIcon::selectSelfIcon();
}
//--------------------------------------------------------------------------------------------

//------------------------------------------Object--------------------------------------------

ConsoleMethod(GuiObjectIcon, setTargetNull, void, 2, 2, "obj.setTargetNull()")
{
	object->setTargetNull();
}
//-------------------------------------------------------------------------------------------

//---------------------------------------Teammate--------------------------------------------

ConsoleFunction(selectTeamIcon,void,2,2,"selectTeamIcon(%index)")
{
	argc,argv;
	U32 index = dAtoi(argv[1]);

	if (index <0 || index >5)
		return;

	GuiTeammateIcon::selectTeamIcon(index);
}
//-------------------------------------------------------------------------------------------



//==================================================================================================================================
//    GuiPlayerIconPopUpMenu
//==================================================================================================================================
IMPLEMENT_CONOBJECT(GuiPlayerIconPopUpMenu);

GuiPlayerIconPopUpMenu::GuiPlayerIconPopUpMenu()
{

}

void GuiPlayerIconPopUpMenu::onMouseDown(const GuiEvent &event)
{
    getRoot()->popDialogControl( 99 );
}

void GuiPlayerIconPopUpMenu::onRightMouseDown(const GuiEvent &event)
{
     getRoot()->popDialogControl( 99 );
}