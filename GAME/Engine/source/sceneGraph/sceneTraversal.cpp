//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/sgUtil.h"
#include "sceneGraph/sceneObject.h"
#include "sceneGraph/sceneState.h"
#include "math/mMatrix.h"
#include "terrain/terrData.h"
#include "gfx/gfxDevice.h"

#include "interior/interiorInstance.h"
#include "Gameplay/Data/GraphicsProfile.h"

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#endif

#ifdef NTJ_EDITOR
#include "Gameplay/EditorGameplayState.h"
#endif

namespace {

class PotentialRenderList
{
  public:
   Point3F farPosLeftUp;
   Point3F farPosLeftDown;
   Point3F farPosRightUp;
   Point3F farPosRightDown;
   Point3F camPos;
   Point3F ObjPos;
   F32     viewDistSquared;
   F32     objSenseDistSquared;
   F32     staticObjViewDistSquared[CGraphicsProfile::MaxTinyStaticLevel+1];
   Box3F   mBox;

   Vector<SceneObject*> mList;

   PlaneF viewPlanes[5];
   SceneState* mState;

  public:
   PotentialRenderList() 
   {
	   viewDistSquared=1000000;
	   objSenseDistSquared = 1000000;
	   for(int i=0;i<=CGraphicsProfile::MaxTinyStaticLevel;i++)
	   {
		   staticObjViewDistSquared[i] = 1000000;
	   }
   }

