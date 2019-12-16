//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "math/frustum.h"

#include "math/mMathFn.h"
#include "platform/profiler.h"


Frustum::Frustum()
{
}

Frustum::Frustum( const Frustum& frustum )
{
   set( frustum );
}

Frustum& Frustum::operator =( const Frustum& frustum )
{
   set( frustum );
   return *this;
}

void Frustum::set( const Frustum& frustum )
{
   mNearLeft = frustum.mNearLeft;
   mNearRight = frustum.mNearRight;
   mNearTop = frustum.mNearTop;
   mNearBottom = frustum.mNearBottom;
   mNearDist = frustum.mNearDist;
   mFarDist = frustum.mFarDist;

   mTransform = frustum.mTransform;
   mBounds = frustum.mBounds;

   dMemcpy( mPoints, frustum.mPoints, sizeof( mPoints ) );
   dMemcpy( mPlanes, frustum.mPlanes, sizeof( mPlanes ) );
}

void Frustum::set(   F32 fovInRadians, 
                     F32 aspectRatio, 
                     F32 nearDist, 
                     F32 farDist,
                     const MatrixF &transform )
{
   F32 left    = -nearDist * mTan( fovInRadians / 2.0f );
   F32 right   = -left;
   F32 bottom  = left / aspectRatio;
   F32 top     = -bottom;

   set( left, right, top, bottom, nearDist, farDist, transform );
}

void Frustum::set(   F32 nearLeft, 
                     F32 nearRight, 
                     F32 nearTop, 
                     F32 nearBottom, 
                     F32 nearDist, 
                     F32 farDist,
                     const MatrixF &transform )
{
   mTransform = transform;

   mNearLeft = nearLeft;
   mNearRight = nearRight;
   mNearTop = nearTop;
   mNearBottom = nearBottom;
   mNearDist = nearDist;
   mFarDist = farDist;

   _updatePlanes();
}

void Frustum::set( const MatrixF &projMat, bool normalize )
{ 
   // From "Fast Extraction of Viewing Frustum Planes from the World-View-Projection Matrix"
   // by Gil Gribb and Klaus Hartmann.
   //
   // http://www2.ravensoft.com/users/ggribb/plane%20extraction.pdf

	// Right clipping plane.
	mPlanes[ PlaneRight ].set( projMat[3] - projMat[0], 
                              projMat[7] - projMat[4], 
                              projMat[11] - projMat[8],
                              projMat[15] - projMat[12] );

	// Left clipping plane.
	mPlanes[ PlaneLeft ].set(  projMat[3] + projMat[0], 
                              projMat[7] + projMat[4], 
                              projMat[11] + projMat[8], 
                              projMat[15] + projMat[12] );

	// Bottom clipping plane.
	mPlanes[ PlaneBottom ].set(   projMat[3] + projMat[1], 
                                 projMat[7] + projMat[5], 
                                 projMat[11] + projMat[9], 
                                 projMat[15] + projMat[13] );

	// Top clipping plane.
	mPlanes[ PlaneTop ].set(   projMat[3] - projMat[1], 
                              projMat[7] - projMat[5], 
                              projMat[11] - projMat[9], 
                              projMat[15] - projMat[13] );

	// Near clipping plane
	mPlanes[ PlaneNear ].set(  projMat[3] + projMat[2], 
                              projMat[7] + projMat[6], 
                              projMat[11] + projMat[10],
                              projMat[15] + projMat[14] );

   // Far clipping plane.
   mPlanes[ PlaneFar ].set(   projMat[3] - projMat[2], 
                              projMat[7] - projMat[6], 
                              projMat[11] - projMat[10], 
                              projMat[15] - projMat[14] );

   if ( normalize )
   {
      for ( S32 i=0; i < PlaneCount; i++ )
         mPlanes[i].normalize();
   }

   /* // Create the corner points via plane intersections.
   mPlanes[ PlaneNear ].intersect( mPlanes[ PlaneTop ], mPlanes[ PlaneLeft ], &mPoints[ NearTopLeft ] );
   mPlanes[ PlaneNear ].intersect( mPlanes[ PlaneTop ], mPlanes[ PlaneRight ], &mPoints[ NearTopRight ] );
   mPlanes[ PlaneNear ].intersect( mPlanes[ PlaneBottom ], mPlanes[ PlaneLeft ], &mPoints[ NearBottomLeft ] );
   mPlanes[ PlaneNear ].intersect( mPlanes[ PlaneBottom ], mPlanes[ PlaneRight ], &mPoints[ NearBottomRight ] );
   mPlanes[ PlaneFar ].intersect( mPlanes[ PlaneTop ], mPlanes[ PlaneLeft ], &mPoints[ FarTopLeft ] );
   mPlanes[ PlaneFar ].intersect( mPlanes[ PlaneTop ], mPlanes[ PlaneRight ], &mPoints[ FarTopRight ] );
   mPlanes[ PlaneFar ].intersect( mPlanes[ PlaneBottom ], mPlanes[ PlaneLeft ], &mPoints[ FarBottomLeft ] );
   mPlanes[ PlaneFar ].intersect( mPlanes[ PlaneBottom ], mPlanes[ PlaneRight ], &mPoints[ FarBottomRight ] );
   */
   // Update the axis aligned bounding box.
   _updateBounds();
}

