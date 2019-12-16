//-----------------------------------------------------------------------------
// PowerEngine Advanced
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "ts/tsShape.h"
#include "ts/tsLastDetail.h"
#include "core/stringTable.h"
#include "console/console.h"
#include "ts/tsShapeInstance.h"
#include "collision/convex.h"
#include "materials/materialPropertyMap.h"
#include "materials/matInstance.h"
#include "math/mathIO.h"

/// most recent version -- this is the version we write
S32 TSShape::smVersion = 33;
/// the version currently being read...valid only during a read
S32 TSShape::smReadVersion = -1;
const U32 TSShape::smMostRecentExporterVersion = DTS_EXPORTER_CURRENT_VERSION;

F32 TSShape::smAlphaOutLastDetail = -1.0f;
F32 TSShape::smAlphaInBillboard = 0.15f;
F32 TSShape::smAlphaOutBillboard = 0.1f;
F32 TSShape::smAlphaInDefault = -1.0f;
F32 TSShape::smAlphaOutDefault = -1.0f;

// don't bother even loading this many of the highest detail levels (but
// always load last renderable detail)
S32 TSShape::smNumSkipLoadDetails = 0;

bool TSShape::smInitOnRead = true;


TSShape::TSShape()
{
   materialList = NULL;
   mReadVersion = -1; // -1 means constructed from scratch (e.g., in exporter or no read yet)
   mMemoryBlock = NULL;
   mSequencesConstructed = false;

   VECTOR_SET_ASSOCIATION(sequences);
   VECTOR_SET_ASSOCIATION(billboardDetails);
   VECTOR_SET_ASSOCIATION(detailCollisionAccelerators);
   VECTOR_SET_ASSOCIATION(names);
   WholeNodesMatters.setAll();
   LowerNodesMatters.setAll();
}

TSShape::~TSShape()
{
   delete materialList;

   S32 i;

   // everything left over here is a legit mesh
   for (i=0; i<meshes.size(); i++)
      if (meshes[i])
         destructInPlace(meshes[i]);

   for (i=0; i<sequences.size(); i++)
      destructInPlace(&sequences[i]);

   for (i=0; i<billboardDetails.size(); i++)
   {
      delete billboardDetails[i];
      billboardDetails[i] = NULL;
   }
   billboardDetails.clear();

   // Delete any generated accelerators
   S32 dca;
   for (dca = 0; dca < detailCollisionAccelerators.size(); dca++)
   {
      ConvexHullAccelerator* accel = detailCollisionAccelerators[dca];
      if (accel != NULL) {
         delete [] accel->vertexList;
         delete [] accel->normalList;
         for (S32 j = 0; j < accel->numVerts; j++)
            delete [] accel->emitStrings[j];
         delete [] accel->emitStrings;
         delete accel;
      }
   }
   for (dca = 0; dca < detailCollisionAccelerators.size(); dca++)
      detailCollisionAccelerators[dca] = NULL;

   delete [] mMemoryBlock;
   mMemoryBlock = NULL;
}

bool TSShape::isUpper(S32 nodeIndex, S32 nameIndex)
{
	if(UpperNodesMatters.test(nodeIndex))
		return true;
	if(nodes[nodeIndex].nameIndex == nameIndex)
	{
		UpperNodesMatters.set(nodeIndex);
		return true;
	}
	if(nodes[nodeIndex].parentIndex < 0)
		return false;
	if(isUpper(nodes[nodeIndex].parentIndex, nameIndex))
	{
		UpperNodesMatters.set(nodeIndex);
		return true;
	}

	return false;
}

void TSShape::initNodesMatters(StringTableEntry midNode)
{
	if(!midNode || !midNode[0])
		return;

	S32 nameIndex = findName(midNode);
	if(nameIndex < 0)
		return;

	for(S32 i=0; i<nodes.size(); i++)
		isUpper(i, nameIndex);

	LowerNodesMatters.setAll(nodes.size());
	LowerNodesMatters.takeAway(UpperNodesMatters);
}

TSIntegerSet& TSShape::getNodesMatters(S32 val)
{
	if(TSThread::Upper == val)
		return UpperNodesMatters;
	else if(TSThread::Lower == val)
		return LowerNodesMatters;
	else
		return WholeNodesMatters;
}

const char * TSShape::getName(S32 nameIndex) const
{
   AssertFatal(nameIndex>=0 && nameIndex<names.size(),"TSShape::getName");
   return names[nameIndex];
}

S32 TSShape::findName(const char * name) const
{
   for (S32 i=0; i<names.size(); i++)
      if (!dStricmp(name,names[i]))
         return i;
   return -1;
}

S32 TSShape::findNode(S32 nameIndex) const
{
   for (S32 i=0; i<nodes.size(); i++)
      if (nodes[i].nameIndex==nameIndex)
         return i;
   return -1;
}

S32 TSShape::findObject(S32 nameIndex) const
{
   for (S32 i=0; i<objects.size(); i++)
      if (objects[i].nameIndex==nameIndex)
         return i;
   return -1;
}

S32 TSShape::findIflMaterial(S32 nameIndex) const
{
   for (S32 i=0; i<iflMaterials.size(); i++)
      if (iflMaterials[i].nameIndex==nameIndex)
         return i;
   return -1;
}

S32 TSShape::findDetail(S32 nameIndex) const
{
   for (S32 i=0; i<details.size(); i++)
      if (details[i].nameIndex==nameIndex)
         return i;
   return -1;
}

S32 TSShape::findSequence(S32 nameIndex) const
{
   for (S32 i=0; i<sequences.size(); i++)
      if (sequences[i].nameIndex==nameIndex)
         return i;
   return -1;
}

void TSShape::init()
{
   S32 numSubShapes = subShapeFirstNode.size();
   AssertFatal(numSubShapes==subShapeFirstObject.size(),"TSShape::init");

   S32 i,j;

   // set up parent/child relationships on nodes and objects
   for (i=0; i<nodes.size(); i++)
      nodes[i].firstObject = nodes[i].firstChild = nodes[i].nextSibling = -1;
   for (i=0; i<nodes.size(); i++)
   {
      S32 parentIndex = nodes[i].parentIndex;
      if (parentIndex>=0)
      {
         if (nodes[parentIndex].firstChild<0)
            nodes[parentIndex].firstChild=i;
         else
         {
            S32 child = nodes[parentIndex].firstChild;
            while (nodes[child].nextSibling>=0)
               child = nodes[child].nextSibling;
            nodes[child].nextSibling = i;
         }
      }
   }
   for (i=0; i<objects.size(); i++)
   {
      objects[i].nextSibling = -1;

      S32 nodeIndex = objects[i].nodeIndex;
      if (nodeIndex>=0)
      {
         if (nodes[nodeIndex].firstObject<0)
            nodes[nodeIndex].firstObject = i;
         else
         {
            S32 objectIndex = nodes[nodeIndex].firstObject;
            while (objects[objectIndex].nextSibling>=0)
               objectIndex = objects[objectIndex].nextSibling;
            objects[objectIndex].nextSibling = i;
         }
      }
   }

   mFlags = 0;
   for (i=0; i<sequences.size(); i++)
   {
      if (!sequences[i].animatesScale())
         continue;

      U32 curVal = mFlags & AnyScale;
      U32 newVal = sequences[i].flags & AnyScale;
      mFlags &= ~(AnyScale);
      mFlags |= getMax(curVal,newVal); // take the larger value (can only convert upwards)
   }

   // set up alphaIn and alphaOut vectors...
   #if defined(POWER_MAX_LIB)
   alphaIn.setSize(details.size());
   alphaOut.setSize(details.size());
   #endif
   for (i=0; i<details.size(); i++)
   {
      if (details[i].size<0)
      {
         // we don't care...
         alphaIn[i]  = 0.0f;
         alphaOut[i] = 0.0f;
      }
      else if (i+1==details.size() || details[i+1].size<0)
      {
         alphaIn[i]  = 0.0f;
         alphaOut[i] = smAlphaOutLastDetail;
      }
      else
      {
         if (details[i+1].subShapeNum<0)
         {
            // following detail is a billboard detail...treat special...
            alphaIn[i]  = smAlphaInBillboard;
            alphaOut[i] = smAlphaOutBillboard;
         }
         else
         {
            // next detail is normal detail
            alphaIn[i] = smAlphaInDefault;
            alphaOut[i] = smAlphaOutDefault;
         }
      }
   }

   for (i=mSmallestVisibleDL-1; i>=0; i--)
   {
      if (i<smNumSkipLoadDetails)
      {
         // this detail level renders when pixel size
         // is larger than our cap...zap all the meshes and decals
         // associated with it and use the next detail level
         // instead...
         S32 ss    = details[i].subShapeNum;
         S32 od    = details[i].objectDetailNum;

         if (ss==details[i+1].subShapeNum && od==details[i+1].objectDetailNum)
            // doh! already done this one (init can be called multiple times on same shape due
            // to sequence importing).
            continue;
         details[i].subShapeNum = details[i+1].subShapeNum;
         details[i].objectDetailNum = details[i+1].objectDetailNum;
      }
   }

   for (i=0; i<details.size(); i++)
   {
      S32 count = 0;
      S32 ss = details[i].subShapeNum;
      S32 od = details[i].objectDetailNum;
      if (ss<0)
      {
         // billboard detail...
         count += 2;
         continue;
      }
      S32 start = subShapeFirstObject[ss];
      S32 end   = start + subShapeNumObjects[ss];
      for (j=start; j<end; j++)
      {
         Object & obj = objects[j];
         if (od<obj.numMeshes)
         {
            TSMesh * mesh = meshes[obj.startMeshIndex+od];
            count += mesh ? mesh->getNumPolys() : 0;
         }
      }
      details[i].polyCount = count;
   }

   // Init the collision accelerator array.  Note that we don't compute the
   //  accelerators until the app requests them
   {
      S32 dca;
      for (dca = 0; dca < detailCollisionAccelerators.size(); dca++)
      {
         ConvexHullAccelerator* accel = detailCollisionAccelerators[dca];
         if (accel != NULL) {
            delete [] accel->vertexList;
            delete [] accel->normalList;
            for (S32 j = 0; j < accel->numVerts; j++)
               delete [] accel->emitStrings[j];
            delete [] accel->emitStrings;
            delete accel;
         }
      }

      detailCollisionAccelerators.setSize(details.size());
      for (dca = 0; dca < detailCollisionAccelerators.size(); dca++)
         detailCollisionAccelerators[dca] = NULL;
   }

   initMaterialList();
}

void TSShape::setupBillboardDetails(TSShapeInstance *shape)
{
   // set up billboard details -- only do this once, meaning that
   // if we add a sequence to the shape we don't redo the billboard
   // details...
   S32 i;
   if (billboardDetails.empty())
   {
      for (i=0; i<details.size(); i++)
      {
         if (details[i].subShapeNum>=0)
            continue; // not a billboard detail
         while (billboardDetails.size()<=i)
            billboardDetails.push_back(NULL);
         U32 props = details[i].objectDetailNum;
         U32 numEquatorSteps = props & 0x7F; // bits 0..6
         U32 numPolarSteps = (props>>7) & 0x3F; // bits 7..12
         F32 polarAngle = 0.5f * M_PI_F * (1.0f/64.0f) * (F32) ((props>>13) & 0x3F); // bits 13..18
         S32 dl = (props>>19) & 0x0F;  // 19..22
         S32 dim = (props>>23) & 0xFF; // 23..30
         bool includePoles = (props & 0x80000000)!=0; // bit 31

         billboardDetails[i] = new TSLastDetail(shape,numEquatorSteps,numPolarSteps,polarAngle,includePoles,dl,dim);
      }
   }
}

void TSShape::initMaterialList()
{

   S32 numSubShapes = subShapeFirstObject.size();
   #if defined(POWER_MAX_LIB)
   subShapeFirstTranslucentObject.setSize(numSubShapes);
   #endif

   S32 i,j,k;
   // for each subshape, find the first translucent object
   // also, while we're at it, set mHasTranslucency
   for (S32 ss = 0; ss<numSubShapes; ss++)
   {
      S32 start = subShapeFirstObject[ss];
      S32 end = subShapeNumObjects[ss];
      subShapeFirstTranslucentObject[ss] = end;
      for (i=start; i<end; i++)
      {
         // check to see if this object has translucency
         Object & obj = objects[i];
         for (j=0; j<obj.numMeshes; j++)
         {
            TSMesh * mesh = meshes[obj.startMeshIndex+j];
            if (!mesh)
               continue;
            for (k=0; k<mesh->primitives.size(); k++)
            {
               if (mesh->primitives[k].matIndex & TSDrawPrimitive::NoMaterial)
                  continue;
               S32 flags = materialList->getFlags(mesh->primitives[k].matIndex & TSDrawPrimitive::MaterialMask);
               if (flags & TSMaterialList::AuxiliaryMap)
                  continue;
               if (flags & TSMaterialList::Translucent)
               {
                  mFlags |= HasTranslucency;
                  subShapeFirstTranslucentObject[ss] = i;
                  break;
               }
            }
            if (k!=mesh->primitives.size())
               break;
         }
         if (j!=obj.numMeshes)
            break;
      }
      if (i!=end)
         break;
   }

}

