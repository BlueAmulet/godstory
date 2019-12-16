//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "core/dnet.h"
#include "core/bitStream.h"
#include "core/frameAllocator.h"
#include "math/mPoint.h"
#include "math/mMatrix.h"
#include "math/mathUtils.h"
#include "T3D/gameConnection.h"
#include "T3D/gameBase.h"
#include "T3D/gameProcess.h"
#include "platform/profiler.h"
#include "console/consoleTypes.h"

//----------------------------------------------------------------------------

ClientProcessList gClientProcessList;
ServerProcessList gServerProcessList;
U32 gNetOrderNextId = 0;

ConsoleFunction(dumpProcessList,void,1,1,"dumpProcessList();")
{
	Con::printf("client process list:");
	gClientProcessList.dumpToConsole();
	Con::printf("server process list:");
	gServerProcessList.dumpToConsole();
}

namespace
{
	// local work class
	struct GameBaseListNode
	{
		GameBaseListNode()
		{
			mPrev=this;
			mNext=this;
			mObject=NULL;
		}

		GameBaseListNode * mPrev;
		GameBaseListNode * mNext;
		GameBase * mObject;

		void linkBefore(GameBaseListNode * obj)
		{
			// Link this before obj
			mNext = obj;
			mPrev = obj->mPrev;
			obj->mPrev = this;
			mPrev->mNext = this;
		}
	};
} // namespace

//--------------------------------------------------------------------------
// ClientProcessList
//--------------------------------------------------------------------------

ClientProcessList::ClientProcessList()
{
}

void ClientProcessList::addObject(ProcessObject * pobj) 
{
	GameBase * obj = dynamic_cast<GameBase*>(pobj);
	if (obj==NULL)
		// don't add
		return;

	if (obj->mNetFlags.test(GameBase::NetOrdered))
	{
		obj->plLinkBefore(&mHead);
		mDirty = true;
	}
	else if (obj->mNetFlags.test(GameBase::TickLast))
	{
		obj->mOrderGUID = 0xFFFFFFFF;
		obj->plLinkBefore(&mHead);
		// not dirty
	}
	else
	{
		obj->plLinkAfter(&mHead);
		// not dirty
	}
}

inline GameBase * ClientProcessList::getGameBase(ProcessObject * obj)
{
	return static_cast<GameBase*>(obj);
}

bool ClientProcessList::doBacklogged(SimTime timeDelta)
{
#ifdef POWER_DEBUG   
	static bool backlogged = false;
	static U32 backloggedTime = 0;
#endif

	// See if the control object has pending moves.
	GameConnection* connection = GameConnection::getConnectionToServer();
	if (connection)
	{
		// If the connection to the server is backlogged
		// the simulation is frozen.
		if (connection->mMoveList.isBacklogged())
		{
#ifdef POWER_DEBUG   
			if (!backlogged)
			{
				Con::printf("client is backlogged, time is frozen");
				backlogged=true;
			}

			backloggedTime += timeDelta;
#endif
			return true;
		}
	}

#ifdef POWER_DEBUG   
	if (backlogged)
	{
		Con::printf("client is no longer backlogged, time is unfrozen (%i ms elapsed)",backloggedTime);
		backlogged=false;
		backloggedTime=0;
	}
#endif
	return false;
}

bool ClientProcessList::advanceTime(SimTime timeDelta)
{
	PROFILE_SCOPE(AdvanceClientTime);

	if (doBacklogged(timeDelta))
		return false;

	GameConnection *pConn = GameConnection::getConnectionToServer();
	if(pConn)
	{
		GameBase * gb = pConn->getControlObject();
		if(gb)
			gb->beginAdvanceTime(timeDelta);
	}

	bool ret = Parent::advanceTime(timeDelta);
	ProcessObject* obj = NULL;

	AssertFatal(mLastDelta>=0.0f && mLastDelta<=1.0f,"Doh!  That would be bad.");

	obj = mHead.mProcessLink.next;
	while (obj != &mHead)
	{
		GameBase * gb = getGameBase(obj);
		if (gb->mProcessTick)
			gb->interpolateTick(mLastDelta);

		obj = obj->mProcessLink.next;
	}

	// Inform objects of total elapsed delta so they can advance
	// client side animations.
	F32 dt = F32(timeDelta) / 1000;

	obj = mHead.mProcessLink.next;
	while (obj != &mHead)
	{
		GameBase * gb = getGameBase(obj);
		gb->advanceTime(dt);

		obj = obj->mProcessLink.next;
	}

	return ret;
}

