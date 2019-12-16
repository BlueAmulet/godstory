//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------


#include "undo.h"
#include "console/console.h"
#include "console/consoleTypes.h"

//-----------------------------------------------------------------------------
// UndoAction
//-----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(UndoAction);
IMPLEMENT_CONOBJECT(UndoScriptAction);

UndoAction::UndoAction(UTF8* actionName)
{
   mActionName = StringTable->insert(actionName);
   mUndoManager = NULL;

   mNSLinkMask = LinkSuperClassName | LinkClassName;
}

UndoAction::~UndoAction()
{
   clearAllNotifications();
}

//-----------------------------------------------------------------------------
void UndoAction::initPersistFields()
{
   Parent::initPersistFields();
   addField("actionName", TypeString, Offset(mActionName, UndoAction), 
      "A brief description of the action, for UI representation of this undo/redo action.");
}

//-----------------------------------------------------------------------------
void UndoAction::addToManager(UndoManager* theMan)
{
   if(theMan)
   {
      mUndoManager = theMan;
      (*theMan).addAction(this);
   }
   else
   {
      mUndoManager = &UndoManager::getDefaultManager();
      mUndoManager->addAction(this);
   }
}

//-----------------------------------------------------------------------------
// UndoManager
//-----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(UndoManager);

UndoManager::UndoManager(U32 levels)
{
   mNumLevels = levels;
   // levels can be arbitrarily high, so we don't really want to reserve(levels).
   mUndoStack.reserve(10);
   mRedoStack.reserve(10);
}

//-----------------------------------------------------------------------------
UndoManager::~UndoManager()
{
   clearStack(mUndoStack);
   clearStack(mRedoStack);
}

//-----------------------------------------------------------------------------
void UndoManager::initPersistFields()
{
   addField("numLevels", TypeS32, Offset(mNumLevels, UndoManager), "Number of undo & redo levels.");
   // arrange for the default undo manager to exist.
//   UndoManager &def = getDefaultManager();
//   Con::printf("def = %s undo manager created", def.getName());
   
}

//-----------------------------------------------------------------------------
UndoManager& UndoManager::getDefaultManager()
{
   // the default manager is created the first time it is asked for.
   static UndoManager *defaultMan = NULL;
   if(!defaultMan)
   {
      defaultMan = new UndoManager();
      defaultMan->assignName("DefaultUndoManager");
      defaultMan->registerObject();
   }
   return *defaultMan;
}

ConsoleMethod(UndoManager, clearAll, void, 2, 2, "Clears the undo manager.")
{
   object->clearAll();
}

void UndoManager::clearAll()
{
   clearStack(mUndoStack); clearStack(mRedoStack);
   Con::executef(this, "onClear");
}

//-----------------------------------------------------------------------------
void UndoManager::clearStack(Vector<UndoAction*> &stack)
{
   Vector<UndoAction*>::iterator itr = stack.begin();
   while (itr != stack.end())
   {
      UndoAction* undo = stack.first();
      stack.pop_front();
      // Don't delete script created undos.
      if (dynamic_cast<UndoScriptAction*>(undo))
         undo->deleteObject();
      else
         delete undo;
   }
   stack.clear();
}

//-----------------------------------------------------------------------------
void UndoManager::clampStack(Vector<UndoAction*> &stack)
{
   while(stack.size() > mNumLevels)
   {
      UndoAction *act = stack.front();
      stack.pop_front();

      UndoScriptAction* scriptAction = dynamic_cast<UndoScriptAction*>(act);
      if (scriptAction)
         scriptAction->deleteObject();
      else
         delete act;
   }
}

void UndoManager::removeAction(UndoAction *action)
{
   Vector<UndoAction*>::iterator itr = mUndoStack.begin();
   while (itr != mUndoStack.end())
   {
      if ((*itr) == action)
      {
         UndoAction* deleteAction = *itr;
         mUndoStack.erase(itr);
         if (!dynamic_cast<UndoScriptAction*>(deleteAction))
            delete deleteAction;
         Con::executef(this, "onRemoveUndo");
         return;
      }
	  itr++;
   }

   itr = mRedoStack.begin();
   while (itr != mRedoStack.end())
   {
      if ((*itr) == action)
      {
         UndoAction* deleteAction = *itr;
         mRedoStack.erase(itr);
         if (!dynamic_cast<UndoScriptAction*>(deleteAction))
            delete deleteAction;
         Con::executef(this, "onRemoveUndo");
         return;
      }
	  itr++;
   }
}

