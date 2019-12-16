//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include <d3dx9math.h>
#include "console/consoleTypes.h"
#include "gfx/primBuilder.h"
#include "gfx/D3D9/gfxD3D9Device.h"
#include "Gameplay/item/Res.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/item/NpcShopData.h"
#include "UI/guiRichTextCtrl.h"
#include "Gameplay/GameObjects/BuffData.h"
#include "Gameplay/Item/Player_EquipIdentify.h"
#include "Gameplay/Item/Player_GemMount.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "UI/dGuiShortCut.h"
#include "UI/dGuiMouseGamePlay.h"
#include "Gameplay/GameObjects/PetOperationManager.h"
#include "Gameplay/GameObjects/PetMountOperation.h"
#include "Gameplay/Item/IndividualStall.h"

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/GameObjects/LivingSkillShortcut.h"
#include "Gameplay/Item/Player_Bank.h"
#include "Gameplay/Item/PetShortcut.h"
#include "Gameplay/Item/SpiritShortcut.h"
#include "Gameplay/Item/TalentShortcut.h"
#endif
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK
GFXStateBlock* dGuiShortCut::mSetCoolSB = NULL;
GFXStateBlock* dGuiShortCut::mClearCoolSB = NULL;

#include "gfx/gfxFontRenderBatcher.h"
#include "Util/Base64.h"
#include "gui/controls/guiTextEditCtrl.h"
CommonFontEX* dGuiShortCut::mCoolDownFont[3] = {NULL,NULL,NULL};
GFXTexHandle dGuiShortCut::mNumTexture = NULL;
GFXTexHandle dGuiShortCut::mSelectTexture = NULL;
GFXTexHandle dGuiShortCut::mEffectTexture = NULL;
GFXTexHandle dGuiShortCut::mCoolFlashTexture = NULL;

#define SELECTFILENAME		"gameres/gui/images/slotselect.png"
#define ICONFILENAME_ITEM	"gameres/data/icon/item/"
#define ICONFILENAME_SKILL	"gameres/data/icon/skill/"
#define ICONFILENAME_PET	"gameres/data/icon/pet/"
#define ICONFILENAME_MOUNT	"gameres/data/icon/mount/"
#define ICONFILENAME_SPIRIT	"gameres/data/icon/spirit/"
#define ICONFILENAME_TALENT	"gameres/data/icon/"
#define ICONNUM				"gameres/gui/images/Uinum.png"
#define UNKNOWICON			"gameres/gui/images/icon_unknown.png"
#define COOLFLASHFILE		"gameres/gui/images/coolflash.png"
#define ICONEFFECTFILE		"gameres/gui/images/GUIEffects03_1_002.png"

std::string g_itemLink;
std::string g_itemLinkName;

IMPLEMENT_CONOBJECT(dGuiShortCut);

dGuiShortCut::dGuiShortCut()
{
	mBoundary.set(0,0,32,32);
	mMousePos.set(0,0);

	mIconTextureName	= StringTable->insert("");
	mTexture			= NULL;

	mEffectPosX			= 0;
	mEffectPosY			= 0;
	mLastTime_Effect	= 0;
	mLoopEffects		= 0;
	mLoopTimes			= 0;

	mCoolPosX			= 0;
	mCoolPosY			= 0;
	mLastTime_CoolFlash = 0;

	mNum[0]				= 0;
	mNum[1]				= 0;
	mNum[2]				= 0;
	mFrames				= 0; 
	
	mMouseOverCommand		= StringTable->insert("");
	mMouseLeaveCommand		= StringTable->insert("");
	mMouseDownCommand		= StringTable->insert("");
	mRightMouseDownCommand	= StringTable->insert("");

	mSlotType			= SHORTCUTTYPE_PANEL;
	mSlotIndex			= 0;

	mIsEnableSelect		= false;
	mIsEnableEffect		= false;
	mIsDisableDrag		= false;
	mIsShowNum			= false;
	mIsMoveIn			= false;
	mIsForbided			= false;
	mIsCool				= false;
	mIsShowCoolFlash	= false;
	mIsRenderFrame		= false;
	mLastFrame			= false;
	
	mTextCtrl			= NULL;
	mCompareTextCtrl    = NULL;
	mOldSlot			= NULL;
	mOldNum				= 0;
}

dGuiShortCut::~dGuiShortCut()
{
	mTexture			= NULL;
}

void dGuiShortCut::initPersistFields()
{
	Parent::initPersistFields();
	addField("slotType",			TypeS32,	Offset(mSlotType,			dGuiShortCut));
	addField("slotCol",				TypeS32,	Offset(mSlotIndex,			dGuiShortCut));
	addField("boudary",				TypeRectI,  Offset(mBoundary,			dGuiShortCut));
	addField("disabledrag",			TypeBool,	Offset(mIsDisableDrag,		dGuiShortCut));
	addField("MouseOverCommand",	TypeString, Offset(mMouseOverCommand,	dGuiShortCut));
	addField("MouseLeaveCommand",	TypeString, Offset(mMouseLeaveCommand,	dGuiShortCut));
	addField("MouseDownCommand",	TypeString, Offset(mMouseDownCommand,	dGuiShortCut));
	addField("RightMouseDownCommand",	TypeString, Offset(mRightMouseDownCommand,	dGuiShortCut));	
}

bool dGuiShortCut::resize(const Point2I &newPosition, const Point2I &newExtent)
{
	Point2I oldsize = mBoundary.extent;
	Parent::resize(newPosition, newExtent);
	Point2I newsize = mBoundary.extent;
	resizeBoundary(mBoundary, oldsize, newsize);
	return true;
}

bool dGuiShortCut::onWake()
{
	if(!Parent::onWake())
		return false;	

	if(!mTextCtrl)
		mTextCtrl = dynamic_cast<GuiRichTextCtrl*>(Sim::findObject("ShowItemInfo"));	

	if(!mCompareTextCtrl)
		mCompareTextCtrl = dynamic_cast<GuiRichTextCtrl*>(Sim::findObject("CompareShowItemInfo"));
	return true;
}

void dGuiShortCut::onSleep()
{
	mTextCtrl			= NULL;
	mCompareTextCtrl	= NULL;
	Parent::onSleep();
}

void dGuiShortCut::resizeBoundary(const RectI& bound, const Point2I& oldsize, const Point2I& newsize)
{
	if(oldsize.x == 0 || oldsize.y == 0)
		return;

	RectI old_boundary = bound;
	mBoundary.point.x	= S32(F32(old_boundary.point.x)	/ F32(oldsize.x)	* F32(newsize.x) + 0.55f);
	mBoundary.point.y	= S32(F32(old_boundary.point.y)	/ F32(oldsize.y)	* F32(newsize.y) + 0.55f);
	mBoundary.extent.x	= S32(F32(old_boundary.extent.x)/ F32(oldsize.x)	* F32(newsize.x) + 0.55f);
	mBoundary.extent.y	= S32(F32(old_boundary.extent.y)/ F32(oldsize.y)	* F32(newsize.y) + 0.55f);
}


//=============================================================================
// 渲染处理
//=============================================================================

void dGuiShortCut::onPreRender()
{
#ifdef NTJ_CLIENT
	m_DirtyFlag = updateSlot();
	if(!m_DirtyFlag)
		Parent::onPreRender();
#endif
}

void dGuiShortCut::onRender(Point2I offset, const RectI &updateRect)
{
	m_DirtyFlag = false;
	GFX->getDrawUtil()->clearBitmapModulation();

#ifdef NTJ_CLIENT
	// 画ICON
	if(mTexture)
	{
		GFX->getDrawUtil()->drawBitmapStretch(mTexture, RectI(offset + mBoundary.point, mBoundary.extent));
		// 画ICON上数字
		drawIconNum(offset, updateRect);
		// 画冷却样式
		drawCoolDown(offset, updateRect);
		// 画冷却后闪动动画图片
		drawCoolFlash(offset, updateRect);
		// 画禁止样式
		drawForbided(offset, updateRect);
		// 画物品品质框样式
		drawFrame(offset, updateRect);		
	}	

	// 画鼠标移上去的效果
	drawMoveIn(offset, updateRect);
	// 画SLOT被选中的效果
	drawBeSelected(offset, updateRect);	
	// 画SLOT边框特效
	drawBorderEffect(offset, updateRect);
#endif

#ifdef NTJ_EDITOR
	GFX->getDrawUtil()->drawRectFill(RectI(offset + mBoundary.point, mBoundary.extent), ColorI(255, 255, 255, 255));
#endif
}

