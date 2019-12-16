//-----------------------------------------------------------------------------
// PowerEngine Advanced
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "ts/tsShape.h"
#include "materials/materialPropertyMap.h"
#include "materials/material.h"
#include "materials/matInstance.h"
#include "materials/sceneData.h"
#include "sceneGraph/sceneGraph.h"

static U32 defaultMatCount = 0;

TSMaterialList::TSMaterialList(U32 materialCount,
                               const char **materialNames,
                               const U32 * materialFlags,
                               const U32 * reflectanceMaps,
							   const U32 * specularMaps,
                               const U32 * bumpMaps,
                               const U32 * detailMaps,
                               const F32 * detailScales,
                               const F32 * reflectionAmounts,
							   const F32 * filterColors,
							   const U32 * sortedOrders,
							   bool bExport)
 : MaterialList(materialCount,materialNames),
   mNamesTransformed(false)
{
   VECTOR_SET_ASSOCIATION(mFlags);
   VECTOR_SET_ASSOCIATION(mReflectanceMaps);
   VECTOR_SET_ASSOCIATION(mSpecularMaps);
   VECTOR_SET_ASSOCIATION(mBumpMaps);
   VECTOR_SET_ASSOCIATION(mDetailMaps);
   VECTOR_SET_ASSOCIATION(mLightMaps);
   VECTOR_SET_ASSOCIATION(mDetailScales);
   VECTOR_SET_ASSOCIATION(mReflectionAmounts);
   VECTOR_SET_ASSOCIATION(mMaterialFilterColors);
   VECTOR_SET_ASSOCIATION(mSortedOrders);

   allocate(getMaterialCount());

   dMemcpy(mFlags.address(),materialFlags,getMaterialCount()*sizeof(U32));
   dMemcpy(mReflectanceMaps.address(),reflectanceMaps,getMaterialCount()*sizeof(U32));
   dMemcpy(mSpecularMaps.address(),specularMaps,getMaterialCount()*sizeof(U32));
   dMemcpy(mBumpMaps.address(),bumpMaps,getMaterialCount()*sizeof(U32));
   dMemcpy(mDetailMaps.address(),detailMaps,getMaterialCount()*sizeof(U32));
   for (U32 i = 0; i < getMaterialCount(); i++)
      mLightMaps[i] = 0xFFFFFFFF;
   dMemcpy(mDetailScales.address(),detailScales,getMaterialCount()*sizeof(F32));
   dMemcpy(mReflectionAmounts.address(),reflectionAmounts,getMaterialCount()*sizeof(F32));
   dMemcpy(mMaterialFilterColors.address(),filterColors,getMaterialCount()*sizeof(F32)*4);
   dMemcpy(mSortedOrders.address(),sortedOrders,getMaterialCount()*sizeof(U32));
}

TSMaterialList::TSMaterialList(U32 materialCount,
                               const char **materialNames,
                               const U32 * materialFlags,
                               const U32 * reflectanceMaps,
                               const U32 * bumpMaps,
                               const U32 * detailMaps,
                               const U32 * lightMaps,
                               const F32 * detailScales,
							   const F32 * reflectionAmounts,
							   const F32 * filterColors,
							   const U32 * sortedOrders)
