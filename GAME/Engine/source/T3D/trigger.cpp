//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "sceneGraph/sceneState.h"
#include "console/consoleTypes.h"
#include "collision/boxConvex.h"

#include "core/bitStream.h"
#include "math/mathIO.h"
//#include "DatBuilder/DatBuilder.h"
#include "Gameplay/Data/readDataFile.h"
#include "T3D/trigger.h"
#include "sceneGraph/sceneGraph.h"
#include "gameplay/ClientGameplayState.h"
#include "sfx/sfxSystem.h"
#include "Util/ColumnData.h"

//-----------------------------------------------------------------------------

ConsoleMethod( TriggerData, onEnterTrigger, void, 4, 4, "( Trigger t, SimObject intruder)")
{
   Trigger* trigger = NULL;
   if (Sim::findObject(argv[2], trigger) == false)
      return;

   // Do nothing with the trigger object id by default...
   SimGroup* pGroup = trigger->getGroup();
   for (SimGroup::iterator itr = pGroup->begin(); itr != pGroup->end(); itr++)
      Con::executef(*itr, "onTrigger", Con::getIntArg(trigger->getId()), "1");
}

ConsoleMethod( TriggerData, onLeaveTrigger, void, 4, 4, "( Trigger t, SimObject intruder)")
{
   Trigger* trigger = NULL;
   if (Sim::findObject(argv[2], trigger) == false)
      return;

   if (trigger->getNumTriggeringObjects() == 0) {
      SimGroup* pGroup = trigger->getGroup();
      for (SimGroup::iterator itr = pGroup->begin(); itr != pGroup->end(); itr++)
         Con::executef(*itr, "onTrigger", Con::getIntArg(trigger->getId()), "0");
   }
}

ConsoleMethod( TriggerData, onTickTrigger, void, 3, 3, "(Trigger t)")
{
   Trigger* trigger = NULL;
   if (Sim::findObject(argv[2], trigger) == false)
      return;

   // Do nothing with the trigger object id by default...
   SimGroup* pGroup = trigger->getGroup();
   for (SimGroup::iterator itr = pGroup->begin(); itr != pGroup->end(); itr++)
      Con::executef(*itr, "onTriggerTick", Con::getIntArg(trigger->getId()));
}

ConsoleMethod( Trigger, getNumObjects, S32, 2, 2, "")
{
   return object->getNumTriggeringObjects();
}

ConsoleMethod( Trigger, getObject, S32, 3, 3, "(int idx)")
{
   S32 index = dAtoi(argv[2]);

   if (index >= object->getNumTriggeringObjects() || index < 0)
      return -1;
   else
      return object->getObject(U32(index))->getId();
}


//----------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(TriggerData);

TriggerData::TriggerData()
{
   tickPeriodMS		= 100;
   mTriggerID		= 0;
   mFatherTriggerID = 0;
   mAreaName		= NULL;
   mPictureName		= NULL;
   mEnterScript		= NULL;
   mExitScript		= NULL;
   mBackMusic		= NULL;
   mZoneID			= 0;
   mReluvePos		= NULL;
   mBaseLimit		= 0;
   mSunAbient		= NULL;
   mLightColor		= NULL;
   mShaderColor		= NULL;
   mFogColor		= NULL;
   mStallPrice		= 0;
   mTradeTax		= 0;
}

bool TriggerData::onAdd()
{
   if (!Parent::onAdd())
      return false;

   return true;
}

void TriggerData::initPersistFields()
{
   Parent::initPersistFields();

   addField("tickPeriodMS", TypeS32,  Offset(tickPeriodMS, TriggerData));
  
}

StringTableEntry TriggerData::getBackSoundID()
{
	return mBackMusic;
}

//--------------------------------------------------------------------------
void TriggerData::packData(BitStream* stream)
{
   Parent::packData(stream);
   stream->write(tickPeriodMS);
  // stream->writeString(mName, 32);
}

void TriggerData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);
   stream->read(&tickPeriodMS);
  // mName = stream->readSTString();
   
}


//--------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1(Trigger);

DECLARE_SERIALIBLE( Trigger );

Trigger::Trigger()
{
   // Don't ghost by default.
   mNetFlags.clear(Ghostable);

   mTypeMask |= TriggerObjectType;

   mObjScale.set(1, 1, 1);
   mObjToWorld.identity();
   mWorldToObj.identity();

   mDataBlock = NULL;
   mFatherDataBlock = NULL;

   mLastThink = 0;
   mCurrTick  = 0;

   mConvexList = new Convex;
  // mName	  = NULL;
}

Trigger::~Trigger()
{
   delete mConvexList;
   mConvexList = NULL;
}