//----------------------------------------------------------------------------
void ClientProcessList::onAdvanceObjects()
{
	GameConnection* connection;
	stdext::hash_set<GameConnection*>::iterator it;
#ifdef NTJ_EDITOR
		connection = GameConnection::getConnectionToServer();
#else
	for( it = GameConnection::gameConnectionSet.begin(); it != GameConnection::gameConnectionSet.end(); it++ )
	{
		connection = *it;
#endif
		if(connection)
		{
			// process any demo blocks that are NOT moves, and exactly one move
			// we advance time in the demo stream by a move inserted on
			// each tick.  So before doing the tick processing we advance
			// the demo stream until a move is ready
			if(connection->isPlayingBack())
			{
				U32 blockType;
				do
				{
					blockType = connection->getNextBlockType();
					bool res = connection->processNextBlock();
					// if there are no more blocks, exit out of this function,
					// as no more client time needs to process right now - we'll
					// get it all on the next advanceClientTime()
					if(!res)
						return;
				}
				while(blockType != GameConnection::BlockTypeMove);
			}

			connection->mMoveList.collectMove();
			advanceObjects();
		}
		else
			advanceObjects();
#ifndef NTJ_EDITOR
	}
#endif
}

void ClientProcessList::onTickObject(ProcessObject * pobj)
{
	SimObjectPtr<GameBase> obj = getGameBase(pobj);

	// Each object is either advanced a single tick, or if it's
	// being controlled by a client, ticked once for each pending move.
	Move* movePtr;
	U32 numMoves;
	GameConnection* con = obj->getControllingClient();
	if (con && con->getControlObject() == obj)
	{
		con->mMoveList.getMoveList(&movePtr, &numMoves);
		if (numMoves)
		{
			// Note: should only have a single move at this point
			AssertFatal(numMoves==1,"ClientProccessList::onTickObject: more than one move in queue");

#ifdef POWER_DEBUG_NET_MOVES
			U32 sum = Move::ChecksumMask & obj->getPacketDataChecksum(obj->getControllingClient());
#endif

			obj->processTick(movePtr);

#pragma message(ENGINE(初步修改了玩家移动及其校验))
			// 修改了校验机制
			if (bool(obj) && obj->getControllingClient())
			{
				U32 newsum = Move::ChecksumMask & obj->getPacketDataChecksum(obj->getControllingClient());

				// Note: 若客户端发现玩家没有移动，则不会像服务器发送move包
				// 这可能会影响服务器对客户端发包速度的正确检测，导致检测变速齿轮用户更加困难
				if (newsum != Move::LastChecksum)
				{
					movePtr->checksum = Move::ChecksumMismatch;
					Move::LastChecksum = newsum;
					con->mMoveList.clearMoves(1);
				}
				else
				{
					con->mMoveList.popBackMove();
					con->mMoveList.clearMoves(0);
					//Con::printf("stop!!");
				}
				/*
				// set checksum if not set or check against stored value if set
				movePtr->checksum = newsum;

				#ifdef POWER_DEBUG_NET_MOVES
				Con::printf("move checksum: %i, (start %i), (move %f %f %f)",
				movePtr->checksum,sum,movePtr->yaw,movePtr->y,movePtr->z);
				#endif
				*/
			}
			//con->mMoveList.clearMoves(1);
		}
	}
	else if (obj->mProcessTick)
		obj->processTick(0);
}

void ClientProcessList::advanceObjects()
{
#ifdef POWER_DEBUG_NET_MOVES
	Con::printf("Advance client time...");
#endif

	Parent::advanceObjects();

#ifdef POWER_DEBUG_NET_MOVES
	Con::printf("---------");
#endif
}

void ClientProcessList::clientCatchup(GameConnection * connection)
{
	SimObjectPtr<GameBase> control = connection->getControlObject();
	if (control)
	{
		Move * movePtr;
		U32 numMoves;
		U32 m = 0;
		connection->mMoveList.getMoveList(&movePtr,&numMoves);

#ifdef POWER_DEBUG_NET_MOVES
		Con::printf("client catching up... (%i)%s", numMoves, mForceHifiReset ? " reset" : "");
#endif

		for (m=0; m<numMoves && control; m++)
			control->processTick(movePtr++);
		connection->mMoveList.clearMoves(m);
	}

#ifdef POWER_DEBUG_NET_MOVES
	Con::printf("---------");
#endif
}

//--------------------------------------------------------------------------
// ServerProcessList
//--------------------------------------------------------------------------

ServerProcessList::ServerProcessList()
{
}