bool TSShape::preloadMaterialList()
{
   if(materialList)
      return materialList->load(MeshTexture, mSourceResource ? mSourceResource->path : "", true);

   return true;
}

bool TSShape::buildConvexHull(S32 dl) const
{
   AssertFatal(dl>=0 && dl<details.size(),"TSShape::buildConvexHull: detail out of range");

   bool ok = true;

   const Detail & detail = details[dl];
   S32 ss = detail.subShapeNum;
   S32 od = detail.objectDetailNum;

   S32 start = subShapeFirstObject[ss];
   S32 end   = subShapeNumObjects[ss];
   for (S32 i=start; i<end; i++)
   {
      TSMesh * mesh = meshes[objects[i].startMeshIndex+od];
      if (!mesh)
         continue;
      ok &= mesh->buildConvexHull();
   }
   return ok;
}

Vector<MatrixF> gTempNodeTransforms(__FILE__, __LINE__);

void TSShape::computeBounds(S32 dl, Box3F & bounds) const
{
   // if dl==-1, nothing to do
   if (dl==-1)
      return;

   AssertFatal(dl>=0 && dl<details.size(),"TSShapeInstance::computeBounds");

   // get subshape and object detail
   const TSDetail * detail = &details[dl];
   S32 ss = detail->subShapeNum;
   S32 od = detail->objectDetailNum;

   // set up temporary storage for non-local transforms...
   S32 i;
   S32 start = subShapeFirstNode[ss];
   S32 end   = subShapeNumNodes[ss] + start;
   gTempNodeTransforms.setSize(end-start);
   for (i=start; i<end; i++)
   {
      MatrixF mat;
      QuatF q;
      TSTransform::setMatrix(defaultRotations[i].getQuatF(&q),defaultTranslations[i],&mat);
      if (nodes[i].parentIndex>=0)
         gTempNodeTransforms[i-start].mul(gTempNodeTransforms[nodes[i].parentIndex-start],mat);
      else
         gTempNodeTransforms[i-start] = mat;
   }

   // run through objects and updating bounds as we go
   bounds.min.set( 10E30f, 10E30f, 10E30f);
   bounds.max.set(-10E30f,-10E30f,-10E30f);
   Box3F box;
   start = subShapeFirstObject[ss];
   end   = subShapeNumObjects[ss] + start;
   for (i=start; i<end; i++)
   {
      const Object * object = &objects[i];
      TSMesh * mesh = od<object->numMeshes ? meshes[object->startMeshIndex+od] : NULL;
      if (mesh)
      {
         static MatrixF idMat(true);
         if (object->nodeIndex<0)
            mesh->computeBounds(idMat,box);
         else
            mesh->computeBounds(gTempNodeTransforms[object->nodeIndex-start],box);
         bounds.min.setMin(box.min);
         bounds.max.setMax(box.max);
      }
   }
}

TSShapeAlloc TSShape::alloc;

#define alloc TSShape::alloc

// messy stuff: check to see if we should "skip" meshNum
// this assumes that meshes for a given object are in a row
// skipDL is the lowest detail number we keep (i.e., the # of details we skip)
bool TSShape::checkSkip(S32 meshNum, S32 & curObject, S32 skipDL)
{
   if (skipDL==0)
      // easy out...
      return false;

   // skip detail level exists on this subShape
   S32 skipSS = details[skipDL].subShapeNum;

   if (curObject<objects.size())
   {
      S32 start = objects[curObject].startMeshIndex;
      if (meshNum>=start)
      {
         // we are either from this object, the next object, or a decal
         if (meshNum < start + objects[curObject].numMeshes)
         {
            // this object...
            if (subShapeFirstObject[skipSS]>curObject)
               // haven't reached this subshape yet
               return true;
            if (skipSS+1==subShapeFirstObject.size() || curObject<subShapeFirstObject[skipSS+1])
               // curObject is on subshape of skip detail...make sure it's after skipDL
               return (meshNum-start<details[skipDL].objectDetailNum);
            // if we get here, then curObject ocurrs on subShape after skip detail (so keep it)
            return false;
         }
         else
            // advance object, try again
            return checkSkip(meshNum,++curObject,skipDL);
      }
   }

   AssertFatal(0,"TSShape::checkSkip: assertion failed");
   return false;
}

