
#include <windows.h>
#include "guiScreenShowCtrl.h"
#include "T3D/gameConnection.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "gfx/primBuilder.h"
#include "Gameplay/ClientGameplayState.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK
GFXStateBlock* GuiScreenShowCtrl::mSetSB = NULL;

IMPLEMENT_CONOBJECT(GuiScreenShowCtrl);
GuiScreenShowCtrl::GuiScreenShowCtrl()
{
  m_ScreenTextureName  = StringTable->insert("");
  m_ScreenTexture      = NULL;
  m_IsStartChangeAlpha = false;
}

void GuiScreenShowCtrl::initPersistFields()
{
	Parent::initPersistFields();
	addGroup("GuiScreenShowCtrl");
	addField("ScreenTextureName" ,  TypeString,  Offset(m_ScreenTextureName, GuiScreenShowCtrl));
	addField("IsStartChangeAlpha",  TypeBool,    Offset(m_IsStartChangeAlpha,GuiScreenShowCtrl));
	addField("alpha",				TypeS32,    Offset(alpha,GuiScreenShowCtrl));
	endGroup("GuiScreenShowCtrl");

}

//void GuiScreenShowCtrl::loadScreenTexture()
//{
//  if (m_ScreenTextureName)
//	  m_ScreenTexture.set(m_ScreenTextureName,&GFXDefaultGUIProfile);
//  else
//	  m_ScreenTexture.free();
//}

void GuiScreenShowCtrl::onPreRender()
{
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(!conn)
		return;
}



void GuiScreenShowCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(!conn)
		return;

	Player* control_Player = dynamic_cast<Player*>(conn->getControlObject());
	if(!control_Player)
		return;

	GFX->setBaseRenderState();
#ifdef STATEBLOCK
	AssertFatal(mSetSB, "GuiScreenShowCtrl::onRender -- mSetSB cannot be NULL.");
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
	GFX->setTexture( 0, m_ScreenTexture );
	GFX->setTexture( 1, NULL);

    if(m_IsStartChangeAlpha && m_dwLastTime == 0 )
		m_dwLastTime = ::GetTickCount();

	if( !m_IsStartChangeAlpha )
		m_dwLastTime = 0;

	alpha = ( ::GetTickCount() - m_dwLastTime ) * ( 255.0f / 3000 );

    if(m_ScreenTexture)
	{	  
	  GFX->getDrawUtil()->clearBitmapModulation();
      RectI SrcRect,DestRect;
	  SrcRect.set(0,0,m_ScreenTexture.getWidth(),m_ScreenTexture.getHeight());
	  DestRect.point.x = offset.x;
	  DestRect.point.y = offset.y;
	  DestRect.extent.x = m_ScreenTexture.getWidth();
	  DestRect.extent.y = m_ScreenTexture.getHeight();
	  
	  if( m_IsStartChangeAlpha )
		GFX->getDrawUtil()->setBitmapModulation( ColorI(255,255,255,255-alpha) );
	  else
	    GFX->getDrawUtil()->clearBitmapModulation();

	  GFX->getDrawUtil()->drawBitmapStretchSR(m_ScreenTexture,DestRect,SrcRect);
	}
}

bool GuiScreenShowCtrl::onWake()
{
	if(!Parent::onWake())
		return false;

	//loadScreenTexture();

	char AreaTrigger_file[256];
	dSprintf(AreaTrigger_file,sizeof(AreaTrigger_file),"gameres\\gui\\images\\logo%d",g_ClientGameplayState->getCurrentZoneId());
	m_ScreenTexture.set(AreaTrigger_file,&GFXDefaultGUIProfile);
    alpha = 255;
	return true;
}

void GuiScreenShowCtrl::onSleep()
{
  m_ScreenTexture.free();
  Parent::onSleep();
}


void GuiScreenShowCtrl::resetStateBlock()
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


void GuiScreenShowCtrl::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}
}

void GuiScreenShowCtrl::init()
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

void GuiScreenShowCtrl::shutdown()
{
	SAFE_DELETE(mSetSB);
}

void GuiScreenShowCtrl::setTranSportTexture(const char* Name)
{
	char AreaTrigger_file[256];
	dSprintf(AreaTrigger_file,sizeof(AreaTrigger_file),"gameres\\gui\\images\\%s",Name);
	m_ScreenTexture.set(AreaTrigger_file,&GFXDefaultGUIProfile);

	alpha = 255;
	m_IsStartChangeAlpha = false;

}

ConsoleMethod(GuiScreenShowCtrl,ShowTranSportTexture,void, 3, 3, "ShowTranSportTexture(%Name)")
{
	object->setTranSportTexture(argv[2]);
}
