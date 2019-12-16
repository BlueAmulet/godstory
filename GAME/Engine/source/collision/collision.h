//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _COLLISION_H_
#define _COLLISION_H_

#ifndef _DATACHUNKER_H_
#include "core/dataChunker.h"
#endif
#ifndef _MPLANE_H_
#include "math/mPlane.h"
#endif

class SceneObject;

//----------------------------------------------------------------------------

struct Collision
{
   SceneObject* object;
   Point3F point;
   VectorF normal;
   U32 material;
   Point2F texCoord;

   // Face and Face dot are currently only set by the extrudedPolyList
   // clipper.  Values are otherwise undefined.
   U32 face;                  // Which face was hit
   F32 faceDot;               // -Dot of face with poly normal
   F32 distance;
   
   Collision() :
      material( 0 )
   {
   }
};

class CollisionList
{
public:
   enum
   {
      MaxCollisions = 64
   };

protected:
   dsize_t mCount;
   Collision mCollision[MaxCollisions];
   F32 mT;
   // MaxHeight is currently only set by the extrudedPolyList
   // clipper.  It represents the maximum vertex z value of
   // the returned collision surfaces.
   F32 mMaxHeight;

public:
   // Constructor
   CollisionList( /* const dsize_t reserveSize = MaxCollisions */ ) :
      mCount( 0 ), mT( 0.0f ), mMaxHeight( 0.0f )
   {

   }

   // Accessors
   int getCount() const { return mCount; }
   F32 getTime() const { return mT; }
   F32 getMaxHeight() const { return mMaxHeight; }

   const Collision &operator[] ( const dsize_t idx ) const
   {
      AssertFatal( idx < mCount, "Out of bounds index." );
      return mCollision[idx];
   }

   Collision &operator[] ( const dsize_t idx )
   {
      AssertFatal( idx < mCount, "Out of bounds index." );
      return mCollision[idx];
   }

   // Increment does NOT reset the collision which it returns. It is the job of
   // the caller to make sure that the entry has data properly assigned to it.
   Collision &increment()
   {
      return mCollision[mCount++];
   }

   void clear()
   {
      mCount = 0;
   }

   void setTime( const F32 t )
   {
      mT = t;
   }

   void setMaxHeight( const F32 height )
   {
      mMaxHeight = height;
   }
};


//----------------------------------------------------------------------------
// BSP Collision tree
// Solid nodes are represented by structures with NULL frontNode and
// backNode pointers. The material field is only valid on a solid node.
// There is no structure for empty nodes, frontNode or backNode
// should be set to NULL to represent empty half-spaces.

struct BSPNode
{
   U32 material;
   PlaneF plane;
   BSPNode *frontNode, *backNode;
};

typedef Chunker<BSPNode> BSPTree;

#endif