void TSShape::assembleShape()
{
   CLockGuard guard(alloc.GetMutex());

   S32 i,j;

   // get counts...
   S32 numNodes = alloc.get32();
   S32 numObjects = alloc.get32();
   S32 numDecals = alloc.get32();
   S32 numSubShapes = alloc.get32();
   S32 numIflMaterials = alloc.get32();
   S32 numNodeRots;
   S32 numNodeTrans;
   S32 numNodeUniformScales;
   S32 numNodeAlignedScales;
   S32 numNodeArbitraryScales;
   if (smReadVersion<22)
   {
      numNodeRots = numNodeTrans = alloc.get32() - numNodes;
      numNodeUniformScales = numNodeAlignedScales = numNodeArbitraryScales = 0;
   }
   else
   {
      numNodeRots = alloc.get32();
      numNodeTrans = alloc.get32();
      numNodeUniformScales = alloc.get32();
      numNodeAlignedScales = alloc.get32();
      numNodeArbitraryScales = alloc.get32();
   }
   S32 numGroundFrames = 0;
   if (smReadVersion>23)
      numGroundFrames = alloc.get32();
   S32 numObjectStates = alloc.get32();
   S32 numDecalStates = alloc.get32();
   S32 numTriggers = alloc.get32();
   S32 numDetails = alloc.get32();
   S32 numMeshes = alloc.get32();
   S32 numSkins = 0;
   if (smReadVersion<23)
      // in later versions, skins are kept with other meshes
      numSkins = alloc.get32();
   S32 numNames = alloc.get32();
   mSmallestVisibleSize = (F32)alloc.get32();
   mSmallestVisibleDL   = alloc.get32();
   S32 skipDL = getMin(mSmallestVisibleDL,smNumSkipLoadDetails);

   alloc.checkGuard();

   // get bounds...
   alloc.get32((S32*)&radius,1);
   alloc.get32((S32*)&tubeRadius,1);
   alloc.get32((S32*)&center,3);
   alloc.get32((S32*)&bounds,6);

   alloc.checkGuard();

   // copy various vectors...
   S32 * ptr32 = alloc.copyToShape32(numNodes*5);
   nodes.set(ptr32,numNodes);

   alloc.checkGuard();

   ptr32 = alloc.copyToShape32(numObjects*6,true);
   if (!ptr32)
      ptr32 = alloc.allocShape32(numSkins*6); // pre v23 shapes store skins and meshes separately...no longer
   else
      alloc.allocShape32(numSkins*6);
   objects.set(ptr32,numObjects);

   alloc.checkGuard();

   // DEPRECATED decals
   ptr32 = alloc.getPointer32(numDecals*5);

   alloc.checkGuard();

   ptr32 = alloc.copyToShape32(numIflMaterials*5);
   iflMaterials.set(ptr32,numIflMaterials);

   alloc.checkGuard();

   ptr32 = alloc.copyToShape32(numSubShapes,true);
   subShapeFirstNode.set(ptr32,numSubShapes);
   ptr32 = alloc.copyToShape32(numSubShapes,true);
   subShapeFirstObject.set(ptr32,numSubShapes);
   // DEPRECATED subShapeFirstDecal
   ptr32 = alloc.getPointer32(numSubShapes);

   alloc.checkGuard();

   ptr32 = alloc.copyToShape32(numSubShapes);
   subShapeNumNodes.set(ptr32,numSubShapes);
   ptr32 = alloc.copyToShape32(numSubShapes);
   subShapeNumObjects.set(ptr32,numSubShapes);
   // DEPRECATED subShapeNumDecals
   ptr32 = alloc.getPointer32(numSubShapes);

   alloc.checkGuard();

   ptr32 = alloc.allocShape32(numSubShapes);
   subShapeFirstTranslucentObject.set(ptr32,numSubShapes);

   // get meshIndexList...older shapes only
   S32 meshIndexListSize = 0;
   S32 * meshIndexList = NULL;
   if (smReadVersion<16)
   {
      meshIndexListSize = alloc.get32();
      meshIndexList = alloc.getPointer32(meshIndexListSize);
   }

   // get default translation and rotation

   //S16 * ptr16 = alloc.allocShape16(0);
   //for (i=0;i<numNodes;i++)
   //   alloc.copyToShape16(4);
   //defaultRotations.set(ptr16,numNodes);
   //alloc.align32();
   //ptr32 = alloc.allocShape32(0);
   //for (i=0;i<numNodes;i++)
   //{
   //   alloc.copyToShape32(3);
   //   alloc.copyToShape32(sizeof(Point3F)-12); // handle alignment issues w/ point3f
   //}
   //defaultTranslations.set(ptr32,numNodes);

   defaultRotations.setSize(numNodes);
   for (i=0;i<numNodes;i++)
	   alloc.get16((S16*)&defaultRotations[i],4);
   defaultTranslations.setSize(numNodes);
   alloc.align32();
   for (i=0;i<numNodes;i++)
	   alloc.get32((S32*)&defaultTranslations[i],3);


   // get any node sequence data stored in shape
   nodeTranslations.setSize(numNodeTrans);
   for (i=0;i<numNodeTrans;i++)
      alloc.get32((S32*)&nodeTranslations[i],3);
   nodeRotations.setSize(numNodeRots);
   for (i=0;i<numNodeRots;i++)
      alloc.get16((S16*)&nodeRotations[i],4);
   alloc.align32();

   alloc.checkGuard();

   if (smReadVersion>21)
   {
      // more node sequence data...scale
      nodeUniformScales.setSize(numNodeUniformScales);
      for (i=0;i<numNodeUniformScales;i++)
         alloc.get32((S32*)&nodeUniformScales[i],1);
      nodeAlignedScales.setSize(numNodeAlignedScales);
      for (i=0;i<numNodeAlignedScales;i++)
         alloc.get32((S32*)&nodeAlignedScales[i],3);
      nodeArbitraryScaleFactors.setSize(numNodeArbitraryScales);
      for (i=0;i<numNodeArbitraryScales;i++)
         alloc.get32((S32*)&nodeArbitraryScaleFactors[i],3);
      nodeArbitraryScaleRots.setSize(numNodeArbitraryScales);
      for (i=0;i<numNodeArbitraryScales;i++)
         alloc.get16((S16*)&nodeArbitraryScaleRots[i],4);
      alloc.align32();

      alloc.checkGuard();
   }

   // old shapes need ground transforms moved to ground arrays...but only do it once
   if (smReadVersion<22 && alloc.allocShape32(0))
   {
      for (i=0; i<sequences.size(); i++)
      {
         // move ground transform data to ground vectors
         Sequence & seq = sequences[i];
         S32 oldSz = groundTranslations.size();
         groundTranslations.setSize(oldSz+seq.numGroundFrames);
         groundRotations.setSize(oldSz+seq.numGroundFrames);
         for (S32 j=0;j<seq.numGroundFrames;j++)
         {
            groundTranslations[j+oldSz] = nodeTranslations[seq.firstGroundFrame+j-numNodes];
            groundRotations[j+oldSz] = nodeRotations[seq.firstGroundFrame+j-numNodes];
         }
         seq.firstGroundFrame = oldSz;
         seq.baseTranslation -= numNodes;
         seq.baseRotation -= numNodes;
         seq.baseScale = 0; // not used on older shapes...but keep it clean
      }
   }

   // version 22 & 23 shapes accidentally had no ground transforms, and ground for
   // earlier shapes is handled just above, so...
   if (smReadVersion>23)
   {
      groundTranslations.setSize(numGroundFrames);
      for (i=0;i<numGroundFrames;i++)
         alloc.get32((S32*)&groundTranslations[i],3);
      groundRotations.setSize(numGroundFrames);
      for (i=0;i<numGroundFrames;i++)
         alloc.get16((S16*)&groundRotations[i],4);
      alloc.align32();

      alloc.checkGuard();
   }

   // object states
   //ptr32 = alloc.copyToShape32(numObjectStates*3);
   //objectStates.set(ptr32,numObjectStates);
   objectStates.setSize(numObjectStates);
    for (i=0;i<numObjectStates;i++)
	   alloc.get32((S32*)&objectStates[i],3);
   
   alloc.allocShape32(numSkins*3); // provide buffer after objectStates for older shapes
   alloc.checkGuard();

   // DEPRECATED decal states
   ptr32 = alloc.getPointer32(numDecalStates);

   alloc.checkGuard();

   // frame triggers
   ptr32 = alloc.getPointer32(numTriggers*2);
   triggers.setSize(numTriggers);
   dMemcpy(triggers.address(),ptr32,sizeof(S32)*numTriggers*2);

   alloc.checkGuard();

   // details
   ptr32 = alloc.copyToShape32(numDetails*7,true);
   details.set(ptr32,numDetails);

   alloc.checkGuard();

   // about to read in the meshes...first must allocate some scratch space
   S32 scratchSize = getMax(numSkins,numMeshes);
   TSMesh::smVertsList.setSize(scratchSize);
   TSMesh::smTVertsList.setSize(scratchSize);
   TSMesh::smNormsList.setSize(scratchSize);
   TSMesh::smEncodedNormsList.setSize(scratchSize);
   TSMesh::smDataCopied.setSize(scratchSize);
   TSSkinMesh::smInitTransformList.setSize(scratchSize);
   TSSkinMesh::smVertexIndexList.setSize(scratchSize);
   TSSkinMesh::smBoneIndexList.setSize(scratchSize);
   TSSkinMesh::smWeightList.setSize(scratchSize);
   TSSkinMesh::smNodeIndexList.setSize(scratchSize);
   for (i=0; i<numMeshes; i++)
   {
      TSMesh::smVertsList[i]=NULL;
      TSMesh::smTVertsList[i]=NULL;
      TSMesh::smNormsList[i]=NULL;
      TSMesh::smEncodedNormsList[i]=NULL;
      TSMesh::smDataCopied[i]=false;
      TSSkinMesh::smInitTransformList[i] = NULL;
      TSSkinMesh::smVertexIndexList[i] = NULL;
      TSSkinMesh::smBoneIndexList[i] = NULL;
      TSSkinMesh::smWeightList[i] = NULL;
      TSSkinMesh::smNodeIndexList[i] = NULL;
   }

   // read in the meshes (sans skins)...
   if (smReadVersion>15)
   {
      // straight forward read one at a time
      ptr32 = alloc.allocShape32(numMeshes + numSkins*numDetails); // leave room for skins on old shapes
      S32 curObject = 0; // for tracking skipped meshes
      for (i=0; i<numMeshes; i++)
      {
         bool skip = checkSkip(i,curObject,skipDL); // skip this mesh?
         S32 meshType = alloc.get32();
         if (meshType == TSMesh::DecalMeshType)
            // decal mesh deprecated
            skip = true;
         TSMesh * mesh = TSMesh::assembleMesh(meshType,skip);
         if (ptr32)
            ptr32[i] = skip ?  0 : (S32)mesh;

         // fill in location of verts, tverts, and normals for detail levels
         if (mesh && meshType!=TSMesh::DecalMeshType)
         {
            TSMesh::smVertsList[i]  = mesh->verts.address();
            TSMesh::smTVertsList[i] = mesh->tverts.address();
            TSMesh::smNormsList[i]  = mesh->norms.address();
            TSMesh::smEncodedNormsList[i] = mesh->encodedNorms.address();
            TSMesh::smDataCopied[i] = !skip; // as long as we didn't skip this mesh, the data should be in shape now
            if (meshType==TSMesh::SkinMeshType)
            {
               TSSkinMesh * skin = (TSSkinMesh*)mesh;
               TSMesh::smVertsList[i]  = skin->initialVerts.address();
               TSMesh::smNormsList[i]  = skin->initialNorms.address();
               TSSkinMesh::smInitTransformList[i] = skin->initialTransforms.address();
               TSSkinMesh::smVertexIndexList[i] = skin->vertexIndex.address();
               TSSkinMesh::smBoneIndexList[i] = skin->boneIndex.address();
               TSSkinMesh::smWeightList[i] = skin->weight.address();
               TSSkinMesh::smNodeIndexList[i] = skin->nodeIndex.address();
            }
         }
      }
      meshes.set(ptr32,numMeshes);
   }
   else
   {
      // use meshIndexList to contruct mesh list...
      ptr32 = alloc.allocShape32(meshIndexListSize + numSkins*numDetails);
      S32 next=0;
      S32 curObject = 0; // for tracking skipped meshes
      for (i=0; i<meshIndexListSize; i++)
      {
         bool skip = checkSkip(i,curObject,skipDL);
         if (meshIndexList[i]>=0)
         {
            AssertFatal(meshIndexList[i]==next,"TSShape::read: assertion failed on obsolete shape");
            S32 meshType = alloc.get32();
            if (meshType == TSMesh::DecalMeshType)
               // decal mesh deprecated
               skip = true;
            TSMesh * mesh = TSMesh::assembleMesh(meshType,skip);
            if (ptr32)
               ptr32[i] = skip ? 0 : (S32) mesh;
            next = meshIndexList[i]+1;

            // fill in location of verts, tverts, and normals for detail levels
            if (mesh && meshType!=TSMesh::DecalMeshType)
            {
               TSMesh::smVertsList[i]  = mesh->verts.address();
               TSMesh::smTVertsList[i] = mesh->tverts.address();
               TSMesh::smNormsList[i]  = mesh->norms.address();
               TSMesh::smEncodedNormsList[i]  = mesh->encodedNorms.address();
               TSMesh::smDataCopied[i] = !skip; // as long as we didn't skip this mesh, the data should be in shape now
               if (meshType==TSMesh::SkinMeshType)
               {
                  TSSkinMesh * skin = (TSSkinMesh*)mesh;
                  TSMesh::smVertsList[i]  = skin->initialVerts.address();
                  TSMesh::smNormsList[i]  = skin->initialNorms.address();
                  TSSkinMesh::smInitTransformList[i] = skin->initialTransforms.address();
                  TSSkinMesh::smVertexIndexList[i] = skin->vertexIndex.address();
                  TSSkinMesh::smBoneIndexList[i] = skin->boneIndex.address();
                  TSSkinMesh::smWeightList[i] = skin->weight.address();
                  TSSkinMesh::smNodeIndexList[i] = skin->nodeIndex.address();
               }
            }
         }
         else if (ptr32)
            ptr32[i] = 0; // no mesh
      }
      meshes.set(ptr32,meshIndexListSize);
   }

   alloc.checkGuard();

   // names
   bool namesInStringTable = (StringTable!=NULL);
   char * nameBufferStart = (char*)alloc.getPointer8(0);
   char * name = nameBufferStart;
   S32 nameBufferSize = 0;
   names.setSize(numNames);
   for (i=0; i<numNames; i++)
   {
      for (j=0; name[j]; j++)
         ;
      if (namesInStringTable)
         names[i] = StringTable->insert(name,false);
      nameBufferSize += j + 1;
      name += j + 1;
   }
   if (!namesInStringTable)
   {
      name = (char*)alloc.copyToShape8(nameBufferSize);
      if (name) // make sure we did copy (might just be getting size of buffer)
         for (i=0; i<numNames; i++)
         {
            for (j=0; name[j]; j++)
               ;
            names[i] = name;
            name += j + 1;
         }
   }
   else
      alloc.getPointer8(nameBufferSize);
   alloc.align32();

   alloc.checkGuard();

   if (smReadVersion<23)
   {
      // get detail information about skins...
      S32 * detailFirstSkin = alloc.getPointer32(numDetails);
      S32 * detailNumSkins = alloc.getPointer32(numDetails);

      alloc.checkGuard();

      // about to read in skins...clear out scratch space...
      if (numSkins)
      {
         TSSkinMesh::smInitTransformList.setSize(numSkins);
         TSSkinMesh::smVertexIndexList.setSize(numSkins);
         TSSkinMesh::smBoneIndexList.setSize(numSkins);
         TSSkinMesh::smWeightList.setSize(numSkins);
         TSSkinMesh::smNodeIndexList.setSize(numSkins);
      }
      for (i=0; i<numSkins; i++)
      {
         TSMesh::smVertsList[i]=NULL;
         TSMesh::smTVertsList[i]=NULL;
         TSMesh::smNormsList[i]=NULL;
         TSMesh::smEncodedNormsList[i]=NULL;
         TSMesh::smDataCopied[i]=false;
         TSSkinMesh::smInitTransformList[i] = NULL;
         TSSkinMesh::smVertexIndexList[i] = NULL;
         TSSkinMesh::smBoneIndexList[i] = NULL;
         TSSkinMesh::smWeightList[i] = NULL;
         TSSkinMesh::smNodeIndexList[i] = NULL;
      }

      // skins
      ptr32 = alloc.allocShape32(numSkins);
      for (i=0; i<numSkins; i++)
      {
         bool skip = i<detailFirstSkin[skipDL];
         TSSkinMesh * skin = (TSSkinMesh*)TSMesh::assembleMesh(TSMesh::SkinMeshType,skip);
         if (meshes.address())
         {
            // add pointer to skin in shapes list of meshes
            // we reserved room for this above...
            meshes.set(meshes.address(),meshes.size()+1);
            meshes[meshes.size()-1] = skip ? NULL : skin;
         }

         // fill in location of verts, tverts, and normals for shared detail levels
         if (skin)
         {
            TSMesh::smVertsList[i]  = skin->initialVerts.address();
            TSMesh::smTVertsList[i] = skin->tverts.address();
            TSMesh::smNormsList[i]  = skin->initialNorms.address();
            TSMesh::smEncodedNormsList[i]  = skin->encodedNorms.address();
            TSMesh::smDataCopied[i] = !skip; // as long as we didn't skip this mesh, the data should be in shape now
            TSSkinMesh::smInitTransformList[i] = skin->initialTransforms.address();
            TSSkinMesh::smVertexIndexList[i] = skin->vertexIndex.address();
            TSSkinMesh::smBoneIndexList[i] = skin->boneIndex.address();
            TSSkinMesh::smWeightList[i] = skin->weight.address();
            TSSkinMesh::smNodeIndexList[i] = skin->nodeIndex.address();
         }
      }

      alloc.checkGuard();

      // we now have skins in mesh list...add skin objects to object list and patch things up
      fixupOldSkins(numMeshes,numSkins,numDetails,detailFirstSkin,detailNumSkins);
   }

   // allocate storage space for some arrays (filled in during Shape::init)...
   ptr32 = alloc.allocShape32(numDetails);
   alphaIn.set(ptr32,numDetails);
   ptr32 = alloc.allocShape32(numDetails);
   alphaOut.set(ptr32,numDetails);
}

