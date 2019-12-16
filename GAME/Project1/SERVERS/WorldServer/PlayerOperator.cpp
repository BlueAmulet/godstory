#include <time.h>
#include <assert.h>
#include <sstream>
#include "DBLib/dbLib.h"
#include "WinTcp/dtServerSocket.h"
#include "Common/DumpHelper.h"
#include "zlib/zlib.h"
#include "base/SafeString.h"
#include "Common/MemGuard.h"
#include "PlayerMgr.h"
#include "ChatMgr.h"
#include "TeamManager.h"
#include "WorldServer.h"
#include "ExportScript.h"
#include "Common/WorkQueue.h"
#include "topmanager.h"
#include "CopymapManager.h"
#include "PlayerOperator.h"

stPlayerData* CPlayerOperator::loadPlayer( int playerId )
{
	stPlayerData* pPlayerData;
	MEMPOOL->GetInstance()->AllocObj<stPlayerData>( pPlayerData );

	CDBOperator dbHandle(DATABASE);
	try
	{
		dbHandle->SQL("SELECT PlayerId, AccountId, PlayerName, MainData FROM PlayerInfo WHERE PlayerID = %d",playerId);
		if( dbHandle->More() )
		{
			pPlayerData->BaseData.PlayerId = dbHandle->GetInt();
			pPlayerData->BaseData.AccountId = dbHandle->GetInt();

			strcpy_s( pPlayerData->BaseData.PlayerName, 32, dbHandle->GetString() );
		}
		else
		{
			MEMPOOL->GetInstance()->FreeObj<stPlayerData>( pPlayerData );
			return NULL;
		}

		dbHandle->SQL("SELECT MainData FROM PlayerInfo WHERE PlayerID = %d",playerId);
		if(dbHandle->Eval())
		{
			int dataSize             = sizeof(stPlayerMain);
			pPlayerData->m_pMainData = (stPlayerMain*)MEMPOOL->Alloc(dataSize);
			char *pDataInDB          = (char *)MEMPOOL->Alloc(sizeof(stPlayerMain));
			int dbDataSize           = sizeof(stPlayerMain);



			dbHandle->GetBlob(dbDataSize,(PBYTE)pDataInDB);

			int Error = uncompress((Bytef*)pPlayerData->m_pMainData,(uLongf*)&dataSize,(const Bytef*)pDataInDB,dbDataSize);

			if(!Error)
			{
				Error = NONE_ERROR;
			}
			else
			{
				g_Log.WriteError( DumpHelper::DumpStack() );
				g_Log.WriteError( "玩家离线数据解压缩错误， MainData" );
				Error = COMPRESS_ERROR;
				MEMPOOL->GetInstance()->FreeObj<stPlayerData>( pPlayerData );

				return NULL;
			}

			MEMPOOL->Free((MemPoolEntry)pDataInDB);
		}


		dbHandle->SQL("SELECT DispData FROM PlayerInfo WHERE PlayerID = %d",playerId);

		if(dbHandle->Eval())
		{
			int dataSize    = sizeof(stPlayerDisp);
			char *pDataInDB = (char *)MEMPOOL->Alloc(sizeof(stPlayerDisp));
			int dbDataSize  = sizeof(stPlayerDisp);
			dbHandle->GetBlob(dbDataSize,(PBYTE)pDataInDB);
			int Error = uncompress((Bytef*)&pPlayerData->DispData,(uLongf*)&dataSize,(const Bytef*)pDataInDB,dbDataSize);

			if(!Error)
			{
				Error = NONE_ERROR;
			}
			else
			{
				g_Log.WriteError( DumpHelper::DumpStack() );
				g_Log.WriteError( "玩家离线数据解压缩错误， DispData" );
				Error = COMPRESS_ERROR;
				MEMPOOL->GetInstance()->FreeObj<stPlayerData>( pPlayerData );

				return NULL;
			}

			MEMPOOL->Free((MemPoolEntry)pDataInDB);
		}
	}
	DB_CATCH_LOG(LoadPlayerDataById);

	return pPlayerData;
}

