/* auto generated file net */
#pragma once
#include "core.h"

#include "net.h"
#include "array.hpp"
#include "error.hpp"
#include "stream.hpp"
#include "string.hpp"

namespace pule {
struct NetHost {
  PuleNetHost _handle;
  inline operator PuleNetHost() const {
    return _handle;
  }
  inline void destroy() {
    return puleNetHostDestroy(this->_handle);
  }
  inline uint64_t pollConnection() {
    return puleNetHostPollConnection(this->_handle);
  }
  inline uint32_t poll(uint32_t maxPacketsToReceive, struct PuleNetPacketReceive * packets, struct PuleError * error) {
    return puleNetHostPoll(this->_handle, maxPacketsToReceive, packets, error);
  }
  inline void sendPacket(uint64_t clientUuid, PuleNetChannelType channel, uint32_t lane, PuleBufferView packet, struct PuleError * error) {
    return puleNetHostSendPacket(this->_handle, clientUuid, channel, lane, packet, error);
  }
  inline void broadcastPacket(PuleNetChannelType channel, uint32_t lane, PuleBufferView packet, struct PuleError * error) {
    return puleNetHostBroadcastPacket(this->_handle, channel, lane, packet, error);
  }
  inline PuleNetStreamTransferUpload uploadStream(uint64_t clientUuid, PuleStreamRead stream, uint32_t uploadStreamLane) {
    return puleNetHostUploadStream(this->_handle, clientUuid, stream, uploadStreamLane);
  }
  static inline NetHost create(PuleNetHostCreateInfo ci, struct PuleError * error) {
    return { ._handle = puleNetHostCreate(ci, error),};
  }
};
}
  inline void destroy(pule::NetHost self) {
    return puleNetHostDestroy(self._handle);
  }
  inline uint64_t pollConnection(pule::NetHost self) {
    return puleNetHostPollConnection(self._handle);
  }
  inline uint32_t poll(pule::NetHost self, uint32_t maxPacketsToReceive, struct PuleNetPacketReceive * packets, struct PuleError * error) {
    return puleNetHostPoll(self._handle, maxPacketsToReceive, packets, error);
  }
  inline void sendPacket(pule::NetHost self, uint64_t clientUuid, PuleNetChannelType channel, uint32_t lane, PuleBufferView packet, struct PuleError * error) {
    return puleNetHostSendPacket(self._handle, clientUuid, channel, lane, packet, error);
  }
  inline void broadcastPacket(pule::NetHost self, PuleNetChannelType channel, uint32_t lane, PuleBufferView packet, struct PuleError * error) {
    return puleNetHostBroadcastPacket(self._handle, channel, lane, packet, error);
  }
  inline PuleNetStreamTransferUpload uploadStream(pule::NetHost self, uint64_t clientUuid, PuleStreamRead stream, uint32_t uploadStreamLane) {
    return puleNetHostUploadStream(self._handle, clientUuid, stream, uploadStreamLane);
  }
