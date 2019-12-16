//-----------------------------------------------
// Synapse Gaming - Lighting Code Pack
// Copyright ?Synapse Gaming 2003 - 2005
// Written by John Kabus
//
// Overview:
//  Code from the Lighting Pack's (PowerEngine Lighting Kit)
//  lighting system, which was modified for use
//  with Constructor.
//-----------------------------------------------

#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "lightingSystem/synapseGaming/interior/sgPlanarMap.h"
#include "lightingSystem/synapseGaming/sgLightingModel.h"
#include "interior/interiorInstance.h"

U32 sgPlanarLightMap::sgCurrentOccluderMaskId = 0;


void sgCalculateLightMapTransforms(InteriorInstance *intinst, const Interior::Surface &surf, MatrixF &objspace, MatrixF &tanspace)
{
   tanspace.setColumn(0, surf.T);
   tanspace.setColumn(1, surf.B);
   tanspace.setColumn(2, surf.normal);
   tanspace.inverse();

   objspace = intinst->getRenderWorldTransform();
}

ColorF sgCalculateLightMapVector(Point3F &lightingnormal, MatrixF &objspace, MatrixF &tangentspace)
{
   Point3F p = lightingnormal;
   objspace.mulV(p);
   tangentspace.mulV(p);
   ColorF ret(p.x, p.y, p.z, 0);
   return ret;
}

inline bool sgDegenerateTriangle(const Point3F &k, const Point3F &j, const Point3F &l)
{
   const F32 ax = k.x-j.x;
   const F32 ay = k.y-j.y;
   const F32 az = k.z-j.z;
   const F32 bx = l.x-j.x;
   const F32 by = l.y-j.y;
   const F32 bz = l.z-j.z;
   const F32 x = ay*bz - az*by;
   const F32 y = az*bx - ax*bz;
   const F32 z = ax*by - ay*bx;
   return (x*x + y*y + z*z) == 0.0f;
}

inline F32 sgGetDistanceSquared(const Point3F &linepointa,
                                const Point3F &linepointb, const Point3F &point)
{
   Point3F vect = linepointb - linepointa;
   F32 dist = vect.lenSquared();
   if(dist <= 0.0f)
      return 100000.0f;

   F32 tang = ((point.x + linepointa.x) * (linepointb.x + linepointa.x)) +
      ((point.y + linepointa.y) * (linepointb.y + linepointa.y)) +
      ((point.z + linepointa.z) * (linepointb.z + linepointa.z));

   tang /= dist;
   if(tang >= 1.0f)
   {
      vect = linepointb - point;
      return vect.lenSquared();
   }

   if(tang <= 0.0f)
   {
      vect = linepointa - point;
      return vect.lenSquared();
   }

   vect = linepointa + (vect * tang);
   vect = vect - point;
   return vect.lenSquared();
}

bool sgPlanarLightMap::sgCastRay(Point3F s, Point3F e, SceneObject *obj, Interior *detail, InteriorSimpleMesh *sm, sgOccluder &occluderinfo)
{
   obj->getWorldTransform().mulP(s);
   obj->getWorldTransform().mulP(e);
   s.convolveInverse(obj->getScale());
   e.convolveInverse(obj->getScale());

   if(sm)
   {
      // expects points in interior space...
      if(!sgShadowObjects::sgCastRayStaticMesh(s, e, sm))
         return false;

      occluderinfo.sgObject = sm;
      occluderinfo.sgSurface = SG_NULL_SURFACE;
   }
   else if(detail)
   {
      RayInfo ri;
      // not set by all objects...
      ri.face = -1;
      if(!detail->castRay(s, e, &ri))
         return false;

      occluderinfo.sgObject = obj;
      occluderinfo.sgSurface = ri.face;
   }
   else
   {
      RayInfo ri;
      // not set by all objects...
      ri.face = -1;
      if(!obj->castRay(s, e, &ri))
         return false;

      occluderinfo.sgObject = obj;
      occluderinfo.sgSurface = ri.face;
   }

   return true;
}

bool sgPlanarLightMap::sgIsValidOccluder(const sgOccluder &occluderinfo, hash_multimap<void *, sgOccluder> &validoccluders, bool isinnerlexel)
{
   hash_multimap<void *, sgOccluder> *entry = validoccluders.find(calculateCRC(&occluderinfo, sizeof(occluderinfo)));

   if(isinnerlexel)
   {
      entry->info = occluderinfo;
      return true;
   }

   if((entry->info.sgObject == occluderinfo.sgObject) && (entry->info.sgSurface == occluderinfo.sgSurface))
      return true;

   return false;
}