: MaterialList(materialCount,materialNames),
   mNamesTransformed(false)
{
   VECTOR_SET_ASSOCIATION(mFlags);
   VECTOR_SET_ASSOCIATION(mReflectanceMaps);
   VECTOR_SET_ASSOCIATION(mSpecularMaps);
   VECTOR_SET_ASSOCIATION(mBumpMaps);
   VECTOR_SET_ASSOCIATION(mDetailMaps);
   VECTOR_SET_ASSOCIATION(mLightMaps);
   VECTOR_SET_ASSOCIATION(mDetailScales);
   VECTOR_SET_ASSOCIATION(mReflectionAmounts);
   VECTOR_SET_ASSOCIATION(mMaterialFilterColors);
   VECTOR_SET_ASSOCIATION(mSortedOrders);

   allocate(getMaterialCount());

   dMemcpy(mFlags.address(),materialFlags,getMaterialCount()*sizeof(U32));
   dMemcpy(mReflectanceMaps.address(),reflectanceMaps,getMaterialCount()*sizeof(U32));
   for (U32 i = 0; i < getMaterialCount(); i++)
	   mSpecularMaps[i] = 0xFFFFFFFF;
   dMemcpy(mBumpMaps.address(),bumpMaps,getMaterialCount()*sizeof(U32));
   dMemcpy(mDetailMaps.address(),detailMaps,getMaterialCount()*sizeof(U32));
   dMemcpy(mLightMaps.address(),lightMaps,getMaterialCount()*sizeof(U32));
   dMemcpy(mDetailScales.address(),detailScales,getMaterialCount()*sizeof(F32));
   dMemcpy(mReflectionAmounts.address(),reflectionAmounts,getMaterialCount()*sizeof(F32));
   dMemcpy(mMaterialFilterColors.address(),filterColors,getMaterialCount()*sizeof(F32)*4);
   dMemcpy(mSortedOrders.address(),sortedOrders,getMaterialCount()*sizeof(U32));
}

TSMaterialList::TSMaterialList()
   : mNamesTransformed(false)
{
   VECTOR_SET_ASSOCIATION(mFlags);
   VECTOR_SET_ASSOCIATION(mReflectanceMaps);
   VECTOR_SET_ASSOCIATION(mSpecularMaps);
   VECTOR_SET_ASSOCIATION(mBumpMaps);
   VECTOR_SET_ASSOCIATION(mDetailMaps);
   VECTOR_SET_ASSOCIATION(mLightMaps);
   VECTOR_SET_ASSOCIATION(mDetailScales);
   VECTOR_SET_ASSOCIATION(mReflectionAmounts);
   VECTOR_SET_ASSOCIATION(mMaterialFilterColors);
   VECTOR_SET_ASSOCIATION(mSortedOrders);
}

TSMaterialList::TSMaterialList(const TSMaterialList* pCopy)
   : MaterialList(pCopy)
{
   VECTOR_SET_ASSOCIATION(mFlags);
   VECTOR_SET_ASSOCIATION(mReflectanceMaps);
   VECTOR_SET_ASSOCIATION(mSpecularMaps);
   VECTOR_SET_ASSOCIATION(mBumpMaps);
   VECTOR_SET_ASSOCIATION(mDetailMaps);
   VECTOR_SET_ASSOCIATION(mDetailScales);
   VECTOR_SET_ASSOCIATION(mReflectionAmounts);
   VECTOR_SET_ASSOCIATION(mLightMaps);
   VECTOR_SET_ASSOCIATION(mMaterialFilterColors);
   VECTOR_SET_ASSOCIATION(mSortedOrders);

   mFlags             = pCopy->mFlags;
   mReflectanceMaps   = pCopy->mReflectanceMaps;
   mSpecularMaps	  = pCopy->mSpecularMaps;;
   mBumpMaps          = pCopy->mBumpMaps;
   mDetailMaps        = pCopy->mDetailMaps;
   mLightMaps         = pCopy->mLightMaps;
   mDetailScales      = pCopy->mDetailScales;
   mReflectionAmounts = pCopy->mReflectionAmounts;
   mNamesTransformed  = pCopy->mNamesTransformed;
   mMaterialFilterColors  = pCopy->mMaterialFilterColors;
   mSortedOrders	  = pCopy->mSortedOrders;
}

TSMaterialList::~TSMaterialList()
{
   free();
}

