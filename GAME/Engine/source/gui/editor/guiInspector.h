//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUI_INSPECTOR_H_
#define _GUI_INSPECTOR_H_

#include "gui/containers/guiStackCtrl.h"


// Forward Declare GuiInspectorGroup
class GuiInspectorGroup;
// Forward Declare GuiInspectorField
class GuiInspectorField;
// Forward Declare GuiInspectorDatablockField
class GuiInspectorDatablockField;

class GuiInspector : public GuiStackControl
{
private:
   typedef GuiStackControl Parent;
public:
   // Members
   Vector<GuiInspectorGroup*>    mGroups;
   SimObjectPtr<SimObject>       mTarget;

   GuiInspector();
   virtual ~GuiInspector();
   DECLARE_CONOBJECT(GuiInspector);

   virtual void parentResized(const RectI &oldParentRect, const RectI &newParentRect);
   void inspectObject( SimObject *object );
   inline SimObject *getInspectObject() { return mTarget.isNull() ? NULL : mTarget; };
   void setName( StringTableEntry newName );
   void clearGroups();
   bool onAdd();
   bool findExistentGroup( StringTableEntry groupName );
};

#endif