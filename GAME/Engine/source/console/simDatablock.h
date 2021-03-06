//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SIMDATABLOCK_H_
#define _SIMDATABLOCK_H_

// Forward Refs
class BitStream;

/// Root DataBlock class.
///
/// @section SimDataBlock_intro Introduction
///
/// Another powerful aspect of PowerEngine's networking is the datablock. Datablocks
/// are used to provide relatively static information about entities; for instance,
/// what model a weapon should use to display itself, or how heavy a player class is.
///
/// This gives significant gains in network efficiency, because it means that all
/// the datablocks on a server can be transferred over the network at client
/// connect time, instead of being intertwined with the update code for NetObjects.
///
/// This makes the network code much simpler overall, as one-time initialization
/// code is segregated from the standard object update code, as well as providing
/// several powerful features, which we will discuss momentarily.
///
/// @section SimDataBlock_preload preload() and File Downloading
///
/// Because datablocks are sent over the wire, using SimDataBlockEvent, before
/// gameplay starts in earnest, we gain in several areas. First, we don't have to
/// try to keep up with the game state while working with incomplete information.
/// Second, we can provide the user with a nice loading screen, instead of the more
/// traditional "Connecting..." message. Finally, and most usefully, we can request
/// missing files from the server as we become aware of them, since we are under
/// no obligation to render anything for the user.
///
/// The mechanism for this is fairly basic. After a datablock is unpacked, the
/// preload() method is called. If it returns false and sets an error, then the
/// network code checks to see if a file (or files) failed to be located by the
/// ResManager; if so, then it requests those files from the server. If preload
/// returns true, then the datablock is considered loaded. If preload returns
/// false and sets no error, then the connection is aborted.
///
/// Once the file(s) is downloaded, the datablock's preload() method is called again.
/// If it fails with the same error, the connection is aborted. If a new error is
/// returned, then the download-retry process is repeated until the preload works.
///
/// @section SimDataBlock_guide Guide To Datablock Code
///
/// To make a datablock subclass, you need to extend three functions:
///      - preload()
///      - packData()
///      - unpackData()
///
/// packData() and unpackData() simply read or write data to a network stream. If you
/// add any fields, you need to add appropriate calls to read or write. Make sure that
/// the order of reads and writes is the same in both functions. Make sure to call
/// the Parent's version of these methods in every subclass.
///
/// preload() is a bit more complex; it is responsible for taking the raw data read by
/// unpackData() and processing it into a form useful by the datablock's owning object. For
/// instance, the Player class' datablock, PlayerData, gets handles to commonly used
/// nodes in the player model, as well as resolving handles to textures and other
/// resources. <b>Any</b> code which loads files or performs other actions beyond simply
/// reading the data from the packet, such as validation, must reside in preload().
///
/// To write your own preload() methods, see any of the existing methods in the codebase; for instance,
/// PlayerData::preload() is an excellent example of error-reporting, data validation, and so forth.
///
/// @note A useful trick, which is used in several places in the engine, is that of temporarily
///       storing SimObjectIds in the variable which will eventually hold the "real" handle. ShapeImage
///       uses this trick in several pllaces; so do the vehicle classes. See GameBaseData for more on
///       using this trick.
///
/// @see GameBaseData for some more information on the datablocks used throughout
///      most of the engine.
/// @see http://hosted.tribalwar.com/t2faq/datablocks.shtml for an excellent
///      explanation of the basics of datablocks from script. Note that these comments
///      mostly apply to GameBaseData and its children.
/// @nosubgrouping
class SimDataBlock: public SimObject
{
   typedef SimObject Parent;
public:

   SimDataBlock();
   DECLARE_CONOBJECT(SimDataBlock);

   /// @name Datablock Internals
   /// @{

protected:
   S32  modifiedKey;

public:
   static SimObjectId sNextObjectId;
   static S32         sNextModifiedKey;

   /// Assign a new modified key.
   ///
   /// Datablocks are assigned a modified key which is updated every time
   /// a static field of the datablock is changed. These are gotten from
   /// a global store.
   static S32 getNextModifiedKey() { return sNextModifiedKey; }

   /// Get the modified key for this particular datablock.
   S32 getModifiedKey() const { return modifiedKey; }

   bool onAdd();
   virtual void onStaticModified(const char* slotName, const char*newValue = NULL);
   //void setLastError(const char*);
   void assignId();

   /// @}

   /// @name Datablock Interface
   /// @{

   ///
   virtual void packData(BitStream* stream);
   virtual void unpackData(BitStream* stream);

   /// Called to prepare the datablock for use, after it has been unpacked.
   ///
   /// @param  server      Set if we're running on the server (and therefore don't need to load
   ///                     things like textures or sounds).
   /// @param  errorBuffer If an error occurs in loading, this is set to a short string describing
   ///                     the error.
   /// @returns True if all went well; false if something failed.
   ///
   /// @see @ref SimDataBlock_preload
   virtual bool preload(bool server, char errorBuffer[256]);
   /// @}
};

//---------------------------------------------------------------------------

class SimDataBlockGroup : public SimGroup
{
private:
   S32 mLastModifiedKey;

public:
   static S32 QSORT_CALLBACK compareModifiedKey(const void* a,const void* b);
   void sort();
   SimDataBlockGroup();
};

#endif // _SIMDATABLOCK_H_
