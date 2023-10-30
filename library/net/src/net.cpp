#include <pulchritude-net/net.h>

#if defined(PULCHRITUDE_FEATURE_NET)

#include <cstdio> // just for debug in enet TODO remove
#define ENET_IMPLEMENTATION
#include <enet.h>

namespace pint {
bool hasInitializedEnet = false;

struct Host {
  ENetHost * host;
  pule::ResourceContainer<ENetPeer *> peers;
  std::unordered_map<uint64_t, uint64_t> peerIdToUuid;
  PuleNetChannelType channels[32];
  uint32_t channelLength;
  void (* receivePacket)(
    PuleNetHost const, PuleNetPacketReceive const, uint64_t const, void * const
  );
  void (* receiveConnect)(
    PuleNetHost const, PuleNetAddress const, uint64_t const, void * const
  );
  void (* receiveDisconnect)(PuleNetHost const, uint64_t const, void * const);
  void * userData;
};

struct Client {
  ENetHost * host;
  ENetPeer * peer;
  PuleNetChannelType channels[32];
  uint32_t channelLength;
  void (*receivePacket)(
   PuleNetClient const, PuleNetPacketReceive const, void * const
 );
  void (*receiveDisconnect)(PuleNetClient const, void * const);
  void * userData;
  bool hasConnected;
};

pule::ResourceContainer<pint::Host> hosts;
pule::ResourceContainer<pint::Client> clients;

void sendPacket(
  ENetPeer * peer,
  PuleNetPacketSend const packet,
  uint8_t channelIndex,
  PuleNetChannelType const channelType,
  PuleError * const error
) {
  ENetPacketFlag flags;
  PuleNetChannelType const channel = channelType;
  switch (channel) {
    case PuleNetChannelType_reliable:
      flags = ENET_PACKET_FLAG_RELIABLE;
    break;
    case PuleNetChannelType_unreliable:
      flags = ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT;
    break;
    case PuleNetChannelType_unsequenced:
      flags = ENET_PACKET_FLAG_UNSEQUENCED;
    break;
  }
  ENetPacket * const enetPacket = (
    enet_packet_create(
      packet.data.data,
      packet.data.byteLength,
      flags
    )
  );
  auto t = enet_peer_send(peer, channelIndex, enetPacket);
  if (t < 0) {
    PULE_error(PuleErrorNet_packetSend, "enet_peer_send() failed");
    puleLog("error %d", t);
    enet_packet_destroy(enetPacket);
  }

  puleLog(
    "sent packet length %d on channel %u",
    enetPacket->dataLength,
    packet.channel
  );
}

}

