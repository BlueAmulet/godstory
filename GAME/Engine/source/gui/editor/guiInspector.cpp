//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/editor/guiInspector.h"
#include "gui/buttons/guiIconButtonCtrl.h"
#include "gui/editor/inspector/dynamicGroup.h"
#include "gui/containers/guiScrollCtrl.h"
//-----------------------------------------------------------------------------
// GuiInspector
//-----------------------------------------------------------------------------
// The GuiInspector Control houses the body of the inspector.
// It is not exposed as a conobject because it merely does the grunt work
// and is only meant to be used when housed by a scroll control.  Therefore
// the GuiInspector control is a scroll control that creates it's own 
// content.  That content being of course, the GuiInspector control.
IMPLEMENT_CONOBJECT(GuiInspector);

GuiInspector::GuiInspector() : mTarget( NULL )
{
   mGroups.clear();
   mPadding = 1;
}

GuiInspector::~GuiInspector()
{
   clearGroups();
}

bool GuiInspector::onAdd()
{
   if( !Parent::onAdd() )
      return false;

   return true;
}

//-----------------------------------------------------------------------------
// Handle Parent Sizing (We constrain ourself to our parents width)
//-----------------------------------------------------------------------------
void GuiInspector::parentResized(const RectI &oldParentRect, const RectI &newParentRect)
{
   GuiControl *parent = getParent();
   if( parent && dynamic_cast<GuiScrollCtrl*>(parent) != NULL )
   {
      GuiScrollCtrl *scroll = dynamic_cast<GuiScrollCtrl*>(parent);
      setWidth( ( newParentRect.extent.x - ( scroll->scrollBarThickness() + 4  ) ) );
   }
   else
      Parent::parentResized(oldParentRect,newParentRect);
}

bool GuiInspector::findExistentGroup( StringTableEntry groupName )
{
   // If we have no groups, it couldn't possibly exist
   if( mGroups.empty() )
      return false;

   // Attempt to find it in the group list
   Vector<GuiInspectorGroup*>::iterator i = mGroups.begin();

   for( ; i != mGroups.end(); i++ )
   {
      if( dStricmp( (*i)->getGroupName(), groupName ) == 0 )
         return true;
   }

   return false;
}

void GuiInspector::clearGroups()
{
   // If we're clearing the groups, we want to clear our target too.
   mTarget = NULL;

   // If we have no groups, there's nothing to clear!
   if( mGroups.empty() )
      return;

   // Attempt to find it in the group list
   Vector<GuiInspectorGroup*>::iterator i = mGroups.begin();

   for( ; i != mGroups.end(); i++ )
   {
      if((*i) && (*i)->isProperlyAdded())
         (*i)->deleteObject();
   }

   mGroups.clear();
}

void GuiInspector::inspectObject( SimObject *object )
{  
   //GuiCanvas  *guiCanvas = getRoot();
   //if( !guiCanvas )
   //   return;

   //SimObjectPtr<GuiControl> currResponder = guiCanvas->getFirstResponder();

   // If our target is the same as our current target, just update the groups.
   if( mTarget == object )
   {
      Vector<GuiInspectorGroup*>::iterator i = mGroups.begin();
      for ( ; i != mGroups.end(); i++ )
         (*i)->inspectGroup();

      // Don't steal first responder
      //if( !currResponder.isNull() )
      //   guiCanvas->setFirstResponder( currResponder );

      return;
   }

   // Give users a chance to customize fields on this object
   if( object->isMethod("onDefineFieldTypes") )
      Con::executef( object, "onDefineFieldTypes" );

   // Clear our current groups
   clearGroups();

   // Set Target
   mTarget = object;
   
   // Always create the 'general' group (for un-grouped fields)
   GuiInspectorGroup* general = new GuiInspectorGroup( mTarget, "General", this );
   if( general != NULL )
   {
      general->registerObject();
      mGroups.push_back( general );
      addObject( general );
   }

   // Grab this objects field list
   AbstractClassRep::FieldList &fieldList = mTarget->getModifiableFieldList();
   AbstractClassRep::FieldList::iterator itr;

   // Iterate through, identifying the groups and create necessary GuiInspectorGroups
   for(itr = fieldList.begin(); itr != fieldList.end(); itr++)
   {
      if(itr->type == AbstractClassRep::StartGroupFieldType && !findExistentGroup( itr->pGroupname ) )
      {
         GuiInspectorGroup *group = new GuiInspectorGroup( mTarget, itr->pGroupname, this );
         if( group != NULL )
         {
            group->registerObject();
            mGroups.push_back( group );
            addObject( group );
         }            
      }
   }

   // Deal with dynamic fields
   GuiInspectorGroup *dynGroup = new GuiInspectorDynamicGroup( mTarget, "Dynamic Fields", this);
   if( dynGroup != NULL )
   {
      dynGroup->registerObject();
      mGroups.push_back( dynGroup );
      addObject( dynGroup );
   }

   // If the general group is still empty at this point, kill it.
   for(S32 i=0; i<mGroups.size(); i++)
   {
      if(mGroups[i] == general && general->mStack->size() == 0)
      {
         mGroups.erase(i);
         general->deleteObject();
         updatePanes();

         break;
      }
   }
}

void GuiInspector::setName( StringTableEntry newName )
{
   if( mTarget == NULL )
      return;

   StringTableEntry name = StringTable->insert(newName);

   // Only assign a new name if we provide one
   mTarget->assignName(name);

}

ConsoleMethod( GuiInspector, inspect, void, 3, 3, "Inspect(Object)")
{
   SimObject * target = Sim::findObject(argv[2]);
   if(!target)
   {
      if(dAtoi(argv[2]) > 0)
         Con::warnf("%s::inspect(): invalid object: %s", argv[0], argv[2]);

      object->clearGroups();
      return;
   }

   object->inspectObject(target);
}


ConsoleMethod( GuiInspector, getInspectObject, const char*, 2, 2, "getInspectObject() - Returns currently inspected object" )
{
   SimObject *pSimObject = object->getInspectObject();
   if( pSimObject != NULL )
      return pSimObject->getIdString();

   return "";
}

ConsoleMethod( GuiInspector, setName, void, 3, 3, "setName(NewObjectName)")
{
   object->setName(argv[2]);
}