void TSShape::disassembleShape()
{
   CLockGuard guard(alloc.GetMutex());

   S32 i;

   // set counts...
   S32 numNodes = alloc.set32(nodes.size());
   S32 numObjects = alloc.set32(objects.size());
   alloc.set32(0); // DEPRECATED decals
   S32 numSubShapes = alloc.set32(subShapeFirstNode.size());
   S32 numIflMaterials = alloc.set32(iflMaterials.size());
   S32 numNodeRotations = alloc.set32(nodeRotations.size());
   S32 numNodeTranslations = alloc.set32(nodeTranslations.size());
   S32 numNodeUniformScales = alloc.set32(nodeUniformScales.size());
   S32 numNodeAlignedScales = alloc.set32(nodeAlignedScales.size());
   S32 numNodeArbitraryScales = alloc.set32(nodeArbitraryScaleFactors.size());
   S32 numGroundFrames = alloc.set32(groundTranslations.size());
   S32 numObjectStates = alloc.set32(objectStates.size());
   alloc.set32(0); // DEPRECATED decals
   S32 numTriggers = alloc.set32(triggers.size());
   S32 numDetails = alloc.set32(details.size());
   S32 numMeshes = alloc.set32(meshes.size());
   S32 numNames = alloc.set32(names.size());
   alloc.set32((S32)mSmallestVisibleSize);
   alloc.set32(mSmallestVisibleDL);

   alloc.setGuard();

   // get bounds...
   alloc.copyToBuffer32((S32*)&radius,1);
   alloc.copyToBuffer32((S32*)&tubeRadius,1);
   alloc.copyToBuffer32((S32*)&center,3);
   alloc.copyToBuffer32((S32*)&bounds,6);

   alloc.setGuard();

   // copy various vectors...
   alloc.copyToBuffer32((S32*)nodes.address(),numNodes*5);
   alloc.setGuard();
   alloc.copyToBuffer32((S32*)objects.address(),numObjects*6);
   alloc.setGuard();
   // DEPRECATED: no copy decals
   alloc.setGuard();
   alloc.copyToBuffer32((S32*)iflMaterials.address(),numIflMaterials*5);
   alloc.setGuard();
   alloc.copyToBuffer32((S32*)subShapeFirstNode.address(),numSubShapes);
   alloc.copyToBuffer32((S32*)subShapeFirstObject.address(),numSubShapes);
   alloc.copyToBuffer32((S32*)subShapeFirstObject.address(),numSubShapes); // DEPRECATED: no copy subShapeFirstDecal
   alloc.setGuard();
   alloc.copyToBuffer32((S32*)subShapeNumNodes.address(),numSubShapes);
   alloc.copyToBuffer32((S32*)subShapeNumObjects.address(),numSubShapes);
   alloc.copyToBuffer32((S32*)subShapeNumObjects.address(),numSubShapes); // DEPRECATED: no copy subShapeNumDecals
   alloc.setGuard();

   // default transforms...
   alloc.copyToBuffer16((S16*)defaultRotations.address(),numNodes*4);
   alloc.copyToBuffer32((S32*)defaultTranslations.address(),numNodes*3);

   // animated transforms...
   alloc.copyToBuffer16((S16*)nodeRotations.address(),numNodeRotations*4);
   alloc.copyToBuffer32((S32*)nodeTranslations.address(),numNodeTranslations*3);

   alloc.setGuard();

   // ...with scale
   alloc.copyToBuffer32((S32*)nodeUniformScales.address(),numNodeUniformScales);
   alloc.copyToBuffer32((S32*)nodeAlignedScales.address(),numNodeAlignedScales*3);
   alloc.copyToBuffer32((S32*)nodeArbitraryScaleFactors.address(),numNodeArbitraryScales*3);
   alloc.copyToBuffer16((S16*)nodeArbitraryScaleRots.address(),numNodeArbitraryScales*4);

   alloc.setGuard();

   alloc.copyToBuffer32((S32*)groundTranslations.address(),3*numGroundFrames);
   alloc.copyToBuffer16((S16*)groundRotations.address(),4*numGroundFrames);

   alloc.setGuard();

   // object states..
   alloc.copyToBuffer32((S32*)objectStates.address(),numObjectStates*3);
   alloc.setGuard();

   // decal states...
   // DEPRECATED (numDecalStates = 0)
   alloc.setGuard();

   // frame triggers
   alloc.copyToBuffer32((S32*)triggers.address(),numTriggers*2);
   alloc.setGuard();

   // details
   alloc.copyToBuffer32((S32*)details.address(),numDetails*7);
   alloc.setGuard();

   // read in the meshes (sans skins)...
   bool * isMesh = new bool[numMeshes]; // funny business because decals are pretend meshes (legacy issue)
   for (i=0;i<numMeshes;i++)
      isMesh[i]=false;
   for (i=0; i<objects.size(); i++)
   {
      for (S32 j=0; j<objects[i].numMeshes; j++)
         // even if an empty mesh, it's a mesh...
         isMesh[objects[i].startMeshIndex+j]=true;
   }
   for (i=0; i<numMeshes; i++)
   {
      TSMesh * mesh = NULL;
      // decal mesh deprecated
      if (isMesh[i])
         mesh = meshes[i];
      alloc.set32(mesh ? mesh->getMeshType() : TSMesh::NullMeshType);
      if (mesh)
         mesh->disassemble();
   }
   delete [] isMesh;
   alloc.setGuard();

   // names
   for (i=0; i<numNames; i++)
      alloc.copyToBuffer8((S8*)names[i],dStrlen(names[i])+1);
   alloc.setGuard();
}

//-------------------------------------------------
// write whole shape
//-------------------------------------------------
void TSShape::write(Stream * s)
{
   CLockGuard guard(alloc.GetMutex());
   // write version
   s->write(smVersion | (mExporterVersion<<16));

   alloc.setWrite();
   disassembleShape();

   S32     * buffer32 = alloc.getBuffer32();
   S16     * buffer16 = alloc.getBuffer16();
   S8      * buffer8  = alloc.getBuffer8();

   S32 size32 = alloc.getBufferSize32();
   S32 size16 = alloc.getBufferSize16();
   S32 size8  = alloc.getBufferSize8();

   // convert sizes to dwords...
   if (size16 & 1)
      size16 += 2;
   size16 >>= 1;
   if (size8 & 3)
      size8 += 4;
   size8 >>= 2;

   S32 sizeMemBuffer, start16, start8;
   sizeMemBuffer = size32 + size16 + size8;
   start16 = size32;
   start8 = start16+size16;

   // in dwords -- write will properly endian-flip.
   s->write(sizeMemBuffer);
   s->write(start16);
   s->write(start8);

	// endian-flip the entire write buffers.
   fixEndian(buffer32,buffer16,buffer8,size32,size16,size8);

   // now write buffers
   s->write(size32*4,buffer32);
   s->write(size16*4,buffer16);
   s->write(size8 *4,buffer8);

   // write sequences - write will properly endian-flip.
   s->write(sequences.size());
   for (S32 i=0; i<sequences.size(); i++)
      sequences[i].write(s);

   // write material list - write will properly endian-flip.
   materialList->write(*s);

   delete [] buffer32;
   delete [] buffer16;
   delete [] buffer8;
}

//-------------------------------------------------
// read whole shape
//-------------------------------------------------

