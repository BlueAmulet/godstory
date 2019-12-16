//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/simObject.h"
#include "console/simFieldDictionary.h"
#include "console/typeValidators.h"

#include "core/frameAllocator.h"
#include "core/fileStream.h"
#include "core/fileObject.h"

SimObject::SimObject( const U8 namespaceLinkMask ) : mNSLinkMask( namespaceLinkMask )
{
   objectName            = NULL;
   mInternalName          = NULL;
   nextNameObject        = (SimObject*)-1;
   nextManagerNameObject = (SimObject*)-1;
   nextIdObject          = NULL;

   mId           = 0;
   mGroup        = 0;
   mNameSpace    = NULL;
   mNotifyList   = NULL;
   mFlags.set( ModStaticFields | ModDynamicFields );
   mTypeMask             = 0;

   mFieldDictionary = NULL;
   mCanSaveFieldDictionary	=	true;

   mClassName = NULL;
   mSuperClassName = NULL;
}

void SimObject::assignDynamicFieldsFrom(SimObject* parent)
{
   if(parent->mFieldDictionary)
   {
      if( mFieldDictionary == NULL )
         mFieldDictionary = new SimFieldDictionary;
      mFieldDictionary->assignFrom(parent->mFieldDictionary);
   }
}

void SimObject::assignFieldsFrom(SimObject *parent)
{
   // only allow field assigns from objects of the same class:
   if(getClassRep() == parent->getClassRep())
   {
      const AbstractClassRep::FieldList &list = getFieldList();

      // copy out all the fields:
      for(U32 i = 0; i < list.size(); i++)
      {
         const AbstractClassRep::Field* f = &list[i];
         S32 lastField = f->elementCount - 1;
         for(S32 j = 0; j <= lastField; j++)
         {
            const char* fieldVal = (*f->getDataFn)( this,  Con::getData(f->type, (void *) (((const char *)parent) + f->offset), j, f->table, f->flag));
            //if(fieldVal)
            //   Con::setData(f->type, (void *) (((const char *)this) + f->offset), j, 1, &fieldVal, f->table);
            if(fieldVal)
            {
               // code copied from SimObject::setDataField().
               // TODO: paxorr: abstract this into a better setData / getData that considers prot fields.
               FrameTemp<char> buffer(2048);
               FrameTemp<char> bufferSecure(2048); // This buffer is used to make a copy of the data
               ConsoleBaseType *cbt = ConsoleBaseType::getType( f->type );
               const char* szBuffer = cbt->prepData( fieldVal, buffer, 2048 );
               dMemset( bufferSecure, 0, 2048 );
               dMemcpy( bufferSecure, szBuffer, dStrlen( szBuffer ) );

               if((*f->setDataFn)( this, bufferSecure ) )
                  Con::setData(f->type, (void *) (((const char *)this) + f->offset), j, 1, &fieldVal, f->table);
            }
         }
      }
   }

   assignDynamicFieldsFrom(parent);
}

bool SimObject::writeField(StringTableEntry fieldname, const char* value)
{
   // Don't write empty fields.
   if (!value || !*value)
      return false;

   // Don't write owner field for components
   if( fieldname == StringTable->insert("owner") )
      return false;

   // Don't write ParentGroup
   if( fieldname == StringTable->insert("parentGroup") )
      return false;


   return true;
}

void SimObject::writeFields(Stream &stream, U32 tabStop)
{
   const AbstractClassRep::FieldList &list = getFieldList();

   for(U32 i = 0; i < list.size(); i++)
   {
      const AbstractClassRep::Field* f = &list[i];

      if( f->type == AbstractClassRep::DeprecatedFieldType ||
         f->type == AbstractClassRep::StartGroupFieldType ||
         f->type == AbstractClassRep::EndGroupFieldType) continue;

      for(U32 j = 0; S32(j) < f->elementCount; j++)
      {
         char array[8];
         dSprintf( array, 8, "%d", j );
         const char *val = getDataField(StringTable->insert( f->pFieldname ), array );

         // Make a copy for the field check.
         if (!val)
            continue;

         U32 nBufferSize = dStrlen( val ) + 1;
         FrameTemp<char> valCopy( nBufferSize );
         dStrcpy( (char *)valCopy, nBufferSize, val );

         if (!writeField(f->pFieldname, valCopy))
            continue;

         val = valCopy;

         U32 expandedBufferSize = ( nBufferSize  * 2 ) + 64;
		 FrameTemp<char> expandedBuffer( expandedBufferSize );

         if(f->elementCount == 1)
            dSprintf(expandedBuffer, expandedBufferSize, "%s = \"", f->pFieldname);
         else
            dSprintf(expandedBuffer, expandedBufferSize, "%s[%d] = \"", f->pFieldname, j);

         // detect and collapse relative path information
         char fnBuf[1024];
         if (f->type == TypeFilename)
         {
            Con::collapseScriptFilename(fnBuf, 1024, val);
            val = fnBuf;
         }

         expandEscape((char*)expandedBuffer + dStrlen(expandedBuffer), val);
         dStrcat(expandedBuffer, expandedBufferSize, "\";\r\n");

         stream.writeTabs(tabStop);
         stream.write(dStrlen(expandedBuffer),expandedBuffer);
      }
   }
   if(mFieldDictionary && mCanSaveFieldDictionary)
      mFieldDictionary->writeFields(this, stream, tabStop);
}

void SimObject::writeGhostAlways(Stream &stream, U32 tabStop, U32 flags)
{
	if(isGhostAlways())
		SimObject::write(stream, tabStop, flags);
}

void SimObject::write(Stream &stream, U32 tabStop, U32 flags)
{
   // Only output selected objects if they want that.
   if((flags & SelectedOnly) && !isSelected())
   {
      if( hasComponents() )
      {
         VectorPtr<SimComponent *> &components = getComponentList();
         for(U32 i = 0; i < components.size(); i++)
         {
            SimObject *pObj = dynamic_cast<SimObject*>( components[i] );
            if( pObj )
               pObj->write(stream, tabStop, flags);
         }

      }
      return;
   }

   stream.writeTabs(tabStop);
   char buffer[1024];
   dSprintf(buffer, sizeof(buffer), "new %s(%s) {\r\n", getClassName(), getName() ? getName() : "");
   stream.write(dStrlen(buffer), buffer);
   writeFields(stream, tabStop + 1);

   // Components?
   if(hasComponents())
   {
      VectorPtr<SimComponent *> &components = getComponentList();

      stream.write(2, "\r\n");
      stream.writeTabs(tabStop+1);
      for(U32 i = 0; i < components.size(); i++)
      {
         SimObject *pObj = dynamic_cast<SimObject*>( components[i] );
         if( pObj )
            pObj->write(stream, tabStop + 1, flags);
      }
   }

   stream.writeTabs(tabStop);
   stream.write(4, "};\r\n");
}

