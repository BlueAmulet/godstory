//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/controls/GuiFileTreeCtrl.h"
#include "core/findMatch.h"
#include "core/frameAllocator.h"

IMPLEMENT_CONOBJECT(GuiFileTreeCtrl);

static bool _isDirInMainDotCsPath(const char* dir)
{
   StringTableEntry cs = Platform::getMainDotCsDir();
   U32 len = dStrlen(cs) + dStrlen(dir) + 2;
   FrameTemp<UTF8> fullpath(len);
   dSprintf(fullpath, len, "%s/%s", cs, dir);

   return Platform::isDirectory(fullpath);
}

static bool _hasChildren(const char* path)
{
   if( Platform::hasSubDirectory(path))
      return true;
      
   Vector<StringTableEntry> dummy;
   Platform::dumpDirectories( path, dummy, 0, true);
   
   return dummy.size() > 0;
}

GuiFileTreeCtrl::GuiFileTreeCtrl(): GuiTreeViewCtrl()
{
   // Parent configuration
   setBounds(0,0,200,100);
   mDestroyOnSleep = false;
   mSupportMouseDragging = false;
   mMultipleSelections = false;

   mSelPath = StringTable->insert("");
   mFileFilter = StringTable->insert("*.cs *.gui *.ed.cs");
   mRootPath = StringTable->insert("");
   initFilters();
}

bool GuiFileTreeCtrl::onAdd()
{
   if( !Parent::onAdd() )
      return false;

   // Specify our icons
   buildIconTable( NULL );

   return true;
}

static void _dumpFiles(const char *path, Vector<StringTableEntry> &directoryVector, S32 depth = 0)
{
   Vector<Platform::FileInfo> fileVec;
   Platform::dumpPath( path, fileVec, depth);
   
   for(U32 i = 0; i < fileVec.size(); i++)
   {
      directoryVector.push_back( StringTable->insert(fileVec[i].pFileName) );
   }
}
//   static bool dumpPath(const char *in_pBasePath, Vector<Platform::FileInfo>& out_rFileVector, S32 recurseDepth = -1);
//   static bool dumpDirectories( const char *path, Vector<StringTableEntry> &directoryVector, S32 depth = 0, bool noBasePath = false );


bool GuiFileTreeCtrl::onWake()
{
   if( !Parent::onWake() )
      return false;

   // Kill off any existing items
   destroyTree();

   // Here we're going to grab our system volumes from the platform layer and create them as roots
   //
   // Note : that we're passing a 1 as the last parameter to Platform::dumpDirectories, which tells it
   // how deep to dump in recursion.  This is an optimization to keep from dumping the whole file system
   // to the tree.  The tree will dump more paths as necessary when the virtual parents are expanded,
   // much as windows does.

   ResourceManager->initExcludedDirectories();

   // get the files in the main.cs dir
   StringTableEntry rootPath = Platform::getMainDotCsDir();
   Vector<StringTableEntry> pathVec;
   Platform::dumpDirectories( rootPath, pathVec, 0, true);
   _dumpFiles( rootPath, pathVec, 0);
   if( ! pathVec.empty() )
   {

      // get the last folder in the path.
//      char *dirname = dStrdup(rootPath);
//      U32 last = dStrlen(dirname)-1;
//      if(dirname[last] == '/')
//         dirname[last] = '\0';
//      char* lastPathComponent = dStrrchr(dirname,'/');
//      if(lastPathComponent)
//         *lastPathComponent++ = '\0';
//      else
//         lastPathComponent = dirname;
      
         
      // Iterate through the returned paths and add them to the tree
      Vector<StringTableEntry>::iterator j = pathVec.begin();
      for( ; j != pathVec.end(); j++ )
      {
         char fullModPathSub [512];
         dMemset( fullModPathSub, 0, 512 );
//         dSprintf( fullModPathSub, 512, "%s/%s", lastPathComponent, (*j) );
//         addPathToTree( *j );
      }
//      dFree(dirname);
   }

   // Success!
   return true;
}

bool GuiFileTreeCtrl::onVirtualParentExpand(Item *item)
{
   if( !item || !item->isExpanded() )
      return true;

   StringTableEntry pathToExpand = item->getValue();

   if( !pathToExpand )
   {
      Con::errorf("GuiFileTreeCtrl::onVirtualParentExpand - Unable to retrieve item value!");
      return false;
   }

   Vector<StringTableEntry> pathVec;
   _dumpFiles( pathToExpand, pathVec, 0 );
   Platform::dumpDirectories( pathToExpand, pathVec, 1, true);
   if( ! pathVec.empty() )
   {
      // Iterate through the returned paths and add them to the tree
      Vector<StringTableEntry>::iterator i = pathVec.begin();
      for( ; i != pathVec.end(); i++ )
         recurseInsert(item, (*i) );

      item->setExpanded( true );
   }


   item->setVirtualParent( false );

   // Update our tree view
   buildVisibleTree();

   return true;

}