void sgPlanarLightMap::sgSetupLighting()
{
   // stats
   elapsedTimeAggregate time = elapsedTimeAggregate(sgStatistics::sgInteriorSurfaceSetupTime);
   sgStatistics::sgInteriorSurfaceSetupCount++;

   // get tranformed points...
   U32 windingcount = triStrip.size();
   if(windingcount < 3)
      return;

   // create plane from tri...
   bool foundplane = false;
   PlaneF testplane;
   Point3F norm = (triStrip[0].sgNorm + triStrip[1].sgNorm + triStrip[2].sgNorm) * 0.3333f;

   for(U32 i=2; i<triStrip.size(); i++)
   {
      const Point3F &a = triStrip[i-2].sgVert;
      const Point3F &b = triStrip[i-1].sgVert;
      const Point3F &c = triStrip[i].sgVert;

      if(sgDegenerateTriangle(a, b, c))
         continue;

      foundplane = true;
      testplane.set(a, b, c);
      break;
   }

   if(!foundplane)
      return;

   bool flippedtestplane = (mDot(testplane, norm) < 0.0f);
   bool flippedplane = (mDot(surfacePlane, norm) < 0.0f);

   if(flippedtestplane)
      sgFlippedWindings = true;

   if(flippedplane)
   {
      surfacePlane.x *= -1.0f;
      surfacePlane.y *= -1.0f;
      surfacePlane.z *= -1.0f;
      surfacePlane.d *= -1.0f;
   }

   // get the axis info...
   F64 smax = 0.0f;
   F64 tmax = 0.0f;
   for(S32 i=0; i<3; i++)
   {
      F64 s = mFabs(sgLightMapSVector[i]);
      F64 t = mFabs(sgLightMapTVector[i]);
      //if((s > 0.0f) && (t > 0.0f))
      //	continue;
      if(s > smax)
      {
         sgSAxis = i;
         smax = s;
      }
      if(t > tmax)
      {
         sgTAxis = i;
         tmax = t;
      }
   }

   // try again...
   if(sgSAxis == sgTAxis)
   {
      // find the axis with the minimal diff (the bad axis)...
      U32 a = (sgSAxis + 1) % 3;
      U32 b = (sgSAxis + 2) % 3;
      F64 absa = mFabs(mFabs(sgLightMapSVector[sgSAxis]) - mFabs(sgLightMapSVector[a]));
      F64 absb = mFabs(mFabs(sgLightMapSVector[sgSAxis]) - mFabs(sgLightMapSVector[b]));
      smax = getMin(absa, absb);

      a = (sgTAxis + 1) % 3;
      b = (sgTAxis + 2) % 3;
      absa = mFabs(mFabs(sgLightMapTVector[sgTAxis]) - mFabs(sgLightMapTVector[a]));
      absb = mFabs(mFabs(sgLightMapTVector[sgTAxis]) - mFabs(sgLightMapTVector[b]));
      tmax = getMin(absa, absb);

      S32 avoidaxis = -1;
      S32 *newaxis = NULL;
      Point3D *vector = NULL;
      if(smax < tmax)
      {
         avoidaxis = sgTAxis;
         newaxis = &sgSAxis;
         vector = &sgLightMapSVector;
      }
      else
      {
         avoidaxis = sgSAxis;
         newaxis = &sgTAxis;
         vector = &sgLightMapTVector;
      }

      // find the max on the bad axis that's not on the original axis...
      F64 max = 0.0f;
      for(S32 i=0; i<3; i++)
      {
         if(i == avoidaxis)
            continue;
         F64 val = mFabs((*vector)[i]);
         if(val > max)
         {
            (*newaxis) = i;
            max = val;
         }
      }
   }

   AssertFatal(((sgSAxis != -1) && (sgTAxis != -1) && (sgSAxis != sgTAxis)),
      "Unable to determine axis info!");


   if (sgInteriorStaticMesh || sgInteriorInstance->useSmoothLighting())
   {
      Vector<sgSmoothingTri> tris;
      tris.increment(windingcount - 2);

      // these are tristrips!!!
      for(U32 t=0; t<tris.size(); t++)
      {
         if(sgDegenerateTriangle(triStrip[t].sgVert, triStrip[t+1].sgVert, triStrip[t+2].sgVert))
         {
            tris.decrement(1);
            continue;
         }

         for(U32 v=0; v<3; v++)
         {
            U32 w = v + t;
            U32 k;

            // reverse winding?
            if(t & 0x1)
               k = ((v+2) % 3) + t;
            else
               k = ((v+1) % 3) + t;

            sgSmoothingVert &vect = tris[t].sgVerts[v];

            const Point3F &pointw  = triStrip[w].sgVert;
            const Point3F &pointk  = triStrip[k].sgVert;
            const Point3F &normalw = triStrip[w].sgNorm;
            const Point3F &tangw   = triStrip[w].sgTang;
            const Point3F &binormw = triStrip[w].sgBinorm;
            const Point2F &textw   = triStrip[w].sgText;

            vect.sgVert   = pointw;
            vect.sgVect   = pointk - pointw;
            vect.sgNorm   = normalw;
            vect.sgTang   = tangw;
            vect.sgBinorm = binormw;
            vect.sgText   = textw;

            AssertFatal((vect.sgVect.lenSquared() > 0.0f), "!");

            // could break this out for speed...
            if((t == 0) && (v == 0))
               sgSurfaceBox = Box3F(pointw, pointw);
            else
            {
               sgSurfaceBox.max.setMax(pointw);
               sgSurfaceBox.min.setMin(pointw);
            }
         }

         sgBuildTangentAndBiNormal(tris[t]);
         sgBuildDerivatives(tris[t]);
      }

      sgBuildLexels(tris);
   }
   else
   {
      // loop through the texels...
	   // sort by inner and outer...
	   const U32 lexelmax = sgHeight * sgWidth;
	   sgInnerLexels.clear();
	   sgInnerLexels.reserve(lexelmax);
	   sgOuterLexels.clear();
	   sgOuterLexels.reserve(lexelmax);

      sgSmoothingVert *tristripptr = triStrip.address();

      // Unwind our triangle strip into a fan
      Vector<U32> unwound(windingcount);
      unwound.push_back(0);
      
      for (U32 i = 1; i < windingcount; i += 2)
         unwound.push_back(i);
      for (U32 i = ((windingcount - 1) & (~0x1)); i > 0; i -= 2)
         unwound.push_back(i);

      if (windingcount > 0)
         sgSurfaceBox = Box3F(tristripptr[0].sgVert, tristripptr[0].sgVert);

	   bool outer;
	   Point3F vec2;
	   Point3F cross;
	   Point3D pos = sgWorldPosition;
	   Point3D run = sgLightMapSVector * (F32)sgWidth;
   	
	   bool halfsize = !sgLightManager::sgAllowFullLightMaps();
	   U32 lmscalemask = sgLightManager::sgGetLightMapScale() - 1;
   	
	   for(U32 y=0; y<sgHeight; y++)
	   {
		   if(halfsize && (y & lmscalemask))
		   {
			   pos += sgLightMapTVector;
			   continue;
		   }

		   for(U32 x=0; x<sgWidth; x++)
		   {
			   if(halfsize && (x & lmscalemask))
			   {
				   pos += sgLightMapSVector;
				   continue;
			   }

            sgStatistics::sgInteriorLexelCount++;

            Point3F pos32(pos.x, pos.y, pos.z);

			   outer = false;
   			
			   for(U32 i=0; i<windingcount; i++)
			   {
               U32 w = unwound[i];
               U32 k = unwound[(i + 1) % windingcount];

               // could break this out for speed...
               sgSurfaceBox.max.setMax(tristripptr[w].sgVert);
               sgSurfaceBox.min.setMin(tristripptr[w].sgVert);

               Point3F sgVect = tristripptr[k].sgVert - tristripptr[w].sgVert;

				   vec2 = pos32 - tristripptr[w].sgVert;
				   mCross(vec2, sgVect, &cross);

               // don't check against 0.0f, otherwise lexels on the edge become "inner"
               // and cause shadowing from adjacent brushes...
               if(sgFlippedWindings)
               {
                  if(mDot(surfacePlane, cross) > -0.001f)
                  {
                     // nope...
					      outer = true;
					      break;
                  }
               }
               else
               {
                  if(mDot(surfacePlane, cross) < 0.001f)
                  {
                     // nope...
					      outer = true;
					      break;
                  }
               }
			   }

            // find respective vector...
            if(outer)
            {
               sgOuterLexels.increment();
               sgLexel &temp = sgOuterLexels.last();
               temp.shadowTestOnly = false;
               temp.lmPos.x = x;
               temp.lmPos.y = y;
               temp.worldPos = pos;
               temp.normal = surfacePlane;

               if (windingcount > 0)
               {
                  temp.tangent = tristripptr[0].sgTang;
                  temp.biNormal = tristripptr[0].sgBinorm;
               }
            }
            else
            {
               sgInnerLexels.increment();
               sgLexel &temp = sgInnerLexels.last();
               temp.shadowTestOnly = false;
               temp.lmPos.x = x;
               temp.lmPos.y = y;
               temp.worldPos = pos;
               temp.normal = surfacePlane;

               if (windingcount > 0)
               {
                  temp.tangent = tristripptr[0].sgTang;
                  temp.biNormal = tristripptr[0].sgBinorm;
               }
            }

			   pos += sgLightMapSVector;
		   }

		   pos -= run;
		   pos += sgLightMapTVector;
	   }

      // add lexels at the verts to help out skinny geometry...
      for(U32 i=0; i<windingcount - 2; i++)
      {
         // if few inner lexels exist fake one for shadow testing...
         if(sgInnerLexels.size() < 5)
         {
            Point3F norm = tristripptr[i].sgNorm + tristripptr[i+1].sgNorm + tristripptr[i+2].sgNorm;
            norm.normalize();

            // center
            sgInnerLexels.increment();
            sgLexel &tempa = sgInnerLexels.last();
            tempa.shadowTestOnly = true;
            tempa.lmPos.x = 0;
            tempa.lmPos.y = 0;
            tempa.worldPos = (tristripptr[i].sgVert + tristripptr[i+1].sgVert + tristripptr[i+2].sgVert) * 0.3333333f;
            tempa.normal = norm;
            tempa.tangent.set(0.0f, 1.0f, 0.0f);
            tempa.biNormal.set(0.0f, 0.0f, 1.0f);
         }

         // vert 0
         sgInnerLexels.increment();
         sgLexel &tempb = sgInnerLexels.last();
         tempb.shadowTestOnly = true;
         tempb.lmPos.x = 0;
         tempb.lmPos.y = 0;
         tempb.worldPos = tristripptr[i].sgVert;
         tempb.normal = tristripptr[i].sgNorm;
         tempb.tangent.set(0.0f, 1.0f, 0.0f);
         tempb.biNormal.set(0.0f, 0.0f, 1.0f);

         // last 2
         if(i == windingcount - 3)
         {
            for(U32 v=1; v<3; v++)
            {
               sgInnerLexels.increment();
               sgLexel &temp = sgInnerLexels.last();
               temp.shadowTestOnly = true;
               temp.lmPos.x = 0;
               temp.lmPos.y = 0;
               temp.worldPos = tristripptr[i+v].sgVert;
               temp.normal = tristripptr[i+v].sgNorm;
               temp.tangent.set(0.0f, 1.0f, 0.0f);
               temp.biNormal.set(0.0f, 0.0f, 1.0f);
            }
         }
      }
   }

   // stats...
   sgStatistics::sgInteriorSurfaceIncludedCount++;
   sgStatistics::sgInteriorSurfaceSmoothedCount++;
   sgStatistics::sgInteriorSurfaceSmoothedLexelCount +=
      sgInnerLexels.size() + sgOuterLexels.size();
}

