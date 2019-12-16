#include "ServerGameNetEvents.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/ClientGameplayState.h"
#include "Util/LocalString.h"
#include "Gameplay/Item/Player_EquipIdentify.h"
#include "Gameplay/Item/Player_Bank.h"
#include "Gameplay/Data/RouteRepository.h"
#include "Gameplay/Item/Prescription.h"
#include "Gameplay/Item/ProduceEquip.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/GameObjects/ScheduleManager.h"
#include "Gameplay/GameObjects/CollectionObject.h"
#include "Gameplay/GameObjects/TimerTrigger.h"
#include "Gameplay/Item/IndividualStall.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/ClientPlayerManager.h"
#include "Gameplay/GameObjects/PetOperationManager.h"
#include "Gameplay/GameObjects/SpiritOperationManager.h"
#include "Gameplay/GameObjects/LivingSkill.h"
#include "Gameplay/GameObjects/PetMountOperation.h"
#include "Gameplay/GameObjects/PetOperationManager.h"
#include "Gameplay/GameObjects/NpcObject.h"

IMPLEMENT_CO_CLIENTEVENT_V1(ServerGameNetEvent);

ServerGameNetEvent::ServerGameNetEvent(U32 info_type)
{
	Parent::m_InfoType = info_type;
}

void ServerGameNetEvent::process(NetConnection *pCon)
{
#ifdef NTJ_CLIENT
	GameConnection* con = (GameConnection*)pCon;
	if(!con)
		return;

	Player* pPlayer = dynamic_cast<Player*>(con->getControlObject());
	if(!pPlayer)
		return;

	switch (m_InfoType)
	{
	case INFO_COOLDOWN:			// ��Ʒ��ȴ
		{
			pPlayer->SetCooldown_Group(GetIntArg(0), GetInt32Arg(0), GetInt32Arg(1));
		}
		break;
	case INFO_NPCSHOP:			// NPCϵͳ�̵꽻��
		{
			if(GetIntArg(0) == 0)	// �ر��̵�
			{
				g_ItemManager->closeNpcShop(pPlayer);
			}
			if(GetIntArg(0) == NPCSHOP_SELL)
			{
				
				Con::executef("refreshAutoSellWnd", Con::getIntArg(pPlayer->autoSellList.getShowType()));
			}
			/*if(GetIntArg(0) == NPCSHOP_OPEN)
			{
				g_ItemManager->openNpcShop(pPlayer, GetIntArg(1), GetIntArg(2));
			}*/
		}
		break;
	case INFO_TRADE:			// ��Ҽ佻��
		{
			U32 TradeState = GetInt32Arg(0);
			switch(TradeState)
			{
			case TRADE_ASKFOR:		// �յ���������
				{
					Player* targetPlayer = g_ClientGameplayState->findPlayer(GetInt32Arg(1));
					if(!targetPlayer)
						return;
					
					// ���������߼��ӳ��¼�
					Player::stLogicEvent* logicEvent = new Player::stLogicEvent;
					logicEvent->type		= Player::LOGICEVENT_TRADEINVITE;
					logicEvent->sender		= targetPlayer->getPlayerID();
					logicEvent->senderName	= targetPlayer->getPlayerName();
					logicEvent->data		= NULL;
					logicEvent->datalen		= 0;
					logicEvent->relationid	= GetInt32Arg(2);
					U32 eid = pPlayer->insertLogicEvent(logicEvent);

					// ����ͨ�������ʱ�Ի���
					Con::executef("CommonInviteDialog", Con::getIntArg(eid));
				}
				break;
			case TRADE_REJECT:	// �յ����׾ܾ�
				{
					Player* targetPlayer = g_ClientGameplayState->findPlayer(GetInt32Arg(1));
					if(!targetPlayer)
					{
						MessageEvent::show(SHOWTYPE_ERROR, MSG_PLAYER_TARGET_LOST);
						return;
					}
					else
					{
						MessageEvent::show(SHOWTYPE_ERROR, avar(GetLocalStr(MSG_TRADE_INVITE_REJECT), targetPlayer->getPlayerName()));
						return;
					}
				}
				break;
			case TRADE_ANSWER: // �ظ�����
				{
					Player* targetPlayer = g_ClientGameplayState->findPlayer(GetInt32Arg(1));
					if(!targetPlayer)
					{
						MessageEvent::show(SHOWTYPE_ERROR, MSG_PLAYER_TARGET_LOST);
						return;
					}
					pPlayer->tradeList.SetTradeState(TRADE_BEGIN);
					Con::executef("OpenTradeWnd", pPlayer->getPlayerName(), targetPlayer->getPlayerName());
				}
				break;
			case TRADE_LOCK:		// �Լ�������Ʒ����
				{
					pPlayer->tradeList.SetTradeState(TRADE_LOCK);
					//�Է��Ƿ�Ҳ�Ѿ�����������
					S32 iAlsoLock = GetInt32Arg(1);
					Con::executef("LockMySelfTradeWnd", Con::getIntArg(iAlsoLock));
				}
				break;
			case TRADE_LOCKNOTIFY:	// �յ��Է�������Ʒ������Ϣ
				{
					//�Է����׽�Ǯ
					S32 iTargetMoney = GetInt32Arg(1);
					// �Լ��Ƿ�Ҳ�Ѿ�����������
					S32 iAlsoLock = (pPlayer->tradeList.GetTradeState() == TRADE_LOCK)? 1 : 0;
					Con::executef("LockTargetTradeWnd", Con::getIntArg(iTargetMoney), Con::getIntArg(iAlsoLock));
				}
				break;
			case TRADE_UNLOCK:		// �Լ�������Ʒ�������
				{
					pPlayer->tradeList.SetTradeState(TRADE_BEGIN);
					Con::executef("UnLockMySelfTradeWnd");
				}
				break;
			case TRADE_UNLOCKNOTIFY:	// �յ��Է�������Ʒ���������Ϣ
				{
					Con::executef("UnLockTargetTradeWnd");
				}
				break;

			case TRADE_END:				// �Լ�ȷ�����׻�Ӧ
				{
					pPlayer->tradeList.SetTradeState(TRADE_END);
					Con::executef("EndMySelfTradeWnd");
				}
				break;
			case TRADE_ENDNOTIFY:		// �յ��Է�ȷ������֪ͨ
				{
					Con::executef("EndTargetTradeWnd");
				}
				break;
			}
		}
		break;
	case INFO_ITEM_IDENTIFY:	// װ������
		{
			gIdentifyManager->openIdentify(pPlayer);
		}
		break;
	case INFO_ITEM_STRENGTHEN:	// װ��ǿ��
		{
			S32 nOptType = GetInt32Arg(0);
			switch(nOptType)
			{
			case 0:		//�ͻ��˴򿪶Ի���
				{
					EquipStrengthen::openStrengthen(pPlayer, NULL);
				}
				break;
			case 1:		//�ͻ��˹رնԻ���
				{
					EquipStrengthen::closeStrengthen(pPlayer);
				}
				break;
			}
			break;
		}
	case INFO_ITEM_MOUNTGEM:	//װ����Ƕ
		{
			S32 nOptType = GetInt32Arg(0);

			switch(nOptType)
			{
			case 0:		//��Ƕ��ʯ�ɹ�
				{
					S32 nHoleIndex = GetInt32Arg(1);	//��óɹ���Ƕʱ�׵�λ��
					Con::executef("EquipEmbedGemWnd_OnMountSuccess", Con::getIntArg(nHoleIndex));
				}
				break;
			case 1:		//ժȡ��ʯ�ɹ�
				{
					S32 nHoleIndex = GetInt32Arg(1);	//��óɹ�ժȡʱ�׵�λ��
					Con::executef("EquipEmbedGemWnd_OnUnmountSuccess", Con::getIntArg(nHoleIndex));
				}
				break;
			}
			break;
		}
	case INFO_ITEM_PUNCHHOLE:	// װ�����
		{
			S32 nOptType = GetInt32Arg(0);
			ItemShortcut *pEquipItem = dynamic_cast<ItemShortcut *>(pPlayer->punchHoleList.GetSlot(0));

			switch(nOptType)
			{
			case 0:		//�ͻ��˴򿪶Ի���
				{
					EquipPunchHole::openPunchHole(pPlayer, NULL);
				}
				break;
			case 1:		//�ͻ��˹رնԻ���
				{
					EquipPunchHole::closePunchHole(pPlayer);
				}
				break;
			case 2:		//��׳ɹ�
				{
					S32 nHoleIndex = GetInt32Arg(1);	//��óɹ����ʱ�׵�λ��
					//ִ�гɹ��ű�
					Con::executef("EquipPunchHoleWnd_OnPunchHoleSuccess", Con::getIntArg(nHoleIndex));
				}
				break;
			case 3:		//���ʧ��
				{
					S32 nHoleIndex = GetInt32Arg(1);	//��óɹ����ʱ�׵�λ��
					//ִ�гɹ��ű�
					Con::executef("EquipPunchHoleWnd_OnPunchHoleFailure", Con::getIntArg(nHoleIndex));
				}
				break;
			}
			break;
		}
	case INFO_SKILL_OPERATE:
		{
			S32 type = GetIntArg(0);
			switch (type)
			{
			case 0:     //�ͻ��˴򿪼���ϵѡ��

				Con::executef("OpenSkillSelectWnd",Con::getIntArg(GetIntArg(1)),Con::getIntArg(GetIntArg(2)),Con::getIntArg(GetIntArg(3)),Con::getIntArg(GetIntArg(4)));
				break;
			case 1:     //�ͻ��˴򿪼���ѧϰ

				Con::executef("OpenSkillStudyWnd",Con::getIntArg(GetIntArg(1)),Con::getIntArg(GetIntArg(2)),Con::getIntArg(GetIntArg(3)),Con::getIntArg(GetIntArg(4)),Con::getIntArg(GetIntArg(5)));
				break;
			} 

		}
         break;
	case INFO_BANK:
		{
			switch (GetInt32Arg(0))
			{
			case PlayerBank::Bank_Open:
				{
					g_PlayerBank->openBank(pPlayer);
				}
				break;
			/*case PlayerBank::Bank_SaveMoney:
				{
					Con::executef("showBankMoney", Con::getIntArg(GetInt32Arg(1)));
				}
				break;
			case PlayerBank::Bank_GetMoney:
				{
					Con::executef("showBankMoney", Con::getIntArg(GetInt32Arg(1)));
				}
				break;*/
			case PlayerBank::Bank_Lock:
				{
					Con::executef("setLockFlag", Con::getIntArg(1));
				}
				break;
			}
			
		}
		break;
	case INFO_CHANGEMAXSLOTS:
		{
			switch (GetInt32Arg(0))
			{
			case SHORTCUTTYPE_BANK:
				{
					pPlayer->bankList.SetMaxSlot(GetInt32Arg(1));
					Con::executef("showSlots", Con::getIntArg(GetInt32Arg(1)));
					if(!g_PlayerBank->canBuySpace(pPlayer))
						Con::executef("setBuyActive", Con::getIntArg(0));
				}
				break;
			case SHORTCUTTYPE_INVENTORY:
				{
					pPlayer->inventoryList.SetMaxSlot(GetInt32Arg(1));
					Con::executef("initIventory");
				}
				break;
			case SHORTCUTTYPE_STALL:
				{

				}
				break;
			}
		}
		break;
	case INFO_INITIALIZCOMPLETE:		// ��ɫ���ݳ�ʼ���������
		{
			if(g_ClientGameplayState->getStrideServerPathFlag())
			{
				U32 iMapID = g_ClientGameplayState->getCurrentZoneId();
				std::vector<U32> vecPath = g_ClientGameplayState->getStrideServerPath();
				RouteData *pData = NULL;

				for(int i=0; i<vecPath.size(); ++i)
				{
					pData = g_RouteRepository.getRouteData(vecPath[i]);
					if(pData)
					{
						U32 ipathMap = g_RouteRepository.getMapID(pData->mMapID);
						if(ipathMap == iMapID)
						{
							g_ClientGameplayState->setFindPathID(vecPath[i]);
							Point3F position = pData->GetRoutePosition();
							pPlayer->SetTarget(position);
							g_ClientGameplayState->setDarwPath(true);
							break;
						}
					}
				}
			}
			((PetTable &)pPlayer->getPetTable()).initTempStats();		//��ʼ����ҳ���������ʾ����
			((SpiritTable &)pPlayer->getSpiritTable()).InitAllSpiritSlot(pPlayer);
		}
		break;
	case INFO_PRESCRIPTION_PRODUCE:
		{
			CHECK_MSGCOUNT(INFO_PRESCRIPTION_PRODUCE,2,1,0);
			U16 itype     = GetIntArg(0);
			U16 iCount    = GetIntArg(1);
			U32 iSerialId = GetInt32Arg(0); 

			Con::executef("uiUpdatePrescriptionItemInfo",Con::getIntArg(iSerialId));

			switch(itype)
			{
			case ProduceEquip::PRODUCE_PROCESS:
				{
					g_ProduceEquip.sendCurrentEvent(pPlayer,iSerialId,iCount);
				}
				break;
			case ProduceEquip::PRODUCE_CANCEL:
				{
					g_ProduceEquip.cancelProduce(pPlayer,0,0);
				}
				break;
			case ProduceEquip::PRODUCE_FINISHED:
				{
					g_ProduceEquip.finishProduce(pPlayer,0,0);
				}
				break;
			}
		}
		break;
	case INFO_COLLECTION_ITEM:		//�ɼ��¼�
		{
			S32 nOpType = GetInt32Arg(0);

			switch (nOpType)
			{
			case 1:		//�ͻ��˿�ʼ�ɼ�
				{					
					U32 nEventType = GetInt32Arg(1);

					if (nEventType == 1)
					{
						Con::executef("CloseFetchTimeProgress"); //ʰȡǰ��������������
						//���Ե����¼�
						DropItemEvent::ClientSendOpenDropItemList(pPlayer->getInteraction());
					}					
				}
				break;
			case 2:		//�ͻ��˽����ɼ�
				{
					Con::executef("CloseFetchTimeProgress");	//����ʱ�������������������
					CollectionObject *pCollection = pPlayer->mSelCollectionPtr.getObject();
					if (pCollection)
					{
						EventTriggerData *pEventTriggerData = gEventTriggerRepository.getEventTriggerData(pCollection->mDataBlock->mEventTriggerId);
						LivingSkillData* pData = NULL;
						if( pEventTriggerData && ((pData = g_LivingSkillRespository.getLivingSkillData(pEventTriggerData->mLivingSkillId)) != NULL) )
						{
							S32 skillId = pEventTriggerData->mLivingSkillId;
							Con::executef(	"UI_UpdateRipeInfoByID",
											Con::getIntArg(skillId),
											Con::getIntArg(pPlayer->getLivingSkillRipe(skillId)),
											Con::getIntArg(pData->getRipe()));
							
						}
						if (!pCollection->IsTriggerCountExceed() && pCollection->IsLoopTrigger())
						{
							//pCollection->clientSendStartMsg(pPlayer);
							pPlayer->setCollectionTarget(pCollection);
						}
					}
				}
				break;
			}
		}
		break;
	case INFO_CANCELVOCAL:
		{
			if(g_ClientGameplayState->pVocalStatus)
			{
				U16 type = g_ClientGameplayState->pVocalStatus->getStatusType();

				//�������������״̬  ��:�ֽ���������״̬
				switch(type)
				{
				case VocalStatus::VOCALSTATUS_ITEMSPLIT:
					{
						if(pPlayer->mItemSplitList.isLock())
							pPlayer->mItemSplitList.sendToCancelEvent(pPlayer);
					}
					break;
				case VocalStatus::VOCALSTATUS_ITEMCOMPOSE:
					{
						if(pPlayer->mItemSplitList.isLock())
							pPlayer->mItemComposeList.sendToComposeCancel(pPlayer);
					}
				}

				//ִ�нű���ֹ����
				Con::executef("recoveryButtonState",Con::getIntArg(type));
				Con::executef("cancleFetchTimeProgress");
			}
		}
		break;
	case INFO_LIVINGSKILL_LEARN:  //�������ѧϰ����
		{
			Con::executef("OpenStudyLivingSkillWnd",Con::getIntArg(GetIntArg(0)));
		}
		break;
	case INFO_TIMER_TRIGGER:
		{
			U32 nOptType = GetInt32Arg(0);
			U32 nId = GetInt32Arg(1);

			switch(nOptType)
			{
			case 1:			//���ɼ�ʱ��
				{
					U32 nLeftTime		= GetInt32Arg(2);
					U32 nIntervalTime	= GetInt32Arg(3);
					U32 nTimeFlag		= GetInt32Arg(4);
					bool bCancelFlag	= GetIntArg(0);
					bool bSaveFlag		= GetIntArg(1);

					TimerTrigger *pTimerTrigger = new TimerTrigger(nId, nLeftTime, nIntervalTime, bCancelFlag, bSaveFlag, nTimeFlag);

					Player *pPlayer  = g_ClientGameplayState->GetControlPlayer();
					if (pPlayer && pPlayer->mTimerTriggerMgr)
					{
						if (!pPlayer->mTimerTriggerMgr->AddToList(pTimerTrigger))
						{
							delete pTimerTrigger;
						}
					}
				}
				break;
			case 2:			//��ʼ��ʱ��
				{
					Player *pPlayer  = g_ClientGameplayState->GetControlPlayer();
					S32	nItemCount = GetInt32Arg(2);
					TimerTrigger *pTimerTrigger = NULL;					
					if (pPlayer && pPlayer->mTimerTriggerMgr &&
						(pTimerTrigger = dynamic_cast<TimerTrigger*>(pPlayer->mTimerTriggerMgr->GetTimerTrigger(nId))) != NULL )
					{
						char *block = GetMemoryBlock();
						for (S32 i = 0; i < nItemCount; i++)
						{							
							U32 nItemId = *((U32 *)(block + sizeof(U32) * i));
							pTimerTrigger->m_arrRewardIds.push_back(nItemId);
						}

						if (pTimerTrigger->Start() )
						{
							//��ʾ����
						}						
					}
				}
				break;
			case 3:			//ֹͣ��ʱ��
				{
					Player *pPlayer  = g_ClientGameplayState->GetControlPlayer();
					TimerTrigger *pTimerTrigger = NULL;
					if (pPlayer && pPlayer->mTimerTriggerMgr &&
						(pTimerTrigger = dynamic_cast<TimerTrigger *>(pPlayer->mTimerTriggerMgr->GetTimerTrigger(nId))) != NULL)
					{
						//�رս���
						Con::executef("SetTimerCtrlVisible", Con::getIntArg(pTimerTrigger->m_ctrlIndex), "0");
						pPlayer->mTimerTriggerMgr->DeleteFromList(nId);
						//������Ϣ����
						StringTableEntry content = Con::executef("GetTimerInfo", Con::getIntArg(nId), "2");
						if (content && content[0])
						{
							U32 nSize = dStrlen(content) + 1;
							char *sContent = Con::getArgBuffer(nSize);
							if (sContent)
							{
								dMemcpy(sContent, content, nSize);
								Con::executef("ShowTimerTriggerWelcomeWnd", sContent);
							}
						}
					}
				}
				break;		//ȡ����ʱ��
			case 4:
				{
					Player *pPlayer  = g_ClientGameplayState->GetControlPlayer();
					TimerTrigger *pTimerTrigger = NULL;
					if (pPlayer && pPlayer->mTimerTriggerMgr &&
						(pTimerTrigger = dynamic_cast<TimerTrigger *>(pPlayer->mTimerTriggerMgr->GetTimerTrigger(nId))) != NULL )
					{
						//�رս���
						Con::executef("SetTimerCtrlVisible", Con::getIntArg(pTimerTrigger->m_ctrlIndex), "0");
						pPlayer->mTimerTriggerMgr->DeleteFromList(nId);
					}
				}
				break;
			}
		}
		break;
	case INFO_TIMER_TRIGGER_PET_STUDY:
		{
			U32 nOptType = GetInt32Arg(0);
			U32 nId = GetInt32Arg(1);

			switch (nOptType)
			{
			case 1:		//���ɼ�ʱ��
				{
					U32 nLeftTime		= GetInt32Arg(2);
					U32 nTotalTime		= GetInt32Arg(3);
					U32 nIntervalTime	= GetInt32Arg(4);
					U32 nTimeFlag		= GetInt32Arg(5);
					U32 nStudyDataId	= GetInt32Arg(6);

					if (pPlayer && pPlayer->mTimerTriggerMgr)
					{
						pPlayer->mTimerPetStudyMgr->CreateTimerPetStudy(nId, nLeftTime, nTotalTime, nIntervalTime, nTimeFlag, nStudyDataId);
					}
				}
				break;
			case 2:		//ֹͣ��ʱ��
				{
					if (!pPlayer || !pPlayer->mTimerPetStudyMgr)
						return;
					TimerTriggerPetStudy *pTimer = dynamic_cast<TimerTriggerPetStudy*>(pPlayer->mTimerPetStudyMgr->GetTimerTrigger(nId));
					if (pTimer)
					{
						pPlayer->mTimerPetStudyMgr->DeleteFromList(nId);
						Con::executef("OnTimerPetStudyTerminate", Con::getIntArg(nId));
					}
				}
				break;
			case 3:		//�ı��ʱ��ID
				{
					U32 nId2 = GetInt32Arg(2);
					if (!pPlayer || !pPlayer->mTimerPetStudyMgr)
						return;
					pPlayer->mTimerPetStudyMgr->SwapTimerPetStudyId(nId, nId2);
				}
				break;
			}
		}
		break;
	case INFO_STALL:	//��̯
		{
			U32 iType = GetInt32Arg(0);
			switch (iType)
			{
			case TRADE_ASKFOR:
				{
					Player* targetPlayer = g_ClientGameplayState->findPlayer(GetInt32Arg(1));
					if(!targetPlayer)
					{
						MessageEvent::show(SHOWTYPE_ERROR, MSG_PLAYER_TARGET_LOST);
						return;
					}
					targetPlayer->individualStallList.setStallName(GetStringArg(0));
					g_Stall->openStall(pPlayer, 0, targetPlayer->getPlayerID());
				}
				break;
			case TRADE_BEGIN:
				{
					pPlayer->individualStallList.setStallState(TRADE_BEGIN);
					pPlayer->individualStallList.setStallName(GetStringArg(0));
					g_Stall->openStall(pPlayer, GetInt32Arg(2), GetInt32Arg(1));

				}
				break;
			case TRADE_DOWNITEM:
				{
					S32 index = GetInt32Arg(1);
					U32 type = GetInt32Arg(2);
					Con::executef("clearSelectState", Con::getIntArg(index), Con::getIntArg(type));
				}
				break;
			case TRADE_SETMONEY:
				{
					U32 index = GetInt32Arg(1);
					U32 money = GetInt32Arg(2);
					U32 iType = GetInt32Arg(3);
					U32 num = 0;
					if(iType == 1)
					{
						ItemShortcut* pItem = pPlayer->individualStallList.GetSlot(index);
						if(!pItem)
							return;
						num = pItem->getRes()->getQuantity();
					}
					else if(iType == 2)
					{
						PetShortcut* petSlot = pPlayer->stallPetList.GetSlot(index);
						if(!petSlot)
							return;
						num = 1;
					}
					else
					{
						stStallBuyItem* item = pPlayer->stallBuyItemList.mSlots[index];
						if(!item)
							return;
						num = item->num;
					}
					Con::executef("showStallMoney", Con::getIntArg(money), Con::getIntArg(num));
				}
				break;
			case STALL_RENAME:
				{
					pPlayer->individualStallList.setStallName(GetStringArg(0));
					Con::executef("stallRename", GetStringArg(0));
				}
				break;
			case STALL_REINFO:
				{
					pPlayer->individualStallList.setStallInfo(GetStringArg(0));
					Con::executef("stallReInfo", GetStringArg(0));
				}
				break;
			case STALL_REMOVE_ALL:
				{
					Con::executef("clearAllSelect");
				}
				break;
			case TRADE_LOCK:
				{
					pPlayer->individualStallList.setStallState(TRADE_LOCK);
					Con::executef("showStallState", Con::getIntArg(1));

				}
				break;
			case TRADE_UNLOCK:
				{
					pPlayer->individualStallList.setStallState(TRADE_UNLOCK);
					Con::executef("showStallState", Con::getIntArg(0));
				}
				break;
			case TRADE_CANCEL:
				{
					if(pPlayer->getPlayerID() == GetInt32Arg(1))
						Con::executef("CloseStallSeeWnd");
					U32 playerID = GetInt32Arg(2);
					Player* targetPlayer = g_ClientGameplayState->findPlayer(playerID);
					if(!targetPlayer)
					{
						MessageEvent::show(SHOWTYPE_ERROR, MSG_PLAYER_TARGET_LOST);
						return;
					}
					targetPlayer->individualStallList.clear();
					targetPlayer->stallPetList.clear();
					targetPlayer->stallBuyItemList.clear();
				}
				break;
			case STALL_CHATMSG:
				{
					U32 playerID = GetInt32Arg(1);
					U32 flag = GetInt32Arg(2);
					Player* targetPlayer = g_ClientGameplayState->findPlayer(playerID);
					if(!targetPlayer)
					{
						MessageEvent::show(SHOWTYPE_ERROR, MSG_PLAYER_TARGET_LOST);
						return;
					}

					if(flag == 1)
					{
						if(targetPlayer->individualStallList.mChatInfoList.empty())
							return;
						stChatRecord* pRecord = targetPlayer->individualStallList.mChatInfoList.last();
						if(!pRecord)
							return;
						char *chatContent = Con::getArgBuffer(256);
						struct tm *systime;
						time_t mtime = pRecord->time;
						systime = localtime((time_t*)&mtime);
						dSprintf(chatContent, 256, "<t>[%d:%d][%s]����˵:%s</t>", systime->tm_hour, systime->tm_min, pRecord->senderName, pRecord->chatContent);
						Con::executef("addStallChatMessage", chatContent);
					}
				}
				break;
			case STALL_TRADERECORD:
				{
					U32 playerID = GetInt32Arg(1);
					U32 flag = GetInt32Arg(2);
					Player* targetPlayer = g_ClientGameplayState->findPlayer(playerID);
					if(!targetPlayer)
					{
						MessageEvent::show(SHOWTYPE_ERROR, MSG_PLAYER_TARGET_LOST);
						return;
					}
					if(flag == 1)//һ��һ��
					{
						if(targetPlayer->individualStallList.mTradeInfoList.empty())
							return;
						stTradeRecord* pRecord = targetPlayer->individualStallList.mTradeInfoList.last();
						if(!pRecord)
							return;
						
						char *tradeContent = Con::getArgBuffer(256);
						struct tm *systime;
						time_t mtime = pRecord->time;
						systime = localtime((time_t*)&mtime);
						if(pRecord->type == stTradeRecord::TRADE_ITEM)
						{
							ItemBaseData* pItemData = g_ItemRepository->getItemData(pRecord->trade.item->ItemID);
							if(!pItemData)
								return;
							U32 num = pRecord->trade.item->Quantity;

							if(GetInt32Arg(3) != 1)
								dSprintf(tradeContent, 256, "<t>[%d:%d][%s]���㹺����[%s]��%d,���%dͭ</t>", systime->tm_hour, systime->tm_min, pRecord->senderName,pItemData->getItemName(), num, pRecord->money);
							else
								dSprintf(tradeContent, 256, "<t>[%d:%d]����[%s]������[%s]��%d,ʧȥ%dͭ</t>", systime->tm_hour, systime->tm_min, pRecord->receiverName, pItemData->getItemName(), num, pRecord->money);
						}
						else if(pRecord->type == stTradeRecord::TRADE_PET)
							dSprintf(tradeContent, 256, "<t>[%d:%d][%s]���㹺����[%s]��1,���%dͭ</t>", systime->tm_hour, systime->tm_min, pRecord->senderName, pRecord->trade.pet->name, pRecord->money);
						Con::executef("addStallTradeMessage", tradeContent);
					}
				}
				break;
			case STALL_INFOMSG:
				{
					if(pPlayer->getPlayerID() == GetInt32Arg(1))
					{
						pPlayer->individualStallList.setStallInfo(GetStringArg(0));
						Con::executef("stallReInfo", GetStringArg(0), Con::getIntArg(0));

						int tradeInfoSize = pPlayer->individualStallList.mTradeInfoList.size();
						struct tm *systime;

						for(int i=0; i<tradeInfoSize; ++i)
						{
							stTradeRecord* pRecord = pPlayer->individualStallList.mTradeInfoList[i];
							char *tradeContent = Con::getArgBuffer(256);
							if(pRecord)
							{
								time_t mtime = pRecord->time;
								systime = localtime((time_t*)&mtime);
								if(pRecord->type == stTradeRecord::TRADE_ITEM)
								{
									ItemBaseData* pItemData = g_ItemRepository->getItemData(pRecord->trade.item->ItemID);
									U32 num = pRecord->trade.item->Quantity;
									if(pItemData)
									{
										if(pRecord->money >= 0)
											dSprintf(tradeContent, 256, "<t>[%d:%d][%s]���㹺����[%s]��%d,���%dͭ</t>", systime->tm_hour, systime->tm_min, pRecord->senderName, pItemData->getItemName(), num, pRecord->money);
										else
											dSprintf(tradeContent, 256, "<t>[%d:%d][%s]���������[%s]��%d,��ʧȥ%dͭ</t>", systime->tm_hour, systime->tm_min, pRecord->senderName, pItemData->getItemName(), num, pRecord->money);
									}
								}
								else if(pRecord->type == stTradeRecord::TRADE_PET)
								{
									dSprintf(tradeContent, 256, "<t>[%d:%d][%s]���㹺����[%s]��1,���%dͭ</t>", systime->tm_hour, systime->tm_min, pRecord->senderName, pRecord->trade.pet->name, pRecord->money);
								}
								Con::executef("addStallTradeMessage", tradeContent);
								
							}
						}

						char *chatContent = Con::getArgBuffer(256);
						for(int i=0; i<pPlayer->individualStallList.mChatInfoList.size(); ++i)
						{
							stChatRecord* pRecord = pPlayer->individualStallList.mChatInfoList[i];
							if(!pRecord)
								return;

							time_t mtime = pRecord->time;
							systime = localtime((time_t*)&mtime);
							dSprintf(chatContent, 256, "<t>[%d:%d][%s]����˵:%s</t>", systime->tm_hour, systime->tm_min, pRecord->senderName, pRecord->chatContent);
							Con::executef("addStallChatMessage", chatContent);

						}
					}
					else
					{
						char stallName[256] = "\0";
						dSprintf(stallName, sizeof(stallName), GetStringArg(0));
						if(stallName[0] == '\0')
						{
							dSprintf(stallName, sizeof(stallName), "%s", "̯������,��ȻʲôҲû����...");
						}
						Con::executef("stallReInfo", stallName, Con::getIntArg(1));

						Player* targetPlayer = g_ClientGameplayState->findPlayer(GetInt32Arg(1));
						if(!targetPlayer)
							return;
						int tradeInfoSize = targetPlayer->individualStallList.mTradeInfoList.size();
						struct tm *systime;
						for(int i=0; i<tradeInfoSize; ++i)
						{
							stTradeRecord* pRecord = targetPlayer->individualStallList.mTradeInfoList[i];
							char *tradeContent = Con::getArgBuffer(256);
							if(pRecord)
							{
								time_t mtime = pRecord->time;
								systime = localtime((time_t*)&mtime);
								if(pRecord->type == stTradeRecord::TRADE_ITEM)
								{
									ItemBaseData* pItemData = g_ItemRepository->getItemData(pRecord->trade.item->ItemID);
									U32 num = pRecord->trade.item->Quantity;
									if(pItemData)
									{
										if(pRecord->money >= 0)
											dSprintf(tradeContent, 256, "<t>[%d:%d][%s]��[%s]������[%s]��%d,̯�����%dͭ</t>", systime->tm_hour, systime->tm_min, pRecord->senderName, pRecord->receiverName, pItemData->getItemName(), num, pRecord->money);
										else
											dSprintf(tradeContent, 256, "<t>[%d:%d][%s]��[%s]������[%s]��%d,̯��ʧȥ%dͭ</t>", systime->tm_hour, systime->tm_min, pRecord->senderName, pRecord->receiverName, pItemData->getItemName(), num, pRecord->money);
									}
								}
								else if(pRecord->type == stTradeRecord::TRADE_PET)
									dSprintf(tradeContent, 256, "<t>[%d:%d][%s]���㹺����[%s]��1,���%dͭ</t>", systime->tm_hour, systime->tm_min, pRecord->senderName, pRecord->trade.pet->name, pRecord->money);
								Con::executef("addStallTradeMessage", tradeContent);
							}
						}
					}
				}
				break;
			case STALL_CLEAR_MSG:
				{
					for(int i=0; i<pPlayer->individualStallList.mChatInfoList.size(); ++i)
					{
						stChatRecord* pRecord = pPlayer->individualStallList.mChatInfoList[i];
						if(pRecord)
							delete pRecord;
						pPlayer->individualStallList.mChatInfoList.erase_fast(i--);
					}
					for(int i=0; i<pPlayer->individualStallList.mTradeInfoList.size(); ++i)
					{
						stTradeRecord* pRecord = pPlayer->individualStallList.mTradeInfoList[i];
						if(pRecord)
							delete pRecord;
						pPlayer->individualStallList.mTradeInfoList.erase_fast(i--);
					}					
					Con::executef("clearTradeAndChatMessage");
				}
				break;
			case STALL_SEE_CLOSE:
				{
					U32 playerID = GetInt32Arg(1);
					Player* targetPlayer = g_ClientGameplayState->findPlayer(playerID);
					if(!targetPlayer)
					{
						MessageEvent::show(SHOWTYPE_ERROR, MSG_PLAYER_TARGET_LOST);
						return;
					}
					targetPlayer->individualStallList.clear();
					targetPlayer->stallPetList.clear();
					targetPlayer->stallBuyItemList.clear();

				}
				break;
			}
		}
		break;
		case INFO_PET_OPERATE:
			{
				U32 nSlot = GetIntArg(1);
				switch(GetIntArg(0))
				{
				case Player::PetOp_UseEgg:
					{
						Con::executef("CloseFetchTimeProgress");
						if(pPlayer->getPetTable().getPetInfoCount() == 1)
						{
							if(g_ClientGameplayState && g_ClientGameplayState->pVocalStatus)
								g_ClientGameplayState->pVocalStatus->clear();
							PetOperationManager::SpawnPet(pPlayer, 0);
						}
					}
					break;
				case Player::PetOp_Spawn:
					{
						bool bSuccess = GetIntArg(2);
						Con::executef("CloseFetchTimeProgress");
						if (bSuccess)
							Con::executef("OnSpawnPetSuccess");
						else
							Con::executef("OnSpawnPetFailure");
					}
					break;			
				case Player::PetOp_Disband:
					{
						bool bSuccess = GetIntArg(2);
						if (bSuccess)
							Con::executef("OnDisbandPetSuccess");
						else
							Con::executef("OnDisbandPetFailure");
					}
					break;
				case Player::PetOp_Release:
					{
						bool bSuccess = GetIntArg(2);
						if (bSuccess)
							Con::executef("OnReleasePetSuccess");
						else
							Con::executef("OnReleasePetFailure");
					}
					break;
				case Player::PetOp_LianHua:
					{
						switch(GetIntArg(2))
						{
						case 1:			// �򿪳�����������
							{
								PetOperationManager::OpenPetLianHua(pPlayer, NULL);
							}
							break;
						case 2:			// ���������ɹ�
							{
								Con::executef("OnPetLianHuaSuccess");
							}
							break;
						case 3:			// ��������ʧ��
							{
								Con::executef("OnPetLianHuaFailure");
							}
							break;
						}
					}
					break;
				case Player::PetOp_Insight:
					{
						switch(GetIntArg(2))
						{
						case 1:			// �������۽���
							{
								PetOperationManager::OpenPetInsight(pPlayer, NULL);
							}
							break;
						case 2:			// �����۳ɹ�
							{
								Con::executef("OnPetInsightSuccess");
							}
							break;
						case 3:			// ������ʧ��
							{
								Con::executef("OnPetInsightFailure");
							}
							break;
						}
					}
					break;
				case Player::PetOp_Identify:
					{
						switch(GetIntArg(2))
						{
						case 1:			// �򿪳ɳ��ʼ�������
							{
								PetOperationManager::OpenPetIdentify(pPlayer, NULL);
							}
							break;
						case 2:			// �ɳ��ʼ����ɹ�
							{
								Con::executef("OnPetIdentifySuccess");
							}
							break;
						case 3:			// �ɳ��ʼ���ʧ��
							{
								Con::executef("OnPetIdentifyFailure");
							}
							break;
						}
					}
					break;
				case Player::PetOp_BecomeBaby:
					{
						switch(GetIntArg(2))
						{
						case 1:			// �򿪻�ͯ����
							{
								PetOperationManager::OpenPetHuanTong(pPlayer, NULL);
							}
							break;
						case 2:			// ��ͯ�ɹ�
							{
								Con::executef("OnPetHuanTongSuccess");
							}
							break;
						case 3:			// ��ͯʧ��
							{
								Con::executef("OnPetHuanTongFailure");
							}
							break;
						}
					}
					break;
				case Player::PetOp_ChangeName:
					{
						U32 nSlot		= GetIntArg(1);
						bool bSuccess	= GetIntArg(2);
						if (bSuccess)
						{
							Con::executef("OnPetChangeNameSuccess");
						}
						else
						{
							Con::executef("OnPetChangeNameFailure");
						}
					}
					break;
				case Player::PetOp_ChangeProperties:
					{
						U32 nSlot		= GetIntArg(1);
						bool bSuccess	= GetIntArg(2);
						if (bSuccess)
						{
							Con::executef("OnPetChangePropertiesSuccess");
						}
						else
						{
							Con::executef("OnPetChangePropertiesFailure");
						}
					}
					break;
				case Player::PetOp_UnLink:
					{
						U32 nPetIdx		= GetIntArg(1);
						bool bSuccess	= GetIntArg(2);
						if (bSuccess)
						{
							char msg[256];
							PetTable &table = ((PetTable &)pPlayer->getPetTable());
							PetObject *pPet = table.getPet(nPetIdx);
							StringTableEntry sPetName = pPet->getName();
							dSprintf(msg, sizeof(msg), "����[%s]����", sPetName);
							table.unlinkPet(nPetIdx);
							Con::executef("OnPetDead");
							Con::executef("AddChatMessage", "13", msg, "0", "0");
						}
					}
					break;
				case Player::PetOp_AddExp:
					{
						U32 nPetSlot		= GetIntArg(1);
						U32 nAddedExp		= GetIntArg(2);

						char msg[256];
						PetTable &table = ((PetTable &)pPlayer->getPetTable());
						StringTableEntry sPetName = table.getPetName(nPetSlot);
						dSprintf(msg, sizeof(msg), "����[%s]���%d����", sPetName, nAddedExp);
						Con::executef("AddChatMessage", "13", msg, "0", "0");
					}
					break;
				case Player::PetOp_AddLevel:
					{
						U32 nPetSlot		= GetIntArg(1);
						U32 nAddedLevel		= GetIntArg(2);						
						
						char msg[256];
						PetTable &table = ((PetTable &)pPlayer->getPetTable());
						StringTableEntry sPetName = table.getPetName(nPetSlot);
						dSprintf(msg, sizeof(msg), "����[%s]�ȼ�������%d��", sPetName, nAddedLevel);
						Con::executef("AddChatMessage", "13", msg, "0", "0");
					}
					break;
				default:
					break;
				}
			}
			break;
	case INFO_MOUNT_PET_OPERATE:
		{
			U32 nSlot = GetIntArg(1);
			bool bSuccess = GetIntArg(2);
			switch(GetIntArg(0))
			{
			case Player::PetOp_UseMountPetEgg:
				{
					Con::executef("CloseFetchTimeProgress");
					if(pPlayer->getPetTable().getMountInfoCount() == 1)
					{
						if(g_ClientGameplayState && g_ClientGameplayState->pVocalStatus)
							g_ClientGameplayState->pVocalStatus->clear();
						PetMountOperationManager::SpawnMountPet(pPlayer, 0);
					}
				}
				break;
			case Player::PetOp_SpawnMountPet:
				{
					Con::executef("CloseFetchTimeProgress");
					if (bSuccess)
						Con::executef("OnSpawnMountSuccess");
					else
						Con::executef("OnSpawnMountFailure");
				}
				break;
			case Player::PetOp_DisbandMountPet:
				{
					if (bSuccess)
						Con::executef("OnDisbandMountSuccess");
					else
						Con::executef("OnSDisbandMountFailure");
				}
				break;
			case Player::PetOp_ReleaseMountPet:
				{
					if (bSuccess)
						Con::executef("OnReleaseMountSuccess");
					else
						Con::executef("OnReleaseMountFailure");
				}
				break;
			default:
				break;
			}
		}
		break;
	case INFO_SPIRIT_OPERATE:
		{
			switch(GetIntArg(0))
			{
			case Player::SpiritOp_OpenSpiritSlot:
				{	
					U32 nSlot = GetIntArg(1);
					bool bSuccess = GetIntArg(2);
					if (bSuccess)
					{
						pPlayer->getSpiritTable().openSlot(nSlot);
						Con::executef("OnOpenSpiritSlotSuccess", Con::getIntArg(nSlot));						
					}
					else
						Con::executef("OnOpenSpiritSlotFailure", Con::getIntArg(nSlot));
				}
				break;
			case Player::SpiritOp_AddSpirit:
				{
					U32 nSlot = GetIntArg(1);
					bool bSuccess = GetIntArg(2);
					if (bSuccess)
					{
						pPlayer->getSpiritTable().AddSpiritSkillSlot(pPlayer, nSlot);
						pPlayer->getSpiritTable().AddTalentSkillSlot(pPlayer, nSlot);
						Con::executef("OnAddSpiritSuccess", Con::getIntArg(nSlot));
					}
					else
						Con::executef("OnAddSpiritFailure", Con::getIntArg(nSlot));
				}
				break;
			case Player::SpiritOp_RemoveSpirit:
				{
					U32 nSlot = GetIntArg(1);
					bool bSuccess = GetIntArg(2);
					if (bSuccess)
					{
						pPlayer->getSpiritTable().clearSpiritSkillSlot(nSlot);
						pPlayer->getSpiritTable().clearTalentSkillSlot(nSlot);						
						Con::executef("OnRemoveSpiritSuccess", Con::getIntArg(nSlot));
					}
					else
						Con::executef("OnRemoveSpiritFailure", Con::getIntArg(nSlot));
				}
				break;
			case Player::SpiritOp_HuanHuaSpirit:
				{
					U32 nSlot = GetIntArg(1);
					bool bSuccess = GetIntArg(2);
					if (bSuccess)
					{
						pPlayer->getSpiritTable().setSpiritHuanHua(true);
						Con::executef("OnHuanHuaSpiritSuccess", Con::getIntArg(nSlot));
					}
					else
						Con::executef("OnHuanHuaSpiritFailure", Con::getIntArg(nSlot));
				}
				break;
			case Player::SpiritOp_CancelHuanHuaSpirit:
				{
					U32 nSlot = GetIntArg(1);
					bool bSuccess = GetIntArg(2);
					if (bSuccess)
					{
						pPlayer->getSpiritTable().setSpiritHuanHua(false);
						Con::executef("OnCancelHuanHuaSuccess", Con::getIntArg(nSlot));
					}
					else
						Con::executef("OnCancelHuanHuaFailure", Con::getIntArg(nSlot));
				}
				break;
			case Player::SpiritOp_ActivateSpirit:
				{
					U32 nSlot = GetIntArg(1);
					bool bSuccess = GetIntArg(2);
					if (bSuccess)
					{
						pPlayer->getSpiritTable().setActiveSlot(nSlot);
						Con::executef("OnActivateSpiritSuccess", Con::getIntArg(nSlot));
					}
					else
						Con::executef("OnActivateSpiritFailure", Con::getIntArg(nSlot));
				}
				break;
			case Player::SpiritOp_Initialize:
				{
					U32 nActiveSlot = GetIntArg(1);
					U32 nOpenedSlotFlag = GetIntArg(2);
					pPlayer->getSpiritTable().initialize(nActiveSlot, nOpenedSlotFlag);
				}
				break;
			case Player::SpiritOp_OpenJueZhaoSlot:
				{
					U32 nSlot = GetIntArg(1);
					bool bSuccess = GetIntArg(2);
					if (bSuccess)
					{
						pPlayer->getSpiritTable().setActiveSlot(nSlot);
						Con::executef("OnOpenJueZhaoSlotSuccess", Con::getIntArg(nSlot));
					}
					else
						Con::executef("OnOpenJueZhaoSlotFailure", Con::getIntArg(nSlot));
				}
				break;
			case Player::SpiritOp_AddJueZhao:
				{
					U32 nSlot = GetIntArg(1);
					bool bSuccess = GetIntArg(2);
					if (bSuccess)
					{
						pPlayer->getSpiritTable().setActiveSlot(nSlot);
						Con::executef("OnAddJueZhaoSuccess", Con::getIntArg(nSlot));
					}
					else
						Con::executef("OnAddJueZhaoFailure", Con::getIntArg(nSlot));
				}
				break;
			case Player::SpiritOp_RemoveJueZhao:
				{
					U32 nSlot = GetIntArg(1);
					bool bSuccess = GetIntArg(2);
					if (bSuccess)
					{
						pPlayer->getSpiritTable().setActiveSlot(nSlot);
						Con::executef("OnRemoveJueZhaoSuccess", Con::getIntArg(nSlot));
					}
					else
						Con::executef("OnRemoveJueZhaoFailure", Con::getIntArg(nSlot));
				}
				break;
			case Player::SpiritOp_AddSpiritExp:
				{
					S32 nSpiritSlot		= GetIntArg(1);
					S32 nAddedExp		= GetIntArg(2);
					Con::executef("OnAddSpiritExp", Con::getIntArg(nSpiritSlot), Con::getIntArg(nAddedExp));
					/*char msg[256];
					PetTable &table = ((PetTable &)pPlayer->getPetTable());
					StringTableEntry sPetName = table.getPetName(nPetSlot);
					dSprintf(msg, sizeof(msg), "����[%s]���%d����", sPetName, nAddedExp);
					Con::executef("AddChatMessage", "13", msg, "0", "0");*/
				}
				break;
			case Player::SpiritOp_AddSpiritLevel:
				{
					S32 nSpiritSlot		= GetIntArg(1);
					S32 nAddedLevel		= GetIntArg(2);						
					Con::executef("OnAddSpiritLevel", Con::getIntArg(nSpiritSlot), Con::getIntArg(nAddedLevel));
					/*char msg[256];
					PetTable &table = ((PetTable &)pPlayer->getPetTable());
					StringTableEntry sPetName = table.getPetName(nPetSlot);
					dSprintf(msg, sizeof(msg), "����[%s]�ȼ�������%d��", sPetName, nAddedLevel);
					Con::executef("AddChatMessage", "13", msg, "0", "0");*/
				}
				break;
			case Player::SpiritOp_AddTalentLevel:
				{
					S32 nSpiritSlot = GetIntArg(1);
					S32 nTalentIndex = GetIntArg(2);
					bool bSuccess = GetIntArg(3);
					if (bSuccess)
					{
						pPlayer->getSpiritTable().updateTalentSkillSlot(pPlayer, nSpiritSlot, nTalentIndex);
						Con::executef("OnAddSpiritTalentLevel", Con::getIntArg(nSpiritSlot));
					}
				}
				break;
			case Player::SpiritOp_OpenJueZhaoWnd:
				{
					NpcObject *pNpc = dynamic_cast<NpcObject*>(pPlayer->getInteraction());
					SpiritOperationManager::OpenSpiritSkillWnd(pPlayer, pNpc);
				}
				break;
			default:
				break;
			}
		}
		break;
	case INFO_QUICKCHANGE:
		{
			U32 falg = GetInt32Arg(0);
		}
		break;
	case INFO_GETENCOURAGE:
		{
			int nRet = GetInt32Arg( 0 );
			Con::evaluatef("OnEncourageResult(%d);", nRet );
		}
		break;
	case INFO_ENTER_TRANSPORT:
		{		
			int ZoneId = GetInt32Arg( 0 );
			Con::evaluatef("ShowTransportTexture(%d);", ZoneId );
			ClientGameNetEvent* pE = new ClientGameNetEvent( INFO_ENTER_TRANSPORT );
			GameConnection* pConn = pPlayer->getControllingClient();
			if( pConn )
				pConn->postNetEvent( pE );
		}
		break;
	case INFO_TEMPLIST:
		{
			switch(GetIntArg(0))
			{
			case 1:	//�ı�templist��OptType
				{
					pPlayer->tempList.SetOptType(NULL, (TempList::OptType)GetIntArg(1));
				}
				break;
			case 2:	//���templist
				{
					pPlayer->tempList.Clean(NULL);
				}
				break;
			}
		}
		break;
	case INFO_MPMANAGER:
		{
			CHECK_MSGCOUNT(INFO_MPMANAGER,1,1,0);
			S32 srcPlayerId = GetInt32Arg(0);
			SocialType::Type type = (SocialType::Type) GetIntArg(0);

			Player* srcPlayer = PLAYER_MGR->GetLocalPlayer(srcPlayerId);
			if(!srcPlayer)
				return;

			switch(type)
			{
			case SocialType::Prentice:
				{
					Con::executef("uiRelationPrentice",Con::getIntArg(srcPlayer->getId()));
				}
				break;
			case SocialType::Master:
				{
					Con::executef("uiRelationMaster",Con::getIntArg(srcPlayer->getId()));
				}
				break;
			default: return;
			}
		}
		break;
	case MAIL_SEND_OK:
		{
			Con::executef("sendMailOk");
		}
		break;
	case MAIL_DELETEALL:
		{
			Con::executef("deleteAllMailAck");
		}
		break;
	case INFO_MISSION:		// �����¼�����
		{
			
		}
		break;
	case INFO_ITEM_REPAIR:
		{
			U32 iType = GetInt32Arg(0);
			g_ItemManager->openRepair(pPlayer, iType);
		}
		break;
	case INFO_ITEMSPLIT:
		{
			CHECK_MSGCOUNT(INFO_ITEMSPLIT,1,0,0);

			U8 type = GetIntArg(0);
			switch(type)
			{
			case ItemSplit::ITEMSPLIT_OPENWND:
				{
					Con::executef("openItemSplitGui");
				}
				break;
			case ItemSplit::ITEMSPLIT_END:
				{
					pPlayer->mItemSplitList.finishItemSplit(pPlayer);
				}
				break;
			}
		}
		break;
	case INFO_ITEMCOMPOSE:
		{
			U8 type = GetIntArg(0);
			if(type == (U8)ItemCompose::ITEMCOMPOSE_OPENWND)
			{
				CHECK_MSGCOUNT(INFO_ITEMCOMPOSE,1,0,1);
			}
			else
			{
				CHECK_MSGCOUNT(INFO_ITEMCOMPOSE,1,0,0);
			}

			switch(type)
			{
			case ItemCompose::ITEMCOMPOSE_OPENWND:
				{
					//���ԭ���ϳ���Ϣ
					pPlayer->mItemComposeList.clearComposeInfo();

					std::string  szStr = GetStringArg(0);
					if(szStr == "")
					{
						Con::executef("openItemComposeGui","0");
					}
					else
					{
						Con::executef("openItemComposeGui","1");
						U8 stringCount = pPlayer->mItemComposeList.setComposeInfo(szStr.c_str());

						char szInfo[512] = {0,};
						ItemCompose::stRecord* pInfo = pPlayer->mItemComposeList.getComposeInfo();
						if(!pInfo)
							return;

						for(U8 i=0; i<stringCount; i++)
						{
							ItemBaseData* pItemData = g_ItemRepository->getItemData(pInfo[i].iItemId);
							if(!pItemData)
								continue;

							char szTemp[64] = {0,};
							dSprintf(szTemp,sizeof(szTemp),"<l i='%d' t='itemid'/><t h = '0'>�� %d</t><b/>",pItemData->getItemID(),pInfo[i].iQuantity);
							dStrcat(szInfo,sizeof(szInfo),szTemp);
						}

						Con::executef("updateItemComposeInfo",
							Con::getIntArg(pPlayer->mItemComposeList.getItemIdTag()),
							szInfo);
					}
				}
				break;
			case ItemCompose::ITEMCOMPOSE_END:
				{
					pPlayer->mItemComposeList.finishItemCompose(pPlayer);
				}
				break;
			}
		}
		break;
	case INFO_SET_TARGET_TOCLIENT:
		{
			CHECK_MSGCOUNT(INFO_SET_TARGET_TOCLIENT,2,0,0);
			GameObject* pObj = GetIntArg(0) ? static_cast<GameObject*>(con->resolveGhost(GetIntArg(1))) : NULL;
			pPlayer->SetTarget(pObj, pPlayer->IsAutoCastEnable(), GameObject::SetTargetToClient);
		}
		break;
	case INFO_FAME:
		{
			CHECK_MSGCOUNT(INFO_SET_TARGET_TOCLIENT,2,1,0);
			pPlayer->setFame(GetIntArg(0), GetIntArg(1), GetInt32Arg(0));
		}
		break;
	case INFO_FLY:
		{
			CHECK_MSGCOUNT(INFO_SET_TARGET_TOCLIENT,0,3,0);
			pPlayer->setFlyPath(GetInt32Arg(0), GetInt32Arg(1), GetInt32Arg(2));
		}
		break;
	case FRIEND_REQUEST:
		{
			int nDestPlayerId = GetInt32Arg( 0 );

			
		}
		break;
	case INFO_SUPERMARKET:
		{
			Con::executef("clearAllBasket");
			Con::executef("CloseShoppingBasketWnd");
			pPlayer->mShopBaksetList.clear();
		}
		break;
	case INFO_TALENT:
		{
			CHECK_MSGCOUNT(INFO_TALENT,1,1,0);
			if(GetIntArg(0))
				pPlayer->mTalentTable.addTalent(GetInt32Arg(0));
			else
				pPlayer->mTalentTable.removeTalent(GetInt32Arg(0));
		}
		break;
	case INFO_SHORTCUT_EXCHANGE:
		{
			if(GetInt32Arg(0) == SHORTCUTTYPE_EQUIP || GetInt32Arg(0) == SHORTCUTTYPE_INVENTORY)
				Con::executef("showAllEquipQuality");
		}
		break;
	case INFO_HELPDIRECT:   //����ָ��
		{
			Con::executef("ShowHelpDirectByIndex",Con::getIntArg(GetIntArg(0)));
		}
		break;
	default:
		Con::errorf("δ֪����Ϸ�¼�%d",m_InfoType);
		break;
	}
#endif
}