   void forceInsertObject(SceneObject* obj){ mList.push_back(obj);}
   bool insertObject(SceneObject* obj);
   void setupClipPlanes(SceneState*);
};

// MM/JF: Added for mirrorSubObject fix.
void PotentialRenderList::setupClipPlanes(SceneState* state)
{
	mState = state;
	camPos = state->getCameraPosition();
#ifndef NTJ_SERVER
#ifdef NTJ_CLIENT
	SceneObject* obj = (SceneObject *)g_ClientGameplayState->GetControlObject();
#else
	SceneObject* obj = (SceneObject *)g_EditorGameplayState->GetControlObject();
#endif
	ObjPos.set(obj ? obj->getPosition() : camPos);
#else
	ObjPos.set(camPos);
#endif
	F32 farOverNear = state->getFarPlane() / state->getNearPlane();
	farPosLeftUp = Point3F(state->getBaseZoneState().frustum[0] * farOverNear, state->getFarPlane(), state->getBaseZoneState().frustum[3] * farOverNear);
	farPosLeftDown = Point3F(state->getBaseZoneState().frustum[0] * farOverNear, state->getFarPlane(), state->getBaseZoneState().frustum[2] * farOverNear);
	farPosRightUp = Point3F(state->getBaseZoneState().frustum[1] * farOverNear, state->getFarPlane(), state->getBaseZoneState().frustum[3] * farOverNear);
	farPosRightDown = Point3F(state->getBaseZoneState().frustum[1] * farOverNear, state->getFarPlane(), state->getBaseZoneState().frustum[2] * farOverNear);
	MatrixF temp = state->mModelview;
	temp.inverse();
	temp.mulP(farPosLeftUp);
	temp.mulP(farPosLeftDown);
	temp.mulP(farPosRightUp);
	temp.mulP(farPosRightDown);
	mBox.min = camPos;
	mBox.min.setMin(farPosLeftUp);
	mBox.min.setMin(farPosLeftDown);
	mBox.min.setMin(farPosRightUp);
	mBox.min.setMin(farPosRightDown);
	mBox.max = camPos;
	mBox.max.setMax(farPosLeftUp);
	mBox.max.setMax(farPosLeftDown);
	mBox.max.setMax(farPosRightUp);
	mBox.max.setMax(farPosRightDown);
	sgOrientClipPlanes(&viewPlanes[0], camPos, farPosLeftUp, farPosLeftDown, farPosRightUp, farPosRightDown);
}


bool PotentialRenderList::insertObject(SceneObject* obj)
{
   // Check to see if we need to render always.
   if(obj->isGlobalBounds())
   {
      mList.push_back(obj);
      return true;
   }

   const Box3F& rObjBox = obj->getObjBox();
   const Point3F& rScale = obj->getScale();

   Point3F center;
   rObjBox.getCenter(&center);
   center.convolve(rScale);

   Point3F xRad((rObjBox.max.x - rObjBox.min.x) * 0.5 * rScale.x, 0, 0);
   Point3F yRad(0, (rObjBox.max.y - rObjBox.min.y) * 0.5 * rScale.y, 0);
   Point3F zRad(0, 0, (rObjBox.max.z - rObjBox.min.z) * 0.5 * rScale.z);

   obj->getRenderTransform().mulP(center);
   obj->getRenderTransform().mulV(xRad);
   obj->getRenderTransform().mulV(yRad);
   obj->getRenderTransform().mulV(zRad);

   bool render = true;
   for (U32 i = 0; i < 5; i++) 
   {
      if (viewPlanes[i].whichSideBox(center, xRad, yRad, zRad, Point3F(0, 0, 0)) == PlaneF::Back) 
      {
         render = false;
         break;
      }
   }

   if (render)
   {
      mList.push_back(obj);
	  return true;
   }

   return false;
}

void prlInsertionCallback(SceneObject* sceneObj, void *key)
{
   PotentialRenderList* prList = (PotentialRenderList*)key;

   RenderableSceneObject *obj = dynamic_cast<RenderableSceneObject*>(sceneObj);
   if(!obj)
	   return;

   if(obj->isGlobalBounds())
   {
	  obj->onlyRenderShadow(false);
      prList->insertObject(obj);
      return;
   }

   F32 distanceSquared;
   if(obj->getType() & (StaticTSObjectType))
   {
	   S32 VDLevel = mClamp(obj->getVisibleDistanceLevel(), 0, CGraphicsProfile::MaxTinyStaticLevel);
	   if(g_GraphicsProfile.mTinyStaticLevel < VDLevel)
	   {
		   obj->setFade(RenderableSceneObject::Fade_PRLInsertion, false,false);
		   return;
	   }

	   distanceSquared = prList->staticObjViewDistSquared[VDLevel];
   }
   else if(obj->getType() & GameBaseObjectType)
   {
		distanceSquared  = prList->objSenseDistSquared;
   }
   else
   {
		distanceSquared  = prList->viewDistSquared;
   }

   Point3F closestPt = obj->getWorldBox().getClosestPoint(prList->ObjPos);
   F32 lenSquared = (closestPt - prList->ObjPos).lenSquared();
   if (lenSquared < distanceSquared)
   {
	   F32 len    = mSqrt(lenSquared);
	   F32 top    = obj->getWorldBox().max.z;
	   F32 bottom = obj->getWorldBox().min.z;
	   if (prList->mState->isBoxFogVisible(len, top, bottom))
	   {
		   obj->onlyRenderShadow(false);
		   if(prList->insertObject(obj))
		   {
			   obj->setFade(RenderableSceneObject::Fade_PRLInsertion, false);
			   return;
		   }
	   }
   }
   else
   {
	   obj->onlyRenderShadow(false);
	   obj->setFade(RenderableSceneObject::Fade_PRLInsertion, true);
	   if(obj->isFading())
		   prList->insertObject(obj);
	   return;
   }

   if(obj->shadowTest(prList->mState))
   {
	   obj->onlyRenderShadow(true);
	   prList->forceInsertObject(obj);
   }
}

} // namespace {}

extern void OnPrepRenderImage(S32 StateKey);