void TSMaterialList::free()
{
   // IflMaterials will duplicate names and textures found in other material slots
   // (In particular, IflFrame material slots).
   // Set the names to NULL now so our parent doesn't delete them twice.
   // Texture handles should stay as is...
   for (U32 i=0; i<getMaterialCount(); i++)
      if (mFlags[i] & TSMaterialList::IflMaterial)
         mMaterialNames[i] = NULL;

   // these aren't found on our parent, clear them out here to keep in synch
   mFlags.clear();
   mReflectanceMaps.clear();
   mSpecularMaps.clear();
   mBumpMaps.clear();
   mDetailMaps.clear();
   mLightMaps.clear();
   mDetailScales.clear();
   mReflectionAmounts.clear();
   mMaterialFilterColors.clear();
   mSortedOrders.clear();

   Parent::free();
}

void TSMaterialList::remap(U32 toIndex, U32 fromIndex)
{
   AssertFatal(toIndex < size() && fromIndex < size(),"TSMaterial::remap");

   // only remap texture handle...flags and maps should stay the same...

   mMaterials[toIndex] = mMaterials[fromIndex];
   mMaterialNames[toIndex] = mMaterialNames[fromIndex];
   mMatInstList[toIndex] = mMatInstList[fromIndex];
}

void TSMaterialList::push_back(const char * name, U32 flags, U32 rMap, U32 sMap,U32 bMap, U32 dMap, F32 dScale, F32 emapAmount, U32 lMap,ColorF filterColor,U32 sortOrder)
{
	Parent::push_back(name);
	mFlags.push_back(flags);
	if (rMap==0xFFFFFFFF)
		mReflectanceMaps.push_back(getMaterialCount()-1);
	else
		mReflectanceMaps.push_back(rMap);
	mSpecularMaps.push_back(sMap);
	mBumpMaps.push_back(bMap);
	mDetailMaps.push_back(dMap);
	mLightMaps.push_back(lMap);
	mDetailScales.push_back(dScale);
	mReflectionAmounts.push_back(emapAmount);
	mMaterialFilterColors.push_back(filterColor);
	mSortedOrders.push_back(sortOrder);
}


void TSMaterialList::allocate(U32 sz)
{
   mFlags.setSize(sz);
   mReflectanceMaps.setSize(sz);
   mSpecularMaps.setSize(sz);
   mBumpMaps.setSize(sz);
   mDetailMaps.setSize(sz);
   mLightMaps.setSize(sz);
   mDetailScales.setSize(sz);
   mReflectionAmounts.setSize(sz);
   mMaterialFilterColors.setSize(sz);
   mSortedOrders.setSize(sz);
}

void TSMaterialList::load(U32 index,const char* path)
{
   AssertFatal(index < getMaterialCount(),"TSMaterialList::getFlags: index out of range");

   if (mFlags[index] & TSMaterialList::NoMipMap)
      mTextureType = BitmapTexture;
   else if (mFlags[index] & TSMaterialList::MipMap_ZeroBorder)
      mTextureType = ZeroBorderTexture;
   else
      mTextureType = MeshTexture;

   Parent::load(index,path);
}

bool TSMaterialList::write(Stream & s)
{
   if (!Parent::write(s))
      return false;

   U32 i;
   for (i=0; i<getMaterialCount(); i++)
      s.write(mFlags[i]);

   for (i=0; i<getMaterialCount(); i++)
      s.write(mReflectanceMaps[i]);

   if(TSShape::smVersion>=30)
   {
	   for (i=0; i<getMaterialCount(); i++)
		   s.write(mSpecularMaps[i]);
   }

   for (i=0; i<getMaterialCount(); i++)
      s.write(mBumpMaps[i]);

   for (i=0; i<getMaterialCount(); i++)
      s.write(mDetailMaps[i]);

   for (i=0; i<getMaterialCount(); i++)
      s.write(mLightMaps[i]);
      
   for (i=0; i<getMaterialCount(); i++)
      s.write(mDetailScales[i]);

   for (i=0; i<getMaterialCount(); i++)
      s.write(mReflectionAmounts[i]);

   for (i=0; i<getMaterialCount(); i++)
	   s.write(mMaterialFilterColors[i]);

   for (i=0; i<getMaterialCount(); i++)
	   s.write(mSortedOrders[i]);

   return (s.getStatus() == Stream::Ok);
}

