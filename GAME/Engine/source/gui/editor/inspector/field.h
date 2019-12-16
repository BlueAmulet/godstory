//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUI_INSPECTOR_FIELD_H_
#define _GUI_INSPECTOR_FIELD_H_

#include "gui/core/guiCanvas.h"
#include "gui/shiny/guiTickCtrl.h"
#include "gui/controls/guiTextEditCtrl.h"
#include "gui/buttons/guiBitmapButtonCtrl.h"
#include "gui/controls/guiPopUpCtrl.h"

#include "gui/containers/guiRolloutCtrl.h"

class GuiInspectorGroup;


class GuiInspectorField : public GuiControl
{
private:
   typedef GuiControl Parent;
public:
   // Static Caption Width (in percentage) for all inspector fields
   static S32                 smCaptionWidth;

   // Members
   StringTableEntry           mCaption;
   GuiInspectorGroup*         mParent;
   SimObjectPtr<SimObject>    mTarget;
   AbstractClassRep::Field*   mField;
   StringTableEntry           mFieldArrayIndex;

   // Constructed Field Edit Control
   GuiControl*                mEdit;

   GuiInspectorField( GuiInspectorGroup* parent, SimObjectPtr<SimObject> target, AbstractClassRep::Field* field );
   GuiInspectorField();
   virtual ~GuiInspectorField();
   DECLARE_CONOBJECT(GuiInspectorField);

   virtual void setTarget( SimObjectPtr<SimObject> target ) { mTarget = target; };
   virtual void setParent( GuiInspectorGroup* parent ) { mParent = parent; };
   virtual void setInspectorField( AbstractClassRep::Field *field, const char*arrayIndex = NULL );

protected:
   void registerEditControl( GuiControl *ctrl );
public:
   virtual GuiControl* constructEditControl();
   virtual void        updateValue( StringTableEntry newValue );
   virtual StringTableEntry getFieldName();
   virtual void              setData( StringTableEntry data );
   virtual StringTableEntry  getData();

   virtual bool resize(const Point2I &newPosition, const Point2I &newExtent);
   virtual bool onAdd();
   virtual void onRender(Point2I offset, const RectI &updateRect);
};

#endif // _GUI_INSPECTOR_FIELD_H_
