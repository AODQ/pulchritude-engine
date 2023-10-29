#include <pulchritude-net/net.h>

#if defined(PULCHRITUDE_FEATURE_NET)

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
  size_t (* receiveConnect)(
    PuleNetHost const, PuleNetHostAddress const, uint64_t const, void * const
  );
  void (* receiveDisconnect)(PuleNetHost const, void * const);
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
  if (enet_peer_send(peer, channelIndex, enetPacket) < 0) {
    PULE_error(PuleErrorNet_packetSend, "enet_peer_send() failed");
    enet_packet_destroy(enetPacket);
  }

  puleLog(
    "sent packet length %d on channel %u",
    enetPacket->dataLength,
    packet.channel
  );
  // TODO do we need to destroy the packet?
}

}

extern "C" {

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
    pint::hasInitializedEnet = true;
  }

  ENetAddress address = {};
  memcpy(&address, &ci.address, sizeof(ENetAddress));
  address.port = ci.address.port;
  address.sin6_scope_id = 0;

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
  switch (event.type) {
    case ENET_EVENT_TYPE_NONE: { break; }
    case ENET_EVENT_TYPE_CONNECT: {
      puleLog("client connected");
      PuleNetHostAddress address;
      memcpy(&address.address, &event.peer->address, sizeof(struct in6_addr));
      address.port = event.peer->address.port;
      uint64_t const peerUuid = host.peers.create(event.peer);
      host.peerIdToUuid.emplace(event.peer->connectID, peerUuid);
      host.receiveConnect(puHost, address, peerUuid, host.userData);
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
      host.receiveDisconnect(puHost, host.userData);
      auto uuid = host.peerIdToUuid.at(event.peer->connectID);
      host.peers.destroy(uuid);
      host.peerIdToUuid.erase(event.peer->connectID);
      event.peer->data = nullptr;
    } break;
  }
}

void puleNetHostSendPacket(
  PuleNetHost const puHost,
  uint64_t const peerUuid,
  PuleNetPacketSend const packet,
  PuleError * const error
) {
  pint::Host & host = *pint::hosts.at(puHost.id);
  pint::sendPacket(
    host.peers.at(peerUuid),
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
    pint::hasInitializedEnet = true;
  }

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

  ENetAddress address = {};
  enet_address_set_host(&address, ci.hostname.contents);
  address.port = ci.port;

  ENetPeer * peer = (
    enet_host_connect(
      host,
      &address,
      ci.channelLength,
      0
    )
  );
  if (peer == nullptr) {
    PULE_error(PuleErrorNet_clientCreate, "enet_host_connect() failed");
    enet_host_destroy(host);
    return { .id = 0, };
  }

  { // attempt connection
    ENetEvent event = {};
    puleLog("connecting to %s:%d", ci.hostname.contents, ci.port);
    if (
      enet_host_service(host, &event, 5000) <= 0
      && event.type == ENET_EVENT_TYPE_CONNECT
    ) {
      PULE_error(PuleErrorNet_clientConnect, "enet_host_service() failed");
      enet_peer_reset(peer);
      enet_peer_disconnect(peer, 0);
      enet_host_destroy(host);
      return { .id = 0, };
    }
  }

  puleLog("connected to %s:%d", ci.hostname.contents, ci.port);
  auto client = pint::Client {
    .host = host,
    .peer = peer,
    .channels = {},
    .channelLength = ci.channelLength,
    .receivePacket = ci.receivePacket,
    .receiveDisconnect = ci.receiveDisconnect,
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

void puleNetClientPoll(
  PuleNetClient const puClient
) {
  pint::Client * client = pint::clients.at(puClient.id);

  ENetEvent event = {};
  if (enet_host_service(client->host, &event, 0) <= 0) {
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
      client->receivePacket(puClient, packet, client->userData);
      enet_packet_destroy(event.packet);
      break;
    }
    case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
    case ENET_EVENT_TYPE_DISCONNECT: {
      puleLog("disconnected from host");
      client->receiveDisconnect(puClient, client->userData);
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