void sgPlanarLightMap::sgBuildTangentAndBiNormal(sgSmoothingTri &tri)
{
   sgSmoothingVert &va = tri.sgVerts[0];
   sgSmoothingVert &vb = tri.sgVerts[1];
   sgSmoothingVert &vc = tri.sgVerts[2];

   float x1 = vb.sgVert.x - va.sgVert.x;
   float x2 = vc.sgVert.x - va.sgVert.x;
   float y1 = vb.sgVert.y - va.sgVert.y;
   float y2 = vc.sgVert.y - va.sgVert.y;
   float z1 = vb.sgVert.z - va.sgVert.z;
   float z2 = vc.sgVert.z - va.sgVert.z;

   float s1 = vb.sgText.x - va.sgText.x;
   float s2 = vc.sgText.x - va.sgText.x;
   float t1 = vb.sgText.y - va.sgText.y;
   float t2 = vc.sgText.y - va.sgText.y;

   float r = 1.0F / (s1 * t2 - s2 * t1);

   Point3F tang(
      (t2 * x1 - t1 * x2) * r, 
      (t2 * y1 - t1 * y2) * r, 
      (t2 * z1 - t1 * z2) * r);

   Point3F binorm(
      (s1 * x2 - s2 * x1) * r, 
      (s1 * y2 - s2 * y1) * r, 
      (s1 * z2 - s2 * z1) * r);

   // convert to prime...
   const Point3F &norm = surfacePlane;
   tang = tang - (mDot(tang, norm) * norm);
   binorm = binorm - (mDot(binorm, norm) * norm) - (mDot(tang, binorm) * tang);

   tang.normalizeSafe();
   binorm.normalizeSafe();

   tri.sgTang = tang;
   tri.sgBiNorm = binorm;
}