bool TSMaterialList::read(Stream & s)
{
   if (!Parent::read(s))
      return false;

   allocate(getMaterialCount());

   U32 i;
   if (TSShape::smReadVersion<2)
   {
      for (i=0; i<getMaterialCount(); i++)
         setFlags(i,S_Wrap|T_Wrap);
   }
   else
   {
      for (i=0; i<getMaterialCount(); i++)
         s.read(&mFlags[i]);
   }

   if (TSShape::smReadVersion<5)
   {
      for (i=0; i<getMaterialCount(); i++)
      {
         mReflectanceMaps[i] = i;
		 mSpecularMaps[i] = 0xFFFFFFFF;
         mBumpMaps[i] = 0xFFFFFFFF;
         mDetailMaps[i] = 0xFFFFFFFF;
         mLightMaps[i] = 0xFFFFFFFF;
      }
   }
   else
   {
      for (i=0; i<getMaterialCount(); i++)
         s.read(&mReflectanceMaps[i]);

	  if(TSShape::smReadVersion>=30)
	  {
		  for (i=0; i<getMaterialCount(); i++)
			  s.read(&mSpecularMaps[i]);
	  }
	  else
	  {
		  for (i=0; i<getMaterialCount(); i++)
			  mSpecularMaps[i] = 0xFFFFFFFF;
	  }

      for (i=0; i<getMaterialCount(); i++)
         s.read(&mBumpMaps[i]);
      for (i=0; i<getMaterialCount(); i++)
         s.read(&mDetailMaps[i]);

      if (TSShape::smReadVersion>24)
      {
         for (i=0; i<getMaterialCount(); i++)
            s.read(&mLightMaps[i]);
      }
      else
      {
         for (i=0; i<getMaterialCount(); i++)
            mLightMaps[i] = 0xFFFFFFFF;
      }
   }

   if (TSShape::smReadVersion>11)
   {
      for (i=0; i<getMaterialCount(); i++)
         s.read(&mDetailScales[i]);
   }
   else
   {
      for (i=0; i<getMaterialCount(); i++)
         mDetailScales[i] = 1.0f;
   }

   if (TSShape::smReadVersion>20)
   {
      for (i=0; i<getMaterialCount(); i++)
         s.read(&mReflectionAmounts[i]);
   }
   else
   {
      for (i=0; i<getMaterialCount(); i++)
         mReflectionAmounts[i] = 1.0f;
   }

   if (TSShape::smReadVersion>=32)
   {
	   for (i=0; i<getMaterialCount(); i++)
		   s.read(&mMaterialFilterColors[i]);

	   for (i=0; i<getMaterialCount(); i++)
		   s.read(&mSortedOrders[i]);
   }
   else
   {
	   for (i=0; i<getMaterialCount(); i++)
	   {
		   mMaterialFilterColors[i] = ColorF(1.0f,1.0f,1.0f);
		   mSortedOrders[i] = 0;
	   }
   }

   if (TSShape::smReadVersion<16)
   {
      // make sure emapping is off for translucent materials on old shapes
      for (i=0; i<getMaterialCount(); i++)
         if (mFlags[i] & TSMaterialList::Translucent)
            mFlags[i] |= TSMaterialList::NeverEnvMap;
   }

   // get rid of name of any ifl material names
   for (i=0; i<getMaterialCount(); i++)
   {
      const char * str = dStrrchr(mMaterialNames[i],'.');
      if (mFlags[i] & TSMaterialList::IflMaterial ||
          (TSShape::smReadVersion<6 && str && dStricmp(str,".ifl")==0))
      {
         delete [] mMaterialNames[i];
         mMaterialNames[i] = NULL;
      }
   }

   return (s.getStatus() == Stream::Ok);
}

