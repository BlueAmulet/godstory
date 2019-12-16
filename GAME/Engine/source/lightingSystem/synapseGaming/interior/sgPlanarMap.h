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

#include "lightingSystem/synapseGaming/sgLightMap.h"
#include "lightingSystem/synapseGaming/sgHashMap.h"


class InteriorInstance;
class Interior;
class InteriorSimpleMesh;


/**
* Used to generate light maps on interiors.  This class will
* calculate one surface at a time (using sgPlanarLightMap::sgSurfaceIndex).
*/
class sgPlanarLightMap : public sgLightMap
{
public:
   struct sgSmoothingVert
   {
      Point3F sgVert;
      Point3F sgVect;
      Point2F sgText;
      Point3F sgNorm;
      Point3F sgTang;
      Point3F sgBinorm;
   };
   struct sgSmoothingTri
   {
      Point3F sgTangSDerivative;
      Point3F sgTangTDerivative;
      Point3F sgNormSDerivative;
      Point3F sgNormTDerivative;
      Point3F sgBinormSDerivative;
      Point3F sgBinormTDerivative;
      Point3F sgTang;
      Point3F sgBiNorm;
      sgSmoothingVert sgVerts[3];
   };
   struct sgLexel
   {
      bool shadowTestOnly;
      Point2D lmPos;
      Point3F worldPos;
      Point3F normal;
      Point3F tangent;
      Point3F biNormal;
   };
   enum sgLightingPass
   {
      sglpInner = 0,
      sglpOuter,
      sglpCount
   };
   struct sgOccluder
   {
      void *sgObject;
      S32 sgSurface;
   };
   enum sgAdjacent
   {
      sgaTrue,
      sgaFalse,
      // I like this one - fuzzy logic in action "umm... maybe?"... :)
      sgaMaybe
   };
protected:
   /// Surface to generate light map.
   PlaneF surfacePlane;
   Vector<sgSmoothingVert> triStrip;
   S32 sgSAxis;
   S32 sgTAxis;
   bool sgFlippedWindings;
   Box3F sgSurfaceBox;
   Vector<sgLexel> sgInnerLexels;
   Vector<sgLexel> sgOuterLexels;
   InteriorInstance *sgInteriorInstance;
   Interior *sgInteriorDetail;
   S32 sgInteriorSurface;
   InteriorSimpleMesh *sgInteriorStaticMesh;
public:
   sgPlanarLightMap(U32 width, U32 height, InteriorInstance *interior, Interior *detail,
      S32 surface, InteriorSimpleMesh *staticmesh, PlaneF surfaceplane, const Vector<sgSmoothingVert> &tristrip)
      : sgLightMap(width, height)
   {
      sgDirty = false;
      sgFlippedWindings = false;
      surfacePlane = surfaceplane;
      triStrip.clear();
      triStrip.merge(tristrip);
      sgSurfaceBox.min = Point3F(F32_MAX, F32_MAX, F32_MAX);
      sgSurfaceBox.max = Point3F(-F32_MAX, -F32_MAX, -F32_MAX);
      sgInteriorInstance = interior;
      sgInteriorDetail = detail;
      sgInteriorSurface = surface;
      sgInteriorStaticMesh = staticmesh;
   }
   /// Transfer the light map to a GBitmap and blur.
   void sgMergeLighting(GBitmap *lightmap, GBitmap *lightnormalmap, U32 xoffset, U32 yoffset);
   /// See: sgLightMap::sgCalculateLighting.
   void sgSetupLighting();
   virtual void sgCalculateLighting(sgLightInfo *light);
   bool sgIsDirty() {return sgDirty;}
protected:
   bool sgDirty;
   static U32 sgCurrentOccluderMaskId;
   void sgBuildTangentAndBiNormal(sgSmoothingTri &tri);
   void sgBuildDerivatives(sgSmoothingTri &tri);
   void sgBuildLexels(const Vector<sgSmoothingTri> &tris);
   bool sgCastRay(Point3F s, Point3F e, SceneObject *obj, Interior *detail, InteriorSimpleMesh *sm, sgOccluder &occluderinfo);
   //bool sgIsValidOccluder(const sgOccluder &occluderinfo, Vector<sgOccluder> &validoccluders, bool isinnerlexel);
   bool sgIsValidOccluder(const sgOccluder &occluderinfo, hash_multimap<void *, sgOccluder> &validoccluders, bool isinnerlexel);

   /// Try to avoid false shadows by ignoring direct neighbors.
   sgAdjacent sgAreAdjacent(U32 surface1, U32 surface2);
};