//-----------------------------------------------------------------------------
void UndoManager::undo()
{
   // make sure we have an action available
   if(mUndoStack.size() < 1)
      return;

   // pop the action off the undo stack
   UndoAction *act = mUndoStack.last();
   mUndoStack.pop_back();
   
   // add it to the redo stack
   mRedoStack.push_back(act);
   if(mRedoStack.size() > mNumLevels)
      mRedoStack.pop_front();
   
   Con::executef(this, "onUndo");

   // perform the undo, whatever it may be.
   (*act).undo();
}

//-----------------------------------------------------------------------------
void UndoManager::redo()
{
   // make sure we have an action available
   if(mRedoStack.size() < 1)
      return;

   // pop the action off the redo stack
   UndoAction *react = mRedoStack.last();
   mRedoStack.pop_back();
   
   // add it to the undo stack
   mUndoStack.push_back(react);
   if(mUndoStack.size() > mNumLevels)
      mUndoStack.pop_front();
   
   Con::executef(this, "onRedo");
   
   // perform the redo, whatever it may be.
   (*react).redo();
}

ConsoleMethod(UndoManager, getUndoCount, S32, 2, 2, "")
{
   return object->getUndoCount();
}

S32 UndoManager::getUndoCount()
{
   return mUndoStack.size();
}

ConsoleMethod(UndoManager, getUndoName, const char*, 3, 3, "(index)")
{
   return object->getUndoName(dAtoi(argv[2]));
}

StringTableEntry UndoManager::getUndoName(S32 index)
{
   if ((index < getUndoCount()) && (index >= 0))
      return mUndoStack[index]->mActionName;

   return NULL;
}

ConsoleMethod(UndoManager, getRedoCount, S32, 2, 2, "")
{
   return object->getRedoCount();
}

S32 UndoManager::getRedoCount()
{
   return mRedoStack.size();
}

ConsoleMethod(UndoManager, getRedoName, const char*, 3, 3, "(index)")
{
   return object->getRedoName(dAtoi(argv[2]));
}

StringTableEntry UndoManager::getRedoName(S32 index)
{
   if ((index < getRedoCount()) && (index >= 0))
      return mRedoStack[getRedoCount() - index - 1]->mActionName;

   return NULL;
}

//-----------------------------------------------------------------------------
StringTableEntry UndoManager::getNextUndoName()
{
   if(mUndoStack.size() < 1)
      return NULL;
      
   UndoAction *act = mUndoStack.last();
   return (*act).mActionName;
}

//-----------------------------------------------------------------------------
StringTableEntry UndoManager::getNextRedoName()
{
   if(mRedoStack.size() < 1)
      return NULL;

   UndoAction *act = mRedoStack.last();
   return (*act).mActionName;
}

//-----------------------------------------------------------------------------
void UndoManager::addAction(UndoAction* action)
{
   // push the incoming action onto the stack, move old data off the end if necessary.
   mUndoStack.push_back(action);
   if(mUndoStack.size() > mNumLevels)
      mUndoStack.pop_front();
   
   Con::executef(this, "onAddUndo");

   // clear the redo stack
   clearStack(mRedoStack);
}

//-----------------------------------------------------------------------------
ConsoleMethod(UndoAction, addToManager, void, 2, 3, "action.addToManager([undoManager])")
{
   UndoManager *theMan = NULL;
   if(argc == 3)
   {
      SimObject *obj = Sim::findObject(argv[2]);
      if(obj)
         theMan = dynamic_cast<UndoManager*> (obj);
   }
   object->addToManager(theMan);
}

//-----------------------------------------------------------------------------
ConsoleMethod(UndoManager, undo, void, 2, 2, "UndoManager.undo();")
{
   object->undo();
}

//-----------------------------------------------------------------------------
ConsoleMethod(UndoManager, redo, void, 2, 2, "UndoManager.redo();")
{
   object->redo();
}

//-----------------------------------------------------------------------------
ConsoleMethod(UndoManager, getNextUndoName, const char *, 2, 2, "UndoManager.getNextUndoName();")
{
   StringTableEntry name = object->getNextUndoName();
   if(!name)
      return NULL;

   S32 length = dStrlen(name) + 1;
   char *ret = Con::getReturnBuffer(length);
   dStrcpy(ret, length, name);
   return ret;
}

//-----------------------------------------------------------------------------
ConsoleMethod(UndoManager, getNextRedoName, const char *, 2, 2, "UndoManager.getNextRedoName();")
{
   StringTableEntry name = object->getNextRedoName();
   if(!name)
      return NULL;

   S32 length = dStrlen(name) + 1;
   char *ret = Con::getReturnBuffer(length);
   dStrcpy(ret, length, name);
   return ret;
}
