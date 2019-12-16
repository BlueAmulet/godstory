//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/buttons/guiIconButtonCtrl.h"
#include "gui/editor/guiInspector.h"
#include "gui/editor/inspector/dynamicGroup.h"
#include "gui/editor/inspector/dynamicField.h"

IMPLEMENT_CONOBJECT(GuiInspectorDynamicGroup);

//-----------------------------------------------------------------------------
// GuiInspectorDynamicGroup - add custom controls
//-----------------------------------------------------------------------------
bool GuiInspectorDynamicGroup::createContent()
{
   if(!Parent::createContent())
      return false;

   // add a button that lets us add new dynamic fields.
   GuiBitmapButtonCtrl* addFieldBtn = new GuiBitmapButtonCtrl();
   {
      // FIXME Hardcoded image
      addFieldBtn->setBitmap("tools/gui/images/iconAdd.png");

      SimObject* profilePtr = Sim::findObject("EditorButton");
      if( profilePtr != NULL )
         addFieldBtn->setControlProfile( dynamic_cast<GuiControlProfile*>(profilePtr) );

      char commandBuf[64];
      dSprintf(commandBuf, 64, "%d.addDynamicField();", this->getId());
      addFieldBtn->setField("command", commandBuf);
      addFieldBtn->setSizing(horizResizeLeft,vertResizeCenter);
      //addFieldBtn->setField("buttonMargin", "2 2");
      addFieldBtn->resize(Point2I(getWidth() - 20,2), Point2I(16, 16));
      addFieldBtn->registerObject("zAddButton");
   }

   // encapsulate the button in a dummy control.
   GuiControl* shell = new GuiControl();
   shell->setField( "profile", "GuiTransparentProfile" );
   shell->registerObject();

   shell->resize(Point2I(0,0), Point2I(getWidth(), 28));
   shell->addObject(addFieldBtn);

   // save off the shell control, so we can push it to the bottom of the stack in inspectGroup()
   mAddCtrl = shell;
   mStack->addObject(shell);

   return true;
}

static S32 QSORT_CALLBACK compareEntries(const void* a,const void* b)
{
   SimFieldDictionary::Entry *fa = *((SimFieldDictionary::Entry **)a);
   SimFieldDictionary::Entry *fb = *((SimFieldDictionary::Entry **)b);
   return dStricmp(fa->slotName, fb->slotName);
}

//-----------------------------------------------------------------------------
// GuiInspectorDynamicGroup - inspectGroup override
//-----------------------------------------------------------------------------
bool GuiInspectorDynamicGroup::inspectGroup()
{
   // We can't inspect a group without a target!
   if( !mTarget )
      return false;

   // Clearing the fields and recreating them will more than likely be more
   // efficient than looking up existent fields, updating them, and then iterating
   // over existent fields and making sure they still exist, if not, deleting them.
   clearFields();

   // Create a vector of the fields
   Vector<SimFieldDictionary::Entry *> flist;

   // Then populate with fields
   SimFieldDictionary * fieldDictionary = mTarget->getFieldDictionary();
   for(SimFieldDictionaryIterator ditr(fieldDictionary); *ditr; ++ditr)
   {
      flist.push_back(*ditr);
   }
   dQsort(flist.address(),flist.size(),sizeof(SimFieldDictionary::Entry *),compareEntries);

   for(U32 i = 0; i < flist.size(); i++)
   {
      SimFieldDictionary::Entry * entry = flist[i];

      // If the field already exists, just update it
      GuiInspectorField *field = constructField( mTarget->getDataFieldType( entry->slotName, NULL ) );
      if( field == NULL )
         field = new GuiInspectorDynamicField( this, mTarget, entry );
	  // <Edit> [3/5/2009 joy] 未在脚本中注册的变量注册必定失败，直接field->mTarget = mTarget也无效，立即删除即可
	  if( field && !field->mTarget)
	  {
		  delete field;
		  continue;
	  }

      // Register the inspector field and add it to our lists
      field->registerObject();
      mChildren.push_back( field );
      mStack->addObject( field );
   }

   mStack->pushObjectToBack(mAddCtrl);

   setUpdate();

   return true;
}
ConsoleMethod(GuiInspectorDynamicGroup, inspectGroup, bool, 2, 2, "Refreshes the dynamic fields in the inspector.")
{
   return object->inspectGroup();
}

void GuiInspectorDynamicGroup::clearFields()
{
   // save mAddCtrl
   Sim::getGuiGroup()->addObject(mAddCtrl);
   // delete everything else
   mStack->clear();
   // clear the mChildren list.
   mChildren.clear();
   // and restore.
   mStack->addObject(mAddCtrl);
}

SimFieldDictionary::Entry* GuiInspectorDynamicGroup::findDynamicFieldInDictionary( StringTableEntry fieldName )
{
   if( !mTarget )
      return false;

   SimFieldDictionary * fieldDictionary = mTarget->getFieldDictionary();

   for(SimFieldDictionaryIterator ditr(fieldDictionary); *ditr; ++ditr)
   {
      SimFieldDictionary::Entry * entry = (*ditr);

      if( dStricmp( entry->slotName, fieldName ) == 0 )
         return entry;
   }

   return NULL;
}

void GuiInspectorDynamicGroup::addDynamicField()
{
   // We can't add a field without a target
   if( !mTarget || !mStack )
   {
      Con::warnf("GuiInspectorDynamicGroup::addDynamicField - no target SimObject to add a dynamic field to.");
      return;
   }

   // find a field name that is not in use. 
   // But we wont try more than 100 times to find an available field.
   U32 uid = 1;
   char buf[64] = "dynamicField";
   SimFieldDictionary::Entry* entry = findDynamicFieldInDictionary(buf);
   while(entry != NULL && uid < 100)
   {
      dSprintf(buf, sizeof(buf), "dynamicField%03d", uid++);
      entry = findDynamicFieldInDictionary(buf);
   }

   Con::evaluatef( "%d.%s = \"defaultValue\";", mTarget->getId(), buf );

   // now we simply re-inspect the object, to see the new field.
   this->inspectGroup();
   instantExpand();
}

ConsoleMethod( GuiInspectorDynamicGroup, addDynamicField, void, 2, 2, "obj.addDynamicField();" )
{
   object->addDynamicField();
}
