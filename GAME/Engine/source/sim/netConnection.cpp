//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "core/dnet.h"
#include "console/simBase.h"
#include "sim/netConnection.h"
#include "core/bitStream.h"
#include "core/fileStream.h"
#include "core/resManager.h"
#ifndef POWER_TGB_ONLY
#  include "sceneGraph/pathManager.h"
#endif
#include "console/consoleTypes.h"
#include "sim/netInterface.h"
#include <stdarg.h>
#include "ts/tsMesh.h"

S32 gNetBitsSent = 0;
extern S32 gNetBitsReceived;
U32 gGhostUpdates = 0;

// 50秒钟超时
enum NetConnectionConstants {
   PingTimeout = 30000, ///< milliseconds
   DefaultPingRetryCount = 10,
};

SimObjectPtr<NetConnection> NetConnection::mServerConnection;
SimObjectPtr<NetConnection> NetConnection::mLocalClientConnection;

//----------------------------------------------------------------------
/// ConnectionMessageEvent
///
/// This event is used inside by the connection and subclasses to message
/// itself when sequencing events occur.  Right now, the message event
/// only uses 6 bits to transmit the message, so
class ConnectionMessageEvent : public NetEvent
{
   U32 sequence;
   U32 message;
   U32 ghostCount;
public:
   ConnectionMessageEvent(U32 msg=0, U32 seq=0, U32 gc=0)
      { message = msg; sequence = seq; ghostCount = gc;}
   void pack(NetConnection *, BitStream *bstream)
   {
      bstream->write(sequence);
      bstream->writeInt(message, 3);
      bstream->writeInt(ghostCount, NetConnection::GhostIdBitSize + 1);
   }
   void write(NetConnection *, BitStream *bstream)
   {
      bstream->write(sequence);
      bstream->writeInt(message, 3);
      bstream->writeInt(ghostCount, NetConnection::GhostIdBitSize + 1);
   }
   void unpack(NetConnection *, BitStream *bstream)
   {
      bstream->read(&sequence);
      message = bstream->readInt(3);
      ghostCount = bstream->readInt(NetConnection::GhostIdBitSize + 1);
   }
   void process(NetConnection *ps)
   {
      ps->handleConnectionMessage(message, sequence, ghostCount);
   }
   DECLARE_CONOBJECT(ConnectionMessageEvent);
};

IMPLEMENT_CO_NETEVENT_V1(ConnectionMessageEvent);

void NetConnection::sendConnectionMessage(U32 message, U32 sequence, U32 ghostCount)
{
   postNetEvent(new ConnectionMessageEvent(message, sequence, ghostCount));
}

//--------------------------------------------------------------------
IMPLEMENT_CONOBJECT(NetConnection);

NetConnection* NetConnection::mConnectionList = NULL;
NetConnection* NetConnection::mHashTable[NetConnection::HashTableSize] = { NULL, };

bool NetConnection::mFilesWereDownloaded = false;

static inline U32 HashNetAddress(const NetAddress *addr)
{
   return *((U32 *)addr->netNum) % NetConnection::HashTableSize;
}

NetConnection *NetConnection::lookup(const NetAddress *addr)
{
   U32 hashIndex = HashNetAddress(addr);
   for(NetConnection *walk = mHashTable[hashIndex]; walk; walk = walk->mNextTableHash)
      if(Net::compareAddresses(addr, walk->getNetAddress()) && walk->mSocket == 0 )
         return walk;
   return NULL;
}

NetConnection *NetConnection::lookup(const NetAddress *addr, int udpS )
{
	U32 hashIndex = HashNetAddress(addr);
	for(NetConnection *walk = mHashTable[hashIndex]; walk; walk = walk->mNextTableHash)
		if(Net::compareAddresses(addr, walk->getNetAddress()) && walk->mSocket == udpS )
			return walk;
	return NULL;
}

void NetConnection::netAddressTableInsert()
{
   U32 hashIndex = HashNetAddress(&mNetAddress);
   mNextTableHash = mHashTable[hashIndex];
   mHashTable[hashIndex] = this;
}