void sgPlanarLightMap::sgBuildDerivatives(sgSmoothingTri &tri)
{
   // build the derivatives for linear interpolation...
   const sgSmoothingVert &va = tri.sgVerts[0];
   const sgSmoothingVert &vb = tri.sgVerts[1];
   const sgSmoothingVert &vc = tri.sgVerts[2];

   const F32 vsac = va.sgVert[sgSAxis] - vc.sgVert[sgSAxis];
   const F32 vsbc = vb.sgVert[sgSAxis] - vc.sgVert[sgSAxis];
   const F32 vtac = va.sgVert[sgTAxis] - vc.sgVert[sgTAxis];
   const F32 vtbc = vb.sgVert[sgTAxis] - vc.sgVert[sgTAxis];
   const F32 spartial = 1.0f / ((vsbc * vtac) - (vsac * vtbc));
   const F32 tpartial = 1.0f / ((vsac * vtbc) - (vsbc * vtac));

   for(S32 c=0; c<3; c++)
   {
      const F32 nac = va.sgNorm[c] - vc.sgNorm[c];
      const F32 nbc = vb.sgNorm[c] - vc.sgNorm[c];
      tri.sgNormSDerivative[c] = ((nbc * vtac) - (nac * vtbc)) * spartial;
      tri.sgNormTDerivative[c] = ((nbc * vsac) - (nac * vsbc)) * tpartial;

      const F32 tac = va.sgTang[c] - vc.sgTang[c];
      const F32 tbc = vb.sgTang[c] - vc.sgTang[c];
      tri.sgTangSDerivative[c] = ((tbc * vtac) - (tac * vtbc)) * spartial;
      tri.sgTangTDerivative[c] = ((tbc * vsac) - (tac * vsbc)) * tpartial;

      const F32 bnac = va.sgBinorm[c] - vc.sgBinorm[c];
      const F32 bnbc = vb.sgBinorm[c] - vc.sgBinorm[c];
      tri.sgBinormSDerivative[c] = ((bnbc * vtac) - (bnac * vtbc)) * spartial;
      tri.sgBinormTDerivative[c] = ((bnbc * vsac) - (bnac * vsbc)) * tpartial;
   }
}