bool Trigger::castRay(const Point3F &start, const Point3F &end, RayInfo* info)
{
   // Collide against bounding box
   F32 st,et,fst = 0,fet = 1;
   F32 *bmin = &mObjBox.min.x;
   F32 *bmax = &mObjBox.max.x;
   F32 const *si = &start.x;
   F32 const *ei = &end.x;

   for (int i = 0; i < 3; i++) 
   {
      if (*si < *ei) 
      {
         if (*si > *bmax || *ei < *bmin)
            return false;
         F32 di = *ei - *si;
         st = (*si < *bmin)? (*bmin - *si) / di: 0;
         et = (*ei > *bmax)? (*bmax - *si) / di: 1;
      }
      else 
      {
         if (*ei > *bmax || *si < *bmin)
            return false;
         F32 di = *ei - *si;
         st = (*si > *bmax)? (*bmax - *si) / di: 0;
         et = (*ei < *bmin)? (*bmin - *si) / di: 1;
      }
      if (st > fst) fst = st;
      if (et < fet) fet = et;
      if (fet < fst)
         return false;
      bmin++; bmax++;
      si++; ei++;
   }

   info->normal = start - end;
   info->normal.normalizeSafe();
   getTransform().mulV( info->normal );

   info->t = fst;
   info->object = this;
   info->point.interpolate(start,end,fst);
   info->material = 0;
   return true;
}


//--------------------------------------------------------------------------
/* Console polyhedron data type exporter
   The polyhedron type is really a quadrilateral and consists of a corner
   point follow by three vectors representing the edges extending from the
   corner.
*/
ConsoleType( TriggerPolyhedron, TypeTriggerPolyhedron, Polyhedron )


ConsoleGetType( TypeTriggerPolyhedron )
{
   U32 i;
   Polyhedron* pPoly = reinterpret_cast<Polyhedron*>(dptr);

   // First point is corner, need to find the three vectors...`
   Point3F origin = pPoly->pointList[0];
   U32 currVec = 0;
   Point3F vecs[3];
   for (i = 0; i < pPoly->edgeList.size(); i++) {
      const U32 *vertex = pPoly->edgeList[i].vertex;
      if (vertex[0] == 0)
         vecs[currVec++] = pPoly->pointList[vertex[1]] - origin;
      else
         if (vertex[1] == 0)
            vecs[currVec++] = pPoly->pointList[vertex[0]] - origin;
   }
   AssertFatal(currVec == 3, "Internal error: Bad trigger polyhedron");

   // Build output string.
   char* retBuf = Con::getReturnBuffer(1024);
   dSprintf(retBuf, 1023, "%7.7f %7.7f %7.7f %7.7f %7.7f %7.7f %7.7f %7.7f %7.7f %7.7f %7.7f %7.7f",
            origin.x, origin.y, origin.z,
            vecs[0].x, vecs[0].y, vecs[0].z,
            vecs[2].x, vecs[2].y, vecs[2].z,
            vecs[1].x, vecs[1].y, vecs[1].z);

   return retBuf;
}

