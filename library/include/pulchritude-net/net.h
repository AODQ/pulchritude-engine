#pragma once

#include <pulchritude-core/core.h>

#include <pulchritude-array/array.h>
#include <pulchritude-error/error.h>
#include <pulchritude-string/string.h>

// implements networking, uses enet and I'm not sure if the backend can even
// be swappable

#if defined(PULCHRITUDE_FEATURE_NET)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { uint64_t id; } PuleNetHost;

typedef struct PuleNetHostAddress {
  uint8_t address[16];
  uint16_t port;
} PuleNetHostAddress;

typedef enum {
  PuleErrorNet_none,
  PuleErrorNet_initialize,
  PuleErrorNet_hostCreate,
  PuleErrorNet_clientCreate,
  PuleErrorNet_clientConnect,
  PuleErrorNet_clientDisconnect,
  PuleErrorNet_clientSend,
} PuleErrorNet;

typedef enum {
  PuleNetChannelType_reliable,
  PuleNetChannelType_unreliable,
  PuleNetChannelType_unsequenced,
} PuleNetChannelType;

typedef struct PuleNetPacketReceive {
  PuleBufferView data;
  uint8_t channel;
} PuleNetPacketReceive;

typedef struct PuleNetPacketSend {
  PuleBufferView data;
  uint64_t peerUuid;
  uint8_t channel;
} PuleNetPacketSend;

typedef struct PuleNetHostCreateInfo {
  PuleNetHostAddress address;
  uint32_t connectionLimit;
  PuleNetChannelType channels[32];
  uint32_t channelLength;
  uint32_t bandwidthIncomingBytesPerSecond PULE_param(0);
  uint32_t bandwidthOutgoingBytesPerSecond PULE_param(0);
  void (* receivePacket)(
    PuleNetHost const host,
    PuleNetPacketReceive const packet,
    uint64_t const peerUuid,
    void * const userData
  );
  size_t (* receiveConnect)(
    PuleNetHost const host,
    PuleNetHostAddress const address,
    uint64_t const peerUuid,
    void * const userData
  );
  void (* receiveDisconnect)(PuleNetHost const host, void * const userdata);
  void * userData;
} PuleNetHostCreateInfo;
PULE_exportFn PuleNetHost puleNetHostCreate(
  PuleNetHostCreateInfo const ci,
  PuleError * const error
);
PULE_exportFn void puleNetHostDestroy(PuleNetHost const host);

PULE_exportFn void puleNetHostPoll(PuleNetHost const host);
// TODO broadcast
PULE_exportFn void puleNetHostSendPacket(
  PuleNetHost const host,
  PuleNetPacketSend const packet,
  PuleError * const error
);


//PULE_exportFn void puleNet


typedef struct { uint64_t id; } PuleNetClient;

typedef struct PuleNetClientCreateInfo {
  PuleStringView hostname;
  uint16_t port;
  PuleNetChannelType channels[32];
  uint32_t channelLength;
  uint32_t bandwidthIncomingBytesPerSecond PULE_param(0);
  uint32_t bandwidthOutgoingBytesPerSecond PULE_param(0);
  void (* receivePacket)(
    PuleNetClient const client,
    PuleNetPacketReceive const packet,
    void * const userData
  );
  void (* receiveDisconnect)(PuleNetClient const client, void * const userdata);
  void * userData;
} PuleNetClientCreateInfo;
PULE_exportFn PuleNetClient puleNetClientCreate(
  PuleNetClientCreateInfo const ci,
  PuleError * const error
);
PULE_exportFn void puleNetClientDestroy(PuleNetClient const client);

PULE_exportFn void puleNetClientPoll(PuleNetClient const client);

#ifdef __cplusplus
} // extern C
#endif

#endif