void NetConnection::netAddressTableRemove()
{
   U32 hashIndex = HashNetAddress(&mNetAddress);
   NetConnection **walk = &mHashTable[hashIndex];
   while(*walk)
   {
      if(*walk == this)
      {
         *walk = mNextTableHash;
         mNextTableHash = NULL;
         return;
      }
      walk = &((*walk)->mNextTableHash);
   }
}

void NetConnection::setNetAddress(const NetAddress *addr)
{
   mNetAddress = *addr;
}

const NetAddress *NetConnection::getNetAddress()
{
   return &mNetAddress;
}

void NetConnection::setSequence(U32 sequence)
{
   mConnectSequence = sequence;
}

U32 NetConnection::getSequence()
{
   return mConnectSequence;
}

static U32 gPacketRateToServer = 10;
static U32 gPacketRateToClient = 10;
static U32 gPacketSize = Net::MaxPacketDataSize;
U32 gMaxPackOnceSize = 0;

void NetConnection::consoleInit()
{
}

void NetConnection::setSendingEvents(bool sending)
{
   AssertFatal(!mEstablished, "Error, cannot change event behavior after a connection has been established.");
   mSendingEvents = sending;
}

void NetConnection::setTranslatesStrings(bool xl)
{
   AssertFatal(!mEstablished, "Error, cannot change event behavior after a connection has been established.");
   mTranslateStrings = xl;
   if(mTranslateStrings)
      mStringTable = new ConnectionStringTable(this);
}

void NetConnection::setNetClassGroup(U32 grp)
{
   AssertFatal(!mEstablished, "Error, cannot change net class group after a connection has been established.");
   mNetClassGroup = grp;
}

NetConnection::NetConnection()
{
   mTranslateStrings = false;
   mConnectSequence = 0;

   mSocket = 0;

   mStringTable = NULL;
   mSendingEvents = true;
   mNetClassGroup = NetClassGroupGame;
   AssertFatal(mNetClassGroup >= NetClassGroupGame && mNetClassGroup < NetClassGroupsCount,
            "Invalid net event class type.");

   mSimulatedPing = 0;
   mSimulatedPacketLoss = 0;
#ifdef POWER_DEBUG_NET
   mLogging = false;
#endif
   mEstablished = false;
   mLastUpdateTime = 0;
   mRoundTripTime = 0;
   mPacketLoss = 0;
   mNextTableHash = NULL;
   mSendDelayCredit = 0;
   mConnectionState = NotConnected;

   mCurrentDownloadingFile = NULL;
   mCurrentFileBuffer = NULL;

   mNextConnection = NULL;
   mPrevConnection = NULL;

   mNotifyQueueHead = NULL;
   mNotifyQueueTail = NULL;

   // event management data:

   mNotifyEventList = NULL;
   mSendEventQueueHead = NULL;
   mSendEventQueueTail = NULL;
   mUnorderedSendEventQueueHead = NULL;
   mUnorderedSendEventQueueTail = NULL;
   mWaitSeqEvents = NULL;

   mNextSendEventSeq = FirstValidSendEventSeq;
   mNextRecvEventSeq = FirstValidSendEventSeq;
   mLastAckedEventSeq = -1;

   // ghost management data:

   mScopeObject = NULL;
   mGhostingSequence = 0;
   mGhosting = false;
   mScoping = false;
   mGhostArray = NULL;
   mGhostRefs = NULL;
   mGhostLookupTable = NULL;
   mLocalGhosts = NULL;

   mGhostsActive = 0;

   mMissionPathsSent = false;
   mDemoWriteStream = NULL;
   mDemoReadStream = NULL;

   mPingSendCount = 0;
   mPingRetryCount = DefaultPingRetryCount;
   mLastPingSendTime = Platform::getVirtualMilliseconds();

   mCurrentDownloadingFile = NULL;
   mCurrentFileBuffer = NULL;
   mCurrentFileBufferSize = 0;
   mCurrentFileBufferOffset = 0;
   mNumDownloadedFiles = 0;

   mLastFileErrorBuffer[0] = 0;
}

