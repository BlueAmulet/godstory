//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "platform/platform.h"
#include "core/resManager.h"
#include "core/stream.h"
#include "materials/materialList.h"
#include "materialPropertyMap.h"
#include "material.h"
#include "materials/sceneData.h"
#include "materials/matInstance.h"

//--------------------------------------
MaterialList::MaterialList()
{
   mTextureType = BitmapTexture;
   mClampToEdge = false;

   VECTOR_SET_ASSOCIATION(mMaterialNames);
   VECTOR_SET_ASSOCIATION(mMaterials);
}

MaterialList::MaterialList(const MaterialList* pCopy)
{
   VECTOR_SET_ASSOCIATION(mMaterialNames);
   VECTOR_SET_ASSOCIATION(mMaterials);

   S32 length = 0;

   mClampToEdge = pCopy->mClampToEdge;
   mTextureType = pCopy->mTextureType;

   mMaterialNames.setSize(pCopy->mMaterialNames.size());
   S32 i;
   for (i = 0; i < mMaterialNames.size(); i++) {
      if (pCopy->mMaterialNames[i]) {
	     length = dStrlen(pCopy->mMaterialNames[i]) + 1;
         mMaterialNames[i] = new char[length];
         dStrcpy(mMaterialNames[i], length, pCopy->mMaterialNames[i]);
      } else {
         mMaterialNames[i] = NULL;
      }
   }

   mMaterials.setSize(pCopy->mMaterials.size());
   for (i = 0; i < mMaterials.size(); i++) {
      constructInPlace(&mMaterials[i]);
      mMaterials[i] = pCopy->mMaterials[i];
   }

   clearMatInstList();
   mMatInstList.setSize(pCopy->mMaterials.size());
   for( i = 0; i < mMatInstList.size(); i++ )
   {
      if( i < pCopy->mMatInstList.size() && pCopy->mMatInstList[i] )
      {
         mMatInstList[i] = new MatInstance( *pCopy->mMatInstList[i]->getMaterial() );
      }
      else
      {
         mMatInstList[i] = NULL;
      }
   }

}



MaterialList::MaterialList(U32 materialCount, const char **materialNames)
{
   VECTOR_SET_ASSOCIATION(mMaterialNames);
   VECTOR_SET_ASSOCIATION(mMaterials);

   set(materialCount, materialNames);
}


//--------------------------------------
void MaterialList::set(U32 materialCount, const char **materialNames)
{
   S32 length = 0;
   free();
   mMaterials.setSize(materialCount);
   mMaterialNames.setSize(materialCount);
   clearMatInstList();
   mMatInstList.setSize(materialCount);
   for(U32 i = 0; i < materialCount; i++)
   {
      // vectors DO NOT initialize classes so manually call the constructor
      constructInPlace(&mMaterials[i]);
	  length = dStrlen(materialNames[i]) + 1;
      mMaterialNames[i] = new char[length];
      dStrcpy(mMaterialNames[i], length, materialNames[i]);
      mMatInstList[i] = NULL;
   }
}


//--------------------------------------
MaterialList::~MaterialList()
{
   free();
}


//--------------------------------------
extern bool gPreviewMission;
void MaterialList::load(U32 index, const char* path)
{
   AssertFatal(index < size(), "MaterialList:: index out of range.");
   if (index < size())
   {
      GFXTexHandle &handle = mMaterials[index];
      if (handle.isNull())
      {
         const char *name = mMaterialNames[index];
         if (name && *name)
         {
            if (path) {
               char buffer[512];
               dSprintf(buffer, sizeof(buffer), "%s/%s", path , name);
#ifdef NTJ_EDITOR
               if ( gPreviewMission )
               {
                  MaterialPropertyMap *matMap = MaterialPropertyMap::get();
                  if ( matMap )
                     matMap->delMapping(name);

                  ResourceObject *ro = GBitmap::findBmpResource(buffer);
                  if (ro)
                  {
                     StringTableEntry fileName = ro->getFullPath();
                     ResourceManager->reload(fileName,true);
                     GFX->getTextureManager()->deleteTexture(fileName);
                  }
               }
#endif
               handle.set(buffer, &GFXDefaultStaticDiffuseProfile);
            }
            else
            {
#ifdef NTJ_EDITOR
                if ( gPreviewMission )
                {
                    MaterialPropertyMap *matMap = MaterialPropertyMap::get();
                    if ( matMap )
                        matMap->delMapping(name);

                    ResourceObject *ro = GBitmap::findBmpResource(name);
                    if (ro)
                    {
                        StringTableEntry fileName = ro->getFullPath();
                        ResourceManager->reload(fileName,true);
                        GFX->getTextureManager()->deleteTexture(fileName);
                    }
                }
#endif
               handle.set(name, &GFXDefaultStaticDiffuseProfile);
            }
         }
      }
   }
}