void SceneGraph::buildSceneTree(SceneState*  state,
                                SceneObject* baseObject,
                                const U32    baseZone,
                                const U32    currDepth,
                                const U32    objectMask )
{
   AssertFatal(this == gClientSceneGraph, "Error, only the client scenegraph can support this call!");

   // Search proceeds from the baseObject, and starts in the baseZone.
   // General Outline:
   //    - Traverse up the tree, stopping at either the root, or the last interior
   //       that prevents traversal outside
   //    - Query the container database for all objects intersecting the viewcone,
   //       which is clipped to the bounding box returned at the last stage of the
   //       above traversal.
   //    - Topo sort the returned objects.
   //    - Traverse through the list, calling setupZones on zone managers,
   //       and retreiving render images from all applicable objects (including
   //       ZM's)
   //    - This process may return "Transform portals", i.e., mirrors, rendered
   //       teleporters, etc.  For each of these, create a new SceneState object
   //       subsidiary to state, and restart the traversal, with the new parameters,
   //       and the correct baseObject and baseZone.

   // Objects (in particular, those managers that are part of the initial up
   //  traversal) keep track of whether or not they have returned a render image
   //  to the current state by a key, and the state object pointer.
   smStateKey++;

   // Save off the base state...
   SceneState::ZoneState saveBase = state->getBaseZoneState();

   SceneObject* pTraversalRoot = baseObject;
   U32          rootZone       = baseZone;
   while (true) 
   {
      if (pTraversalRoot->prepRenderImage(state, smStateKey, rootZone, true)) 
      {
         if (pTraversalRoot->getNumCurrZones() != 1)
            Con::errorf(ConsoleLogEntry::General,
                        "Error, must have one and only one zone to be a traversal root.  %s has %d",
                        pTraversalRoot->getName(), pTraversalRoot->getNumCurrZones());

         rootZone       = pTraversalRoot->getCurrZone(0);
         pTraversalRoot = getZoneOwner(rootZone);
      } 
      else 
      {
         break;
      }
   }

   // Restore the base state...
   SceneState::ZoneState& rState = state->getBaseZoneStateNC();
   rState = saveBase;

   // Ok.  Now we have renderimages for anything north of the object in the
   //  tree.  Create the query polytope, and clip it to the bounding box of
   //  the traversalRoot object.
   PotentialRenderList prl;
   prl.setupClipPlanes(state);
   prl.viewDistSquared = getVisibleDistanceMod() * getVisibleDistanceMod();
   prl.objSenseDistSquared = getObjSenseDistanceMod() * getObjSenseDistanceMod();
   prl.staticObjViewDistSquared[0] = getStaticObjDistanceMod() * getStaticObjDistanceMod();
   for(S32 i=1; i<=CGraphicsProfile::MaxTinyStaticLevel; i++)
   {
      prl.staticObjViewDistSquared[i] = prl.staticObjViewDistSquared[i-1]/2;
   }

   // We only have to clip the mBox field
   AssertFatal(prl.mBox.isOverlapped(pTraversalRoot->getWorldBox()),
               "Error, prl box must overlap the traversal root");
   prl.mBox.min.setMax(pTraversalRoot->getWorldBox().min);
   prl.mBox.max.setMin(pTraversalRoot->getWorldBox().max);
   prl.mBox.min -= Point3F(5, 5, 5);
   prl.mBox.max += Point3F(5, 5, 5);
   AssertFatal(prl.mBox.isValidBox(), "Error, invalid query box created!");

   // Query against the container database, storing the objects in the
   //  potentially rendered list.  Note: we can query against the client
   //  container without testing, since only the client will be calling this
   //  function.  This is assured by the assert at the top...
   gClientContainer.findObjects(prl.mBox, objectMask, prlInsertionCallback, &prl);

   // Clear the object colors
   U32 i;
   for (i = 0; i < prl.mList.size(); i++)
      prl.mList[i]->setTraversalState( SceneObject::Pending );

   OnPrepRenderImage(smStateKey);

   for (i = 0; i < prl.mList.size(); i++)
      if( prl.mList[i]->getTraversalState() == SceneObject::Pending )
         treeTraverseVisit(prl.mList[i], state, smStateKey);

   if (currDepth < csmMaxTraversalDepth && state->mTransformPortals.size() != 0) {
      // Need to handle the transform portals here.
      //
      for (U32 i = 0; i < state->mTransformPortals.size(); i++) {
         const SceneState::TransformPortal& rPortal  = state->mTransformPortals[i];
         const SceneState::ZoneState&       rPZState = state->getZoneState(rPortal.globalZone);
         AssertFatal(rPZState.render == true, "Error, should not have returned a portal if the zone isn't rendering!");

         Point3F cameraPosition = state->getCameraPosition();
         rPortal.owner->transformPosition(rPortal.portalIndex, cameraPosition);

         // Setup the new modelview matrix...
         MatrixF oldMV = GFX->getWorldMatrix();
         MatrixF newMV;
         rPortal.owner->transformModelview(rPortal.portalIndex, oldMV, &newMV);

         // Here's the tricky bit.  We have to derive a new frustum and viewport
         //  from the portal, but we have to do it in the NEW coordinate space.
         //  Seems easiest to dump the responsibility on the object that was rude
         //  enough to make us go to all this trouble...
         F64   newFrustum[4];
         RectI newViewport;

         bool goodPortal = rPortal.owner->computeNewFrustum(rPortal.portalIndex, // which portal?
                                                            rPZState.frustum,    // old view params
                                                            state->mNearPlane,
                                                            state->mFarPlane,
                                                            rPZState.viewport,
                                                            newFrustum,          // new view params
                                                            newViewport,
                                                            state->mFlipCull);

         if (goodPortal == false) 
         {
            // Portal isn't visible, or is clipped out by the zone parameters...
            continue;
         }

         SceneState* newState = new SceneState(state,
                                               mCurrZoneEnd,
                                               newFrustum[0],
                                               newFrustum[1],
                                               newFrustum[2],
                                               newFrustum[3],
                                               state->mNearPlane,
                                               state->mFarPlane,
                                               newViewport,
                                               cameraPosition,
                                               newMV,
                                               mFogDistance,
                                               mVisibleDistance,
                                               mFogColor,
                                               mNumFogVolumes,
                                               mFogVolumes,
                                               smVisibleDistanceMod);
         newState->mFlipCull = state->mFlipCull ^ rPortal.flipCull;
         newState->setPortal(rPortal.owner, rPortal.portalIndex);

         GFX->pushWorldMatrix();

         GFX->setWorldMatrix( newMV );

         // Find the start zone.  Note that in a traversal descent, we start from
         //  the traversePoint of the transform portal, which is conveniently in
         //  world space...
         SceneObject* startObject;
         U32          startZone;
         findZone(rPortal.traverseStart, startObject, startZone);

         buildSceneTree(newState, startObject, startZone, currDepth + 1, objectMask);

         // Pop off the new modelview
         GFX->popWorldMatrix();

         // Push the subsidiary...
         state->mSubsidiaries.push_back(newState);
      }
   }

   // Ok, that's it!
}

