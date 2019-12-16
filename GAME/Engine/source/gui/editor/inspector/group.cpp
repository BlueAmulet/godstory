//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "gui/editor/guiInspector.h"
#include "gui/editor/inspector/group.h"
#include "gui/editor/inspector/dynamicField.h"
#include "gui/editor/inspector/datablockField.h"
#include "gui/buttons/guiIconButtonCtrl.h"

//-----------------------------------------------------------------------------
// GuiInspectorGroup
//-----------------------------------------------------------------------------
//
// The GuiInspectorGroup control is a helper control that the inspector
// makes use of which houses a collapsible pane type control for separating
// inspected objects fields into groups.  The content of the inspector is 
// made up of zero or more GuiInspectorGroup controls inside of a GuiStackControl
//
//
//
IMPLEMENT_CONOBJECT(GuiInspectorGroup);

GuiInspectorGroup::GuiInspectorGroup() : mTarget( NULL ), mParent( NULL ), mStack(NULL)
{
   setBounds(0,0,200,20);

   mChildren.clear();

   mCanSave = false;

   // Make sure we receive our ticks.
   setProcessTicks();
}

GuiInspectorGroup::GuiInspectorGroup( SimObjectPtr<SimObject> target, StringTableEntry groupName, SimObjectPtr<GuiInspector> parent ) :
mTarget( target ), mParent( parent ), mStack(NULL)
{

   setBounds(0,0,200,20);

   mCaption = StringTable->insert( groupName );
   mCanSave = false;

   mChildren.clear();
}

GuiInspectorGroup::~GuiInspectorGroup()
{
   if( !mChildren.empty() )
      mChildren.clear();
}

//-----------------------------------------------------------------------------
// Persistence 
//-----------------------------------------------------------------------------
void GuiInspectorGroup::initPersistFields()
{
   Parent::initPersistFields();

   addField("Caption", TypeString, Offset(mCaption, GuiInspectorGroup));
}

//-----------------------------------------------------------------------------
// Scene Events
//-----------------------------------------------------------------------------
bool GuiInspectorGroup::onAdd()
{
   setField( "profile", "GuiInspectorGroupProfile" );

   if( !Parent::onAdd() )
      return false;

   // Create our inner controls. Allow subclasses to provide other content.
   if(!createContent())
      return false;

   inspectGroup();

   return true;
}

bool GuiInspectorGroup::createContent()
{
   // Create our field stack control
   mStack = new GuiStackControl();
   if( !mStack )
      return false;

   // Prefer GuiTransperantProfile for the stack.
   mStack->setField( "profile", "GuiTransparentProfile" );
   mStack->registerObject();

   addObject( mStack );
   mStack->setField( "padding", "0" );

   return true;
}

//-----------------------------------------------------------------------------
// Control Sizing Animation Functions
//-----------------------------------------------------------------------------
void GuiInspectorGroup::animateToContents()
{
   calculateHeights();
   if(size() > 0)
      animateTo( mExpanded.extent.y );
   else
      animateTo( mHeader.extent.y );
}

GuiInspectorField* GuiInspectorGroup::constructField( S32 fieldType )
{
   // See if we can construct a field of this type
   ConsoleBaseType *cbt = ConsoleBaseType::getType(fieldType);
   if( !cbt )
      return NULL;

   // Alright, is it a datablock?
   if(cbt->isDatablock())
   {
      // Default to GameBaseData
      StringTableEntry typeClassName = cbt->getTypeClassName();

      if (mTarget)
      {
         // Try and setup the classname based on the object type
         char className[256];
         dSprintf(className,256,"%sData",mTarget->getClassName());
         // Walk the ACR list and find a matching class if any.
         AbstractClassRep *walk = AbstractClassRep::getClassList();
         while(walk)
         {
            if(!dStricmp(walk->getClassName(), className))
               break;

            walk = walk->getNextClass();
         }

         // We found a valid class
         if (walk)
            typeClassName = walk->getClassName();

      }


      GuiInspectorDatablockField *dbFieldClass = new GuiInspectorDatablockField( typeClassName );
      if( dbFieldClass != NULL )
      {
         // return our new datablock field with correct datablock type enumeration info
         return dbFieldClass;
      }
   }

   // Nope, not a datablock. So maybe it has a valid inspector field override we can use?
   if(!cbt->getInspectorFieldType())
      // Nothing, so bail.
      return NULL;

   // Otherwise try to make it!
   ConsoleObject *co = create(cbt->getInspectorFieldType());
   GuiInspectorField *gif = dynamic_cast<GuiInspectorField*>(co);

   if(!gif)
   {
      // Wasn't appropriate type, bail.
      delete co;
      return NULL;
   }

   return gif;
}

