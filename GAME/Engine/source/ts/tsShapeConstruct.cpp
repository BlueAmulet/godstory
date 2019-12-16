//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "ts/tsShapeConstruct.h"
#include "console/consoleTypes.h"
#include "core/fileStream.h"
#include "core/bitStream.h"

IMPLEMENT_CO_DATABLOCK_V1(TSShapeConstructor);

S32 gNumTransforms = 0;
S32 gRotTransforms = 0;
S32 gTransTransforms = 0;

TSShapeConstructor::TSShapeConstructor()
{
   mShape = NULL;
   for (S32 i = 0; i<MaxSequences; i++)
      mSequence[i] = NULL;
   mMidNode = StringTable->insert("");
}

TSShapeConstructor::~TSShapeConstructor()
{
}

bool TSShapeConstructor::onAdd()
{
   if(!Parent::onAdd())
      return false;
   return true;
}

bool TSShapeConstructor::preload(bool server, char errorBuffer[256])
{
   if(!Parent::preload(server, errorBuffer))
      return false;

   bool error = false;
   hShape = ResourceManager->load(mShape);
   if(!bool(hShape))
      return false;

   if (hShape->getSequencesConstructed())
      return true;

   // We want to write into the resource:
   TSShape * shape = const_cast<TSShape*>((const TSShape*)hShape);

   Stream * f;
   for (S32 i=0; i<MaxSequences; i++)
   {
      if (mSequence[i])
      {
         char fileBuf[256];
         dStrcpy(fileBuf, sizeof(fileBuf), mSequence[i]);

         // spaces and tabs indicate the end of the file name:
         char * terminate1 = dStrchr(fileBuf,' ');
         char * terminate2 = dStrchr(fileBuf,'\t');
         if (terminate1 && terminate2)
            // select the earliest one:
            *(terminate1<terminate2 ? terminate1 : terminate2) = '\0';
         else if (terminate1 || terminate2)
            // select the non-null one:
            *(terminate1 ? terminate1 : terminate2) = '\0';

         f = ResourceManager->openStream(fileBuf);
         if (!f || f->getStatus() != Stream::Ok)
         {
            dSprintf(errorBuffer, 256, "Missing sequence %s for %s",mSequence[i],mShape);
            error = true;
            continue;
         }
         if (!shape->importSequences(f) || f->getStatus()!=Stream::Ok)
         {
            ResourceManager->closeStream(f);
            dSprintf(errorBuffer, 256, "Load sequence %s failed for %s",mSequence[i],mShape);
            return false;
         }
         ResourceManager->closeStream(f);

         // if there was a space or tab in mSequence[i], then a new name was supplied for it
         // change it here...will still be in fileBuf
         if (terminate1 || terminate2)
         {
            // select the latter one:
            char * nameStart = terminate1<terminate2 ? terminate2 : terminate1;
            do
               nameStart++;
            while ( (*nameStart==' ' || *nameStart=='\t') && *nameStart != '\0' );
            // find the name in the shape (but keep the old one there in case used by something else)
            shape->sequences.last().nameIndex = shape->findName(nameStart);
            if (shape->sequences.last().nameIndex == -1)
            {
               shape->sequences.last().nameIndex = shape->names.size();
               shape->names.increment();
               shape->names.last() = StringTable->insert(nameStart,false);
            }
         }
      }
      else
         break;
   }

   if(!error)
      hShape->initNodesMatters(mMidNode);

   if(!error)
      hShape->setSequencesConstructed(true);
   return !error;
}

void TSShapeConstructor::packData(BitStream* stream)
{
   Parent::packData(stream);

   // pack the shape's path
   stream->writeString(mShape);
   stream->writeString(mMidNode);

   // copy everything before last '/' to shape_path
   char shape_path[255];
   U32 path_len = (dStrlen(mShape) - dStrlen(dStrrchr(mShape,'/'))) ;
   dStrncpy(shape_path, sizeof(shape_path), mShape, path_len);
   shape_path[path_len] = '\0';

   // count all the sequences
   S32 count = 0;
   for (S32 b=0; b<MaxSequences; b++)
      if (mSequence[b])
         count++;

   // pack the sequence count
   stream->writeInt(count,NumSequenceBits);

   // pack the sequences
   for (S32 i=0; i<MaxSequences; i++)
   {
      if (mSequence[i])
      {
         // Since it is pretty easy to have a path longer than 256 characters
         // we are making the path relative to the shape and reassembling it
         // in the unpackData.
         // TODO: Handle "relative" paths that are longer than 256 characters
         StringTableEntry seqRelPath = Platform::makeRelativePathName(mSequence[i], shape_path);

         stream->writeString(seqRelPath);
      }
   }
}

void TSShapeConstructor::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  // unpack the shape's path
  mShape = stream->readSTString();
  mMidNode = stream->readSTString();

  // copy everything before last '/' to shape_path
  char shape_path[255];
  U32 path_len = (dStrlen(mShape) - dStrlen(dStrrchr(mShape,'/'))) ;
  dStrncpy(shape_path, sizeof(shape_path), mShape, path_len);
  shape_path[path_len] = '\0';

  // unpack the sequences
  char seq_name[255];
  S32 i = 0, count = stream->readInt(NumSequenceBits);
  for (; i<count; i++)
  {
    stream->readString(seq_name,255);

    char fullSequence[1024];
    Platform::makeFullPathName(seq_name, fullSequence, sizeof(fullSequence), shape_path);

    mSequence[i] = StringTable->insert(fullSequence);
  }

  while (i<MaxSequences)
     mSequence[i++] = NULL;
}

void TSShapeConstructor::initPersistFields()
{
   Parent::initPersistFields();
   addGroup("Media");	
   addField("baseShape", TypeFilename, Offset(mShape, TSShapeConstructor));
   endGroup("Media");	

   char buf[30];
   if (MaxSequences) addGroup("Sequences");	
   for (S32 i=0; i<MaxSequences; i++)
   {
      dSprintf(buf,sizeof(buf),"sequence%i",i);
      addField(buf, TypeFilename, Offset(mSequence[i], TSShapeConstructor));
   }
   if (MaxSequences) endGroup("Sequences");	

   addField("MidNode", TypeString, Offset(mMidNode, TSShapeConstructor));
}