void Frustum::setNearDist( F32 nearDist )
{
   setNearFarDist( nearDist, mFarDist );
}

void Frustum::setFarDist( F32 farDist )
{
   setNearFarDist( mNearDist, farDist );
}

void Frustum::setNearFarDist( F32 nearDist, F32 farDist )
{
   // Extract the fov and aspect ratio.
   F32 fovInRadians = ( mAtan( mNearDist, mNearLeft ) * 2.0f ) - M_PI_F;
   F32 aspectRatio = mNearLeft / mNearBottom;

   // Store the inverted state.
   bool wasInverted = isInverted();

   // Recalculate the frustum.
   MatrixF xfm( mTransform ); 
   set( fovInRadians, aspectRatio, nearDist, farDist, xfm );

   // If the cull does not match then we need to invert.
   if ( wasInverted != isInverted() )
      invert();
}

void Frustum::_updatePlanes()
{
   PROFILE_SCOPE( Frustum_UpdatePlanes );

   const F32 farOverNear = mFarDist / mNearDist;

   // Build the frustum points in camera space first.
   mPoints[ CameraPosition ].zero();
   mPoints[ NearTopLeft ].set( mNearLeft, mNearDist, mNearTop );
   mPoints[ NearTopRight ].set( mNearRight, mNearDist, mNearTop );
   mPoints[ NearBottomLeft ].set( mNearLeft, mNearDist, mNearBottom );
   mPoints[ NearBottomRight ].set( mNearRight, mNearDist, mNearBottom );
   mPoints[ FarTopLeft ].set( mNearLeft * farOverNear, mFarDist, mNearTop * farOverNear );
   mPoints[ FarTopRight ].set( mNearRight * farOverNear, mFarDist, mNearTop * farOverNear );
   mPoints[ FarBottomLeft ].set( mNearLeft * farOverNear, mFarDist, mNearBottom * farOverNear );
   mPoints[ FarBottomRight ].set( mNearRight * farOverNear, mFarDist, mNearBottom * farOverNear );

   // Transform the points into the desired culling space.
   for ( S32 i=0; i < PlaneLeftCenter; i++ )
      mTransform.mulP( mPoints[i] );

   // Update the axis aligned bounding box from 
   // the newly transformed points.
   _updateBounds();

   // Finally build the planes.
   mPlanes[ PlaneLeft ].set(  mPoints[ CameraPosition ], 
                              mPoints[ NearTopLeft ], 
                              mPoints[ NearBottomLeft ] );

   mPlanes[ PlaneRight ].set( mPoints[ CameraPosition ], 
                              mPoints[ NearBottomRight ], 
                              mPoints[ NearTopRight ] );

   mPlanes[ PlaneTop ].set(   mPoints[ CameraPosition ], 
                              mPoints[ NearTopRight ], 
                              mPoints[ NearTopLeft ] );

   mPlanes[ PlaneBottom ].set(   mPoints[ CameraPosition ], 
                                 mPoints[ NearBottomLeft ], 
                                 mPoints[ NearBottomRight ] );

   mPlanes[ PlaneNear ].set(  mPoints[ NearTopLeft ], 
                              mPoints[ NearBottomLeft ], 
                              mPoints[ NearTopRight ] );

   mPlanes[ PlaneFar ].set(   mPoints[ FarTopLeft ], 
                              mPoints[ FarTopRight ], 
                              mPoints[ FarBottomLeft ] );

   // And now the center points... mostly just used in debug rendering.
   mPoints[ PlaneLeftCenter ] = (   mPoints[ NearTopLeft ] + 
                                    mPoints[ NearBottomLeft ] + 
                                    mPoints[ FarTopLeft ] + 
                                    mPoints[ FarBottomLeft ] ) / 4.0f;

   mPoints[ PlaneRightCenter ] = (  mPoints[ NearTopRight ] + 
                                    mPoints[ NearBottomRight ] + 
                                    mPoints[ FarTopRight ] + 
                                    mPoints[ FarBottomRight ] ) / 4.0f;

   mPoints[ PlaneTopCenter ] = ( mPoints[ NearTopLeft ] + 
                                 mPoints[ NearTopRight ] + 
                                 mPoints[ FarTopLeft ] + 
                                 mPoints[ FarTopRight ] ) / 4.0f;

   mPoints[ PlaneBottomCenter ] = ( mPoints[ NearBottomLeft ] + 
                                    mPoints[ NearBottomRight ] + 
                                    mPoints[ FarBottomLeft ] + 
                                    mPoints[ FarBottomRight ] ) / 4.0f;

   mPoints[ PlaneNearCenter ] = (   mPoints[ NearTopLeft ] + 
                                    mPoints[ NearTopRight ] + 
                                    mPoints[ NearBottomLeft ] + 
                                    mPoints[ NearBottomRight ] ) / 4.0f;

   mPoints[ PlaneFarCenter ] = ( mPoints[ FarTopLeft ] + 
                                 mPoints[ FarTopRight ] + 
                                 mPoints[ FarBottomLeft ] + 
                                 mPoints[ FarBottomRight ] ) / 4.0f;
}