#pragma message(ENGINE(支持导出指定是否背面剔除))
void TSMaterialList::prepareMaterial(int Index,Material *newMat)
{
	if(mFlags[Index] & NoFog)
		newMat->noFog = true;

	if(mFlags[Index] & NoCullBack)
		newMat->doubleSided = true;

	if(mFlags[Index] & SelfIlluminating)
	{
		newMat->emissive[0] = true;
	}

	if(mFlags[Index] & VertexColor)
	{
		newMat->enableVertexColor = true;
		//newMat->emissive[0] = true;
	}

	if(mFlags[Index] & EffectFlag)
	{
		newMat->emissive[0] = true;
        newMat->diffuse[0] = ColorF(1,1,1);
		newMat->effectLight = true;
		newMat->translucent = true;
		newMat->translucentZWrite =	false;
		newMat->translucentBlendOp = Material::LerpAlpha;
		newMat->alphaRef = 0;

		if(mFlags[Index] & FilterBlend)
		{
			newMat->enableFilterColor = true;
			newMat->filterColor = mMaterialFilterColors[Index];
			newMat->translucentBlendOp = Material::AddAlpha;
		}
	}
    newMat->filterColor.alpha = mMaterialFilterColors[Index].alpha; // color exposure

	if(mFlags[Index] & NoAlphaBland)
		newMat->translucentBlendOp = Material::None;
	else
	if(mFlags[Index] & MulBland)
		newMat->translucentBlendOp = Material::Mul;
	else
	if(mFlags[Index] & AddBland)
		newMat->translucentBlendOp = Material::Add;
	else
	if(mFlags[Index] & Additive)
		newMat->translucentBlendOp = Material::AddAlpha;
	else
	if(mFlags[Index] & Subtractive)
		newMat->translucentBlendOp = Material::Sub;
}

bool TSMaterialList::compareMaterialParam(int Index,Material *currentMat)
{
	if((mFlags[Index] & NoCullBack) && !currentMat->doubleSided)
		return false;

	if((mFlags[Index] & SelfIlluminating) && !currentMat->emissive[0])
		return false;

	if((mFlags[Index] & VertexColor) && !currentMat->enableVertexColor)
		return false;

	if((mFlags[Index] & EffectFlag) && !currentMat->effectLight)
		return false;

	if((mFlags[Index] & NoAlphaBland) && currentMat->translucentBlendOp != Material::None)
		return false;
	else
	if((mFlags[Index] & Additive) &&	currentMat->translucentBlendOp != Material::AddAlpha)
		return false;
	else
	if((mFlags[Index] & AddBland) &&	currentMat->translucentBlendOp != Material::Add)
		return false;
	else
	if((mFlags[Index] & MulBland) && currentMat->translucentBlendOp != Material::Mul)
		return false;
	else
	if((mFlags[Index] & Subtractive) && currentMat->translucentBlendOp != Material::Sub)
		return false;

	return true;
}