#ifdef NTJ_CLIENT
void dGuiShortCut::onMouseUp(const GuiEvent &event)
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return;
	if(mSlotType == SHORTCUTTYPE_AUTOSELL)
	{
		if(player->autoSellList.getShowType() < 3)
			Con::executef("addCommonToAutoList", Con::getIntArg(mSlotIndex));
	}
	// 判断是否允许拖动或禁止操作
	if (mIsForbided)
		return;

	ShortcutObject* pCurrentSlot = g_ItemManager->getShortcutSlot(player, mSlotType, mSlotIndex);
	
	ClientGameplayAction* currentAction = g_ClientGameplayState->getCurrentAction();
	if(currentAction)
	{
		switch(currentAction->getActionType())
		{
		case INFO_SHORTCUT_EXCHANGE:
			{
				ClientGameplayParam* param0 = currentAction->getParam(0);
				if(param0->getIntArg(0) == mSlotType  && param0->getIntArg(1) == mSlotIndex)
				{
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}
				if(pCurrentSlot && pCurrentSlot->getSlotState() == ShortcutObject::SLOT_LOCK &&  param0->getIntArg(0) == SHORTCUTTYPE_INVENTORY)
				{
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}
				//摆摊收购蓝存在物品直接返回
				if (pCurrentSlot && mSlotType == SHORTCUTTYPE_STALL_BUYLIST)
				{
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}
				enWarnMessage msg = checkShortcutMove(param0->getIntArg(0), param0->getIntArg(1));
				if ( msg == MSG_WAITING_CONFIRM)	//等待玩家确认操作
					return;
				else if ( msg != MSG_NONE)
				{
					g_ClientGameplayState->cancelCurrentAction();
					MessageEvent::show(SHOWTYPE_NOTIFY, msg);
					return;
				}

				ClientGameplayParam* param1 = new ClientGameplayParam;
				param1->setIntArgValues(2, mSlotType, mSlotIndex);
				g_ClientGameplayState->setCurrentActionParam(param1);
			}
			break;
		case INFO_ITEM_REPAIR:		// 物品修理
			{
				if(!pCurrentSlot || (mSlotType != SHORTCUTTYPE_INVENTORY && mSlotType != SHORTCUTTYPE_EQUIP))
				{
					g_ClientGameplayState->cancelCurrentAction();
					MessageEvent::show(SHOWTYPE_NOTIFY, "选择物品位置不对！");
					return;
				}

				ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pCurrentSlot);
				if(!pItem)
				{
					MessageEvent::show(SHOWTYPE_NOTIFY, "物品数据错误！");
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}
				//判断物品能否修理
				if(!pItem->getRes()->canBaseLimit(Res::ITEM_FIX))
				{
					//物品无法修理
					MessageEvent::show(SHOWTYPE_ERROR, MSG_ITEM_CANNOTFIXED);
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}
				//判断是否有足够的修理费
				if(!player->canReduceMoney(pItem->getRes()->getRepairNeedMoney(), 1))
				{
					MessageEvent::show(SHOWTYPE_ERROR, MSG_PLAYER_MONEYNOTENOUGH);
				   g_ClientGameplayState->cancelCurrentAction();
					return;				
				}

				ClientGameplayParam* param = new ClientGameplayParam;
				param->setInt32ArgValues(3, NPCSHOP_REPAIRSIGLE, mSlotType, mSlotIndex);
				g_ClientGameplayState->setCurrentActionParam(param);
			}
			break;
		case INFO_ITEM_SPLIT:		// 物品拆分
			{
				if(mSlotType != SHORTCUTTYPE_INVENTORY)
				{
					MessageEvent::show(SHOWTYPE_NOTIFY, "此物品无法拆分！");
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}

				if(currentAction->getCurrentParamCount() == 0)
				{
					if(!pCurrentSlot)
					{
						MessageEvent::show(SHOWTYPE_NOTIFY, "目标物品不存在！");
						g_ClientGameplayState->cancelCurrentAction();
						return;
					}

					ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pCurrentSlot);
					Res* pRes = NULL;
					if(!pItem || !(pRes = pItem->getRes()))
					{
						// 拆分物品数量不够
						//MessageEvent::show(SHOWTYPE_ERROR, MSG_ITEM_CANNOTFIXED);
						g_ClientGameplayState->cancelCurrentAction();
						MessageEvent::show(SHOWTYPE_NOTIFY, MSG_ITEM_ITEMDATAERROR);
						return;
					}
					U32 iHaveNum = pRes->getQuantity();
					if(iHaveNum <= 1)
					{
						g_ClientGameplayState->cancelCurrentAction();
						MessageEvent::show(SHOWTYPE_NOTIFY, "拆分数量不够");
						return;
					}
					
					ClientGameplayParam* param = new ClientGameplayParam;
					param->setIntArgValues(2, mSlotType, mSlotIndex);
					g_ClientGameplayState->setCurrentActionParam(param);
					// 弹出拆分物品窗口
					Con::executef("PopupSplitItem", Con::getIntArg(mSlotIndex), Con::getIntArg(iHaveNum));
				}
				else
				{
					if(pCurrentSlot)
					{
						ClientGameplayParam* param0 = currentAction->getParam(0);
						ClientGameplayParam* param1 = currentAction->getParam(1);
						ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pCurrentSlot);
						if(!pItem || !param0 || !param1)
						{
							g_ClientGameplayState->cancelCurrentAction();
							MessageEvent::show(SHOWTYPE_NOTIFY, "物品错误！");
							return;
						}
						
						ShortcutObject* pSrcSlot = g_ItemManager->getShortcutSlot(player, param0->getIntArg(0), param0->getIntArg(1));
						if(!g_ItemManager->isSameItem(pCurrentSlot, pSrcSlot))
						{
							// 不是同一个物品，无法叠加
							MessageEvent::show(SHOWTYPE_ERROR, MSG_ITEM_CANNOTDRAGTOOBJECT);
							g_ClientGameplayState->cancelCurrentAction();
							return;
						}

						if((pItem->getRes()->getQuantity() + param1->getIntArg(0)) > pItem->getRes()->getMaxOverNum())
						{
							MessageEvent::show(SHOWTYPE_ERROR, MSG_ITEM_CANNOTDRAGTOOBJECT);
							g_ClientGameplayState->cancelCurrentAction();
							return;
						}
					}

					ClientGameplayParam* param2 = new ClientGameplayParam;
					param2->setIntArgValues(2, mSlotType, mSlotIndex);
					g_ClientGameplayState->setCurrentActionParam(param2);
				}									
			}
			break;
		case INFO_ITEM_DROP:		// 物品丢弃
			{
				if(!pCurrentSlot || (mSlotType != SHORTCUTTYPE_INVENTORY))
				{
					MessageEvent::show(SHOWTYPE_NOTIFY, "物品错误！");
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}

				ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pCurrentSlot);
				Res* pRes = NULL;
				if(!pItem || !(pRes = pItem->getRes()))
				{
					MessageEvent::show(SHOWTYPE_NOTIFY, "物品错误！");
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}
				if(!pRes->canBaseLimit(Res::ITEM_DROP))
				{
					MessageEvent::show(SHOWTYPE_NOTIFY, "此物品不能丢弃！");
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}
				char buff[256] = "\0";
				char tempbuffer[128] = "\0";
				dSprintf(buff, sizeof(buff), "<t  m='0'  c='0xffffffff'>%s</t>", "你确定要销毁【");
				pRes->getItemName(tempbuffer, 128);
				dStrcat(buff, sizeof(buff), tempbuffer);
				U32 iCount = pRes->getQuantity();
				dSprintf(tempbuffer, sizeof(tempbuffer), "<t  m='0' c='0xffffffff'>%s%d%s</t>", "】×", iCount, " 吗?");
				dStrcat(buff, sizeof(buff), tempbuffer);

				Con::executef("DorpItemPopUp", buff, Con::getIntArg(mSlotIndex));
			}
			break;
		case INFO_ITEM_IDENTIFY:	// 物品鉴定
			{
				ClientGameplayParam* param0 = currentAction->getParam(0);
				U32 iIndex = param0->getIntArg(2);
				if(param0->getIntArg(1) != SHORTCUTTYPE_INVENTORY /*&& param0->getIntArg(1) != SHORTCUTTYPE_EQUIP*/)
				{
					g_ClientGameplayState->cancelCurrentAction();
					MessageEvent::show(SHOWTYPE_NOTIFY, MSG_ITEM_SELECTTARGET_ERROR);
					return;
				}
				if(iIndex == mSlotIndex)
				{
					MessageEvent::show(SHOWTYPE_NOTIFY, MSG_ITEM_SELECTTARGET_ERROR);
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}
				ShortcutObject* pShortObj = g_ItemManager->getShortcutSlot(player, SHORTCUTTYPE_INVENTORY, iIndex);
				if(!pShortObj)
				{
					MessageEvent::show(SHOWTYPE_NOTIFY, MSG_ITEM_ITEMDATAERROR);
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}
				ItemShortcut* pItem1 = dynamic_cast<ItemShortcut*>(pShortObj);
				ItemShortcut* pItem2 = dynamic_cast<ItemShortcut*>(pCurrentSlot);
				if(!pItem1 || !pItem2)
				{
					MessageEvent::show(SHOWTYPE_NOTIFY, MSG_ITEM_ITEMDATAERROR);
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}
				//if(pItem1->getRes()->getLimitLevel() < pItem2->getRes()->getLimitLevel())
				//{
				//	//无法鉴定
				//	MessageEvent::show(SHOWTYPE_ERROR, MSG_ITEM_CANNOTIDENT);
				//	g_ClientGameplayState->cancelCurrentAction();
				//	return;
				//}
				enWarnMessage msg = gIdentifyManager->canIdentify(player, pCurrentSlot, pShortObj);
				if(msg != MSG_NONE)
				{
					//无法鉴定
					MessageEvent::show(SHOWTYPE_ERROR, msg);
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}
				ClientGameplayParam* param1 = new ClientGameplayParam;
				param1->setIntArgValues(2, mSlotType, mSlotIndex);
				g_ClientGameplayState->setCurrentActionParam(param1);
			}
			break;
		case INFO_ITEM_IMPRESS:		// 物品刻铭
			{
				ClientGameplayParam* param0 = currentAction->getParam(0);
				U32 iIndex = param0->getIntArg(1);

				if(param0->getIntArg(0) != SHORTCUTTYPE_INVENTORY || param0->getIntArg(0) != SHORTCUTTYPE_EQUIP)
				{
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}
				if(iIndex == mSlotIndex)
				{
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}
				ShortcutObject* pShortObj = g_ItemManager->getShortcutSlot(player, SHORTCUTTYPE_INVENTORY, iIndex);
				if(!pShortObj)
				{
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}
				if(EquipImpress::canImpress(player, pCurrentSlot, pShortObj) != MSG_NONE)
				{
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}
				
				ClientGameplayParam* param1 = new ClientGameplayParam;
				param1->setIntArgValues(1, mSlotIndex);
				g_ClientGameplayState->setCurrentActionParam(param1);
			}
			break;
		case INFO_ITEM_SOULLINK:	// 物品灵魂链接
			{

			}
			break;
		case INFO_NPCSHOP:
			{		
				if(mSlotType != SHORTCUTTYPE_NPCSHOP)
				{
					g_ClientGameplayState->cancelCurrentAction();
					Con::executef("clearBuyFlag");
					return;
				}
				if(!pCurrentSlot)
				{
					g_ClientGameplayState->cancelCurrentAction();
					Con::executef("clearBuyFlag");
					return;
				}
				if(pCurrentSlot->getSlotState() == ShortcutObject::SLOT_LOCK)
				{
					g_ClientGameplayState->cancelCurrentAction();
					Con::executef("clearBuyFlag");
					return;
				}
				if(mSlotIndex < 0 || mSlotIndex >= NpcShopList::MAXSLOTS)
				{
					g_ClientGameplayState->cancelCurrentAction();
					Con::executef("clearBuyFlag");
					return;
				}
				ItemShortcut* pItem = (ItemShortcut*)(pCurrentSlot);
				if(!pItem)
				{
					g_ClientGameplayState->cancelCurrentAction();
					Con::executef("clearBuyFlag");
					return;
				}
				//条件判断，是否可以购买
				NpcShopItem* pShopItem = player->npcShopList.mSlots[mSlotIndex];
				if(!pShopItem)
					return;
				char tempbuffer[128] = "\0";
				pItem->getRes()->getItemName(tempbuffer, 128);

				Con::executef("buyGoodFromShop", Con::getIntArg(mSlotIndex), tempbuffer, Con::getIntArg(pShopItem->CurrencyID), Con::getIntArg(pShopItem->CurrencyValue));		
			}
			break;
		case INFO_ITEM_BATCHBUY:
			{			
				if(mSlotType != SHORTCUTTYPE_NPCSHOP)
				{
					g_ClientGameplayState->cancelCurrentAction();
					Con::executef("clearBuyFlag");
					return;
				}
				if(!pCurrentSlot)	
				{
					g_ClientGameplayState->cancelCurrentAction();
					Con::executef("clearBuyFlag");
					return;
				}
				if(pCurrentSlot->getSlotState() == ShortcutObject::SLOT_LOCK)
				{
					g_ClientGameplayState->cancelCurrentAction();
					Con::executef("clearBuyFlag");
					return;
				}
				if(mSlotIndex < 0 || mSlotIndex >= NpcShopList::MAXSLOTS)
				{
					g_ClientGameplayState->cancelCurrentAction();
					Con::executef("clearBuyFlag");
					return;
				}

				ItemShortcut* pItem = (ItemShortcut*)(pCurrentSlot);
				if(!pItem)
				{
					g_ClientGameplayState->cancelCurrentAction();
					Con::executef("clearBuyFlag");
					return;
				}

				Con::executef("buyGoodsFromShop", Con::getIntArg(mSlotIndex));
			}
			break;
		case INFO_ITEM_BUY:
			{
				if(mSlotType != SHORTCUTTYPE_INVENTORY || !pCurrentSlot)
				{
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}
				ItemShortcut* pItem = (ItemShortcut*)(pCurrentSlot);
				if(!pItem)
				{
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}

				if(!pItem->getRes()->canBaseLimit(Res::ITEM_SELLNPC))
				{
					MessageEvent::show(SHOWTYPE_NOTIFY, "此物品不能出售");
					return;
				}
				// 发送卖出商品消息给服务端
				ClientGameNetEvent* event = new ClientGameNetEvent(INFO_NPCSHOP);
				event->SetIntArgValues(2, NPCSHOP_SELL, mSlotIndex);
				player->getControllingClient()->postNetEvent(event);

			}
			break;
		}
	}
	else
	{
		if(event.modifier & SI_SHIFT)
		{
			//if( mSlotType != SHORTCUTTYPE_INVENTORY )
			//	return ;

			ItemShortcut* pItem = (ItemShortcut*)pCurrentSlot;

			GuiTextEditCtrl* pCtrl = (GuiTextEditCtrl*)Sim::findObject( "InputWnd" );
			
			char chatText[GuiTextEditCtrl::MAX_STRING_LENGTH+1] = { 0, };

			if( pCtrl )
				pCtrl->getText(chatText);

			Res* pRes = NULL;
			
			if(!pItem || !(pRes = pItem->getRes()))
				return;

			stItemInfo* pIteminfo = pRes->getExtData();

			char buf[MAX_PACKET_SIZE];
			Base::BitStream info( buf, MAX_PACKET_SIZE );
			pIteminfo->WriteData( &info );

			int size = info.getPosition();

			char destBuf[MAX_PACKET_SIZE];
			dBase64::encode( (unsigned char*)buf, destBuf, size, 10240 );

			char destText[MAX_PACKET_SIZE];
			sprintf_s( destText, MAX_PACKET_SIZE, "<l i='%s' t='item'/>", destBuf );

			g_itemLink = destText;
			g_itemLinkName = "【";
			g_itemLinkName += pRes->getItemName();
			g_itemLinkName += "】";

			std::string inputText = chatText;
			inputText += g_itemLinkName;

			if( pCtrl )
				pCtrl->setText( inputText.c_str() );
		}
	}
}

void dGuiShortCut::onMouseDown(const GuiEvent &event)
{
	// 触发控件绑定鼠标操作脚本
	if (mMouseDownCommand && mMouseDownCommand[0])
	{
		Con::setVariable("$ThisControl",avar("%d",getId()));
		Con::evaluate(mMouseDownCommand,false);
	}

	//判定是否可点击拖动和禁止操作
	if(mIsForbided)
		return;
	if(mSlotType == SHORTCUTTYPE_SHOPBASKET)
		return;

	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return;	

	ClientGameplayAction* currentAction = g_ClientGameplayState->getCurrentAction();
	if(currentAction)
		return;

	ShortcutObject* pSlot = g_ItemManager->getShortcutSlot(player, mSlotType, mSlotIndex);
	if(pSlot == NULL)
		return;

	if(pSlot->getSlotState() == ShortcutObject::SLOT_LOCK)
		return;

	if( event.modifier & SI_SHIFT )
		return ;

	// 设置物品交换操作行为
	ClientGameplayAction* action = new ClientGameplayAction(2, INFO_SHORTCUT_EXCHANGE);	
	g_ClientGameplayState->setCurrentAction(action);

	ClientGameplayParam* param = new ClientGameplayParam;
	U32 mSkillID = 0;	

	if(mSlotType == SHORTCUTTYPE_SKILL_COMMON || mSlotType == SHORTCUTTYPE_SKILL_HINT )
	{
		SkillShortcut* pSkillScrSolt = dynamic_cast<SkillShortcut*>(pSlot);
		if(pSkillScrSolt && pSkillScrSolt->getSkill() && pSkillScrSolt->getSkill()->GetData())
		{
			SkillData* pSkill = pSkillScrSolt->getSkill()->GetData();
			mSkillID = pSkill->GetSeriesID();
		}
	}
	if (mSlotType == SHORTCUTTYPE_LIVINGCATEGORY)
	{
		LivingSkillShortcut* pSrcSlot = dynamic_cast<LivingSkillShortcut*>(pSlot);
		LivingSkillData* pData = NULL;
		if(pSrcSlot && pSrcSlot->getLivingSkill() && (pData = pSrcSlot->getLivingSkill()->getData()))
			mSkillID = pData->getID();
	}
	if(mSlotType == SHORTCUTTYPE_INVENTORY)
	{
		ItemShortcut* pItem = (ItemShortcut*)pSlot;
		if(pItem && pItem->getRes()->isEquip())
		{
			Con::executef("showEquipPos", 
					Con::getIntArg(pItem->getRes()->getEquipPos()),
					Con::getIntArg(0));
		}
	}
	param->setIntArgValues(2, mSlotType, mSlotIndex);
	param->setInt32ArgValues(1,mSkillID);
	g_ClientGameplayState->setCurrentActionParam(param);
}

void dGuiShortCut::onMouseEnter(const GuiEvent &event)
{
	Con::executef( this, "onMouseEnter" );

	// 触发控件绑定鼠标操作脚本
	if (mMouseOverCommand && mMouseOverCommand[0])
	{
		Con::setVariable("$ThisControl", avar("%d",getId()));
		Con::evaluate(mMouseOverCommand,false);
	}

	mIsMoveIn	= true;
	m_DirtyFlag = true;
	mMousePos = event.mousePoint;
	showHotInfo();

}