void ServerProcessList::addObject(ProcessObject * pobj) 
{
	GameBase * obj = dynamic_cast<GameBase*>(pobj);
	if (obj==NULL)
		// don't add
		return;
	AssertFatal(obj->isServerObject(),"Adding client object to server list");

	if (obj->mNetFlags.test(GameBase::NetOrdered))
	{
		if ((gNetOrderNextId & 0xFFFF) == 0)
			// don't let it be zero
			gNetOrderNextId++;
		obj->mOrderGUID = (gNetOrderNextId++) & 0xFFFF; // 16 bits should be enough
		obj->plLinkBefore(&mHead);
		mDirty = true;
	}
	else if (obj->mNetFlags.test(GameBase::TickLast))
	{
		obj->mOrderGUID = 0xFFFFFFFF;
		obj->plLinkBefore(&mHead);
		// not dirty
	}
	else
	{
		obj->plLinkAfter(&mHead);
		// not dirty
	}
}

inline GameBase * ServerProcessList::getGameBase(ProcessObject * obj)
{
	return static_cast<GameBase*>(obj);
}

void ServerProcessList::onTickObject(ProcessObject * pobj)
{
	SimObjectPtr<GameBase> obj = getGameBase(pobj);
	AssertFatal(obj->isServerObject(),"Client object on server process list");

	// Each object is either advanced a single tick, or if it's
	// being controlled by a client, ticked once for each pending move.
	GameConnection * con = obj->getControllingClient();

	if (con && con->getControlObject() == obj)
	{
		Move* movePtr;
		U32 m, numMoves;
		con->mMoveList.getMoveList(&movePtr, &numMoves);

#pragma message(ENGINE(初步修改了玩家移动及其校验))
		// IsInNoMove此变量的作用，是因为在判定对人物禁止移动时，虽然需要忽略移动包里多余移动的ProcessTick
		// 但至少也要保证人物有获得一次执行ProcessTick的机会。
		bool IsInNoMove = !(bool)numMoves;		// numMoves可能为0（玩家没有移动时），保证能执行ProcessTick

		for (m=0; m<numMoves && con && con->getControlObject() == obj; m++, movePtr++)
		{
#ifdef POWER_DEBUG_NET_MOVES
			U32 sum = Move::ChecksumMask & obj->getPacketDataChecksum(obj->getControllingClient());
#endif

			// 作一个保护
			if (movePtr->mVelocity.lenSquared() > 500000.0f)
		 {
			 con->SetNoMove();
			 Con::errorf("严重位置错误:%f %f %f", obj->getPosition());
		 }

		 if (!con->m_bNoMove)
			 obj->processTick(movePtr);
		 else
			 IsInNoMove = true;

		 if (con && con->getControlObject() == obj)
		 {
			 /*
			 U32 newsum = Move::ChecksumMask & obj->getPacketDataChecksum(obj->getControllingClient());

			 // check move checksum
			 if (movePtr->checksum != newsum)
			 {
			 #ifdef POWER_DEBUG_NET_MOVES
			 if (!obj->mIsAiControlled)
			 Con::printf("move %i checksum disagree: %i != %i, (start %i), (move %f %f %f)",
			 movePtr->id, movePtr->checksum,newsum,sum,movePtr->yaw,movePtr->y,movePtr->z);
			 #endif

			 movePtr->checksum = Move::ChecksumMismatch;
			 }
			 */
			 const Point3F &pos = obj->getPosition();
			 if((mFabs(movePtr->mPos.x-pos.x) > 0.01f &&
				 mFabs(movePtr->mPos.y-pos.y) > 0.01f &&
				 mFabs(movePtr->mPos.z-pos.z) > 9.01f) || con->m_bNoMove)
			 {
#ifndef NTJ_EDITOR
				 movePtr->checksum = Move::ChecksumMismatch;
#endif
			 }
			 else
			 {
#ifdef POWER_DEBUG_NET_MOVES
				 Con::printf("move %i checksum agree: %i == %i, (start %i), (move %f %f %f)",
					 movePtr->id, movePtr->checksum,newsum,sum,movePtr->yaw,movePtr->y,movePtr->z);
#endif
			 }
		 }
		}
		if(IsInNoMove)
			obj->processTick(0);//有禁止移动，确保能执行一次ProcessTick

		con->mMoveList.clearMoves(m);
	}
	else if (obj->mProcessTick)
		obj->processTick(0);
}

void ServerProcessList::advanceObjects()
{
#ifdef POWER_DEBUG_NET_MOVES
	Con::printf("Advance server time...");
#endif

	Parent::advanceObjects();

	// Credit all the connections with the elapsed ticks.
	SimGroup *g = Sim::getClientGroup();
	for (SimGroup::iterator i = g->begin(); i != g->end(); i++)
		if (GameConnection *t = dynamic_cast<GameConnection *>(*i))
			t->mMoveList.incMoveCredit(1);

#ifdef POWER_DEBUG_NET_MOVES
	Con::printf("---------");
#endif
}



