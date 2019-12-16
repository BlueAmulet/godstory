//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/editor/inspector/dynamicField.h"
#include "gui/editor/inspector/dynamicGroup.h"
#include "gui/editor/guiInspector.h"
#include "gui/buttons/guiIconButtonCtrl.h"

//-----------------------------------------------------------------------------
// GuiInspectorDynamicField - Child class of GuiInspectorField 
//-----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiInspectorDynamicField);

GuiInspectorDynamicField::GuiInspectorDynamicField( GuiInspectorGroup* parent, SimObjectPtr<SimObject> target, SimFieldDictionary::Entry* field )
{
   mCaption    = NULL;

   mParent     = parent;
   mTarget     = target;
   mDynField   = field;
   setBounds(0,0,100,20);
   mRenameCtrl = NULL;
   mDeleteButton = NULL;
}

void GuiInspectorDynamicField::setData( StringTableEntry data )
{
   if( mTarget == NULL || mDynField == NULL )
      return;

   char buf[1024];
   const char * newValue = mEdit->getScriptValue();
   dStrcpy( buf, sizeof(buf), newValue ? newValue : "" );
   collapseEscape(buf);

   mTarget->getFieldDictionary()->setFieldValue(mDynField->slotName, buf);

   // Force our edit to update
   updateValue( data );

}

StringTableEntry GuiInspectorDynamicField::getData()
{
   if( mTarget == NULL || mDynField == NULL )
      return "";

   return mTarget->getFieldDictionary()->getFieldValue( mDynField->slotName );
}

void GuiInspectorDynamicField::renameField( StringTableEntry newFieldName )
{
   if( mTarget == NULL || mDynField == NULL || mParent == NULL || mEdit == NULL )
   {
      Con::warnf("GuiInspectorDynamicField::renameField - No target object or dynamic field data found!" );
      return;
   }

   if( !newFieldName )
   {
      Con::warnf("GuiInspectorDynamicField::renameField - Invalid field name specified!" );
      return;
   }

   // Only proceed if the name has changed
   if( dStricmp( newFieldName, getFieldName() ) == 0 )
      return;

   // Grab a pointer to our parent and cast it to GuiInspectorDynamicGroup
   GuiInspectorDynamicGroup *group = dynamic_cast<GuiInspectorDynamicGroup*>(mParent);

   if( group == NULL )
   {
      Con::warnf("GuiInspectorDynamicField::renameField - Unable to locate GuiInspectorDynamicGroup parent!" );
      return;
   }

   char currentValue[512];
   
   // Grab our current dynamic field value (we use a temporary buffer as this gets corrupted upon Con::eval)
   dSprintf(currentValue, 512, getData());

   char szBuffer[512];
   dSprintf(szBuffer, 512, "%d.%s = \"\";",mTarget->getId(), getFieldName());
   Con::evaluate(szBuffer);


   dSprintf(szBuffer, 512, "%d.%s = \"%s\";",mTarget->getId(), newFieldName, currentValue);
   Con::evaluate(szBuffer);

   // Configure our field to grab data from the new dynamic field
   SimFieldDictionary::Entry *newEntry = group->findDynamicFieldInDictionary( newFieldName );

   if( newEntry == NULL )
   {
      Con::warnf("GuiInspectorDynamicField::renameField - Unable to find new field!" );
      return;
   }

   // Assign our dynamic field pointer (where we retrieve field information from) to our new field pointer
   mDynField = newEntry;

   // Lastly we need to reassign our Command and AltCommand fields for our value edit control
   dSprintf( szBuffer, 512, "%d.%s = %d.getText();",mTarget->getId(), newFieldName, mEdit->getId() );
   mEdit->setField("AltCommand", szBuffer );
   mEdit->setField("Validate", szBuffer );

   if (mDeleteButton)
   {
      dSprintf(szBuffer, 512, "%d.%s = \"\";%d.inspectGroup();", mTarget->getId(), newFieldName, group->getId());
      Con::printf(szBuffer);
      mDeleteButton->setField("Command", szBuffer);
   }

   group->inspectGroup();
}

