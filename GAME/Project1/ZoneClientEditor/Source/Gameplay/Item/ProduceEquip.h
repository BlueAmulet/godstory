//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _PRODUCEEQUIP_H_
#define _PRODUCEEQUIP_H_
class Prescription;
class PrescriptionData;
class Player;
class ProduceEquip
{
public:
	enum ProduceState
	{
		PRODUCE_CANCEL,          // 取消
		PRODUCE_START,           // 开始
		PRODUCE_PROCESS,         // 处理
		PRODUCE_FINISHED,        // 完成
	};
public:
	ProduceEquip();
	enWarnMessage canProduce(Player* pPlayer,U32 SerialID);
	bool Produce(Player* pPlayer,U32 SerialId,S32 Count);
	void finishProduce(Player* pPlayer,U32 SerialId,S32 Count = 0);
	void cancelProduce(Player* pPlayer,U32 SerialId,S32 Count);
	bool sendCurrentEvent(Player* pPlayer,U32 SerialId,S32 Count);
#ifdef NTJ_CLIENT
	void refreshUI(Player* pPlayer,PrescriptionData* pData);
#endif
};
extern ProduceEquip g_ProduceEquip;
#endif