bool SimObject::save(const char* pcFileName, bool bOnlySelected)
{
   static const char *beginMessage = "//--- SERVER OBJECT WRITE BEGIN ---";
   static const char *endMessage = "//--- SERVER OBJECT WRITE END ---";
   FileStream stream;
   FileObject f;
   f.readMemory(pcFileName);

   // check for flags <selected, ...>
   U32 writeFlags = 0;
   if(bOnlySelected)
      writeFlags |= SimObject::SelectedOnly;

   if(!ResourceManager->openFileForWrite(stream, pcFileName))
      return false;

   char docRoot[256];
   char modRoot[256];

   dStrcpy(docRoot, sizeof(docRoot), pcFileName);
   char *p = dStrrchr(docRoot, '/');
   if (p) *++p = '\0';
   else  docRoot[0] = '\0';

   dStrcpy(modRoot, sizeof(modRoot), pcFileName);
   p = dStrchr(modRoot, '/');
   if (p) *++p = '\0';
   else  modRoot[0] = '\0';

   Con::setVariable("$DocRoot", docRoot);
   Con::setVariable("$ModRoot", modRoot);

   const char *buffer;
   while(!f.isEOF())
   {
      buffer = (const char *) f.readLine();
      if(!dStrcmp(buffer, beginMessage))
         break;
      stream.write(dStrlen(buffer), buffer);
      stream.write(2, "\r\n");
   }
   stream.write(dStrlen(beginMessage), beginMessage);
   stream.write(2, "\r\n");
   write(stream, 0, writeFlags);
   stream.write(dStrlen(endMessage), endMessage);
   stream.write(2, "\r\n");
   while(!f.isEOF())
   {
      buffer = (const char *) f.readLine();
      if(!dStrcmp(buffer, endMessage))
         break;
   }
   while(!f.isEOF())
   {
      buffer = (const char *) f.readLine();
      stream.write(dStrlen(buffer), buffer);
      stream.write(2, "\r\n");
   }

   Con::setVariable("$DocRoot", NULL);
   Con::setVariable("$ModRoot", NULL);

   return true;

}

bool SimObject::GhostAlwaysSave(const char* pcFileName, bool bOnlySelected)
{
	static const char *beginMessage = "//--- CLIENT OBJECT WRITE BEGIN ---";
	static const char *endMessage = "//--- CLIENT OBJECT WRITE END ---";

	char Filename[1024];
	int len = (int)dStrlen(pcFileName);
	if(!_stricmp(&pcFileName[len-4],".mis"))
	{
		memcpy(Filename,pcFileName,len-4);
		memcpy(&Filename[len-4],".cs",3);
		Filename[len-1] = 0;
	}
	else
	{
		memcpy(Filename,pcFileName,len);
		memcpy(&Filename[len-1],".cs",3);
		Filename[len+1] = 0;
	}
	pcFileName = Filename;

	FileStream stream;
	FileObject f;
	f.readMemory(pcFileName);

	// check for flags <selected, ...>
	U32 writeFlags = 0;
	if(bOnlySelected)
		writeFlags |= SimObject::SelectedOnly;

	if(!ResourceManager->openFileForWrite(stream, pcFileName))
		return false;

	char docRoot[256];
	char modRoot[256];

	dStrcpy(docRoot, sizeof(docRoot), pcFileName);
	char *p = dStrrchr(docRoot, '/');
	if (p) *++p = '\0';
	else  docRoot[0] = '\0';

	dStrcpy(modRoot, sizeof(modRoot), pcFileName);
	p = dStrchr(modRoot, '/');
	if (p) *++p = '\0';
	else  modRoot[0] = '\0';

	Con::setVariable("$DocRoot", docRoot);
	Con::setVariable("$ModRoot", modRoot);

	const char *buffer;
	while(!f.isEOF())
	{
		buffer = (const char *) f.readLine();
		if(!dStrcmp(buffer, beginMessage))
			break;
		stream.write(dStrlen(buffer), buffer);
		stream.write(2, "\r\n");
	}
	stream.write(dStrlen(beginMessage), beginMessage);
	stream.write(2, "\r\n");
	writeGhostAlways(stream, 0, writeFlags);
	stream.write(dStrlen(endMessage), endMessage);
	stream.write(2, "\r\n");
	while(!f.isEOF())
	{
		buffer = (const char *) f.readLine();
		if(!dStrcmp(buffer, endMessage))
			break;
	}
	while(!f.isEOF())
	{
		buffer = (const char *) f.readLine();
		stream.write(dStrlen(buffer), buffer);
		stream.write(2, "\r\n");
	}

	Con::setVariable("$DocRoot", NULL);
	Con::setVariable("$ModRoot", NULL);

	return true;

}

void SimObject::setInternalName(const char* newname)
{
   if(newname)
      mInternalName = StringTable->insert(newname);
}

StringTableEntry SimObject::getInternalName()
{
   return mInternalName;
}

static S32 QSORT_CALLBACK compareFields(const void* a,const void* b)
{
   const AbstractClassRep::Field* fa = *((const AbstractClassRep::Field**)a);
   const AbstractClassRep::Field* fb = *((const AbstractClassRep::Field**)b);

   return dStricmp(fa->pFieldname, fb->pFieldname);
}

bool SimObject::isMethod( const char* methodName )
{
   if( !methodName || !methodName[0] )
      return false;

   StringTableEntry stname = StringTable->insert( methodName );

   if( getNamespace() )
      return ( getNamespace()->lookup( stname ) != NULL );

   return false;
}

const char *SimObject::tabComplete(const char *prevText, S32 baseLen, bool fForward)
{
   return mNameSpace->tabComplete(prevText, baseLen, fForward);
}

