//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUI_FILETREECTRL_H_
#define _GUI_FILETREECTRL_H_

#include "platform/platform.h"
#include "core/resManager.h"
#include "gui/controls/guiTreeViewCtrl.h"

class GuiFileTreeCtrl : public GuiTreeViewCtrl
{
private:
   typedef GuiTreeViewCtrl Parent;

   // Utility functions
   void recurseInsert( Item* parent, StringTableEntry path );
   void addPathToTree( StringTableEntry path );

protected:
   StringTableEntry     mSelPath;
   StringTableEntry     mFileFilter;
   StringTableEntry     mRootPath;
   
   Vector<StringTableEntry>  mFilters;

public:
   enum
   {
      Icon_Folder = 1,
      Icon_FolderClosed = 2,
      Icon_Doc = 3
   };
   GuiFileTreeCtrl();

   bool onWake();
   bool onAdd();
   bool onVirtualParentExpand(Item *item);
   void onItemSelected( Item *item );
   StringTableEntry getSelectedPath();
   bool setSelectedPath( StringTableEntry path );
   bool buildIconTable(const char * icons);
   
   static void initPersistFields();
   
   void initFilters();
   void addFilter(const char*);
   void removeFilter(const char*);
   bool matchesFilters(const char* filename);

   // Mod Path Parsing
   StringTableEntry getUnit(const char *string, U32 index, const char *set);
   StringTableEntry getUnits(const char *string, S32 startIndex, S32 endIndex, const char *set);
   U32 getUnitCount(const char *string, const char *set);


   DECLARE_CONOBJECT(GuiFileTreeCtrl);
};

#endif //_GUI_FILETREECTRL_H_
