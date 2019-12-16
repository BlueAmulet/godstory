//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SIMPATH_H_
#define _SIMPATH_H_

#ifndef _RENDERABLESCENEOBJECT_H
#include "sceneGraph/RenderableSceneObject.h"
#endif

//--------------------------------------------------------------------------
/// A path!
class Path : public SimGroup
{
   typedef SimGroup Parent;

  public:
   enum {
      NoPathIndex = 0xFFFFFFFF
   };


  private:
   U32 mPathID;
   U32 mPathIndex;
   bool mIsLooping;

  protected:
   bool onAdd();
   void onRemove();

  public:
   Path();
   ~Path();

   void OnSerialize( CTinyStream& stream );
   void OnUnserialize( CTinyStream& stream );

   void addObject(SimObject*);
   void removeObject(SimObject*);

   void sortMarkers();
   void updatePath();
   bool isLooping() { return mIsLooping; }
   void  setPathID(S32 pathID) { mPathID = pathID; }
   S32  getPathID() { return mPathID; }	

   U32 getPathIndex() const;

   DECLARE_CONOBJECT(Path);
   static void initPersistFields();
};


//--------------------------------------------------------------------------
class Marker : public RenderableSceneObject
{
   typedef RenderableSceneObject Parent;
   friend class Path;

  public:
   enum {
      SmoothingTypeLinear,
      SmoothingTypeSpline,
      SmoothingTypeAccelerate,
   };

   enum {
      KnotTypeNormal,
      KnotTypePositionOnly,
      KnotTypeKink,
   };


   U32   mSeqNum;
   U32   mSmoothingType;
   U32   mKnotType;

   U32   mMSToNext;

   // Rendering
  protected:
   bool prepRenderImage(SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState);
   void renderObject(SceneState *state);

  protected:
   bool onAdd();
   void onRemove();
   void onGroupAdd();

   void onEditorEnable();
   void onEditorDisable();


  public:
   Marker();
   ~Marker();

   void OnSerialize( CTinyStream& stream );
   void OnUnserialize( CTinyStream& stream );

   DECLARE_CONOBJECT(Marker);
   static void initPersistFields();
   void inspectPostApply();

   U64  packUpdate  (NetConnection *conn, U64 mask, BitStream *stream);
   void unpackUpdate(NetConnection *conn,           BitStream *stream);
};

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
inline U32 Path::getPathIndex() const
{
   return mPathIndex;
}


#endif // _H_PATH

