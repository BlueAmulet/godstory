//-----------------------------------------------------------------------------
// PowerEngine
//
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PATHMANAGER_H_
#define _PATHMANAGER_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif
#ifndef _MPOINT_H_
#include "math/mPoint.h"
#endif
#ifndef _MQUAT_H_
#include "math/mQuat.h"
#endif
#ifndef _SCENEOBJECT_H_
#include "sceneGraph/sceneObject.h"
#endif

#ifndef _SIMPATH_H_
#include "sceneGraph/simPath.h"
#endif

#include <hash_map>

class NetConnection;
class BitStream;

class PathManager
{
   friend class PathManagerEvent;

  public:
   struct PathEntry {
	  U32			  mPathID;
      U32             totalTime;
	  bool            isLooping;

      Vector<Point3F> positions;
      Vector<QuatF>   rotations;
      Vector<U32>     smoothingType;
	  Vector<U32>     msToNext;        //表示停留时间或者移动时间

      PathEntry() {
         VECTOR_SET_ASSOCIATION(positions);
         VECTOR_SET_ASSOCIATION(rotations);
         VECTOR_SET_ASSOCIATION(smoothingType);
         VECTOR_SET_ASSOCIATION(msToNext);
      }

	 /* PathEntry& operator = (PathEntry &entry)
	  {
		  this->mPathID = entry.mPathID;
		  this->totalTime = entry.totalTime;
		  this->positions = entry.positions;
	  }
	  */
   };

  private:
   typedef stdext::hash_map<U32, PathEntry*> PathMap;

   PathMap mPaths;

  public:
   enum PathType {
      BackAndForth,
      Looping
   };

  public:
   PathManager(const bool isServer);
   ~PathManager();

   static void init();
   static void destroy();
   void clearPaths();

   //-------------------------------------- Path querying
  public:
   bool isValidPath(const U32 id) const;
   bool getPathPosition(const U32 id, const F64 msPosition, Point3F& rPosition, QuatF &rotation);
   U32  getPathTotalTime(const U32 id);
   U32  getPathNumWaypoints(const U32 id);          //得到ID路径的路点数
   U32  getWaypointTime(const U32 id, const U32 wayPoint);
  
   U32 getPathTimeBits(const U32 id);
   U32 getPathWaypointBits(const U32 id);
	
   //
  bool GetPathPostionBySeq(U32 id,U32& nSeqIndex, Point3F& rPosition, U32 &msToNext);
  const PathEntry* getPathEntry(const U32 id);

   //-------------------------------------- Path Registration/Transmission/Management
  public:
   // Called after mission load to clear out the paths on the client, and to transmit
   //  the information for the current mission's paths.
   void transmitPaths(NetConnection*);
   void transmitPath(U32);

   U32  allocatePathId(Path *path);
   void updatePath(const U32 id, const bool isLooping, const Vector<Point3F>&, const Vector<QuatF>&, const Vector<U32> &, const Vector<U32>&);

   //-------------------------------------- State dumping/reading
  public:
   bool dumpState(BitStream*);
   bool readState(BitStream*);

  private:
   bool mIsServer;
   bool mPathsSent;
};

struct PathNode {
   Point3F  position;
   QuatF    rotation;
   U32      smoothingType;
   U32      msToNext;
};

extern PathManager* gClientPathManager;
extern PathManager* gServerPathManager;

//--------------------------------------------------------------------------
inline bool PathManager::isValidPath(const U32 id) const
{
	PathMap::const_iterator it = mPaths.find(id);
	
	return (it != mPaths.end() && it->second->positions.size() > 0);
}

#endif // _H_PATHMANAGER
