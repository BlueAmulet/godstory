#include "ClientChat.h"
#include "Gameplay/ClientGameplayState.h"

CClientChatManager g_ClientChatManager;

S32 CClientChatManager::IsChatingIndex(int destPlayerId)
{
	for(int i =0; i<ChatPlayerMap.size(); i++)
	{
		if (ChatPlayerMap[i].playerId == destPlayerId)
			return i;
	}
	return -1;
}

void CClientChatManager::createChatDialog( int destPlayerId,U8 states)
{
  if (IsChatingIndex(destPlayerId) == -1 && ChatPlayerMap.size() <=3)
  {
    chatDialog Chat;
	Chat.playerId = destPlayerId;
	Chat.States = states;
	ChatPlayerMap.push_back(Chat);

	Con::evaluatef("BuildChatDialogUI(%d,%d,%d);",destPlayerId,states,ChatPlayerMap.size());
  }
}

void CClientChatManager::openChatDialog( int destPlayerId )
{
  int index = IsChatingIndex(destPlayerId);
  if (index != -1 )
  {
	  if (ChatPlayerMap[index].States == CClientChatManager::STATES_MIN)
	  {
		  ChatPlayerMap[index].States = CClientChatManager::STATES_NORMAL;

		  Con::evaluatef("MaxChatDialogUI(%d);",destPlayerId);
	  }
  }
}

void CClientChatManager::closeChatDialog( int destPlayerId )
{
	int index = IsChatingIndex(destPlayerId);
	if (index != -1 )
	{        
		ChatPlayerMap.erase(index);

		Con::evaluatef("CloseChatDialogUI(%d);",destPlayerId);
	}
}

void CClientChatManager::MinChatDialog(int destPlayerId)
{
	int index = IsChatingIndex(destPlayerId);
	if (index != -1 )
	{
		if (ChatPlayerMap[index].States == CClientChatManager::STATES_NORMAL)
		{
			ChatPlayerMap[index].States = CClientChatManager::STATES_MIN;

			Con::evaluatef("MinChatDialogUI(%d);",destPlayerId);
		}
	}
}


void CClientChatManager::onNewMsg( const char* pText, int destPlayerId, const char* pName)
{
	int index = IsChatingIndex(destPlayerId);
    
	if (index == -1)
	{
		createChatDialog(destPlayerId,CClientChatManager::STATES_MIN);
	}
	char textBuf[512];
	strcpy_s(textBuf,sizeof(textBuf),pText);       
	Con::evaluatef("AddChatDialogMsg(%d,\"%s\",\"%s\");",destPlayerId,textBuf,pName);
}


//创建一个对话框
ConsoleFunction(SptCreateChatDialog,void, 2, 2, "SptCreateDialog(%playerId)")
{
   g_ClientChatManager.createChatDialog(atoi(argv[1]));
}

//打开对话框
ConsoleFunction(SptOpenChatDialog,void, 2, 2, "SptOpenChatDialog(%playerId)")
{
  g_ClientChatManager.openChatDialog(atoi(argv[1]));
}

//最小化对话框
ConsoleFunction(SptMinChatDialog,void, 2, 2,"SptMinChatDialog(%playerId)")
{
  g_ClientChatManager.MinChatDialog(atoi(argv[1]));
}

// 关闭对话框
ConsoleFunction(SptClostChatDialog,void, 2, 2,"SptCloseChatDialog(%playerId)")
{
	g_ClientChatManager.closeChatDialog(atoi(argv[1]));
}