NetConnection::~NetConnection()
{
   AssertFatal(mNotifyQueueHead == NULL, "Uncleared notifies remain.");
   netAddressTableRemove();

   dFree(mCurrentFileBuffer);
   if(mCurrentDownloadingFile)
      ResourceManager->closeStream(mCurrentDownloadingFile);

   delete[] mLocalGhosts;
   delete[] mGhostLookupTable;
   delete[] mGhostRefs;
   delete[] mGhostArray;
   delete mStringTable;
   if(mDemoWriteStream)
      delete mDemoWriteStream;
   if(mDemoReadStream)
      ResourceManager->closeStream(mDemoReadStream);
}

NetConnection::PacketNotify::PacketNotify()
{
   sendTime = 0;
   eventList = 0;
   ghostList = 0;
}

void NetConnection::SetSocket( int socket )
{
	mSocket = socket;
	Net::hashSocketMap.insert( socket );
}

int NetConnection::GetSocket()
{
	return mSocket;
}

bool NetConnection::checkTimeout(U32 time)
{
   if(!isNetworkConnection())
      return false;

   if(time > mLastPingSendTime + PingTimeout)
   {
      if(mPingSendCount >= mPingRetryCount)
         return true;
      mLastPingSendTime = time;
      mPingSendCount++;
      sendPingPacket();
   }
   return false;
}

void NetConnection::keepAlive()
{
   mLastPingSendTime = Platform::getVirtualMilliseconds();
   mPingSendCount = 0;
}

void NetConnection::handleConnectionEstablished()
{
}

bool NetConnection::isRobotConnection()
{
	return Net::hashSocketMap.find( mSocket ) != Net::hashSocketMap.end();
}
//--------------------------------------------------------------------------
#ifndef POWER_TGB_ONLY
ConsoleMethod(NetConnection,transmitPaths,void,2,2,"conn.transmitPaths();")
{
   argc; argv;

   gServerPathManager->transmitPaths(object);
   object->setMissionPathsSent(true);
}

ConsoleMethod(NetConnection,clearPaths,void,2,2,"conn.clearPaths();")
{
   argc; argv;
   object->setMissionPathsSent(false);
}
#endif

ConsoleMethod(NetConnection,getAddress,const char *,2,2,"Returns the address we're connected to.")
{
   argc; argv;
   if(object->isLocalConnection())
      return "local";
   char *buffer = Con::getReturnBuffer(256);
   Net::addressToString(object->getNetAddress(), buffer);
   return buffer;
}

ConsoleMethod(NetConnection,setSimulatedNetParams,void,4, 4,"(float packetLoss, int delay)")
{
   argc;
   object->setSimulatedNetParams(dAtof(argv[2]), dAtoi(argv[3]));
}

ConsoleMethod( NetConnection, getPing, S32, 2, 2, "conn.getPing()" )
{
   argc; argv;
   return( S32( object->getRoundTripTime() ) );
}

ConsoleMethod( NetConnection, getPacketLoss, S32, 2, 2, "conn.getPacketLoss()" )
{
   argc; argv;
   return( S32( 100 * object->getPacketLoss() ) );
}

#ifdef POWER_DEBUG_NET

ConsoleMethod( NetConnection, setLogging, void, 3, 3, "conn.setLogging(bool)")
{
   argc;
   object->setLogging(dAtob(argv[2]));
}

#endif

//--------------------------------------------------------------------

void NetConnection::setEstablished()
{
   AssertFatal(!mEstablished, "NetConnection::setEstablished - Error, this NetConnection has already been established.");

   mEstablished = true;
   mNextConnection = mConnectionList;
   if(mConnectionList)
      mConnectionList->mPrevConnection = this;
   mConnectionList = this;

   if(isNetworkConnection())
      netAddressTableInsert();

}

void NetConnection::onRemove()
{
   // delete any ghosts that may exist for this connection, but aren't added
   while(mGhostAlwaysSaveList.size())
   {
      delete mGhostAlwaysSaveList[0].ghost;
      mGhostAlwaysSaveList.pop_front();
   }
   if(mNextConnection)
      mNextConnection->mPrevConnection = mPrevConnection;
   if(mPrevConnection)
      mPrevConnection->mNextConnection = mNextConnection;
   if(mConnectionList == this)
      mConnectionList = mNextConnection;
   while(mNotifyQueueHead)
      handleNotify(false);

   ghostOnRemove();
   eventOnRemove();

   Parent::onRemove();
}

