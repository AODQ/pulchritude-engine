#pragma once

#include <pulchritude-core/core.h>

#include <pulchritude-array/array.h>
#include <pulchritude-error/error.h>
#include <pulchritude-stream/stream.h>
#include <pulchritude-string/string.h>

// implements networking, uses enet and I'm not sure if the backend can even
// be swappable

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { uint64_t id; } PuleNetHost;

typedef struct PuleNetHostAddress {
  uint8_t address[16];
  uint16_t port;
} PuleNetAddress;

PULE_exportFn PuleNetAddress puleNetAddressLocalhost(
  uint16_t const port
);

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
PULE_exportFn PuleNetHost puleNetHostCreate(
  PuleNetHostCreateInfo const ci,
  PuleError * const error
);
PULE_exportFn void puleNetHostDestroy(PuleNetHost const host);

// returns client uuid, 0 if no connection
PULE_exportFn uint64_t puleNetHostPollConnection(
  PuleNetHost const host
);
PULE_exportFn uint32_t puleNetHostPoll(
  PuleNetHost const host,
  uint32_t const maxPacketsToReceive,
  PuleNetPacketReceive * const packets,
  PuleError * const error
);
PULE_exportFn void puleNetHostSendPacket(
  PuleNetHost const host,
  uint64_t clientUuid,
  PuleNetChannelType const channel,
  uint32_t const lane,
  PuleBufferView const packet,
  PuleError * const error
);
PULE_exportFn void puleNetHostBroadcastPacket(
  PuleNetHost const host,
  PuleNetChannelType const channel,
  uint32_t const lane,
  PuleBufferView const packet,
  PuleError * const error
);

typedef struct { uint64_t id; } PuleNetClient;

typedef struct PuleNetClientCreateInfo {
  PuleStringView hostname;
  uint16_t port;
  uint32_t outgoingLaneCount;
} PuleNetClientCreateInfo;
PULE_exportFn PuleNetClient puleNetClientCreate(
  PuleNetClientCreateInfo const ci,
  PuleError * const error
);
PULE_exportFn void puleNetClientDestroy(PuleNetClient const client);

PULE_exportFn bool puleNetClientConnected(PuleNetClient const client);
PULE_exportFn uint32_t puleNetClientPoll(
  PuleNetClient const client,
  uint32_t const maxPacketsToReceive,
  PuleNetPacketReceive * const packets,
  PuleError * const error
);
PULE_exportFn void puleNetClientSendPacket(
  PuleNetClient const client,
  PuleNetChannelType const channel,
  uint32_t const lane,
  PuleBufferView const packet,
  PuleError * const error
);

// -- utility, might move this to its own third party plugin --

typedef struct { uint64_t id; } PuleNetStreamTransferUpload;
typedef struct { uint64_t id; } PuleNetStreamTransferDownload;

// You must not send packets on the specified lane until the stream is finished
// Only one transfer per-stream can be done at a time
// TODO right now this blocks, needs progress callback I suppose
PULE_exportFn PuleNetStreamTransferUpload puleNetHostUploadStream(
  PuleNetHost const host,
  uint64_t clientUuid,
  PuleStreamRead const stream,
  uint32_t uploadStreamLane
);

// to support this on client-side, you need to pass in all packets received,
//   and the stream transfer will communicate on the specified dedicated lane
PULE_exportFn PuleNetStreamTransferDownload puleNetClientDownloadStreamCheck(
  PuleNetClient const client,
  uint32_t const packetsLength,
  PuleNetPacketReceive * const packets,
  uint32_t uploadStreamLane,
  PuleStreamWrite const streamWriter
);

// returns true if the stream exists
PULE_exportFn bool puleNetClientDownloadStreamExists(
  PuleNetClient const client,
  PuleNetStreamTransferDownload const transfer
);

// returns true if more data to consume
PULE_exportFn bool puleNetClientDownloadStreamContinue(
  PuleNetClient const client,
  PuleNetStreamTransferDownload const transfer,
  uint32_t const packetsLength,
  PuleNetPacketReceive * const packets
);

#ifdef __cplusplus
} // extern C
#endif
