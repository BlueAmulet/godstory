//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "core/bitStream.h"
#include "ts/tsShapeInstance.h"
#include "console/consoleInternal.h"
#include "console/consoleTypes.h"
#include "sceneGraph/lightInfo.h"
#include "sceneGraph/lightManager.h"
#include "T3D/fx/particleEmitter.h"
#include "T3D/shapeBase.h"
#include "T3D/projectile.h"
#include "T3D/gameConnection.h"
#include "math/mathIO.h"
#include "T3D/debris.h"
#include "math/mathUtils.h"
#include "sim/netObject.h"
#include "sfx/sfxSystem.h"
#include "ts/TSShapeRepository.h"
#include "Effects/EffectPacket.h"

#include "util/aniThread.h"

// <Edit> [3/4/2009 joy] 精简mountImage相关内容
//----------------------------------------------------------------------------

ShapeBase::MountedImage::MountedImage()
{
   shapeInstance = 0;
   shapeName = NULL;
   mountNode = -1;
   mountPoint = 0;
   mountPointSelf = 0;
   mountTransform.identity();
   skinNameHandle = NetStringHandle();
   isAnimated = true;
   isUpper = false;
   isRender = false;
   imageEPItem = 0;
}

ShapeBase::MountedImage::~MountedImage()
{
#ifdef USE_MULTITHREAD_ANIMATE
   if(	g_MultThreadWorkMgr && g_MultThreadWorkMgr->isEnable())
	   g_MultThreadWorkMgr->addInDeleteingShapeInsList(shapeInstance);
   else
	   delete shapeInstance;
#else
   delete shapeInstance;
#endif
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Any item with an item image is selectable

bool ShapeBase::mountImage(StringTableEntry shapeName,U32 imageSlot,NetStringHandle &skinNameHandle,StringTableEntry mountPoint, StringTableEntry mountPointSelf)
{
   AssertFatal(imageSlot<MaxMountedImages,"Out of range image slot");

   // <Edit> [3/9/2009 joy] 必须先放到stringtable中，其他情况也如此
   // <Edit> [3/17/2010 joy] NULL除外
   if(shapeName)
      shapeName = StringTable->insert(shapeName);
   MountedImage& image = mMountedImageList[imageSlot];
   if (image.shapeName) {
      if ((image.shapeName == shapeName) && (image.skinNameHandle == skinNameHandle)) {
         // Image already loaded
         return true;
      }
   }
   //
   setImage(imageSlot,shapeName,skinNameHandle,mountPoint,mountPointSelf);

   return true;
}

bool ShapeBase::unmountImage(U32 imageSlot)
{
   AssertFatal(imageSlot<MaxMountedImages,"Out of range image slot");

	bool returnValue = false;
   MountedImage& image = mMountedImageList[imageSlot];
   if (image.shapeName)
   {
      NetStringHandle temp;
      setImage(imageSlot,0, temp,0,0);
      returnValue = true;
   }

   return returnValue;
}


//----------------------------------------------------------------------------

StringTableEntry ShapeBase::getMountedImage(U32 imageSlot)
{
   AssertFatal(imageSlot<MaxMountedImages,"Out of range image slot");

   return mMountedImageList[imageSlot].shapeName;
}


ShapeBase::MountedImage* ShapeBase::getImageStruct(U32 imageSlot)
{
   return &mMountedImageList[imageSlot];
}

bool ShapeBase::isImageMounted(StringTableEntry shapeName)
{
   shapeName = StringTable->insert(shapeName);
   for (U32 i = 0; i < MaxMountedImages; i++)
      if (shapeName == mMountedImageList[i].shapeName)
         return true;
   return false;
}

S32 ShapeBase::getMountSlot(StringTableEntry shapeName)
{
	shapeName = StringTable->insert(shapeName);
   for (U32 i = 0; i < MaxMountedImages; i++)
      if (shapeName == mMountedImageList[i].shapeName)
         return i;
   return -1;
}

NetStringHandle ShapeBase::getImageSkinTag(U32 imageSlot)
{
   MountedImage& image = mMountedImageList[imageSlot];
   return image.skinNameHandle;
}

//----------------------------------------------------------------------------

void ShapeBase::getMountTransform(StringTableEntry mountPoint,MatrixF* mat)
{
   if (!mShapeInstance || !mShapeInstance->getShape())
   {
	   *mat = mObjToWorld;
       return;
   }

   // Returns mount point to world space transform
   //if (mountPoint < ShapeBaseData::NumMountPoints) {
      S32 ni = mTSSahpeInfo->getNodeIndex(mountPoint);
      if (ni != -1 && ni < mShapeInstance->getNodeTransforms().size()) {
         MatrixF mountTransform = mShapeInstance->getNodeTransforms()[ni];
         const Point3F& scale = getScale();

         // The position of the mount point needs to be scaled.
         Point3F position = mountTransform.getPosition();
         position.convolve( scale );
         mountTransform.setPosition( position );

         // Also we would like the object to be scaled to the model.
         mat->mul(mObjToWorld, mountTransform);
         return;
      }
   //}
   *mat = mObjToWorld;
}

void ShapeBase::getLocalMountTransform(StringTableEntry mountPoint,MatrixF* mat)
{
   if (!mShapeInstance || !mShapeInstance->getShape())
   {
	   mat->identity();
       return;
   }

   // Returns mount point to world space transform
   //if (mountPoint < ShapeBaseData::NumMountPoints) {
      S32 ni = mTSSahpeInfo->getNodeIndex(mountPoint);
      if (ni != -1 && ni < mShapeInstance->getNodeTransforms().size()) {
         MatrixF mountTransform = mShapeInstance->getNodeTransforms()[ni];
         const Point3F& scale = getScale();

         // The position of the mount point needs to be scaled.
         Point3F position = mountTransform.getPosition();
         position.convolve( scale );
         mountTransform.setPosition( position );
		 *mat = mountTransform;
         return;
      }
   //}
   mat->identity();
}

void ShapeBase::getImageTransform(U32 imageSlot,MatrixF* mat)
{
   // Image transform in world space
   MountedImage& image = mMountedImageList[imageSlot];
   if (image.shapeName) {
         MatrixF nmat;
         getMountTransform(image.mountPoint,&nmat);
         mat->mul(nmat,image.mountTransform);
   }
   else
      *mat = mObjToWorld;
}

void ShapeBase::getImageTransform(U32 imageSlot,S32 node,MatrixF* mat)
{
   // Muzzle transform in world space
   MountedImage& image = mMountedImageList[imageSlot];
   if (image.shapeName && image.shapeInstance) {
      if (node != -1 && node < mShapeInstance->getNodeTransforms().size()) {
         MatrixF imat;
         getImageTransform(imageSlot,&imat);
         mat->mul(imat,image.shapeInstance->getNodeTransforms()[node]);
      }
      else
         getImageTransform(imageSlot,mat);
   }
   else
      *mat = mObjToWorld;
}

void ShapeBase::getImageTransform(U32 imageSlot,StringTableEntry nodeName,MatrixF* mat)
{
   getImageTransform( imageSlot, getNodeIndex( imageSlot, nodeName ), mat );
}

//----------------------------------------------------------------------------

void ShapeBase::getRenderMountTransform(StringTableEntry mountPoint,MatrixF* mat)
{
   if (!mShapeInstance || !mShapeInstance->getShape())
   {
	   *mat = mRenderObjToWorld;
       return;
   }

   // Returns mount point to world space transform
   //if (mountPoint < ShapeBaseData::NumMountPoints) {
      S32 ni = mTSSahpeInfo->getNodeIndex(mountPoint);
      if (ni != -1 && ni < mShapeInstance->getNodeTransforms().size()) {
         MatrixF mountTransform = mShapeInstance->getNodeTransforms()[ni];
         const Point3F& scale = getScale();

         // The position of the mount point needs to be scaled.
         Point3F position = mountTransform.getPosition();
         position.convolve( scale );
         mountTransform.setPosition( position );

         // Also we would like the object to be scaled to the model.
         mountTransform.scale( scale );
         mat->mul(getRenderTransform(), mountTransform);
         return;
      }
   //}
   *mat = getRenderTransform();
}

void ShapeBase::getRenderMountTransform_(StringTableEntry mountPoint,MatrixF* mat)
{
   if (!mShapeInstance || !mShapeInstance->getShape())
   {
	   *mat = mRenderObjToWorld;
       return;
   }

   // Returns mount point to world space transform
   //if (mountPoint < ShapeBaseData::NumMountPoints) {
      S32 ni = mTSSahpeInfo->getNodeIndex(mountPoint);
      if (ni != -1 && ni < mShapeInstance->getNodeTransforms().size()) {
         MatrixF mountTransform = mShapeInstance->getNodeTransforms()[ni];
         const Point3F& scale = getScale();

         // The position of the mount point needs to be scaled.
         Point3F position = mountTransform.getPosition();
         position.convolve( scale );
         mountTransform.setPosition( position );

         // without scale
         // mountTransform.scale( scale );
         mat->mul(getRenderTransform(), mountTransform);
         return;
      }
   //}
   *mat = getRenderTransform();
}

void ShapeBase::getRenderNodeTransform(StringTableEntry nodeName,MatrixF* mat)
{
    if (!mShapeInstance || !mShapeInstance->getShape())
	{
		*mat = mRenderObjToWorld;
        return;
	}

	// Returns point to world space transform
	//S32 ni = mShapeInstance->getShape()->findNode(nodeName);
	S32 ni = mTSSahpeInfo->getNodeIndex(nodeName);
	if (ni != -1 && ni < mShapeInstance->getNodeTransforms().size()) {
		MatrixF mountTransform = mShapeInstance->getNodeTransforms()[ni];
		const Point3F& scale = getScale();

		// The position of the node point needs to be scaled.
		Point3F position = mountTransform.getPosition();
		position.convolve( scale );
		mountTransform.setPosition( position );

		// Also we would like the object to be scaled to the model.
		mountTransform.scale( scale );
		mat->mul(getRenderTransform(), mountTransform);
		return;
	}
	*mat = getRenderTransform();
}

void ShapeBase::getRenderNodeTransform(S32 node,MatrixF* mat)
{
    if (!mShapeInstance || !mShapeInstance->getShape())
	{
		*mat = mRenderObjToWorld;
        return;
	}

	// Returns point to world space transform
	if (node != -1 && node < mShapeInstance->getNodeTransforms().size()) {
		MatrixF mountTransform = mShapeInstance->getNodeTransforms()[node];
		const Point3F& scale = getScale();

		// The position of the node point needs to be scaled.
		Point3F position = mountTransform.getPosition();
		position.convolve( scale );
		mountTransform.setPosition( position );

		// Also we would like the object to be scaled to the model.
		mountTransform.scale( scale );
		mat->mul(getRenderTransform(), mountTransform);
		return;
	}
	*mat = getRenderTransform();
}


void ShapeBase::getRenderImageTransform(U32 imageSlot,MatrixF* mat)
{
   // Image transform in world space
   MountedImage& image = mMountedImageList[imageSlot];
   if (image.shapeName && image.shapeInstance) {
         MatrixF nmat(true);
         getRenderMountTransform(image.mountPoint,&nmat);
		 // <Edit> [3/4/2009 joy] Image取得image的链接点
		 if(image.mountNode != -1 && image.mountNode < image.shapeInstance->getNodeTransforms().size())
		 {
			 MatrixF mountTrans = image.shapeInstance->getNodeTransforms()[image.mountNode];
			 image.mountTransform = mountTrans.inverse();
		 }
		 else
			 image.mountTransform.identity();
         mat->mul(nmat,image.mountTransform);
   }
   else
      *mat = getRenderTransform();
}

void ShapeBase::getRenderImageTransform(U32 imageSlot,S32 node,MatrixF* mat)
{
   // Muzzle transform in world space
   MountedImage& image = mMountedImageList[imageSlot];
   if (image.shapeName && image.shapeInstance) {
      if (node != -1 && node < image.shapeInstance->getNodeTransforms().size()) {
         MatrixF imat(true);
         getRenderImageTransform(imageSlot,&imat);
         mat->mul(imat,image.shapeInstance->getNodeTransforms()[node]);
      }
      else
         getRenderImageTransform(imageSlot,mat);
   }
   else
      *mat = getRenderTransform();
}

void ShapeBase::getRenderImageTransform(U32 imageSlot,StringTableEntry nodeName,MatrixF* mat)
{
   getRenderImageTransform( imageSlot, getNodeIndex( imageSlot, nodeName ), mat );
}


//----------------------------------------------------------------------------

S32 ShapeBase::getNodeIndex(U32 imageSlot,StringTableEntry nodeName)
{
   MountedImage& image = mMountedImageList[imageSlot];

   if (image.shapeName && image.shapeInstance && image.shapeInstance->getShape())
      return image.shapeInstance->getShape()->findNode(nodeName);
   else
      return -1;
}

// Modify muzzle if needed to aim at whatever is straight in front of eye.  Let the
// caller know if we actually modified the result.
//bool ShapeBase::getCorrectedAim(const MatrixF& muzzleMat, VectorF* result)
//{
//   const F32 pullInD = 6.0;
//   const F32 maxAdjD = 500;
//
//   VectorF  aheadVec(0, maxAdjD, 0);
//
//   MatrixF  eyeMat;
//   Point3F  eyePos;
//   getEyeTransform(&eyeMat);
//   eyeMat.getColumn(3, &eyePos);
//   eyeMat.mulV(aheadVec);
//   Point3F  aheadPoint = (eyePos + aheadVec);
//
//   // Should we check if muzzle point is really close to eye?  Does that happen?
//   Point3F  muzzlePos;
//   muzzleMat.getColumn(3, &muzzlePos);
//
//   Point3F  collidePoint;
//   VectorF  collideVector;
//   disableCollision();
//      RayInfo rinfo;
//      if (getContainer()->castRay(eyePos, aheadPoint, STATIC_COLLISION_MASK|DAMAGEABLE_MASK, &rinfo))
//         collideVector = ((collidePoint = rinfo.point) - eyePos);
//      else
//         collideVector = ((collidePoint = aheadPoint) - eyePos);
//   enableCollision();
//
//   // For close collision we want to NOT aim at ground since we're bending
//   // the ray here as it is.  But we don't want to pop, so adjust continuously.
//   F32   lenSq = collideVector.lenSquared();
//   if (lenSq < (pullInD * pullInD) && lenSq > 0.04)
//   {
//      F32   len = mSqrt(lenSq);
//      F32   mid = pullInD;    // (pullInD + len) / 2.0;
//      // This gives us point beyond to focus on-
//      collideVector *= (mid / len);
//      collidePoint = (eyePos + collideVector);
//   }
//
//   VectorF  muzzleToCollide = (collidePoint - muzzlePos);
//   lenSq = muzzleToCollide.lenSquared();
//   if (lenSq > 0.04)
//   {
//      muzzleToCollide *= (1 / mSqrt(lenSq));
//      * result = muzzleToCollide;
//      return true;
//   }
//   return false;
//}
//
//----------------------------------------------------------------------------

void ShapeBase::updateMass()
{
      mMass = mDataBlock->mass;
      mOneOverMass = 1 / mMass;
}

//----------------------------------------------------------------------------

void ShapeBase::setImage(U32 imageSlot, StringTableEntry shapeName, NetStringHandle &skinNameHandle, StringTableEntry mountPoint, StringTableEntry mountPointSelf)
{
   AssertFatal(imageSlot<MaxMountedImages,"Out of range image slot");

   if(shapeName && shapeName[0])
      shapeName = StringTable->insert(shapeName);
   else
      shapeName = NULL;
   MountedImage& image = mMountedImageList[imageSlot];

   // If we already have this datablock...
   if (image.shapeName == shapeName) {
      // Change the skin handle if necessary.
      if (image.skinNameHandle != skinNameHandle) {
         if (!isGhost()) {
            // Serverside, note the skin handle and tell the client.
            image.skinNameHandle = skinNameHandle;
            setMaskBits(ImageMaskN << imageSlot);
         }
         else {
            // Clientside, do the reskin.
            image.skinNameHandle = skinNameHandle;
            if (image.shapeInstance) {
               image.shapeInstance->reSkin(skinNameHandle);
            }
         }
      }
      return;
   }

   // Mark that updates are happenin'.
   setMaskBits(ImageMaskN << imageSlot);

   // Stop anything currently going on with the image.
   resetImageSlot(imageSlot);

   //记录信息
   image.skinNameHandle = skinNameHandle;
   image.shapeName      = shapeName;
   image.mountPoint     = mountPoint;
   image.mountPointSelf = mountPointSelf;
   image.animThread = 0;
   image.isRender = true;

   // If we're just unselecting the current shape without swapping
   // in a new one, then bail.
   if (!shapeName || !shapeName[0]) 
   {
      return;
   }
     
   TSShapeInfo* pInfo = g_TSShapeRepository.GetTSShapeInfo(shapeName);

   if (!pInfo)
       return;

   if (isServerObject()) 
		return;
    
   // Otherwise, init the new shape.
   image.shapeInstance = new TSShapeInstance(pInfo->GetShape(), isClientObject());

   if (isClientObject()) 
   {
       if (image.shapeInstance) 
       {
           image.shapeInstance->cloneMaterialList();
           image.shapeInstance->reSkin(image.skinNameHandle);
       }
	   image.mountNode = pInfo->GetShape()->findNode(image.mountPointSelf);
   }

   if (isGhost()) 
   {
       image.isAnimated = true;
       if (image.isAnimated) 
       {
           image.animThread = image.shapeInstance->addThread();
           if(image.animThread)
               image.shapeInstance->setTimeScale(image.animThread,0);
           else
               image.isAnimated = false;
       }
   }

   if(mTSSahpeInfo && mTSSahpeInfo->GetShape())
   {
       S32 id = mTSSahpeInfo->GetShape()->findNode(mountPoint);
       S32 nameIndex = mTSSahpeInfo->GetShape()->findName("midNode");
       if(id < 0 || nameIndex < 0)
           image.isUpper = false;
       else
       {
           if(mTSSahpeInfo->GetShape()->isUpper(id, nameIndex))
               image.isUpper = true;
           else
               image.isUpper = false;
       }
   }
   else
       AssertFatal(isServerObject(), "DoSetImage : main shape error!");
   refreshImageAction(imageSlot);

   // <Edit> [10/26/2009 joy] 设置残影
#ifdef NTJ_CLIENT
   char linkName[256];
   if(imageSlot == GameObject::Slot_LeftHand)
   {
       for (S32 i=0; i<(EdgeLinkpoints>>1); ++i)
       {
           dSprintf(linkName, sizeof(linkName), "Link%02dPoint", i+4);
           mEdgePts[i] = pInfo->GetShape()->findNode(linkName);
       }
       if(!mEdgeBlur[EB_Left_A] && mEdgePts[0] != -1 && mEdgePts[1] != -1)
       {
           mEdgeBlur[EB_Left_A] = new CEdgeBlur();
           mEdgeBlur[EB_Left_A]->Create(EdgeBlurPts);
           mEdgeBlur[EB_Left_A]->SetTexture(mEdgeBlurTextureName.getString());
       }
       else
           SAFE_DELETE(mEdgeBlur[EB_Left_A]);

       if(!mEdgeBlur[EB_Left_B] && mEdgePts[2] != -1 && mEdgePts[3] != -1)
       {
           mEdgeBlur[EB_Left_B] = new CEdgeBlur();
           mEdgeBlur[EB_Left_B]->Create(EdgeBlurPts);
           mEdgeBlur[EB_Left_B]->SetTexture(mEdgeBlurTextureName.getString());
       }
       else
           SAFE_DELETE(mEdgeBlur[EB_Left_B]);
   }
   else if(imageSlot == GameObject::Slot_RightHand)
   {
       for (S32 i=(EdgeLinkpoints>>1); i<EdgeLinkpoints; ++i)
       {
           dSprintf(linkName, sizeof(linkName), "Link%02dPoint", i);
           mEdgePts[i] = pInfo->GetShape()->findNode(linkName);
       }
       if(!mEdgeBlur[EB_Right_A] && mEdgePts[4] != -1 && mEdgePts[5] != -1)
       {
           mEdgeBlur[EB_Right_A] = new CEdgeBlur();
           mEdgeBlur[EB_Right_A]->Create(EdgeBlurPts);
           mEdgeBlur[EB_Right_A]->SetTexture(mEdgeBlurTextureName.getString());
       }
       else
           SAFE_DELETE(mEdgeBlur[EB_Right_A]);

       if(!mEdgeBlur[EB_Right_B] && mEdgePts[6] != -1 && mEdgePts[7] != -1)
       {
           mEdgeBlur[EB_Right_B] = new CEdgeBlur();
           mEdgeBlur[EB_Right_B]->Create(EdgeBlurPts);
           mEdgeBlur[EB_Right_B]->SetTexture(mEdgeBlurTextureName.getString());
       }
       else
           SAFE_DELETE(mEdgeBlur[EB_Right_B]);
   }
#endif

   // Update the mass for the mount object.
   updateMass();

   // Done.
}

//----------------------------------------------------------------------------

void ShapeBase::resetImageSlot(U32 imageSlot)
{
   AssertFatal(imageSlot<MaxMountedImages,"Out of range image slot");

   // Clear out current image
   MountedImage& image = mMountedImageList[imageSlot];

   if(isClientObject() && image.shapeInstance)
   {
#ifdef USE_MULTITHREAD_ANIMATE
	   if(	g_MultThreadWorkMgr->isEnable())
		   g_MultThreadWorkMgr->addInDeleteingShapeInsList(image.shapeInstance);
	   else
		   delete image.shapeInstance;
#else
	   delete image.shapeInstance;
#endif
	   image.shapeInstance = NULL;
   }
   image.shapeName = NULL;
   image.mountNode = -1;
   image.mountPoint = 0;
   image.mountPointSelf = 0;
   image.mountTransform.identity();

   image.skinNameHandle = NetStringHandle();
   image.isRender = false;

   // clear EPs
   if(image.imageEPItem)
      g_EffectPacketContainer.removeEffectPacket(image.imageEPItem);

#ifdef NTJ_CLIENT
   if(imageSlot == GameObject::Slot_LeftHand)
   {
	   SAFE_DELETE(mEdgeBlur[EB_Left_A]);
	   SAFE_DELETE(mEdgeBlur[EB_Left_B]);
   }
   if(imageSlot == GameObject::Slot_RightHand)
   {
	   SAFE_DELETE(mEdgeBlur[EB_Right_A]);
	   SAFE_DELETE(mEdgeBlur[EB_Right_B]);
   }
#endif
   updateMass();
}

//----------------------------------------------------------------------------

void ShapeBase::updateImageAnimation(U32 imageSlot, F32 dt)
{
   if (!mMountedImageList[imageSlot].shapeName || !mMountedImageList[imageSlot].shapeName[0])
      return;

   MountedImage& image = mMountedImageList[imageSlot];

   if (image.animThread && image.isAnimated && image.shapeInstance)
      image.shapeInstance->advanceTime(dt,image.animThread);
}

void ShapeBase::hideImage()
{
	if(!isProperlyAdded())
		return;

	// 现在只需要隐藏左右手
	for (S32 i=GameObject::Slot_LeftHand; i<=GameObject::Slot_RightHand; ++i)
	{
		if (mMountedImageList[i].shapeInstance && mMountedImageList[i].shapeInstance->getShape() && mMountedImageList[i].isRender != isRenderImage(i))
		{
			mMountedImageList[i].isRender = !mMountedImageList[i].isRender;
			g_EffectPacketContainer.hideEffect(mMountedImageList[i].imageEPItem, !mMountedImageList[i].isRender);
		}
	}
}