void sgPlanarLightMap::sgBuildLexels(const Vector<sgSmoothingTri> &tris)
{
   // loop through the texels...
   // sort by inner and outer...
   const U32 lexelmax = sgHeight * sgWidth;
   //const U32 buffersize = lexelmax * sizeof(sgLexel);
   sgInnerLexels.clear();
   sgInnerLexels.reserve(lexelmax);
   sgOuterLexels.clear();
   sgOuterLexels.reserve(lexelmax);

   // this is faster than Vector[]...
   sgSmoothingTri *trisptr = tris.address();

   bool outer;
   Point3F vec2;
   Point3F cross;
   Point3D pos = sgWorldPosition;
   Point3D run = sgLightMapSVector * sgWidth;
   sgSmoothingTri *container;

   bool halfsize = !sgLightManager::sgAllowFullLightMaps();
   U32 lmscalemask = sgLightManager::sgGetLightMapScale() - 1;

   for(U32 y=0; y<sgHeight; y++)
   {
      if(halfsize && (y & lmscalemask))
      {
         pos += sgLightMapTVector;
         continue;
      }

      for(U32 x=0; x<sgWidth; x++)
      {
         if(halfsize && (x & lmscalemask))
         {
            pos += sgLightMapSVector;
            continue;
         }

         sgStatistics::sgInteriorLexelCount++;

         Point3F pos32(pos.x, pos.y, pos.z);

         // find containing tri...
         outer = true;
         container = NULL;
         for(U32 t=0; t<tris.size(); t++)
         {
            for(U32 v=0; v<3; v++)
            {
               vec2 = pos32 - trisptr[t].sgVerts[v].sgVert;
               mCross(vec2, trisptr[t].sgVerts[v].sgVect, &cross);

               // don't check against 0.0f, otherwise lexels on the edge become "inner"
               // and cause shadowing from adjacent brushes...
               if(sgFlippedWindings)
               {
                  if(mDot(surfacePlane, cross) > -0.001f)
                     break;//not this one...
               }
               else
               {
                  if(mDot(surfacePlane, cross) < 0.001f)
                     break;//not this one...
               }

               if(v == 2)
               {
                  //must be this one...
                  outer = false;
                  container = &trisptr[t];
               }
            }

            if(container)
               break;//already found...
         }

         // use line test to find a container for the outer lexels if smoothing...
         // get the normal...
         Point3F normal = surfacePlane;
         Point3F tang(0.0f, 1.0f, 0.0f);
         Point3F binorm(0.0f, 0.0f, 1.0f);

         if(!container)
         {
            // set a default...
            container = trisptr;

            F32 maxdist = 1000000.0f;
            for(U32 t=0; t<tris.size(); t++)
            {
               for(U32 v=0; v<3; v++)
               {
                  U32 v2 = (v + 1) % 3;
                  const F32 d = sgGetDistanceSquared(
                     trisptr[t].sgVerts[v].sgVert,
                     trisptr[t].sgVerts[v2].sgVert, pos32);

                  if(d < maxdist)
                  {
                     maxdist = d;
                     container = &trisptr[t];
                  }
               }
            }
         }

         // get the normal...
         Point3F posrelative = pos32 - container->sgVerts[0].sgVert;

         normal = container->sgVerts[0].sgNorm;
         normal += (container->sgNormSDerivative * posrelative[sgSAxis]) +
            (container->sgNormTDerivative * posrelative[sgTAxis]);
         tang = container->sgVerts[0].sgTang;
         tang += (container->sgTangSDerivative * posrelative[sgSAxis]) +
            (container->sgTangTDerivative * posrelative[sgTAxis]);
         binorm = container->sgVerts[0].sgBinorm;
         binorm += (container->sgBinormSDerivative * posrelative[sgSAxis]) +
            (container->sgBinormTDerivative * posrelative[sgTAxis]);

         normal.normalize();
         tang.normalize();
         binorm.normalize();

         // find respective vector...
         if(outer)
         {
            sgOuterLexels.increment();
            sgLexel &temp = sgOuterLexels.last();
            temp.shadowTestOnly = false;
            temp.lmPos.x = x;
            temp.lmPos.y = y;
            temp.worldPos = pos32;
            temp.normal = normal;
            temp.tangent = tang;
            temp.biNormal = binorm;
         }
         else
         {
            sgInnerLexels.increment();
            sgLexel &temp = sgInnerLexels.last();
            temp.shadowTestOnly = false;
            temp.lmPos.x = x;
            temp.lmPos.y = y;
            temp.worldPos = pos32;
            temp.normal = normal;
            temp.tangent = tang;
            temp.biNormal = binorm;
         }

         pos += sgLightMapSVector;
      }

      pos -= run;
      pos += sgLightMapTVector;
   }

   // add lexels at the verts to help out skinny geometry...
   for(U32 i=0; i<tris.size(); i++)
   {
      // if few inner lexels exist fake one for shadow testing...
      if(sgInnerLexels.size() < 5)
      {
         Point3F norm = tris[i].sgVerts[0].sgNorm + tris[i].sgVerts[1].sgNorm + tris[i].sgVerts[2].sgNorm;
         norm.normalize();

         // center
         sgInnerLexels.increment();
         sgLexel &tempa = sgInnerLexels.last();
         tempa.shadowTestOnly = true;
         tempa.lmPos.x = 0;
         tempa.lmPos.y = 0;
         tempa.worldPos = (tris[i].sgVerts[0].sgVert + tris[i].sgVerts[1].sgVert + tris[i].sgVerts[2].sgVert) * 0.3333333f;
         tempa.normal = norm;
         tempa.tangent.set(0.0f, 1.0f, 0.0f);
         tempa.biNormal.set(0.0f, 0.0f, 1.0f);
      }

      // vert 0
      sgInnerLexels.increment();
      sgLexel &tempb = sgInnerLexels.last();
      tempb.shadowTestOnly = true;
      tempb.lmPos.x = 0;
      tempb.lmPos.y = 0;
      tempb.worldPos = tris[i].sgVerts[0].sgVert;
      tempb.normal = tris[i].sgVerts[0].sgNorm;
      tempb.tangent.set(0.0f, 1.0f, 0.0f);
      tempb.biNormal.set(0.0f, 0.0f, 1.0f);

      // last 2
      if(i == (tris.size() - 1))
      {
         for(U32 v=1; v<3; v++)
         {
            sgInnerLexels.increment();
            sgLexel &temp = sgInnerLexels.last();
            temp.shadowTestOnly = true;
            temp.lmPos.x = 0;
            temp.lmPos.y = 0;
            temp.worldPos = tris[i].sgVerts[v].sgVert;
            temp.normal = tris[i].sgVerts[v].sgNorm;
            temp.tangent.set(0.0f, 1.0f, 0.0f);
            temp.biNormal.set(0.0f, 0.0f, 1.0f);
         }
      }
   }
}