/* Console polyhedron data type loader
   The polyhedron type is really a quadrilateral and consists of an corner
   point follow by three vectors representing the edges extending from the
   corner.
*/
ConsoleSetType( TypeTriggerPolyhedron )
{
   if (argc != 1) {
      Con::printf("(TypeTriggerPolyhedron) multiple args not supported for polyhedra");
      return;
   }

   Point3F origin;
   Point3F vecs[3];

   U32 numArgs = dSscanf(argv[0], "%g %g %g %g %g %g %g %g %g %g %g %g",
                         &origin.x, &origin.y, &origin.z,
                         &vecs[0].x, &vecs[0].y, &vecs[0].z,
                         &vecs[1].x, &vecs[1].y, &vecs[1].z,
                         &vecs[2].x, &vecs[2].y, &vecs[2].z);
   if (numArgs != 12) {
      Con::printf("Bad polyhedron!");
      return;
   }

   Polyhedron* pPoly = reinterpret_cast<Polyhedron*>(dptr);

   pPoly->pointList.setSize(8);
   pPoly->pointList[0] = origin;
   pPoly->pointList[1] = origin + vecs[0];
   pPoly->pointList[2] = origin + vecs[1];
   pPoly->pointList[3] = origin + vecs[2];
   pPoly->pointList[4] = origin + vecs[0] + vecs[1];
   pPoly->pointList[5] = origin + vecs[0] + vecs[2];
   pPoly->pointList[6] = origin + vecs[1] + vecs[2];
   pPoly->pointList[7] = origin + vecs[0] + vecs[1] + vecs[2];

   Point3F normal;
   pPoly->planeList.setSize(6);

   mCross(vecs[2], vecs[0], &normal);
   pPoly->planeList[0].set(origin, normal);
   mCross(vecs[0], vecs[1], &normal);
   pPoly->planeList[1].set(origin, normal);
   mCross(vecs[1], vecs[2], &normal);
   pPoly->planeList[2].set(origin, normal);
   mCross(vecs[1], vecs[0], &normal);
   pPoly->planeList[3].set(pPoly->pointList[7], normal);
   mCross(vecs[2], vecs[1], &normal);
   pPoly->planeList[4].set(pPoly->pointList[7], normal);
   mCross(vecs[0], vecs[2], &normal);
   pPoly->planeList[5].set(pPoly->pointList[7], normal);

   pPoly->edgeList.setSize(12);
   pPoly->edgeList[0].vertex[0]  = 0; pPoly->edgeList[0].vertex[1]  = 1; pPoly->edgeList[0].face[0]  = 0; pPoly->edgeList[0].face[1]  = 1;
   pPoly->edgeList[1].vertex[0]  = 1; pPoly->edgeList[1].vertex[1]  = 5; pPoly->edgeList[1].face[0]  = 0; pPoly->edgeList[1].face[1]  = 4;
   pPoly->edgeList[2].vertex[0]  = 5; pPoly->edgeList[2].vertex[1]  = 3; pPoly->edgeList[2].face[0]  = 0; pPoly->edgeList[2].face[1]  = 3;
   pPoly->edgeList[3].vertex[0]  = 3; pPoly->edgeList[3].vertex[1]  = 0; pPoly->edgeList[3].face[0]  = 0; pPoly->edgeList[3].face[1]  = 2;
   pPoly->edgeList[4].vertex[0]  = 3; pPoly->edgeList[4].vertex[1]  = 6; pPoly->edgeList[4].face[0]  = 3; pPoly->edgeList[4].face[1]  = 2;
   pPoly->edgeList[5].vertex[0]  = 6; pPoly->edgeList[5].vertex[1]  = 2; pPoly->edgeList[5].face[0]  = 2; pPoly->edgeList[5].face[1]  = 5;
   pPoly->edgeList[6].vertex[0]  = 2; pPoly->edgeList[6].vertex[1]  = 0; pPoly->edgeList[6].face[0]  = 2; pPoly->edgeList[6].face[1]  = 1;
   pPoly->edgeList[7].vertex[0]  = 1; pPoly->edgeList[7].vertex[1]  = 4; pPoly->edgeList[7].face[0]  = 4; pPoly->edgeList[7].face[1]  = 1;
   pPoly->edgeList[8].vertex[0]  = 4; pPoly->edgeList[8].vertex[1]  = 2; pPoly->edgeList[8].face[0]  = 1; pPoly->edgeList[8].face[1]  = 5;
   pPoly->edgeList[9].vertex[0]  = 4; pPoly->edgeList[9].vertex[1]  = 7; pPoly->edgeList[9].face[0]  = 4; pPoly->edgeList[9].face[1]  = 5;
   pPoly->edgeList[10].vertex[0] = 5; pPoly->edgeList[10].vertex[1] = 7; pPoly->edgeList[10].face[0] = 3; pPoly->edgeList[10].face[1] = 4;
   pPoly->edgeList[11].vertex[0] = 7; pPoly->edgeList[11].vertex[1] = 6; pPoly->edgeList[11].face[0] = 3; pPoly->edgeList[11].face[1] = 5;
}


//-----------------------------------------------------------------------------

void Trigger::initPersistFields()
{
   Parent::initPersistFields();

   addField("polyhedron", TypeTriggerPolyhedron, Offset(mTriggerPolyhedron, Trigger));
   addField("triggerID",	TypeS32,  Offset(mDataBlockId, Trigger));
}


//--------------------------------------------------------------------------

bool Trigger::onAdd()
{
	Parent::mDataBlock = g_TriggerDataManager->getTriggerData(mDataBlockId);

   if(!Parent::onAdd())
      return false;
  
   Con::executef(this, "onAdd", Con::getIntArg(getId()));

   Polyhedron temp = mTriggerPolyhedron;
   setTriggerPolyhedron(temp);

   addToScene();

   return true;
}

void Trigger::onRemove()
{
   Con::executef(this, "onRemove", Con::getIntArg(getId()));

   mConvexList->nukeList();

   removeFromScene();
   Parent::onRemove();
}

