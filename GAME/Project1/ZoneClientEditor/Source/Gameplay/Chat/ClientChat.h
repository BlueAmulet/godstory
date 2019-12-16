#ifndef CLIENT_CHAT_H
#define CLIENT_CHAT_H

#include "platform/types.h"
#include "core/tVector.h"

class CClientChatManager
{
public:
	enum  DialogStates
	{
      STATES_NONE = 0,
	  STATES_NORMAL,
	  STATES_MIN,
	};

    struct chatDialog  
    {
		int playerId;
        U8 States;

		chatDialog()
		{
			playerId = 0;
			States = 0;
		}
    };
	void createChatDialog( int destPlayerId,U8 states=STATES_NORMAL);
	void openChatDialog( int destPlayerId );
	void closeChatDialog( int destPlayerId );
	void MinChatDialog(int destPlayerId);
    S32  IsChatingIndex(int destPlayerId);

	void onNewMsg( const char* pText, int destPlayerId,const char* pName );
	Vector<chatDialog> ChatPlayerMap;
};

extern CClientChatManager g_ClientChatManager;

#define CHAR_MGR (&g_ClientChatManager)

#endif