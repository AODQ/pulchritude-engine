#include <pulchritude/net.h>

#include <unordered_map>

#if defined(PULCHRITUDE_FEATURE_NET)

#include <steam/steamnetworkingsockets.h>
#include <steam/steamnetworkingsockets_flat.h>

#include <string>
#include <vector>

namespace pint {

struct {
  SteamNetworkingIPAddr address;
  SteamNetworkingMicroseconds lastPollTime;

  HSteamListenSocket listenSocket;
  HSteamNetPollGroup pollGroup;
  ISteamNetworkingSockets * interface;
  std::unordered_map<uint64_t, HSteamNetConnection> connections;
  ISteamNetworkingUtils * utilsInterface;

  std::vector<std::vector<uint8_t>> receiveBuffers;
  std::vector<uint64_t> receiveClientUuids;

  uint32_t outgoingLanes;
} host;

struct Client {
  SteamNetworkingIPAddr hostAddress;
  SteamNetworkingMicroseconds lastPollTime;

  HSteamNetConnection connection;
  ISteamNetworkingSockets * interface;
  ISteamNetworkingUtils * utilsInterface;

  std::vector<std::vector<uint8_t>> receiveBuffers;

  uint32_t outgoingLanes;
} client;

bool hasInit = false;

void connectionStatusChangedHostCallback(
  SteamNetConnectionStatusChangedCallback_t * const info
) {
  switch (info->m_info.m_eState) {
    default: break;
    case k_ESteamNetworkingConnectionState_None: break;
    case k_ESteamNetworkingConnectionState_ClosedByPeer:
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
      puleLog("client disconnected: %d", info->m_hConn);
      // ignore if not previously connected
      if (info->m_eOldState != k_ESteamNetworkingConnectionState_Connected) {
        return;
      }
      PULE_assert(
        host.connections.find(info->m_hConn) != host.connections.end()
      );
      host.connections.erase(info->m_hConn);
    break;
    case k_ESteamNetworkingConnectionState_Connecting:
      puleLog("client connecting: %d", info->m_hConn);
      // ignore if not previously connected
      PULE_assert(
        host.connections.find(info->m_hConn) == host.connections.end()
      );
      PULE_assert(info->m_hConn != k_HSteamNetConnection_Invalid);
      host.connections.emplace(info->m_hConn, info->m_hConn);
      puleLog("[host] client connected: %d", info->m_hConn);

      if (host.interface->AcceptConnection(info->m_hConn) != k_EResultOK) {
        puleLogError("[host] AcceptConnection failed");
        host.interface->CloseConnection(info->m_hConn, 0, nullptr, false);
      }
      if (
        !host.interface->SetConnectionPollGroup(info->m_hConn, host.pollGroup)
      ) {
        puleLogError("[host] SetConnectionPollGroup failed");
        host.interface->CloseConnection(info->m_hConn, 0, nullptr, false);
        break;
      }
    break;
    case k_ESteamNetworkingConnectionState_Connected:
      puleLog("client connected: %d", info->m_hConn);
      host.receiveClientUuids.push_back(info->m_hConn);
      // create connection lanes
      host.interface->ConfigureConnectionLanes(
        info->m_hConn,
        host.outgoingLanes,
        nullptr, // all same priority for now (whatever)
        nullptr // all same weights
      );
    break;
  }
}

void connectionStatusChangedClientCallback(
  SteamNetConnectionStatusChangedCallback_t * const info
) {
  switch (info->m_info.m_eState) {
    default: break;
    case k_ESteamNetworkingConnectionState_None: break;
    case k_ESteamNetworkingConnectionState_ClosedByPeer:
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
      puleLog("host disconnected\n");
      client.interface->CloseConnection(info->m_hConn, 0, nullptr, false);
      client.connection = k_HSteamNetConnection_Invalid;
      client.interface = nullptr;
    break;
    case k_ESteamNetworkingConnectionState_Connecting:
    break;
    case k_ESteamNetworkingConnectionState_Connected:
      puleLog("connecting to host\n");
      // create connection lanes
      client.interface->ConfigureConnectionLanes(
        info->m_hConn,
        client.outgoingLanes,
        nullptr, // all same priority for now (whatever)
        nullptr // all same weights
      );
    break;
  }
}

uint32_t netPoll(
  std::vector<std::vector<uint8_t>> & receiveBuffers,
  PuleNetPacketReceive * const packets,
  int32_t const incomingMessageCount,
  ISteamNetworkingMessage * * const incomingMessages,
  PuleError * const error
) {
  if (incomingMessageCount == 0) { return 0; }
  if (incomingMessageCount < 0) {
    PULE_error(PuleErrorNet_poll, "ReceiveMessagesOnPollGroup failed");
    return 0;
  }
  receiveBuffers.resize(incomingMessageCount);
  for (int32_t it = 0; it < incomingMessageCount; ++ it) {
    auto & msg = *incomingMessages[it];
    if (msg.m_conn == k_HSteamNetConnection_Invalid) {
      PULE_error(PuleErrorNet_poll, "invalid connection");
      continue;
    }
    // copy packet data into our own temporary buffer that lasts until next poll
    switch (msg.m_nFlags) {
      default:
      case k_nSteamNetworkingSend_Reliable:
        packets[it].channel = PuleNetChannelType_reliable;
      break;
      case k_nSteamNetworkingSend_Unreliable:
      case k_nSteamNetworkingSend_UnreliableNoNagle:
        packets[it].channel = PuleNetChannelType_reliable;
      break;

    }
    receiveBuffers[it].resize(msg.m_cbSize);
    memcpy(
      receiveBuffers[it].data(),
      msg.m_pData,
      msg.m_cbSize
    );
    packets[it].data = {
      .data = receiveBuffers[it].data(),
      .byteLength = (uint64_t)msg.m_cbSize,
    };
    packets[it].clientUuid = msg.m_conn;
    packets[it].lane = msg.m_idxLane;

    msg.Release();
  }
  return incomingMessageCount;

}

void netSendPacket(
  HSteamNetConnection connection,
  ISteamNetworkingSockets  * interface,
  ISteamNetworkingUtils * utilsInterface,
  PuleNetChannelType const channel,
  int32_t const lane,
  PuleBufferView const packet,
  PuleError * const error
) {
  SteamNetworkingMessage_t * message = (
    utilsInterface->AllocateMessage(packet.byteLength)
  );
  memcpy(message->m_pData, packet.data, packet.byteLength);
  switch (channel) {
    case PuleNetChannelType_reliable:
      message->m_nFlags = k_nSteamNetworkingSend_Reliable;
    break;
    case PuleNetChannelType_unreliable:
      message->m_nFlags = k_nSteamNetworkingSend_Unreliable;
    break;
    case PuleNetChannelType_unsequenced:
      PULE_assert(false && "unsequenced not implemented");
    break;
  }
  message->m_conn = connection;
  message->m_idxLane = lane;
  int64 errorCode;
  interface->SendMessages(1, &message, &errorCode);
  if (!errorCode) {
    PULE_error(PuleErrorNet_packetSend, "failed to send packet");
  }
}

} // namespace pint