bool Trigger::onNewDataBlock(GameBaseData* dptr)
{
   mDataBlock = dynamic_cast<TriggerData*>(dptr);
   if (!mDataBlock || !Parent::onNewDataBlock(dptr))
      return false;

   if(mDataBlock)
   {
	   mFatherDataBlock = g_TriggerDataManager->getTriggerData(mDataBlock->mFatherTriggerID);
   }


   scriptOnNewDataBlock();
   return true;
}

void Trigger::onDeleteNotify(SimObject* obj)
{
   GameBase* pScene = dynamic_cast<GameBase*>(obj);
   if (pScene != NULL) 
   {
      for (U32 i = 0; i < mObjects.size(); i++) 
      {
         if (pScene == mObjects[i]) 
         {
            mObjects.erase(i);

			/*if(mFatherDataBlock)
				setEnterSettings(mFatherDataBlock, pScene);
			else
			{
				if(mDataBlock)
					setExistSettings(mDataBlock, pScene);
			}*/
			
           // Con::executef(mDataBlock, "onLeaveTrigger", scriptThis(), Con::getIntArg(pScene->getId()));
            break;
         }
      }
   }

   Parent::onDeleteNotify(obj);
}

void Trigger::onEditorEnable()
{
   // Enable scoping so we can see this thing on the client.
   mNetFlags.set(Ghostable);
   setScopeAlways();
}

void Trigger::onEditorDisable()
{
   // Disable scoping so we can see this thing on the client
   mNetFlags.clear(Ghostable);
   clearScopeAlways();
}

//--------------------------------------------------------------------------

void Trigger::buildConvex(const Box3F& box, Convex* convex)
{
   // These should really come out of a pool
   mConvexList->collectGarbage();

   Box3F realBox = box;
   mWorldToObj.mul(realBox);
   realBox.min.convolveInverse(mObjScale);
   realBox.max.convolveInverse(mObjScale);

   if (realBox.isOverlapped(getObjBox()) == false)
      return;

   // Just return a box convex for the entire shape...
   Convex* cc = 0;
   CollisionWorkingList& wl = convex->getWorkingList();
   for (CollisionWorkingList* itr = wl.wLink.mNext; itr != &wl; itr = itr->wLink.mNext) {
      if (itr->mConvex->getType() == BoxConvexType &&
          itr->mConvex->getObject() == this) {
         cc = itr->mConvex;
         break;
      }
   }
   if (cc)
      return;

   // Create a new convex.
   BoxConvex* cp = new BoxConvex;
   mConvexList->registerObject(cp);
   convex->addToWorkingList(cp);
   cp->init(this);

   mObjBox.getCenter(&cp->mCenter);
   cp->mSize.x = mObjBox.len_x() / 2.0f;
   cp->mSize.y = mObjBox.len_y() / 2.0f;
   cp->mSize.z = mObjBox.len_z() / 2.0f;
}


//------------------------------------------------------------------------------

void Trigger::setTransform(const MatrixF & mat)
{
   Parent::setTransform(mat);

   if (isServerObject()) {
      MatrixF base(true);
      base.scale(Point3F(1.0/mObjScale.x,
                         1.0/mObjScale.y,
                         1.0/mObjScale.z));
      base.mul(mWorldToObj);
      mClippedList.setBaseTransform(base);

      setMaskBits(GameBase::InitialUpdateMask);
   }
}

void Trigger::setTriggerPolyhedron(const Polyhedron& rPolyhedron)
{
   mTriggerPolyhedron = rPolyhedron;

   if (mTriggerPolyhedron.pointList.size() != 0) {
      mObjBox.min.set(1e10, 1e10, 1e10);
      mObjBox.max.set(-1e10, -1e10, -1e10);
      for (U32 i = 0; i < mTriggerPolyhedron.pointList.size(); i++) {
         mObjBox.min.setMin(mTriggerPolyhedron.pointList[i]);
         mObjBox.max.setMax(mTriggerPolyhedron.pointList[i]);
      }
   } else {
      mObjBox.min.set(-0.5, -0.5, -0.5);
      mObjBox.max.set( 0.5,  0.5,  0.5);
   }

   MatrixF xform = getTransform();
   setTransform(xform);

   mClippedList.clear();
   mClippedList.mPlaneList = mTriggerPolyhedron.planeList;
//   for (U32 i = 0; i < mClippedList.mPlaneList.size(); i++)
//      mClippedList.mPlaneList[i].neg();

   MatrixF base(true);
   base.scale(Point3F(1.0/mObjScale.x,
                      1.0/mObjScale.y,
                      1.0/mObjScale.z));
   base.mul(mWorldToObj);

   mClippedList.setBaseTransform(base);
}


//--------------------------------------------------------------------------