bool TSShape::read(Stream * s)
{
   CLockGuard guard(alloc.GetMutex());   
   // read version - read handles endian-flip
   s->read(&smReadVersion);
   mExporterVersion = smReadVersion >> 16;
   smReadVersion &= 0xFF;
   if (smReadVersion>smVersion)
   {
      // error -- don't support future versions yet :>
      Con::errorf(ConsoleLogEntry::General,
                  "Error: attempt to load a version %i dts-shape, can currently only load version %i and before.",
                   smReadVersion,smVersion);
      return false;
   }
   mReadVersion = smReadVersion;

   S32 * memBuffer32;
   S16 * memBuffer16;
   S8 * memBuffer8;
   S32 count32, count16, count8;
   if (mReadVersion<19)
   {
   	Con::printf("... Shape with old version.");
      readOldShape(s,memBuffer32,memBuffer16,memBuffer8,count32,count16,count8);
   }
   else
   {
      S32 i;
      U32 sizeMemBuffer, startU16, startU8;

      // in dwords. - read handles endian-flip
      s->read(&sizeMemBuffer);
      s->read(&startU16);
      s->read(&startU8);

      if (s->getStatus()!=Stream::Ok)
      {
         Con::errorf(ConsoleLogEntry::General, "Error: bad shape file.");
         return false;
      }

      S32 * tmp = new S32[sizeMemBuffer];
      s->read(sizeof(S32)*sizeMemBuffer,(U8*)tmp);
      memBuffer32 = tmp;
      memBuffer16 = (S16*)(tmp+startU16);
      memBuffer8  = (S8*)(tmp+startU8);

      count32 = startU16;
      count16 = startU8-startU16;
      count8  = sizeMemBuffer-startU8;

      // read sequences
      S32 numSequences;
      s->read(&numSequences);
      sequences.setSize(numSequences);
      for (i=0; i<numSequences; i++)
      {
         constructInPlace(&sequences[i]);
         sequences[i].read(s);
      }

      // read material list
      delete materialList; // just in case...
      materialList = new TSMaterialList;
      materialList->read(*s);
   }
   
	// since we read in the buffers, we need to endian-flip their entire contents...
   fixEndian(memBuffer32,memBuffer16,memBuffer8,count32,count16,count8);

   alloc.setRead(memBuffer32,memBuffer16,memBuffer8,true);
   assembleShape(); // determine size of buffer needed
   S32 buffSize = alloc.getSize();
   alloc.doAlloc();
   mMemoryBlock = alloc.getBuffer();
   alloc.setRead(memBuffer32,memBuffer16,memBuffer8,false);
   assembleShape(); // copy to buffer
   AssertFatal(alloc.getSize()==buffSize,"TSShape::read: shape data buffer size mis-calculated");

   if (smReadVersion<19)
   {
      delete [] memBuffer32;
      delete [] memBuffer16;
      delete [] memBuffer8;
   }
   else
      delete [] memBuffer32; // this covers all the buffers

   if (smInitOnRead)
      init();

   //if (names.size() == 3 && dStricmp(names[2], "Box") == 0)
   //{
   //   Con::errorf("\nnodes.set(dMalloc(%d * sizeof(Node)), %d);", nodes.size(), nodes.size());
   //   for (U32 i = 0; i < nodes.size(); i++)
   //   {
   //      Node& obj = nodes[i];

   //      Con::errorf("   nodes[%d].nameIndex = %d;", i, obj.nameIndex);
   //      Con::errorf("   nodes[%d].parentIndex = %d;", i, obj.parentIndex);
   //      Con::errorf("   nodes[%d].firstObject = %d;", i, obj.firstObject);
   //      Con::errorf("   nodes[%d].firstChild = %d;", i, obj.firstChild);
   //      Con::errorf("   nodes[%d].nextSibling = %d;", i, obj.nextSibling);
   //   }

   //   Con::errorf("\nobjects.set(dMalloc(%d * sizeof(Object)), %d);", objects.size(), objects.size());
   //   for (U32 i = 0; i < objects.size(); i++)
   //   {
   //      Object& obj = objects[i];

   //      Con::errorf("   objects[%d].nameIndex = %d;", i, obj.nameIndex);
   //      Con::errorf("   objects[%d].numMeshes = %d;", i, obj.numMeshes);
   //      Con::errorf("   objects[%d].startMeshIndex = %d;", i, obj.startMeshIndex);
   //      Con::errorf("   objects[%d].nodeIndex = %d;", i, obj.nodeIndex);
   //      Con::errorf("   objects[%d].nextSibling = %d;", i, obj.nextSibling);
   //      Con::errorf("   objects[%d].firstDecal = %d;", i, obj.firstDecal);
   //   }

   //   //Con::errorf("numIflMaterials = %d", iflMaterials.size());
   //   Con::errorf("\nobjectStates.set(dMalloc(%d * sizeof(ObjectState)), %d);", objectStates.size(), objectStates.size());
   //   for (U32 i = 0; i < objectStates.size(); i++)
   //   {
   //      ObjectState& obj = objectStates[i];

   //      Con::errorf("   objectStates[%d].vis = %g;", i, obj.vis);
   //      Con::errorf("   objectStates[%d].frameIndex = %d;", i, obj.frameIndex);
   //      Con::errorf("   objectStates[%d].matFrameIndex = %d;", i, obj.matFrameIndex);
   //   }
   //   Con::errorf("\nsubShapeFirstNode.set(dMalloc(%d * sizeof(S32)), %d);", subShapeFirstNode.size(), subShapeFirstNode.size());
   //   for (U32 i = 0; i < subShapeFirstNode.size(); i++)
   //      Con::errorf("   subShapeFirstNode[%d] = %d;", i, subShapeFirstNode[i]);

   //   Con::errorf("\nsubShapeFirstObject.set(dMalloc(%d * sizeof(S32)), %d);", subShapeFirstObject.size(), subShapeFirstObject.size());
   //   for (U32 i = 0; i < subShapeFirstObject.size(); i++)
   //      Con::errorf("   subShapeFirstObject[%d] = %d;", i, subShapeFirstObject[i]);

   //   //Con::errorf("numDetailFirstSkins = %d", detailFirstSkin.size());
   //   Con::errorf("\nsubShapeNumNodes.set(dMalloc(%d * sizeof(S32)), %d);", subShapeNumNodes.size(), subShapeNumNodes.size());
   //   for (U32 i = 0; i < subShapeNumNodes.size(); i++)
   //      Con::errorf("   subShapeNumNodes[%d] = %d;", i, subShapeNumNodes[i]);

   //   Con::errorf("\nsubShapeNumObjects.set(dMalloc(%d * sizeof(S32)), %d);", subShapeNumObjects.size(), subShapeNumObjects.size());
   //   for (U32 i = 0; i < subShapeNumObjects.size(); i++)
   //      Con::errorf("   subShapeNumObjects[%d] = %d;", i, subShapeNumObjects[i]);

   //   Con::errorf("\ndetails.set(dMalloc(%d * sizeof(Detail)), %d);", details.size(), details.size());
   //   for (U32 i = 0; i < details.size(); i++)
   //   {
   //      Detail& obj = details[i];

   //      Con::errorf("   details[%d].nameIndex = %d;", i, obj.nameIndex);
   //      Con::errorf("   details[%d].subShapeNum = %d;", i, obj.subShapeNum);
   //      Con::errorf("   details[%d].objectDetailNum = %d;", i, obj.objectDetailNum);
   //      Con::errorf("   details[%d].size = %g;", i, obj.size);
   //      Con::errorf("   details[%d].averageError = %g;", i, obj.averageError);
   //      Con::errorf("   details[%d].maxError = %g;", i, obj.maxError);
   //      Con::errorf("   details[%d].polyCount = %d;", i, obj.polyCount);
   //   }

   //   Con::errorf("\ndefaultRotations.set(dMalloc(%d * sizeof(Quat16)), %d);", defaultRotations.size(), defaultRotations.size());
   //   for (U32 i = 0; i < defaultRotations.size(); i++)
   //   {
   //      Con::errorf("   defaultRotations[%d].x = %g;", i, defaultRotations[i].x);
   //      Con::errorf("   defaultRotations[%d].y = %g;", i, defaultRotations[i].y);
   //      Con::errorf("   defaultRotations[%d].z = %g;", i, defaultRotations[i].z);
   //      Con::errorf("   defaultRotations[%d].w = %g;", i, defaultRotations[i].w);
   //   }

   //   Con::errorf("\ndefaultTranslations.set(dMalloc(%d * sizeof(Point3F)), %d);", defaultTranslations.size(), defaultTranslations.size());
   //   for (U32 i = 0; i < defaultTranslations.size(); i++)
   //      Con::errorf("   defaultTranslations[%d].set(%g, %g, %g);", i, defaultTranslations[i].x, defaultTranslations[i].y, defaultTranslations[i].z);

   //   Con::errorf("\nsubShapeFirstTranslucentObject.set(dMalloc(%d * sizeof(S32)), %d);", subShapeFirstTranslucentObject.size(), subShapeFirstTranslucentObject.size());
   //   for (U32 i = 0; i < subShapeFirstTranslucentObject.size(); i++)
   //      Con::errorf("   subShapeFirstTranslucentObject[%d] = %d;", i, subShapeFirstTranslucentObject[i]);

   //   Con::errorf("\nmeshes.set(dMalloc(%d * sizeof(TSMesh)), %d);", meshes.size(), meshes.size());
   //   for (U32 i = 0; i < meshes.size(); i++)
   //   {
   //      TSMesh* obj = meshes[i];

   //      if (obj)
   //      {
   //         Con::errorf("   meshes[%d]->meshType = %d;", i, obj->meshType);
   //         Con::errorf("   meshes[%d]->mBounds.min.set(%g, %g, %g);", i, obj->mBounds.min.x, obj->mBounds.min.y, obj->mBounds.min.z);
   //         Con::errorf("   meshes[%d]->mBounds.max.set(%g, %g, %g);", i, obj->mBounds.max.x, obj->mBounds.max.y, obj->mBounds.max.z);
   //         Con::errorf("   meshes[%d]->mCenter.set(%g, %g, %g);", i, obj->mCenter.x, obj->mCenter.y, obj->mCenter.z);
   //         Con::errorf("   meshes[%d]->mRadius = %g;", i, obj->mRadius);
   //         Con::errorf("   meshes[%d]->mVisibility = %g;", i, obj->mVisibility);
   //         Con::errorf("   meshes[%d]->mDynamic = %d;", i, obj->mDynamic);
   //         Con::errorf("   meshes[%d]->parentMesh = %d;", i, obj->parentMesh);
   //         Con::errorf("   meshes[%d]->numFrames = %d;", i, obj->numFrames);
   //         Con::errorf("   meshes[%d]->numMatFrames = %d;", i, obj->numMatFrames);
   //         Con::errorf("   meshes[%d]->vertsPerFrame = %d;", i, obj->vertsPerFrame);

   //         Con::errorf("\n   meshes[%d]->verts.set(dMalloc(%d * sizeof(Point3F)), %d);", obj->verts.size(), obj->verts.size());
   //         for (U32 j = 0; j < obj->verts.size(); j++)
   //            Con::errorf("   meshes[%d]->verts[%d].set(%g, %g, %g);", i, j, obj->verts[j].x, obj->verts[j].y, obj->verts[j].z);

   //         Con::errorf("\n   meshes[%d]->norms.set(dMalloc(%d * sizeof(Point3F)), %d);", obj->norms.size(), obj->norms.size());
   //         for (U32 j = 0; j < obj->norms.size(); j++)
   //            Con::errorf("   meshes[%d]->norms[%d].set(%g, %g, %g);", i, j, obj->norms[j].x, obj->norms[j].y, obj->norms[j].z);

   //         Con::errorf("\n   meshes[%d]->tverts.set(dMalloc(%d * sizeof(Point2F)), %d);", obj->tverts.size(), obj->tverts.size());
   //         for (U32 j = 0; j < obj->tverts.size(); j++)
   //            Con::errorf("   meshes[%d]->tverts[%d].set(%g, %g);", i, j, obj->tverts[j].x, obj->tverts[j].y);

   //         Con::errorf("\n   meshes[%d]->primitives.set(dMalloc(%d * sizeof(TSDrawPrimitive)), %d);", obj->primitives.size(), obj->primitives.size());
   //         for (U32 j = 0; j < obj->primitives.size(); j++)
   //         {
   //            TSDrawPrimitive& prim = obj->primitives[j];

   //            Con::errorf("   meshes[%d]->primitives[%d].start = %d;", i, j, prim.start);
   //            Con::errorf("   meshes[%d]->primitives[%d].numElements = %d;", i, j, prim.numElements);
   //            Con::errorf("   meshes[%d]->primitives[%d].matIndex = %d;", i, j, prim.matIndex);
   //         }

   //         Con::errorf("\n   meshes[%d]->encodedNorms.set(dMalloc(%d * sizeof(U8)), %d);", obj->encodedNorms.size(), obj->encodedNorms.size());
   //         for (U32 j = 0; j < obj->encodedNorms.size(); j++)
   //            Con::errorf("   meshes[%d]->encodedNorms[%d] = %c;", i, j, obj->encodedNorms[j]);

   //         Con::errorf("\n   meshes[%d]->indices.set(dMalloc(%d * sizeof(U16)), %d);", obj->indices.size(), obj->indices.size());
   //         for (U32 j = 0; j < obj->indices.size(); j++)
   //            Con::errorf("   meshes[%d]->indices[%d] = %d;", i, j, obj->indices[j]);

   //         Con::errorf("\n   meshes[%d]->initialTangents.set(dMalloc(%d * sizeof(Point3F)), %d);", obj->initialTangents.size(), obj->initialTangents.size());
   //         for (U32 j = 0; j < obj->initialTangents.size(); j++)
   //            Con::errorf("   meshes[%d]->initialTangents[%d].set(%g, %g, %g);", i, j, obj->initialTangents[j].x, obj->initialTangents[j].y, obj->initialTangents[j].z);

   //         Con::errorf("\n   meshes[%d]->tangents.set(dMalloc(%d * sizeof(Point4F)), %d);", obj->tangents.size(), obj->tangents.size());
   //         for (U32 j = 0; j < obj->tangents.size(); j++)
   //            Con::errorf("   meshes[%d]->tangents[%d].set(%g, %g, %g, %g);", i, j, obj->tangents[j].x, obj->tangents[j].y, obj->tangents[j].z, obj->tangents[j].w);

   //         Con::errorf("   meshes[%d]->billboardAxis.set(%g, %g, %g);", i, obj->billboardAxis.x, obj->billboardAxis.y, obj->billboardAxis.z);

   //         Con::errorf("\n   meshes[%d]->planeNormals.set(dMalloc(%d * sizeof(Point3F)), %d);", obj->planeNormals.size(), obj->planeNormals.size());
   //         for (U32 j = 0; j < obj->planeNormals.size(); j++)
   //            Con::errorf("   meshes[%d]->planeNormals[%d].set(%g, %g, %g);", i, j, obj->planeNormals[j].x, obj->planeNormals[j].y, obj->planeNormals[j].z);

   //         Con::errorf("\n   meshes[%d]->planeConstants.set(dMalloc(%d * sizeof(F32)), %d);", obj->planeConstants.size(), obj->planeConstants.size());
   //         for (U32 j = 0; j < obj->planeConstants.size(); j++)
   //            Con::errorf("   meshes[%d]->planeConstants[%d] = %g;", i, j, obj->planeConstants[j]);

   //         Con::errorf("\n   meshes[%d]->planeMaterials.set(dMalloc(%d * sizeof(U32)), %d);", obj->planeMaterials.size(), obj->planeMaterials.size());
   //         for (U32 j = 0; j < obj->planeMaterials.size(); j++)
   //            Con::errorf("   meshes[%d]->planeMaterials[%d] = %d;", i, j, obj->planeMaterials[j]);

   //         Con::errorf("   meshes[%d]->planesPerFrame = %d;", i, obj->planesPerFrame);
   //         Con::errorf("   meshes[%d]->mergeBufferStart = %d;", i, obj->mergeBufferStart);
   //      }
   //   }

   //   Con::errorf("\nalphaIn.set(dMalloc(%d * sizeof(F32)), %d);", alphaIn.size(), alphaIn.size());
   //   for (U32 i = 0; i < alphaIn.size(); i++)
   //      Con::errorf("   alphaIn[%d] = %g;", i, alphaIn[i]);

   //   Con::errorf("\nalphaOut.set(dMalloc(%d * sizeof(F32)), %d);", alphaOut.size(), alphaOut.size());
   //   for (U32 i = 0; i < alphaOut.size(); i++)
   //      Con::errorf("   alphaOut[%d] = %g;", i, alphaOut[i]);

   //   //Con::errorf("numSequences = %d", sequences.size());
   //   //Con::errorf("numNodeRotations = %d", nodeRotations.size());
   //   //Con::errorf("numNodeTranslations = %d", nodeTranslations.size());
   //   //Con::errorf("numNodeUniformScales = %d", nodeUniformScales.size());
   //   //Con::errorf("numNodeAlignedScales = %d", nodeAlignedScales.size());
   //   //Con::errorf("numNodeArbitraryScaleRots = %d", nodeArbitraryScaleRots.size());
   //   //Con::errorf("numNodeArbitraryScaleFactors = %d", nodeArbitraryScaleFactors.size());
   //   //Con::errorf("numGroundRotations = %d", groundRotations.size());
   //   //Con::errorf("numGroundTranslations = %d", groundTranslations.size());
   //   //Con::errorf("numTriggers = %d", triggers.size());
   //   //Con::errorf("numIflFrameOffTimes = %d", iflFrameOffTimes.size());
   //   //Con::errorf("numBillboardDetails = %d", billboardDetails.size());

   //   //Con::errorf("\nnumDetailCollisionAccelerators = %d", detailCollisionAccelerators.size());
   //   //for (U32 i = 0; i < detailCollisionAccelerators.size(); i++)
   //   //{
   //   //   ConvexHullAccelerator* obj = detailCollisionAccelerators[i];

   //   //   if (obj)
   //   //   {
   //   //      Con::errorf("   detailCollisionAccelerators[%d].numVerts = %d", i, obj->numVerts);

   //   //      for (U32 j = 0; j < obj->numVerts; j++)
   //   //      {
   //   //         Con::errorf("      verts[%d](%g, %g, %g)", j, obj->vertexList[j].x, obj->vertexList[j].y, obj->vertexList[j].z);
   //   //         Con::errorf("      norms[%d](%g, %g, %g)", j, obj->normalList[j].x, obj->normalList[j].y, obj->normalList[j].z);
   //   //         //U8**     emitStrings;
   //   //      }
   //   //   }
   //   //}

   //   Con::errorf("\nnames.setSize(%d);", names.size());
   //   for (U32 i = 0; i < names.size(); i++)
   //      Con::errorf("   names[%d] = StringTable->insert(\"%s\");", i, names[i]);

   //   //TSMaterialList * materialList;

   //   Con::errorf("\nradius = %g;", radius);
   //   Con::errorf("tubeRadius = %g;", tubeRadius);
   //   Con::errorf("center.set(%g, %g, %g);", center.x, center.y, center.z);
   //   Con::errorf("bounds.min.set(%g, %g, %g);", bounds.min.x, bounds.min.y, bounds.min.z);
   //   Con::errorf("bounds.max.set(%g, %g, %g);", bounds.max.x, bounds.max.y, bounds.max.z);

   //   Con::errorf("\nmExporterVersion = %d;", mExporterVersion);
   //   Con::errorf("mSmallestVisibleSize = %g;", mSmallestVisibleSize);
   //   Con::errorf("mSmallestVisibleDL = %d;", mSmallestVisibleDL);
   //   Con::errorf("mReadVersion = %d;", mReadVersion);
   //   Con::errorf("mFlags = %d;", mFlags);
   //   //Con::errorf("data = %d", data);
   //   Con::errorf("mSequencesConstructed = %d;", mSequencesConstructed);
   //}

   return true;
}

