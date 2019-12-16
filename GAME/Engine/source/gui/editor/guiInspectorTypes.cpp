//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "gui/editor/guiInspectorTypes.h"
#include "gui/editor/inspector/group.h"

#include "gui/controls/guiTextEditSliderCtrl.h"

//-----------------------------------------------------------------------------
// GuiInspectorTypeEnum 
//-----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiInspectorTypeEnum);

GuiControl* GuiInspectorTypeEnum::constructEditControl()
{
   GuiControl* retCtrl = new GuiPopUpMenuCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   GuiPopUpMenuCtrl *menu = dynamic_cast<GuiPopUpMenuCtrl*>(retCtrl);

   // Let's make it look pretty.
   retCtrl->setField( "profile", "InspectorTypeEnumProfile" );

   menu->setField("text", getData());

   registerEditControl( retCtrl );

   // Configure it to update our value when the popup is closed
   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.%s = %d.getText();%d.inspect(%d);",mTarget->getId(), mField->pFieldname, menu->getId(), mParent->mParent->getId(), mTarget->getId() );
   menu->setField("Command", szBuffer );

   //now add the entries
   for(S32 i = 0; i < mField->table->size; i++)
      menu->addEntry(mField->table->table[i].label, mField->table->table[i].index);

   return retCtrl;
}

void GuiInspectorTypeEnum::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeEnum)->setInspectorFieldType("GuiInspectorTypeEnum");
}

void GuiInspectorTypeEnum::updateValue( StringTableEntry newValue )
{
   GuiPopUpMenuCtrl *ctrl = dynamic_cast<GuiPopUpMenuCtrl*>( mEdit );
   if( ctrl != NULL )
      ctrl->setText( newValue );
}

void GuiInspectorTypeEnum::setData( StringTableEntry data )
{
   if( mField == NULL || mTarget == NULL )
      return;

   mTarget->setDataField( mField->pFieldname, NULL, data );

   // Force our edit to update
   updateValue( data );
}

StringTableEntry  GuiInspectorTypeEnum::getData()
{
   if( mField == NULL || mTarget == NULL )
      return "";

   return mTarget->getDataField( mField->pFieldname, NULL );
}

//-----------------------------------------------------------------------------
// GuiInspectorTypeCheckBox 
//-----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiInspectorTypeCheckBox);

GuiControl* GuiInspectorTypeCheckBox::constructEditControl()
{
   GuiControl* retCtrl = new GuiCheckBoxCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   GuiCheckBoxCtrl *check = dynamic_cast<GuiCheckBoxCtrl*>(retCtrl);

   // Let's make it look pretty.
   retCtrl->setField( "profile", "InspectorTypeCheckboxProfile" );
   retCtrl->setField( "text", "" );

   check->mIndent = 4;

   retCtrl->setScriptValue( getData() );

   registerEditControl( retCtrl );

   // Configure it to update our value when the popup is closed
   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.apply(%d.getValue());",getId(),check->getId() );
   check->setField("Command", szBuffer );

   return retCtrl;
}


void GuiInspectorTypeCheckBox::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeBool)->setInspectorFieldType("GuiInspectorTypeCheckBox");
}

//-----------------------------------------------------------------------------
// GuiInspectorTypeGuiProfile 
//-----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiInspectorTypeGuiProfile);

void GuiInspectorTypeGuiProfile::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeGuiProfile)->setInspectorFieldType("GuiInspectorTypeGuiProfile");
}

static S32 QSORT_CALLBACK stringCompare(const void *a,const void *b)
{
   StringTableEntry sa = *(StringTableEntry*)a;
   StringTableEntry sb = *(StringTableEntry*)b;
   return(dStricmp(sa, sb));
}

