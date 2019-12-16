//-----------------------------------------------------------------------------
// PowerEngine
//
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "sceneGraph/pathManager.h"
#include "sim/netConnection.h"
#include "core/bitStream.h"
#include "sceneGraph/simPath.h"
#include "interior/interiorInstance.h"
#include "math/mathIO.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/sceneGraph.h"
#include "platform/profiler.h"

extern bool gEditingMission;


namespace {

U32 countNumBits(U32 n)
{
   U32 count = 0;
   while (n != 0) {
      n >>= 1;
      count++;
   }

   return count ? count : 1;
}

} // namespace {}



//--------------------------------------------------------------------------
//-------------------------------------- PathManagerEvent
//
class PathManagerEvent : public NetEvent
{
  public:
   bool clearPaths;
   PathManager::PathEntry path;

  public:
   PathManagerEvent() { }

   void pack(NetConnection*, BitStream*);
   void write(NetConnection*, BitStream*);
   void unpack(NetConnection*, BitStream*);
   void process(NetConnection*);

   DECLARE_CONOBJECT(PathManagerEvent);
};

void PathManagerEvent::pack(NetConnection*, BitStream* stream)
{
   // Write out the modified path...
   stream->writeFlag(clearPaths);
   stream->write(path.mPathID);
   stream->write(path.totalTime);
   stream->write(path.positions.size());


   // This is here for safety. You can remove it if you want to try your luck at bigger sizes. -- BJG
   AssertWarn(path.positions.size() < 1500/40, "Warning! Path size is pretty big - may cause packet overrun!");

   // Each one of these is about 8 floats and 2 ints
   // so we'll say it's about 40 bytes in size, which is where the 40 in the above calc comes from.
   for (U32 j = 0; j < path.positions.size(); j++)
   {
      mathWrite(*stream, path.positions[j]);
      mathWrite(*stream, path.rotations[j]);
      stream->write(path.msToNext[j]);
      stream->write(path.smoothingType[j]);
   }
}

void PathManagerEvent::write(NetConnection*nc, BitStream *stream)
{
   pack(nc, stream);
}

void PathManagerEvent::unpack(NetConnection*, BitStream* stream)
{
   // Read in the modified path...

   clearPaths = stream->readFlag();
   stream->read(&path.mPathID);
   stream->read(&path.totalTime);

   U32 numPoints;
   stream->read(&numPoints);
   path.positions.setSize(numPoints);
   path.rotations.setSize(numPoints);
   path.msToNext.setSize(numPoints);
   path.smoothingType.setSize(numPoints);
   for (U32 j = 0; j < path.positions.size(); j++)
   {
      mathRead(*stream, &path.positions[j]);
      mathRead(*stream, &path.rotations[j]);
      stream->read(&path.msToNext[j]);
      stream->read(&path.smoothingType[j]);
   }
}

void PathManagerEvent::process(NetConnection*)
{
	if (clearPaths)
	{
		// Clear out all the client's paths...
		gClientPathManager->clearPaths();
	}

	PathManager::PathMap::const_iterator it = gClientPathManager->mPaths.find(path.mPathID);

	if (it == gClientPathManager->mPaths.end())
	{
		PathManager::PathEntry *pe = new PathManager::PathEntry;
		*pe = path;
		gClientPathManager->mPaths[pe->mPathID] = pe;
	}
	else
	{
		*(gClientPathManager->mPaths[path.mPathID]) = path;
	}

}

IMPLEMENT_CO_NETEVENT_V1(PathManagerEvent);


//--------------------------------------------------------------------------
//-------------------------------------- PathManager Implementation
//
PathManager* gClientPathManager = NULL;
PathManager* gServerPathManager = NULL;

//--------------------------------------------------------------------------
PathManager::PathManager(const bool isServer)
{
//   VECTOR_SET_ASSOCIATION(mPaths);

   mIsServer  = isServer;
}

PathManager::~PathManager()
{
   clearPaths();
}

void PathManager::clearPaths()
{
   PathMap::iterator itb = mPaths.begin();
   while(itb!=mPaths.end())
   {
		delete itb->second;
		itb++;
   }
   mPaths.clear();

#ifdef POWER_DEBUG
   // This gets rid of the memory used by the vector.
   // Prevents it from showing up in memory leak logs.
   //mPaths.compact();
#endif
}

ConsoleFunction(clearServerPaths, void, 1, 1, "")
{
   gServerPathManager->clearPaths();
}

ConsoleFunction(clearClientPaths, void, 1, 1, "")
{
   gClientPathManager->clearPaths();
}

void PathManager::init()
{
   AssertFatal(gClientPathManager == NULL && gServerPathManager == NULL, "Error, already initialized the path manager!");

   gClientPathManager = new PathManager(false);
   gServerPathManager = new PathManager(true);
}