stPlayerData* CPlayerOperator::loadPlayer( std::string playerName )
{
	stPlayerData* pPlayerData;
	MEMPOOL->GetInstance()->AllocObj<stPlayerData>( pPlayerData );

	CDBOperator dbHandle(DATABASE);
	try
	{
		dbHandle->SQL("SELECT PlayerId, AccountId, PlayerName, MainData FROM PlayerInfo WHERE PlayerName = '%s'",playerName.c_str());
		if( dbHandle->More() )
		{
			pPlayerData->BaseData.PlayerId = dbHandle->GetInt();
			pPlayerData->BaseData.AccountId = dbHandle->GetInt();

			strcpy_s( pPlayerData->BaseData.PlayerName, 32, dbHandle->GetString() );
		}
		else
		{
			MEMPOOL->GetInstance()->FreeObj<stPlayerData>( pPlayerData );
			return NULL;
		}

		dbHandle->SQL("SELECT MainData FROM PlayerInfo WHERE PlayerName = '%s'",playerName.c_str());
		if(dbHandle->Eval())
		{
			int dataSize             = sizeof(stPlayerMain);
			pPlayerData->m_pMainData = (stPlayerMain*)MEMPOOL->Alloc(dataSize);
			char *pDataInDB          = (char *)MEMPOOL->Alloc(sizeof(stPlayerMain));
			int dbDataSize           = sizeof(stPlayerMain);

			dbHandle->GetBlob(dbDataSize,(PBYTE)pDataInDB);

			int Error = uncompress((Bytef*)pPlayerData->m_pMainData,(uLongf*)&dataSize,(const Bytef*)pDataInDB,dbDataSize);

			if(!Error)
			{
				Error = NONE_ERROR;
			}
			else
			{
				g_Log.WriteError( DumpHelper::DumpStack() );
				g_Log.WriteError( "玩家离线数据解压缩错误， MainData" );

				Error = COMPRESS_ERROR;
				MEMPOOL->GetInstance()->FreeObj<stPlayerData>( pPlayerData );

				return NULL;
			}

			MEMPOOL->Free((MemPoolEntry)pDataInDB);
		}


		dbHandle->SQL("SELECT DispData FROM PlayerInfo WHERE PlayerName = '%s'", playerName.c_str());

		if(dbHandle->Eval())
		{
			int dataSize    = sizeof(stPlayerDisp);
			char *pDataInDB = (char *)MEMPOOL->Alloc(sizeof(stPlayerDisp));
			int dbDataSize  = sizeof(stPlayerDisp);
			dbHandle->GetBlob(dbDataSize,(PBYTE)pDataInDB);
			int Error = uncompress((Bytef*)&pPlayerData->DispData,(uLongf*)&dataSize,(const Bytef*)pDataInDB,dbDataSize);

			if(!Error)
			{
				Error = NONE_ERROR;
			}
			else
			{
				g_Log.WriteError( DumpHelper::DumpStack() );
				g_Log.WriteError( "玩家离线数据解压缩错误， DispData" );
				Error = COMPRESS_ERROR;
				MEMPOOL->GetInstance()->FreeObj<stPlayerData>( pPlayerData );

				return NULL; 
			}

			MEMPOOL->Free((MemPoolEntry)pDataInDB);
		}
	}
	DB_CATCH_LOG(LoadPlayerDataByName);

	return pPlayerData;
}

int CPlayerOperator::savePlayer( stPlayerData* pPlayerData )
{
	DO_LOCK( Lock::Player );

	CDBOperator dbHandle(DATABASE);

	int MaxSize = getMax(sizeof(stPlayerDisp),sizeof(stPlayerMain)) + 128;

	CMemGuard guard(MaxSize);
	char *Buffer = guard.get();

	int Size = MaxSize;
	int error = compress((Bytef *)Buffer,(uLongf*)&Size,(const Bytef*)&pPlayerData->DispData,sizeof(stPlayerDisp));
	if(error){
		g_Log.WriteError( DumpHelper::DumpStack() );
		return COMPRESS_ERROR;
	}

	try
	{
		dbHandle->SQL("SELECT DispData FROM PlayerInfo WHERE PlayerId=%d",pPlayerData->BaseData.PlayerId);
		if(dbHandle->Eval())
		{
			dbHandle->SetBlob("PlayerInfo","DispData",(PBYTE)Buffer,Size);
		}

		Size = MaxSize;
		error = compress((Bytef *)Buffer,(uLongf*)&Size,(const Bytef*)(pPlayerData->m_pMainData),sizeof(stPlayerMain));
		if(error){
			g_Log.WriteError( "玩家数据压缩错误！" );
			return COMPRESS_ERROR;
		}

		dbHandle->SQL("SELECT MainData FROM PlayerInfo WHERE PlayerId=%d",pPlayerData->BaseData.PlayerId);
		if(dbHandle->Eval())
		{
			dbHandle->SetBlob("PlayerInfo","MainData",(PBYTE)Buffer,Size);
		}

	}
	DB_CATCH_LOG(SavePlayerData);

	return NONE_ERROR;
}

int CPlayerOperator::savePlayer( stPlayerStruct* pPlayerData )
{
	DO_LOCK( Lock::Player );

	bool bFound = false;

	CDBOperator dbHandle(DATABASE);

	int MaxSize = getMax(sizeof(stPlayerDisp),sizeof(stPlayerMain)) + 128;

	CMemGuard guard(MaxSize);
	char *Buffer = guard.get();

	int Size = MaxSize;
	int error = compress((Bytef *)Buffer,(uLongf*)&Size,(const Bytef*)&pPlayerData->DispData,sizeof(stPlayerDisp));
	if(error){
		return COMPRESS_ERROR;
	}

	try
	{
		dbHandle->SQL("SELECT DispData FROM PlayerInfo WHERE PlayerId=%d",pPlayerData->BaseData.PlayerId);
		if(dbHandle->Eval())
		{
			dbHandle->SetBlob("PlayerInfo","DispData",(PBYTE)Buffer,Size);
		}

		Size = MaxSize;
		error = compress((Bytef *)Buffer,(uLongf*)&Size,(const Bytef*)&(pPlayerData->MainData),sizeof(stPlayerMain));
		if(error){
			return COMPRESS_ERROR;
		}

		dbHandle->SQL("SELECT MainData FROM PlayerInfo WHERE PlayerId=%d",pPlayerData->BaseData.PlayerId);
		if(dbHandle->Eval())
		{
			dbHandle->SetBlob("PlayerInfo","MainData",(PBYTE)Buffer,Size);
		}

	}
	DB_CATCH_LOG(SavePlayerData);

	return NONE_ERROR;
}