void SimObject::setDataField(StringTableEntry slotName, const char *array, const char *value)
{
   // first search the static fields if enabled
   if(mFlags.test(ModStaticFields))
   {
      const AbstractClassRep::Field *fld = findField(slotName);
      if(fld)
      {
         if( fld->type == AbstractClassRep::DeprecatedFieldType ||
            fld->type == AbstractClassRep::StartGroupFieldType ||
            fld->type == AbstractClassRep::EndGroupFieldType)
            return;

         S32 array1 = array ? dAtoi(array) : 0;

         if(array1 >= 0 && array1 < fld->elementCount && fld->elementCount >= 1)
         {
            // If the set data notify callback returns true, then go ahead and
            // set the data, otherwise, assume the set notify callback has either
            // already set the data, or has deemed that the data should not
            // be set at all.
            FrameTemp<char> buffer(2048);
            FrameTemp<char> bufferSecure(2048); // This buffer is used to make a copy of the data
            // so that if the prep functions or any other functions use the string stack, the data
            // is not corrupted.

            ConsoleBaseType *cbt = ConsoleBaseType::getType( fld->type );
            AssertFatal( cbt != NULL, "Could not resolve Type Id." );

            const char* szBuffer = cbt->prepData( value, buffer, 2048 );
            dMemset( bufferSecure, 0, 2048 );
            dMemcpy( bufferSecure, szBuffer, dStrlen( szBuffer ) );

            if( (*fld->setDataFn)( this, bufferSecure ) )
               Con::setData(fld->type, (void *) (((const char *)this) + fld->offset), array1, 1, &value, fld->table);

            if(fld->validator)
               fld->validator->validateType(this, (void *) (((const char *)this) + fld->offset));

            onStaticModified( slotName, value );

            return;
         }

         if(fld->validator)
            fld->validator->validateType(this, (void *) (((const char *)this) + fld->offset));

         onStaticModified( slotName, value );
         return;
      }
   }

   if(mFlags.test(ModDynamicFields))
   {
      if(!mFieldDictionary)
         mFieldDictionary = new SimFieldDictionary;

      if(!array)
         mFieldDictionary->setFieldValue(slotName, value);
      else
      {
         char buf[256];
         dStrcpy(buf, sizeof(buf), slotName);
         dStrcat(buf, sizeof(buf), array);
         mFieldDictionary->setFieldValue(StringTable->insert(buf), value);
      }
   }
}


void  SimObject::dumpClassHierarchy()
{
   AbstractClassRep* pRep = getClassRep();
   while(pRep)
   {
      Con::warnf("%s ->", pRep->getClassName());
      pRep	=	pRep->getParentClass();
   }
}

const char *SimObject::getDataField(StringTableEntry slotName, const char *array)
{
   if(mFlags.test(ModStaticFields))
   {
      S32 array1 = array ? dAtoi(array) : -1;
      const AbstractClassRep::Field *fld = findField(slotName);

      if(fld)
      {
         if(array1 == -1 && fld->elementCount == 1)
            return (*fld->getDataFn)( this, Con::getData(fld->type, (void *) (((const char *)this) + fld->offset), 0, fld->table, fld->flag) );
         if(array1 >= 0 && array1 < fld->elementCount)
            return (*fld->getDataFn)( this, Con::getData(fld->type, (void *) (((const char *)this) + fld->offset), array1, fld->table, fld->flag) );// + typeSizes[fld.type] * array1));
         return "";
      }
   }

   if(mFlags.test(ModDynamicFields))
   {
      if(!mFieldDictionary)
         return "";

      if(!array)
      {
         if (const char* val = mFieldDictionary->getFieldValue(slotName))
            return val;
      }
      else
      {
         static char buf[256];
         dStrcpy(buf, sizeof(buf), slotName);
         dStrcat(buf, sizeof(buf), array);
         if (const char* val = mFieldDictionary->getFieldValue(StringTable->insert(buf)))
            return val;
      }
   }

   return "";
}

U32 SimObject::getDataFieldType( StringTableEntry slotName, const char* array )
{
   const AbstractClassRep::Field* field = findField( slotName );
   if(field)
      return field->type;

   // Check dynamic fields
   if(!mFieldDictionary)
      return 0;

   if(array == NULL || *array == 0)
      return mFieldDictionary->getFieldType( slotName );
   else
   {
      static char buf[256];
      dStrcpy( buf, sizeof(buf), slotName );
      dStrcat( buf, sizeof(buf), array );

      return mFieldDictionary->getFieldType( StringTable->insert( buf ) );
   }
}

void SimObject::setDataFieldType(const U32 fieldTypeId, StringTableEntry slotName, const char *array)
{
   // This only works on dynamic fields, bail if we have no field dictionary
   if(!mFieldDictionary)
      return;

   if(array == NULL || *array == 0)
      mFieldDictionary->setFieldType( slotName, fieldTypeId );
   else
   {
      static char buf[256];
      dStrcpy( buf, sizeof(buf), slotName );
      dStrcat( buf, sizeof(buf), array );

      mFieldDictionary->setFieldType( StringTable->insert( buf ), fieldTypeId );
   }
}

void SimObject::setDataFieldType(const char *typeName, StringTableEntry slotName, const char *array)
{
   // This only works on dynamic fields, bail if we have no field dictionary
   if(!mFieldDictionary)
      return;

   if(array == NULL || *array == 0)
      mFieldDictionary->setFieldType( slotName, typeName );
   else
   {
      static char buf[256];
      dStrcpy( buf, sizeof(buf), slotName );
      dStrcat( buf, sizeof(buf), array );

      mFieldDictionary->setFieldType( StringTable->insert( buf ), typeName );
   }
}

SimObject::~SimObject()
{
   delete mFieldDictionary;

   AssertFatal(nextNameObject == (SimObject*)-1,avar(
      "SimObject::~SimObject:  Not removed from dictionary: name %s, id %i",
      objectName, mId));
   AssertFatal(nextManagerNameObject == (SimObject*)-1,avar(
      "SimObject::~SimObject:  Not removed from manager dictionary: name %s, id %i",
      objectName,mId));
   AssertFatal(mFlags.test(Added) == 0, "SimObject::object "
      "missing call to SimObject::onRemove");
}

//---------------------------------------------------------------------------