GuiControl* GuiInspectorTypeGuiProfile::constructEditControl()
{
   GuiControl* retCtrl = new GuiPopUpMenuCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   GuiPopUpMenuCtrl *menu = dynamic_cast<GuiPopUpMenuCtrl*>(retCtrl);

   // Let's make it look pretty.
   retCtrl->setField( "profile", "InspectorTypeEnumProfile" );

   menu->setField("text", getData());

   registerEditControl( retCtrl );

   // Configure it to update our value when the popup is closed
   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.apply(%d.getText());",getId(),menu->getId() );
   menu->setField("Command", szBuffer );

   Vector<StringTableEntry> entries;

   SimGroup * grp = Sim::getGuiDataGroup();
   for(SimGroup::iterator i = grp->begin(); i != grp->end(); i++)
   {
      GuiControlProfile * profile = dynamic_cast<GuiControlProfile *>(*i);
      if(profile)
         entries.push_back(profile->getName());
   }

   // sort the entries
   dQsort(entries.address(), entries.size(), sizeof(StringTableEntry), stringCompare);
   for(U32 j = 0; j < entries.size(); j++)
      menu->addEntry(entries[j], 0);

   return retCtrl;
}


//-----------------------------------------------------------------------------
// GuiInspectorTypeFileName 
//-----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiInspectorTypeFileName);

void GuiInspectorTypeFileName::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeFilename)->setInspectorFieldType("GuiInspectorTypeFileName");
}

GuiControl* GuiInspectorTypeFileName::constructEditControl()
{
   GuiControl* retCtrl = new GuiTextEditCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   // Let's make it look pretty.
   retCtrl->setField( "profile", "GuiInspectorTextEditProfile" );

   // Don't forget to register ourselves
   registerEditControl( retCtrl );

   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.apply(%d.getText());",getId(),retCtrl->getId() );
   retCtrl->setField("AltCommand", szBuffer );
   retCtrl->setField("Validate", szBuffer );

   mBrowseButton = new GuiButtonCtrl();

   if( mBrowseButton != NULL )
   {
      RectI browseRect( Point2I( ( getLeft() + getWidth()) - 26, getTop() + 2), Point2I(20, getHeight() - 4) );
      char szBuffer[512];
      dSprintf( szBuffer, 512, "getLoadFilename(\"*.*\", \"%d.apply\", \"%s\");",getId(), getData());
      mBrowseButton->setField( "Command", szBuffer );
      mBrowseButton->setField( "text", "..." );
      mBrowseButton->setField( "Profile", "GuiInspectorTypeFileNameProfile" );
      mBrowseButton->registerObject();
      addObject( mBrowseButton );

      // Position
      mBrowseButton->resize( browseRect.point, browseRect.extent );
   }

   return retCtrl;
}

bool GuiInspectorTypeFileName::resize( const Point2I &newPosition, const Point2I &newExtent )
{
   if( !Parent::resize( newPosition, newExtent ) )
      return false;

   if( mEdit != NULL )
   {
      // Calculate Caption Rect
      RectI captionRect( getPosition() , Point2I( (S32)mFloor( getWidth() * GuiInspectorField::smCaptionWidth / 100.0f ) - 2, getHeight() ) );

      // Calculate Edit Field Rect
      RectI editFieldRect( Point2I( captionRect.extent.x + 1, 0 ) , Point2I( getWidth() - ( captionRect.extent.x + 25 ) , getHeight() ) );

      bool editResize = mEdit->resize( editFieldRect.point, editFieldRect.extent );
      bool browseResize = false;
      if( mBrowseButton != NULL )
      {
         RectI browseRect( Point2I( ( getLeft() + getWidth()) - 26, 2), Point2I(20, getHeight() - 4) );
         browseResize = mBrowseButton->resize( browseRect.point, browseRect.extent );
      }

      return (editResize || browseResize);
   }

   return false;
}

ConsoleMethod( GuiInspectorTypeFileName, apply, void, 3,3, "apply(newValue);" )
{
	if(!dStrcmp(argv[2],""))
		object->setData(StringTable->insert( "" ));
	else
		object->setData( Platform::makeRelativePathName(argv[2], Platform::getMainDotCsDir()) );
}


//-----------------------------------------------------------------------------
// GuiInspectorTypeColor (Base for ColorI/ColorF) 
//-----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiInspectorTypeColor);

