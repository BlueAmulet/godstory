#include "T3D/gameConnection.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "terrain/terrData.h"
#include "NetWork/CommonClient.h"
#include "UI/dGuiObjectView.h"
#include "Gameplay/GameObjects/GameObject.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/ClientPlayerManager.h"

#include "GamePlay/Team/ClientTeam.h"
#include "T3D/fx/cameraFXMgr.h"
#include "sceneGraph/decalManager.h"

ConsoleFunction( SptCopyMap_Open, void, 2, 2, "" )
{
	UserPacketProcess *process = g_ClientGameplayState->GetPacketProcess();
	if( process )
		// [LivenHotch]: the 1st argrument is copy map type id 
		process->SendSimplePacket( g_ClientGameplayState->getSelectedPlayerId(), CLIENT_COPYMAP_OpenRequest, atoi( argv[1] ) ); 
}

ConsoleFunction( SptCopyMap_Close, void, 2, 2, "" )
{
	UserPacketProcess *process = g_ClientGameplayState->GetPacketProcess();
	if( process )
		process->SendSimplePacket( g_ClientGameplayState->getSelectedPlayerId(), CLIENT_COPYMAP_CloseRequest, atoi( argv[1] ) );
}

ConsoleFunction( SptCopyMap_Enter, void, 2, 2, "" )
{
	UserPacketProcess *process = g_ClientGameplayState->GetPacketProcess();
	if( process )
		process->SendSimplePacket( g_ClientGameplayState->getSelectedPlayerId(), CLIENT_COPYMAP_EnterRequest, atoi( argv[1] ) );
}

ConsoleFunction( SptCopyMap_Leave, void, 1, 1, "" )
{
	UserPacketProcess *process = g_ClientGameplayState->GetPacketProcess();
	if( process )
		process->SendSimplePacket( g_ClientGameplayState->getSelectedPlayerId(), CLIENT_COPYMAP_LeaveRequest );
}