bool SimObject::isLocked()
{
   if(!mFieldDictionary)
      return false;

   const char * val = mFieldDictionary->getFieldValue( StringTable->insert( "locked", false ) );

   return( val ? dAtob(val) : false );
}

void SimObject::setLocked( bool b = true )
{
   setDataField(StringTable->insert("locked", false), NULL, b ? "true" : "false" );
}

bool SimObject::isHidden()
{
   if(!mFieldDictionary)
      return false;

   const char * val = mFieldDictionary->getFieldValue( StringTable->insert( "hidden", false ) );
   return( val ? dAtob(val) : false );
}

void SimObject::setHidden(bool b = true)
{
   setDataField(StringTable->insert("hidden", false), NULL, b ? "true" : "false" );
}

const char* SimObject::getIdString() const
{
   static char IDbuffer[64];
   dSprintf(IDbuffer, sizeof(IDbuffer), "%d", mId);
   return IDbuffer;
}

//---------------------------------------------------------------------------

bool SimObject::onAdd()
{
   mFlags.set(Added);

   if(getClassRep())
      mNameSpace = getClassRep()->getNameSpace();

   linkNamespaces();

   // If we contain components, register them
   if( hasComponents() )
   {
      // Register Interfaces
      _registerInterfaces( this );

      // Any Components if we have them
      if( !_registerComponents( this ) )
         return false;

      if( !_postRegisterComponents() )
         return false;
   }

   // onAdd() should return FALSE if there was an error
   return true;
}

void SimObject::onRemove()
{
   if( hasComponents() )
   {
      _unregisterComponents();

      // Delete all components
      VectorPtr<SimComponent *>&componentList = getComponentList();
      while(componentList.size() > 0)
      {
         SimComponent *c = componentList[0];
         componentList.erase( componentList.begin() );

         SimObject *obj = dynamic_cast<SimObject*>(c);
         if( obj && obj->isProperlyAdded() )
            obj->deleteObject();
         else if( obj && !obj->isRemoved() && !obj->isDeleted() )
            delete obj;
         // else, something else is deleting this, don't mess with it
      }
   }

   mFlags.clear(Added);

   unlinkNamespaces();
}

void SimObject::onGroupAdd()
{
}

void SimObject::onGroupRemove()
{
}

void SimObject::onDeleteNotify(SimObject*)
{
}

void SimObject::onNameChange(const char*)
{
}

void SimObject::onStaticModified(const char* slotName, const char* newValue)
{
}

//-----------------------------------------------------------------------------
// Component Initialization
//-----------------------------------------------------------------------------
bool SimObject::processArguments(S32 argc, const char**argv)
{
   for(S32 i = 0; i < argc; i++)
   {
      SimComponent *obj = dynamic_cast<SimComponent*> (Sim::findObject(argv[i]) );
      if(obj)
         addComponent(obj);
      else
         Con::printf("SimComponent::processArguments - Invalid Component Object \"%s\"", argv[i]);
   }

   return true;
}

bool SimObject::addComponentFromField( void* obj, const char* data )
{
   SimObject *pObj = static_cast<SimObject*>(obj);

   // Can only add components before they're registered with the Sim
   if( !pObj->isProperlyAdded() )
   {
      // Try to find this component, if it doesn't exist, create it
      SimComponent *pComponent = dynamic_cast<SimComponent*>( Sim::findObject( data ) );
      if( pComponent == NULL )
         pComponent = dynamic_cast<SimComponent*>( create( data ) );

      if( pComponent )
      {
         pObj->addComponent( pComponent ); 
         static_cast<SimObject*>(pComponent)->registerObject();
      }

   }
   return false;
}

bool SimObject::onComponentAdd(SimComponent *target)
{
   SimObjectId objId = static_cast<SimObject*>(target)->getId();
   if( static_cast<SimObject*>(target)->isMethod( "onComponentAdd" ) )
      Con::executef(this, "onComponentAdd", Con::getIntArg(objId));
   return true;
}

void SimObject::onComponentRemove(SimComponent *target)
{
   SimObjectId objId = static_cast<SimObject*>(target)->getId();
   if( static_cast<SimObject*>(target)->isMethod( "onComponentRemove" ) )
      Con::executef(this, "onComponentRemove", Con::getIntArg(objId));

}

bool SimObject::isChildOfGroup(SimGroup* pGroup)
{
   if(!pGroup)
      return false;

   //if we *are* the group in question,
   //return true:
   if(pGroup == dynamic_cast<SimGroup*>(this))
      return true;

   SimGroup* temp	=	mGroup;
   while(temp)
   {
      if(temp == pGroup)
         return true;
      temp = temp->mGroup;
   }

   return false;
}

//---------------------------------------------------------------------------

static Chunker<SimObject::Notify> notifyChunker(128000);
SimObject::Notify *SimObject::mNotifyFreeList = NULL;

SimObject::Notify *SimObject::allocNotify()
{
   if(mNotifyFreeList)
   {
      SimObject::Notify *ret = mNotifyFreeList;
      mNotifyFreeList = ret->next;
      return ret;
   }
   return notifyChunker.alloc();
}

void SimObject::freeNotify(SimObject::Notify* note)
{
   AssertFatal(note->type != SimObject::Notify::Invalid, "Invalid notify");
   note->type = SimObject::Notify::Invalid;
   note->next = mNotifyFreeList;
   mNotifyFreeList = note;
}

//------------------------------------------------------------------------------

SimObject::Notify* SimObject::removeNotify(void *ptr, SimObject::Notify::Type type)
{
   Notify **list = &mNotifyList;
   while(*list)
   {
      if((*list)->ptr == ptr && (*list)->type == type)
      {
         SimObject::Notify *ret = *list;
         *list = ret->next;
         return ret;
      }
      list = &((*list)->next);
   }
   return NULL;
}

void SimObject::deleteNotify(SimObject* obj)
{
   AssertFatal(!obj->isDeleted(),
      "SimManager::deleteNotify: Object is being deleted");
   Notify *note = allocNotify();
   note->ptr = (void *) this;
   note->next = obj->mNotifyList;
   note->type = Notify::DeleteNotify;
   obj->mNotifyList = note;

   note = allocNotify();
   note->ptr = (void *) obj;
   note->next = mNotifyList;
   note->type = Notify::ClearNotify;
   mNotifyList = note;

   //obj->deleteNotifyList.pushBack(this);
   //clearNotifyList.pushBack(obj);
}

