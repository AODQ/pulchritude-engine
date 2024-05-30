/* auto generated file net */
#pragma once
#include "core.h"

#include "array.h"
#include "error.h"
#include "stream.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleErrorNet_none = 0,
  PuleErrorNet_initialize = 1,
  PuleErrorNet_hostCreate = 2,
  PuleErrorNet_poll = 3,
  PuleErrorNet_clientCreate = 4,
  PuleErrorNet_clientConnect = 5,
  PuleErrorNet_clientDisconnect = 6,
  PuleErrorNet_clientSend = 7,
  PuleErrorNet_packetSend = 8,
} PuleErrorNet;
const uint32_t PuleErrorNetSize = 9;
typedef enum {
  PuleNetChannelType_reliable = 0,
  PuleNetChannelType_unreliable = 1,
  PuleNetChannelType_unsequenced = 2,
} PuleNetChannelType;
const uint32_t PuleNetChannelTypeSize = 3;

// entities
typedef struct PuleNetHost { uint64_t id; } PuleNetHost;
typedef struct PuleNetClient { uint64_t id; } PuleNetClient;
typedef struct PuleNetStreamTransferUpload { uint64_t id; } PuleNetStreamTransferUpload;
typedef struct PuleNetStreamTransferDownload { uint64_t id; } PuleNetStreamTransferDownload;

// structs
struct PuleNetAddress;
struct PuleNetPacketReceive;
struct PuleNetHostCreateInfo;
struct PuleNetClientCreateInfo;

typedef struct PuleNetAddress {
  uint8_t address[16] ;
  uint16_t port;
} PuleNetAddress;
typedef struct PuleNetPacketReceive {
  PuleBufferView data;
  uint64_t clientUuid;
  uint32_t lane;
  uint8_t channel;
} PuleNetPacketReceive;
typedef struct PuleNetHostCreateInfo {
  PuleNetAddress address;
  uint32_t connectionLimit;
  uint32_t outgoingLaneCount;
} PuleNetHostCreateInfo;
typedef struct PuleNetClientCreateInfo {
  PuleStringView hostname;
  uint16_t port;
  uint32_t outgoingLaneCount;
} PuleNetClientCreateInfo;

// functions
PULE_exportFn PuleNetAddress puleNetAddressLocalhost(uint16_t port);
PULE_exportFn PuleNetHost puleNetHostCreate(PuleNetHostCreateInfo ci, struct PuleError * error);
PULE_exportFn void puleNetHostDestroy(PuleNetHost host);
/*  returns client uuid, 0 if no connection  */
PULE_exportFn uint64_t puleNetHostPollConnection(PuleNetHost host);
PULE_exportFn uint32_t puleNetHostPoll(PuleNetHost host, uint32_t maxPacketsToReceive, struct PuleNetPacketReceive * packets, struct PuleError * error);
PULE_exportFn void puleNetHostSendPacket(PuleNetHost host, uint64_t clientUuid, PuleNetChannelType channel, uint32_t lane, PuleBufferView packet, struct PuleError * error);
PULE_exportFn void puleNetHostBroadcastPacket(PuleNetHost host, PuleNetChannelType channel, uint32_t lane, PuleBufferView packet, struct PuleError * error);
PULE_exportFn PuleNetClient puleNetClientCreate(PuleNetClientCreateInfo ci, struct PuleError * error);
PULE_exportFn void puleNetClientDestroy(PuleNetClient client);
PULE_exportFn bool puleNetClientConnected(PuleNetClient client);
PULE_exportFn uint32_t puleNetClientPoll(PuleNetClient client, uint32_t maxPacketsToReceive, struct PuleNetPacketReceive * packets, struct PuleError * error);
PULE_exportFn void puleNetClientSendPacket(PuleNetClient client, PuleNetChannelType channel, uint32_t lane, PuleBufferView packet, struct PuleError * error);
/* 
You must not send packets on the specified lane until the stream is finished
Only one transfer per-stream can be done at a time
TODO right now this blocks, needs progress callback I suppose
 */
PULE_exportFn PuleNetStreamTransferUpload puleNetHostUploadStream(PuleNetHost host, uint64_t clientUuid, PuleStreamRead stream, uint32_t uploadStreamLane);
/* 
to support this on client-side, you need to pass in all packets received, and
the stream transfer will communicate on the specified dedicated lane
 */
PULE_exportFn PuleNetStreamTransferDownload puleNetClientDownloadStreamCheck(PuleNetClient client, uint32_t packetsLength, struct PuleNetPacketReceive * packets, uint32_t uploadStreamLane, PuleStreamWrite streamWriter);
/*  returns true if the stream exists  */
PULE_exportFn bool puleNetClientDownloadStreamExists(PuleNetClient client, PuleNetStreamTransferDownload transfer);
/* 
  returns true if more data to consume
 */
PULE_exportFn bool puleNetClientDownloadStreamContinue(PuleNetClient client, PuleNetStreamTransferDownload transfer, uint32_t packetsLength, struct PuleNetPacketReceive * packets);

#ifdef __cplusplus
} // extern C
#endif