extern "C" {

// -- address ------------------------------------------------------------------

PULE_exportFn PuleNetAddress puleNetAddressLocalhost(
  uint16_t const port
) {
  PuleNetAddress address = {};
  in6_addr host = ENET_HOST_ANY;
  memcpy(address.address, &host, 16);
  address.port = port;
  return address;
}

// -- host ---------------------------------------------------------------------

PuleNetHost puleNetHostCreate(
  PuleNetHostCreateInfo const ci,
  PuleError * const error
) {
  if (!pint::hasInitializedEnet) {
    if (enet_initialize() != 0) {
      PULE_error(PuleErrorNet_initialize, "enet_initialize() failed");
      return { .id = 0, };
    }
    atexit(enet_deinitialize);
    pint::hasInitializedEnet = true;
  }

  ENetAddress address = {};
  memcpy(&address.host, &ci.address.address, sizeof(in6_addr));
  address.host = ENET_HOST_ANY;
  address.port = ci.address.port;
  address.sin6_scope_id = 0;
  puleLog(
    "ci channel length %d bandwidth in %d bandwidth out %d",
    ci.channelLength,
    ci.bandwidthIncomingBytesPerSecond,
    ci.bandwidthOutgoingBytesPerSecond
  );
  ENetHost * const host = (
    enet_host_create(
      &address,
      ci.connectionLimit,
      ci.channelLength,
      ci.bandwidthIncomingBytesPerSecond,
      ci.bandwidthOutgoingBytesPerSecond
    )
  );

  if (host == nullptr) {
    PULE_error(PuleErrorNet_hostCreate, "enet_host_create() failed");
    return { .id = 0, };
  }
  auto server = pint::Host {
    .host = host,
    .peers = {},
    .peerIdToUuid = {},
    .channels = {},
    .channelLength = ci.channelLength,
    .receivePacket = ci.receivePacket,
    .receiveConnect = ci.receiveConnect,
    .receiveDisconnect = ci.receiveDisconnect,
  };
  memcpy(server.channels, ci.channels, sizeof(PuleNetChannelType) * 32);
  return { .id = pint::hosts.create(server) };
}

void puleNetHostDestroy(
  PuleNetHost const host
) {
  enet_host_destroy(reinterpret_cast<ENetHost *>(host.id));
}

PULE_exportFn void puleNetHostPoll(PuleNetHost const puHost) {
  pint::Host & host = *pint::hosts.at(puHost.id);

  ENetEvent event = {};
  if (enet_host_service(host.host, &event, 0) <= 0) {
    return;
  }
  puleLog("enet_host_service() returned %d", event.type);
  switch (event.type) {
    case ENET_EVENT_TYPE_NONE: { break; }
    case ENET_EVENT_TYPE_CONNECT: {
      puleLog("[server] client connected");
      PuleNetAddress address;
      memcpy(&address.address, &event.peer->address, sizeof(struct in6_addr));
      address.port = event.peer->address.port;
      uint64_t const clientUuid = host.peers.create(event.peer);
      host.peerIdToUuid.emplace(event.peer->connectID, clientUuid);
      host.receiveConnect(puHost, address, clientUuid, host.userData);
    } break;
    case ENET_EVENT_TYPE_RECEIVE: {
      puleLog(
        "received packet length %d on channel %u",
        event.packet->dataLength,
        event.channelID
      );
      PuleNetPacketReceive const packet = {
        .data = {
          .data = event.packet->data,
          .byteLength = event.packet->dataLength,
        },
        .channel = event.channelID,
      };
      host.receivePacket(
        puHost,
        packet,
        host.peerIdToUuid.at(event.peer->connectID),
        host.userData
      );
      enet_packet_destroy(event.packet);
    } break;
    case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
    case ENET_EVENT_TYPE_DISCONNECT: {
      puleLog("client disconnected");
      auto uuid = host.peerIdToUuid.at(event.peer->connectID);
      host.receiveDisconnect(puHost, uuid, host.userData);
      host.peers.destroy(uuid);
      host.peerIdToUuid.erase(event.peer->connectID);
      event.peer->data = nullptr;
    } break;
  }
}

void puleNetHostSendPacket(
  PuleNetHost const puHost,
  uint64_t const clientUuid,
  PuleNetPacketSend const packet,
  PuleError * const error
) {
  pint::Host & host = *pint::hosts.at(puHost.id);
  pint::sendPacket(
    host.peers.at(clientUuid),
    packet,
    packet.channel,
    host.channels[packet.channel],
    error
  );
}
PULE_exportFn void puleNetHostBroadcastPacket(
  PuleNetHost const puHost,
  PuleNetPacketSend const packet,
  PuleError * const error
) {
  pint::Host & host = *pint::hosts.at(puHost.id);
  for (auto const & peer : host.peers.data) {
    pint::sendPacket(
      peer.second,
      packet,
      packet.channel,
      host.channels[packet.channel],
      error
    );
  }
}

// -- client -------------------------------------------------------------------

PuleNetClient puleNetClientCreate(
  PuleNetClientCreateInfo const ci,
  PuleError * const error
) {
  if (!pint::hasInitializedEnet) {
    if (enet_initialize() != 0) {
      PULE_error(PuleErrorNet_initialize, "enet_initialize() failed");
      return { .id = 0, };
    }
    atexit(enet_deinitialize);
    pint::hasInitializedEnet = true;
  }

  ENetAddress address = {};
  //enet_address_set_host(&address, ci.hostname.contents);
  enet_address_set_host(&address, "127.0.0.1");
  address.port = ci.port;

  puleLog("ci channel length %d bandwidth in %d bandwidth out %d",
    ci.channelLength,
    ci.bandwidthIncomingBytesPerSecond,
    ci.bandwidthOutgoingBytesPerSecond
  );
  ENetHost * const host = enet_host_create(
    nullptr,
    1,
    ci.channelLength,
    ci.bandwidthIncomingBytesPerSecond,
    ci.bandwidthOutgoingBytesPerSecond
  );

  if (host == nullptr) {
    PULE_error(PuleErrorNet_hostCreate, "enet_host_create() failed");
    return { .id = 0, };
  }

  ENetPeer * const peer = (
    enet_host_connect(host, &address, ci.channelLength, 0)
  );
  if (peer == nullptr) {
    PULE_error(PuleErrorNet_clientCreate, "enet_host_connect() failed");
    enet_host_destroy(host);
    return { .id = 0, };
  }

  puleLog("connected to %s:%d", ci.hostname.contents, ci.port);
  auto client = pint::Client {
    .host = host,
    .peer = peer,
    .channels = {},
    .channelLength = ci.channelLength,
    .receivePacket = ci.receivePacket,
    .receiveDisconnect = ci.receiveDisconnect,
    .hasConnected = false,
  };
  memcpy(client.channels, ci.channels, sizeof(PuleNetChannelType) * 32);
  return { .id = pint::clients.create(client) };
}

void puleNetClientDestroy(
  PuleNetClient const puClient
) {
  if (puClient.id == 0) { return; }
  pint::Client * client = pint::clients.at(puClient.id);
  if (client == nullptr) { return; }
  enet_peer_reset(client->peer);
  enet_peer_disconnect(client->peer, 0);
  enet_host_destroy(client->host);
  pint::clients.destroy(puClient.id);
}

bool puleNetClientConnected(PuleNetClient const client) {
  return pint::clients.at(client.id)->hasConnected;
}

void puleNetClientPoll(
  PuleNetClient const puClient
) {
  pint::Client & client = *pint::clients.at(puClient.id);

  if (!client.hasConnected) {
    // attempt connection
    ENetEvent event = {};
    puleLog("attempting connection");
    auto const t = enet_host_service(client.host, &event, 0);
    if (t <= 0 || event.type != ENET_EVENT_TYPE_CONNECT) {
      puleLog("Attempt to connect failed");
      puleLog("return value %d", t);
      puleLog(
        "event type != connect? %d: %d",
        event.type != ENET_EVENT_TYPE_CONNECT,
        event.type
      );
      return;
    }
    puleLog("connected");
    client.hasConnected = true;
  }

  ENetEvent event = {};
  if (enet_host_service(client.host, &event, 0) <= 0) {
    return;
  }
  switch (event.type) {
    case ENET_EVENT_TYPE_NONE: { break; }
    case ENET_EVENT_TYPE_CONNECT: {
      PULE_assert(false && "unexpected ENET_EVENT_TYPE_CONNECT");
      break;
    }
    case ENET_EVENT_TYPE_RECEIVE: {
      puleLog(
        "received packet length %d on channel %u",
        event.packet->dataLength,
        event.channelID
      );
      PuleNetPacketReceive const packet = {
        .data = {
          .data = event.packet->data,
          .byteLength = event.packet->dataLength,
        },
        .channel = event.channelID,
      };
      client.receivePacket(puClient, packet, client.userData);
      enet_packet_destroy(event.packet);
      break;
    }
    case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
    case ENET_EVENT_TYPE_DISCONNECT: {
      puleLog("disconnected from host");
      client.receiveDisconnect(puClient, client.userData);
      break;
    }
  }
}

PULE_exportFn void puleNetClientSendPacket(
  PuleNetClient const puClient,
  PuleNetPacketSend const packet,
  PuleError * const error
) {
  pint::Client & client = *pint::clients.at(puClient.id);
  pint::sendPacket(
    client.peer,
    packet,
    packet.channel,
    client.channels[packet.channel],
    error
  );
}

} // extern C

#endif