GuiControl* GuiInspectorTypeColor::constructEditControl()
{
   GuiControl* retCtrl = new GuiTextEditCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   // Let's make it look pretty.
   retCtrl->setField( "profile", "GuiInspectorTextEditProfile" );

   // Don't forget to register ourselves
   registerEditControl( retCtrl );

   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.apply(%d.getText());",getId(), retCtrl->getId() );
   retCtrl->setField("AltCommand", szBuffer );
   retCtrl->setField("Validate", szBuffer );

   mBrowseButton = new GuiButtonCtrl();

   if( mBrowseButton != NULL )
   {
      RectI browseRect( Point2I( ( getLeft() + getWidth()) - 26, getTop() + 2), Point2I(20, getHeight() - 4) );
      char szBuffer[512];
      dSprintf( szBuffer, 512, "%s(\"%s\", \"%d.apply\", %d.getRoot());", mColorFunction, getData(), getId(), getId());
      mBrowseButton->setField( "Command", szBuffer );
      mBrowseButton->setField( "text", "..." );
      mBrowseButton->setField( "Profile", "GuiInspectorTypeFileNameProfile" );
      mBrowseButton->registerObject();
      addObject( mBrowseButton );

      // Position
      mBrowseButton->resize( browseRect.point, browseRect.extent );
   }

   return retCtrl;
}

bool GuiInspectorTypeColor::resize( const Point2I &newPosition, const Point2I &newExtent )
{
   if( !Parent::resize( newPosition, newExtent ) )
      return false;

   if( mEdit != NULL )
   {
      // Calculate Caption Rect
      RectI captionRect( getPosition() , Point2I( (S32)mFloor( getWidth() * GuiInspectorField::smCaptionWidth / 100.0f) - 2, getHeight() ));

      // Calculate Edit Field Rect
      RectI editFieldRect( Point2I( captionRect.extent.x + 1, 0 ) , Point2I( getWidth() - ( captionRect.extent.x + 25 ) , getHeight() ) );

      bool editResize = mEdit->resize( editFieldRect.point, editFieldRect.extent );
      bool browseResize = false;

      if( mBrowseButton != NULL )
      {
         RectI browseRect( Point2I( ( getLeft() + getWidth()) - 26, 2), Point2I(20, getHeight() - 4) );
         browseResize = mBrowseButton->resize( browseRect.point, browseRect.extent );
      }

      return (editResize || browseResize);
   }
   return false;
}


//-----------------------------------------------------------------------------
// GuiInspectorTypeColorI
//-----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiInspectorTypeColorI);

void GuiInspectorTypeColorI::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeColorI)->setInspectorFieldType("GuiInspectorTypeColorI");
}

GuiInspectorTypeColorI::GuiInspectorTypeColorI()
{
   mColorFunction = StringTable->insert("getColorI");
}

//-----------------------------------------------------------------------------
// GuiInspectorTypeColorF
//-----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiInspectorTypeColorF);

void GuiInspectorTypeColorF::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeColorF)->setInspectorFieldType("GuiInspectorTypeColorF");
}

GuiInspectorTypeColorF::GuiInspectorTypeColorF()
{
   mColorFunction = StringTable->insert("getColorF");
}

//-----------------------------------------------------------------------------
// GuiInspectorTypeS32
//-----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiInspectorTypeS32);

void GuiInspectorTypeS32::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeS32)->setInspectorFieldType("GuiInspectorTypeS32");
}

GuiControl* GuiInspectorTypeS32::constructEditControl()
{
   GuiControl* retCtrl = new GuiTextEditSliderCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   retCtrl->setField( "profile", "GuiInspectorTextEditProfile" );

   // Don't forget to register ourselves
   registerEditControl( retCtrl );

   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.apply(%d.getText());",getId(), retCtrl->getId() );
   retCtrl->setField("AltCommand", szBuffer );
   retCtrl->setField("Validate", szBuffer );
   retCtrl->setField("increment","1");
   retCtrl->setField("format","%d");
   retCtrl->setField("range","-2147483648 2147483647");

   return retCtrl;
}

