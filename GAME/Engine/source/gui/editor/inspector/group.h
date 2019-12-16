//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUI_INSPECTOR_GROUP_H_
#define _GUI_INSPECTOR_GROUP_H_

#include "gui/core/guiCanvas.h"
#include "gui/controls/guiTextEditCtrl.h"
#include "gui/buttons/guiBitmapButtonCtrl.h"
#include "gui/containers/guiRolloutCtrl.h"

// Forward refs
class GuiInspector;
class GuiInspectorField;

class GuiInspectorGroup : public GuiRolloutCtrl
{
private:
   typedef GuiRolloutCtrl Parent;
public:
   // Members
   SimObjectPtr<SimObject>             mTarget;
   SimObjectPtr<GuiInspector>          mParent;
   Vector<GuiInspectorField*>          mChildren;
   GuiStackControl*                    mStack;

   // Constructor/Destructor/Conobject Declaration
   GuiInspectorGroup();
   GuiInspectorGroup( SimObjectPtr<SimObject> target, StringTableEntry groupName, SimObjectPtr<GuiInspector> parent );
   virtual ~GuiInspectorGroup();
   DECLARE_CONOBJECT(GuiInspectorGroup);

   // Persistence ( Inspector Exposed Fields )
   static void initPersistFields();

   virtual GuiInspectorField* constructField( S32 fieldType );
   virtual GuiInspectorField* findField( StringTableEntry fieldName );

   // Publicly Accessible Information about this group
   StringTableEntry getGroupName() { return mCaption; };
   SimObjectPtr<SimObject> getGroupTarget() { return mTarget; };
   SimObjectPtr<GuiInspector> getContentCtrl() { return mParent; };

   bool onAdd();
   virtual bool inspectGroup();

   virtual void animateToContents();
protected:
   // overridable method that creates our inner controls.
   virtual bool createContent();
};

#endif // _GUI_INSPECTOR_GROUP_H_
