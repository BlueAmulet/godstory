//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _PLAYERBANK_H_
#define _PLAYERBANK_H_
class PlayerBank
{
public:
	enum
	{
		Bank_Open,
		Bank_Close,
		Bank_CheckPassWord,
		Bank_SetPassWord,
		Bank_Lock,
		Bank_SaveMoney,
		Bank_GetMoney,
		Bank_CleanUp,
		Bank_BuySpace,
		Bank_ItemMoveIn,  //ÓÒ¼üµã»÷ÎïÆ·À¸
		Bank_ItemMoveOut, //ÓÒ¼üµã»÷²Ö¿â
	};
	PlayerBank();
	~PlayerBank();
	void openBank(Player* player, U32 openType = 0);
	void closeBank(Player* player);
	bool getLocked(Player* player);
	void setLocked(Player* player, bool locked);
	bool canBuySpace(Player* player);
	bool canSaveMoney(Player* player, U32 money);
	bool canTakeMoney(Player* player, U32 money);
	U32	 NeedMoney(Player* player);
	
#ifdef NTJ_SERVER
	void savingMoney(Player* player, U32 money);
	void takeMoney(Player* player, U32 money);
	enWarnMessage addItemToBank(Player* player, ItemShortcut* pItem, U32 num);
	void buySpace(Player* player);
	bool checkPassWord(Player* player, StringTableEntry word);
	void setPassWord(Player* player, StringTableEntry word);
#endif	
};

extern PlayerBank* g_PlayerBank;
#endif