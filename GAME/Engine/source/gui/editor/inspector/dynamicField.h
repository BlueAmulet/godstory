//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUI_INSPECTOR_DYNAMICFIELD_H_
#define _GUI_INSPECTOR_DYNAMICFIELD_H_

#include "console/simFieldDictionary.h"
#include "gui/editor/inspector/field.h"


class GuiInspectorDynamicField : public GuiInspectorField
{
private:
   typedef GuiInspectorField Parent;
   SimObjectPtr<GuiControl>     mRenameCtrl;
   GuiBitmapButtonCtrl *mDeleteButton;
public:
   SimFieldDictionary::Entry*   mDynField;

   GuiInspectorDynamicField( GuiInspectorGroup* parent, SimObjectPtr<SimObject> target, SimFieldDictionary::Entry* field );
   GuiInspectorDynamicField() {};
   ~GuiInspectorDynamicField() {};
   DECLARE_CONOBJECT(GuiInspectorDynamicField);

   virtual void              setData( StringTableEntry data );
   virtual StringTableEntry  getData();

   virtual StringTableEntry getFieldName() { return ( mDynField != NULL ) ? mDynField->slotName : ""; };

   // Override onAdd so we can construct our custom field name edit control
   virtual bool onAdd();
   // Rename a dynamic field
   void renameField( StringTableEntry newFieldName );
   // Create an edit control to overlay the field name (for renaming dynamic fields)
   GuiControl* constructRenameControl();
   // Override parentResized so we can resize our renaming control
   virtual bool resize(const Point2I &newPosition, const Point2I &newExtent);
};

#endif // _GUI_INSPECTOR_DYNAMICFIELD_H_