void PathManager::destroy()
{
   AssertFatal(gClientPathManager != NULL && gServerPathManager != NULL, "Error, path manager not initialized!");

   delete gClientPathManager;
   gClientPathManager = NULL;
   delete gServerPathManager;
   gServerPathManager = NULL;
}


//--------------------------------------------------------------------------
U32 PathManager::allocatePathId(Path *path)
{
	PathEntry *entry =  new PathEntry;
	entry->mPathID = path->getPathID();

	mPaths[path->getPathID()] = entry;
	return path->getPathID();
}

void PathManager::updatePath(const U32              id,
							 const bool             isLooping,
                             const Vector<Point3F>& positions,
                             const Vector<QuatF>&   rotations,
                             const Vector<U32>&     times,
                             const Vector<U32>&     smoothingTypes)
{
   //AssertFatal(mIsServer == true, "PathManager::updatePath: Error, must be called on the server side");
   //AssertFatal(mPaths[id] != NULL, "PathManager::updatePath: error, path id not exist");
   AssertFatal(positions.size() == times.size() && positions.size() == smoothingTypes.size(), "Error, times and positions must match!");

   PathEntry& rEntry = *mPaths[id];

   rEntry.isLooping = isLooping;
   rEntry.positions = positions;
   rEntry.rotations = rotations;
   rEntry.msToNext  = times;
   rEntry.smoothingType = smoothingTypes;

   rEntry.totalTime = 0;
   S32 t = rEntry.isLooping ? rEntry.msToNext.size() : ((S32)rEntry.msToNext.size() - 1);
   for (S32 i = 0; i < t; i++)
      rEntry.totalTime += rEntry.msToNext[i];

   transmitPath(id);
}


//--------------------------------------------------------------------------
void PathManager::transmitPaths(NetConnection* nc)
{
   AssertFatal(mIsServer, "Error, cannot call transmitPaths on client path manager!");

#ifdef NTJ_EDITOR
   // Send over paths

   for (PathMap::iterator it=mPaths.begin(); it!=mPaths.end(); ++it)
   {
		PathManagerEvent* event = new PathManagerEvent;
		event->clearPaths       = (it == mPaths.begin());
		event->path				= *(it->second);
		nc->postNetEvent(event);
   }

#endif
}

void PathManager::transmitPath(const U32 id)
{
	return;
   //AssertFatal(mIsServer, "Error, cannot call transmitNewPath on client path manager!");

   // Post to all active clients that have already received their paths...
   //
   SimGroup* pClientGroup = Sim::getClientGroup();
   for (SimGroup::iterator itr = pClientGroup->begin(); itr != pClientGroup->end(); itr++) {
      NetConnection* nc = dynamic_cast<NetConnection*>(*itr);
      if (nc && nc->missionPathsSent())
      {
         // Transmit the updated path...
         PathManagerEvent* event = new PathManagerEvent;
         event->clearPaths       = false;
         event->path             = *(mPaths[id]);
         nc->postNetEvent(event);
      }
   }
}

bool PathManager::getPathPosition(const U32 id,
                                  const F64 msPosition,
                                  Point3F&  rPosition,
                                  QuatF &rotation)
{
   AssertFatal(isValidPath(id), "Error, this is not a valid path!");
   PROFILE_START(PathManGetPos);

   // Ok, query holds our path information...
   F64 ms = msPosition;
   if (ms >= mPaths[id]->totalTime)
   {
      if(!mPaths[id]->isLooping)
	  {
		  rPosition = mPaths[id]->positions.last();
		  rotation = mPaths[id]->rotations.last();
		  PROFILE_END(PathManGetPos);
		  return true;
	  }
      ms = mPaths[id]->totalTime;
   }

   S32 startNode = 0;
   while (ms >= mPaths[id]->msToNext[startNode]) {
      ms -= mPaths[id]->msToNext[startNode];
      startNode++;
   }
   S32 endNode = (startNode + 1) % mPaths[id]->positions.size();

   Point3F& rStart = mPaths[id]->positions[startNode];
   Point3F& rEnd   = mPaths[id]->positions[endNode];

   F64 interp = ms / F32(mPaths[id]->msToNext[startNode]);
   if(mPaths[id]->smoothingType[startNode] == Marker::SmoothingTypeLinear)
   {
      rPosition = (rStart * (1.0 - interp)) + (rEnd * interp);
   }
   else if(mPaths[id]->smoothingType[startNode] == Marker::SmoothingTypeAccelerate)
   {
      interp = mSin(interp * M_PI - (M_PI / 2)) * 0.5 + 0.5;
      rPosition = (rStart * (1.0 - interp)) + (rEnd * interp);
   }
   else if(mPaths[id]->smoothingType[startNode] == Marker::SmoothingTypeSpline)
   {
      S32 preStart = startNode - 1;
      S32 postEnd = endNode + 1;
      if(postEnd >= mPaths[id]->positions.size())
         postEnd = mPaths[id]->isLooping ? 0 : mPaths[id]->positions.size()-1;
      if(preStart < 0)
         preStart = mPaths[id]->isLooping ? mPaths[id]->positions.size()-1 : 0;
      Point3F p0 = mPaths[id]->positions[preStart];
      Point3F p1 = rStart;
      Point3F p2 = rEnd;
      Point3F p3 = mPaths[id]->positions[postEnd];
      rPosition.x = mCatmullrom(interp, p0.x, p1.x, p2.x, p3.x);
      rPosition.y = mCatmullrom(interp, p0.y, p1.y, p2.y, p3.y);
      rPosition.z = mCatmullrom(interp, p0.z, p1.z, p2.z, p3.z);
   }
   rotation.interpolate( mPaths[id]->rotations[startNode], mPaths[id]->rotations[endNode], interp );
   PROFILE_END(PathManGetPos);
   return false;
}

