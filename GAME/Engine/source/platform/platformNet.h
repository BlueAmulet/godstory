//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PLATFORM_PLATFORMNET_H_
#define _PLATFORM_PLATFORMNET_H_

#include "platform/platform.h"
#include "core/coreRes.h"
#include "util/journal/journaledSignal.h"

#include <hash_set>

#define MAXPACKETSIZE 1500
#define LIMITEDPACKETSIZE 700

typedef int NetConnectionId;

/// Generic network address
///
/// This is used to represent IP addresses.
struct NetAddress 
{
   int type;        ///< Type of address (IPAddress currently)

   /// Acceptable NetAddress types.
   enum 
   {
      IPAddress,
   };

   U8 netNum[4];    ///< For IP:  sin_addr<br>
   U8 nodeNum[6];   ///< For IP:  Not used.<br>
   U16  port;       ///< For IP:  sin_port<br>
};

typedef S32 NetSocket;
const NetSocket InvalidSocket = -1;

/// void event(NetSocket sock, U32 state) 
typedef JournaledSignal<NetSocket,U32> ConnectionNotifyEvent;

/// void event(NetSocket listeningPort, NetSocket newConnection, NetAddress originatingAddress)
typedef JournaledSignal<NetSocket,NetSocket,NetAddress> ConnectionAcceptedEvent;

/// void event(NetSocket connection, RawData incomingData)
typedef JournaledSignal<NetSocket,RawData> ConnectionReceiveEvent;

/// void event(NetAddress originator, RawData incomingData)
typedef JournaledSignal<NetAddress,RawData> PacketReceiveEvent;

/// void event(NetAddress originator, int udpSocket, RawData incomingData)
typedef JournaledSignal<NetAddress, int, RawData> PacketReceiveExEvent;

/// Platform-specific network operations.
struct Net
{
   enum Error
   {
      NoError,
      WrongProtocolType,
      InvalidPacketProtocol,
      WouldBlock,
      NotASocket,
      UnknownError
   };

   enum ConnectionState {
      DNSResolved,
      DNSFailed,
      Connected,
      ConnectFailed,
      Disconnected
   };

   enum Protocol
   {
      UDPProtocol,
      TCPProtocol
   };

   static const int MaxPacketBuffSize = MAXPACKETSIZE+128;
   static const int MaxPacketDataSize = MAXPACKETSIZE;
   static const int LimitedPacketDataSize = LIMITEDPACKETSIZE;
   static const int OnePacketDataSize = MaxPacketDataSize-LimitedPacketDataSize;

   static int mLastSendTime;
   static int mLastRecvTime;
   static int mSendSize;
   static int mRecvSize;
   static int mSSPS;
   static int mRSPS;

   static ConnectionNotifyEvent   smConnectionNotify;
   static ConnectionAcceptedEvent smConnectionAccept;
   static ConnectionReceiveEvent  smConnectionReceive;
   static PacketReceiveEvent      smPacketReceive;
   static PacketReceiveExEvent	  smPacketReceiveEx;

   static stdext::hash_set<int>		hashSocketMap;

   static bool init();
   static void shutdown();

   // Unreliable net functions (UDP)
   // sendto is for sending data
   // all incoming data comes in on packetReceiveEventType
   // App can only open one unreliable port... who needs more? ;)
   static bool openPort(S32 connectPort);
   static int openPortEx( S32 connectPort );
   static void closePort();
   static void closePortEx();
   static Error sendto(const NetAddress *address, const U8 *buffer, S32 bufferSize);
   static Error sendtoEx(const NetAddress *address, int udpS, const U8 *buffer, S32 bufferSize);

   // Reliable net functions (TCP)
   // all incoming messages come in on the Connected* events
   static NetSocket openListenPort(U16 port);
   static NetSocket openConnectTo(const char *stringAddress); // does the DNS resolve etc.
   static void closeConnectTo(NetSocket socket);
   static Error sendtoSocket(NetSocket socket, const U8 *buffer, S32 bufferSize);

   static bool compareAddresses(const NetAddress *a1, const NetAddress *a2);
   static bool stringToAddress(const char *addressString, NetAddress *address);
   static void addressToString(const NetAddress *address, char addressString[256]);

   // lower level socked based network functions
   static NetSocket openSocket();
   static Error closeSocket(NetSocket socket);

   static Error send(NetSocket socket, const U8 *buffer, S32 bufferSize);
   static Error recv(NetSocket socket, U8 *buffer, S32 bufferSize, S32 *bytesRead);

   static Error connect(NetSocket socket, const NetAddress *address);
   static Error listen(NetSocket socket, S32 maxConcurrentListens);
   static NetSocket accept(NetSocket acceptSocket, NetAddress *remoteAddress);

   static Error bind(NetSocket socket, U16 port);
   static Error setBufferSize(NetSocket socket, S32 bufferSize);
   static Error setBroadcast(NetSocket socket, bool broadcastEnable);
   static Error setBlocking(NetSocket socket, bool blockingIO);


   
private:
   static void process();

};

#endif