//--------------------------------------------------------------------------
// Sets the specified material in the list to the specified texture.  also 
// remaps mat instances based on the new texture name.  Returns false if 
// the specified texture is not valid.
//--------------------------------------------------------------------------
bool TSMaterialList::setMaterial(U32 i, const char* texPath)
{
   if (i < 0 || i > mMaterials.size())
      return false;

   if (texPath == NULL || texPath[0] == NULL)
      return false;

   // figure out new material name to make sure that we are actually changing the material
   StringTableEntry path;
   StringTableEntry filename;
   ResManager::getPaths(texPath, path, filename);

   if (dStrlen(filename) <= 0)
      return false;

   S32 length = dStrlen(filename)+1;
   char* matName = new char[length];
   dStrcpy(matName, length, filename);
   // eliminate extension from filename
   //char* ext = dStrrchr(matName, '.');
   //if (ext)
   //   *ext = 0;
   
   // ok, is our current material same as the supposedly new material?
   if (mMaterials[i].isValid() && dStrcmp(mMaterialNames[i], matName) == 0)
   {
      // same material, return true since we aren't changing it
      delete [] matName;
      return true;
   }

   GFXTexHandle tex( texPath, &GFXDefaultStaticDiffuseProfile );
   if (!tex.isValid())
   {
      delete [] matName;
      return false;
   }

   // change texture
   mMaterials[i] = tex;

   // change material name
   if (mMaterialNames[i])
      delete [] mMaterialNames[i];
   mMaterialNames[i] = matName;

   // dump the old mat instance
   if (mMatInstList[i])
      delete mMatInstList[i];
   mMatInstList[i] = NULL;

   // see if we can map it
   MaterialPropertyMap *matMap = MaterialPropertyMap::get();
   if( !matMap ) 
      return true;

   const MaterialPropertyMap::MapEntry * entry = matMap->getMapEntry(matName);
   // <Edit> [6/22/2009 joy] 更换纹理时生成新的MatInstance
   if(!(entry && entry->materialName) && mMaterials[i] )
   {
	   // No Material found, create new "default" material with just a baseTex
	   Material *newMat = new Material();
	   static char newMatName[32];
	   dSprintf( newMatName, 32, "Default_Material_%d", defaultMatCount++ );
	   newMat->assignName( newMatName );
	   newMat->mapTo = StringTable->insert( mMaterialNames[i] );
	   bool registered = newMat->registerObject();
	   AssertFatal( registered, "Unable to register material" );
	   Sim::getRootGroup()->addObject( newMat );

	   // overwrite baseTex in new material - hackish, but works
	   newMat->stages[0].tex[GFXShaderFeatureData::BaseTex] = mMaterials[i];
	   newMat->baseTexFilename[0] = mMaterials[i]->mTextureFileName;

	   // Set up some defaults for transparent textures
	   if (mMaterials[i]->hasTransparency)
	   {
		   newMat->translucent = true;
		   newMat->translucentBlendOp = Material::LerpAlpha;
		   newMat->translucentZWrite = true;
		   newMat->alphaRef = 20;
	   }

	   prepareMaterial(i,newMat); //Ray: fix bug

	   // create a MatInstance for the new material
	   MatInstance * matInst = new MatInstance( *newMat );
	   mMatInstList[i] = matInst;
	   // 再次查找
	   entry = matMap->getMapEntry(matName);
   }

   while(entry && entry->materialName)
   {
      Material *mat = dynamic_cast<Material*>( Sim::findObject( entry->materialName ) );
      if( mat )
      {
		  if(compareMaterialParam(i,mat))
		  {
			  MatInstance * matInst = new MatInstance( *mat );
			  mMatInstList[i] = matInst;

			  // initialize the new mat instance
			  SceneGraphData sgData;
			  sgData.setDefaultLights();
			  sgData.useLightDir = true;
			  sgData.useFog = SceneGraph::renderFog;

			  if(mat->enableVertexColor)
			  {
				  GFXVertexPCNT *tsVertex = NULL;
				  matInst->init( sgData, (GFXVertexFlags)getGFXVertFlags( tsVertex ) );
			  }
			  else
			  {
				  GFXVertexPNT *tsVertex = NULL;
				  matInst->init( sgData, (GFXVertexFlags)getGFXVertFlags( tsVertex ) );
			  }
			  break;
		  }
      }
	  entry = entry->pNext;
   }
   return true;
}


