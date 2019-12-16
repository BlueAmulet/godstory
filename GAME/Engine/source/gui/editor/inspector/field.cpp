//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/buttons/guiIconButtonCtrl.h"
#include "gui/editor/inspector/field.h"
#include "util/safeDelete.h"
#include "gfx/gfxFontRenderBatcher.h"
//-----------------------------------------------------------------------------
// GuiInspectorField
//-----------------------------------------------------------------------------
// The GuiInspectorField control is a representation of a single abstract
// field for a given ConsoleObject derived object.  It handles creation
// getting and setting of it's fields data and editing control.  
//
// Creation of custom edit controls is done through this class and is
// dependent upon the dynamic console type, which may be defined to be
// custom for different types.
//
// Note : GuiInspectorField controls must have a GuiInspectorGroup as their
//        parent.  
IMPLEMENT_CONOBJECT(GuiInspectorField);

// Caption width is in percentage of total width
S32 GuiInspectorField::smCaptionWidth = 50;

GuiInspectorField::GuiInspectorField( GuiInspectorGroup* parent, SimObjectPtr<SimObject> target, AbstractClassRep::Field* field ) :
mParent( parent ), mTarget( target ), mField( field ), mFieldArrayIndex( NULL ), mEdit( NULL )
{
   if( field != NULL )
      mCaption    = StringTable->insert( field->pFieldname );
   else
      mCaption    = StringTable->insert( "" );

   mCanSave = false;
   setBounds(0,0,100,18);
}

GuiInspectorField::GuiInspectorField() : mParent( NULL ), mTarget( NULL ), mEdit( NULL ),
mField( NULL ), mFieldArrayIndex( NULL ), mCaption( StringTable->insert( "" ) )
{
   setBounds(0,0,100,18);
   mCanSave = false;
}

GuiInspectorField::~GuiInspectorField()
{
}

//-----------------------------------------------------------------------------
// Get/Set Data Functions
//-----------------------------------------------------------------------------
void GuiInspectorField::setData( StringTableEntry data )
{
   if( mField == NULL || mTarget == NULL )
      return;

   data = StringTable->insert(data, true);

   mTarget->inspectPreApply();

   mTarget->setDataField( mField->pFieldname, mFieldArrayIndex, data );
   
   // give the target a chance to validate
   mTarget->inspectPostApply();

   // Force our edit to update
   updateValue( mTarget->getDataField( mField->pFieldname, mFieldArrayIndex));

}

StringTableEntry GuiInspectorField::getData()
{
   if( mField == NULL || mTarget == NULL )
      return "";

   return mTarget->getDataField( mField->pFieldname, mFieldArrayIndex );
}

void GuiInspectorField::setInspectorField( AbstractClassRep::Field *field, const char*arrayIndex ) 
{
   mField = field; 

   if( arrayIndex != NULL )
   {
      mFieldArrayIndex = StringTable->insert( arrayIndex );

      S32 frameTempSize = dStrlen( getFieldName() ) + 32;
      FrameTemp<char> valCopy( frameTempSize );
      dSprintf( (char *)valCopy, frameTempSize, "%s%s", getFieldName(), arrayIndex );

      mCaption = StringTable->insert( valCopy ); 
   }
   else
      mCaption = getFieldName();
}


StringTableEntry GuiInspectorField::getFieldName() 
{ 
   // Sanity
   if ( mField == NULL )
      return StringTable->lookup("");

   // Array element?
   if( mFieldArrayIndex != NULL )
   {
      S32 frameTempSize = dStrlen( mField->pFieldname ) + 32;
      FrameTemp<char> valCopy( frameTempSize );
      dSprintf( (char *)valCopy, frameTempSize, "%s%s", mField->pFieldname, mFieldArrayIndex );

      // Return formatted element
      return StringTable->insert( valCopy );
   }

   // Plain ole field name.
   return mField->pFieldname; 
};
//-----------------------------------------------------------------------------
// Overrideables for custom edit fields
//-----------------------------------------------------------------------------
GuiControl* GuiInspectorField::constructEditControl()
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


   return retCtrl;
}