void dGuiShortCut::showHotInfo()
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;

	ShortcutObject* pSlot = g_ItemManager->getShortcutSlot(pPlayer, mSlotType, mSlotIndex);	
	if(!pSlot)
		return;

	char cDesc[4096];
	dMemset(cDesc, 0, 4096);
	char cTempPath[128] = "\0";

	char cCompareDesc[4096];
	dMemset(cCompareDesc, 0, 4096);
	char cCompareTempPath[128] = "\0";

	if(pSlot->isItemObject())
	{
		ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pSlot);		
		if(pItem)
		{
			g_ItemManager->insertText(pPlayer, pItem->getRes(), cDesc,mSlotType,mSlotIndex);	
			dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_ITEM, pSlot->getIconName());

			if (pItem->getRes()->isEquip())
			{
				S32 iEquipPos = pItem->getRes()->getEquipPos();
				ItemShortcut* pEquipItem = (ItemShortcut*)pPlayer->equipList.GetSlot(iEquipPos);
				if (pEquipItem)
				{
					g_ItemManager->insertText(pPlayer, pEquipItem->getRes(), cCompareDesc,SHORTCUTTYPE_EQUIP,iEquipPos);	
					dSprintf(cCompareTempPath, sizeof(cCompareTempPath), "%s%s", ICONFILENAME_ITEM, pEquipItem->getIconName());
				}
			}
		}		
	}
	else if(pSlot->isSkillObject())
	{
		SkillData* pData = NULL;
		SkillShortcut* pSkill= dynamic_cast<SkillShortcut*>(pSlot);
		SkillStudyShortcut* pSkillData = dynamic_cast<SkillStudyShortcut*>(pSlot);
		if (pSkill && (pData = pSkill->getSkill()->GetData()))
		{
			g_ItemManager->insertText(pPlayer, pData, cDesc,mSlotType,mSlotIndex);
			dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_SKILL, pSlot->getIconName());
		}
		if (pSkillData && (pData = pSkillData->getSkillData()))
		{
			g_ItemManager->insertText(pPlayer, pData, cDesc,mSlotType,mSlotIndex);
			dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_SKILL, pSlot->getIconName());
		}
	}
	else if (pSlot->isPetObject())
	{
		PetShortcut *pPetShortcut = dynamic_cast<PetShortcut *>(pSlot);
		stPetInfo *pPetInfo = NULL;
		if ( pPetShortcut )
		{
			Player *pSrcPlayer = (pPetShortcut->getTargetType() == PetShortcut::TARGET_SELF) ? pPlayer : dynamic_cast<Player *>(pPlayer->getInteraction());
			if ( pSrcPlayer && (pPetInfo = (stPetInfo *)pSrcPlayer->getPetTable().getPetInfo(pPetShortcut->getSlotIndex())) )
			{
				StringTableEntry sIconName = pPetShortcut->getIconName();
				if (sIconName && sIconName[0])
				{
					g_ItemManager->insertText(pPlayer, pPetInfo, cDesc, mSlotType, mSlotIndex);
					dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_PET, sIconName);
				}
			}			
		}
	}
	else if (pSlot->isMountObject())
	{
		MountShortcut *pMountShortcut = dynamic_cast<MountShortcut *>(pSlot);
		stMountInfo *pMountInfo = NULL;
		if ( pMountShortcut && (pMountInfo = (stMountInfo*)pPlayer->getPetTable().getMountInfo(pMountShortcut->getSlotIndex())) )
		{
			StringTableEntry sIconName = pMountShortcut->getIconName();
			if (sIconName && sIconName[0])
			{
				g_ItemManager->insertText(pPlayer, pMountInfo, cDesc, mSlotType, mSlotIndex);
				dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_MOUNT, sIconName);
			}
		}
	}
	else if (pSlot->isSpiritObject())
	{
		SpiritShortcut *pSpiritShortcut = dynamic_cast<SpiritShortcut *>(pSlot);
		stSpiritInfo *pSpiritInfo = NULL;
		if (pSpiritShortcut && (pSpiritInfo = (stSpiritInfo *)pPlayer->getSpiritTable().getSpiritInfo(pSpiritShortcut->getSlotIndex())))
		{
			StringTableEntry sIconName = pSpiritShortcut->getIconName();
			if (sIconName && sIconName[0])
			{
				g_ItemManager->insertText(pPlayer, pSpiritInfo, cDesc, mSlotType, mSlotIndex);
				dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_SPIRIT, sIconName);
			}
		}
	}
	else if (pSlot->isLivingSkillObject())
	{
		LivingSkillShortcut *pLivingShortcut = dynamic_cast<LivingSkillShortcut*>(pSlot);
        LivingSkillData *pData = NULL;
		if(pLivingShortcut && (pData = pLivingShortcut->getLivingSkill()->getData()))
		{
          g_ItemManager->insertText(pPlayer,pData,cDesc,mSlotType,mSlotIndex);
		  dSprintf(cTempPath,sizeof(cTempPath),"%s%s",ICONFILENAME_SKILL,pLivingShortcut->getIconName());
		}
	}
	else if (pSlot->isTalentObject())
	{
		TalentShortcut *pTalentShortcut = dynamic_cast<TalentShortcut *>(pSlot);
		stSpiritInfo *pSpiritInfo = NULL;
		if (pTalentShortcut)
		{
			StringTableEntry sIconName = pTalentShortcut->getIconName();
			if (sIconName && sIconName[0])
			{
				TalentData *pTalentData = pTalentShortcut->getTalentData();
				g_ItemManager->insertText(pPlayer, pTalentData, cDesc, mSlotType, mSlotIndex);
				dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_TALENT, sIconName);
			}
		}
	}

	if(mTextCtrl && cDesc[0])
	{
		mTextCtrl->getDrawer()->setDefaultLineHeight(16);
		mTextCtrl->setContent(cDesc);
		S32 iHight = mTextCtrl->getHeight() + 10;
		//Point2I	mMousePos = event.mousePoint;
		
		Point2I newPos;
		GuiCanvas* pCanvas = getRoot();
		S32 iWindowHight = 600;
		if(pCanvas)
			iWindowHight = pCanvas->getHeight();
		if((mMousePos.y + mTextCtrl->getHeight()) > iWindowHight)
		{
			newPos.y = iWindowHight - mTextCtrl->getHeight() - 15;
		}
		else
			newPos.y = mMousePos.y;// - mTextCtrl->getHeight()/2;

		if (mCompareTextCtrl && cCompareDesc[0])
		{
			mCompareTextCtrl->getDrawer()->setDefaultLineHeight(16);
			mCompareTextCtrl->setContent(cCompareDesc);
			S32 iCompareHight = mCompareTextCtrl->getHeight() + 10;
			if(mMousePos.x - 96 - mTextCtrl->getWidth() - mCompareTextCtrl->getWidth() < 0)
				newPos.x = mMousePos.x + 32 ;
			else
				newPos.x = mMousePos.x - 96 - mTextCtrl->getWidth() - mCompareTextCtrl->getWidth();

			Con::executef("ShowItemHot", Con::getIntArg(newPos.x + mCompareTextCtrl->getWidth() + 27), Con::getIntArg(newPos.y), Con::getIntArg(iHight), cTempPath);
			if(mSlotType != SHORTCUTTYPE_EQUIP)
				Con::executef("ShowCompareItemHot", Con::getIntArg(newPos.x), Con::getIntArg(newPos.y), Con::getIntArg(iCompareHight), cCompareTempPath);
		}
		else
		{
			if(mMousePos.x - 64 - mTextCtrl->getWidth() < 0)
				newPos.x = mMousePos.x + 32 ;
			else
				newPos.x = mMousePos.x - 64 - mTextCtrl->getWidth();

			Con::executef("ShowItemHot", Con::getIntArg(newPos.x), Con::getIntArg(newPos.y), Con::getIntArg(iHight), cTempPath);
		}
	}	
}

void dGuiShortCut::onMouseLeave(const GuiEvent &event)
{
	Con::executef( this, "onMouseLeave" );
	// 触发控件绑定鼠标操作脚本
	if (mMouseLeaveCommand && mMouseLeaveCommand[0])
	{
		Con::setVariable("$ThisControl",avar("%d",getId()));
		Con::evaluate(mMouseLeaveCommand,false);
	}

	mIsMoveIn	= false;
	m_DirtyFlag = true;
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return;

	//清除ShortCut的热感提示
	Con::executef("HideItemHot");
	Con::executef("HideCompareItemHot");
	if(mTextCtrl)
	{
		mTextCtrl->setContent("");
		mTextCtrl->refresh();
	}	
	if (mCompareTextCtrl)
	{
		mCompareTextCtrl->setContent("");
		mCompareTextCtrl->refresh();
	}

	ShortcutObject* pSlot = g_ItemManager->getShortcutSlot(player, mSlotType, mSlotIndex);
	if(pSlot == NULL)
		return;


	ClientGameplayAction* currentAction = g_ClientGameplayState->getCurrentAction();
	if(currentAction && currentAction->getActionType() == INFO_SHORTCUT_EXCHANGE)
	{
		// 判断是否禁止拖动,若可拖动则设置物品ICON
		if(mIsDisableDrag)
		{
			ClientGameplayParam* param1 = currentAction->getParam(0);
			if(param1->getIntArg(0) == mSlotType)
				g_ClientGameplayState->cancelCurrentAction();
		}
		else
			currentAction->activeCursor(pSlot->getCursorName());
	}
}

void dGuiShortCut::onMouseDragged(const GuiEvent &event)
{	
}

void dGuiShortCut::onMouseMove(const GuiEvent &event)
{	
}