void TSShape::createEmptyShape()
{
   nodes.set(dMalloc(1 * sizeof(Node)), 1);
      nodes[0].nameIndex = 1;
      nodes[0].parentIndex = -1;
      nodes[0].firstObject = 0;
      nodes[0].firstChild = -1;
      nodes[0].nextSibling = -1;

   objects.set(dMalloc(1 * sizeof(Object)), 1);
      objects[0].nameIndex = 2;
      objects[0].numMeshes = 1;
      objects[0].startMeshIndex = 0;
      objects[0].nodeIndex = 0;
      objects[0].nextSibling = -1;
      objects[0].firstDecal = -1;

   iflMaterials.set(NULL, 0);

   objectStates.set(dMalloc(1 * sizeof(ObjectState)), 1);
      objectStates[0].vis = 1;
      objectStates[0].frameIndex = 0;
      objectStates[0].matFrameIndex = 0;

   subShapeFirstNode.set(dMalloc(1 * sizeof(S32)), 1);
      subShapeFirstNode[0] = 0;

   subShapeFirstObject.set(dMalloc(1 * sizeof(S32)), 1);
      subShapeFirstObject[0] = 0;

   detailFirstSkin.set(NULL, 0);

   subShapeNumNodes.set(dMalloc(1 * sizeof(S32)), 1);
      subShapeNumNodes[0] = 1;

   subShapeNumObjects.set(dMalloc(1 * sizeof(S32)), 1);
      subShapeNumObjects[0] = 1;

   details.set(dMalloc(1 * sizeof(Detail)), 1);
      details[0].nameIndex = 0;
      details[0].subShapeNum = 0;
      details[0].objectDetailNum = 0;
      details[0].size = 2.0f;
      details[0].averageError = 0.0f;
      details[0].maxError = 0.0f;
      details[0].polyCount = 0;

   defaultRotations.set(dMalloc(1 * sizeof(Quat16)), 1);
      defaultRotations[0].x = 0.0f;
      defaultRotations[0].y = 0.0f;
      defaultRotations[0].z = 0.0f;
      defaultRotations[0].w = 0.0f;

   defaultTranslations.set(dMalloc(1 * sizeof(Point3F)), 1);
      defaultTranslations[0].set(0.0f, 0.0f, 0.0f);

   subShapeFirstTranslucentObject.set(dMalloc(1 * sizeof(S32)), 1);
      subShapeFirstTranslucentObject[0] = 1;

   alphaIn.set(dMalloc(1 * sizeof(F32)), 1);
      alphaIn[0] = 0;

   alphaOut.set(dMalloc(1 * sizeof(F32)), 1);
      alphaOut[0] = -1;

   sequences.set(NULL, 0);
   nodeRotations.set(NULL, 0);
   nodeTranslations.set(NULL, 0);
   nodeUniformScales.set(NULL, 0);
   nodeAlignedScales.set(NULL, 0);
   nodeArbitraryScaleRots.set(NULL, 0);
   nodeArbitraryScaleFactors.set(NULL, 0);
   groundRotations.set(NULL, 0);
   groundTranslations.set(NULL, 0);
   triggers.set(NULL, 0);
   iflFrameOffTimes.set(NULL, 0);
   billboardDetails.set(NULL, 0);

   names.setSize(3);
      names[0] = StringTable->insert("Detail2");
      names[1] = StringTable->insert("Mesh2");
      names[2] = StringTable->insert("Mesh");

   radius = 0.866025f;
   tubeRadius = 0.707107f;
   center.set(0.0f, 0.5f, 0.0f);
   bounds.min.set(-0.5f, 0.0f, -0.5f);
   bounds.max.set(0.5f, 1.0f, 0.5f);

   mExporterVersion = 124;
   mSmallestVisibleSize = 2;
   mSmallestVisibleDL = 0;
   mReadVersion = 24;
   mFlags = 0;
   mSequencesConstructed = 0;

   // Init the collision accelerator array.  Note that we don't compute the
   //  accelerators until the app requests them
   detailCollisionAccelerators.setSize(details.size());
   for (U32 i = 0; i < detailCollisionAccelerators.size(); i++)
      detailCollisionAccelerators[i] = NULL;
}

struct SimplePoint3
{
	F32 x, y, z;
};

SimplePoint3 anorms[] = { 
   { -0.525731f,  0.000000f,  0.850651f }, 
   { -0.442863f,  0.238856f,  0.864188f }, 
   { -0.295242f,  0.000000f,  0.955423f }, 
   { -0.309017f,  0.500000f,  0.809017f }, 
   { -0.162460f,  0.262866f,  0.951056f }, 
   {  0.000000f,  0.000000f,  1.000000f }, 
   {  0.000000f,  0.850651f,  0.525731f }, 
   { -0.147621f,  0.716567f,  0.681718f }, 
   {  0.147621f,  0.716567f,  0.681718f }, 
   {  0.000000f,  0.525731f,  0.850651f }, 
   {  0.309017f,  0.500000f,  0.809017f }, 
   {  0.525731f,  0.000000f,  0.850651f }, 
   {  0.295242f,  0.000000f,  0.955423f }, 
   {  0.442863f,  0.238856f,  0.864188f }, 
   {  0.162460f,  0.262866f,  0.951056f }, 
   { -0.681718f,  0.147621f,  0.716567f }, 
   { -0.809017f,  0.309017f,  0.500000f }, 
   { -0.587785f,  0.425325f,  0.688191f }, 
   { -0.850651f,  0.525731f,  0.000000f }, 
   { -0.864188f,  0.442863f,  0.238856f }, 
   { -0.716567f,  0.681718f,  0.147621f }, 
   { -0.688191f,  0.587785f,  0.425325f }, 
   { -0.500000f,  0.809017f,  0.309017f }, 
   { -0.238856f,  0.864188f,  0.442863f }, 
   { -0.425325f,  0.688191f,  0.587785f }, 
   { -0.716567f,  0.681718f, -0.147621f }, 
   { -0.500000f,  0.809017f, -0.309017f }, 
   { -0.525731f,  0.850651f,  0.000000f }, 
   {  0.000000f,  0.850651f, -0.525731f }, 
   { -0.238856f,  0.864188f, -0.442863f }, 
   {  0.000000f,  0.955423f, -0.295242f }, 
   { -0.262866f,  0.951056f, -0.162460f }, 
   {  0.000000f,  1.000000f,  0.000000f }, 
   {  0.000000f,  0.955423f,  0.295242f }, 
   { -0.262866f,  0.951056f,  0.162460f }, 
   {  0.238856f,  0.864188f,  0.442863f }, 
   {  0.262866f,  0.951056f,  0.162460f }, 
   {  0.500000f,  0.809017f,  0.309017f }, 
   {  0.238856f,  0.864188f, -0.442863f }, 
   {  0.262866f,  0.951056f, -0.162460f }, 
   {  0.500000f,  0.809017f, -0.309017f }, 
   {  0.850651f,  0.525731f,  0.000000f }, 
   {  0.716567f,  0.681718f,  0.147621f }, 
   {  0.716567f,  0.681718f, -0.147621f }, 
   {  0.525731f,  0.850651f,  0.000000f }, 
   {  0.425325f,  0.688191f,  0.587785f }, 
   {  0.864188f,  0.442863f,  0.238856f }, 
   {  0.688191f,  0.587785f,  0.425325f }, 
   {  0.809017f,  0.309017f,  0.500000f }, 
   {  0.681718f,  0.147621f,  0.716567f }, 
   {  0.587785f,  0.425325f,  0.688191f }, 
   {  0.955423f,  0.295242f,  0.000000f }, 
   {  1.000000f,  0.000000f,  0.000000f }, 
   {  0.951056f,  0.162460f,  0.262866f }, 
   {  0.850651f, -0.525731f,  0.000000f }, 
   {  0.955423f, -0.295242f,  0.000000f }, 
   {  0.864188f, -0.442863f,  0.238856f }, 
   {  0.951056f, -0.162460f,  0.262866f }, 
   {  0.809017f, -0.309017f,  0.500000f }, 
   {  0.681718f, -0.147621f,  0.716567f }, 
   {  0.850651f,  0.000000f,  0.525731f }, 
   {  0.864188f,  0.442863f, -0.238856f }, 
   {  0.809017f,  0.309017f, -0.500000f }, 
   {  0.951056f,  0.162460f, -0.262866f }, 
   {  0.525731f,  0.000000f, -0.850651f }, 
   {  0.681718f,  0.147621f, -0.716567f }, 
   {  0.681718f, -0.147621f, -0.716567f }, 
   {  0.850651f,  0.000000f, -0.525731f }, 
   {  0.809017f, -0.309017f, -0.500000f }, 
   {  0.864188f, -0.442863f, -0.238856f }, 
   {  0.951056f, -0.162460f, -0.262866f }, 
   {  0.147621f,  0.716567f, -0.681718f }, 
   {  0.309017f,  0.500000f, -0.809017f }, 
   {  0.425325f,  0.688191f, -0.587785f }, 
   {  0.442863f,  0.238856f, -0.864188f }, 
   {  0.587785f,  0.425325f, -0.688191f }, 
   {  0.688191f,  0.587785f, -0.425325f }, 
   { -0.147621f,  0.716567f, -0.681718f }, 
   { -0.309017f,  0.500000f, -0.809017f }, 
   {  0.000000f,  0.525731f, -0.850651f }, 
   { -0.525731f,  0.000000f, -0.850651f }, 
   { -0.442863f,  0.238856f, -0.864188f }, 
   { -0.295242f,  0.000000f, -0.955423f }, 
   { -0.162460f,  0.262866f, -0.951056f }, 
   {  0.000000f,  0.000000f, -1.000000f }, 
   {  0.295242f,  0.000000f, -0.955423f }, 
   {  0.162460f,  0.262866f, -0.951056f }, 
   { -0.442863f, -0.238856f, -0.864188f }, 
   { -0.309017f, -0.500000f, -0.809017f }, 
   { -0.162460f, -0.262866f, -0.951056f }, 
   {  0.000000f, -0.850651f, -0.525731f }, 
   { -0.147621f, -0.716567f, -0.681718f }, 
   {  0.147621f, -0.716567f, -0.681718f }, 
   {  0.000000f, -0.525731f, -0.850651f }, 
   {  0.309017f, -0.500000f, -0.809017f }, 
   {  0.442863f, -0.238856f, -0.864188f }, 
   {  0.162460f, -0.262866f, -0.951056f }, 
   {  0.238856f, -0.864188f, -0.442863f }, 
   {  0.500000f, -0.809017f, -0.309017f }, 
   {  0.425325f, -0.688191f, -0.587785f }, 
   {  0.716567f, -0.681718f, -0.147621f }, 
   {  0.688191f, -0.587785f, -0.425325f }, 
   {  0.587785f, -0.425325f, -0.688191f }, 
   {  0.000000f, -0.955423f, -0.295242f }, 
   {  0.000000f, -1.000000f,  0.000000f }, 
   {  0.262866f, -0.951056f, -0.162460f }, 
   {  0.000000f, -0.850651f,  0.525731f }, 
   {  0.000000f, -0.955423f,  0.295242f }, 
   {  0.238856f, -0.864188f,  0.442863f }, 
   {  0.262866f, -0.951056f,  0.162460f }, 
   {  0.500000f, -0.809017f,  0.309017f }, 
   {  0.716567f, -0.681718f,  0.147621f }, 
   {  0.525731f, -0.850651f,  0.000000f }, 
   { -0.238856f, -0.864188f, -0.442863f }, 
   { -0.500000f, -0.809017f, -0.309017f }, 
   { -0.262866f, -0.951056f, -0.162460f }, 
   { -0.850651f, -0.525731f,  0.000000f }, 
   { -0.716567f, -0.681718f, -0.147621f }, 
   { -0.716567f, -0.681718f,  0.147621f }, 
   { -0.525731f, -0.850651f,  0.000000f }, 
   { -0.500000f, -0.809017f,  0.309017f }, 
   { -0.238856f, -0.864188f,  0.442863f }, 
   { -0.262866f, -0.951056f,  0.162460f }, 
   { -0.864188f, -0.442863f,  0.238856f }, 
   { -0.809017f, -0.309017f,  0.500000f }, 
   { -0.688191f, -0.587785f,  0.425325f }, 
   { -0.681718f, -0.147621f,  0.716567f }, 
   { -0.442863f, -0.238856f,  0.864188f }, 
   { -0.587785f, -0.425325f,  0.688191f }, 
   { -0.309017f, -0.500000f,  0.809017f }, 
   { -0.147621f, -0.716567f,  0.681718f }, 
   { -0.425325f, -0.688191f,  0.587785f }, 
   { -0.162460f, -0.262866f,  0.951056f }, 
   {  0.442863f, -0.238856f,  0.864188f }, 
   {  0.162460f, -0.262866f,  0.951056f }, 
   {  0.309017f, -0.500000f,  0.809017f }, 
   {  0.147621f, -0.716567f,  0.681718f }, 
   {  0.000000f, -0.525731f,  0.850651f }, 
   {  0.425325f, -0.688191f,  0.587785f }, 
   {  0.587785f, -0.425325f,  0.688191f }, 
   {  0.688191f, -0.587785f,  0.425325f }, 
   { -0.955423f,  0.295242f,  0.000000f }, 
   { -0.951056f,  0.162460f,  0.262866f }, 
   { -1.000000f,  0.000000f,  0.000000f }, 
   { -0.850651f,  0.000000f,  0.525731f }, 
   { -0.955423f, -0.295242f,  0.000000f }, 
   { -0.951056f, -0.162460f,  0.262866f }, 
   { -0.864188f,  0.442863f, -0.238856f }, 
   { -0.951056f,  0.162460f, -0.262866f }, 
   { -0.809017f,  0.309017f, -0.500000f }, 
   { -0.864188f, -0.442863f, -0.238856f }, 
   { -0.951056f, -0.162460f, -0.262866f }, 
   { -0.809017f, -0.309017f, -0.500000f }, 
   { -0.681718f,  0.147621f, -0.716567f }, 
   { -0.681718f, -0.147621f, -0.716567f }, 
   { -0.850651f,  0.000000f, -0.525731f }, 
   { -0.688191f,  0.587785f, -0.425325f }, 
   { -0.587785f,  0.425325f, -0.688191f }, 
   { -0.425325f,  0.688191f, -0.587785f }, 
   { -0.425325f, -0.688191f, -0.587785f }, 
   { -0.587785f, -0.425325f, -0.688191f }, 
   { -0.688191f, -0.587785f, -0.425325f }
};

