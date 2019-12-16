#ifndef TOP_MANAGER_H
#define TOP_MANAGER_H

#include "AccountHandler.h"
#include <hash_map>
#include "ManagerBase.h"
#include "LockCommon.h"
#include "Event.h"
#include "base\bitStream.h"

// ������
struct TopType
{
	enum
	{
		Level,		// �ȼ�

		Count,		// ����
	};
};

// �����
class CTopInstance
{
public:
	virtual int getValue( PlayerDataRef pPlayerData ) = 0;
};

struct stTopPlayerItem
{
	// ���Id
	int playerId;
	// ֵ
	int value;
	//  ����
	std::string name;
	// ����
	int honour;
	// ��������
	int indexValue;

	stTopPlayerItem()
	{
		memset( this, 0, sizeof( stTopPlayerItem ) );
	}
};

// ���а������
class CTopManager : ILockable,  public CManagerBase< CTopManager, MSG_TOP_BEGIN, MSG_TOP_END >
{
public:
	CTopManager();
	virtual ~CTopManager();

	/// ע���
	void registerTopInstance( int type, CTopInstance* topInstance );

	// �����ݿ��ж�ȡ��������
	void onInit();

	// �µ�ͳ������
	void onNewDataComming( PlayerDataRef pPlayerData );

	// ͳ������
	void onData( int type, int playerId, int value, std::string name, int honour );

	// ������д�ص����ݿ�
	void onShutdown();	

	// �����а���
	void packTopData( int playerId, int type, Base::BitStream& packet, int page );

	// ���ֲ���
	int binarySearch( int type, int value );

	// �Ӱ����Ƴ��������
	void removePlayer( int type, int playerId );

	// ��������
	void insertNew( int type, int index, int playerId, int value, std::string name, int honour );

	// �¼��������
	void HandleRankRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);

	// ����ͳ�ư�
	void HandleUpdateRankRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);

	// ��������
	void HandleSnootyReuqest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);

	// ��������
	void HandleRevereReuqest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);

	// ͳ�ư����ݣ�������
	void statistic( int type );

private:
	typedef stdext::hash_map< int, CTopInstance* > TopInstanceMap;

	// ���ͳ������
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
		//// ���ID
		//int playerId;
		// ֮ǰ������
		int pIdx;
		// ���ڵ�����
		int nIdx;
		// ����
		std::string name;
		// ֵ
		int value;
		// ����
		int honour;
		// ����
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

	// ͳ�Ƶİ�
	TopPlayersMap mCurrentTopPlayersMap;
} ;

class CTopLevelInst : public CTopInstance
{
public:
	int getValue( PlayerDataRef pPlayerData );
};

#endif