void dGuiShortCut::onRightMouseUp(const GuiEvent &event)
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player || player->isDisabled())
		return;
	
	if(mIsForbided)
		return;

	ShortcutObject* pShortcut = g_ItemManager->getShortcutSlot(player,mSlotType, mSlotIndex);
	if(!pShortcut)
		return;

	ClientGameplayAction* currentAction = g_ClientGameplayState->getCurrentAction();
	if (currentAction) 
		g_ClientGameplayState->cancelCurrentAction();

	switch(mSlotType)
	{
	case SHORTCUTTYPE_INVENTORY:		// 点击在物品栏
		{
			ItemShortcut* pItem = NULL;
			// 再次确定是否为物品
			if(pShortcut->isItemObject() && (pItem = dynamic_cast<ItemShortcut*>(pShortcut)))
			{
				//注：对于物品的右键操作，目前只存在着交换物品、使用物品和卖出物品三种操作
				// 判定是否卖出物品操作
				if (player->getInteractionState() == Player::INTERACTION_NPCTRADE) // 是否在与NPC交易
				{
					// 判断是否能出售给NPC
					if(!pItem->getRes()->canBaseLimit(Res::ITEM_SELLNPC))
					{
						MessageEvent::show(SHOWTYPE_NOTIFY, "此物品不能出售");
						return;
					}

					// 绑定不能出售
				/*	if(pItem->getRes()->getBindPlayerID() > 0 && pItem->getRes()->IsActivatePro(EAPF_BIND))
					{
						MessageEvent::show(SHOWTYPE_NOTIFY, "此物品已经绑定，无法出售");
						return;
					}*/

					// 发送卖出商品消息给服务端
					ClientGameNetEvent* event = new ClientGameNetEvent(INFO_NPCSHOP);
					event->SetIntArgValues(2, NPCSHOP_SELL, mSlotIndex);
					player->getControllingClient()->postNetEvent(event);
				}
				//交换到交易栏
				else if(player->getInteractionState() == Player::INTERACTION_PLAYERTRADE)
				{
					ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pShortcut);
					if(!pSrcItem || pSrcItem->getSlotState() == ShortcutObject::SLOT_LOCK)
						return;

					// 判断是否能交易
					if(!pSrcItem->getRes()->canTrade())
					{
						MessageEvent::show(SHOWTYPE_NOTIFY, MSG_ITEM_CANNOTTRADE);
						return;
					}
					ClientGameNetEvent* event = new ClientGameNetEvent(INFO_TRADE);
					event->SetInt32ArgValues(2, TRADE_UPITEM, mSlotIndex);
					player->getControllingClient()->postNetEvent(event);
				}
				//交换到仓库
				else if(player->getInteractionState() == Player::INTERACTION_BANKTRADE)
				{
					if(g_PlayerBank->getLocked(player))
					{
						MessageEvent::show(SHOWTYPE_NOTIFY, "仓库被锁定");
						return;
					}
					ClientGameNetEvent* event = new ClientGameNetEvent(INFO_BANK);
					event->SetInt32ArgValues(2, PlayerBank::Bank_ItemMoveIn, mSlotIndex);
					player->getControllingClient()->postNetEvent(event);
				}
				//交换到鉴定栏
				else if(player->getInteractionState() == Player::INTERACTION_IDENTIFY)
				{
					ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pShortcut);
					Res* pRes = NULL;
					if(!pSrcItem || !(pRes = pSrcItem->getRes()))
						return ;
					if(pRes->getColorLevel() < 3)
						return;
					ClientGameplayAction* action = new ClientGameplayAction(2, INFO_SHORTCUT_EXCHANGE);
					g_ClientGameplayState->setCurrentAction(action);

					ClientGameplayParam* param0 = new ClientGameplayParam;
					param0->setIntArgValues(2, mSlotType, mSlotIndex);
					param0->setInt32ArgValues(1, 0);
					g_ClientGameplayState->setCurrentActionParam(param0);

					if(pRes->isWeapon() || pRes->isBody())
					{
						if(pRes->getIdentifyType() != Res::IDENTIFYTYPE_NPC)
						{
							g_ClientGameplayState->cancelCurrentAction();
							return ;
						}
						if(pRes->IsActivatePro(EAPF_ATTACH))
						{
							g_ClientGameplayState->cancelCurrentAction();
							return ;
						}
						ClientGameplayParam* param1 = new ClientGameplayParam;
						param1->setIntArgValues(2, SHORTCUTTYPE_IDENTIFY, 0);
						g_ClientGameplayState->setCurrentActionParam(param1);
					}
					else if(pRes->getSubCategory() != Res::CATEGORY_IDENTIFY)
					{
						ClientGameplayParam* param1 = new ClientGameplayParam;
						param1->setIntArgValues(2, SHORTCUTTYPE_IDENTIFY, 1);
						g_ClientGameplayState->setCurrentActionParam(param1);
					}
				}
				//交换到强化栏
				else if(player->getInteractionState() == Player::INTERACTION_STRENGTHEN)
				{
					ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pShortcut);
					enWarnMessage msg = MSG_NONE;
					S32 nDestIndex = 0;
					if (!EquipStrengthen::getEquipStrengthenSlotIndex(pSrcItem, nDestIndex))
						return;

					if ( (msg = EquipStrengthen::checkMoveFromInventory(player, pSrcItem, nDestIndex)) == MSG_NONE)
					{
						ClientGameplayAction* action = new ClientGameplayAction(2, INFO_SHORTCUT_EXCHANGE);
						g_ClientGameplayState->setCurrentAction(action);
						ClientGameplayParam* param = new ClientGameplayParam;
						param->setIntArgValues(2, mSlotType, mSlotIndex);
						param->setInt32ArgValues(1, 0);
						g_ClientGameplayState->setCurrentActionParam(param);
						ClientGameplayParam* param1 = new ClientGameplayParam;
						param1->setIntArgValues(2, SHORTCUTTYPE_STRENGTHEN, nDestIndex);
						g_ClientGameplayState->setCurrentActionParam(param1);
					}
					else
					{
						MessageEvent::show(SHOWTYPE_NOTIFY, msg);
						return;
					}
				}
				//交换到打孔栏
				else if(player->getInteractionState() == Player::INTERACTION_PUNCHHOLE)
				{
					ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pShortcut);
					enWarnMessage msg = MSG_NONE;
					if ( (msg = EquipPunchHole::checkMoveFromInventory(player, pSrcItem, 0)) == MSG_NONE)
					{
						ClientGameplayAction* action = new ClientGameplayAction(2, INFO_SHORTCUT_EXCHANGE);
						g_ClientGameplayState->setCurrentAction(action);
						ClientGameplayParam* param = new ClientGameplayParam;
						param->setIntArgValues(2, mSlotType, mSlotIndex);
						param->setInt32ArgValues(1, 0);
						g_ClientGameplayState->setCurrentActionParam(param);
						ClientGameplayParam* param1 = new ClientGameplayParam;
						param1->setIntArgValues(2, SHORTCUTTYPE_EQUIP_PUNCHHOLE, 0);
						g_ClientGameplayState->setCurrentActionParam(param1);
					}
					else
					{
						MessageEvent::show(SHOWTYPE_NOTIFY, msg);
						return;
					}
				}
				//交换到摆摊栏
				else if(player->getInteractionState() == Player::INTERACTION_STALLTRADE)
				{
					ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pShortcut);
					if(!pSrcItem)
						return ;

					// 判断物品是否可交易
					if(!pSrcItem->getRes()->canTrade())
					{
						MessageEvent::show(SHOWTYPE_WARN, MSG_ITEM_CANNOTTRADE);
						return ;
					}
					
					if(player->individualStallList.getStallState() == TRADE_LOCK)
					{
						MessageEvent::show(SHOWTYPE_WARN, MSG_TRADE_STALL_UNABLEUP);
						return ;
					}

					S32 index = player->individualStallList.findEmptyIndex();
					if(index >= 0)
						Con::executef("oPenSetPrice", Con::getIntArg(index), Con::getIntArg(mSlotIndex));
				}
				else if(player->getInteractionState() == Player::INTERACTION_REPAIR)// 修理
				{
					ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pShortcut);
					if(!pSrcItem)
						return ;
					Res* pRes = pSrcItem->getRes();
					if(!pRes)
						return ;
					if((!pRes->isWeapon()) && (pRes->isEquipment() && !(pRes->getSubCategory() < Res::CATEGORY_FASHION_FAMILY)))
						return ;

					GameConnection *conn = player->getControllingClient();
					if(!conn)
					 return ;

					ClientGameNetEvent *event = new ClientGameNetEvent(INFO_SHORTCUT_EXCHANGE);
					event->SetIntArgValues(4, SHORTCUTTYPE_INVENTORY, mSlotIndex, SHORTCUTTYPE_REPAIR, 0);
					event->SetInt32ArgValues(1, 0);
					conn->postNetEvent(event);
				}
				else
				{
					//交换到装备栏
					Res* pRes = pItem->getRes();
					if(!pRes)
					{
						MessageEvent::show(SHOWTYPE_NOTIFY, MSG_ITEM_ITEMDATAERROR);
						return;
					}
					if(!pRes->canBaseLimit(Res::ITEM_CANUSE))
					{	
						// 若同时有按下Shift键，则判定为镶嵌操作
						if(event.modifier & SI_SHIFT)
						{
							// 把镶嵌栏打开，并将装备拖动到镶嵌栏的装备槽中
							enWarnMessage msg = EquipMountGem::checkMoveFromInventory(player, pItem, 0);
							if (msg != MSG_NONE)
							{
								MessageEvent::show(SHOWTYPE_NOTIFY, msg);
								return;
							}

							GameConnection *conn = player->getControllingClient();
							ClientGameNetEvent *event = new ClientGameNetEvent(INFO_SHORTCUT_EXCHANGE);
							event->SetIntArgValues(4, SHORTCUTTYPE_INVENTORY, mSlotIndex, SHORTCUTTYPE_GEM_MOUNT, 0);
							event->SetInt32ArgValues(1, 0);
							conn->postNetEvent(event);
							return;
						}

						// 判断是否装备物品
						if(!pRes->isEquip())
						{
							//MessageEvent::show(SHOWTYPE_NOTIFY, "无法装备此物品");
							return;
						}
						
						// 判断是否能拖动到装备栏
						enWarnMessage msg = g_ItemManager->canDragToEquipment(player, pItem);
						if(msg != MSG_NONE)
						{
							MessageEvent::show(SHOWTYPE_NOTIFY, msg);
							return ;
						}
						Con::executef("showEquipPos", Con::getIntArg(pRes->getEquipPos()), Con::getIntArg(1));
						if(pRes->getBindMode() == Res::BINDMODE_EQUIP && !pItem->getRes()->IsActivatePro(EAPF_BIND))
							Con::executef("ShowBindWnd", Con::getIntArg(mSlotIndex), Con::getIntArg(2));
						else 
						{
							GameConnection *conn = player->getControllingClient();
							ClientGameNetEvent *event = new ClientGameNetEvent(INFO_SHORTCUT_EXCHANGE);
							event->SetIntArgValues(4, SHORTCUTTYPE_INVENTORY, mSlotIndex, SHORTCUTTYPE_EQUIP, -1);
							event->SetInt32ArgValues(1, 0);
							conn->postNetEvent(event);
						}
					}					
					else
					{
						//鉴定符
						if(pItem->getRes()->getSubCategory() == Res::CATEGORY_IDENTIFY)
						{
							ClientGameplayAction* action = new ClientGameplayAction(2, INFO_ITEM_IDENTIFY);
							action->setCursorName("Identify");
							g_ClientGameplayState->setCurrentAction(action);
							ClientGameplayParam* param0 = new ClientGameplayParam;
							param0->setIntArgValues(3, 0, mSlotType, mSlotIndex);
							g_ClientGameplayState->setCurrentActionParam(param0);
						}
						//判定是否使用物品
						else
						{
							GameObject* pViewObject = player->GetTarget();
							//有目标，对目标使用
							if(!pViewObject)
								pViewObject = player;
							enWarnMessage msg = g_ItemManager->canUseItem(player, pItem, player);
							if(msg != MSG_NONE)
							{
								MessageEvent::show(SHOWTYPE_NOTIFY, msg);
								return;
							}
							//可以使用
							else if(pItem->getRes() && pItem->getRes()->getUseStateID() > 0)
							{
								player->PreCastSpell(pItem->getRes()->getUseStateID(), pViewObject, NULL, mSlotType, mSlotIndex, pItem->getRes()->getUID());
							}
							else
							{
								//宠物蛋使用需要吟唱
								if (pItem->getRes()->isPetEgg())
								{			
									// VocalStatus::VOCALSTATUS_USE_PET_EGG
									PetOperationManager::UsePetEgg(player, mSlotType, mSlotIndex);
									return;
								}
								else if (pItem->getRes()->isMountPetEgg())
								{
									PetMountOperationManager::UseMountPetEgg(player, mSlotType, mSlotIndex);
									return;
								}
								ClientGameNetEvent* event = new ClientGameNetEvent(INFO_ITEM_USE);
								event->SetIntArgValues(2, mSlotType, mSlotIndex);
								player->getControllingClient()->postNetEvent(event);								
							}					
						}						
					}
				}
			}
		}
		break;	
	case SHORTCUTTYPE_NPCSHOP:			// 点击在NPC店铺栏上
		{
			if(player->getInteractionState() == Player::INTERACTION_NPCTRADE)
			{
				if(mSlotIndex < 0 || mSlotIndex >= NpcShopList::MAXSLOTS)
					return;
				NpcShopItem* pShopItem = player->npcShopList.mSlots[mSlotIndex];
				if(!pShopItem)
					return;

				ShortcutObject* pShortcut = player->npcShopList.GetSlot(mSlotIndex);
				if(!pShortcut)
					return;
				ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortcut);
				if(!pItem)
					return;

				//条件判断，是否可以购买
				char tempbuffer[128] = "\0";
				pItem->getRes()->getItemName(tempbuffer, 128);

				Con::executef("buyGoodFromShop", Con::getIntArg(mSlotIndex), tempbuffer, Con::getIntArg(pShopItem->CurrencyID), Con::getIntArg(pShopItem->CurrencyValue));		
			}
		}
		break;
	case SHORTCUTTYPE_TRADE:			// 点击在交易栏上
		{
			if (player->getInteractionState() == Player::INTERACTION_PLAYERTRADE)
			{
				ShortcutObject* pShortcut = g_ItemManager->getShortcutSlot(player, mSlotType, mSlotIndex);
				ItemShortcut* pItem = NULL;
				if(!pShortcut || !(pItem = dynamic_cast<ItemShortcut*>(pShortcut)))
					return;
				ClientGameNetEvent* event = new ClientGameNetEvent(INFO_TRADE);
				event->SetInt32ArgValues(3, TRADE_DOWNITEM, 0, mSlotIndex);
				player->getControllingClient()->postNetEvent(event);
			}
		}
		break;
	case SHORTCUTTYPE_DUMP:				// 点击在回购栏上
		{
			if (player->getInteractionState() == Player::INTERACTION_NPCTRADE)
			{
				ShortcutObject* pShortcut = g_ItemManager->getShortcutSlot(player, mSlotType, mSlotIndex);
				ItemShortcut* pItem = NULL;
				if(!pShortcut || !(pItem = dynamic_cast<ItemShortcut*>(pShortcut)))
					return;

				// 回购价格
				S32 iPrice = pItem->getRes()->getQuantity() * (pItem->getRes()->getSalePrice());
				if(player->getMoney() < iPrice)
					return;

				// 发送回购物品消息给服务端
				ClientGameNetEvent* event = new ClientGameNetEvent(INFO_NPCSHOP);
				event->SetIntArgValues(2, NPCSHOP_RECYCLE, mSlotIndex);
				player->getControllingClient()->postNetEvent(event);
			}
		}
		break;
	case SHORTCUTTYPE_EQUIP:			// 点击在装备栏上
		{
			//寻找物品栏中的空位
			ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortcut);
			S32 iEmptySlots = 0;
			Vector<S32> emptyslotlist;
			if(pItem)
				iEmptySlots = player->inventoryList.QueryEmptySlot(emptyslotlist);
			
			if(iEmptySlots < 1)
			{
				MessageEvent::show(SHOWTYPE_NOTIFY, MSG_ITEM_ITEMNOSPACE);
				return;
			}

			ClientGameplayAction* action = new ClientGameplayAction(2, INFO_SHORTCUT_EXCHANGE);
			g_ClientGameplayState->setCurrentAction(action);
			ClientGameplayParam* param0 = new ClientGameplayParam;
			param0->setIntArgValues(2, mSlotType, mSlotIndex);
			param0->setInt32ArgValues(1, 0);
			ClientGameplayParam* param1 = new ClientGameplayParam;
			param1->setIntArgValues(2, SHORTCUTTYPE_INVENTORY, emptyslotlist[0]);
			g_ClientGameplayState->setCurrentActionParam(param0);
			g_ClientGameplayState->setCurrentActionParam(param1);
		}
		break;
	case SHORTCUTTYPE_SKILL_COMMON:		// 点击在技能面板栏上
		{
			SkillShortcut* pSkill = dynamic_cast<SkillShortcut*>(pShortcut);
			if(pSkill && pSkill->getSkill() && pSkill->getSkill()->GetData() && player->GetTarget())
				player->CastSpell(pSkill->getSkill()->GetData()->GetID(), *player->GetTarget());
		}
		break;
	case SHORTCUTTYPE_PANEL:			// 点击在快捷栏上
		{
			if(pShortcut->isSkillObject())
			{
				SkillShortcut* pSkill = dynamic_cast<SkillShortcut*>(pShortcut);
				if(pSkill && pSkill->getSkill() && pSkill->getSkill()->GetData())
					player->PreCastSpell(pSkill->getSkill()->GetData()->GetID(), player->GetTarget(), NULL);
			}
			else if(pShortcut->isItemObject())
			{
				ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortcut);
				GameObject* pViewObject = player->GetTarget();
				if(!pViewObject)
					pViewObject = player;
				enWarnMessage msg = g_ItemManager->canUseItem(player, pItem, player);
				if(msg != MSG_NONE)
				{
					MessageEvent::show(SHOWTYPE_NOTIFY, msg);	
					return;
				}
				//可以使用
				else if(pItem->getRes() && pItem->getRes()->getUseStateID() > 0)
				{
					player->PreCastSpell(pItem->getRes()->getUseStateID(), pViewObject, NULL, mSlotType, mSlotIndex, pItem->getRes()->getUID());
				}
				else
				{
					ClientGameNetEvent* event = new ClientGameNetEvent(INFO_ITEM_USE);
					event->SetIntArgValues(2, mSlotType, mSlotIndex);
					player->getControllingClient()->postNetEvent(event);
				}
			}
			else if (pShortcut->isLivingSkillObject())
			{
				LivingSkillShortcut* pSkill = dynamic_cast<LivingSkillShortcut*>(pShortcut);
				if (pSkill && pSkill->getLivingSkill() && pSkill->getLivingSkill()->getData())
				{
					char sTab[8] = {0,};
					dSprintf(sTab,sizeof(sTab),"%d",(pSkill->getLivingSkill()->getData()->getCategory()-1));
					Con::executef("UI_OpenPrescription",sTab);
				}
			}
			else if (pShortcut->isPetObject())
			{
				PetShortcut *pPetShortcut = dynamic_cast<PetShortcut*>(pShortcut);
				S32 nSlot = pPetShortcut->getSlotIndex();
				const stPetInfo *pPetInfo = player->getPetTable().getPetInfo(nSlot);
				if (pPetInfo && pPetInfo->petDataId)
				{
					if (pPetInfo->status == PetObject::PetStatus_Idle)
						PetOperationManager::SpawnPet(player, nSlot);
					else if (pPetInfo->status == PetObject::PetStatus_Combat)
						PetOperationManager::DisbandPet(player, nSlot);
				}
			}
			else if (pShortcut->isMountObject())
			{
				MountShortcut *pMountShortcut = dynamic_cast<MountShortcut*>(pShortcut);
				S32 nSlot = pMountShortcut->getSlotIndex();
				const stMountInfo *pMountInfo = player->getPetTable().getMountInfo(nSlot);
				if (pMountInfo && pMountInfo->mDataId)
				{
					if (pMountInfo->mStatus == MountObject::MountStatus_Idle)
						PetMountOperationManager::SpawnMountPet(player, nSlot);
					else if (pMountInfo->mStatus == MountObject::MountStatus_Mount)
						PetMountOperationManager::DisbandMountPet(player, nSlot);
				}
			}
		}
		break;
	case SHORTCUTTYPE_BANK:				// 点击在银行仓库栏上
		{
			if(g_PlayerBank->getLocked(player))
			{
				MessageEvent::show(SHOWTYPE_NOTIFY, "仓库被锁定");
				return;
			}
			ClientGameNetEvent* event = new ClientGameNetEvent(INFO_BANK);
			event->SetInt32ArgValues(2, PlayerBank::Bank_ItemMoveOut, mSlotIndex);
			player->getControllingClient()->postNetEvent(event);
		}
		break;
	case SHORTCUTTYPE_IDENTIFY:			// 点击在装备鉴定栏上
		{
			if(!pShortcut)
				return;
			ClientGameplayAction* action = new ClientGameplayAction(2, INFO_SHORTCUT_EXCHANGE);
			g_ClientGameplayState->setCurrentAction(action);

			ClientGameplayParam* param0 = new ClientGameplayParam;
			param0->setIntArgValues(2, mSlotType, mSlotIndex);
			param0->setInt32ArgValues(1, 0);
			g_ClientGameplayState->setCurrentActionParam(param0);
			ClientGameplayParam* param1 = new ClientGameplayParam;
			param1->setIntArgValues(2, SHORTCUTTYPE_INVENTORY, pShortcut->getLockedItemIndex());
			g_ClientGameplayState->setCurrentActionParam(param1);

		}
		break;
	case SHORTCUTTYPE_LIVINGCATEGORY:
		{
			if (pShortcut->isLivingSkillObject())
			{
				LivingSkillShortcut* pSkill = dynamic_cast<LivingSkillShortcut*>(pShortcut);
				if (pSkill && pSkill->getLivingSkill() && pSkill->getLivingSkill()->getData())
				{
					char sTab[8] = {0,};
					dSprintf(sTab,sizeof(sTab),"%d",(pSkill->getLivingSkill()->getData()->getCategory()-1));
					Con::executef("UI_OpenPrescription",sTab);
				}
			}
		}
		break;
	case SHORTCUTTYPE_AUTOSELL:			// 点击在自动出售栏上
		{
			if(player->autoSellList.getShowType() < 3)
				player->autoSellList.autoSellByIndex(player, mSlotIndex);
		}
		break;
	case SHORTCUTTYPE_STALL:			// 点击在摆摊栏上
		{
			if(!pShortcut)
				return;
			if(player->individualStallList.getStallState() == TRADE_LOCK)
				return;
		
			ClientGameNetEvent* event = new ClientGameNetEvent(INFO_STALL);
			event->SetInt32ArgValues(3, TRADE_DOWNITEM, mSlotIndex, 1);
			player->getControllingClient()->postNetEvent(event);

		}
		break;
	case SHORTCUTTYPE_TARGET_STALL:		// 点击在对方摊主摆摊栏上
		{
			if(!pShortcut)
				return;
			
			if(player->getInteractionState() == Player::INTERACTION_STALLBUY)
			{
				SceneObject* pTargetObject = player->getInteraction();
				Player* pTarget = NULL;
				if(!pTargetObject || !(pTarget = dynamic_cast<Player*>(pTargetObject)))
					return;
				StallItem* pStallItem = pTarget->individualStallList.mSlots[mSlotIndex];
				if(!pStallItem)
					return;
				ItemShortcut* pItem = pTarget->individualStallList.GetSlot(mSlotIndex);
				Res* pRes = NULL;
				if(!pItem || !(pRes = pItem->getRes()))
					return;
				U32 iMoney = pStallItem->money;
				U32 iHaveNum = pRes->getQuantity();
				U32 iMaxNum = pRes->getMaxOverNum();
				Con::executef("openBuyStallItemWnd", Con::getIntArg(mSlotIndex), Con::getIntArg(1), Con::getIntArg(iMoney), Con::getIntArg(iHaveNum), Con::getIntArg(iMaxNum));
			}
		}
		break;
	case SHORTCUTTYPE_STALL_PET:		// 点击在摆摊宠物栏上
		{
			if(!pShortcut)
				return;
			if(player->individualStallList.getStallState() == TRADE_LOCK)
				return;
			
			ClientGameNetEvent* event = new ClientGameNetEvent(INFO_STALL);
			event->SetInt32ArgValues(3, TRADE_DOWNITEM, mSlotIndex, 2);
			player->getControllingClient()->postNetEvent(event);
		}
		break;
	case SHORTCUTTYPE_TARGET_STALL_PET:	// 点击在对方摊主摆摊宠物栏上
		{
			if(!pShortcut)
				return;
			if (player->getInteractionState() == Player::INTERACTION_STALLBUY)
			{
				SceneObject* pTargetObject = player->getInteraction();
				Player* pTarget = NULL;
				if(!pTargetObject || !(pTarget = dynamic_cast<Player*>(pTargetObject)))
					return;
				stStallPet* pPet = pTarget->stallPetList.mSlots[mSlotIndex];
				if(!pPet)
					return;

				Con::executef("openBuyStallItemWnd", Con::getIntArg(mSlotIndex), Con::getIntArg(2), Con::getIntArg(pPet->money));
			}
		}
		break;
	case SHORTCUTTYPE_STALL_BUYLIST:	// 点击在摆摊收购栏上
		{
			if(!pShortcut)
				return;
			if(player->individualStallList.getStallState() == TRADE_LOCK)
				return;

			ClientGameNetEvent* event = new ClientGameNetEvent(INFO_STALL);
			event->SetInt32ArgValues(3, TRADE_DOWNITEM, mSlotIndex, 3);
			player->getControllingClient()->postNetEvent(event);
		}
		break;
	case SHORTCUTTYPE_TARGET_STALLBUYLIST:// 点击在对方摊主收购栏上
		{
			if(!pShortcut)
				return;
			if(player->individualStallList.getStallState() == TRADE_LOCK)
				return;
			if (player->getInteractionState() != Player::INTERACTION_STALLBUY)
				return;
			SceneObject* pTargetObject = player->getInteraction();
			Player* pTarget = NULL;
			if(!pTargetObject || !(pTarget = dynamic_cast<Player*>(pTargetObject)))
				return;
			stStallBuyItem* pBuyItem = pTarget->stallBuyItemList.mSlots[mSlotIndex];
			if(!pBuyItem)
				return;
			U32 num = pBuyItem->num;
			U32 money = pBuyItem->money;
			U32 iMaxNum = pBuyItem->buyItem->getRes()->getMaxOverNum();
			Con::executef("showSellStallItemInfo", Con::getIntArg(money), Con::getIntArg(num), Con::getIntArg(mSlotIndex), Con::getIntArg(iMaxNum));
		}
		break;
	case SHORTCUTTYPE_TRADE_STALL_HELP:
		{
			TradeHelpShortCut* pHelpShortcut = player->mTradeStallHelpList.getShortcut(mSlotIndex);
			if(!pHelpShortcut)
				return;
			if(player->individualStallList.getStallState() == TRADE_LOCK)
				return ;
			if(g_Stall->getType() < 3)
			{
				ShortcutObject* pObject = g_ItemManager->getShortcutSlot(player, SHORTCUTTYPE_INVENTORY, pHelpShortcut->mIndex);
				if(!pObject || pObject->getSlotState() == ShortcutObject::SLOT_LOCK)
				{
					MessageEvent::show(SHOWTYPE_WARN, "此物品已经在摆摊栏中");
					return;
				}
				S32 index = player->individualStallList.findEmptyIndex();
				if(index < 0)
				{
					MessageEvent::show(SHOWTYPE_WARN, "摆摊物品栏已满");
					return;
				}
				else
					Con::executef("oPenSetPrice", Con::getIntArg(index), Con::getIntArg(pHelpShortcut->mIndex));
			}
			else
			{
				for(int i=0; i<StallPetList::MAXSLOTS; ++i)
				{
					PetShortcut* pet = player->stallPetList.GetSlot(i);
					if(pet)
					{
						if(pet->getSlotIndex() == pHelpShortcut->mIndex)
						{
							MessageEvent::show(SHOWTYPE_WARN, "此宠物已经在摆摊栏中");
							return;
						}
					}
				}
				
				S32 index = player->stallPetList.findEmptyIndex();
				if(index < 0)
				{
					MessageEvent::show(SHOWTYPE_WARN, "摆摊宠物栏已满");
					g_ClientGameplayState->cancelCurrentAction();
					return;
				}
				if(index >= 0)
				{
					Con::executef("openSetPetPrice", Con::getIntArg(index), Con::getIntArg(pHelpShortcut->mIndex));
				}
			}
		}
		break;
	case SHORTCUTTYPE_ITEMSPLIT:
		{
			if(player->mItemSplitList.isLock())
			{
				MessageEvent::show(SHOWTYPE_NOTIFY,MSG_ITEMCOMPOSE_LOCK);
				return;
			}

			if(mSlotIndex == 0) //取回分解物品
				player->mItemSplitList.sendToCancelEvent(player);
			else               //拾取分解后产物
				player->mItemSplitList.sendToPickupItemByIndex(player,mSlotIndex);
		}
		break;
	case SHORTCUTTYPE_ITEMCOMPOSE:
		{
			if(player->mItemComposeList.isLock()) 
			{
				MessageEvent::show(SHOWTYPE_NOTIFY,MSG_ITEMCOMPOSE_LOCK);
				return;
			}

			player->mItemComposeList.sendToPickupItemByIndex(player,mSlotIndex);
		}
		break;
	case SHORTCUTTYPE_REPAIR://修理栏
		{
			if(mSlotIndex != 0)
				return;
			GameConnection* conn = player->getControllingClient();
			if(!conn)
				return;
			ClientGameNetEvent *event = new ClientGameNetEvent(INFO_SHORTCUT_EXCHANGE);
			event->SetIntArgValues(4, SHORTCUTTYPE_REPAIR, mSlotIndex, SHORTCUTTYPE_NONE, 0);
			event->SetInt32ArgValues(1, 0);
			conn->postNetEvent(event);
		}
		break;
	default:;
	}
}