void Frustum::_updateBounds()
{
   // Note this code depends on the order of the
   // enum in the header... don't change it.

   mBounds.min.set( mPoints[FirstCornerPoint] );
   mBounds.max.set( mPoints[FirstCornerPoint] );

   for ( S32 i=FirstCornerPoint + 1; i <= LastCornerPoint; i++ )
      mBounds.extend( mPoints[i] );      
}

void Frustum::invert()
{
   for( U32 i = 0; i < PlaneCount; i++ )
      mPlanes[i].invert();
}

bool Frustum::isInverted() const
{
   Point3F position;
   mTransform.getColumn( 3, &position );
   return mPlanes[ PlaneNear ].whichSide( position ) != PlaneF::Back;
}

void Frustum::scaleFromCenter( F32 scale )
{
   // Extract the fov and aspect ratio.
   F32 fovInRadians = ( mAtan( mNearDist, mNearLeft ) * 2.0f ) - M_PI_F;
   F32 aspectRatio = mNearLeft / mNearBottom;

   // Now move the near and far planes out.
   F32 halfDist = ( mFarDist - mNearDist ) / 2.0f;
   mNearDist   -= halfDist * ( scale - 1.0f );
   mFarDist    += halfDist * ( scale - 1.0f );

   // Setup the new scaled frustum.
   set( fovInRadians, aspectRatio, mNearDist, mFarDist, mTransform );
}