bool GuiInspectorDynamicField::onAdd()
{
   if( !Parent::onAdd() )
      return false;

   pushObjectToBack(mEdit);
   mRenameCtrl = constructRenameControl();
   

   return true;
}

GuiControl* GuiInspectorDynamicField::constructRenameControl()
{
   // Create our renaming field
   GuiControl* retCtrl = new GuiTextEditCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   // Let's make it look pretty.
   retCtrl->setField( "profile", "GuiInspectorTextEditRightProfile" );

   // Don't forget to register ourselves
   char szName[512];
   dSprintf( szName, 512, "IE_%s_%d_%s_Rename", retCtrl->getClassName(), mTarget->getId(), getFieldName() );
   retCtrl->registerObject( szName );


   // Our command will evaluate to :
   //
   //    if( (editCtrl).getText() !$= "" )
   //       (field).renameField((editCtrl).getText());
   //
   char szBuffer[512];
   dSprintf( szBuffer, 512, "if( %d.getText() !$= \"\" ) %d.renameField(%d.getText());",retCtrl->getId(), getId(), retCtrl->getId() );
   dynamic_cast<GuiTextEditCtrl*>(retCtrl)->setText( getFieldName() );
   retCtrl->setField("AltCommand", szBuffer );
   retCtrl->setField("Validate", szBuffer );

   // Calculate Caption Rect (Adjust for 16 pixel wide delete button)
   RectI captionRect(Point2I(getLeft(), 0), Point2I((S32)mFloor(getWidth() * GuiInspectorField::smCaptionWidth / 100.0f), getHeight()));
   RectI valueRect(mEdit->getPosition(), mEdit->getExtent() - Point2I(20, 0));
   RectI deleteRect( Point2I( getLeft() + getWidth() - 20,2), Point2I( 16, getHeight() - 4));
   addObject( retCtrl );

   // Resize the name control to fit in our caption rect (tricksy!)
   retCtrl->resize( captionRect.point, captionRect.extent );
   // resize the value control to leave space for the delete button
   mEdit->resize(valueRect.point, valueRect.extent);

   // Finally, add a delete button for this field
   GuiButtonCtrl * delButt = mDeleteButton = new GuiBitmapButtonCtrl();
   if( delButt != NULL )
   {
      dSprintf(szBuffer, 512, "%d.%s = \"\";%d.inspectGroup();", mTarget->getId(), getFieldName(), mParent->getId());

      // FIXME Hardcoded image
      delButt->setField("Bitmap", "tools/gui/images/iconDelete");
      delButt->setField("Text", "X");
      delButt->setField("Command", szBuffer);
      delButt->setSizing(horizResizeLeft,vertResizeCenter);
      delButt->registerObject();

      delButt->resize( deleteRect.point,deleteRect.extent);

      addObject(delButt);
   }

   return retCtrl;
}

bool GuiInspectorDynamicField::resize( const Point2I &newPosition, const Point2I &newExtent )
{
   if( !Parent::resize( newPosition, newExtent ) )
      return false;

   // If we don't have a field rename control, bail!
   if( mRenameCtrl == NULL )
      return false;

   // Calculate Caption Rect
   RectI captionRect( Point2I(getLeft(), 0), Point2I((S32)mFloor(getWidth() * GuiInspectorField::smCaptionWidth / 100.0f), getHeight()));
   RectI valueRect(mEdit->getPosition(), mEdit->getExtent() - Point2I(20, 0));

   // Resize the edit control to fit in our caption rect (tricksy!)
   return ( mRenameCtrl->resize( captionRect.point, captionRect.extent ) || mEdit->resize( valueRect.point, valueRect.extent) );
}

ConsoleMethod( GuiInspectorDynamicField, renameField, void, 3,3, "field.renameField(newDynamicFieldName);" )
{
   object->renameField( argv[2] );
}