extern "C" {

PuleNetAddress puleNetAddressLocalhost(
  uint16_t const port
) {
  PuleNetAddress address;
  address.port = port;
  address.address[0] = 0;
  address.address[1] = 0;
  address.address[2] = 0;
  address.address[3] = 0;
  address.address[4] = 0;
  address.address[5] = 0;
  address.address[6] = 0;
  address.address[7] = 0;
  address.address[8] = 0;
  address.address[9] = 0;
  address.address[10] = 0;
  address.address[11] = 0;
  address.address[12] = 0;
  address.address[13] = 0;
  address.address[14] = 0;
  address.address[15] = 1;
  return address;
}

// -- host ---------------------------------------------------------------------

PuleNetHost puleNetHostCreate(
  PuleNetHostCreateInfo const ci,
  PuleError * const error
) {
  PULE_assert(!pint::hasInit);
  puleLog("host init");
  if (!pint::hasInit) {
    pint::hasInit = true;
    SteamDatagramErrMsg errMsg;
    if (!GameNetworkingSockets_Init(nullptr, errMsg)) {
      PULE_error(PuleErrorNet_initialize, "%s", errMsg);
      return { .id = 0, };
    }
  }

  auto & host = pint::host;
  host.interface = SteamNetworkingSockets();
  host.utilsInterface = SteamNetworkingUtils();
  host.address.Clear();
  host.address.SetIPv6LocalHost(ci.address.port);

  SteamNetworkingConfigValue_t configValues[1];
  configValues[0].SetPtr(
    k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
    (void*)pint::connectionStatusChangedHostCallback
  );

  host.listenSocket = (
    host.interface->CreateListenSocketIP(host.address, 1, configValues)
  );
  if (host.listenSocket == k_HSteamListenSocket_Invalid) {
    PULE_error(PuleErrorNet_hostCreate, "CreateListenSocketIP failed");
    return { .id = 0, };
  }

  host.pollGroup = host.interface->CreatePollGroup();
  if (host.pollGroup == k_HSteamNetPollGroup_Invalid) {
    PULE_error(PuleErrorNet_hostCreate, "CreatePollGroup failed");
    return { .id = 0, };
  }

  host.outgoingLanes = ci.outgoingLaneCount;

  puleLog("host init success");
  return { .id = 1, };
}

uint64_t puleNetHostPollConnection(PuleNetHost const puHost) {
  PULE_assert(pint::hasInit && puHost.id != 0);
  auto & host = pint::host;
  host.interface->RunCallbacks();
  if (host.receiveClientUuids.size() == 0) { return 0; }
  uint64_t const clientUuid = host.receiveClientUuids.back();
  host.receiveClientUuids.pop_back();
  return clientUuid;
}

uint32_t puleNetHostPoll(
  PuleNetHost const puHost,
  uint32_t const maxPacketsToReceive,
  PuleNetPacketReceive * const packets,
  PuleError * const error
) {
  PULE_assert(pint::hasInit && puHost.id != 0);
  auto & host = pint::host;
  std::vector<ISteamNetworkingMessage *> incomingMessages;
  incomingMessages.resize(maxPacketsToReceive);
  int32_t const incomingMessageCount = (
    host.interface->ReceiveMessagesOnPollGroup(
      host.pollGroup, &incomingMessages[0], maxPacketsToReceive
    )
  );
  return (
    pint::netPoll(
      host.receiveBuffers,
      packets,
      incomingMessageCount, incomingMessages.data(),
      error
    )
  );
}

void puleNetHostSendPacket(
  PuleNetHost const puHost,
  uint64_t clientUuid,
  PuleNetChannelType const channel,
  uint32_t const lane,
  PuleBufferView const packet,
  PuleError * const error
) {
  PULE_assert(pint::hasInit && puHost.id != 0);
  auto & host = pint::host;
  pint::netSendPacket(
    host.connections.at(clientUuid),
    host.interface,
    host.utilsInterface,
    channel,
    lane,
    packet,
    error
  );
}

void puleNetHostBroadcastPacket(
  PuleNetHost const puHost,
  PuleNetChannelType const channel,
  uint32_t const lane,
  PuleBufferView const packet,
  PuleError * const error
) {
  for (auto const & [clientUuid, _] : pint::host.connections) {
    puleNetHostSendPacket(puHost, clientUuid, channel, lane, packet, error);
  }
}

void puleNetHostDestroy(
  PuleNetHost const host
) {
  //TODO SteamDatagramErrMsg errMsg;
  //GameNetworkingSockets_Kill(errMsg);
}

// -- client -------------------------------------------------------------------

PuleNetClient puleNetClientCreate(
  PuleNetClientCreateInfo const ci,
  PuleError * const error
) {
  PULE_assert(!pint::hasInit);
  if (!pint::hasInit) {
    pint::hasInit = true;
    SteamDatagramErrMsg errMsg;
    if (!GameNetworkingSockets_Init(nullptr, errMsg)) {
      PULE_error(PuleErrorNet_initialize, "%s", errMsg);
      return { .id = 0, };
    }
  }

  auto & client = pint::client;
  client.hostAddress.Clear();
  client.hostAddress.SetIPv6LocalHost(ci.port);

  SteamNetworkingConfigValue_t configValues[1];
  configValues[0].SetPtr(
    k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
    (void*)pint::connectionStatusChangedClientCallback
  );

  client.interface = SteamNetworkingSockets();
  client.utilsInterface = SteamNetworkingUtils();
  client.connection = (
    client.interface->ConnectByIPAddress(client.hostAddress, 1, configValues)
  );
  if (client.connection == k_HSteamNetConnection_Invalid) {
    PULE_error(PuleErrorNet_clientCreate, "ConnectByIPAddress failed");
    return { .id = 0, };
  }
  puleLog("connected client");

  client.outgoingLanes = ci.outgoingLaneCount;

  return { .id = 1, };
}

void puleNetClientDestroy(
  PuleNetClient const client
) {
  //TODO SteamDatagramErrMsg errMsg;
  //GameNetworkingSockets_Kill(errMsg);
}

bool puleNetClientConnected(PuleNetClient const client) {
  PULE_assert(pint::hasInit && client.id != 0);
  if (pint::client.interface == nullptr) { return false; }
  pint::client.interface->RunCallbacks();
  return pint::client.connection != k_HSteamNetConnection_Invalid;
}

uint32_t puleNetClientPoll(
  PuleNetClient const puClient,
  uint32_t const maxPacketsToReceive,
  PuleNetPacketReceive * const packets,
  PuleError * const error
) {
  PULE_assert(pint::hasInit && puClient.id != 0);
  auto & client = pint::client;
  std::vector<ISteamNetworkingMessage *> incomingMessages;
  incomingMessages.resize(maxPacketsToReceive);
  int32_t const incomingMessageCount = (
    client.interface->ReceiveMessagesOnConnection(
      client.connection, &incomingMessages[0], maxPacketsToReceive
    )
  );
  return (
    pint::netPoll(
      client.receiveBuffers,
      packets,
      incomingMessageCount, incomingMessages.data(),
      error
    )
  );
}

void puleNetClientSendPacket(
  PuleNetClient const puClient,
  PuleNetChannelType const channel,
  uint32_t const lane,
  PuleBufferView const packet,
  PuleError * const error
) {
  auto & client = pint::client;
  pint::netSendPacket(
    client.connection,
    client.interface,
    client.utilsInterface,
    channel, lane, packet, error
  );
}

} // extern "C"

