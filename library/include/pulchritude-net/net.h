#pragma once

#include <pulchritude-core/core.h>

#include <pulchritude-array/array.h>
#include <pulchritude-error/error.h>
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
  uint8_t channel;
} PuleNetPacketReceive;

typedef struct PuleNetPacketSend {
  PuleBufferView data;
  uint8_t channel;
} PuleNetPacketSend;

typedef struct PuleNetHostCreateInfo {
  PuleNetAddress address;
  uint32_t connectionLimit;
  PuleNetChannelType channels[32];
  uint32_t channelLength;
  void (* receivePacket)(
    PuleNetHost const host,
    PuleNetPacketReceive const packet,
    uint64_t const clientUuid,
    void * const userData
  );
  void (* receiveConnect)(
    PuleNetHost const host,
    PuleNetAddress const address,
    uint64_t const clientUuid,
    void * const userData
  );
  void (* receiveDisconnect)(
    PuleNetHost const host,
    uint64_t const clientUuid,
    void * const userdata
  );
  void * userData PULE_param(nullptr);
  uint32_t bandwidthIncomingBytesPerSecond PULE_param(0);
  uint32_t bandwidthOutgoingBytesPerSecond PULE_param(0);
} PuleNetHostCreateInfo;
PULE_exportFn PuleNetHost puleNetHostCreate(
  PuleNetHostCreateInfo const ci,
  PuleError * const error
);
PULE_exportFn void puleNetHostDestroy(PuleNetHost const host);

PULE_exportFn void puleNetHostPoll(PuleNetHost const host);
PULE_exportFn void puleNetHostSendPacket(
  PuleNetHost const host,
  uint64_t clientUuid,
  PuleNetPacketSend const packet,
  PuleError * const error
);
PULE_exportFn void puleNetHostBroadcastPacket(
  PuleNetHost const host,
  PuleNetPacketSend const packet,
  PuleError * const error
);

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

PULE_exportFn bool puleNetClientConnected(PuleNetClient const client);
PULE_exportFn void puleNetClientPoll(PuleNetClient const client);
PULE_exportFn void puleNetClientSendPacket(
  PuleNetClient const client,
  PuleNetPacketSend const packet,
  PuleError * const error
);

#ifdef __cplusplus
} // extern C
#endif