void SimObject::registerReference(SimObject **ptr)
{
   Notify *note = allocNotify();
   note->ptr = (void *) ptr;
   note->next = mNotifyList;
   note->type = Notify::ObjectRef;
   mNotifyList = note;
}

void SimObject::unregisterReference(SimObject **ptr)
{
   Notify *note = removeNotify((void *) ptr, Notify::ObjectRef);
   if(note)
   {
      freeNotify(note);

      if( mFlags.test( AutoDelete ) )
      {
         for( Notify* n = mNotifyList; n != NULL; n = n->next )
            if( n->type == Notify::ObjectRef )
               return;

         deleteObject();
      }
   }
}

void SimObject::clearNotify(SimObject* obj)
{
   Notify *note = obj->removeNotify((void *) this, Notify::DeleteNotify);
   if(note)
      freeNotify(note);

   note = removeNotify((void *) obj, Notify::ClearNotify);
   if(note)
      freeNotify(note);
}

void SimObject::processDeleteNotifies()
{
   // clear out any delete notifies and
   // object refs.

   while(mNotifyList)
   {
      Notify *note = mNotifyList;
      mNotifyList = note->next;

      AssertFatal(note->type != Notify::ClearNotify, "Clear notes should be all gone.");

      if(note->type == Notify::DeleteNotify)
      {
         SimObject *obj = (SimObject *) note->ptr;
         Notify *cnote = obj->removeNotify((void *)this, Notify::ClearNotify);
         obj->onDeleteNotify(this);
         freeNotify(cnote);
      }
      else
      {
         // it must be an object ref - a pointer refs this object
         *((SimObject **) note->ptr) = NULL;
      }
      freeNotify(note);
   }
}

void SimObject::clearAllNotifications()
{
   for(Notify **cnote = &mNotifyList; *cnote; )
   {
      Notify *temp = *cnote;
      if(temp->type == Notify::ClearNotify)
      {
         *cnote = temp->next;
         Notify *note = ((SimObject *) temp->ptr)->removeNotify((void *) this, Notify::DeleteNotify);
         freeNotify(temp);
         freeNotify(note);
      }
      else
         cnote = &(temp->next);
   }
}

//---------------------------------------------------------------------------

void SimObject::initPersistFields()
{
   Parent::initPersistFields();

   //add the canSaveDynamicFields property:
   addGroup("SimBase");
   addField("canSaveDynamicFields", TypeBool, Offset(mCanSaveFieldDictionary, SimObject));
   addField("internalName", TypeString, Offset(mInternalName, SimObject));
   addProtectedField("parentGroup", TypeSimObjectPtr, Offset(mGroup, SimObject), &setProtectedParent, &defaultProtectedGetFn, "Group hierarchy parent of the object." );
   endGroup("SimBase");

   // Namespace Linking.
   addGroup("Namespace Linking");
   addProtectedField("superClass", TypeString, Offset(mSuperClassName, SimObject), &setSuperClass, &defaultProtectedGetFn,
      "Script super-class of object.");
   addProtectedField("class", TypeString, Offset(mClassName, SimObject), &setClass, &defaultProtectedGetFn,
      "Script class of object.");
   // For legacy support
   addProtectedField("className", TypeString, Offset(mClassName, SimObject), &setClass, &defaultProtectedGetFn,
      "Script class of object.");
   endGroup("Namespace Linking");

   // Component Enabled
   addGroup("Component");
   addProtectedField( "Enabled", TypeBool, Offset(mEnabled, SimObject), &setEnabled, &defaultProtectedGetFn, "" );
   addProtectedField( "Component", TypeString, Offset(mInternalName, SimObject), &addComponentFromField, &defaultProtectedGetFn, "" );
   endGroup("Component");

}

bool SimObject::setProtectedParent(void* obj, const char* data)
{
   SimGroup *parent = NULL;
   SimObject *object = static_cast<SimObject*>(obj);

   if(Sim::findObject(data, parent))
      parent->addObject(object);

   // always return false, because we've set mGroup when we called addObject
   return false;
}


bool SimObject::addToSet(SimObjectId spid)
{
   if (mFlags.test(Added) == false)
      return false;

   SimObject* ptr = Sim::findObject(spid);
   if (ptr)
   {
      SimSet* sp = dynamic_cast<SimSet*>(ptr);
      AssertFatal(sp != 0,
         "SimObject::addToSet: "
         "ObjectId does not refer to a set object");
      sp->addObject(this);
      return true;
   }
   return false;
}

bool SimObject::addToSet(const char *ObjectName)
{
   if (mFlags.test(Added) == false)
      return false;

   SimObject* ptr = Sim::findObject(ObjectName);
   if (ptr)
   {
      SimSet* sp = dynamic_cast<SimSet*>(ptr);
      AssertFatal(sp != 0,
         "SimObject::addToSet: "
         "ObjectName does not refer to a set object");
      sp->addObject(this);
      return true;
   }
   return false;
}

bool SimObject::removeFromSet(SimObjectId sid)
{
   if (mFlags.test(Added) == false)
      return false;

   SimSet *set;
   if(Sim::findObject(sid, set))
   {
      set->removeObject(this);
      return true;
   }
   return false;
}

bool SimObject::removeFromSet(const char *objectName)
{
   if (mFlags.test(Added) == false)
      return false;

   SimSet *set;
   if(Sim::findObject(objectName, set))
   {
      set->removeObject(this);
      return true;
   }
   return false;
}

void SimObject::inspectPreApply()
{
}

void SimObject::inspectPostApply()
{
}