void dGuiShortCut::onRightMouseDragged(const GuiEvent &event)
{
}

void dGuiShortCut::onRightMouseDown(const GuiEvent &event)
{
	// 触发控件绑定鼠标操作脚本
	if (mRightMouseDownCommand && mRightMouseDownCommand[0])
	{
		Con::setVariable("$ThisControl",avar("%d",getId()));
		Con::evaluate(mRightMouseDownCommand,false);
	}
	ClientGameplayAction* currentAction = g_ClientGameplayState->getCurrentAction();
	if (currentAction) 
	{
		if(currentAction->getActionType() == INFO_NPCSHOP || currentAction->getActionType() == INFO_ITEM_BATCHBUY)
			Con::executef("clearBuyFlag");
		g_ClientGameplayState->cancelCurrentAction();
	}
}

// ----------------------------------------------------------------------------
// 渲染ICON禁止样式
void dGuiShortCut::drawForbided(Point2I offset, const RectI &updateRect)
{
	if(mIsForbided && mSelectTexture)
	{
		S32 bmpWidth	= mSelectTexture.getWidth() / 11;
		S32 bmpHeight	= mSelectTexture.getHeight() / 11;
		RectI renderOffset(offset + mBoundary.point, mBoundary.extent);
		RectI bmpOffset(bmpWidth * 9, 0, bmpWidth,bmpWidth);
		GFX->getDrawUtil()->drawBitmapStretchSR(mSelectTexture, renderOffset, bmpOffset);
	}
}

// ----------------------------------------------------------------------------
// 渲染鼠标移到Slot的样式
void dGuiShortCut::drawMoveIn(Point2I offset, const RectI &updateRect)
{
	if(mIsMoveIn && mSelectTexture)
	{
		S32 bmpWidth	= mSelectTexture.getWidth() / 11;
		S32 bmpHeight	= mSelectTexture.getHeight() / 11;
		RectI renderOffset(offset + mBoundary.point, mBoundary.extent);
		RectI bmpOffset(bmpWidth * 7, 0, bmpWidth,bmpWidth);
		GFX->getDrawUtil()->drawBitmapStretchSR(mSelectTexture, renderOffset, bmpOffset);
	}
}

// ----------------------------------------------------------------------------
// 设置是否选中Slot
void dGuiShortCut::setBeSelected(bool IsSelected)
{ 
	mIsEnableSelect = IsSelected;
	m_DirtyFlag = true;
}

// ----------------------------------------------------------------------------
// 渲染Slot被选中的状态
void dGuiShortCut::drawBeSelected(Point2I offset, const RectI &updateRect)
{
	if(mIsEnableSelect && mSelectTexture)
	{
		S32 bmpWidth	= mSelectTexture.getWidth() / 11;
		S32 bmpHeight	= mSelectTexture.getHeight() / 11;
		RectI renderOffset(offset + mBoundary.point, mBoundary.extent);
		RectI bmpOffset(bmpWidth * 8, 0, bmpWidth,bmpWidth);
		GFX->getDrawUtil()->drawBitmapStretchSR(mSelectTexture, renderOffset, bmpOffset);
	}
}

// ----------------------------------------------------------------------------
// 设置是否启用边框特效
void dGuiShortCut::setEnableEffect(bool IsBorderEffect, U32 looptimes/*=0*/)
{ 
	mIsEnableEffect = IsBorderEffect;
	m_DirtyFlag = true;
	if(mIsEnableEffect && mEffectTexture)
	{
		S32 bmpWidth	= mEffectTexture.getWidth() / 6;
		S32 bmpHeight	= mEffectTexture.getHeight() / 6;
		Point2I extents = getExtent();
		if(bmpWidth > extents.x)
			setWidth(bmpWidth);
		if(bmpHeight > extents.y)
			setHeight(bmpHeight);
		mLoopTimes = looptimes;
		mLoopEffects = 0;
	}
}

// ----------------------------------------------------------------------------
// 渲染Slot的边框特效
void dGuiShortCut::drawBorderEffect(Point2I offset, const RectI &updateRect)
{
	if(mIsEnableEffect && mEffectTexture)
	{		
		if(mLoopTimes != 0 && mLoopTimes <= mLoopEffects)
			return;

		S32 bmpWidth	= mEffectTexture.getWidth() / 6;
		S32 bmpHeight	= mEffectTexture.getHeight() / 6;
		S32 offsetX		= offset.x + (S32)(32 - bmpWidth)/2;
		S32 offsetY		= offset.y + (S32)(32 - bmpHeight)/2;

		U32 currentTime = Platform::getVirtualMilliseconds();
		RectI renderPos(offsetX, offsetY, bmpWidth, bmpHeight);
		RectI bmpOffset(bmpWidth * mEffectPosX, bmpHeight * mEffectPosY, bmpWidth, bmpHeight);
		GFX->getDrawUtil()->drawBitmapStretchSR(mEffectTexture, renderPos, bmpOffset);

		if(currentTime - mLastTime_Effect > 100)
		{
			if(mEffectPosX== 5 && mEffectPosY == 5)
				mLoopEffects++;

			mEffectPosX ++;
			if(mEffectPosX >= 6)
			{
				if(mEffectPosY < 5)
					mEffectPosY++;
				else
					mEffectPosY = 0;
				mEffectPosX = 0;
			}
			mLastTime_Effect = currentTime;
		}
	}
}

// ----------------------------------------------------------------------------
// 渲染Slot的边框
void dGuiShortCut::drawFrame(Point2I offset, const RectI &updateRect)
{
	if(mIsRenderFrame && mSelectTexture)
	{
		S32 bmpWidth	= mSelectTexture.getWidth() / 11;
		S32 bmpHeight	= mSelectTexture.getHeight() / 11;
		RectI renderOffset(offset + mBoundary.point, mBoundary.extent);
		RectI bmpOffset(bmpWidth * mFrames, 0, bmpWidth,bmpWidth);
		GFX->getDrawUtil()->drawBitmapStretchSR(mSelectTexture, renderOffset, bmpOffset);
	}
}