bool Trigger::testObject(GameBase* enter)
{
   if (mTriggerPolyhedron.pointList.size() == 0)
      return false;

   mClippedList.clear();

   SphereF sphere;
   sphere.center = (mWorldBox.min + mWorldBox.max) * 0.5;
   VectorF bv = mWorldBox.max - sphere.center;
   sphere.radius = bv.len();

   enter->buildPolyList(&mClippedList, mWorldBox, sphere);
   return mClippedList.isEmpty() == false;
}


void Trigger::potentialEnterObject(GameBase* enter)
{
   //AssertFatal(isServerObject(), "Error, should never be called on the client!");
  
   for (U32 i = 0; i < mObjects.size(); i++) {
      if (mObjects[i] == enter)
         return;
   }
	
   GameObject* obj = (enter->getType() & GameObjectType) ? (GameObject*)enter : NULL;
   if (testObject(enter) || (obj && obj->isMounted())) {
      mObjects.push_back(enter);
      deleteNotify(enter);
	// <Edit> [12/29/2009 iceRain] 针对玩家做的，避免重复触发
	  if(!obj)
		  return;
	  if((obj->getGameObjectMask() & PlayerObjectType))
	  {
		  Player* pPlayer = (Player*)(obj);
		  if(!pPlayer)
			return;
		  if(pPlayer->findTriggerID(mDataBlock->mTriggerID))
			  pPlayer->mEnterTriggerID.push_back(mDataBlock->mTriggerID);
		  else
			  return;
	  }
	  else if((obj->getGameObjectMask() & MountObjectType))
	  {
		 MountObject* pMount = (MountObject*)(obj);
		 if(!pMount)
			 return;
		 for (ShapeBase* pSB = pMount->getMountList(); pSB; pSB = pSB->getMountLink())
		 {
			 if(pSB->getTypeMask() & GameObjectType)
				potentialEnterObject((GameObject*)pSB);
		 }
	  }
	  else 
		  return;
	  if(mDataBlock)
	    setEnterSettings(mDataBlock,enter);
   }
}


void Trigger::processTick(const Move* move)
{
   Parent::processTick(move);

 /*  if (isClientObject())
      return;*/

   //
   if (mObjects.size() == 0)
      return;

   if (mLastThink + mDataBlock->tickPeriodMS < mCurrTick) {
      mCurrTick  = 0;
      mLastThink = 0;

      for (S32 i = S32(mObjects.size() - 1); i >= 0; i--) {
         if (testObject(mObjects[i]) == false) {
            GameBase* remove = mObjects[i];
            mObjects.erase(i);
            clearNotify(remove);
			if(mFatherDataBlock)
				setEnterSettings(mFatherDataBlock,remove);
			else
			{
				if(mDataBlock)
					setExistSettings(mDataBlock,remove);
			}
			GameObject* obj = (GameObject*)remove;
			if(obj)
			{
				bool exit = false;
				for (S32 i=0; i<obj->mEnterTriggerID.size(); ++i)
				{
					if(obj->mEnterTriggerID[i] == mDataBlock->mTriggerID)
					{
						obj->mEnterTriggerID.setSize(i+1);
						exit = true;
						break;
					}
				}
				// 移除出错？
				if(!exit)
					Con::warnf("GameObject::mEnterTriggerID !");
			}
         }
      }

	  //Ray: 屏蔽ticktrigger
      //if (mObjects.size() != 0)
      //   Con::executef(mDataBlock, "onTickTrigger", scriptThis());
   } else {
      mCurrTick += TickMs;
   }
}


//--------------------------------------------------------------------------

bool Trigger::prepRenderImage(SceneState* state, const U32 stateKey,
                              const U32 /*startZone*/, const bool /*modifyBaseState*/)
{
   if (isLastState(state, stateKey))
      return false;
   setLastState(state, stateKey);

   // This should be sufficient for most objects that don't manage zones, and
   //  don't need to return a specialized RenderImage...
   if (state->isObjectRendered(this)) {
/*
      SceneRenderImage* image = new SceneRenderImage;
      image->obj = this;
      image->isTranslucent = true;
      image->sortType = SceneRenderImage::Point;
      state->insertRenderImage(image);
*/
   }
   return false;
}

void Trigger::renderObject(SceneState* state)
{
	//mTriggerPolyhedron.render();
/*
   AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on entry");

   RectI viewport;
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   dglGetViewport(&viewport);

   // Uncomment this if this is a "simple" (non-zone managing) object
   state->setupObjectProjection(this);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   dglMultMatrix(&mObjToWorld);
   glScalef(mObjScale.x, mObjScale.y, mObjScale.z);

   mTriggerPolyhedron.render();

   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   dglSetViewport(viewport);

   AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on exit");
*/
}