void SimObject::linkNamespaces()
{
   if( mNameSpace )
      unlinkNamespaces();

   StringTableEntry parent = StringTable->insert( getClassName() );
   if( ( mNSLinkMask & LinkSuperClassName ) && mSuperClassName && mSuperClassName[0] )
   {
      if( Con::linkNamespaces( parent, mSuperClassName ) )
         parent = mSuperClassName;
      else
         mSuperClassName = StringTable->insert( "" ); // CodeReview Is this behavior that we want?
      // CodeReview This will result in the mSuperClassName variable getting hosed
      // CodeReview if Con::linkNamespaces returns false. Looking at the code for
      // CodeReview Con::linkNamespaces, and the call it makes to classLinkTo, it seems
      // CodeReview like this would only fail if it had bogus data to begin with, but
      // CodeReview I wanted to note this behavior which occurs in some implementations
      // CodeReview but not all. -patw
   }

   // ClassName -> SuperClassName
   if ( ( mNSLinkMask & LinkClassName ) && mClassName && mClassName[0] )
   {
      if( Con::linkNamespaces( parent, mClassName ) )
         parent = mClassName;
      else
         mClassName = StringTable->insert( "" ); // CodeReview (See previous note on this code)
   }

   // ObjectName -> ClassName
   StringTableEntry objectName = getName();
   if( objectName && objectName[0] )
   {
      if( Con::linkNamespaces( parent, objectName ) )
         parent = objectName;
   }

   // Store our namespace.
   mNameSpace = Con::lookupNamespace( parent );
}

void SimObject::unlinkNamespaces()
{
   if (!mNameSpace)
      return;

   // Restore NameSpaces
   StringTableEntry child = getName();
   if( child && child[0] )
   {
      if( ( mNSLinkMask & LinkClassName ) && mClassName && mClassName[0])
      {
         if( Con::unlinkNamespaces( mClassName, child ) )
            child = mClassName;
      }

      if( ( mNSLinkMask & LinkSuperClassName ) && mSuperClassName && mSuperClassName[0] )
      {
         if( Con::unlinkNamespaces( mSuperClassName, child ) )
            child = mSuperClassName;
      }

      Con::unlinkNamespaces( getClassName(), child );
   }
   else
   {
      child = mClassName;
      if( child && child[0] )
      {
         if( ( mNSLinkMask & LinkSuperClassName ) && mSuperClassName && mSuperClassName[0] )
         {
            if( Con::unlinkNamespaces( mSuperClassName, child ) )
               child = mSuperClassName;
         }

         if( mNSLinkMask & LinkClassName )
            Con::unlinkNamespaces( getClassName(), child );
      }
      else
      {
         if( ( mNSLinkMask & LinkSuperClassName ) && mSuperClassName && mSuperClassName[0] )
            Con::unlinkNamespaces( getClassName(), mSuperClassName );
      }
   }

   mNameSpace = NULL;
}

void SimObject::setClassNamespace( const char *classNamespace )
{
   mClassName = StringTable->insert( classNamespace );
}

void SimObject::setSuperClassNamespace( const char *superClassNamespace )
{
   mSuperClassName = StringTable->insert( superClassNamespace );
}

IMPLEMENT_CONOBJECT(SimObject);

void SimObject::OnSerialize( CTinyStream& stream )
{
	stream << mCanSaveFieldDictionary;
	stream << mEnabled;
	stream.writeString( objectName );
}

void SimObject::OnUnserialize( CTinyStream& stream )
{
	stream >> mCanSaveFieldDictionary;
	stream >> mEnabled;

	char buf[1024];
	stream.readString( buf, 1024 );
	assignName( buf );
}
//-----------------------------------------------------------------------------
// Console Methods
//-----------------------------------------------------------------------------

DECLARE_SERIALIBLE( SimObject );

ConsoleMethod(SimObject, save, bool, 3, 4, "obj.save(fileName, <selectedOnly>)")
{
   bool bSelectedOnly	=	false;
   if(argc > 3)
      bSelectedOnly	= dAtob(argv[3]);

   return object->save(argv[2], bSelectedOnly);

}

//Ray: GhostAlway物件单独保存一份放客户端本地载
ConsoleMethod(SimObject, GhostAlwaysSave, bool, 3, 4, "obj.GhostAlwaysSave(fileName, <selectedOnly>)")
{
	bool bSelectedOnly	=	false;
	if(argc > 3)
		bSelectedOnly	= dAtob(argv[3]);

	const char* pcFileName = argv[2];
	char Filename[1024];
	int len = (int)dStrlen(pcFileName);
	if(!_stricmp(&pcFileName[len-4],".mis"))
	{
		memcpy(Filename,pcFileName,len-4);
		memcpy(&Filename[len-4],".bin",4);
		Filename[len] = 0;
	}
	else
	{
		memcpy(Filename,pcFileName,len);
		memcpy(&Filename[len-1],".bin",4);
		Filename[len+2] = 0;
	}

	std::string fileName = Filename;
	CTinySerializer serializer;
	CTinyStream stream( fileName.c_str(), CTinyStream::Write );
	std::string objectName = "class ";
	objectName += object->getClassName();

	tagTinySerializerFileHeader header;
	header.version = tagTinySerializerFileHeader::CurrentVersion;
	stream.writeByte( (char*)&header, sizeof( tagTinySerializerFileHeader ) );
	serializer.serializer( object, objectName, stream );

	return true;//object->GhostAlwaysSave(argv[2], bSelectedOnly);

}

ConsoleMethod(SimObject, setName, void, 3, 3, "obj.setName(newName)")
{
   argc;
   object->assignName(argv[2]);
}

ConsoleMethod(SimObject, getName, const char *, 2, 2, "obj.getName()")
{
   argc; argv;
   const char *ret = object->getName();
   return ret ? ret : "";
}

ConsoleMethod(SimObject, getClassName, const char *, 2, 2, "obj.getClassName()")
{
   argc; argv;
   const char *ret = object->getClassName();
   return ret ? ret : "";
}

ConsoleMethod(SimObject, getFieldValue, const char *, 3, 3, "obj.getFieldValue(fieldName);")
{
   argc; argv;

   const char *fieldName = StringTable->insert( argv[2] );
   return object->getDataField( fieldName, NULL );
}


ConsoleMethod(SimObject, setFieldValue, bool, 4, 4, "obj.setFieldValue(fieldName,value);")
{
   argc; argv;
   const char *fieldName = StringTable->insert(argv[2]);
   const char *value = argv[3];

   object->setDataField( fieldName, NULL, value );

   return true;

}

ConsoleMethod(SimObject, getFieldType, const char *, 3, 3, "obj.getFieldType(fieldName);")
{
   argc; argv;

   const char *fieldName = StringTable->insert( argv[2] );
   U32 typeID = object->getDataFieldType( fieldName, NULL );
   ConsoleBaseType* type = ConsoleBaseType::getType( typeID );

   if( type )
      return type->getTypeName();

   return "";
}