void sgPlanarLightMap::sgCalculateLighting(sgLightInfo *light)
{
   // stats...
   sgStatistics::sgInteriorSurfaceIlluminationCount++;
   elapsedTimeAggregate time = elapsedTimeAggregate(sgStatistics::sgInteriorLexelIlluminationTime);


   // setup zone info...
   bool isinzone = false;
   if(light->sgDiffuseRestrictZone || light->sgAmbientRestrictZone)
   {
      for(U32 z=0; z<sgInteriorInstance->getNumCurrZones(); z++)
      {
         S32 zone = sgInteriorInstance->getCurrZone(z);
         if(zone > 0)
         {
            if((zone == light->sgZone[0]) || (zone == light->sgZone[1]))
            {
               isinzone = true;
               break;
            }
         }
      }

      if(!isinzone && (sgInteriorSurface != SG_NULL_SURFACE))
      {
         S32 zone = sgInteriorInstance->getSurfaceZone(sgInteriorSurface, sgInteriorDetail);
         if((light->sgZone[0] == zone) || (light->sgZone[1] == zone))
            isinzone = true;
      }

      if(!isinzone && sgInteriorStaticMesh)
      {
         // need to find zone(s)...
      }
   }

   // allow what?
   bool allowdiffuse = (!light->sgDiffuseRestrictZone) || isinzone;
   bool allowambient = (!light->sgAmbientRestrictZone) || isinzone;

   // should I bother?
   if((!allowdiffuse) && (!allowambient))
      return;

   // first get lighting model...
   sgLightingModel &model = sgLightingModelManager::sgGetLightingModel(
      light->sgLightingModelName);
   model.sgSetState(light);

   // test for early out...
   if(!model.sgCanIlluminate(sgSurfaceBox))
   {
      model.sgResetState();
      return;
   }

   // this is slow, so do it after the early out...
   model.sgInitStateLM();

   // setup shadow objects...
   Box3F lightvolume = sgSurfaceBox;
   if(light->mType == LightInfo::Vector)
   {
      for(U32 i=0; i<triStrip.size(); i++)
      {
         Point3F lightpos = triStrip[i].sgVert +
            (SG_STATIC_LIGHT_VECTOR_DIST * light->mDirection * -1.0f);
         lightvolume.max.setMax(lightpos);
         lightvolume.min.setMin(lightpos);
      }
   }
   else
   {
      lightvolume.max.setMax(light->mPos);
      lightvolume.min.setMin(light->mPos);
   }

   // build a list of potential shadow casters...
   if(light->sgCastsShadows && sgLightManager::sgAllowShadows())
      sgGetIntersectingObjects(lightvolume, sgInteriorInstance);


   // stats...
   sgStatistics::sgInteriorSurfaceIlluminatedCount++;
   sgStatistics::sgInteriorLexelIlluminationCount += sgInnerLexels.size() + sgOuterLexels.size();


   hash_multimap<void *, sgOccluder> shadowingsurfaces;

   // put rayCast into lighting mode?
   bool faststaticmeshshadows = !sgLightManager::sgAllowFullLightMaps();
   if(!faststaticmeshshadows || sgInteriorStaticMesh)
      Interior::smLightingCastRays = true;
   else
      Interior::smLightingCastRays = false;

   for(U32 i=0; i<sgPlanarLightMap::sglpCount; i++)
   {
      // set which list...
      U32 templexelscount;
      sgLexel *templexels;
      if(i == sgPlanarLightMap::sglpInner)
      {
         templexelscount = sgInnerLexels.size();
         templexels = sgInnerLexels.address();
      }
      else
      {
         templexelscount = sgOuterLexels.size();
         templexels = sgOuterLexels.address();
      }

      for(U32 ii=0; ii<templexelscount; ii++)
      {
         // get the current lexel...
         const sgLexel &lexel = templexels[ii];
         const U32 x = lexel.lmPos.x;
         const U32 y = lexel.lmPos.y;
         const U32 lmindex = ((y * sgWidth) + x);

         // early out...
         // not likely to happen, but great potential...
         ColorF &col = sgTexels->sgData[lmindex];
         if((col.red >= 1.0f) && (col.green >= 1.0f) && (col.blue >= 1.0f))
            continue;

         // too often unset, must do these here...
         ColorF diffuse(0.0f, 0.0f, 0.0f);
         ColorF ambient(0.0f, 0.0f, 0.0f);
         VectorF lightingnormal(0.0f, 0.0f, 0.0f);
         model.sgLightingLM(lexel.worldPos, lexel.normal, diffuse, ambient, lightingnormal);


         // testing...
         //diffuse = ColorF(lexel.normal.x, lexel.normal.y, lexel.normal.z);
         /*if(i == sgPlanarLightMap::sglpOuter)
         diffuse = ColorF(1.0, 0, 1.0);
         else
         diffuse = ColorF(0.0, 1.0, 1.0);

         if((x & 0x1) == (y & 0x1))
         diffuse.green = 0;
         else
         diffuse.blue = 0;
         */
         /*Point3F p = lexel.worldPos;
         diffuse = ColorF(mFabs(p.x - mFloor(p.x)),
         mFabs(p.y - mFloor(p.y)),
         mFabs(p.z - mFloor(p.z)));*/

         /*if(sgUseSmoothing)
         {
         Point3F col = lexel.normal * 0.5 + Point3F(0.5, 0.5, 0.5);//ColorF(1, 0, 0);
         ambient = ColorF(col.x, col.y, col.z);
         //ambient = ColorF(1, 0, 0);
         }*/

         //if(badtexgen)
         //   diffuse = ColorF(1.0, 1.0, 0.0);


         // increment the occluder mask...
         sgCurrentOccluderMaskId++;
         if(sgCurrentOccluderMaskId == 0)
            sgCurrentOccluderMaskId++;

         // in the event the diffuse is too small or
         // the alpha occluders attenuated the value too much...
         if(allowdiffuse && ((diffuse.red > SG_MIN_LEXEL_INTENSITY) || (diffuse.green > SG_MIN_LEXEL_INTENSITY) || (diffuse.blue > SG_MIN_LEXEL_INTENSITY)))
         {
            // stats
            sgStatistics::sgInteriorLexelDiffuseCount++;

            // step four: check for shadows...
            bool shadowed = false;

            if(light->sgCastsShadows && sgLightManager::sgAllowShadows())
            {
               // set light pos for shadows...
               Point3F lightpos = light->mPos;
               if(light->mType == LightInfo::Vector)
               {
                  lightpos = SG_STATIC_LIGHT_VECTOR_DIST * light->mDirection * -1.0f;
                  lightpos = lexel.worldPos + lightpos;
               }

               // potential problem with this setup is the rare but possible
               // occurrence of outer lexel light leaks from raycast hitting an
               // interior surface that's not in the shadow list but also has valid
               // occluders between it and the lexel (ie: stops the ray before
               // finding the valid occluders).
               //
               // possible solutions:
               // -Ctor style shadow system
               // -custom raycast that collects ALL occluding surfaces
               //
               // both solutions are heavyweight - first see if the problem occurs
               // in the wild, this may be a theoretical problem.

               // start by finding any shadow casters
               sgOccluder info;
               for(U32 o=0; o<sgIntersectingSceneObjects.size(); o++)
               {
                  if(!sgCastRay(lightpos, lexel.worldPos, sgIntersectingSceneObjects[o], NULL, NULL, info))
                     continue;

                  // no valid testing on non-self objects...

                  shadowed = true;
                  break;
               }

               // if false then self interior test (include sms at the end)...
               if(!shadowed)
               {
                  if(sgCastRay(lightpos, lexel.worldPos, sgInteriorInstance, sgInteriorDetail, NULL, info))
                  {
                     if(info.sgSurface != sgInteriorSurface)
                     {
                        sgAdjacent adj = sgAreAdjacent(sgInteriorSurface, info.sgSurface);
                        if(adj == sgaFalse)
                           shadowed = true;
                        else if(adj == sgaMaybe)
                        {
                           if(sgIsValidOccluder(info, shadowingsurfaces, (i == sglpInner)))
                              shadowed = true;
                        }
                     }
                  }

                  if(!shadowed && !faststaticmeshshadows)
                  {
                     for(U32 sm=0; sm<sgInteriorDetail->getStaticMeshCount(); sm++)
                     {
                        InteriorSimpleMesh *mesh = (InteriorSimpleMesh *)sgInteriorDetail->getStaticMesh(sm);
                        if(mesh == sgInteriorStaticMesh)
                           continue;

                        if(!sgCastRay(lightpos, lexel.worldPos, sgInteriorInstance, NULL, mesh, info))
                           continue;

                        if(!sgIsValidOccluder(info, shadowingsurfaces, (i == sglpInner)))
                           continue;

                        shadowed = true;
                        break;
                     }
                  }
               }

               // if false then try the sm list...
               if(!shadowed && !faststaticmeshshadows)
               {
                  sgStaticMeshBVPT::objectList list;
                  sgIntersectingStaticMeshObjects.collectObjectsClipped(lightpos, lexel.worldPos, list);

                  for(U32 o=0; o<list.size(); o++)
                  {
                     sgStaticMeshInfo &sminfo = list[o];
                     if(!sgCastRay(lightpos, lexel.worldPos, sminfo.sgInteriorInstance, NULL, sminfo.sgStaticMesh, info))
                        continue;

                     // no valid testing on non-self objects...

                     shadowed = true;
                     break;
                  }
               }

               // if false then self sm test...
               if(!shadowed && sgInteriorStaticMesh && !faststaticmeshshadows)
               {
                  // reverse cast direction...
                  if(sgCastRay(lexel.worldPos, lightpos, sgInteriorInstance, NULL, sgInteriorStaticMesh, info))
                  {
                     if(sgIsValidOccluder(info, shadowingsurfaces, (i == sglpInner)))
                        shadowed = true;
                  }
               }
            }

            if(!lexel.shadowTestOnly && !shadowed)
            {
               // cool, got this far so either there was no occluder
               // or it was an alpha and eventually we found a light
               // ray segment that didn't get blocked...
               //
               // step five: apply the lighting to the light map...
               sgDirty = true;
               sgTexels->sgData[lmindex] += diffuse;

               Point3F vec = lightingnormal;

               // transform to tangent space...
               MatrixF mat;
               mat.identity();
               mat.setRow(0, lexel.tangent);
               mat.setRow(1, lexel.biNormal);
               mat.setRow(2, lexel.normal);
               mat.mulV(vec);

               // modulate by intensity...
               Point3F color(diffuse.red, diffuse.green, diffuse.blue);
               F32 intensity = color.len();
               vec *= intensity;

               sgNexels->sgData[lmindex] += ColorF(vec.x, vec.y, vec.z, intensity);
            }
         }

         if(!lexel.shadowTestOnly && allowambient && ((ambient.red > SG_MIN_LEXEL_INTENSITY) || (ambient.green > SG_MIN_LEXEL_INTENSITY) || (ambient.blue > SG_MIN_LEXEL_INTENSITY)))
         {
            sgDirty = true;
            sgTexels->sgData[lmindex] += ambient;

            Point3F vec = Point3F(0.0f, 0.0f, 1.0f);

            // modulate by intensity...
            Point3F color(ambient.red, ambient.green, ambient.blue);
            F32 intensity = color.len();
            vec *= intensity;

            sgNexels->sgData[lmindex] += ColorF(vec.x, vec.y, vec.z, intensity);
         }
      }
   }

   // put rayCast back to normal mode...
   Interior::smLightingCastRays = false;

   model.sgResetState();
}

