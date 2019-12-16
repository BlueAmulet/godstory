//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "math/mBox.h"
#include "math/mathUtils.h"
#include "interior/interiorInstance.h"
#include "sceneGraph/sceneGraph.h"
#include "platform/profiler.h"
#include "util/triRayCheck.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "lightingSystem/synapseGaming/sgLightMap.h"
#include "lightingSystem/synapseGaming/sgLightingModel.h"

VectorPtr<SceneObject *> sgShadowObjects::sgObjects;
Vector<sgShadowObjects::sgObjectInfo *> sgShadowObjects::sgObjectInfoStorage;
sgShadowObjects::sgStaticMeshBVPTEntry sgShadowObjects::sgStaticMeshBVPTMap;

/// used to generate light map indexes that wrap around
/// instead of exceeding the index bounds.
inline S32 sgGetIndex(S32 width, S32 height, S32 x, S32 y)
{
	if(x > (width-1))
		x -= width;
	else if(x < 0)
		x += width;

	if(y > (height-1))
		y -= height;
	else if(y < 0)
		y += height;

	return (y * width) + x;
}

void sgColorMap::sgFillInLighting()
{
	U32 x, y;
	U32 lastgoodx = 0, lastgoody = 0;
	U32 lastgoodyoffset = 0, yoffset = 0;

	if(sgLightManager::sgAllowFullLightMaps())
		return;

	U32 lmscalemask = sgLightManager::sgGetLightMapScale() - 1;

	for(y=0; y<sgHeight; y++)
	{
		// do we have a good y?
		if((y & lmscalemask) == 0)
		{
			lastgoody = y;
			lastgoodyoffset = lastgoody * sgWidth;
		}

		yoffset = y * sgWidth;

		for(x=0; x<sgWidth; x++)
		{
			// do we have a good x?
			if((x & lmscalemask) == 0)
			{
				lastgoodx = x;

				// only bailout if we're on a good y, otherwise
				// all of the x entries are empty...
				if((y & lmscalemask) == 0)
					continue;
			}

			ColorF &last = sgData[(lastgoodyoffset + lastgoodx)];
			sgData[(yoffset + x)] = last;
		}
	}
}

void sgColorMap::sgBlur()
{
	static F32 blur[3][3] = {{0.1f, 0.125f, 0.1f}, {0.125f, 0.1f, 0.125f}, {0.1f, 0.125f, 0.1f}};
	//static F32 blur[3][3] = {{0.075, 0.125, 0.075}, {0.125, 0.2, 0.125}, {0.075, 0.125, 0.075}};

	ColorF *buffer = new ColorF[sgWidth * sgHeight];

	// lets get the values that we don't blur...
	dMemcpy(buffer, sgData, (sizeof(ColorF) * sgWidth * sgHeight));

	for(U32 y=1; y<(sgHeight-1); y++)
	{
		for(U32 x=1; x<(sgWidth-1); x++)
		{
			ColorF &col = buffer[((y * sgWidth) + x)];
				
			col = ColorF(0.0f, 0.0f, 0.0f);

			for(S32 by=-1; by<2; by++)
			{
				for(S32 bx=-1; bx<2; bx++)
				{
					col += sgData[(((y + by) * sgWidth) + (x + bx))] * blur[bx+1][by+1];
				}
			}
		}
	}
	
	delete[] sgData;
	sgData = buffer;
}
/*
void sgColorMap::sgMerge(GBitmap *lightmap, U32 xoffset, U32 yoffset, bool normalblend)
{
	U32 y, x, c, frag;
	U8 *bits;

	sgFillInLighting();
	sgBlur();

	for(y=0; y<sgHeight; y++)
	{
		bits = lightmap->getAddress(xoffset, (yoffset + y));
		c = 0;

		for(x=0; x<sgWidth; x++)
		{
			ColorF &texel = sgData[((y * sgWidth) + x)];

			if(normalblend)
			{
				Point3F src(texel.red, texel.green, texel.blue);
				//Point3F dst(0, 0, 0);
				Point3F dst(bits[c+2], bits[c+1], bits[c]);
				dst *= 0.0039215f;
				dst = (dst - Point3F(0.5f, 0.5f, 0.5f)) * 2.0f;
				dst += src;
				dst.normalizeSafe();
				dst = (dst * 0.5) + Point3F(0.5f, 0.5f, 0.5f);

				bits[c++] = dst.z * 255.0f;
				bits[c++] = dst.y * 255.0f;
				bits[c++] = dst.x * 255.0f;
			}
			else
			{
				frag = bits[c] + (texel.red * 255.0f);
				if(frag > 255) frag = 255;
				bits[c++] = frag;

				frag = bits[c] + (texel.green * 255.0f);
				if(frag > 255) frag = 255;
				bits[c++] = frag;

				frag = bits[c] + (texel.blue * 255.0f);
				if(frag > 255) frag = 255;
				bits[c++] = frag;
			}
		}
	}
}
*/