ConsoleMethod(SimObject, setFieldType, void, 4, 4, "obj.setFieldType(fieldName, typeString);")
{
   const char *fieldName = StringTable->insert( argv[2] );
   object->setDataFieldType( argv[3], fieldName, NULL );
}

ConsoleMethod( SimObject, call, const char*, 2, 0, "( %args ) - Dynamically call a method on an object." )
{
   argv[1] = argv[2];
   return Con::execute( object, argc - 1, argv + 1 );
}

//-----------------------------------------------------------------------------
//	Set the internal name, can be used to find child objects
//	in a meaningful way, usually from script, while keeping
//	common script functionality together using the controls "Name" field.
//-----------------------------------------------------------------------------
ConsoleMethod( SimObject, setInternalName, void, 3, 3, "string InternalName")
{
   object->setInternalName(argv[2]);
}

ConsoleMethod( SimObject, addComponents, bool, 3, 64, "%obj.addComponents( %compObjName, %compObjName2, ... );" )
{
   for(S32 i = 2; i < argc; i++)
   {
      SimComponent *obj = dynamic_cast<SimComponent*> (Sim::findObject(argv[i]) );
      if(obj)
         object->addComponent(obj);
      else
         Con::printf("SimComponent::addComponents - Invalid Component Object \"%s\"", argv[i]);
   }
   return true;
}

ConsoleMethod( SimObject, removeComponents, bool, 3, 64, "%obj.removeComponents( %compObjName, %compObjName2, ... );" )
{
   for(S32 i = 2; i < argc; i++)
   {
      SimComponent *obj = dynamic_cast<SimComponent*> (Sim::findObject(argv[i]) );
      if(obj)
         object->removeComponent(obj);
      else
         Con::printf("SimComponent::removeComponents - Invalid Component Object \"%s\"", argv[i]);
   }
   return true;
}

ConsoleMethod( SimObject, getComponentCount, S32, 2, 2, "()")
{
   return object->getComponentCount();
}

ConsoleMethod( SimObject, getComponent, S32, 3, 3, "(idx)")
{
   S32 idx = dAtoi(argv[2]);
   if(idx < 0 || idx >= object->getComponentCount())
   {
      Con::errorf("SimComponent::getComponent - Invalid index %d", idx);
      return 0;
   }

   SimComponent *c = object->getComponent(idx);
   return c ? static_cast<SimObject*>(c)->getId() : 0;
}

ConsoleMethod(SimObject, setEnabled, void, 3, 3, "(enabled)")
{
   object->setEnabled(dAtob(argv[2]));
}

ConsoleMethod(SimObject, isEnabled, bool, 2, 2, "()")
{
   return object->isEnabled();
}

ConsoleMethod( SimObject, getInternalName, const char*, 2, 2, "getInternalName returns the objects internal name")
{
   return object->getInternalName();
}

ConsoleMethod(SimObject, dumpClassHierarchy, void, 2, 2, "obj.dumpClassHierarchy()")
{
   object->dumpClassHierarchy();
}
ConsoleMethod(SimObject, isMemberOfClass, bool, 3, 3, " isMemberOfClass(string classname) -- returns true if this object is a member of the specified class")
{

   AbstractClassRep* pRep = object->getClassRep();
   while(pRep)
   {
      if(!dStricmp(pRep->getClassName(), argv[2]))
      {
         //matches
         return true;
      }

      pRep	=	pRep->getParentClass();
   }

   return false;
}
ConsoleMethod(SimObject, getId, S32, 2, 2, "obj.getId()")
{
   argc; argv;
   return object->getId();
}

ConsoleMethod(SimObject, getGroup, S32, 2, 2, "obj.getGroup()")
{
   argc; argv;
   SimGroup *grp = object->getGroup();
   if(!grp)
      return -1;
   return grp->getId();
}

ConsoleMethod(SimObject, delete, void, 2, 2,"obj.delete()")
{
   argc;argv;
   object->deleteObject();
}

ConsoleMethod(SimObject,schedule, S32, 4, 0, "object.schedule(time, command, <arg1...argN>);")
{
   U32 timeDelta = U32(dAtof(argv[2]));
   argv[2] = argv[3];
   argv[3] = argv[1];
   SimConsoleEvent *evt = new SimConsoleEvent(argc - 2, argv + 2, true);
   S32 ret = Sim::postEvent(object, evt, Sim::getCurrentTime() + timeDelta);
   // #ifdef DEBUG
   //    Con::printf("obj %s schedule(%s) = %d", argv[3], argv[2], ret);
   //    Con::executef( "backtrace");
   // #endif
   return ret;
}

ConsoleMethod(SimObject, getDynamicFieldCount, S32, 2, 2, "obj.getDynamicFieldCount()")
{
   S32 count = 0;
   SimFieldDictionary* fieldDictionary = object->getFieldDictionary();
   for (SimFieldDictionaryIterator itr(fieldDictionary); *itr; ++itr)
      count++;

   return count;
}

ConsoleMethod(SimObject, getDynamicField, const char*, 3, 3, "obj.getDynamicField(index)")
{
   SimFieldDictionary* fieldDictionary = object->getFieldDictionary();
   SimFieldDictionaryIterator itr(fieldDictionary);
   S32 index = dAtoi(argv[2]);
   for (S32 i = 0; i < index; i++)
   {
      if (!(*itr))
      {
         Con::warnf("Invalid dynamic field index passed to SimObject::getDynamicField!");
         return NULL;
      }
      ++itr;
   }

   char* buffer = Con::getReturnBuffer(256);
   if (*itr)
   {
      SimFieldDictionary::Entry* entry = *itr;
      dSprintf(buffer, 256, "%s", entry->slotName);
      return buffer;
   }

   Con::warnf("Invalid dynamic field index passed to SimObject::getDynamicField!");
   return NULL;
}

ConsoleMethod( SimObject, getFieldCount, S32, 2, 2, "() - Gets the number of persistent fields on the object." )
{
   const AbstractClassRep::FieldList &list = object->getFieldList();
   const AbstractClassRep::Field* f;
   U32 numDummyEntries = 0;
   for(int i = 0; i < list.size(); i++)
   {
      f = &list[i];

      if( f->type == AbstractClassRep::DeprecatedFieldType ||
         f->type == AbstractClassRep::StartGroupFieldType ||
         f->type == AbstractClassRep::EndGroupFieldType )
      {
         numDummyEntries++;
      }
   }

   return list.size() - numDummyEntries;
}