U32 PathManager::getPathTotalTime(const U32 id)
{
   AssertFatal(isValidPath(id), "Error, this is not a valid path!");

   return mPaths[id]->totalTime;
}

U32 PathManager::getPathNumWaypoints(const U32 id)
{
   AssertFatal(isValidPath(id), "Error, this is not a valid path!");

   return mPaths[id]->positions.size();
}

U32 PathManager::getWaypointTime(const U32 id, const U32 wayPoint)
{
   AssertFatal(isValidPath(id), "Error, this is not a valid path!");
   AssertFatal(wayPoint < getPathNumWaypoints(id), "Invalid waypoint!");

   U32 time = 0;
   for (U32 i = 0; i < wayPoint; i++)
      time += mPaths[id]->msToNext[i];

   return time;
}

U32 PathManager::getPathTimeBits(const U32 id)
{
   AssertFatal(isValidPath(id), "Error, this is not a valid path!");

   return countNumBits(mPaths[id]->totalTime);
}

U32 PathManager::getPathWaypointBits(const U32 id)
{
   AssertFatal(isValidPath(id), "Error, this is not a valid path!");

   return countNumBits(mPaths[id]->positions.size());
}


bool PathManager::dumpState(BitStream* stream)
{
   stream->write(mPaths.size());

   for (U32 i = 0; i < mPaths.size(); i++) {
      const PathEntry& rEntry = *mPaths[i];
      stream->write(rEntry.totalTime);

      stream->write(rEntry.positions.size());
      for (U32 j = 0; j < rEntry.positions.size(); j++) {
         mathWrite(*stream, rEntry.positions[j]);
         stream->write(rEntry.msToNext[j]);
      }
   }

   return stream->getStatus() == Stream::Ok;
}

bool PathManager::readState(BitStream* stream)
{
   U32 i;
   for (i = 0; i < mPaths.size(); i++)
      delete mPaths[i];

   U32 numPaths;
   stream->read(&numPaths);
   //mPaths.setSize(numPaths);

   for (i = 0; i < mPaths.size(); i++) {
      mPaths[i] = new PathEntry;
      PathEntry& rEntry = *mPaths[i];

      stream->read(&rEntry.totalTime);

      U32 numPositions;
      stream->read(&numPositions);
      rEntry.positions.setSize(numPositions);
      rEntry.msToNext.setSize(numPositions);
      for (U32 j = 0; j < rEntry.positions.size(); j++) {
         mathRead(*stream, &rEntry.positions[j]);
         stream->read(&rEntry.msToNext[j]);
      }
   }

   return stream->getStatus() == Stream::Ok;
}

bool  PathManager::GetPathPostionBySeq(U32 id,U32& nSeqIndex, Point3F& rPosition, U32 &msToNext)
{
	//
	if(!gServerPathManager->isValidPath(id))
	{
		Con::printf("error PathId !");
		return  false;
	}
	//
	if(nSeqIndex >= gServerPathManager->getPathNumWaypoints(id))
	{
		Con::printf("error nSeqIndex!");
		return false;
	}
	rPosition = mPaths[id]->positions[nSeqIndex];
	msToNext  = mPaths[id]->msToNext[nSeqIndex];
	return true;
}

const PathManager::PathEntry* PathManager::getPathEntry(const U32 id)
{
	PathMap::iterator it = mPaths.find(id);
	if(it != mPaths.end())
		return it->second;
	return NULL;
}


