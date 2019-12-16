//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/missionEditor/editor.h"
#include "console/console.h"
#include "console/consoleInternal.h"
#include "gui/controls/guiTextListCtrl.h"
#include "T3D/shapeBase.h"
#include "T3D/gameConnection.h"

#ifndef POWER_PLAYER
// See matching #ifdef in app/game.cpp
bool gEditingMission = false;
#endif

//------------------------------------------------------------------------------
// Class EditManager
//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(EditManager);

EditManager::EditManager()
{
   for(U32 i = 0; i < 10; i++)
      mBookmarks[i] = MatrixF(true);
}

EditManager::~EditManager()
{
}

//------------------------------------------------------------------------------

bool EditManager::onWake()
{
   if(!Parent::onWake())
      return(false);

   for(SimGroupIterator itr(Sim::getRootGroup());  *itr; ++itr)
      (*itr)->onEditorEnable();

   gEditingMission = true;

   return(true);
}

void EditManager::onSleep()
{
   for(SimGroupIterator itr(Sim::getRootGroup());  *itr; ++itr)
   {
      SimObject *so = *itr;
      AssertFatal(so->isProperlyAdded() && !so->isRemoved(), "bad");
      so->onEditorDisable();
   }

   gEditingMission = false;
   Parent::onSleep();
}

//------------------------------------------------------------------------------

bool EditManager::onAdd()
{
   if(!Parent::onAdd())
      return(false);

   // hook the namespace
   const char * name = getName();
   if(name && name[0] && getClassRep())
   {
      Namespace * parent = getClassRep()->getNameSpace();
      Con::linkNamespaces(parent->mName, name);
      mNameSpace = Con::lookupNamespace(name);
   }

   return(true);
}

//------------------------------------------------------------------------------

static GameBase * getControlObj()
{
   GameConnection * connection = GameConnection::getLocalClientConnection();
   ShapeBase* control = 0;
   if(connection)
      control = dynamic_cast<ShapeBase*>(connection->getControlObject());
   return(control);
}

ConsoleMethod( EditManager, setBookmark, void, 3, 3, "(int slot)")
{
   S32 val = dAtoi(argv[2]);
   if(val < 0 || val > 9)
      return;

   GameBase * control = getControlObj();
   if(control)
      object->mBookmarks[val] = control->getTransform();
}

ConsoleMethod( EditManager, gotoBookmark, void, 3, 3, "(int slot)")
{
   S32 val = dAtoi(argv[2]);
   if(val < 0 || val > 9)
      return;

   GameBase * control = getControlObj();
   if(control)
      control->setTransform(object->mBookmarks[val]);
}