void GuiInspectorField::registerEditControl( GuiControl *ctrl )
{
   if(!mTarget)
      return;

   char szName[512];
   dSprintf( szName, 512, "IE_%s_%d_%s_Field", ctrl->getClassName(), mTarget->getId(),mCaption);

   // Register the object
   ctrl->registerObject( szName );
}

void GuiInspectorField::onRender(Point2I offset, const RectI &updateRect)
{
   if(mCaption && mCaption[0])
   {
      // Calculate Caption Rect
      RectI captionRect( offset , Point2I( (S32)mFloor( getWidth() * (F32)( GuiInspectorField::smCaptionWidth / 100.0f ) ), getHeight() ) );
      // Calculate Y Offset to center vertically the caption
      U32 captionYOffset = (U32)mFloor( (F32)( captionRect.extent.y - mProfile->mFont->getHeight() ) / 2 );

      RectI clipRect = GFX->getClipRect();

      if( clipRect.intersect( captionRect ) )
      {
         // Backup Bitmap Modulation
         ColorI currColor;
         GFX->getDrawUtil()->getBitmapModulation( &currColor );

         GFX->getDrawUtil()->setBitmapModulation( mProfile->mFontColor );

         GFX->setClipRect( RectI( clipRect.point, Point2I( captionRect.extent.x, clipRect.extent.y ) ));
         // Draw Caption ( Vertically Centered )
         U32 textY = captionRect.point.y + captionYOffset;
         U32 textX = captionRect.point.x + captionRect.extent.x - mProfile->mFont->getStrWidth(const_cast<char*>(mCaption)) - 6;
         Point2I textPT(textX, textY);

         GFX->getDrawUtil()->drawText( mProfile->mFont, textPT, mCaption, &mProfile->mFontColor );

         GFX->getDrawUtil()->setBitmapModulation( currColor );

         GFX->setClipRect( clipRect );
      }
   }

   Parent::onRender( offset, updateRect );
}

bool GuiInspectorField::onAdd()
{
   if( !Parent::onAdd() )
      return false;

   if( !mTarget )
      return false;

   mEdit = constructEditControl();

   if( mEdit == NULL )
      return false;

   // Add our edit as a child
   addObject( mEdit );

   // Calculate Caption Rect
   RectI captionRect( getPosition() , Point2I( (S32)mFloor( getWidth() * (F32)(GuiInspectorField::smCaptionWidth / 100.0f ) ), getHeight() ) );

   // Calculate Edit Field Rect
   RectI editFieldRect(Point2I(captionRect.extent.x + 1, 1), Point2I(getWidth() - (captionRect.extent.x + 5), getHeight() - 1));

   // Resize to fit properly in allotted space
   mEdit->resize( editFieldRect.point, editFieldRect.extent );

   // Prefer GuiInspectorFieldProfile
   setField( "profile", "GuiInspectorFieldProfile" );

   // Force our editField to set it's value
   updateValue( getData() );

   return true;
}

void GuiInspectorField::updateValue( StringTableEntry newValue )
{
   GuiTextEditCtrl *ctrl = dynamic_cast<GuiTextEditCtrl*>( mEdit );
   if( ctrl != NULL )
      ctrl->setText( newValue );
}

bool GuiInspectorField::resize( const Point2I &newPosition, const Point2I &newExtent )
{
   if( !Parent::resize( newPosition, newExtent ) )
      return false;

   if( mEdit != NULL )
   {
      // Calculate Caption Rect
      RectI captionRect(getPosition() , Point2I((S32)mFloor(getWidth() * (F32)(GuiInspectorField::smCaptionWidth / 100.0f)), getHeight() ) );

      // Calculate Edit Field Rect
      RectI editFieldRect( Point2I( captionRect.extent.x + 1, 1 ) , Point2I( getWidth() - ( captionRect.extent.x + 5 ) , getHeight() - 1) );

      return mEdit->resize( editFieldRect.point, editFieldRect.extent );
   }

   return false;
}


ConsoleMethod( GuiInspectorField, apply, void, 3,3, "apply(newValue);" )
{
   object->setData( argv[2] );
}