sgPlanarLightMap::sgAdjacent sgPlanarLightMap::sgAreAdjacent(U32 surface1, U32 surface2)
{
   U32 i1, i2;
   Vector<U32> index1, index2;

   //if we can't identify the surfaces then assume the worst...
   if((surface1 == U32(-1)) || (surface2 == U32(-1)))
      return sgaFalse;

   if((surface1 == SG_NULL_SURFACE) || (surface2 == SG_NULL_SURFACE))
      return sgaFalse;

   // get the compare surfaces...
   const Interior::Surface &surf1 = sgInteriorDetail->getSurface(surface1);
   const Interior::Surface &surf2 = sgInteriorDetail->getSurface(surface2);

   // grab the surface's winding indexes...
   for(i1=0; i1<surf1.windingCount; i1++)
      index1.push_back(sgInteriorDetail->getWinding(surf1.windingStart + i1));

   // grab the surface's winding indexes...
   for(i2=0; i2<surf2.windingCount; i2++)
      index2.push_back(sgInteriorDetail->getWinding(surf2.windingStart + i2));

   // compare for common index...
   long indexcount = 0;
   for(i1=0; i1<index1.size(); i1++)
   {
      for(i2=0; i2<index2.size(); i2++)
      {
         if(index1[i1] == index2[i2])
         {
            indexcount++;
            if(indexcount > 1)
               return sgaTrue;
         }
      }
   }

   if(indexcount < 1)
      return sgaFalse;

   return sgaMaybe;
}