struct vertMap
{
   U16 oldVert;
   U16 oldTex;
   U16 newVert;
   U16 newTex;
};

struct md2Tri
{
   S16 vertIdx[3];
   S16 texCoordIdx[3];
};

bool TSShape::readMD2(Stream * s)
{
   S32 ident;                  // magic number: "IDP2"
   s->read(&ident);

   Con::printf("ident = %d", ident);

   S32 version;                // version: must be 8
   s->read(&version);

   Con::printf("version = %d", version);

   S32 skinWidth;              // texture width
   s->read(&skinWidth);
   
   S32 skinHeight;             // texture height
   s->read(&skinHeight);

   Con::printf("skinWidth = %d, skinHeight = %d", skinWidth, skinHeight);

   S32 framesize;              // size in bytes of a frame
   s->read(&framesize);

   S32 numSkins;               // number of skins
   s->read(&numSkins);
   S32 numVerts;               // number of vertices per frame
   s->read(&numVerts);
   S32 numTexCoords;           // number of texture coordinates
   s->read(&numTexCoords);
   S32 numTris;                // number of triangles
   s->read(&numTris);
   S32 numGLCmds;              // number of opengl commands
   s->read(&numGLCmds);
   S32 numFrames;              // number of frames
   s->read(&numFrames);

   Con::printf("numVerts = %d, numTris = %d", numVerts, numTris);

   S32 offsetSkins;            // offset skin data
   s->read(&offsetSkins);
   S32 offsetTexCoords;        // offset texture coordinate data
   s->read(&offsetTexCoords);
   S32 offsetTris;             // offset triangle data
   s->read(&offsetTris);
   S32 offsetFrames;           // offset frame data
   s->read(&offsetFrames);
   S32 offsetGLCmds;           // offset OpenGL command data
   s->read(&offsetGLCmds);
   S32 offsetEnd;              // offset end of file
   s->read(&offsetEnd);

   // First get our texture names
   s->setPosition(offsetSkins);

   for (U32 i = 0; i < numSkins; i++)
   {
      char texName[64];
      s->read(64, &texName);

      Con::errorf("texName[%d] = %s", i, texName);
   }

   createEmptyShape();

   // Have to set our polyCount
   if (details.size() > 0)
      details[0].polyCount = 32;

   // Build our mesh
   TSMesh* mesh = new TSMesh;

   mesh->meshType = 0;
   mesh->mBounds.min.set(-0.5f, 0.0f, -0.5f);
   mesh->mBounds.max.set(0.5f, 1.0f, 0.5f);
   mesh->mCenter.set(0.0f, 0.5f, 0.0f);
   mesh->mRadius = 0.0f;
   mesh->mVisibility = 1.0f;
   mesh->mDynamic = 0;
   mesh->parentMesh = -1;
   mesh->numFrames = 1;
   mesh->numMatFrames = 1;
   mesh->vertsPerFrame = numVerts;

   // Create our primitives (one per material)
   mesh->primitives.set(dMalloc(1 * sizeof(TSDrawPrimitive)), 1);
   mesh->primitives[0].start = 0;
   mesh->primitives[0].numElements = numTris * 3;
   // TODO: |= in the material index
   mesh->primitives[0].matIndex = TSDrawPrimitive::Triangles | TSDrawPrimitive::Indexed;

   // Need to load the texCoords first
   Vector<Point2F> texCoords;

   s->setPosition(offsetTexCoords);

   for (U32 i = 0; i < numTexCoords; i++)
   {
      S16 u;
      S16 v;

      s->read(&u);
      s->read(&v);

      texCoords.increment();
      // TODO: This is hardcoded for a 64x64 texture
      texCoords.last().set((F32)u / 64.0f, (F32)v / 64.0f);
   }

   // Go ahead and load our triangles
   Vector<md2Tri> tris;

   s->setPosition(offsetTris);

   for (U32 i = 0; i < numTris; i++)
   {
      tris.increment();

      s->read(&tris.last().vertIdx[1]);
      s->read(&tris.last().vertIdx[0]);
      s->read(&tris.last().vertIdx[2]);

      s->read(&tris.last().texCoordIdx[1]);
      s->read(&tris.last().texCoordIdx[0]);
      s->read(&tris.last().texCoordIdx[2]);
   }

   // Load our frames
   s->setPosition(offsetFrames);

   Point3F frameScale(1.0f, 1.0f, 1.0f);
   Point3F frameTranslate(0.0f, 0.0f, 0.0f);
   char frameName[16];

   mathRead(*s, &frameScale);
   mathRead(*s, &frameTranslate);
   s->read(16, &frameName);

   // Little bit of a magic number here...looks right in engine
   frameScale /= 32.0f;
   frameTranslate /= 32.0f;

   //frameTranslate.z += 0.48f;

   // Next load our verts and norms...also increment a tvert per vert
   Vector<Point3F> verts;
   Vector<Point3F> norms;
   Vector<Point2F> tverts;
   Vector<S32>     tvertMap;

   for (U32 i = 0; i < numVerts; i++)
   {
      U8 compVert[3];
      U8 normalIndex;

      s->read(&compVert[0]);
      s->read(&compVert[1]);
      s->read(&compVert[2]);

      s->read(&normalIndex);

      verts.increment();
      norms.increment();
      tverts.increment();
      tvertMap.increment();

      verts.last().x = (compVert[1] * frameScale.y) + frameTranslate.y;
      verts.last().y = (compVert[0] * frameScale.x) + frameTranslate.x;
      verts.last().z = (compVert[2] * frameScale.z) + frameTranslate.z;

      norms.last().x = anorms[normalIndex].y;
      norms.last().y = anorms[normalIndex].x;
      norms.last().z = anorms[normalIndex].z;

      tverts.last().set(0.0f, 0.0f);

      tvertMap.last() = -1;
   }

   // Now we need to suss out our texCoords. This gets a little tricky since
   // MD2 allows different UVs per triangle and 3space requires a 1:1 mapping
   // of verts to texCoords
   Vector<vertMap> newVertsMap;

   for (U32 i = 0; i < tris.size(); i++)
   {
      S16 vert0 = tris[i].vertIdx[0];
      S16 vert1 = tris[i].vertIdx[1];
      S16 vert2 = tris[i].vertIdx[2];

      S16 tvert0 = tris[i].texCoordIdx[0];
      S16 tvert1 = tris[i].texCoordIdx[1];
      S16 tvert2 = tris[i].texCoordIdx[2];

      // TODO: Build a newVertMap so we don't keep duplicating
      // vert + texCoord pairs that we have already duplicated
      if (tvertMap[vert0] == -1)
         tvertMap[vert0] = tvert0;
      else if (tvertMap[vert0] != tvert0)
      {
         // Ok...same vert, different texCoord index
         Point2F texCoordCurrent = texCoords[tvertMap[vert0]];
         Point2F texCoordNew = texCoords[tvert0];

         if (texCoordCurrent.x != texCoordNew.x || texCoordCurrent.y != texCoordNew.y)
         {
            verts.increment();
            verts.last() = verts[vert0];
            norms.increment();
            norms.last() = norms[vert0];
            tvertMap.push_back(tvert0);

            tris[i].vertIdx[0] = verts.size() - 1;
         }
      }
      if (tvertMap[vert1] == -1)
         tvertMap[vert1] = tvert1;
      else if (tvertMap[vert1] != tvert1)
      {
         // Ok...same vert, different texCoord index
         Point2F texCoordCurrent = texCoords[tvertMap[vert1]];
         Point2F texCoordNew = texCoords[tvert1];

         if (texCoordCurrent.x != texCoordNew.x || texCoordCurrent.y != texCoordNew.y)
         {
            verts.increment();
            verts.last() = verts[vert1];
            norms.increment();
            norms.last() = norms[vert1];
            tvertMap.push_back(tvert1);

            tris[i].vertIdx[1] = verts.size() - 1;
         }
      }
      if (tvertMap[vert2] == -1)
         tvertMap[vert2] = tvert2;
      else if (tvertMap[vert2] != tvert2)
      {
         // Ok...same vert, different texCoord index
         Point2F texCoordCurrent = texCoords[tvertMap[vert2]];
         Point2F texCoordNew = texCoords[tvert2];

         if (texCoordCurrent.x != texCoordNew.x || texCoordCurrent.y != texCoordNew.y)
         {
            verts.increment();
            verts.last() = verts[vert2];
            norms.increment();
            norms.last() = norms[vert2];
            tvertMap.push_back(tvert2);

            tris[i].vertIdx[2] = verts.size() - 1;
         }
      }
   }

   mesh->verts.set(dMalloc(verts.size() * sizeof(Point3F)), verts.size());

   for (U32 i = 0; i < verts.size(); i++)
      mesh->verts[i] = verts[i];

   mesh->norms.set(dMalloc(norms.size() * sizeof(Point3F)), norms.size());

   for (U32 i = 0; i < norms.size(); i++)
      mesh->norms[i] = norms[i];

   mesh->initialTangents.set(dMalloc(norms.size() * sizeof(Point3F)), norms.size());

   for (U32 i = 0; i < norms.size(); i++)
   {
      mesh->initialTangents[i].x = norms[i].y;
      mesh->initialTangents[i].y = norms[i].x;
      mesh->initialTangents[i].z = norms[i].z;
   }

   mesh->tangents.set(dMalloc(norms.size() * sizeof(Point4F)), norms.size());

   for (U32 i = 0; i < norms.size(); i++)
   {
      mesh->tangents[i].x = norms[i].y;
      mesh->tangents[i].y = norms[i].x;
      mesh->tangents[i].z = norms[i].z;
      mesh->tangents[i].w = -1;
   }

   mesh->tverts.set(dMalloc(tvertMap.size() * sizeof(Point2F)), tvertMap.size());

   for (U32 i = 0; i < tvertMap.size(); i++)
   {
      // It is apparently possible to have verts that are never used
      if (tvertMap[i] == -1)
         mesh->tverts[i] = Point2F(0.0f, 0.0f);
      else
         mesh->tverts[i] = texCoords[tvertMap[i]];
   }

   mesh->indices.set(dMalloc(tris.size() * 3 * sizeof(U16)), tris.size() * 3);

   for (U32 i = 0; i < tris.size(); i++)
   {
      mesh->indices[i * 3]     = tris[i].vertIdx[0];
      mesh->indices[i * 3 + 1] = tris[i].vertIdx[1];
      mesh->indices[i * 3 + 2] = tris[i].vertIdx[2];
   }

   // This is stuff that will stay the same from mesh-to-mesh (for now)
   mesh->encodedNorms.set(NULL, 0);
   mesh->billboardAxis.set(-0.00132704f, -0.00132704f, -0.00132704f);
   mesh->planeNormals.set(NULL, 0);
   mesh->planeConstants.set(NULL, 0);
   mesh->planeMaterials.set(NULL, 0);
   mesh->planesPerFrame = -1163005939;
   mesh->mergeBufferStart = -1163005939;

   // Need to do some post processing
   mesh->createVBIB();

   // Finally, allocate and set our new mesh
   meshes.set(dMalloc(1 * sizeof(TSMesh)), 1);
   meshes[0] = mesh;

   // Build our materialList
   materialList = new TSMaterialList;
   materialList->push_back("wizard", 67, 0);

   initMaterialList();

   return true;
}

void TSShape::fixEndian(S32 * buff32, S16 * buff16, S8 *, S32 count32, S32 count16, S32)
{
	// if endian-ness isn't the same, need to flip the buffer contents.
   if (0x12345678!=convertLEndianToHost(0x12345678))
   {
      for (S32 i=0; i<count32; i++)
         buff32[i]=convertLEndianToHost(buff32[i]);
      for (S32 i=0; i<count16*2; i++)
         buff16[i]=convertLEndianToHost(buff16[i]);
   }
}

