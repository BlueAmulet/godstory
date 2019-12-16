#ifndef CLIENT_TEAM_H
#define CLIENT_TEAM_H

#include <list>
#include <string>
#include <hash_set>
#include "Common/TeamBase.h"

struct stClientTeamInvation
{
	int mTime;
	int mPlayerId;
	int mType;

	stClientTeamInvation()
	{
		memset( this, 0, sizeof( stClientTeamInvation ) );
	}
	
	void operator = ( const stClientTeamInvation& invation )
	{
		mTime = invation.mTime;
		mPlayerId = invation.mPlayerId;
		mType = invation.mType;
	}
};

class CClientTeam
{
public:
	static CClientTeam* GetInstance();

	void OnCreate( std::list<int> playerList );
	void OnAddPlayer( int nPlayerId );
	void OnRemovePlayer( int nPlayerId );
	void OnDisband();
	bool IsTeammate( int nPlayerId );

	std::list<int>* GetPlayerList();

	int GetCaption();
	void SetCaption( int nPlayerId );

	int GetTeamId();
	void SetTeamId( int nTeamId );

	int GetPlayerByIndex( int nIndex, int isMyselfIncluded );
	bool HasTeam();

	bool AddInvation( int nPlayerId, int nType );
	void RemoveInvation( int nPlayerId );
	void RefuseInvation( int index, int type );

	void ProcessWork();

	std::string m_strName;

	stTeamMapMark mapMarks[MAX_TEAM_MAP_MARK_COUNT];
	stTeamTargetMark targetMarks[MAX_TEAM_TARGET_COUNT];

//*private:*/
	CClientTeam();
	
	std::list<int>::iterator FindPlayer( int nPlayerId );
	int GetInvationPlayerByIndex(int index, int type);
	void AcceptInvationPlayerByIndex( int index, int type );
	void RefuseInvationPlayerByIndex( int index, int type );
	void EnableFollow( bool enabled );
	void OnEnableFollow( bool enabled );
	int GetInvationIndexById( int playerId, int type );
	std::list<int> m_playerList;

	int m_nCaption;
	int m_nTeamId;

	char m_cU;
	char m_cV;

	std::list<stClientTeamInvation*> mInvationSet;
	bool mFollowEnable;
	bool mLocalFollowEnable;
	enum
	{
		TeamState = 1,              // 组队 
		TeamCaptainState,           // 队长
		TeammateState               // 队友
	};
	
};

#define CLIENT_TEAM CClientTeam::GetInstance()

#endif