namespace pule {
struct NetClient {
  PuleNetClient _handle;
  inline operator PuleNetClient() const {
    return _handle;
  }
  inline void destroy() {
    return puleNetClientDestroy(this->_handle);
  }
  inline bool connected() {
    return puleNetClientConnected(this->_handle);
  }
  inline uint32_t poll(uint32_t maxPacketsToReceive, struct PuleNetPacketReceive * packets, struct PuleError * error) {
    return puleNetClientPoll(this->_handle, maxPacketsToReceive, packets, error);
  }
  inline void sendPacket(PuleNetChannelType channel, uint32_t lane, PuleBufferView packet, struct PuleError * error) {
    return puleNetClientSendPacket(this->_handle, channel, lane, packet, error);
  }
  inline PuleNetStreamTransferDownload downloadStreamCheck(uint32_t packetsLength, struct PuleNetPacketReceive * packets, uint32_t uploadStreamLane, PuleStreamWrite streamWriter) {
    return puleNetClientDownloadStreamCheck(this->_handle, packetsLength, packets, uploadStreamLane, streamWriter);
  }
  inline bool downloadStreamExists(PuleNetStreamTransferDownload transfer) {
    return puleNetClientDownloadStreamExists(this->_handle, transfer);
  }
  inline bool downloadStreamContinue(PuleNetStreamTransferDownload transfer, uint32_t packetsLength, struct PuleNetPacketReceive * packets) {
    return puleNetClientDownloadStreamContinue(this->_handle, transfer, packetsLength, packets);
  }
  static inline NetClient create(PuleNetClientCreateInfo ci, struct PuleError * error) {
    return { ._handle = puleNetClientCreate(ci, error),};
  }
};
}
  inline void destroy(pule::NetClient self) {
    return puleNetClientDestroy(self._handle);
  }
  inline bool connected(pule::NetClient self) {
    return puleNetClientConnected(self._handle);
  }
  inline uint32_t poll(pule::NetClient self, uint32_t maxPacketsToReceive, struct PuleNetPacketReceive * packets, struct PuleError * error) {
    return puleNetClientPoll(self._handle, maxPacketsToReceive, packets, error);
  }
  inline void sendPacket(pule::NetClient self, PuleNetChannelType channel, uint32_t lane, PuleBufferView packet, struct PuleError * error) {
    return puleNetClientSendPacket(self._handle, channel, lane, packet, error);
  }
  inline PuleNetStreamTransferDownload downloadStreamCheck(pule::NetClient self, uint32_t packetsLength, struct PuleNetPacketReceive * packets, uint32_t uploadStreamLane, PuleStreamWrite streamWriter) {
    return puleNetClientDownloadStreamCheck(self._handle, packetsLength, packets, uploadStreamLane, streamWriter);
  }
  inline bool downloadStreamExists(pule::NetClient self, PuleNetStreamTransferDownload transfer) {
    return puleNetClientDownloadStreamExists(self._handle, transfer);
  }
  inline bool downloadStreamContinue(pule::NetClient self, PuleNetStreamTransferDownload transfer, uint32_t packetsLength, struct PuleNetPacketReceive * packets) {
    return puleNetClientDownloadStreamContinue(self._handle, transfer, packetsLength, packets);
  }
namespace pule {
using NetStreamTransferUpload = PuleNetStreamTransferUpload;
}
namespace pule {
using NetStreamTransferDownload = PuleNetStreamTransferDownload;
}
namespace pule {
struct NetAddress {
  PuleNetAddress _handle;
  inline operator PuleNetAddress() const {
    return _handle;
  }
  static inline NetAddress localhost(uint16_t port) {
    return { ._handle = puleNetAddressLocalhost(port),};
  }
};
}
namespace pule {
using NetPacketReceive = PuleNetPacketReceive;
}
namespace pule {
using NetHostCreateInfo = PuleNetHostCreateInfo;
}
namespace pule {
using NetClientCreateInfo = PuleNetClientCreateInfo;
}
namespace pule {
inline char const * toStr(PuleErrorNet const e) {
  switch (e) {
    case PuleErrorNet_none: return "none";
    case PuleErrorNet_initialize: return "initialize";
    case PuleErrorNet_hostCreate: return "hostCreate";
    case PuleErrorNet_poll: return "poll";
    case PuleErrorNet_clientCreate: return "clientCreate";
    case PuleErrorNet_clientConnect: return "clientConnect";
    case PuleErrorNet_clientDisconnect: return "clientDisconnect";
    case PuleErrorNet_clientSend: return "clientSend";
    case PuleErrorNet_packetSend: return "packetSend";
    default: return "N/A";
  }
}
inline char const * toStr(PuleNetChannelType const e) {
  switch (e) {
    case PuleNetChannelType_reliable: return "reliable";
    case PuleNetChannelType_unreliable: return "unreliable";
    case PuleNetChannelType_unsequenced: return "unsequenced";
    default: return "N/A";
  }
}
}