GuiInspectorField *GuiInspectorGroup::findField( StringTableEntry fieldName )
{
   // If we don't have any field children we can't very well find one then can we?
   if( mChildren.empty() )
      return NULL;

   Vector<GuiInspectorField*>::iterator i = mChildren.begin();

   for( ; i != mChildren.end(); i++ )
   {
      if( (*i)->getFieldName() != NULL && dStricmp( (*i)->getFieldName(), fieldName ) == 0 )
         return (*i);
   }

   return NULL;
}

bool GuiInspectorGroup::inspectGroup()
{
   // We can't inspect a group without a target!
   if( !mTarget )
      return false;

   // to prevent crazy resizing, we'll just freeze our stack for a sec..
   mStack->freeze(true);

   bool bNoGroup = false;

   // Un-grouped fields are all sorted into the 'general' group
   if ( dStricmp( mCaption, "General" ) == 0 )
      bNoGroup = true;

   AbstractClassRep::FieldList &fieldList = mTarget->getModifiableFieldList();
   AbstractClassRep::FieldList::iterator itr;

   bool bGrabItems = false;
   bool bNewItems = false;

   for(itr = fieldList.begin(); itr != fieldList.end(); itr++)
   {
      if( itr->type == AbstractClassRep::StartGroupFieldType )
      {
         // If we're dealing with general fields, always set grabItems to true (to skip them)
         if( bNoGroup == true )
            bGrabItems = true;
         else if( itr->pGroupname != NULL && dStricmp( itr->pGroupname, mCaption ) == 0 )
            bGrabItems = true;
         continue;
      }
      else if ( itr->type == AbstractClassRep::EndGroupFieldType )
      {
         // If we're dealing with general fields, always set grabItems to false (to grab them)
         if( bNoGroup == true )
            bGrabItems = false;
         else if( itr->pGroupname != NULL && dStricmp( itr->pGroupname, mCaption ) == 0 )
            bGrabItems = false;
         continue;
      }

      if( ( bGrabItems == true || ( bNoGroup == true && bGrabItems == false ) ) && itr->type != AbstractClassRep::DeprecatedFieldType )
      {
         if( bNoGroup == true && bGrabItems == true )
            continue; 
         // This is weird, but it should work for now. - JDD
         // We are going to check to see if this item is an array
         // if so, we're going to construct a field for each array element
         if( itr->elementCount > 1 )
         {
            for(S32 nI = 0; nI < itr->elementCount; nI++)
            {
               FrameTemp<char> intToStr( 64 );
               dSprintf( intToStr, 64, "%d", nI );

               const char *val = mTarget->getDataField( itr->pFieldname, intToStr );
               if (!val)
                  val = StringTable->lookup("");


               // Copy Val and construct proper ValueName[nI] format 
               //      which is "ValueName0" for index 0, etc.
               S32 frameTempSize = dStrlen( val ) + 32;
               FrameTemp<char> valCopy( frameTempSize );
               dSprintf( (char *)valCopy, frameTempSize, "%s%d", itr->pFieldname, nI );

               // If the field already exists, just update it
               GuiInspectorField *field = findField( valCopy );
               if( field != NULL )
               {
                  field->updateValue( field->getData() );
                  continue;
               }

               bNewItems = true;

               field = constructField( itr->type );
               if( field == NULL )
               {
                  field = new GuiInspectorField( this, mTarget, itr );
                  field->setInspectorField( itr, intToStr );
               }
               else
               {
                  field->setTarget( mTarget );
                  field->setParent( this );
                  field->setInspectorField( itr, intToStr );
               }

               field->registerObject();
               mChildren.push_back( field );
               mStack->addObject( field );
            }
         }
         else
         {
            // If the field already exists, just update it
            GuiInspectorField *field = findField( itr->pFieldname );
            if( field != NULL )
            {
               field->updateValue( field->getData() );
               continue;
            }

            bNewItems = true;

            field = constructField( itr->type );
            if( field == NULL )
               field = new GuiInspectorField( this, mTarget, itr );
            else
            {
               field->setTarget( mTarget );
               field->setParent( this );
               field->setInspectorField( itr );
            }

            field->registerObject();
            mChildren.push_back( field );
            mStack->addObject( field );

         }       
      }
   }
   mStack->freeze(false);
   mStack->updatePanes();

   // If we've no new items, there's no need to resize anything!
   if( bNewItems == false && !mChildren.empty() )
      return true;

   sizeToContents();

   setUpdate();

   return true;
}