//--------------------------------------
bool MaterialList::load(const char* path)
{
   AssertFatal(mMaterials.size() == mMaterials.size(), "MaterialList::load: internal vectors out of sync.");

   for(S32 i=0; i < mMaterials.size(); i++)
      load(i,path);

   for(S32 i=0; i < mMaterials.size(); i++)
   {
      // TSMaterialList nulls out the names of IFL materials, so
      // we need to ignore empty names.
      const char *name = mMaterialNames[i];
      if (name && *name && !mMaterials[i])
         return false;
   }
   return true;
}


//--------------------------------------
void MaterialList::unload()
{
   AssertFatal(mMaterials.size() == mMaterials.size(), "MaterialList::unload: internal vectors out of sync.");
   for(S32 i=0; i < mMaterials.size(); i++)
      mMaterials[i].free();
}


//--------------------------------------
void MaterialList::free()
{
   AssertFatal(mMaterials.size() == mMaterialNames.size(), "MaterialList::free: internal vectors out of sync.");
   for(S32 i=0; i < mMaterials.size(); i++)
   {
      if(mMaterialNames[i])
      {
         delete [] mMaterialNames[i];
      }
      mMaterials[i] = NULL;
   }
   clearMatInstList();
   mMatInstList.setSize(0);
   mMaterialNames.setSize(0);
   mMaterials.setSize(0);
}


//--------------------------------------
U32 MaterialList::push_back(GFXTexHandle textureHandle, const char * filename)
{
   S32 length = 0;
   mMaterials.increment();
   mMaterialNames.increment();
   mMatInstList.push_back(NULL);

   // vectors DO NOT initialize classes so manually call the constructor
   constructInPlace(&mMaterials.last());
   mMaterials.last()    = textureHandle;
   length = dStrlen(filename) + 1;
   mMaterialNames.last() = new char[length];
   dStrcpy(mMaterialNames.last(), length, filename);

   // return the index
   return mMaterials.size()-1;
}

//--------------------------------------
U32 MaterialList::push_back(const char *filename)
{
   S32 length = 0;
   mMaterials.increment();
   mMaterialNames.increment();
   mMatInstList.push_back(NULL);

   // vectors DO NOT initialize classes so manually call the constructor
   constructInPlace(&mMaterials.last());
   length = dStrlen(filename) + 1;
   mMaterialNames.last() = new char[length];
   dStrcpy(mMaterialNames.last(), length, filename);

   // return the index
   return mMaterials.size()-1;
}

//--------------------------------------
bool MaterialList::read(Stream &stream)
{
   free();

   S32 length = 0;
   // check the stream version
   U8 version;
   if ( stream.read(&version) && version != BINARY_FILE_VERSION)
      return readText(stream,version);

   // how many materials?
   U32 count;
   if ( !stream.read(&count) )
      return false;

   // pre-size the vectors for efficiency
   mMaterials.reserve(count);
   mMaterialNames.reserve(count);

   // read in the materials
   for (U32 i=0; i<count; i++)
   {
      // Load the bitmap name
      char buffer[256];
      stream.readString(buffer,256);
      if( !buffer[0] )
      {
         AssertWarn(0, "MaterialList::read: error reading stream");
         return false;
      }

      // Material paths are a legacy of Tribes tools,
      // strip them off...
      char *name = &buffer[dStrlen(buffer)];
      while (name != buffer && name[-1] != '/' && name[-1] != '\\')
         name--;

      // Add it to the list
      mMaterials.increment();
      mMaterialNames.increment();
      mMatInstList.push_back(NULL);
      // vectors DO NOT initialize classes so manually call the constructor
      constructInPlace(&mMaterials.last());
	  length = dStrlen(name) + 1;
      mMaterialNames.last() = new char[length];
      dStrcpy(mMaterialNames.last(), length, name);
   }

   return (stream.getStatus() == Stream::Ok);
}


//--------------------------------------
bool MaterialList::write(Stream &stream)
{
   AssertFatal(mMaterials.size() == mMaterialNames.size(), "MaterialList::write: internal vectors out of sync.");

   stream.write((U8)BINARY_FILE_VERSION);    // version
   stream.write((U32)mMaterials.size());     // material count

   for(S32 i=0; i < mMaterials.size(); i++)  // material names
      stream.writeString(mMaterialNames[i]);

   return (stream.getStatus() == Stream::Ok);
}