U32 Frustum::testPlanes( const Box3F &bounds, U32 planeMask, F32 expand ) const
{
   PROFILE_SCOPE( Frustum_TestPlanes );

   // This is based on the paper "A Faster Overlap Test for a Plane and a Bounding Box" 
   // by Kenny Hoff.  See http://www.cs.unc.edu/~hoff/research/vfculler/boxplane.html

   U32 retMask = 0;

   Point3F minPoint, maxPoint;
   F32 maxDot, minDot;
   U32 mask;

   // Note the planes are ordered left, right, near, 
   // far, top, bottom for getting early rejections
   // from the typical horizontal scene.
   for ( S32 i = 0; i < PlaneCount; i++ )
   {
      mask = ( 1 << i );

      if ( !( planeMask & mask ) )
         continue;

      const PlaneF& plane = mPlanes[i];

      if ( plane.x > 0 )
      {
         maxPoint.x = bounds.max.x;
         minPoint.x = bounds.min.x;
      }
      else
      {
         maxPoint.x = bounds.min.x;
         minPoint.x = bounds.max.x;
      }

      if ( plane.y > 0 )
      {
         maxPoint.y = bounds.max.y;
         minPoint.y = bounds.min.y;
      }
      else
      {
         maxPoint.y = bounds.min.y;
         minPoint.y = bounds.max.y;
      }

      if ( plane.z > 0 )
      {
         maxPoint.z = bounds.max.z;
         minPoint.z = bounds.min.z;
      }
      else
      {
         maxPoint.z = bounds.min.z;
         minPoint.z = bounds.max.z;
      }

      maxDot = mDot( maxPoint, plane );

      if ( maxDot <= -( plane.d + expand ) )
         return -1;

      minDot = mDot( minPoint, plane );

      if ( ( minDot + plane.d ) < 0.0f )
         retMask |= mask;
   }

   return retMask;
}

bool Frustum::intersects( const Box3F &bounds ) const
{
   PROFILE_SCOPE( Frustum_Intersects );

   // This is based on the paper "A Faster Overlap Test for a Plane and a Bounding Box" 
   // by Kenny Hoff.  See http://www.cs.unc.edu/~hoff/research/vfculler/boxplane.html

   Point3F maxPoint;
   F32 maxDot;

   // Note the planes are ordered left, right, near, 
   // far, top, bottom for getting early rejections
   // from the typical horizontal scene.
   for ( S32 i = 0; i < PlaneCount; i++ )
   {
      // This is pretty much as optimal as you can
      // get for a plane vs AABB test...
      // 
      // 4 comparisions
      // 3 multiplies
      // 2 adds
      // 1 negation
      //
      // It will early out as soon as it detects the
      // bounds is outside one of the planes.

      if ( mPlanes[i].x > 0 )
         maxPoint.x = bounds.max.x;
      else
         maxPoint.x = bounds.min.x;

      if ( mPlanes[i].y > 0 )
         maxPoint.y = bounds.max.y;
      else
         maxPoint.y = bounds.min.y;

      if ( mPlanes[i].z > 0 )
         maxPoint.z = bounds.max.z;
      else
         maxPoint.z = bounds.min.z;

      maxDot = mDot( maxPoint, mPlanes[ i ] );

      if ( maxDot <= -mPlanes[ i ].d )
         return false;
   }

   return true;
}