void sgPlanarLightMap::sgMergeLighting(GBitmap *lightmap, GBitmap *normalmap, U32 xoffset, U32 yoffset)
{
   sgTexels->sgFillInLighting();
   sgTexels->sgBlur();
   sgNexels->sgFillInLighting();
   sgNexels->sgBlur();

   // CodeReview: Catch this early. I don't think this will fail ever, but I want to know. [5/14/2007 Pat]
   AssertFatal( lightmap->bytesPerPixel == normalmap->bytesPerPixel, "Assumption failed in sgMergeLighting" );

   U32 y, x, d, s;
   F32 si, di;
   U8 *bits;
   U8 *nbits;
   ColorF texel;
   ColorF curtexel;
   Point3F nexel;
   Point3F curnexel;
   Point3F half = Point3F(0.5f, 0.5f, 0.5f);

   ColorF *texels = sgTexels->sgData;
   ColorF *vectors = sgNexels->sgData;

   for(y=0; y<sgHeight; y++)
   {
      bits = lightmap->getAddress(xoffset, (yoffset + y));
      nbits = normalmap->getAddress(xoffset, (yoffset + y));
      d = 0;

      for(x=0; x<sgWidth; x++)
      {
         s = ((y * sgWidth) + x);
         ColorF &texel1 = texels[s];
         //ColorF &texel2 = texels[(s + 1)];
         ColorF &p1c = vectors[s];
         //ColorF &p2c = vectors[(s + 1)];

         Point3F p1 = Point3F(p1c.red, p1c.green, p1c.blue);
         //Point3F p2 = Point3F(p2c.red, p2c.green, p2c.blue);

         texel = texel1;// + texel2 + texel3 + texel4;
         //texel *= 0.25;

         nexel = p1;// + p2 + p3 + p4;
         nexel.normalizeSafe();

         // get the current texel (BGR)...
         curtexel = ColorF(bits[d], bits[d+1], bits[d+2]); // * (See Note)
         curtexel *= 0.0039215f;

         // get the current nexel (BGR)...
         curnexel = Point3F(nbits[d], nbits[d+1], nbits[d+2]); // * (See Note)
         curnexel *= 0.0039215f;
         curnexel -= half;
         curnexel *= 2.0f;

         // adjust...
         si = texel.red + texel.green + texel.blue;
         di = curtexel.red + curtexel.green + curtexel.blue;
         di = getMax(di, 0.00001f);
         si = si / di;

         // and add...
         curtexel += texel;
         curnexel = (nexel * si) + curnexel;
         curnexel.normalizeSafe();

         // convert...
         curtexel *= 255.0f;
         curnexel = ((curnexel * 0.5f) + half) * 255.0f;

         // save (BGR)...
         bits[d] = mClamp(((U32)curtexel.red), 0, 255);
         nbits[d++] = mClamp(((U32)curnexel.x), 0, 255); // * (See Note)

         bits[d] = mClamp(((U32)curtexel.green), 0, 255);
         nbits[d++] = mClamp(((U32)curnexel.y), 0, 255);

         bits[d] = mClamp(((U32)curtexel.blue), 0, 255);
         nbits[d++] = mClamp(((U32)curnexel.z), 0, 255); // * (See Note)

         // CodeReview: '*' In these two cases, dealing with the light directional map,
         // assumptions were made that GBitmap would be converting it's own bits
         // into BGR on a texture upload, which occurred in TSE. It is kind of a 
         // hack to reverse the values here, however it should yield deterministic
         // results on all API's. [5/18/2007 Pat]

         // I set the sun to full red and got a blue interior on the screen.  So I think something is converting
         // it to BGR correctly.  I've reverted this back to RGB [9/11/07 btr]

         // CodeReview: Another 24-bit bitmap assumption... [5/14/2007 Pat]
         if( lightmap->bytesPerPixel > 3 )
            d += ( lightmap->bytesPerPixel - 3 );
      }
   }
}