void sgObjectCallback(SceneObject *object, void *vector)
{
	VectorPtr<SceneObject *> *intersectingobjects = static_cast<VectorPtr<SceneObject *> *>(vector);
	intersectingobjects->push_back(object);
}

void sgShadowObjects::sgGetObjects(SceneObject *obj)
{
	sgObjects.clear();
	obj->getContainer()->findObjects(ShadowCasterObjectType, &sgObjectCallback, &sgObjects);
}

bool sgShadowObjects::sgCastRayStaticMesh(Point3F s, Point3F e, InteriorSimpleMesh *staticmesh)
{
   sgStatistics::sgStaticMeshCastRayCount++;

   sgStaticMeshBVPTEntry *entry = sgStaticMeshBVPTMap.find(U32(staticmesh));
   AssertFatal((entry), "hash_map should always return an entry!");

   // is the BVPT there?
   if(!entry->info)
   {
      sgObjectInfo *objinfo = new sgObjectInfo();
      entry->info = objinfo;
      sgObjectInfoStorage.push_back(objinfo);

      objinfo->sgInverseTransform = staticmesh->transform;
      objinfo->sgInverseTransform.inverse();

      // get the tri count...
      U32 tricount = 0;
      for(U32 p=0; p<staticmesh->primitives.size(); p++)
      {
         const InteriorSimpleMesh::primitive &prim = staticmesh->primitives[p];
         if(prim.alpha)
            continue;
         tricount += prim.count;
      }

      objinfo->sgTris.reserve(tricount);

      // add the tris...
      for(U32 p=0; p<staticmesh->primitives.size(); p++)
      {
         const InteriorSimpleMesh::primitive &prim = staticmesh->primitives[p];
         if(prim.alpha || (prim.count < 3))
            continue;

         PlaneF plane(staticmesh->verts[0], staticmesh->verts[1], staticmesh->verts[2]);
         Point3F norm = (staticmesh->norms[0] + staticmesh->norms[1] + staticmesh->norms[2]) * 0.3333f;
         bool flip = (mDot(plane, norm) < 0.0f);

         for(U32 t=2; t<prim.count; t++)
         {
            U32 baseindex = prim.start + t;
            objinfo->sgTris.increment();
            sgStaticMeshTri &tri = objinfo->sgTris.last();

            bool winding = (t & 0x1);
            if(flip)
               winding = !winding;

            Point3F *v0, *v1, *v2;
            if(winding)
            {
               v0 = &staticmesh->verts[baseindex-1];
               v1 = &staticmesh->verts[baseindex-2];
               v2 = &staticmesh->verts[baseindex];
            }
            else
            {
               v0 = &staticmesh->verts[baseindex-2];
               v1 = &staticmesh->verts[baseindex-1];
               v2 = &staticmesh->verts[baseindex];
            }

            tri.sgVert[0].set(v0->x, v0->y, v0->z);
            tri.sgVert[1].set(v1->x, v1->y, v1->z);
            tri.sgVert[2].set(v2->x, v2->y, v2->z);
            tri.sgPlane.set((*v0), (*v1), (*v2));

            tri.sgBox.min = (*v0);
            tri.sgBox.max = (*v0);
            tri.sgBox.min.setMin((*v1));
            tri.sgBox.max.setMax((*v1));
            tri.sgBox.min.setMin((*v2));
            tri.sgBox.max.setMax((*v2));
         }
      }

      // find the avoid axis if any...
      sgStaticMeshBVPT::axisType avoidaxis = sgStaticMeshBVPT::atNone;
      U32 maxspancount = 0;
      Point3F mid;

      tricount = objinfo->sgTris.size();
      staticmesh->bounds.getCenter(&mid);

      for(S32 a=0; a<3; a++)
      {
         Point3F normal(0.0f, 0.0f, 0.0f);
         normal[a] = 1.0f;
         PlaneF plane(mid, normal);

         U32 spancount = 0;
         for(U32 t=0; t<tricount; t++)
         {
            sgStaticMeshTri &tri = objinfo->sgTris[t];
            bool frontmin = (plane.distToPlane(tri.sgBox.min) > 0.0f);
            bool frontmax = (plane.distToPlane(tri.sgBox.max) > 0.0f);

            if(frontmin == frontmax)
               continue;

            spancount++;
         }

         if((tricount > 0) && (maxspancount < spancount) && ((F32(spancount) / F32(tricount)) > 0.5f))
         {
            maxspancount = spancount;
            avoidaxis = sgStaticMeshBVPT::axisType(a);
         }
      }

      objinfo->sgBVPT.init(staticmesh->bounds, avoidaxis);

      // need to do this last so any vector copy/resize doesn't kill the pointers...
      for(U32 t=0; t<objinfo->sgTris.size(); t++)
      {
         sgStaticMeshTri &tri = objinfo->sgTris[t];
         objinfo->sgBVPT.storeObject(tri.sgBox, &tri);
      }
   }

   // convert to static mesh space...
   sgObjectInfo *objinfo = entry->info;
   objinfo->sgInverseTransform.mulP(s);
   objinfo->sgInverseTransform.mulP(e);
   s.convolveInverse(staticmesh->scale);
   e.convolveInverse(staticmesh->scale);

   // early out test...
   F32 t;
   Point3F n;
   if(!staticmesh->bounds.collideLine(s, e, &t, &n))
      return false;

   // get the likely occluders...
   static S32 lastlistsize = 100;
   sgStaticMeshBVPT::objectList list;
   list.reserve(lastlistsize);
   objinfo->sgBVPT.collectObjectsClipped(s, e, list);
   lastlistsize = getMax(lastlistsize, list.size());

   sgStatistics::sgStaticMeshBVPTPotentialOccluderCount += list.size();

   Point3D s64(s.x, s.y, s.z);
   Point3D vect64 = Point3D(e.x, e.y, e.z) - s64;
   Point3F vect(vect64.x, vect64.y, vect64.z);

   // now cast against them...
   U32 tc = list.size();
   sgStaticMeshTri **tris = list.address();

   for(U32 i=0; i<tc; i++)
   {
      sgStaticMeshTri *tri = tris[i];

      if(mDot(tri->sgPlane, vect) >= 0.0f)
         continue;

      //stats
      sgStatistics::sgStaticMeshSurfaceOccluderCount++;

      if(castRayTriangle(s64, vect64, tri->sgVert[0], tri->sgVert[1], tri->sgVert[2]))
         return true;
   }

   return false;
}