//--------------------------------------------------------------------------

U64 Trigger::packUpdate(NetConnection* con, U64 mask, BitStream* stream)
{
   U32 i;
   U64 retMask = Parent::packUpdate(con, mask, stream);

   // Note that we don't really care about efficiency here, since this is an
   //  edit-only ghost...
   stream->writeAffineTransform(mObjToWorld);
   mathWrite(*stream, mObjScale);

   // Write the polyhedron
   stream->write(mTriggerPolyhedron.pointList.size());
   for (i = 0; i < mTriggerPolyhedron.pointList.size(); i++)
      mathWrite(*stream, mTriggerPolyhedron.pointList[i]);

   stream->write(mTriggerPolyhedron.planeList.size());
   for (i = 0; i < mTriggerPolyhedron.planeList.size(); i++)
      mathWrite(*stream, mTriggerPolyhedron.planeList[i]);

   stream->write(mTriggerPolyhedron.edgeList.size());
   for (i = 0; i < mTriggerPolyhedron.edgeList.size(); i++) {
      const Polyhedron::Edge& rEdge = mTriggerPolyhedron.edgeList[i];

      stream->write(rEdge.face[0]);
      stream->write(rEdge.face[1]);
      stream->write(rEdge.vertex[0]);
      stream->write(rEdge.vertex[1]);
   }
	//stream->write(mName);
   return retMask;
}

void Trigger::unpackUpdate(NetConnection* con, BitStream* stream)
{
   Parent::unpackUpdate(con, stream);

   U32 i, size;
   MatrixF temp;
   Point3F tempScale;
   Polyhedron tempPH;

   // Transform
   stream->readAffineTransform(&temp);
   mathRead(*stream, &tempScale);

   // Read the polyhedron
   stream->read(&size);
   tempPH.pointList.setSize(size);
   for (i = 0; i < tempPH.pointList.size(); i++)
      mathRead(*stream, &tempPH.pointList[i]);

   stream->read(&size);
   tempPH.planeList.setSize(size);
   for (i = 0; i < tempPH.planeList.size(); i++)
      mathRead(*stream, &tempPH.planeList[i]);

   stream->read(&size);
   tempPH.edgeList.setSize(size);
   for (i = 0; i < tempPH.edgeList.size(); i++) {
      Polyhedron::Edge& rEdge = tempPH.edgeList[i];

      stream->read(&rEdge.face[0]);
      stream->read(&rEdge.face[1]);
      stream->read(&rEdge.vertex[0]);
      stream->read(&rEdge.vertex[1]);
   }
   //mName = stream->readSTString();
   setTriggerPolyhedron(tempPH);
   setScale(tempScale);
   setTransform(temp);
}
// <Edit> [3/11/2009 iceRain] 进入或退出触发器进行的操作
void Trigger::setEnterSettings(TriggerData* data, GameBase* player)
{
	if(!(player->getTypeMask() & GameObjectType))
		return;
	GameObject* pObj = (GameObject*)player;
	pObj->setTriggerID(data->mTriggerID);

	if(!(pObj->getGameObjectMask() & PlayerObjectType))
		return;
	Player* pPlayer = (Player*)(player);

#ifdef NTJ_CLIENT

	Player* self = g_ClientGameplayState->GetControlPlayer();
	if(self && pPlayer == self)
	{
		static ColorF sunColor;
		static ColorF lightColor;
		static ColorF shaderColor;
		static ColorF fogColor;

		ColorF *pSunColor = NULL;
		ColorF *pLightColor = NULL;
		ColorF *pShaderColor = NULL;
		if(data->mSunAbient && data->mSunAbient[0] )
		{
			dSscanf(data->mSunAbient, "%f %f %f", &sunColor.red, &sunColor.green, &sunColor.blue);
			pSunColor = &sunColor;
		}
		if(data->mLightColor && data->mLightColor[0])
		{
			dSscanf(data->mLightColor, "%f %f %f", &lightColor.red, &lightColor.green, &lightColor.blue);
			pLightColor = &lightColor;
		}
		if(data->mSunAbient && data->mSunAbient[0])
		{
			dSscanf(data->mSunAbient, "%f %f %f", &shaderColor.red, &shaderColor.green, &shaderColor.blue);
			pShaderColor = &shaderColor;
		}
		gClientSceneGraph->setSunColor(pSunColor, pLightColor, pShaderColor);

		if(data->mFogColor && data->mFogColor[0])
		{
			dSscanf(data->mFogColor, "%f %f %f", &fogColor.red, &fogColor.green, &fogColor.blue);
			gClientSceneGraph->setFogColor(fogColor);
		}
		//设置背景声音
		if(data->mBackMusic && data->mBackMusic[0])
		{

			U32 fatherID = data->mFatherTriggerID;
			if(fatherID > 0)
			{
				StringTableEntry soundID = g_TriggerDataManager->getBackMusicID(fatherID);
				Con::executef("SoundFadeInByID", soundID, data->getBackSoundID());
			}
			else
			{
				/*FMOD_CHANNEL* chanel = SFX->getChannelBySoundID("");
				if(chanel)
				{
				Con::executef("SoundFadeInByID", "B_1001_01", data->getBackSoundID());
				}
				else*/
				Con::executef("ChannelPauseByID", data->getBackSoundID(), Con::getIntArg(0));
			}
		}

		if(data->mPictureName && data->mPictureName[0] && (data->mTriggerID != pPlayer->mShowTriggerID))
		{
			char id[12];
			dSprintf(id, sizeof(id), "%d", data->mTriggerID);		
			Con::executef("ShowAreaTriggerTexture",id);
			pPlayer->mShowTriggerID = data->mTriggerID;
		}
	}
	
	if (data->mAreaName && data->mAreaName[0])
	{
		Con::executef("ShowMapName",data->mAreaName);
	}
#endif
	

	//地图
	if(data->mZoneID > 0)
	{

	}
	//复活坐标
	if(data->mReluvePos && data->mReluvePos[0])
	{
		Point3F pos;
		dSscanf(data->mReluvePos, "%f %f %f", &pos.x, &pos.y, &pos.z);
//#ifdef NTJ_SERVER
//		Player* pPlayer = dynamic_cast<Player*>(player);
//		if(pPlayer)
//			pPlayer->transportObject(g_ClientGameplayState->getCurrentLineId(), pPlayer->getTriggerId(), data->mZoneID, pos.x, pos.y, pos.z);
//#endif
//		
//#ifdef NTJ_CLIENT
//		Con::executef("RiseRequest", Con::getIntArg(0));
//#endif
		
	}
	
	//限制
	if(data->mBaseLimit > 0)
	{
		
	}
	//摆摊价格
	if(data->mStallPrice)
	{

	}
	//交易税
	if(data->mTradeTax)
	{

	}
	//进入时脚本
	char id[12];
	dSprintf(id, sizeof(id), "%d", data->mTriggerID);
	Con::executef("OnEnterTrigger", player->scriptThis(),id,Con::getIntArg(data->mFatherTriggerID));
	

}