//--------------------------------------
bool MaterialList::readText(Stream &stream, U8 firstByte)
{
   free();

   S32 length = 0;

   if (!firstByte)
      return (stream.getStatus() == Stream::Ok || stream.getStatus() == Stream::EOS);

   char buf[1024];
   buf[0] = firstByte;
   U32 offset = 1;

   for(;;)
   {
      stream.readLine((U8*)(buf+offset), sizeof(buf)-offset);
      if(!buf[0])
         break;
      offset = 0;

      // Material paths are a legacy of Tribes tools,
      // strip them off...
      char *name = &buf[dStrlen(buf)];
      while (name != buf && name[-1] != '/' && name[-1] != '\\')
         name--;

      // Add it to the list
      mMaterials.increment();
      mMaterialNames.increment();
      mMatInstList.push_back(NULL);
      // vectors DO NOT initialize classes so manually call the constructor
      constructInPlace(&mMaterials.last());
	  length = dStrlen(name) + 1;
      mMaterialNames.last() = new char[length];
      dStrcpy(mMaterialNames.last(), length, name);
   }
   return (stream.getStatus() == Stream::Ok || stream.getStatus() == Stream::EOS);
}

bool MaterialList::readText(Stream &stream)
{
   U8 firstByte;
   stream.read(&firstByte);
   return readText(stream,firstByte);
}

//--------------------------------------
bool MaterialList::writeText(Stream &stream)
{
   AssertFatal(mMaterials.size() == mMaterialNames.size(), "MaterialList::writeText: internal vectors out of sync.");

   for(S32 i=0; i < mMaterials.size(); i++)
      stream.writeLine((U8*)mMaterialNames[i]);
   stream.writeLine((U8*)"");

   return (stream.getStatus() == Stream::Ok);
}


//--------------------------------------
ResourceInstance* constructMaterialList(Stream &stream)
{
   MaterialList *matList = new MaterialList;
   if(matList->readText(stream))
      return matList;
   else
   {
      delete matList;
      return NULL;
   }
}

//--------------------------------------------------------------------------
// Clear all materials in the mMatInstList member variable
//--------------------------------------------------------------------------
void MaterialList::clearMatInstList()
{
   // clear out old materials.  any non null element of the list should be pointing at deletable memory,
   // although multiple indexes may be pointing at the same memory so we have to be careful (see
   // comment in loop body)
   for (U32 i=0; i<mMatInstList.size(); i++)
   {
      if (mMatInstList[i])
      {
         MatInstance* current = mMatInstList[i];
         delete current;
         mMatInstList[i] = NULL;

         // ok, since ts material lists can remap difference indexes to the same object (e.g. for ifls),
         // we need to make sure that we don't delete the same memory twice.  walk the rest of the list
         // and null out any pointers that match the one we deleted.
         for (U32 j=0; j<mMatInstList.size(); j++)
            if (mMatInstList[j] == current)
               mMatInstList[j] = NULL;
      }
   }
}

//--------------------------------------------------------------------------
// Map materials - map materials to the textures in the list
//--------------------------------------------------------------------------
void MaterialList::mapMaterials()
{

   MaterialPropertyMap *matMap = MaterialPropertyMap::get();
   if( !matMap ) return;
   
//   clearMatInstList();  // don't clear this anymore, .ifls need it

   mMatInstList.setSize( mMaterials.size() );

   for( U32 i=0; i<mMaterials.size(); i++ )
   {
      if( mMatInstList[i] != NULL ) continue;

      // lookup a material property entry
      const char * matName = getMaterialName(i);

      // JMQ: this code assumes that all materials have names, but ifls don't (they are nuked by tsmateriallist loader code).  what to do?
      if( !matName )
      {
         mMatInstList[i] = NULL;
         continue;
      }

      const MaterialPropertyMap::MapEntry * entry = matMap->getMapEntry(matName);

      if(entry && entry->materialName)
      {
         Material *mat = dynamic_cast<Material*>( Sim::findObject( entry->materialName ) );
         if( mat )
         {
            MatInstance * matInst = new MatInstance( *mat );
            mMatInstList[i] = matInst;
         }
         else
         {
            mMatInstList[i] = NULL;
         }
      }
      else
      {
         if( mMaterials[i] )
         {
            // No Material found, create new "default" material with just a baseTex
            Material *newMat = new Material();
            static U32 defaultMatCount = 0;
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

			prepareMaterial(i,newMat); //Ray: fix doubleSize bug

            // create a MatInstance for the new material
            MatInstance * matInst = new MatInstance( *newMat );
            mMatInstList[i] = matInst;
         }
      }
   }
}


//--------------------------------------------------------------------------
// Set material instance
//--------------------------------------------------------------------------
void MaterialList::setMaterialInst( MatInstance *matInst, U32 texIndex )
{
   if( texIndex >= mMatInstList.size() )
   {
      AssertFatal( false, "Index out of range" );
   }
   mMatInstList[texIndex] = matInst;
}














