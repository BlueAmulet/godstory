#ifndef TOP_MANAGER_H
#define TOP_MANAGER_H

#include "AccountHandler.h"
#include <hash_map>
#include "ManagerBase.h"
#include "LockCommon.h"
#include "Event.h"
#include "base\bitStream.h"

// 榜单类型
struct TopType
{
	enum
	{
		Level,		// 等级

		Count,		// 总数
	};
};

// 单项榜单
class CTopInstance
{
public:
	virtual int getValue( PlayerDataRef pPlayerData ) = 0;
};

struct stTopPlayerItem
{
	// 玩家Id
	int playerId;
	// 值
	int value;
	//  名称
	std::string name;
	// 人气
	int honour;
	// 排名升降
	int indexValue;

	stTopPlayerItem()
	{
		memset( this, 0, sizeof( stTopPlayerItem ) );
	}
};

// 排行榜管理器
class CTopManager : ILockable,  public CManagerBase< CTopManager, MSG_TOP_BEGIN, MSG_TOP_END >
{
public:
	CTopManager();
	virtual ~CTopManager();

	/// 注册榜单
	void registerTopInstance( int type, CTopInstance* topInstance );

	// 从数据库中读取所有数据
	void onInit();

	// 新的统计数据
	void onNewDataComming( PlayerDataRef pPlayerData );

	// 统计数据
	void onData( int type, int playerId, int value, std::string name, int honour );

	// 将数据写回到数据库
	void onShutdown();	

	// 将排行榜打包
	void packTopData( int playerId, int type, Base::BitStream& packet, int page );

	// 二分查找
	int binarySearch( int type, int value );

	// 从榜单中移除玩家数据
	void removePlayer( int type, int playerId );

	// 插入新项
	void insertNew( int type, int index, int playerId, int value, std::string name, int honour );

	// 事件处理相关
	void HandleRankRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);

	// 整理统计榜单
	void HandleUpdateRankRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);

	// 鄙视请求
	void HandleSnootyReuqest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);

	// 敬仰请求
	void HandleRevereReuqest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);

	// 统计榜单数据（拷贝）
	void statistic( int type );

private:
	typedef stdext::hash_map< int, CTopInstance* > TopInstanceMap;

	// 最大统计数量
	static const int MaxTopCount = 19;

	struct TopPlayers
	{
		stTopPlayerItem mItems[MaxTopCount];

		int currentCount;

		inline stTopPlayerItem& operator [] ( int index )
		{
			if( index > currentCount - 1 || index < 0 )
			{
				assert( false );
			}

			return mItems[index];
		}

		TopPlayers()
		{
			currentCount = 0;
		}
	};

	struct topPlayerTemp
	{
		//// 玩家ID
		//int playerId;
		// 之前的排名
		int pIdx;
		// 现在的排名
		int nIdx;
		// 名称
		std::string name;
		// 值
		int value;
		// 人气
		int honour;
		// 升降
		int indexValue;

		topPlayerTemp()
		{
			memset( this, 0, sizeof( topPlayerTemp ) );
		}
	};

	typedef stdext::hash_map< int, topPlayerTemp > TempTopPlayersMap;

	typedef stdext::hash_map< int ,TopPlayers > TopPlayersMap;

	TopPlayersMap mTopPlayersMap;
	TopInstanceMap mInstanceMap;

	// 统计的榜单
	TopPlayersMap mCurrentTopPlayersMap;
} ;

class CTopLevelInst : public CTopInstance
{
public:
	int getValue( PlayerDataRef pPlayerData );
};

#endif