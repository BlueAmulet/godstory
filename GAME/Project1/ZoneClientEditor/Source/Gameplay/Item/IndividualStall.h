//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _INDIVIDUAL_STALL_H_
#define _INDIVIDUAL_STALL_H_ 
class Player;

class IndividualStall
{
public:
	IndividualStall();
	~IndividualStall();
	bool canOpenStall(Player* pPlayer, U32 type);
	void openStall(Player* pPlayer, U32 type, U32 playerID);
	void closeStall(Player* pPlayer, U32 type);
	bool canChangeBag(Player* pPlayer, U32 index);
	void setType(U32 type);
	U32 getType();

	void addToLookUpList(U32 itemID);
	void clearLookUpList();

	void showLookUpItem(S32 page = 1);

#ifdef NTJ_SERVER
	enWarnMessage buyStallItemSucc(Player* pTarget, Player* pSelf, U32 money, U32 index, U32 num, U32 flag);
	void	openStallerStall(Player* pPlayer, GameConnection* con, U32 playerID);
	void	upItemToStall(Player* pPlayer, GameConnection* con, S32 index, U32 type);
	void	downItemFromStall(Player* pPlayer, GameConnection* con, S32 index, U32 type);
	void	setStallItemPrice(Player* pPlayer, GameConnection* con, S32 index, U32 money, U32 type);
	void    downAllStallItem(Player* pPlayer, GameConnection* con);
	void    stallTrade(Player* pPlayer, GameConnection* con, S32 index, U32 num, U32 playerID, U32 flag);
#endif
private:
	U32 mType;
	Vector<U32> mLookUpList;
};

extern IndividualStall* g_Stall;
#endif