void sgShadowObjects::sgClearStaticMeshBVPTData()
{
   for(U32 i=0; i<sgObjectInfoStorage.size(); i++)
      delete sgObjectInfoStorage[i];

   sgObjectInfoStorage.clear();
   sgStaticMeshBVPTMap.clear();
}

void sgLightMap::sgGetIntersectingObjects(const Box3F &surfacebox, const SceneObject *skipobject)
{
   sgIntersectingSceneObjects.clear();
   sgIntersectingStaticMeshObjects.clear();

   bool boxset = false;
   Box3F maxworld = Box3F(F32_MIN, F32_MIN, F32_MIN, F32_MAX, F32_MAX, F32_MAX);
   Vector<sgStaticMeshInfo> meshes;
   meshes.reserve(256);

   for(U32 i=0; i<sgShadowObjects::sgObjects.size(); i++)
	{
      SceneObject *obj = sgShadowObjects::sgObjects[i];
      if(obj == skipobject)
         continue;
      if(!surfacebox.isOverlapped(obj->getWorldBox()))
         continue;

      sgIntersectingSceneObjects.push_back(obj);

      InteriorInstance *inst = dynamic_cast<InteriorInstance *>(obj);
      if(!inst)
         continue;

      Interior *detail = inst->getDetailLevel(0);
      for(U32 sm=0; sm<detail->getStaticMeshCount(); sm++)
		{
         const InteriorSimpleMesh *smobj = detail->getStaticMesh(sm);
         Box3F bounds = smobj->bounds;
         Box3F worldbounds;

         MathUtils::transformBoundingBox(bounds, smobj->transform, smobj->scale, worldbounds);
         bounds = worldbounds;
         MathUtils::transformBoundingBox(bounds, inst->getTransform(), inst->getScale(), worldbounds);

         if(!surfacebox.isOverlapped(worldbounds))
            continue;

         meshes.increment(1);
         sgStaticMeshInfo &sminfo = meshes.last();

         sminfo.sgWorldBounds = worldbounds;
         sminfo.sgStaticMesh = (InteriorSimpleMesh *)smobj;
         sminfo.sgInteriorInstance = inst;

         if(!boxset)
			{
            maxworld = worldbounds;
            boxset = true;
			}
			else
			{
            maxworld.max.setMax(worldbounds.max);
            maxworld.min.setMin(worldbounds.min);
			}
		}
	}

   sgIntersectingStaticMeshObjects.init(maxworld);

   for(U32 i=0; i<meshes.size(); i++)
   {
      const sgStaticMeshInfo &sminfo = meshes[i];
      sgIntersectingStaticMeshObjects.storeObject(sminfo.sgWorldBounds, sminfo);
   }
}

//----------------------------------------------