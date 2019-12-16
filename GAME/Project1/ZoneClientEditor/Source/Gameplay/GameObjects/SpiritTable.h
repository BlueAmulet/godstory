#pragma once

#include "Common/PlayerStruct.h"

class ShortcutObject;
class SkillList_Spirit;
class TalentSkillList;

class SpiritTable
{
public:
	enum SpiritStatus
	{
		Status_NotExist		= 1,
		Status_Close		= 2,
		Status_Open			= 3,
		Status_Active		= 4,
	};
	friend class Player;

	SpiritTable										();
	~SpiritTable									();

	void				SaveData					(stPlayerStruct* playerInfo);
	void				LoadData					(stPlayerStruct* playerInfo);

	void				PackData					(Player*, BitStream* stream, S32 slot = -1, U32 flag = 0xFFFFFFFF);
	void				UnpackData					(Player*, BitStream* stream);

	//元神幻化属性
	void				setSpiritHuanHua			(bool bIsHuanHua)	{ mIsHuanHua = bIsHuanHua; }
	bool				isSpiritHuanHua				()					{ return mIsHuanHua; }
	U32					getHuanHuaId				(S32 nSlot);
	//元神升级
	bool				addExp						(S32 slot, S32 nExp, Player *pPlayer);
	bool				addLevel					(S32 slot, S32 nLevel, Player *pPlayer, S32 nExp = 0);
	bool				addJingJie					(S32 slot, S32 nJingJie, Player *pPlayer);
	
	stSpiritInfo*		getSpiritInfo				(S32 nSlot);
	S32					findEmptySlot				();
	S32					findFirstLockedSlot			();
	void				createSpiritInfo			(Player *pPlayer, S32 nSlot, U32 nSpiritId);
	void				removeSpiritInfo			(S32 nSlot);
	bool				isSpiritOpened				(S32 nSlot);
	bool				isSpiritActive				(S32 nSlot);
	void				setActiveSlot				(S32 nSlot)		{ mActiveSlot = nSlot; }
	S32					getActiveSlot				()				{ return mActiveSlot; }
	void				openSlot					(S32 nSlot)	{ mOpenedSlotFlag |= (BIT(nSlot)); }
	U32					getOpenedSlotFlag			()			{ return mOpenedSlotFlag; }
	S32					getSpiritCount				();

	bool				isJueZhaoOpened				(S32 nSpiritSlot, S32 nSlot);
	bool				isJueZhaoExist				(S32 nSpiritSlot, S32 nSlot);
	void				OpenJueZhao					(S32 nSpiritSlot, S32 nSlot);
	void				AddJueZhao					(Player *pPlayer, S32 nSpiritSlot, S32 nSlot, U32 nSkillId);
	void				RemoveJueZhao				(Player *pPlayer, S32 nSpiritSlot, S32 nSlot);

#ifdef NTJ_SERVER
	void				sendInitialData				(Player *pPlayer);
	void				AddTalentSetToTalentTable	(Player* pPlayer, S32 nSlot);
	void				RemoveTalentSetFromTalentTable(Player* pPlayer, S32 nSlot);
	void				AddTalentToTalentTable		(Player* pPlayer, S32 nSlot, S32 nIndex);
	void				RemoveTalentFromTalentTable	(Player* pPlayer, S32 nSlot, S32 nIndex);
#endif

#ifdef NTJ_CLIENT
	void				initialize					(S32 nActiveSlot, U32 nOpenedSlotFlag);
#endif

	//////////////////////////////// 元神相关属性 ///////////////////////////////////////////////
	S32					getSpiritType				(S32 nSlot);
	bool				isSlotValid					(S32 nSlot);		//必须首先调用isSlotValid,然后再调用isSpiritExist
	bool				isSpiritExist				(S32 nSlot);		//调用下面的函数都必须先调用isSpiritExist()
	StringTableEntry	getSpiritName				(S32 nSlot);
	U32					getTalentSetId				(S32 nSlot);		//天赋树ID
	U16					getTalentExtPts				(S32 nSlot);
	U16					getTalentLeftPts			(S32 nSlot);
	U16					getTalentPts				(S32 nSlot);
	U32					getCleanPtsCount			(S32 nSlot);
	U32					getValueBuffId				(S32 nSlot);
	U32					getCostBuffId				(S32 nSlot);
	U32					getCurrExp					(S32 nSlot);
	U32					getMaxExp					(S32 nSlot);
	S32					getCurrLevel				(S32 nSlot);
	S32					getMaxLevel					(S32 nSlot);
	bool				isTalentExist				(S32 nSlot, S32 nTalentIndex);
	S32					getTalentLevel				(S32 nSlot, S32 nTalentIndex);
	S32					getTalentMaxLevel			(S32 nSlot, S32 nTalentIndex);
	StringTableEntry	getTalentBackImage			(S32 nSlot);

protected:
	bool mIsHuanHua;
	S32 mActiveSlot;
	U32 mOpenedSlotFlag;
	stSpiritInfo mSpiritInfo[SPIRIT_MAXSLOTS];

#ifdef NTJ_CLIENT
public:	
	void				InitAllSpiritSlot			(Player *pPlayer);
	void				ClearAllSpiritSlot			();

	//////////////////// 元神绝招栏相关函数 /////////////////////////////
	ShortcutObject*		getCurrentSpiritSkillSlot	(S32 nSkillSlot);
	ShortcutObject*		getCurrentSpiritSkillSlot_2	(S32 nSkillSlot);
	void				InitAllSpiritSkillSlot		(Player *pPlayer);
	void				clearAllSpiritSkillSlot		();
	void				clearSpiritSkillSlot		(S32 nSpiritSlot);
	void				removeSpiritSkillSlot		(S32 nSpiritSlot, S32 nIndex);
	void				AddSpiritSkillSlot			(Player *pPlayer, S32 nSpiritSlot);

	//////////////////// 元神天赋技能栏相关函数 /////////////////////////////
	void				initAllTalentSkillSlot		(Player *pPlayer);	
	ShortcutObject*		getCurrentTalentSkillSlot	(S32 nTalentSkillSlot);
	void				clearAllTalentSkillSlot		();
	void				clearTalentSkillSlot		(S32 nSpiritSlot);
	void				AddTalentSkillSlot			(Player *pPlayer, S32 nSpiritSlot);
	void				updateTalentSkillSlot		(Player *pPlayer, S32 nSpiritSlot, S32 nTalentIndex);

	SkillList_Spirit	**m_ppSpiritSkillList;
	TalentSkillList		**m_ppTalentSkillList;
#endif
};