bool terrCheck(TerrainBlock* pBlock,
               SceneObject*  pObj,
               const Point3F camPos);

void SceneGraph::treeTraverseVisit(SceneObject* obj,
                                   SceneState*  state,
                                   const U32    stateKey)
{
   if (obj->getNumCurrZones() == 0) 
   {
      obj->setTraversalState( SceneObject::Done );
      return;
   }

   PROFILE_START(treeTraverseVisit);

   AssertFatal(obj->getTraversalState() == SceneObject::Pending,
               "Wrong state for this stage of the traversal!");
   obj->setTraversalState(SceneObject::Working); //	TraversalState Not being updated correctly 'Gonzo'

   SceneObjectRef* pWalk = obj->mZoneRefHead;
   AssertFatal(pWalk != NULL, "Error, must belong to something!");
   while (pWalk) 
   {
      // Determine who owns this zone...
      SceneObject* pOwner = getZoneOwner(pWalk->zone);
      if( pOwner->getTraversalState() == SceneObject::Pending )
         treeTraverseVisit(pOwner, state, stateKey);

      pWalk = pWalk->nextInObj;
   }

   obj->setTraversalState( SceneObject::Done );

#ifndef NTJ_CLIENT
   // Cull it, but not if it's too low or there's no terrain to occlude against, or if it's global...
   if (getCurrentTerrain() != NULL && obj->getWorldBox().min.x > -1e5 && !obj->isGlobalBounds())
   {
      bool doTerrCheck = true;
      SceneObjectRef* pRef = obj->mZoneRefHead;
      while (pRef != NULL)
      {
         if (pRef->zone != 0)
         {
            doTerrCheck = false;
            break;
         }
         pRef = pRef->nextInObj;
      }

      if (doTerrCheck == true && terrCheck(getCurrentTerrain(), obj, state->getCameraPosition()) == true)
      {
         PROFILE_END(treeTraverseVisit);
         return;
      }
   }
#endif

   PROFILE_START(treeTraverseVisit_prepRenderImage);
   obj->prepRenderImage(state, stateKey, 0xFFFFFFFF);
   PROFILE_END(treeTraverseVisit_prepRenderImage);

   PROFILE_END(treeTraverseVisit);
}

