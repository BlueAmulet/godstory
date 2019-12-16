//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "util/frustrumCuller.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/sgUtil.h"
#include "gfx/gfxDevice.h"

void FrustrumCuller::init(SceneState *state)
{
   // Set up some general info.
   mSceneState = state;
   mFarDistance = state->getVisibleDistance();

   // Now determine the frustrum.
   F32 frustumParam[6];
   F64 realfrustumParam[6];

   GFX->getFrustum(&frustumParam[0], &frustumParam[1],
      &frustumParam[2], &frustumParam[3],
      &frustumParam[4], &frustumParam[5]);

   MatrixF camToObj = GFX->getWorldMatrix();
   camToObj.inverse();
   camToObj.getColumn(3, &mCamPos);

   // Cast so we can put them in the sg call.
   for(U32 i=0; i<6; i++)
      realfrustumParam[i] = frustumParam[i];

   Point3F osCamPoint(0,0,0);
   camToObj.mulP(osCamPoint);
   computeFrustumPlanes(  
      realfrustumParam,
      camToObj,
      osCamPoint,
      mClipPlane[4],
      mClipPlane[0],
      mClipPlane[1],
      mClipPlane[2],
      mClipPlane[3]);

   mNumClipPlanes = 4;

   if (state->mFlipCull) 
   {
      mClipPlane[0].neg();
      mClipPlane[1].neg();
      mClipPlane[2].neg();
      mClipPlane[3].neg();
      mClipPlane[4].neg();
   }

   AssertFatal(mNumClipPlanes <= MaxClipPlanes, "FrustrumCuller::init - got too many clip planes!");
}


const F32 FrustrumCuller::getBoxDistance(const Box3F &box) const
{
   Point3F vec;

   const Point3F &minPoint = box.min;
   const Point3F &maxPoint = box.max;

   if(mCamPos.z < minPoint.z)
      vec.z = minPoint.z - mCamPos.z;
   else if(mCamPos.z > maxPoint.z)
      vec.z = mCamPos.z - maxPoint.z;
   else
      vec.z = 0;

   if(mCamPos.x < minPoint.x)
      vec.x = minPoint.x - mCamPos.x;
   else if(mCamPos.x > maxPoint.x)
      vec.x = mCamPos.x - maxPoint.x;
   else
      vec.x = 0;

   if(mCamPos.y < minPoint.y)
      vec.y = minPoint.y - mCamPos.y;
   else if(mCamPos.y > maxPoint.y)
      vec.y = mCamPos.y - maxPoint.y;
   else
      vec.y = 0;

   return vec.len();
}

const S32 FrustrumCuller::testBoxVisibility(const Box3F &bounds, const S32 mask, const F32 expand) const
{
   S32 retMask = 0;

   Point3F minPoint, maxPoint;
   for(S32 i = 0; i < S32(mNumClipPlanes); i++)
   {
      if(mask & (1 << i))
      {
         if(mClipPlane[i].x > 0)
         {
            maxPoint.x = bounds.max.x;
            minPoint.x = bounds.min.x;
         }
         else
         {
            maxPoint.x = bounds.min.x;
            minPoint.x = bounds.max.x;
         }
         if(mClipPlane[i].y > 0)
         {
            maxPoint.y = bounds.max.y;
            minPoint.y = bounds.min.y;
         }
         else
         {
            maxPoint.y = bounds.min.y;
            minPoint.y = bounds.max.y;
         }

         if(mClipPlane[i].z > 0)
         {
            maxPoint.z = bounds.max.z;
            minPoint.z = bounds.min.z;
         }
         else
         {
            maxPoint.z = bounds.min.z;
            minPoint.z = bounds.max.z;
         }


         F32 maxDot = mDot(maxPoint, mClipPlane[i]);
         F32 minDot = mDot(minPoint, mClipPlane[i]);
         F32 planeD = mClipPlane[i].d;

         if(maxDot <= -(planeD + expand))
            return -1;

         if(minDot <= -planeD)
            retMask |= (1 << i);
      }
   }

   // Check the far distance as well.
   if(mask & FarSphereMask)
   {
      F32 squareDistance = getBoxDistance(bounds);

      // Reject stuff outside our visible range...
      if(squareDistance >= mFarDistance)
         return -1;

      // See if the box potentially hits the far sphere. (Sort of assumes a square box!)
      if(squareDistance + bounds.len_x() + bounds.len_y() > mFarDistance)
         retMask |= FarSphereMask;
   }

   return retMask;
}

void FrustrumCuller::computeFrustumPlanes(const F64      frustumParameters[6],
                              const MatrixF& worldSpaceToObjectSpace,
                              const Point3F& wsCamPoint,
                              PlaneF&        outFarPlane,
                              PlaneF&        outXMinPlane,
                              PlaneF&        outXMaxPlane,
                              PlaneF&        outYMinPlane,
                              PlaneF&        outYMaxPlane)
{
   // Create the object space clipping planes...
   Point3F ul(frustumParameters[0] * 1000.0, frustumParameters[4] * 1000.0, frustumParameters[3] * 1000.0);
   Point3F ur(frustumParameters[1] * 1000.0, frustumParameters[4] * 1000.0, frustumParameters[3] * 1000.0);
   Point3F ll(frustumParameters[0] * 1000.0, frustumParameters[4] * 1000.0, frustumParameters[2] * 1000.0);
   Point3F lr(frustumParameters[1] * 1000.0, frustumParameters[4] * 1000.0, frustumParameters[2] * 1000.0);
   Point3F farPlane(0, frustumParameters[5], 0);

   worldSpaceToObjectSpace.mulP(ul);
   worldSpaceToObjectSpace.mulP(ur);
   worldSpaceToObjectSpace.mulP(ll);
   worldSpaceToObjectSpace.mulP(lr);
   worldSpaceToObjectSpace.mulP(farPlane);

   outFarPlane.set(farPlane, wsCamPoint - farPlane);
   outXMinPlane.set(wsCamPoint, ul, ll);
   outXMaxPlane.set(wsCamPoint, lr, ur);
   outYMinPlane.set(wsCamPoint, ur, ul);
   outYMaxPlane.set(wsCamPoint, ll, lr);
}