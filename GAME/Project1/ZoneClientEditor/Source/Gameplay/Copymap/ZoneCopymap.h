#ifndef ZONE_COPYMAP_H
#define ZONE_COPYMAP_H

#include "..\GameObjects\PlayerObject.h"

class CZoneCopymap
{
public:
	CZoneCopymap();
	virtual ~CZoneCopymap();
	
	// �ƶ�����ID
	void assignId( int copymapInstId );
	// ���ID
	int getId();

	void addPlayer( Player* pPlayer );
	void removePlayer( Player* pPlayer );

	// �رո���
	void close();

	// ��ʼ����
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

	// �õ�һ��Copymap
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