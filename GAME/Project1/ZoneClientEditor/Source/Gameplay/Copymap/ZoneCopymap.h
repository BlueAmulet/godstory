#ifndef ZONE_COPYMAP_H
#define ZONE_COPYMAP_H

#include "..\GameObjects\PlayerObject.h"

class CZoneCopymap
{
public:
	CZoneCopymap();
	virtual ~CZoneCopymap();
	
	// 制定副本ID
	void assignId( int copymapInstId );
	// 获得ID
	int getId();

	void addPlayer( Player* pPlayer );
	void removePlayer( Player* pPlayer );

	// 关闭副本
	void close();

	// 开始副本
	void start();

private:
	int nCopymapInstId;
	typedef stdext::hash_map< int, Player* > PlayerMap;
	PlayerMap mPlayerMap;
};

class CZoneCopymapMgr
{
public:
	CZoneCopymapMgr();
	virtual ~CZoneCopymapMgr();

	// 得到一个Copymap
	CZoneCopymap* getCopymap( int nCopymapInstId );
	void closeCopymap( int nCopymapInstId );

	void onPlayerEnter( Player* pPlayer );
	void onPlayerLeave( Player* pPlayer );
private:
	typedef stdext::hash_map< int, CZoneCopymap* > CopymapMap;
	typedef stdext::hash_map< int, int > PlayerCopymapMap;
	CopymapMap mCopymapMap;
	PlayerCopymapMap mPlayerCopymapMap;
};


extern CZoneCopymapMgr g_ZoneCopymapMgr;

#define ZONE_COPYMAP (&g_ZoneCopymapMgr)

#endif 