char NetConnection::mErrorBuffer[256]="";

void NetConnection::setLastError(const char *fmt, ...)
{
   va_list argptr;
   va_start(argptr, fmt);
   dVsprintf(mErrorBuffer, sizeof(mErrorBuffer), fmt, argptr);
   va_end(argptr);

#ifdef POWER_DEBUG_NET
   // setLastErrors assert in net_debug builds
   AssertFatal(false, mErrorBuffer);
#endif

}

//--------------------------------------------------------------------

void NetConnection::handleNotify(bool recvd)
{
//   Con::printf("NET  %d: NOTIFY - %d %s", getId(), gPacketId, recvd ? "RECVD" : "DROPPED");

   PacketNotify *note = mNotifyQueueHead;
   AssertFatal(note != NULL, "Error: got a notify with a null notify head.");
   mNotifyQueueHead = mNotifyQueueHead->nextPacket;

   if(recvd) 
   {
      // Running average of roundTrip time
      U32 curTime = Platform::getVirtualMilliseconds();
      mRoundTripTime = (mRoundTripTime + (curTime - note->sendTime)) * 0.5;
      packetReceived(note);
   }
   else
      packetDropped(note);

   delete note;
}

void NetConnection::processRawPacket(BitStream *bstream)
{
   if(mDemoWriteStream)
      recordBlock(BlockTypePacket, bstream->getReadByteSize(), bstream->getBuffer());

   ConnectionProtocol::processRawPacket(bstream);
}

void NetConnection::handlePacket(BitStream *bstream)
{
//   Con::printf("NET  %d: RECV - %d", getId(), mLastSeqRecvd);
   // clear out any errors

   mErrorBuffer[0] = 0;
   readPacket(bstream);

   if(mErrorBuffer[0])
      connectionError(mErrorBuffer);
}

void NetConnection::connectionError(const char *errorString)
{
   errorString;
}

//--------------------------------------------------------------------

NetConnection::PacketNotify *NetConnection::allocNotify()
{
   return new PacketNotify;
}

/// Used when simulating lag.
///
/// We post this SimEvent when we want to send a packet; it delays for a bit, then
/// sends the actual packet.
class NetDelayEvent : public SimEvent
{
   U8 buffer[Net::MaxPacketBuffSize];
   BitStream stream;
public:
   NetDelayEvent(BitStream *inStream) : stream(NULL, 0)
   {
      dMemcpy(buffer, inStream->getBuffer(), inStream->getPosition());
      stream.setBuffer(buffer, inStream->getPosition());
      stream.setPosition(inStream->getPosition());
   }
   void process(SimObject *object)
   {
      ((NetConnection *) object)->sendPacket(&stream);
   }
};

void NetConnection::checkPacketSend(bool force)
{
   U32 curTime = Platform::getVirtualMilliseconds();

#ifdef NTJ_EDITOR
   U32 delay = 1024/128;
#endif
#ifdef NTJ_SERVER
   U32 delay = 1024/gPacketRateToClient;
#endif
#ifdef NTJ_CLIENT
   U32 delay = 1024/gPacketRateToServer;
#endif

   if(!force)
   {
      if(curTime < mLastUpdateTime + delay - mSendDelayCredit)
         return;

      mSendDelayCredit = curTime - (mLastUpdateTime + delay - mSendDelayCredit);
      if(mSendDelayCredit > 1000)
         mSendDelayCredit = 1000;

      if(mDemoWriteStream)
         recordBlock(BlockTypeSendPacket, 0, 0);
   }
   if(windowFull())
      return;

   BitStream *stream = BitStream::getPacketStream();  //Ray: 固定方式，禁止协商
   buildSendPacketHeader(stream);

   mLastUpdateTime = curTime;

   PacketNotify *note = allocNotify();
   if(!mNotifyQueueHead)
      mNotifyQueueHead = note;
   else
      mNotifyQueueTail->nextPacket = note;
   mNotifyQueueTail = note;
   note->nextPacket = NULL;
   note->sendTime = curTime;

 #ifdef POWER_DEBUG_NET
   U32 start = stream->getCurPos();
#endif

   DEBUG_LOG(("PKLOG %d START", getId()) );
   writePacket(stream, note);
   DEBUG_LOG(("PKLOG %d END - %d", getId(), stream->getCurPos() - start) );
   if(mSimulatedPacketLoss && Platform::getRandom() < mSimulatedPacketLoss)
   {
      //Con::printf("NET  %d: SENDDROP - %d", getId(), mLastSendSeq);
      return;
   }
   if(mSimulatedPing)
   {
      Sim::postEvent(getId(), new NetDelayEvent(stream), Sim::getCurrentTime() + mSimulatedPing);
      return;
   }
   sendPacket(stream);
}

