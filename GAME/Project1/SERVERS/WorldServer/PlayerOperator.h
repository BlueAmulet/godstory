#ifndef PLAYER_OPERATOR_H
#define PLAYER_OPERATOR_H

struct stPlayerData;
struct stPlayerStruct;

#include <string>

#include "AccountHandler.h"

class CPlayerOperator
{
protected:
	stPlayerData* loadPlayer(  int playerId );
	stPlayerData* loadPlayer( std::string playerName );

	int savePlayer( stPlayerData* pPlayerData );
	int savePlayer( stPlayerStruct* pPlayerData );
};

#endif