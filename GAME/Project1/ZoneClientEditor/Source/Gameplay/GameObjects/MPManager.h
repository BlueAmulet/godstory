//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _MPMANAGER_H_
#define _MPMANAGER_H_

#include "platform/types.h"

#define  MAXEXP_POOL    0x4C4B400  //���Ŀǰ8000��
struct stMPTable
{
	StringTableEntry     mTitleName;           // �ʸ�����
	U8                   mMaxPrenticeNum;      // ��ͽ����
	U32                  mNeedExploit;         // ����ʦ��
	U16                  mTradeExp;            // ʦ�¿ɶһ�����ֵ
	U8                   mPrizeExp;            // ʦͽ��ӽ���
	U16                  mOtherPrize;          // ��������
};

class Player;
class MPManager
{
public:
	enum enConstants
	{
		MaxMasterLevel = 6,    // ʦ������ʸ�ȼ�
	};

	enum updateType
	{
		MASTERLEVEL    = BIT(0),       // �ȼ�
		MASTEREXPLOIT  = BIT(1),       // ʦ��
		EXPPOOL        = BIT(2),       // ����
		PRENTICEFLAGE  = BIT(3),       // ͽ״̬
		LEAVEMASTER    = BIT(4),       // ��ʦ
		CALLBOARDTEXT  = BIT(5),       // ʦ���������Ϣ
		STUDYLEVEL     = BIT(6),       // ��ʦ�ȼ�
		PRENTICENUM    = BIT(7),       // ͽ������
	};

protected:
	U8                   mMasterLevel;         // ʦ���ʸ�ȼ�
	U8                   mCurrPrenticeNum;     // ��ǰӵ��ͽ������
	U32                  mCurrMasterExp;       // ��ǰʦ��
	U32                  mExpPool;             // ����� 
	bool                 mbPrentice;           // �Ƿ�ͽ״̬
	bool                 mbFarPrentice;        // ��ʦ���
	U16                  mStudyLevel;          // ��ʦ�ȼ�
	StringTableEntry     mCallboardString;     // ʦ������  
	static stMPTable     MPManagerData[];

public:
	MPManager();

	inline   U8                getMasterLevel()               { return mMasterLevel;}
	inline   void              setMasterLevel(U8 level)       { mMasterLevel = level;}
	inline   U8                getCurrPrenticeNum()           { return mCurrPrenticeNum;}
	inline   U32               getCurrMasterExp()             { return mCurrMasterExp;}
	inline   void              setCurrMasterExp(U32 exp)      { mCurrMasterExp = exp;}
	inline   U32               getExpPool()                   { return mExpPool;}
	inline   void              setExpPool(U32 exp)            { mExpPool = exp;}
	inline   bool              getPrenticeState()             { return mbPrentice; }
	inline   bool              getFarPrentice()               { return mbFarPrentice;}
	inline   U16               getStudyLevel()                { return mStudyLevel;}
	inline   StringTableEntry  getMastermTitle();
	inline   StringTableEntry  getCallboardString()           { return mCallboardString;}
	inline   static stMPTable  getData(U8 level)              { return MPManagerData[level];}

	void                       setPrenticeNum(Player* pPlayer,S8 num);                  // ͽ������
	void                       setStudyLevel(Player* pPlayer,U16 level);                // ��¼��ʦ�ȼ�
	void                       setCallboardString(const char* string);                  // ����ʦ���������Ϣ
	void                       setPrenticeState(Player* pPlayer,bool flage);            // ����ͽ�ܱ��
	void                       setFarPrentice(Player* pPlayer,bool flage);              // ���ó�ʦ���
	void                       addMasterLevel(Player* pPlayer,S32 level);               // �����ʸ�              
	void                       addMasterExploit(Player* pPlayer,S32 exp);               // addʦ��
	void                       addExpPool(Player* pPlayer,S32 exp,S32 friendValue=0);   // add����
	void                       tradeExp(Player* pPlayer,S32 expPool);                   // ��ʦ�¶Ի�����������
	enWarnMessage              canMaster(Player* pPlayer);                              // �жϳ�Ϊʦ��

#ifdef NTJ_SERVER
	enWarnMessage              recruitPrentice(Player* pPlayer,Player* destPlayer);   // �ж��ܷ���ͽ
	enWarnMessage              followMaster(Player* pPlayer,Player* destPlayer);      // �ж��ܷ��ʦ
	void                       createRelation(Player* pPlayer,SocialType::Type type); // ����ʦͽ��ϵ 
	void                       destroyRelation(Player* pPlayer,S32 destPlayerId,SocialType::Type type);  // ���ʦͽ��ϵ
	bool                       sendClientRequest(Player* pPlayer,Player* destPlayer,SocialType::Type type);
	void                       initialize(stPlayerStruct *pPlayerData);
	bool                       saveData(stPlayerStruct *pPlayerData);
	bool                       sendInitialData(Player* pPlayer);
	bool                       sendEvent(Player* pPlayer,U16 mask, bool isFirstUpdate = false) ;                      
	void                       calcMasterExp(Player* pPlayer,NpcObject* npc,F32 expDist);             // ��ȡʦ��
	void                       calcAttachPrize(Player* pPlayer);           // ���⽱��
	bool                       autoLeaveMaster(Player* pPlayer);           // ͽ�ܳ�ʦ
	Player*                    getMasterPlayer(Player* pPlayer);           // ��ȡ�Լ���ʦ��
	U32                        getMasterPlayerId(Player* pPlayer);         // ��ȡ�Լ�ʦ����ID
	void                       getPrenticeList(Player* pPlayer,Vector<int>& listId);           // ��ȡ�Լ���ͽ���б�
#endif
#ifdef NTJ_CLIENT
	bool                       sendZoneResonse(Player* pPlayer,Player* destPlayer,SocialType::Type type);
	bool                       sendZoneChangeText(Player* pPlayer,const char* str);
#endif
};

#endif