bool Frustum::pointInFrustum( const Point3F &point ) const
{
   PROFILE_SCOPE( Frustum_PointInFrustum );

   F32 maxDot;

   // Note the planes are ordered left, right, near, 
   // far, top, bottom for getting early rejections
   // from the typical horizontal scene.
   for ( S32 i = 0; i < PlaneCount; i++ )
   {
      const PlaneF &plane = mPlanes[ i ];

      // This is pretty much as optimal as you can
      // get for a plane vs point test...
      // 
      // 1 comparision
      // 2 multiplies
      // 1 adds
      //
      // It will early out as soon as it detects the
      // point is outside one of the planes.

      maxDot = mDot( point, plane ) + plane.d;
      if ( maxDot < 0.0f )
         return false;
   }

   return true;
}

bool Frustum::sphereInFrustum( const Point3F &center, F32 radius ) const
{
   PROFILE_SCOPE( Frustum_SphereInFrustum );

   F32 maxDot;

   // Note the planes are ordered left, right, near, 
   // far, top, bottom for getting early rejections
   // from the typical horizontal scene.
   for ( S32 i = 0; i < PlaneCount; i++ )
   {
      const PlaneF &plane = mPlanes[ i ];

      // This is pretty much as optimal as you can
      // get for a plane vs point test...
      // 
      // 1 comparision
      // 2 multiplies
      // 1 adds
      // 1 negation
      //
      // It will early out as soon as it detects the
      // point is outside one of the planes.

      maxDot = mDot( center, plane ) + plane.d;
      if ( maxDot < -radius )
         return false;
   }

   return true;
}

bool Frustum::boxInFrustum(const Box3F &box) const
{
	Point3F pt[8];
	pt[0] = box.min;
	pt[1] = box.max;
	pt[2].set(box.min.x, box.min.y, box.max.z);
	pt[3].set(box.min.x, box.max.y, box.min.z);
	pt[4].set(box.min.x, box.max.y, box.max.z);
	pt[5].set(box.max.x, box.min.y, box.min.z);
	pt[6].set(box.max.x, box.min.y, box.max.z);
	pt[7].set(box.max.x, box.max.y, box.min.z);


	bool noRender = true;
	for (U32 i = 0; i < PlaneCount; i++) 
	{
		noRender = true;
		for (U32 j=0; j< 8; j++)
		{
			if (mPlanes[i].whichSide(pt[j]) != PlaneF::Back) 
			{
				noRender = false;
				break;
			}
		}
		if (noRender)
		{
			return false;
		}
	}

	return true;
}

void Frustum::getCenterPoint( Point3F *center ) const
{
   center->set( mPoints[ FirstCornerPoint ] );

   for ( U32 i = FirstCornerPoint+1; i <= LastCornerPoint; i++ )
      *center += mPoints[ i ];

   *center /= (F32)CornerPointCount;
}

void Frustum::mul( const MatrixF& mat )
{
   mTransform.mul( mat );
   _updatePlanes();
}

void Frustum::mulL( const MatrixF& mat )
{
   MatrixF last( mTransform );
   mTransform.mul( mat, last );

   _updatePlanes();
}

void Frustum::getProjectionMatrix( MatrixF *proj ) const
{
   Point4F col;
   col.x = 2.0 * mNearDist / ( mNearRight - mNearLeft );
   col.y = 0.0;
   col.z = 0.0;
   col.w = 0.0;
   proj->setColumn( 0, col );

   col.x = 0.0;
   col.y = 2.0 * mNearDist / ( mNearTop - mNearBottom );
   col.z = 0.0;
   col.w = 0.0;
   proj->setColumn( 1, col );

   col.x = ( mNearRight + mNearLeft ) / ( mNearRight - mNearLeft );
   col.y = ( mNearTop + mNearBottom ) / ( mNearTop - mNearBottom );
   col.z = mFarDist / ( mFarDist - mNearDist );
   col.w = 1.0;
   proj->setColumn( 2, col );

   col.x = 0.0;
   col.y = 0.0;
   col.z = -( mFarDist * mNearDist / ( mFarDist - mNearDist ) );
   col.w = 0.0;
   proj->setColumn( 3, col );
}