// ----------------------------------------------------------------------------
// 渲染ICON上数字
void dGuiShortCut::drawIconNum(Point2I offset, const RectI &updateRect)
{
	if(!mIsShowNum || !mTexture || !mNumTexture)
		return;

	S32 offsetX		= offset.x + mBoundary.point.x + mBoundary.extent.x - 2;
	S32 offsetY		= offset.y + mBoundary.point.y + mBoundary.extent.y - 3;
	S32 bmpWidth	= mNumTexture.getWidth() / 10;
	S32 bmpHeight	= mNumTexture.getHeight();

	U32 count = 1;
	for(S32 n = 2; n >= 0; n--)
	{
		if(mNum[n] >= 0)
		{
			RectI renderPos((offsetX - bmpWidth * count), (offsetY - bmpHeight), bmpWidth, bmpHeight);
			RectI bmpOffset(bmpWidth * mNum[n], 0, bmpWidth, bmpHeight);
			GFX->getDrawUtil()->drawBitmapStretchSR(mNumTexture, renderPos, bmpOffset);
			count ++;
		}
	}
}

// ----------------------------------------------------------------------------
// 渲染ICON的冷却样式
void dGuiShortCut::drawCoolDown(Point2I offset, const RectI &updateRect)
{
	if(!mIsCool || !mTexture)
		return;

	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return;

	ShortcutObject* pShortcut = g_ItemManager->getShortcutSlot(player,mSlotType, mSlotIndex);
	if(!pShortcut)
		return;

	// 获取剩余冷却时间和总冷却时间
	static SimTime cdTime;
	static SimTime totalTime;
	if(!player->GetCooldown(pShortcut->getCooldownGroup(), cdTime, totalTime, false))
	{
		mIsCool = false;
		mIsShowCoolFlash = true;
		mCoolPosY = mCoolPosX = 0;
		mLastTime_CoolFlash = Platform::getVirtualMilliseconds();
		return;
	}

	mIsShowCoolFlash = false;
	
	F32 a = mAtan((F32)updateRect.len_x(), (F32)updateRect.len_y());
	F32 aPercent = a/M_2PI_F;
	F32 bPercent = (F32)cdTime/(F32)totalTime;
	F32 b = bPercent * M_2PI_F;
	Point2F cp((updateRect.len_x()+updateRect.len_y())*(-mCos(b-M_PI_F/2)) + (updateRect.len_x() / 2) + offset.x, (updateRect.len_x()+updateRect.len_y())*mSin(b-M_PI_F/2) + (updateRect.len_y() / 2) + offset.y);

	static ColorF color(0,0,0,100);
	S32 startIndex = 0;
	S32 triCount = 5;
	GFXVertexBufferHandle<GFXVertexPC> verts(GFX, 7, GFXBufferTypeVolatile);
	verts.lock();
	verts[0].point.set(offset.x + (updateRect.len_x() / 2), offset.y + (updateRect.len_y() / 2), 0.0f);
	verts[1].point.set(offset.x + (updateRect.len_x() / 2), offset.y, 0.0f);
	verts[2].point.set(offset.x + updateRect.len_x(), offset.y, 0.0f);
	verts[3].point.set(offset.x + updateRect.len_x(), offset.y + updateRect.len_y(), 0.0f);
	verts[4].point.set(offset.x, offset.y + updateRect.len_y(), 0.0f);
	verts[5].point.set(offset.x, offset.y, 0.0f);
	verts[6].point.set(offset.x + (updateRect.len_x() / 2), offset.y, 0.0f);

	if(bPercent < aPercent)
	{
		verts[1].point.set(verts[0].point);
		verts[2].point.set(verts[0].point);
		verts[3].point.set(verts[0].point);
		verts[4].point.set(verts[0].point);
		verts[5].point.set(cp.x, cp.y, 0.0f);
		S32 startIndex = 4;
		S32 triCount = 1;
	}
	else if(bPercent < 0.5f - aPercent)
	{
		verts[1].point.set(verts[0].point);
		verts[2].point.set(verts[0].point);
		verts[3].point.set(verts[0].point);
		verts[4].point.set(cp.x, cp.y, 0.0f);
		S32 startIndex = 3;
		S32 triCount = 2;
	}
	else if(bPercent < 0.5f + aPercent)
	{
		verts[1].point.set(verts[0].point);
		verts[2].point.set(verts[0].point);
		verts[3].point.set(cp.x, cp.y, 0.0f);
		S32 startIndex = 2;
		S32 triCount = 3;
	}
	else if(bPercent < 1.0f - aPercent)
	{
		verts[1].point.set(verts[0].point);
		verts[2].point.set(cp.x, cp.y, 0.0f);
		S32 startIndex = 1;
		S32 triCount = 4;
	}
	else
	{
		verts[1].point.set(cp.x, cp.y, 0.0f);
	}
	verts[0].color = verts[1].color = verts[2].color = verts[3].color = verts[4].color = verts[5].color = verts[6].color = color;
	verts.unlock();

#ifdef STATEBLOCK
	AssertFatal(mSetCoolSB, "dGuiShortCut::drawCoolDown -- mSetCoolSB cannot be NULL.");
	mSetCoolSB->apply();
#else
	GFX->setCullMode(GFXCullNone);
	GFX->setLightingEnable(false);
	GFX->setAlphaBlendEnable(true);
	GFX->setSrcBlend(GFXBlendSrcAlpha);
	GFX->setDestBlend(GFXBlendInvSrcAlpha);
	GFX->setTextureStageAlphaOp(0, GFXTOPDisable);
#endif

	GFX->setVertexBuffer(verts);
	GFX->drawPrimitive(GFXTriangleFan, startIndex, triCount);

#ifdef STATEBLOCK
	AssertFatal(mClearCoolSB, "dGuiShortCut::drawCoolDown -- mClearCoolSB cannot be NULL.");
	mClearCoolSB->apply();
#else
	GFX->setTextureStageAlphaOp(0, GFXTOPModulate);
	GFX->setLightingEnable(true);
#endif
	GFX->setBaseRenderState();

#ifdef NTJ_CLIENT
	if(pShortcut->isSkillObject())
	{
		char szNumber[10];
		Point2I pos;
		if(cdTime < 10000)
		{
			_itoa_s( cdTime / 1000, szNumber, 10, 10);
			pos.x = offset.x + (updateRect.len_x() - mCoolDownFont[0]->getStrWidth(szNumber)) / 2;
			pos.y = (S32)offset.y + (S32)(updateRect.len_y() - mCoolDownFont[0]->getHeight()) / 2;
			mCoolDownFont[0]->BeginRender();
			mCoolDownFont[0]->SetText( pos, ColorI(255,255,255,180), szNumber);
			mCoolDownFont[0]->Render();
		}
		else if(cdTime >= 10000 && cdTime < 60000)
		{
			_itoa_s( cdTime / 1000, szNumber, 10, 10);
			pos.x = offset.x + (updateRect.len_x() - mCoolDownFont[1]->getStrWidth(szNumber)) / 2;
			pos.y = offset.y + (updateRect.len_y() - mCoolDownFont[1]->getHeight()) / 2;
			mCoolDownFont[1]->BeginRender();
			mCoolDownFont[1]->SetText( pos, ColorI(255,255,255,180), szNumber);
			mCoolDownFont[1]->Render();
		}
		else if(cdTime >= 60000)
		{
			dSprintf(szNumber, 10, "%dm", cdTime/60000);
			pos.x = offset.x + (updateRect.len_x() - mCoolDownFont[2]->getStrWidth(szNumber)) / 2;
			pos.y = offset.y + (updateRect.len_y() - mCoolDownFont[2]->getHeight()) / 2;
			mCoolDownFont[2]->BeginRender();
			mCoolDownFont[2]->SetText( pos, ColorI(255,255,255,180), szNumber);
			mCoolDownFont[2]->Render();
		}
	}
#endif// NTJ_CLIENT
}

// ----------------------------------------------------------------------------
// 渲染ICON的冷却样式
void dGuiShortCut::drawCoolFlash(Point2I offset, const RectI &updateRect)
{
	if(!mIsShowCoolFlash || !mCoolFlashTexture)
		return;

	S32 offsetX		= offset.x + mBoundary.point.x;
	S32 offsetY		= offset.y + mBoundary.point.y;
	S32 bmpWidth	= mCoolFlashTexture.getWidth() / 5;
	S32 bmpHeight	= mCoolFlashTexture.getHeight() / 2;
	
	U32 currentTime = Platform::getVirtualMilliseconds();
	RectI renderPos(offsetX, offsetY, bmpWidth, bmpHeight);
	RectI bmpOffset(bmpWidth * mCoolPosX, bmpHeight * mCoolPosY, bmpWidth, bmpHeight);
	GFX->getDrawUtil()->drawBitmapStretchSR(mCoolFlashTexture, renderPos, bmpOffset);
	
	if(currentTime - mLastTime_CoolFlash > 30)
	{
		mCoolPosX ++;
		if(mCoolPosX >= 5)
		{
			if(mCoolPosY == 0)
				mCoolPosY++;
			else
			{
				mIsShowCoolFlash = false;
				mLastFrame = true;
				mCoolPosY = 0;
			}
			mCoolPosX = 0;
		}
		mLastTime_CoolFlash = currentTime;
	}
}