bool terrCheck(TerrainBlock* pBlock,
               SceneObject*  pObj,
               const Point3F camPos)
{
   PROFILE_SCOPE(terrCheck);

   // Don't try to occlude globally bounded objects.
   if(pObj->isGlobalBounds())
   {
      return false;
   }

   Point3F localCamPos = camPos;
   pBlock->getWorldTransform().mulP(localCamPos);
   F32 height;
   pBlock->getHeight(Point2F(localCamPos.x, localCamPos.y), &height);
   bool aboveTerrain = (height <= localCamPos.z);

   // Don't occlude if we're below the terrain.  This prevents problems when
   //  looking out from underground bases...
   if (aboveTerrain == false)
   {
      return false;
   }

   const Box3F& oBox = pObj->getObjBox();
   F32 minSide = getMin(oBox.len_x(), oBox.len_y());
   if (minSide > 85.0f)
   {
      return false;
   }

   const Box3F& rBox = pObj->getWorldBox();
   Point3F ul(rBox.min.x, rBox.min.y, rBox.max.z);
   Point3F ur(rBox.min.x, rBox.max.y, rBox.max.z);
   Point3F ll(rBox.max.x, rBox.min.y, rBox.max.z);
   Point3F lr(rBox.max.x, rBox.max.y, rBox.max.z);

   pBlock->getWorldTransform().mulP(ul);
   pBlock->getWorldTransform().mulP(ur);
   pBlock->getWorldTransform().mulP(ll);
   pBlock->getWorldTransform().mulP(lr);

   Point3F xBaseL0_s = ul - localCamPos;
   Point3F xBaseL0_e = lr - localCamPos;
   Point3F xBaseL1_s = ur - localCamPos;
   Point3F xBaseL1_e = ll - localCamPos;

   static F32 checkPoints[3] = {0.75, 0.5, 0.25};
   RayInfo rinfo;
   for (U32 i = 0; i < 3; i++)
   {
      Point3F start = (xBaseL0_s * checkPoints[i]) + localCamPos;
      Point3F end   = (xBaseL0_e * checkPoints[i]) + localCamPos;

      if (pBlock->castRay(start, end, &rinfo))
         continue;

      pBlock->getHeight(Point2F(start.x, start.y), &height);
      if ((height <= start.z) == aboveTerrain)
         continue;

      start = (xBaseL1_s * checkPoints[i]) + localCamPos;
      end   = (xBaseL1_e * checkPoints[i]) + localCamPos;

      if (pBlock->castRay(start, end, &rinfo))
         continue;

      Point3F test = (start + end) * 0.5;
      if (pBlock->castRay(localCamPos, test, &rinfo) == false)
         continue;

      return true;
   }
   return false;
}