#endif // defined(PULCHRITUDE_FEATURE_NET)


// --- the elite and luxurious stream module ---

namespace pintstream {

struct DownloadStream {
  uint32_t streamLane;
  PuleStreamWrite streamWriter;
};

std::string requestString = "__PULE_UPLOAD_REQUEST__";

PuleStreamWrite streamWriter;
bool transferInProgress = false;
uint32_t transferLane;

} // namespace

extern "C" {

PuleNetStreamTransferUpload puleNetHostUploadStream(
  PuleNetHost const host,
  uint64_t clientUuid,
  PuleStreamRead const stream,
  uint32_t uploadStreamLane
) {
  PuleError err = puleError();
  if (puleStreamReadIsDone(stream)) { // no work to do I suppose
    return { 0 };
  }
  puleNetHostSendPacket(
    host, clientUuid, PuleNetChannelType_reliable, uploadStreamLane,
    PuleBufferView {
      .data = (uint8_t const *)pintstream::requestString.data(),
      .byteLength = pintstream::requestString.size(),
    },
    &err
  );
  // blocking for now
  bool isFinished = false;
  while (!isFinished) {
    PULE_assert(
      !puleStreamReadIsDone(stream) && "pre-emptively consumed stream"
    );
    uint8_t intermediateBuffer[32];
    size_t it;
    // { isDone : 0, user-data : 1..32 }
    for (it = 1; it < 32; ++ it) {
      intermediateBuffer[it] = puleStreamReadByte(stream);
      if (puleStreamReadIsDone(stream)) {
        isFinished = true;
        break;
      }
    }
    intermediateBuffer[0] = isFinished?1:0; // reserve first byte for the 'last'
    puleLog("Sent %d bytes | isFinished %d", it, isFinished);
    puleNetHostSendPacket(
      host,
      clientUuid,
      PuleNetChannelType_reliable,
      uploadStreamLane,
      PuleBufferView { .data = intermediateBuffer, .byteLength = it, },
      &err
    );
    if (puleErrorConsume(&err)) { return { 0 }; }
  }
  return { 0 }; // this blocks, nothing to return
}

PuleNetStreamTransferDownload puleNetClientDownloadStreamCheck(
  PuleNetClient const client,
  uint32_t const packetsLength,
  PuleNetPacketReceive * const packets,
  uint32_t uploadStreamLane,
  PuleStreamWrite const streamWriter
) {
  bool hasData = true;
  PuleNetStreamTransferDownload dl = { 0 };
  size_t packetIt = 0;
  for (packetIt = 0; packetIt < packetsLength; ++ packetIt) {
    PuleNetPacketReceive & packet = packets[packetIt];
    if (packet.lane != uploadStreamLane) { continue; }
    puleLog("data: %s", packet.data.data);
    if (
         packet.data.byteLength == pintstream::requestString.size()
      && pintstream::requestString == (char *)packet.data.data
    ) {
      dl.id = 1;
      pintstream::transferInProgress = true;
      pintstream::streamWriter = streamWriter;
      pintstream::transferLane = uploadStreamLane;
      break;
    }
  }
  // return early if no transfer initiated
  if (dl.id == 0) { return dl; }
  // iterate through remaining packets in case any have more data
  puleLog("iterate thru");
  packetIt += 1; // iterate past the request packet
  puleNetClientDownloadStreamContinue(
    client, dl, packetsLength-packetIt, packets + packetIt
  );
  puleLog("iterate thru finished");
  return dl;
}

bool puleNetClientDownloadStreamExists(
  PuleNetClient const client,
  PuleNetStreamTransferDownload const transfer
) {
  puleLog("transfer id: %d", transfer.id);
  puleLog("transferInProgress: %d", pintstream::transferInProgress);
  return transfer.id > 0 && pintstream::transferInProgress;
}

bool puleNetClientDownloadStreamContinue(
  PuleNetClient const client,
  PuleNetStreamTransferDownload const transfer,
  uint32_t const packetsLength,
  PuleNetPacketReceive * const packets
) {
  if (!puleNetClientDownloadStreamExists(client, transfer)) { return false; }
  puleLog("packets length: %d", packetsLength);
  for (size_t packetIt = 0; packetIt < packetsLength; ++ packetIt) {
    PuleNetPacketReceive & packet = packets[packetIt];
    if (packet.lane != pintstream::transferLane) { continue; }
    // minimum 1 byte for length
    PULE_assert(packet.data.byteLength > 0);
    puleStreamWriteBytes(
      pintstream::streamWriter, packet.data.data + 1, packet.data.byteLength-1
    );
    puleLog("finished? %d", packet.data.data[0]);
    if (packet.data.data[0]) { // is finished?
      pintstream::transferInProgress = false;
      break;
    }
  }
  puleStreamWriteFlush(pintstream::streamWriter);
  return !pintstream::transferInProgress;
}

} // extern "C"