//检查拖动交换
enWarnMessage dGuiShortCut::checkShortcutMove(S32 old_type, S32 old_col)
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return MSG_ITEM_CANNOTDRAG;

	ShortcutObject* pSrcSlot = g_ItemManager->getShortcutSlot(pPlayer, old_type, old_col);
	if(!pSrcSlot)
		return MSG_ITEM_ITEMDATAERROR;

	if(pSrcSlot->getSlotState() != ShortcutObject::SLOT_COMMON)
		return MSG_ITEM_CANNOTDRAG;

	switch (old_type)
	{
	case SHORTCUTTYPE_PANEL:
		{
			if(mSlotType != SHORTCUTTYPE_PANEL)
				return MSG_ITEM_CANNOTDRAG;
		}		
		break;
	case SHORTCUTTYPE_INVENTORY:
		{
			switch (mSlotType)
			{
			case SHORTCUTTYPE_PANEL:
				{
					// Note:对于拖动物品到快捷栏，必须累计该物品栏里此物品的所有数量，所以必须同时跟踪
					// 快捷栏物品使用时数量的变化
					ItemShortcut* pSrcItem =  dynamic_cast<ItemShortcut*>(pSrcSlot);
					Res* pSrcRes = pSrcItem->getRes();
					// 判断是否消耗性物品,坐骑也可以拖到快捷栏
					if(!pSrcRes /*|| ( pSrcRes->getSubCategory() != Res::CATEGORY_RIDE)*/)
						return MSG_ITEM_CANNOTDRAGTOOBJECT;			// 此物品不能拖动到目标位置

					// 查询快捷栏中是否已经存在此物品
					S32 iCol = -1;
					g_ItemManager->getSameObject(pPlayer, SHORTCUTTYPE_PANEL,pSrcItem, iCol);
					ShortcutObject* pShortObj = NULL;
					if( iCol != -1 && (pShortObj = g_ItemManager->getShortcutSlot(pPlayer, mSlotType, iCol)))
						return MSG_ITEM_ALREADYHASSHORTCUT;			// 此物品已经在快捷栏中
				
				}
				break;
			case SHORTCUTTYPE_INVENTORY:
				{
					ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pSrcSlot);
					if(!pItem)
						return MSG_ITEM_ITEMDATAERROR;
				}
				break;
			case SHORTCUTTYPE_EQUIP:
				{
					ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pSrcSlot);
					if(!pSrcItem)
						return MSG_ITEM_ITEMDATAERROR;
					
					enWarnMessage msg = g_ItemManager->canDragToEquipment(pPlayer, pSrcItem, mSlotIndex);
					if(msg != MSG_NONE)
						return msg;
					//Con::executef("showDragToPos", Con::getIntArg(pSrcItem->getRes()->getEquipPos()), Con::getIntArg(1));
					if(pSrcItem->getRes()->getBindMode() == Res::BINDMODE_EQUIP && !pSrcItem->getRes()->IsActivatePro(EAPF_BIND))
					{
						g_ClientGameplayState->popCursor();
						Con::executef("ShowBindWnd", Con::getIntArg(mSlotIndex), Con::getIntArg(1));
						return MSG_WAITING_CONFIRM;
					}
				}
				break;
			case SHORTCUTTYPE_IDENTIFY:
				{
					ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pSrcSlot);
					Res* pRes = NULL;
					if(!pSrcItem || !(pRes = pSrcItem->getRes()))
						return MSG_ITEM_ITEMDATAERROR;
					
					if(mSlotIndex == 0)
					{
						if(!pRes->isWeapon() && !pRes->isBody())
							return MSG_ITEM_CANNOTDRAGTOOBJECT;
						if(pRes->getIdentifyType() != Res::IDENTIFYTYPE_NPC)
							return MSG_ITEM_CANNOTDRAGTOOBJECT;
						if(pRes->IsActivatePro(EAPF_ATTACH))
							return MSG_ITEM_HADINDENTIFIED;
						if(pRes->getColorLevel() < 3)
							return MSG_ITEM_CANNOTDRAGTOOBJECT;
					}
					else if(mSlotIndex == 1)
					{
						if(pRes->getSubCategory() != Res::CATEGORY_ITEM_IDENTIFY)
							return MSG_ITEM_CANNOTDRAGTOOBJECT;
					}
				}
				break;
			case SHORTCUTTYPE_STRENGTHEN:
				{
					ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pSrcSlot);
					enWarnMessage msg = EquipStrengthen::checkMoveFromInventory(pPlayer, pSrcItem, mSlotIndex);
					if (msg != MSG_NONE)
					{
						//客户端显示消息
					}
				}
				break;
			case SHORTCUTTYPE_GEM_MOUNT:
				{
					ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pSrcSlot);
					enWarnMessage msg = EquipMountGem::checkMoveFromInventory(pPlayer, pSrcItem, mSlotIndex);
					if (msg != MSG_NONE)	//客户端显示错误消息					
						return msg;					
				}
				break;
			case SHORTCUTTYPE_EQUIP_PUNCHHOLE:
				{
					ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pSrcSlot);
					enWarnMessage msg = EquipPunchHole::checkMoveFromInventory(pPlayer, pSrcItem, mSlotIndex);
					if (msg != MSG_NONE)	//客户端显示错误消息					
						return msg;					
				}
				break;
			case SHORTCUTTYPE_TEMP:
				{
					ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pSrcSlot);
					enWarnMessage msg = TempList::CheckInventoryMoveToTempList(pPlayer, pSrcItem, mSlotIndex);
					if (msg != MSG_NONE)	//客户端显示错误消息					
						return msg;
				}
				break;
			case SHORTCUTTYPE_BANK:
				{
					if(pPlayer->getBankFlag())
						return MSG_BANK_LOCK;
					ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pSrcSlot);
					Res* pRes = NULL;
					if(!pSrcItem || !(pRes = pSrcItem->getRes()))
						return MSG_ITEM_ITEMDATAERROR;
					if(!pRes->canBaseLimit(Res::ITEM_SAVEBANK))
						return MSG_ITEM_ITEMDATAERROR;
				}
				break;
			case SHORTCUTTYPE_TRADE:
				{
					ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pSrcSlot);
					if(!pSrcItem)
						return MSG_ITEM_ITEMDATAERROR;

					// 判断物品是否可交易
					if(!pSrcItem->getRes()->canTrade())
						return MSG_ITEM_CANNOTTRADE;

					// 判断玩家当前状态是否玩家交易状态
					if(pPlayer->isBusy(Player::INTERACTION_PLAYERTRADE))
						return MSG_TRADE_SELF_BUSY;
				}
				break;
			case SHORTCUTTYPE_STALL:
				{
					ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pSrcSlot);
					if(!pSrcItem)
						return MSG_ITEM_ITEMDATAERROR;

					// 判断物品是否可交易
					if(!pSrcItem->getRes()->canTrade())
						return MSG_ITEM_CANNOTTRADE;

					if(pPlayer->individualStallList.getStallState() == TRADE_LOCK)
						return MSG_TRADE_SELF_STATELOCK;
					g_ClientGameplayState->popCursor();
					Con::executef("OpenPrice", Con::getIntArg(mSlotIndex));
					return MSG_WAITING_CONFIRM;
				}
				break;
			case SHORTCUTTYPE_STALL_BUYLIST:
				{
					ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pSrcSlot);
					Res* pRes = NULL;
					if(!pSrcItem || !(pRes = pSrcItem->getRes()))
						return MSG_ITEM_ITEMDATAERROR;
					
					// 判断物品是否可交易
					if(!pRes->canTrade())
						return MSG_ITEM_CANNOTTRADE;

					if(pPlayer->individualStallList.getStallState() == TRADE_LOCK)
						return MSG_TRADE_SELF_STATELOCK;
					g_ClientGameplayState->popCursor();

					U32 iMaxOver = pRes->getMaxOverNum();
					Con::executef("OpenWillBuyItemPrice", Con::getIntArg(mSlotIndex), Con::getIntArg(iMaxOver));
					return MSG_WAITING_CONFIRM;
				}
				break;
			case SHORTCUTTYPE_REPAIR:
				{
					ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pSrcSlot);
					if(!pSrcItem)
						return MSG_ITEM_ITEMDATAERROR;
					Res* pRes = pSrcItem->getRes();
					if(!pRes)
						return MSG_ITEM_ITEMDATAERROR;
					if(!pRes->isWeapon() && !(pRes->isEquipment() && pRes->getSubCategory() < Res::CATEGORY_FASHION_FAMILY))
						return MSG_ITEM_CANNOTFIXED;
					if(!pRes->canBaseLimit(Res::ITEM_FIX))
						return MSG_ITEM_CANNOTFIXED;
					if(mSlotIndex != 0)
						return MSG_ITEM_CANNOTDRAGTOOBJECT;

				}
				break;
			case SHORTCUTTYPE_ITEMSPLIT:
				{
					if(pPlayer->mItemSplitList.isLock())
						return MSG_ITEMSPLIT_ISLOCK;

					ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pSrcSlot);
					if(!pItem)
						return MSG_ITEM_ITEMDATAERROR;
				}
				break;
			case SHORTCUTTYPE_ITEMCOMPOSE:
				{
					ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pSrcSlot);
					if(!pItem)
						return MSG_ITEM_ITEMDATAERROR;
				}
				break;
			}
		}
		break;
	case SHORTCUTTYPE_EQUIP:
		{
			if(mSlotType != SHORTCUTTYPE_INVENTORY)
				return MSG_ITEM_CANNOTDRAGTOOBJECT;
		}
		break;
	case SHORTCUTTYPE_TRADE:
		{
			if(mSlotType != SHORTCUTTYPE_INVENTORY)
				return MSG_ITEM_CANNOTDRAGTOOBJECT;
		}
		break;
	case SHORTCUTTYPE_SKILL_COMMON:
		{
			switch (mSlotType)
			{
			case SHORTCUTTYPE_PANEL:
				{
					SkillShortcut* pSkill = dynamic_cast<SkillShortcut*>(pSrcSlot);
					if(!pSkill)
						return MSG_ITEM_CANNOTDRAGTOOBJECT;
					SkillData* pData = pSkill->getSkill() ? pSkill->getSkill()->GetData() : NULL;
					//判断是否是主动技能
					if(!pData || pData->IsFlags(SkillData::Flags_Passive))
						return MSG_ITEM_CANNOTDRAGTOOBJECT;
				}
				break;
			default:
				return MSG_ITEM_CANNOTDRAGTOOBJECT;
			}
		}
		break;
	case SHORTCUTTYPE_SKILL_HINT:
		{
			
			if(mSlotType == SHORTCUTTYPE_PANEL)
			{
					SkillShortcut* pSkill = dynamic_cast<SkillShortcut*>(pSrcSlot);
					if(!pSkill)
						return MSG_ITEM_CANNOTDRAGTOOBJECT;
					SkillData* pData = pSkill->getSkill() ? pSkill->getSkill()->GetData() : NULL;
					if(!pData || pData->IsFlags(SkillData::Flags_Passive))
						return MSG_ITEM_CANNOTDRAGTOOBJECT;
			}
		}
		break;
	case SHORTCUTTYPE_IDENTIFY:
		{
			ShortcutObject* pDestObj = g_ItemManager->getShortcutSlot(pPlayer, mSlotType, mSlotIndex);
		}
		break;
	case SHORTCUTTYPE_STRENGTHEN:
		{

		}
		break;
	case SHORTCUTTYPE_EQUIP_PUNCHHOLE:
		{

		}
		break;
	case SHORTCUTTYPE_LIVINGCATEGORY:
		{
			switch(mSlotType)
			{
			case SHORTCUTTYPE_PANEL:
				{
					LivingSkillShortcut* pSkillShortcut = dynamic_cast<LivingSkillShortcut*>(pSrcSlot);
					if(!pSkillShortcut)
						return MSG_ITEM_CANNOTDRAGTOOBJECT;;
				}
				break;
			default:return MSG_ITEM_CANNOTDRAGTOOBJECT;
			}
		}
		break;
	case SHORTCUTTYPE_BANK:
		{
			if(pPlayer->getBankFlag())
				return MSG_BANK_LOCK;
			switch(mSlotType)
			{
			case SHORTCUTTYPE_BANK:
				break;
			case SHORTCUTTYPE_INVENTORY:
				break;
			default:
				return MSG_ITEM_CANNOTDRAGTOOBJECT;
			}
		}
		break;
	case SHORTCUTTYPE_STALL:
		{
			if(mSlotType != SHORTCUTTYPE_INVENTORY)
				return MSG_ITEM_CANNOTDRAGTOOBJECT;
		}
		break;
	case SHORTCUTTYPE_PET:
		{
			switch(mSlotType)
			{
			case SHORTCUTTYPE_PET:
				break;
			case SHORTCUTTYPE_PANEL:
				break;
			case SHORTCUTTYPE_STALL_PET:
				{
					PetShortcut* pSrcItem = dynamic_cast<PetShortcut*>(pSrcSlot);
					if(!pSrcItem)
						return MSG_ITEM_ITEMDATAERROR;
					stPetInfo *pPetInfo = (stPetInfo *)pPlayer->getPetTable().getPetInfo(pSrcItem->getSlotIndex());
					if (!pPetInfo)
						return MSG_PET_NOT_EXIST;
					if (pPetInfo->status != PetObject::PetStatus_Idle)
						return MSG_PET_STATUS_NOT_IDLE;

					if(pPlayer->individualStallList.getStallState() == TRADE_LOCK)
						return MSG_TRADE_SELF_STATELOCK;
					g_ClientGameplayState->popCursor();
					Con::executef("oPenPetPrice", Con::getIntArg(mSlotIndex));
					return MSG_WAITING_CONFIRM;
				}
				break;
			default:
				return MSG_ITEM_CANNOTDRAGTOOBJECT;
			}
		}
		break;
	case SHORTCUTTYPE_MOUNT_PET_LIST:
		{
			switch(mSlotType)
			{
			case SHORTCUTTYPE_MOUNT_PET_LIST:
				break;
			case SHORTCUTTYPE_PANEL:
				break;
			default:
				return MSG_ITEM_CANNOTDRAGTOOBJECT;
			}
		}
		break;
	case SHORTCUTTYPE_STALL_PET:
		{
			if(mSlotType == SHORTCUTTYPE_STALL_PET)
				return MSG_ITEM_CANNOTDRAGTOOBJECT;
			else
				return MSG_NONE;
		}
		break;
	case SHORTCUTTYPE_ITEMSPLIT:
		{
			if(pPlayer->mItemSplitList.isLock())
				return MSG_ITEMSPLIT_ISLOCK;

			if(mSlotType != SHORTCUTTYPE_INVENTORY)
				return MSG_ITEM_CANNOTDRAGTOOBJECT;
		}
		break;
	case SHORTCUTTYPE_ITEMCOMPOSE:
		{
			if(pPlayer->mItemSplitList.isLock())
				return MSG_ITEMSPLIT_ISLOCK;

			if(mSlotType != SHORTCUTTYPE_INVENTORY)
				return MSG_ITEM_CANNOTDRAGTOOBJECT;
		}
		break;
	case SHORTCUTTYPE_REPAIR:
		{
			if(mSlotType == SHORTCUTTYPE_REPAIR)
				return MSG_ITEM_CANNOTDRAGTOOBJECT;
		}
		break;
	case SHORTCUTTYPE_TEMP:
		{
			
		}
		break;
	default:
		return MSG_UNKOWNERROR;

	}
	return MSG_NONE;
}

