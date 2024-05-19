/* auto generated file net */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/array.h>
#include <pulchritude/error.h>
#include <pulchritude/stream.h>
#include <pulchritude/string.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
  uint8_t [16] address;
  uint16_t port;
} PuleNetAddress;
typedef struct {
  PuleBufferView data;
  uint64_t clientUuid;
  uint32_t lane;
  uint8_t channel;
} PuleNetPacketReceive;
typedef struct {
  PuleNetAddress address;
  uint32_t connectionLimit;
  uint32_t outgoingLaneCount;
} PuleNetHostCreateInfo;
typedef struct {
  PuleStringView hostname;
  uint16_t port;
  uint32_t outgoingLaneCount;
} PuleNetClientCreateInfo;

// enum
typedef enum {
  PuleErrorNet_none,
  PuleErrorNet_initialize,
  PuleErrorNet_hostCreate,
  PuleErrorNet_poll,
  PuleErrorNet_clientCreate,
  PuleErrorNet_clientConnect,
  PuleErrorNet_clientDisconnect,
  PuleErrorNet_clientSend,
  PuleErrorNet_packetSend,
} PuleErrorNet;
typedef enum {
  PuleNetChannelType_reliable,
  PuleNetChannelType_unreliable,
  PuleNetChannelType_unsequenced,
} PuleNetChannelType;

// entities
typedef struct { uint64_t id; } PuleNetHost;
typedef struct { uint64_t id; } PuleNetClient;
typedef struct { uint64_t id; } PuleNetStreamTransferUpload;
typedef struct { uint64_t id; } PuleNetStreamTransferDownload;

// functions
PULE_exportFn PuleNetAddress puleNetAddressLocalhost(uint16_t port);
PULE_exportFn PuleNetHost puleNetHostCreate(PuleNetHostCreateInfo ci, PuleError * error);
PULE_exportFn void puleNetHostDestroy(PuleNetHost host);
/*  returns client uuid, 0 if no connection  */
PULE_exportFn uint64_t puleNetHostPollConnection(PuleNetHost host);
PULE_exportFn uint32_t puleNetHostPoll(PuleNetHost host, uint32_t maxPacketsToReceive, PuleNetPacketReceive * packets, PuleError * error);
PULE_exportFn void puleNetHostSendPacket(PuleNetHost host, uint64_t clientUuid, PuleNetChannelType channel, uint32_t lane, PuleBufferView packet, PuleError * error);
PULE_exportFn void puleNetHostBroadcastPacket(PuleNetHost host, PuleNetChannelType channel, uint32_t lane, PuleBufferView packet, PuleError * error);
PULE_exportFn PuleNetClient puleNetClientCreate(PuleNetClientCreateInfo ci, PuleError * error);
PULE_exportFn void puleNetClientDestroy(PuleNetClient client);
PULE_exportFn bool puleNetClientConnected(PuleNetClient client);
PULE_exportFn uint32_t puleNetClientPoll(PuleNetClient client, uint32_t maxPacketsToReceive, PuleNetPacketReceive * packets, PuleError * error);
PULE_exportFn void puleNetClientSendPacket(PuleNetClient client, PuleNetChannelType channel, uint32_t lane, PuleBufferView packet, PuleError * error);
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
PULE_exportFn PuleNetStreamTransferDownload puleNetClientDownloadStreamCheck(PuleNetClient client, uint32_t packetsLength, PuleNetPacketReceive * packets, uint32_t uploadStreamLane, PuleStreamWrite streamWriter);
/*  returns true if the stream exists  */
PULE_exportFn bool puleNetClientDownloadStreamExists(PuleNetClient client, PuleNetStreamTransferDownload transfer);
/* 
  returns true if more data to consume
 */
PULE_exportFn bool puleNetClientDownloadStreamContinue(PuleNetClient client, PuleNetStreamTransferDownload transfer, uint32_t packetsLength, PuleNetPacketReceive * packets);

#ifdef __cplusplus
} // extern C
#endif