bool GuiFileTreeCtrl::buildIconTable(const char * icons)
{
   // Icons should be designated by the bitmap/png file names (minus the file extensions)
   // and separated by colons (:).
   if (!icons)
      icons = StringTable->insert("common/gui/images/folder:common/gui/images/folder:common/gui/images/folderClosed:common/gui/images/file");

   return Parent::buildIconTable( icons );
}

void GuiFileTreeCtrl::addPathToTree( StringTableEntry path )
{
   if( !path )
   {
      Con::errorf("GuiFileTreeCtrl::addPathToTree - Invalid Path!");
      return;
   }

   // Identify which root (volume) this path belongs to (if any)
   S32 root = getFirstRootItem();
   StringTableEntry ourPath = &path[ dStrcspn( path, "//" ) + 1];
   StringTableEntry ourRoot = getUnit( path, 0, "//" );
   // There are no current roots, we can safely create one
   if( root == 0 )
   {
      recurseInsert( NULL, path );
   }
   else
   {
      while( root != 0 )
      {
         if( dStrcmp( getItemValue( root ), ourRoot ) == 0 )
         {
            recurseInsert( getItem( root ), ourPath );
            break;
         }
         root = this->getNextSiblingItem( root );
      }
      // We found none so we'll create one
      if ( root == 0 )
      {
         recurseInsert( NULL, path );
      }
   }
}

void GuiFileTreeCtrl::onItemSelected( Item *item )
{
   Con::executef( this, "onSelectPath", avar("%s",item->getValue()) );

   mSelPath = StringTable->insert( item->getValue() );

   if( _hasChildren( item->getValue() ) )
      item->setVirtualParent( true );
}

void GuiFileTreeCtrl::initFilters()
{
   mFilters.clear();
   addFilter("*.cs");
   addFilter("*.gui");
   addFilter("*.mis");
}

S32 _findStringInVector(StringTableEntry s, Vector<StringTableEntry> &v)
{
   for(int i = 0; i < v.size(); i++)
   {
      if(v[i] == s)
         return i;
   }
   return -1;
}

void GuiFileTreeCtrl::addFilter(const char* newfilt)
{
   StringTableEntry s = StringTable->insert(newfilt);
   if(_findStringInVector(s, mFilters) < 0)
      mFilters.push_back(s);
}

void GuiFileTreeCtrl::removeFilter(const char* filt)
{
   StringTableEntry s = StringTable->insert(filt);
   S32 i = _findStringInVector(s, mFilters);
   if(i >=0)
      mFilters.erase_fast(i);
}

bool GuiFileTreeCtrl::matchesFilters(const char* filename)
{
   for(int i = 0; i < mFilters.size(); i++)
   {
      if(FindMatch::isMatch( mFilters[i], filename))
         return true;
   }
   return false;
}

void GuiFileTreeCtrl::recurseInsert( Item* parent, StringTableEntry path )
{
   if( !path )
      return;

   char szPathCopy [ 1024 ];
   dMemset( szPathCopy, 0, 1024 );
   dStrcpy( szPathCopy, sizeof(szPathCopy), path );

   // Jump over the first character if it's a root /
   char *curPos = szPathCopy;
   if( *curPos == '/' )
      curPos++;

   char szValue[1024];
   dMemset( szValue, 0, 1024 );
   if( parent )
   {
      dMemset( szValue, 0, 1024 );
      dSprintf( szValue, 1024, "%s/%s", parent->getValue(), curPos );
   }
   
   char *delim = dStrchr( curPos, '/' );
   if ( delim )
   {
      // terminate our / and then move our pointer to the next character (rest of the path)
      *delim = 0x00;
      delim++;
   }
   S32 itemIndex = 0;
   // only insert blindly if we have no root
   if( !parent )
   {
      itemIndex = insertItem( 0, curPos, curPos );
      getItem( itemIndex )->setNormalImage( Icon_FolderClosed );
      getItem( itemIndex )->setExpandedImage( Icon_Folder );
   }
   else
   {
      bool allowed = (_isDirInMainDotCsPath(szValue) || matchesFilters(szValue));
      Item *exists = parent->findChildByValue( szValue );
      if( allowed && !exists && dStrcmp( curPos, "" ) != 0 )
      {
         // Since we're adding a child this parent can't be a virtual parent, so clear that flag
         parent->setVirtualParent( false );

         itemIndex = insertItem( parent->getID(), curPos);
         Item *newitem = getItem(itemIndex);
         newitem->setValue(szValue);

      }
      else
      {
         itemIndex = ( parent != NULL ) ? ( ( exists != NULL ) ? exists->getID() : -1 ) : -1;
      }
   }

   Item *newitem = getItem(itemIndex);
   if(newitem)
   {
      newitem->setValue(szValue);
      if(_isDirInMainDotCsPath(szValue))
      {
         newitem->setNormalImage( Icon_FolderClosed );
         newitem->setExpandedImage( Icon_Folder );
         newitem->setVirtualParent(true);
         newitem->setExpanded(false);
      }
      else
      {
         newitem->setNormalImage( Icon_Doc );
      }
   }
   // since we're only dealing with volumes and directories, all end nodes will be virtual parents
   // so if we are at the bottom of the rabbit hole, set the item to be a virtual parent
//   Item* item = getItem( itemIndex );
//   if(item)
//   {
//      item->setExpanded(false);
//      if(parent && _isDirInMainDotCsPath(item->getValue()) && Platform::hasSubDirectory(item->getValue()))
//         item->setVirtualParent(true);
//   }
//   if( delim )
//   {
//      if( ( dStrcmp( delim, "" ) == 0 ) && item )
//      {
//         item->setExpanded( false );
//         if( parent && _hasChildren( item->getValue() ) )
//            item->setVirtualParent( true );
//      }
//   }
//   else
//   {
//      if( item )
//      {
//         item->setExpanded( false );
//         if( parent &&  _hasChildren( item->getValue() ) )
//            item->setVirtualParent( true );
//      }
//   }
   
   // Down the rabbit hole we go
   recurseInsert( getItem( itemIndex ), delim );

}