ConsoleMethod( SimObject, getField, const char*, 3, 3, "(int index) - Gets the name of the field at the given index." )
{
   S32 index = dAtoi( argv[2] );
   const AbstractClassRep::FieldList &list = object->getFieldList();
   if( ( index < 0 ) || ( index >= list.size() ) )
      return "";

   const AbstractClassRep::Field* f;
   S32 currentField = 0;
   for(int i = 0; i < list.size() && currentField <= index; i++)
   {
      f = &list[i];

      // skip any dummy fields
      if(f->type == AbstractClassRep::DeprecatedFieldType ||
         f->type == AbstractClassRep::StartGroupFieldType ||
         f->type == AbstractClassRep::EndGroupFieldType)
      {
         continue;
      }

      if(currentField == index)
         return f->pFieldname;

      currentField++;
   }

   // if we found nada, return nada.
   return "";
}

ConsoleMethod(SimObject,dump, void, 2, 2, "obj.dump()")
{
   argc; argv;
   const AbstractClassRep::FieldList &list = object->getFieldList();
   char expandedBuffer[4096];

   Con::printf("Member Fields:");
   Vector<const AbstractClassRep::Field *> flist(__FILE__, __LINE__);

   for(U32 i = 0; i < list.size(); i++)
      flist.push_back(&list[i]);

   dQsort(flist.address(),flist.size(),sizeof(AbstractClassRep::Field *),compareFields);

   for(Vector<const AbstractClassRep::Field *>::iterator itr = flist.begin(); itr != flist.end(); itr++)
   {
      const AbstractClassRep::Field* f = *itr;
      if( f->type == AbstractClassRep::DeprecatedFieldType ||
         f->type == AbstractClassRep::StartGroupFieldType ||
         f->type == AbstractClassRep::EndGroupFieldType) continue;

      for(U32 j = 0; S32(j) < f->elementCount; j++)
      {
         // [neo, 07/05/2007 - #3000]
         // Some objects use dummy vars and projected fields so make sure we call the get functions 
         //const char *val = Con::getData(f->type, (void *) (((const char *)object) + f->offset), j, f->table, f->flag);                          
         const char *val = (*f->getDataFn)( object, Con::getData(f->type, (void *) (((const char *)object) + f->offset), j, f->table, f->flag) );// + typeSizes[fld.type] * array1));

         if(!val /*|| !*val*/)
            continue;
         if(f->elementCount == 1)
            dSprintf(expandedBuffer, sizeof(expandedBuffer), "  %s = \"", f->pFieldname);
         else
            dSprintf(expandedBuffer, sizeof(expandedBuffer), "  %s[%d] = \"", f->pFieldname, j);
         expandEscape(expandedBuffer + dStrlen(expandedBuffer), val);
         Con::printf("%s\"", expandedBuffer);
      }
   }

   Con::printf("Tagged Fields:");
   if(object->getFieldDictionary())
      object->getFieldDictionary()->printFields(object);

   Con::printf("Methods:");
   Namespace *ns = object->getNamespace();
   VectorPtr<Namespace::Entry *> vec(__FILE__, __LINE__);

   if(ns)
      ns->getEntryList(&vec);

   bool sawCBs = false;

   for(Vector<Namespace::Entry *>::iterator j = vec.begin(); j != vec.end(); j++)
   {
      Namespace::Entry *e = *j;

      if(e->mType == Namespace::Entry::ScriptCallbackType)
         sawCBs = true;

      if(e->mType < 0)
         continue;

      Con::printf("  %s() - %s", e->mFunctionName, e->mUsage ? e->mUsage : "");
   }

   if(sawCBs)
   {
      Con::printf("Callbacks:");

      for(Vector<Namespace::Entry *>::iterator j = vec.begin(); j != vec.end(); j++)
      {
         Namespace::Entry *e = *j;

         if(e->mType != Namespace::Entry::ScriptCallbackType)
            continue;

         Con::printf("  %s() - %s", e->cb.mCallbackName, e->mUsage ? e->mUsage : "");
      }
   }

}

ConsoleMethod(SimObject, getType, S32, 2, 2, "obj.getType()")
{
   argc; argv;
   return((S32)object->getType());
}

ConsoleMethod(SimObject, isMethod, bool, 3, 3, "obj.isMethod(string method name)")
{
   return object->isMethod( argv[2] );
}

ConsoleMethod(SimObject, isChildOfGroup, bool, 3,3," returns true, if we are in the specified simgroup - or a subgroup thereof")
{
   SimGroup* pGroup = dynamic_cast<SimGroup*>(Sim::findObject(dAtoi(argv[2])));
   if(pGroup)
   {
      return object->isChildOfGroup(pGroup);
   }

   return false;
}

ConsoleMethod(SimObject, getClassNamespace, const char*, 2, 2, "")
{
   return object->getClassNamespace();
}

ConsoleMethod(SimObject, getSuperClassNamespace, const char*, 2, 2, "")
{
   return object->getSuperClassNamespace();
}

ConsoleMethod(SimObject, setClassNamespace, void, 2, 3, "")
{
   object->setClassNamespace(argv[2]);
}

ConsoleMethod(SimObject, setSuperClassNamespace, void, 2, 3, "")
{
   object->setSuperClassNamespace(argv[2]);
}

ConsoleMethod(SimObject, isSelected, bool, 2, 2, "Get whether the object has been marked as selected. (in editor)")
{
   return object->isSelected();
}

ConsoleMethod(SimObject, setIsSelected, void, 3, 3, "Set whether the object has been marked as selected. (in editor)")
{
   object->setSelected(dAtob(argv[2]));
}

ConsoleMethod(SimObject, isExpanded, bool, 2, 2, "Get whether the object has been marked as expanded. (in editor)")
{
   return object->isExpanded();
}

ConsoleMethod(SimObject, setIsExpanded, void, 3, 3, "Set whether the object has been marked as expanded. (in editor)")
{
   object->setExpanded(dAtob(argv[2]));
}

ConsoleMethod(SimObject,isLocked,bool,2,2,"returns the simObject isLocked")
{
	return object->isLocked();
}