//更新
bool dGuiShortCut::updateSlot()
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return false;

	if(!isVisible())
		return false;

	bool dirty = m_DirtyFlag;
	mIsShowNum = false;
	ShortcutObject* pSlot = NULL;
	pSlot = g_ItemManager->getShortcutSlot(pPlayer, mSlotType, mSlotIndex);
	if(mOldSlot != pSlot || mSlotType == SHORTCUTTYPE_NPCSHOP)
	{
		mOldSlot = pSlot;
		dirty = true;
	}
	
	if(pSlot)
	{
		if(pSlot->getSlotState() == ShortcutObject::SLOT_LOCK)
		{
			if(!mIsForbided)
				dirty = true;			
			mIsForbided = true;
		}
		else
		{
			if(mIsForbided)
				dirty = true;
			mIsForbided = false;
		}

		if(pSlot->isItemObject())
		{
			mIsRenderFrame = true;
			char cTempPath[128] = "\0";
			dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_ITEM, pSlot->getIconName());
			StringTableEntry str = StringTable->insert(cTempPath);
			if(str != mIconTextureName || mTexture.isNull())
			{
				mIconTextureName = str;
				mTexture.set(mIconTextureName, &GFXDefaultGUIProfile);
				if(!mTexture)
					mTexture.set(UNKNOWICON, &GFXDefaultGUIProfile);
				dirty = true;
			}
		
			ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pSlot);
			S32 iTotalNum = 0;
			if(mSlotType == SHORTCUTTYPE_AUTOSELL)
				iTotalNum = pPlayer->inventoryList.GetItemCount(pItem,false,false);
			else if(mSlotType == SHORTCUTTYPE_STALL_BUYLIST)
				iTotalNum = pPlayer->stallBuyItemList.mSlots[mSlotIndex]->num;
			else if(mSlotType == SHORTCUTTYPE_TARGET_STALLBUYLIST)
			{
				SceneObject* pTargetObject = pPlayer->getInteraction();
				Player* pTarget = NULL;
				if(pTargetObject && (pTarget = dynamic_cast<Player*>(pTargetObject)))
					iTotalNum = pTarget->stallBuyItemList.mSlots[mSlotIndex]->num;
			}
			else
				iTotalNum = pItem->getRes()->getQuantity();

			if(mOldNum != iTotalNum)
			{
				mOldNum = iTotalNum;
				dirty = true;
			}			

			mFrames = pItem->getRes()->getColorLevel() - 1;
			mFrames = mFrames >= 0 && mFrames < 7 ? mFrames : 0;

			if(mSlotType == SHORTCUTTYPE_SUPERMARKET || mSlotType == SHORTCUTTYPE_REPAIR || mSlotType == SHORTCUTTYPE_NPCSHOP || mSlotType == SHORTCUTTYPE_RECOMMEND || 
				mSlotType == SHORTCUTTYPE_STALL_LOOKUP || mSlotType == SHORTCUTTYPE_PRESCRIPTION)
				return true;

			mNum[0] = mNum[1] = mNum[2] = -1;
		
			if((pItem->getRes()->getMaxOverNum() >1 || (mSlotType == SHORTCUTTYPE_STALL_BUYLIST || mSlotType == SHORTCUTTYPE_TARGET_STALLBUYLIST)) 
				&& !(mSlotType == SHORTCUTTYPE_AUTOSELL && pPlayer->autoSellList.getShowType() == 3) || mSlotType == SHORTCUTTYPE_SHOPBASKET)
			{
				mIsShowNum = iTotalNum > 0;
				char szNum[10];
				dSprintf(szNum, 10, "%d", iTotalNum);
				U32  szLen = dStrlen(szNum);
				if(szLen > 3)
				{
					mNum[0] = mNum[1] = mNum[2] = 9;
				}
				else
				{
					for(U32 n = 0; n < szLen; n++)
						mNum[n] = szNum[n] - '0';
				}
			}
		}
		else if(pSlot->isSkillObject())
		{
			mIsRenderFrame = true;
			mFrames = 0;
			char cTempPath[128] = "\0";
			dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_SKILL, pSlot->getIconName());
			StringTableEntry str = StringTable->insert(cTempPath);
			if(str != mIconTextureName|| mTexture.isNull())
			{
				mIconTextureName = str;
				mTexture.set(mIconTextureName, &GFXDefaultGUIProfile);
				if(!mTexture)
				{
					mTexture.set(UNKNOWICON, &GFXDefaultGUIProfile);
				}
				dirty = true;
			}
		}
		else if (pSlot->isLivingSkillObject())
		{
			mIsRenderFrame = false;
			char cTempPath[128] = "\0";
			dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_SKILL, pSlot->getIconName());
			StringTableEntry str = StringTable->insert(cTempPath);
			if(str != mIconTextureName|| mTexture.isNull())
			{
				mIconTextureName = str;
				mTexture.set(mIconTextureName, &GFXDefaultGUIProfile);
				if(!mTexture)
				{
					mTexture.set(UNKNOWICON, &GFXDefaultGUIProfile);
				}
				dirty = true;
			}
		}
		else if (pSlot->isPetObject())
		{
			mIsRenderFrame = false;
			char cTempPath[128] = "\0";
			PetShortcut *pPetShortcut = dynamic_cast<PetShortcut *>(pSlot);
			stPetInfo *pPetInfo = NULL;
			if ( pPetShortcut )
			{
				StringTableEntry sIconName = pPetShortcut->getIconName();
				if (sIconName && sIconName[0])
				{
					dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_PET, pPetShortcut->getIconName());
					StringTableEntry str = StringTable->insert(cTempPath);
					if(str != mIconTextureName || mTexture.isNull())
					{
						mIconTextureName = str;
						mTexture.set(mIconTextureName, &GFXDefaultGUIProfile);
						if(!mTexture)
							mTexture.set(UNKNOWICON, &GFXDefaultGUIProfile);
						dirty = true;
					}
				}
				else
				{
					mIconTextureName = NULL;
					mTexture = NULL;
				}
			}
			else
			{
				mIconTextureName = NULL;
				mTexture = NULL;
			}
		}
		else if (pSlot->isMountObject())
		{
			mIsRenderFrame = false;
			char cTempPath[128] = "\0";
			MountShortcut *pMountShortcut = dynamic_cast<MountShortcut *>(pSlot);
			stMountInfo *pMountInfo = NULL;
			if ( pMountShortcut && (pMountInfo = (stMountInfo *)pPlayer->getPetTable().getMountInfo(pMountShortcut->getSlotIndex())) )
			{
				StringTableEntry sIconName = pMountShortcut->getIconName();
				if (sIconName && sIconName[0])
				{
					dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_MOUNT, pMountShortcut->getIconName());
					StringTableEntry str = StringTable->insert(cTempPath);
					if(str != mIconTextureName || mTexture.isNull())
					{
						mIconTextureName = str;
						mTexture.set(mIconTextureName, &GFXDefaultGUIProfile);
						if(!mTexture)
							mTexture.set(UNKNOWICON, &GFXDefaultGUIProfile);
						dirty = true;
					}	
				}
				else
				{
					mIconTextureName = NULL;
					mTexture = NULL;
				}
			}
			else
			{
				mIconTextureName = NULL;
				mTexture = NULL;
			}
		}
		else if (pSlot->isSpiritObject())
		{
			mIsRenderFrame = false;
			char cTempPath[128] = "\0";
			SpiritShortcut *pSpiritShortcut = dynamic_cast<SpiritShortcut *>(pSlot);
			stSpiritInfo *pSpiritInfo = NULL;
			if ( pSpiritShortcut && (pSpiritInfo = (stSpiritInfo *)pPlayer->getSpiritTable().getSpiritInfo(pSpiritShortcut->getSlotIndex())) )
			{
				StringTableEntry sIconName = pSpiritShortcut->getIconName();
				if (sIconName && sIconName[0])
				{
					dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_SPIRIT, pSpiritShortcut->getIconName());
					StringTableEntry str = StringTable->insert(cTempPath);
					if(str != mIconTextureName || mTexture.isNull())
					{
						mIconTextureName = str;
						mTexture.set(mIconTextureName, &GFXDefaultGUIProfile);
						if(!mTexture)
							mTexture.set(UNKNOWICON, &GFXDefaultGUIProfile);
						dirty = true;
					}	
				}
				else
				{
					mIconTextureName = NULL;
					mTexture = NULL;
				}
			}
			else
			{
				mIconTextureName = NULL;
				mTexture = NULL;
			}
		}
		else if (pSlot->isTalentObject())
		{
			mIsRenderFrame = false;
			char cTempPath[128] = "\0";
			TalentShortcut *pTalentShortcut = dynamic_cast<TalentShortcut *>(pSlot);
			stSpiritInfo *pSpiritInfo = NULL;
			if ( pTalentShortcut)
			{
				StringTableEntry sIconName = pTalentShortcut->getIconName();
				if (sIconName && sIconName[0])
				{
					dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_TALENT, pTalentShortcut->getIconName());
					StringTableEntry str = StringTable->insert(cTempPath);
					if(str != mIconTextureName || mTexture.isNull())
					{
						mIconTextureName = str;
						mTexture.set(mIconTextureName, &GFXDefaultGUIProfile);
						if(!mTexture)
							mTexture.set(UNKNOWICON, &GFXDefaultGUIProfile);
						dirty = true;
					}	
				}
				else
				{
					mIconTextureName = NULL;
					mTexture = NULL;
				}
			}
			else
			{
				mIconTextureName = NULL;
				mTexture = NULL;
			}
		}
	}
	else
	{
		mIsForbided = false;
		if(mSlotType == SHORTCUTTYPE_NPCSHOP)
		{
			ShortcutObject* pItem = pPlayer->npcShopList.GetSlotTrue(mSlotIndex);
			if(!pItem)
			{
				mIconTextureName = StringTable->insert("");
				mTexture = NULL;
			}
			else
			{
				mTexture.set(UNKNOWICON, &GFXDefaultGUIProfile);
			}

		}
		else
		{
			mIconTextureName = StringTable->insert("");
			mTexture = NULL;
		}
	}	

	if(pSlot)
	{
		static SimTime cdTime;
		static SimTime totalTime;
		if(pPlayer->GetCooldown(pSlot->getCooldownGroup(), cdTime, totalTime, false))
		{
			dirty = true;
			mIsCool = true;
		}
		else if(mIsCool || mIsShowCoolFlash || mLastFrame)
		{
			if(!mIsCool && !mIsShowCoolFlash )	//最后一次需要绘制
				mLastFrame = false;

			dirty = true;
		}
	}
	return dirty;
}

void dGuiShortCut::onShortCut()
{
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(!conn)
		return;

	Player* control_player = dynamic_cast<Player*>(conn->getControlObject());
	if(!control_player || control_player->isDisabled())
		return;

	ShortcutObject* pShortcut = g_ItemManager->getShortcutSlot(control_player,mSlotType,mSlotIndex);
	if(!pShortcut)
		return;

	switch(mSlotType)
	{
	case SHORTCUTTYPE_PANEL:
		{
			if(pShortcut->isSkillObject())
			{
				SkillShortcut* pSkill = dynamic_cast<SkillShortcut*>(pShortcut);
				if(pSkill && pSkill->getSkill() && pSkill->getSkill()->GetData())
					control_player->PreCastSpell(pSkill->getSkill()->GetData()->GetID(), control_player->GetTarget(), NULL);
			}
			else if(pShortcut->isItemObject())
			{
				ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortcut);
				GameObject* pViewObject = control_player->GetTarget();
				if(!pViewObject)
					pViewObject = control_player;
				enWarnMessage msg = g_ItemManager->canUseItem(control_player, pItem, control_player);
				if(msg != MSG_NONE)
				{
					MessageEvent::show(SHOWTYPE_NOTIFY, msg);	
					return;
				}
				else if(pItem->getRes()->getUseStateID() > 0)
				{
					control_player->PreCastSpell(pItem->getRes()->getUseStateID(), pViewObject, NULL, mSlotType, mSlotIndex, pItem->getRes()->getUID());
				}
				else
				{
					ClientGameNetEvent* event = new ClientGameNetEvent(INFO_ITEM_USE);
					event->SetIntArgValues(2, mSlotType, mSlotIndex);
					control_player->getControllingClient()->postNetEvent(event);
				}
			}
			else if (pShortcut->isLivingSkillObject())
			{
				LivingSkillShortcut* pSkill = dynamic_cast<LivingSkillShortcut*>(pShortcut);
				if (pSkill && pSkill->getLivingSkill() && pSkill->getLivingSkill()->getData())
				{
					char sTab[8] = {0,};
					dSprintf(sTab,sizeof(sTab),"%d",(pSkill->getLivingSkill()->getData()->getCategory()-1));
					Con::executef("UI_OpenPrescription",sTab);
				}
			}
			else if (pShortcut->isPetObject())
			{
				PetShortcut *pPetShortcut = dynamic_cast<PetShortcut*>(pShortcut);
				S32 nSlot = pPetShortcut->getSlotIndex();
				const stPetInfo *pPetInfo = control_player->getPetTable().getPetInfo(nSlot);
				if (pPetInfo && pPetInfo->petDataId)
				{
					if (pPetInfo->status == PetObject::PetStatus_Idle)
						PetOperationManager::SpawnPet(control_player, nSlot);
					else if (pPetInfo->status == PetObject::PetStatus_Combat)
						PetOperationManager::DisbandPet(control_player, nSlot);
				}
			}
			else if (pShortcut->isMountObject())
			{
				MountShortcut *pMountShortcut = dynamic_cast<MountShortcut*>(pShortcut);
				S32 nSlot = pMountShortcut->getSlotIndex();
				const stMountInfo *pMountInfo = control_player->getPetTable().getMountInfo(nSlot);
				if (pMountInfo && pMountInfo->mDataId)
				{
					if (pMountInfo->mStatus == MountObject::MountStatus_Idle)
						PetMountOperationManager::SpawnMountPet(control_player, nSlot);
					else if (pMountInfo->mStatus == MountObject::MountStatus_Mount)
						PetMountOperationManager::DisbandMountPet(control_player, nSlot);
				}
			}
		}
		break;
	default: break;
	}
}

S32 dGuiShortCut::getSkillID()
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return 0;

	ShortcutObject* pShortcut = g_ItemManager->getShortcutSlot(pPlayer,mSlotType,mSlotIndex);
	if(!pShortcut)
		return 0;

	switch(mSlotType)
	{
	case SHORTCUTTYPE_LIVINGSKILL:
		{
			if (pShortcut->isLivingSkillObject())
			{
				LivingSkillShortcut* pSkill = dynamic_cast<LivingSkillShortcut*>(pShortcut);
				if (pSkill && pSkill->getLivingSkill() && pSkill->getLivingSkill()->getData())
					return pSkill->getLivingSkill()->getData()->getID();
			}
		}
		break;
	case SHORTCUTTYPE_SKILL_STUDY:
		{
			if (pShortcut->isSkillObject())
			{
				SkillStudyShortcut* pSkillData = dynamic_cast<SkillStudyShortcut*>(pShortcut);
				if (pSkillData && pSkillData->getSkillData())
					return pSkillData->getSkillData()->GetID();
			}
		}
		break;
	default : break;
	}
	return 0;
}

ConsoleMethod(dGuiShortCut,setBeSelected,void,3,3,"object.setBeSelected(%isSelected)")
{
	object->setBeSelected(dAtob(argv[2]));
}

ConsoleMethod(dGuiShortCut,getBeSelected,bool,2,2,"object.getBeSelected()")
{
	return object->getBeSelected();
}

ConsoleMethod(dGuiShortCut,setEnableEffect,void,3,4,"object.setEnableEffect(%isEnable, %times)")
{
	U32 times = 0;
	if(argc > 3)
		times = dAtoi(argv[3]);
	object->setEnableEffect(dAtob(argv[2]), times);
}

ConsoleMethod(dGuiShortCut,onShortCut,void,2,2,"object.onShortCut()")
{
	object->onShortCut();
}

ConsoleMethod(dGuiShortCut,getShortcutLivingSkillID,S32,2,2,"object.getShortcutLivingSkillID()")
{
	return object->getSkillID();
}

ConsoleMethod(dGuiShortCut,getShortcutStudySkillID,S32, 2, 2, "object.getShortCutStudySkillID()")
{
	return object->getSkillID();
}

ConsoleMethod(dGuiShortCut, showHot, void, 2, 2, "object.showHot()")
{
	object->showHotInfo();
}

ConsoleMethod(dGuiShortCut,setSlotCol,void, 3, 3, "object.setSlotCol(%index)")
{
	object->setSlotCol(atoi(argv[2]));
}

#endif

// ----------------------------------------------------------------------------
// 设备重置时调用
void dGuiShortCut::resetStateBlock()
{
	//mSetCoolSB
	GFX->beginStateBlock();
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPDisable );
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->endStateBlock(mSetCoolSB);

	//mClearCoolSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSLighting, true);
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPModulate);
	GFX->endStateBlock(mClearCoolSB);
}

// ----------------------------------------------------------------------------
// 设备丢失时调用
void dGuiShortCut::releaseStateBlock()
{
	if (mSetCoolSB)
	{
		mSetCoolSB->release();
	}

	if (mClearCoolSB)
	{
		mClearCoolSB->release();
	}
}

void dGuiShortCut::init()
{
	if (mSetCoolSB == NULL)
	{
		mSetCoolSB = new GFXD3D9StateBlock;
		mSetCoolSB->registerResourceWithDevice(GFX);
		mSetCoolSB->mZombify = &releaseStateBlock;
		mSetCoolSB->mResurrect = &resetStateBlock;

		mClearCoolSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}

// ----------------------------------------------------------------------------
//设备丢失时调用
void dGuiShortCut::shutdown()
{
	SAFE_DELETE(mSetCoolSB);
	SAFE_DELETE(mClearCoolSB);
}

// ----------------------------------------------------------------------------
// 静态资源初始化方法
void dGuiShortCut::Initialize()
{
	mNumTexture			= GFXTexHandle(ICONNUM, &GFXDefaultGUIProfile);
	mSelectTexture		= GFXTexHandle(SELECTFILENAME, &GFXDefaultGUIProfile);
	mEffectTexture		= GFXTexHandle(ICONEFFECTFILE, &GFXDefaultGUIProfile);
	mCoolFlashTexture	= GFXTexHandle(COOLFLASHFILE, &GFXDefaultGUIProfile);

	mCoolDownFont[0]	= CommonFontManager::GetFont("Bernard MT Condensed", 32);
	mCoolDownFont[1]	= CommonFontManager::GetFont("Bernard MT Condensed", 28);
	mCoolDownFont[2]	= CommonFontManager::GetFont("Bernard MT Condensed", 20);
	
}

// ----------------------------------------------------------------------------
// 静态资源释放方法
void dGuiShortCut::Destory()
{
	mNumTexture			= NULL;
	mSelectTexture		= NULL;
	mEffectTexture		= NULL;
	mCoolFlashTexture	= NULL;

	if(mCoolDownFont[0])
		mCoolDownFont[0]->ReduceRef();
	if(mCoolDownFont[1])
		mCoolDownFont[1]->ReduceRef();
	if(mCoolDownFont[2])
		mCoolDownFont[2]->ReduceRef();
}

// 清除选定的物品连接信息
ConsoleFunction( SptClearItemLink, void, 1, 1, "" )
{
	g_itemLink = "";
}

// 获得物品连接信息
ConsoleFunction( SptGetItemLink, const char*, 1, 1, "" )
{
	return StringTable->insert( g_itemLink.c_str() );
}

// 获得物品的描述信息
ConsoleFunction( SptGetItemLinkName, const char*, 1, 1, "" )
{
	return StringTable->insert( g_itemLinkName.c_str() );
}