void TSMaterialList::mapMaterials()
{
	//Ray: 为了直接从插件导出材质，避免写材质脚本，做此修改
	//为了兼容没有做大改动

	MaterialPropertyMap *matMap = MaterialPropertyMap::get();
	if( !matMap ) return;

	mMatInstList.setSize( mMaterials.size() );

	//用基础材质作为材质名称
	U32 i;
	for( i=0; i<mReflectanceMaps.size(); i++ )
	{
		if( mMatInstList[i] != NULL ) continue;

		U32 Index = mReflectanceMaps[i];
		if(Index==0xFFFFFFFF)
		{
			mMatInstList[i] = NULL;
			continue;
		}			

		// lookup a material property entry
		const char * matName = getMaterialName(Index);

		// JMQ: this code assumes that all materials have names, but ifls don't (they are nuked by tsmateriallist loader code).  what to do?
		if( !matName )
		{
			mMatInstList[i] = NULL;
			continue;
		}

		bool newMatFlag = true;

        Material* pMatFinded = 0;

		const MaterialPropertyMap::MapEntry * entry = matMap->getMapEntry(matName);
		while(entry && entry->materialName)
		{
			Material *mat = dynamic_cast<Material*>( Sim::findObject( entry->materialName ) );

			if( mat )
			{
				if(compareMaterialParam(i,mat))
				{
					MatInstance * matInst = new MatInstance( *mat );
					mMatInstList[i] = matInst;
					newMatFlag = false;

                    pMatFinded = mat;
					break;
				}
			}
			else
			{
				mMatInstList[i] = NULL;
				newMatFlag = false;
				break;
			}

			entry = entry->pNext;
		}

        bool isNewMaterialInst = false;
		
        if (0 == pMatFinded && mMaterials[Index])
        {
            // No Material found, create new "default" material with just a baseTex
            Material* newMat = new Material();
            strncpy( newMat->mPath, mPath, 128 ); // 给出当前路径 [7/2/2009 LivenHotch]
            static char newMatName[32];
            dSprintf( newMatName, 32, "Default_Material_%d", defaultMatCount++ );
            newMat->assignName( newMatName );
            newMat->mapTo = StringTable->insert( mMaterialNames[i] );
            bool registered = newMat->registerObject();
            AssertFatal( registered, "Unable to register material" );
            Sim::getRootGroup()->addObject( newMat );
            
            isNewMaterialInst = true;
            pMatFinded = newMat;
        }

        //现在在切换场景的时候material中的纹理将会被释放
        //所以在把创建或查找material时，从新赋值纹理数据
        if (0 != pMatFinded && mMaterials[Index])
        {
            pMatFinded->stages[0].tex[GFXShaderFeatureData::BaseTex] = mMaterials[Index];
            pMatFinded->baseTexFilename[0] = mMaterials[Index]->mTextureFileName;

            Index = mBumpMaps[i];
            if(Index!=0xFFFFFFFF)
            {
                pMatFinded->stages[0].tex[GFXShaderFeatureData::BumpMap] = mMaterials[Index];
                pMatFinded->bumpFilename[0] = mMaterials[Index]->mTextureFileName;
            }

            Index = mSpecularMaps[i];
            if(Index!=0xFFFFFFFF)
            {
                pMatFinded->stages[0].tex[GFXShaderFeatureData::SpecularMap] = mMaterials[Index];
                pMatFinded->SpecularFilename[0] = mMaterials[Index]->mTextureFileName;
                pMatFinded->pixelSpecular[0] = true;
            }

			Index = mDetailMaps[i];
			if(Index!=0xFFFFFFFF)
			{
				pMatFinded->stages[0].tex[GFXShaderFeatureData::DetailMap] = mMaterials[Index];
				pMatFinded->detailFilename[0] = mMaterials[Index]->mTextureFileName;
			}
			

            if (isNewMaterialInst)
            {
                // Set up some defaults for transparent textures
                bool hasTransparency = mMaterials[i]->hasTransparency;
                if(!hasTransparency && (mFlags[i] & Translucent))
                {
                    if(mMaterials[i]->mFormat>=GFXFormatDXT1 && mMaterials[i]->mFormat<=GFXFormatDXT5)	//Ray: Fix DXT5 bug
                        hasTransparency = true;
                }

                if (hasTransparency)
                {
                    pMatFinded->translucent = true;
                    pMatFinded->translucentBlendOp =  Material::LerpAlpha;
                    pMatFinded->translucentZWrite = true;
                    pMatFinded->alphaRef = 20;
                }

                prepareMaterial(i,pMatFinded); //Ray: fix doubleSize bug

                // create a MatInstance for the new material
                MatInstance * matInst = new MatInstance( *pMatFinded );
                mMatInstList[i] = matInst;
            }
        }
	}

	for(;i<mMaterials.size();i++)
	{
		mMatInstList[i] = NULL;
	}
}