StringTableEntry GuiFileTreeCtrl::getUnit(const char *string, U32 index, const char *set)
{
   U32 sz;
   while(index--)
   {
      if(!*string)
         return "";
      sz = dStrcspn(string, set);
      if (string[sz] == 0)
         return "";
      string += (sz + 1);
   }
   sz = dStrcspn(string, set);
   if (sz == 0)
      return "";
   char *ret = Con::getReturnBuffer(sz+1);
   dStrncpy(ret, sz+1, string, sz);
   ret[sz] = '\0';
   return ret;
}
StringTableEntry GuiFileTreeCtrl::getUnits(const char *string, S32 startIndex, S32 endIndex, const char *set)
{
   S32 sz;
   S32 index = startIndex;
   while(index--)
   {
      if(!*string)
         return "";
      sz = dStrcspn(string, set);
      if (string[sz] == 0)
         return "";
      string += (sz + 1);
   }
   const char *startString = string;
   while(startIndex <= endIndex--)
   {
      sz = dStrcspn(string, set);
      string += sz;
      if (*string == 0)
         break;
      string++;
   }
   if(!*string)
      string++;
   U32 totalSize = (U32(string - startString));
   char *ret = Con::getReturnBuffer(totalSize);
   dStrncpy(ret, totalSize, startString, totalSize - 1);
   ret[totalSize-1] = '\0';
   return ret;
}

U32 GuiFileTreeCtrl::getUnitCount(const char *string, const char *set)
{
   U32 count = 0;
   U8 last = 0;
   while(*string)
   {
      last = *string++;

      for(U32 i =0; set[i]; i++)
      {
         if(last == set[i])
         {
            count++;
            last = 0;
            break;
         }
      }
   }
   if(last)
      count++;
   return count;
}


ConsoleMethod( GuiFileTreeCtrl, getSelectedPath, const char*, 2,2, "getSelectedPath() - returns the currently selected path in the tree")
{
   return object->getSelectedPath();
}

StringTableEntry GuiFileTreeCtrl::getSelectedPath()
{
   return mSelPath;
}

ConsoleMethod( GuiFileTreeCtrl, setSelectedPath, bool, 3, 3, "setSelectedPath(path) - expands the tree to the specified path")
{
   return object->setSelectedPath( argv[2] );
}

bool GuiFileTreeCtrl::setSelectedPath( StringTableEntry path )
{
   if( !path )
      return false;

   // Since we only list one deep on paths, we need to add the path to the tree just incase it isn't already indexed in the tree
   // or else we wouldn't be able to select a path we hadn't previously browsed to. :)
   if( _isDirInMainDotCsPath( path ) )
      addPathToTree( path );

   // see if we have a child that matches what we want
   for(U32 i = 0; i < mItems.size(); i++)
   {
      if( dStricmp( mItems[i]->getValue(), path ) == 0 )
      {
         Item* item = mItems[i];
         AssertFatal(item,"GuiFileTreeCtrl::setSelectedPath - Item Index Bad, Fatal Mistake!!!");
         item->setExpanded( true );
         clearSelection();
         setItemSelected( item->getID(), true );
         // make sure all of it's parents are expanded
         S32 parent = getParentItem( item->getID() );
         while( parent != 0 )
         {
            setItemExpanded( parent, true );
            parent = getParentItem( parent );
         }
         // Rebuild our tree just incase we've oops'd
         buildVisibleTree();
         scrollVisible( item );
      }
   }
   return false;
}

void GuiFileTreeCtrl::initPersistFields()
{
   Parent::initPersistFields();
}