void Trigger::setExistSettings(TriggerData* data, GameBase* player)
{
	Player* pPlayer = dynamic_cast<Player*>(player);
	if(!pPlayer)
		return;
#ifdef NTJ_CLIENT
	if(data->mBackMusic && data->mBackMusic[0])
	{
		U32 fatherID = data->mFatherTriggerID;
		if(fatherID > 0)
		{
			StringTableEntry soundID = g_TriggerDataManager->getBackMusicID(fatherID);
			Con::executef("SoundFadeInByID",  data->mBackMusic, soundID);
		}
		else
			Con::executef("ChannelPauseByID", data->mBackMusic, Con::getIntArg(1));
	}
#endif
	char id[12];
	dSprintf(id, sizeof(id), "%d", data->mTriggerID);	
	Con::executef("OnLeaveTrigger", player->scriptThis(), id, Con::getIntArg(data->mFatherTriggerID));
	
}

//addField("polyhedron", TypeTriggerPolyhedron, Offset(mTriggerPolyhedron, Trigger));
//addField("triggerID",	TypeS32,  Offset(mDataBlockId, Trigger));

void Trigger::OnSerialize( CTinyStream& stream )
{
	__super::OnSerialize( stream );

	operator << ( stream, mTriggerPolyhedron );
	stream << mDataBlockId;

}

void Trigger::OnUnserialize( CTinyStream& stream )
{
	__super::OnUnserialize( stream );

	operator >> ( stream, mTriggerPolyhedron );
	stream >> mDataBlockId;
	
}

//--------------------------------------------------------------------------
// <Edit> [3/11/2009 iceRain] 触发器数据管理
triggerDataMannager* g_TriggerDataManager = NULL;

triggerDataMannager::triggerDataMannager()
{
	//ReadTriggerData();
	mColumnData = NULL;
}

triggerDataMannager::~triggerDataMannager()
{
	//clearTriggerData();
	if(mColumnData)
		delete mColumnData;
}

void triggerDataMannager::creatTriggerDataManager()
{
	if(g_TriggerDataManager == NULL)
		g_TriggerDataManager = new triggerDataMannager;
}