// Read the .ifl material file
// The .ifl file (output by max) is essentially a keyframe list for
// the animation and contains the names of the textures along
// with a duration time.  This function parses the file to and appends
// the textures to the shape's master material list.
void TSShape::readIflMaterials(const char* shapePath)
{

   if (mFlags & IflInit)
      return;

   for (S32 i=0; i<iflMaterials.size(); i++)
   {
      IflMaterial & iflMaterial = iflMaterials[i];

      iflMaterial.firstFrame = materialList->size(); // next material added will be our first frame
      iflMaterial.firstFrameOffTimeIndex = iflFrameOffTimes.size(); // next entry added will be our first
      iflMaterial.numFrames = 0; // we'll increment as we read the file

	  U32 flag = materialList->getFlags(iflMaterial.materialSlot) | TSMaterialList::IflFrame;
      U32 totalTime = 0;

      // Fix slashes that face the wrong way
      char * pName = (char*)getName(iflMaterial.nameIndex);
      S32 len = dStrlen(pName);
      for (S32 j=0; j<len; j++)
         if (pName[j]=='\\')
            pName[j]='/';

      // Open the file which should be located in the same directory
      // as the .dts shape.
      // Tg: I cut out a some backwards compatibilty code dealing
      // with the old file prefixing. If someone is having compatibilty
      // problems, you may want to check the previous version of this
      // file.

      char nameBuffer[256];
      dSprintf(nameBuffer,sizeof(nameBuffer),"%s/%s",shapePath,pName);

      Stream * s = ResourceManager->openStream(nameBuffer);
      if (!s || s->getStatus() != Stream::Ok)
      {
         iflMaterial.firstFrame = iflMaterial.materialSlot; // avoid over-runs
         if (s)
            ResourceManager->closeStream(s);
         continue;
      }

      // find Material associated with this .ifl
      MaterialPropertyMap* matMap = MaterialPropertyMap::get();
      MatInstance *matInst = NULL;

      if( matMap == NULL )
      {
         Con::errorf(ConsoleLogEntry::General, "Error, cannot find the global material map object");
      }
      else
      {
         const MaterialPropertyMap::MapEntry * entry = matMap->getMapEntry(pName);

         if( entry )
         {
            Material *mat = dynamic_cast<Material*>( Sim::findObject( entry->materialName ) );
            if( mat )
            {
               matInst = new MatInstance( *mat );
            }
         }
      }

      // Parse the file, creat ifl "key" frames and append
      // texture names to the shape's material list.
      char buffer[512];
      U32 duration;
      bool setMat = false;
      while (s->getStatus() == Stream::Ok)
      {
         s->readLine((U8*)buffer,512);
         if (s->getStatus() == Stream::Ok || s->getStatus() == Stream::EOS)
         {
            char * pos = buffer;
            while (*pos)
            {
               if (*pos=='\t')
                  *pos=' ';
               pos++;
            }
            pos = buffer;

            // Find the name and duration start points
            while (*pos && *pos==' ')
               pos++;
            char * pos2 = dStrchr(pos,' ');
            if (pos2)
            {
               *pos2='\0';
               pos2++;
               while (*pos2 && *pos2==' ')
                  pos2++;
            }

            // skip line with no material
            if (!*pos)
               continue;

            // Extract frame duration
            duration = pos2 ? dAtoi(pos2) : 1;
            if (duration==0)
               // just in case...
               duration = 1;

            // Tg: I cut out a some backwards compatibilty code dealing
            // with the old file prefixing. If someone is having compatibilty
            // problems, you may want to check the previous version of this
            // file.
            // Strip off texture extension first.
            if ((pos2 = dStrchr(pos,'.')) != 0)
               *pos2 = '\0';

            materialList->push_back(pos,flag);

            // set Material if one is there
            if( matInst )
            {
               materialList->setMaterialInst( matInst, materialList->size() - 1 );
               setMat = true;  // if material is set, no need to free
            }
            //
            totalTime += duration;
            iflFrameOffTimes.push_back((1.0f/30.0f) * (F32)totalTime); // ifl units are frames (1 frame = 1/30 sec)
            iflMaterial.numFrames++;
         }
      }

      // If matInstance is not set anywhere, we need to free it
      if( !setMat )
      {
         delete matInst;
      }

      // close the file...
      ResourceManager->closeStream(s);
   }
   initMaterialList();
   mFlags |= IflInit;

}

ResourceInstance *constructTSShape(Stream &stream, ResourceObject *)
{
   TSShape * ret = new TSShape;
   if( !ret->read(&stream) )
   {
      delete ret;
      ret = NULL;
   }

   return ret;
}

ResourceInstance *constructTSShapeMD2(Stream &stream, ResourceObject *)
{
   TSShape * ret = new TSShape;
   if( !ret->readMD2(&stream) )
   {
      delete ret;
      ret = NULL;
   }

   return ret;
}

#if 1
TSShape::ConvexHullAccelerator* TSShape::getAccelerator(S32 dl)
{
   AssertFatal(dl < details.size(), "Error, bad detail level!");
   if (dl == -1)
      return NULL;

   if (detailCollisionAccelerators[dl] == NULL)
      computeAccelerator(dl);

   AssertFatal(detailCollisionAccelerators[dl] != NULL, "This should be non-null after computing it!");
   return detailCollisionAccelerators[dl];
}


void TSShape::computeAccelerator(S32 dl)
{
   AssertFatal(dl < details.size(), "Error, bad detail level!");

   // Have we already computed this?
   if (detailCollisionAccelerators[dl] != NULL)
      return;

   // Create a bogus features list...
   ConvexFeature cf;
   MatrixF mat(true);
   Point3F n(0, 0, 1);

   const TSDetail* detail = &details[dl];
   S32 ss = detail->subShapeNum;
   S32 od = detail->objectDetailNum;

   S32 start = subShapeFirstObject[ss];
   S32 end   = subShapeNumObjects[ss] + start;
   if (start < end)
   {
      // run through objects and collide
      // DMMNOTE: This assumes that the transform of the collision hulls is
      //  identity...
      U32 surfaceKey = 0;
      for (S32 i = start; i < end; i++)
      {
         const TSObject* obj = &objects[i];

         if (obj->numMeshes && od < obj->numMeshes) {
            TSMesh* mesh = meshes[obj->startMeshIndex + od];
            if (mesh)
               mesh->getFeatures(0, mat, n, &cf, surfaceKey);
         }
      }
   }

   Vector<Point3F> fixedVerts;
   VECTOR_SET_ASSOCIATION(fixedVerts);
   S32 i;
   for (i = 0; i < cf.mVertexList.size(); i++) {
      S32 j;
      bool found = false;
      for (j = 0; j < cf.mFaceList.size(); j++) {
         if (cf.mFaceList[j].vertex[0] == i ||
             cf.mFaceList[j].vertex[1] == i ||
             cf.mFaceList[j].vertex[2] == i) {
            found = true;
            break;
         }
      }
      if (!found)
         continue;

      found = false;
      for (j = 0; j < fixedVerts.size(); j++) {
         if (fixedVerts[j] == cf.mVertexList[i]) {
            found = true;
            break;
         }
      }
      if (found == true) {
         // Ok, need to replace any references to vertex i in the facelists with
         //  a reference to vertex j in the fixed list
         for (S32 k = 0; k < cf.mFaceList.size(); k++) {
            for (S32 l = 0; l < 3; l++) {
               if (cf.mFaceList[k].vertex[l] == i)
                  cf.mFaceList[k].vertex[l] = j;
            }
         }
      } else {
         for (S32 k = 0; k < cf.mFaceList.size(); k++) {
            for (S32 l = 0; l < 3; l++) {
               if (cf.mFaceList[k].vertex[l] == i)
                  cf.mFaceList[k].vertex[l] = fixedVerts.size();
            }
         }
         fixedVerts.push_back(cf.mVertexList[i]);
      }
   }
   cf.mVertexList.setSize(0);
   cf.mVertexList = fixedVerts;

   // Ok, so now we have a vertex list.  Lets copy that out...
   ConvexHullAccelerator* accel = new ConvexHullAccelerator;
   detailCollisionAccelerators[dl] = accel;
   accel->numVerts    = cf.mVertexList.size();
   accel->vertexList  = new Point3F[accel->numVerts];
   dMemcpy(accel->vertexList, cf.mVertexList.address(), sizeof(Point3F) * accel->numVerts);

   accel->normalList = new PlaneF[cf.mFaceList.size()];
   for (i = 0; i < cf.mFaceList.size(); i++)
      accel->normalList[i] = cf.mFaceList[i].normal;

   accel->emitStrings = new U8*[accel->numVerts];
   dMemset(accel->emitStrings, 0, sizeof(U8*) * accel->numVerts);

   for (i = 0; i < accel->numVerts; i++) {
      S32 j;

      Vector<U32> faces;
      VECTOR_SET_ASSOCIATION(faces);
      for (j = 0; j < cf.mFaceList.size(); j++) {
         if (cf.mFaceList[j].vertex[0] == i ||
             cf.mFaceList[j].vertex[1] == i ||
             cf.mFaceList[j].vertex[2] == i) {
            faces.push_back(j);
         }
      }
      AssertFatal(faces.size() != 0, "Huh?  Vertex unreferenced by any faces");

      // Insert all faces that didn't make the first cut, but share a plane with
      //  a face that's on the short list.
      for (j = 0; j < cf.mFaceList.size(); j++) {
         bool found = false;
         S32 k;
         for (k = 0; k < faces.size(); k++) {
            if (faces[k] == j)
               found = true;
         }
         if (found)
            continue;

         found = false;
         for (k = 0; k < faces.size(); k++) {
            if (mDot(accel->normalList[faces[k]], accel->normalList[j]) > 0.999) {
               found = true;
               break;
            }
         }
         if (found)
            faces.push_back(j);
      }

      Vector<U32> vertRemaps;
      VECTOR_SET_ASSOCIATION(vertRemaps);
      for (j = 0; j < faces.size(); j++) {
         for (U32 k = 0; k < 3; k++) {
            U32 insert = cf.mFaceList[faces[j]].vertex[k];
            bool found = false;
            for (S32 l = 0; l < vertRemaps.size(); l++) {
               if (insert == vertRemaps[l]) {
                  found = true;
                  break;
               }
            }
            if (!found)
               vertRemaps.push_back(insert);
         }
      }

      Vector<Point2I> edges;
      VECTOR_SET_ASSOCIATION(edges);
      for (j = 0; j < faces.size(); j++) {
         for (U32 k = 0; k < 3; k++) {
            U32 edgeStart = cf.mFaceList[faces[j]].vertex[(k + 0) % 3];
            U32 edgeEnd   = cf.mFaceList[faces[j]].vertex[(k + 1) % 3];

            U32 e0 = getMin(edgeStart, edgeEnd);
            U32 e1 = getMax(edgeStart, edgeEnd);

            bool found = false;
            for (S32 l = 0; l < edges.size(); l++) {
               if (edges[l].x == e0 && edges[l].y == e1) {
                  found = true;
                  break;
               }
            }
            if (!found)
               edges.push_back(Point2I(e0, e1));
         }
      }

      AssertFatal(vertRemaps.size() < 256 && faces.size() < 256 && edges.size() < 256,
                  "Error, ran over the shapebase assumptions about convex hulls.");

      U32 emitStringLen = 1 + vertRemaps.size()  +
                          1 + (edges.size() * 2) +
                          1 + (faces.size() * 4);
      accel->emitStrings[i] = new U8[emitStringLen];

      U32 currPos = 0;

      accel->emitStrings[i][currPos++] = vertRemaps.size();
      for (j = 0; j < vertRemaps.size(); j++)
         accel->emitStrings[i][currPos++] = vertRemaps[j];

      accel->emitStrings[i][currPos++] = edges.size();
      for (j = 0; j < edges.size(); j++) {
         S32 l;
         U32 old = edges[j].x;
         bool found = false;
         for (l = 0; l < vertRemaps.size(); l++) {
            if (vertRemaps[l] == old) {
               found = true;
               accel->emitStrings[i][currPos++] = l;
               break;
            }
         }
         AssertFatal(found, "Error, couldn't find the remap!");

         old = edges[j].y;
         found = false;
         for (l = 0; l < vertRemaps.size(); l++) {
            if (vertRemaps[l] == old) {
               found = true;
               accel->emitStrings[i][currPos++] = l;
               break;
            }
         }
         AssertFatal(found, "Error, couldn't find the remap!");
      }

      accel->emitStrings[i][currPos++] = faces.size();
      for (j = 0; j < faces.size(); j++) {
         accel->emitStrings[i][currPos++] = faces[j];
         for (U32 k = 0; k < 3; k++) {
            U32 old = cf.mFaceList[faces[j]].vertex[k];
            bool found = false;
            for (S32 l = 0; l < vertRemaps.size(); l++) {
               if (vertRemaps[l] == old) {
                  found = true;
                  accel->emitStrings[i][currPos++] = l;
                  break;
               }
            }
            AssertFatal(found, "Error, couldn't find the remap!");
         }
      }
      AssertFatal(currPos == emitStringLen, "Error, over/underflowed the emission string!");
   }
}
#endif