Net::Error NetConnection::sendPacket(BitStream *stream)
{
   //Con::printf("NET  %d: SEND - %d", getId(), mLastSendSeq);
   // do nothing on send if this is a demo replay.
   if(mDemoReadStream)
      return Net::NoError;

   gNetBitsSent = stream->getStreamSize();

   if(isLocalConnection())
   {
      // short circuit connection to the other side.
      // handle the packet, then force a notify.
      stream->setBuffer(stream->getBuffer(), stream->getPosition(), stream->getPosition());
      mRemoteConnection->processRawPacket(stream);

      return Net::NoError;
   }
   else
   {
	  if( mSocket == 0 )
		return Net::sendto(getNetAddress(), stream->getBuffer(), stream->getPosition());
	  else
		return Net::sendtoEx(getNetAddress(), mSocket, stream->getBuffer(), stream->getPosition());
   }
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------

// these are the virtual function defs for Connection -
// if your subclass has additional data to read / write / notify, add it in these functions.

void NetConnection::readPacket(BitStream *bstream)
{
   eventReadPacket(bstream);
   ghostReadPacket(bstream);
}

void NetConnection::writePacket(BitStream *bstream, PacketNotify *note)
{
   eventWritePacket(bstream, note);
   ghostWritePacket(bstream, note);
}

void NetConnection::packetReceived(PacketNotify *note)
{
   eventPacketReceived(note);
   ghostPacketReceived(note);
}

void NetConnection::packetDropped(PacketNotify *note)
{
   eventPacketDropped(note);
   ghostPacketDropped(note);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------

void NetConnection::writeDemoStartBlock(ResizeBitStream* stream)
{
   ConnectionProtocol::writeDemoStartBlock(stream);

   stream->write(mRoundTripTime);
   stream->write(mPacketLoss);
#ifndef POWER_TGB_ONLY
   // Write all the current paths to the stream...
   gClientPathManager->dumpState(stream);
#endif
   stream->validate();
   mStringTable->writeDemoStartBlock(stream);

   U32 start = 0;
   PacketNotify *note = mNotifyQueueHead;
   while(note)
   {
      start++;
      note = note->nextPacket;
   }
   stream->write(start);

   eventWriteStartBlock(stream);
   ghostWriteStartBlock(stream);
}

bool NetConnection::readDemoStartBlock(BitStream* stream)
{
   ConnectionProtocol::readDemoStartBlock(stream);

   stream->read(&mRoundTripTime);
   stream->read(&mPacketLoss);

#ifndef POWER_TGB_ONLY
   // Read
   gClientPathManager->readState(stream);
#endif

   mStringTable->readDemoStartBlock(stream);
   U32 pos;
   stream->read(&pos); // notify count
   for(U32 i = 0; i < pos; i++)
   {
      PacketNotify *note = allocNotify();
      note->nextPacket = NULL;
      if(!mNotifyQueueHead)
         mNotifyQueueHead = note;
      else
         mNotifyQueueTail->nextPacket = note;
      mNotifyQueueTail = note;
   }
   eventReadStartBlock(stream);
   ghostReadStartBlock(stream);
   return true;
}

bool NetConnection::startDemoRecord(const char *fileName)
{
   FileStream *fs = new FileStream;

   if(!ResourceManager->openFileForWrite(*fs, fileName))
   {
      delete fs;
      return false;
   }

   mDemoWriteStream = fs;
   mDemoWriteStream->write(mProtocolVersion);
   ResizeBitStream bs;

   // then write out the start block
   writeDemoStartBlock(&bs);
   U32 size = bs.getPosition() + 1;
   mDemoWriteStream->write(size);
   mDemoWriteStream->write(size, bs.getBuffer());
   return true;
}

bool NetConnection::replayDemoRecord(const char *fileName)
{
   Stream *fs = ResourceManager->openStream(fileName);
   if(!fs)
      return false;

   mDemoReadStream = fs;
   mDemoReadStream->read(&mProtocolVersion);
   U32 size;
   mDemoReadStream->read(&size);
   U8 *block = new U8[size];
   mDemoReadStream->read(size, block);
   BitStream bs(block, size);

   bool res = readDemoStartBlock(&bs);
   delete[] block;
   if(!res)
      return false;

   // prep for first block read
   // type/size stored in U16: [type:4][size:12]
   U16 typeSize;
   mDemoReadStream->read(&typeSize);

   mDemoNextBlockType = typeSize >> 12;
   mDemoNextBlockSize = typeSize & 0xFFF;

   if(mDemoReadStream->getStatus() != Stream::Ok)
      return false;
   return true;
}

void NetConnection::stopRecording()
{
   if(mDemoWriteStream)
   {
      delete mDemoWriteStream;
      mDemoWriteStream = NULL;
   }
}

void NetConnection::recordBlock(U32 type, U32 size, void *data)
{
   AssertFatal(type < MaxNumBlockTypes, "NetConnection::recordBlock: invalid type");
   AssertFatal(size < MaxBlockSize, "NetConnection::recordBlock: invalid size");
   if((type >= MaxNumBlockTypes) || (size >= MaxBlockSize))
      return;

   if(mDemoWriteStream)
   {
      // store type/size in U16: [type:4][size:12]
      U16 typeSize = (type << 12) | size;
      mDemoWriteStream->write(typeSize);
      if(size)
         mDemoWriteStream->write(size, data);
   }
}

void NetConnection::handleRecordedBlock(U32 type, U32 size, void *data)
{
   switch(type)
   {
      case BlockTypePacket: {
         BitStream bs(data, size);
         processRawPacket(&bs);
         break;
      }
      case BlockTypeSendPacket:
         checkPacketSend(true);
         break;
   }
}

void NetConnection::demoPlaybackComplete()
{
}

void NetConnection::stopDemoPlayback()
{
   demoPlaybackComplete();
   deleteObject();
}

bool NetConnection::processNextBlock()
{
   U8 buffer[Net::MaxPacketBuffSize];

   // read in and handle
   if(mDemoReadStream->read(mDemoNextBlockSize, buffer))
      handleRecordedBlock(mDemoNextBlockType, mDemoNextBlockSize, buffer);

   U16 typeSize;
   mDemoReadStream->read(&typeSize);

   mDemoNextBlockType = typeSize >> 12;
   mDemoNextBlockSize = typeSize & 0xFFF;

   if(mDemoReadStream->getStatus() != Stream::Ok)
   {
      stopDemoPlayback();
      return false;
   }
   return true;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------

// some handy string functions for compressing strings over a connection:
enum NetStringConstants
{
   NullString = 0,
   CString,
   TagString,
   Integer
};

void NetConnection::validateSendString(const char *str)
{
   if(U8(*str) == StringTagPrefixByte)
   {
      NetStringHandle strHandle(dAtoi(str + 1));
      checkString(strHandle);
   }
}

void NetConnection::packString(BitStream *stream, const char *str)
{
   char buf[16];
   if(!*str)
   {
      stream->writeInt(NullString, 2);
      return;
   }
   if(U8(str[0]) == StringTagPrefixByte)
   {
      stream->writeInt(TagString, 2);
      stream->writeInt(dAtoi(str + 1), ConnectionStringTable::EntryBitSize);
      return;
   }
   if(str[0] == '-' || (str[0] >= '0' && str[0] <= '9'))
   {
      S32 num = dAtoi(str);
      dSprintf(buf, sizeof(buf), "%d", num);
      if(!dStrcmp(buf, str))
      {
         stream->writeInt(Integer, 2);
         if(stream->writeFlag(num < 0))
            num = -num;
         if(stream->writeFlag(num < 128))
         {
            stream->writeInt(num, 7);
            return;
         }
         if(stream->writeFlag(num < 32768))
         {
            stream->writeInt(num, 15);
            return;
         }
         else
         {
            stream->writeInt(num, 31);
            return;
         }
      }
   }
   stream->writeInt(CString, 2);
   S32   len = strlen( str ) + 1;
   // 1024 is the max size of the buffer in unpackString() and RemoteCommandEvent::mBuf
   stream->writeRangedS32( len, 0, 1024 );
   stream->_write(len, str);
}

void NetConnection::unpackString(BitStream *stream, char readBuffer[1024])
{
   U32 code = stream->readInt(2);
   switch(code)
   {
      case NullString:
         readBuffer[0] = 0;
         return;
      case CString:
         {
            // 1024 is the max size of the buffer in unpackString() and RemoteCommandEvent::mBuf
            S32   len = stream->readRangedS32( 0, 1024 );
            stream->_read(len, readBuffer);
            return;
         }
      case TagString:
         U32 tag;
         tag = stream->readInt(ConnectionStringTable::EntryBitSize);
         readBuffer[0] = StringTagPrefixByte;
         dSprintf(readBuffer+1, 1023, "%d", tag);
         return;
      case Integer:
         bool neg;
         neg = stream->readFlag();
         S32 num;
         if(stream->readFlag())
            num = stream->readInt(7);
         else if(stream->readFlag())
            num = stream->readInt(15);
         else
            num = stream->readInt(31);
         if(neg)
            num = -num;
         dSprintf(readBuffer, 1024, "%d", num);
   }
}

void NetConnection::packNetStringHandleU(BitStream *stream, NetStringHandle &h)
{
   if(stream->writeFlag(h.isValidString() ))
   {
      bool isReceived;
      U32 netIndex = checkString(h, &isReceived);
      if(stream->writeFlag(isReceived))
         stream->writeInt(netIndex, ConnectionStringTable::EntryBitSize);
      else
         stream->writeString(h.getString());
   }
}

NetStringHandle NetConnection::unpackNetStringHandleU(BitStream *stream)
{
   NetStringHandle ret;
   if(stream->readFlag())
   {
      if(stream->readFlag())
         ret = mStringTable->lookupString(stream->readInt(ConnectionStringTable::EntryBitSize));
      else
      {
         char buf[256];
         stream->readString(buf,256);
         ret = NetStringHandle(buf);
      }
   }
   return ret;
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void NetConnection::setAddressDigest(U32 digest[4])
{
   mAddressDigest[0] = digest[0];
   mAddressDigest[1] = digest[1];
   mAddressDigest[2] = digest[2];
   mAddressDigest[3] = digest[3];
}

void NetConnection::getAddressDigest(U32 digest[4])
{
   digest[0] = mAddressDigest[0];
   digest[1] = mAddressDigest[1];
   digest[2] = mAddressDigest[2];
   digest[3] = mAddressDigest[3];
}

bool NetConnection::canRemoteCreate()
{
   return false;
}

void NetConnection::onTimedOut()
{

}

void NetConnection::connect(const NetAddress *address)
{
   mNetAddress = *address;
   GNet->startConnection(this);
}

void NetConnection::onConnectTimedOut()
{

}

void NetConnection::sendDisconnectPacket(const char *reason)
{
   GNet->sendDisconnectPacket(this, reason);
}

void NetConnection::onDisconnect(const char *reason)
{
   reason;
}

void NetConnection::onConnectionRejected(const char *reason)
{
}

void NetConnection::onConnectionEstablished(bool isInitiator)
{

}

void NetConnection::handleStartupError(const char *errorString)
{

}

void NetConnection::writeConnectRequest(BitStream *stream)
{
   stream->write(mNetClassGroup);
   stream->write(U32(AbstractClassRep::getClassCRC(mNetClassGroup)));
}

bool NetConnection::readConnectRequest(BitStream *stream, const char **errorString)
{
   U32 classGroup, classCRC;
   stream->read(&classGroup);
   stream->read(&classCRC);

   if(classGroup == mNetClassGroup && classCRC == AbstractClassRep::getClassCRC(mNetClassGroup))
      return true;

   *errorString = "CHR_INVALID";
   return false;
}

void NetConnection::writeConnectAccept(BitStream *stream)
{
   stream;
}

bool NetConnection::readConnectAccept(BitStream *stream, const char **errorString)
{
   stream;
   errorString;
   return true;
}

ConsoleMethod(NetConnection, resolveGhostID, S32, 3, 3, "( S32 ghostID ) Convert a ghost id from this connection to a real id.")
{
   S32 gID = dAtoi(argv[2]);

   // Safety check
   if(gID < 0 || gID > NetConnection::MaxGhostCount) return 0;

   NetObject *foo = object->resolveGhost(gID);

   if(foo)
      return foo->getId();
   else
      return 0;
}

ConsoleMethod(NetConnection, resolveObjectFromGhostIndex, S32, 3, 3, "( S32 ghostIdx) Convert a ghost index from this connection to a real id.")
{
   S32 gID = dAtoi(argv[2]);

   // Safety check
   if(gID < 0 || gID > NetConnection::MaxGhostCount) return 0;

   NetObject *foo = object->resolveObjectFromGhostIndex(gID);

   if(foo)
      return foo->getId();
   else
      return 0;
}

ConsoleMethod(NetConnection, getGhostID, S32, 3, 3, "( S32 realID ) Convert a real id to the ghost id for this connection.")
{
   NetObject * foo;

   if(Sim::findObject(argv[2], foo))
   {
      return object->getGhostIndex(foo);
   }
   else
   {
      Con::errorf("NetConnection::serverToGhostID - could not find specified object");
      return -1;
   }
}

ConsoleMethod(NetConnection, connect, void, 3, 3, "(string remoteAddress) Connects this NC object to the remote address.")
{
   NetAddress addr;
   if(!Net::stringToAddress(argv[2], &addr))
   {
      Con::errorf("NetConnection::connect: invalid address - %s", argv[2]);
      return;
   }
   object->connect(&addr);
}

ConsoleMethod(NetConnection, SetPort, void, 3,3, "" )
{
	object->SetSocket( atoi( argv[2] ) );
}

ConsoleMethod(NetConnection, connectLocal, const char *, 2, 2, "Connects a connection to the server running in the same process.")
{
   ConsoleObject *co = ConsoleObject::create(object->getClassName());
   NetConnection *client = object;
   NetConnection *server = dynamic_cast<NetConnection *>(co);
   const char *error = NULL;
   BitStream *stream = BitStream::getPacketStream();

   if(!server || !server->canRemoteCreate())
	   goto errorOut;
   server->registerObject();
   server->setIsLocalClientConnection();

   server->setSequence(0);
   client->setSequence(0);
   client->setRemoteConnectionObject(server);
   server->setRemoteConnectionObject(client);

   stream->setPosition(0);
   client->writeConnectRequest(stream);
   stream->setPosition(0);
   if(!server->readConnectRequest(stream, &error))
      goto errorOut;

   stream->setPosition(0);
   server->writeConnectAccept(stream);
   stream->setPosition(0);

   if(!client->readConnectAccept(stream, &error))
      goto errorOut;

   client->onConnectionEstablished(true);
   server->onConnectionEstablished(false);
   client->setEstablished();
   server->setEstablished();
   client->setConnectSequence(0);
   server->setConnectSequence(0);
   NetConnection::setLocalClientConnection(server);
   server->assignName("LocalClientConnection");
   return "";

errorOut:
   server->deleteObject();
   client->deleteObject();
   if(!error)
      error = "Unknown Error";
   return error;
}