void triggerDataMannager::destroyTriggerDataManager()
{
	if(g_TriggerDataManager != NULL)
		delete g_TriggerDataManager;
	g_TriggerDataManager = NULL;
}

void triggerDataMannager::clearTriggerData()
{
	dataMap::iterator it = mDataMap.begin();
	for(; it != mDataMap.end(); ++it)
	{
		if(it->second)
			delete it ->second;
	}
	mDataMap.clear();
}

//--------------------------------------------------------------------------
void triggerDataMannager::ReadTriggerData()
{
	CDataFile  op;
	RData tempdata;
	op.ReadDataInit();
	char filename[1024];
	Platform::makeFullPathName(GAME_TRIGGERDATA_FILE, filename, sizeof(filename));
	AssertRelease(op.readDataFile(filename), "can't read file : TriggerRepository.dat!");
	mColumnData = new IColumnData(op.ColumNum, "TriggerRepository.dat");

	mColumnData->setField(CalcOffset(mTriggerID,			TriggerData),	DType_U32,		"触发器ID");
	mColumnData->setField(CalcOffset(mFatherTriggerID,		TriggerData),	DType_U32,		"父触发器ID");
	mColumnData->setField(CalcOffset(mPictureName,			TriggerData),	DType_string,	"一级触发器图片名");
	mColumnData->setField(CalcOffset(mAreaName,				TriggerData),	DType_string,	"区域名称");
	mColumnData->setField(CalcOffset(mEnterScript,			TriggerData),	DType_string,	"进入时执行脚本");
	mColumnData->setField(CalcOffset(mExitScript,			TriggerData),	DType_string,	"退出时执行脚本");
	mColumnData->setField(CalcOffset(mZoneID,				TriggerData),	DType_U16,		"复活地图编号");
	mColumnData->setField(CalcOffset(mReluvePos,			TriggerData),	DType_string,	"复活坐标");
	mColumnData->setField(CalcOffset(mBackMusic,			TriggerData),	DType_string,	"背景音乐");
	mColumnData->setField(CalcOffset(mBaseLimit,			TriggerData),	DType_U32,		"基本限制");
	mColumnData->setField(CalcOffset(mLightColor,			TriggerData),	DType_string,	"光源颜色");
	mColumnData->setField(CalcOffset(mShaderColor,			TriggerData),	DType_string,	"顶点色");
	mColumnData->setField(CalcOffset(mSunAbient,			TriggerData),	DType_string,	"环境光颜色");
	mColumnData->setField(CalcOffset(mFogColor,				TriggerData),	DType_string,	"雾的颜色");
	mColumnData->setField(CalcOffset(mStallPrice,			TriggerData),	DType_U16,		"摆摊价格");
	mColumnData->setField(CalcOffset(mTradeTax,				TriggerData),	DType_U16,		"交易税");

	for(int i=0; i<op.RecordNum; ++i)
	{		
		TriggerData* data = new TriggerData;
		for(int h = 0; h < op.ColumNum; ++h)
		{
			op.GetData(tempdata);
			mColumnData->setData(data, h, tempdata);
		}
		data->tickPeriodMS = 100;
		data->dataBlockId = data->mTriggerID;
		mDataMap.insert(dataMap::value_type(data->mTriggerID, data));
	}
	op.ReadDataClose();
	
}

TriggerData* triggerDataMannager::getTriggerData(S32 id)
{
	if(mDataMap.size() == 0)
		return NULL;
	dataMap::iterator it = mDataMap.find(id);
	if(it !=  mDataMap.end())
		return it->second;
	else
		return NULL;
}


void triggerDataMannager::removeTriggerData(S32 id)
{
	dataMap::iterator  it = mDataMap.find(id);
	if(it != mDataMap.end())
	{
		delete mDataMap[id];
		mDataMap.erase(it);
	}
	else
		return;
			
}

StringTableEntry triggerDataMannager::getBackMusicID(U32 ID)
{
	if(mDataMap.size() == 0)
		return NULL;
	dataMap::iterator it = mDataMap.find(ID);
	if(it !=  mDataMap.end())
		return it->second->mBackMusic;
	else
		return NULL;
}

// ----------------------------------------------------------------------------
// 通过列数获取区域数据字段值
ConsoleFunction( getTriggerData, const char*, 3, 3, "getTriggerData(%triggerid, %col)")
{
	TriggerData* data = g_TriggerDataManager->getTriggerData(dAtol(argv[1]));
	if(data)
	{
		std::string to;
		g_TriggerDataManager->mColumnData->getData(data, dAtoi(argv[2]), to);
		char* value = Con::getReturnBuffer(512);
		dStrcpy(value, 512, to.c_str());
		return value;		
	}
	return false;
}
