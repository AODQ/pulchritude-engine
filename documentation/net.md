# net

## structs
### PuleNetAddress
```c
struct {
  address : uint8_t [16];
  port : uint16_t;
};
```
### PuleNetPacketReceive
```c
struct {
  data : PuleBufferView;
  clientUuid : uint64_t;
  lane : uint32_t;
  channel : uint8_t;
};
```
### PuleNetHostCreateInfo
```c
struct {
  address : PuleNetAddress;
  connectionLimit : uint32_t;
  outgoingLaneCount : uint32_t;
};
```
### PuleNetClientCreateInfo
```c
struct {
  hostname : PuleStringView;
  port : uint16_t;
  outgoingLaneCount : uint32_t;
};
```

## enums
### PuleErrorNet
```c
enum {
  none,
  initialize,
  hostCreate,
  poll,
  clientCreate,
  clientConnect,
  clientDisconnect,
  clientSend,
  packetSend,
}
```
### PuleNetChannelType
```c
enum {
  reliable,
  unreliable,
  unsequenced,
}
```

## entities
### PuleNetHost
### PuleNetClient
### PuleNetStreamTransferUpload
### PuleNetStreamTransferDownload

## functions
### puleNetAddressLocalhost
```c
puleNetAddressLocalhost(
  port : uint16_t
) PuleNetAddress;
```
### puleNetHostCreate
```c
puleNetHostCreate(
  ci : PuleNetHostCreateInfo,
  error : PuleError ptr
) PuleNetHost;
```
### puleNetHostDestroy
```c
puleNetHostDestroy(
  host : PuleNetHost
) void;
```
### puleNetHostPollConnection
 returns client uuid, 0 if no connection 
```c
puleNetHostPollConnection(
  host : PuleNetHost
) uint64_t;
```
### puleNetHostPoll
```c
puleNetHostPoll(
  host : PuleNetHost,
  maxPacketsToReceive : uint32_t,
  packets : PuleNetPacketReceive ptr,
  error : PuleError ptr
) uint32_t;
```
### puleNetHostSendPacket
```c
puleNetHostSendPacket(
  host : PuleNetHost,
  clientUuid : uint64_t,
  channel : PuleNetChannelType,
  lane : uint32_t,
  packet : PuleBufferView,
  error : PuleError ptr
) void;
```
### puleNetHostBroadcastPacket
```c
puleNetHostBroadcastPacket(
  host : PuleNetHost,
  channel : PuleNetChannelType,
  lane : uint32_t,
  packet : PuleBufferView,
  error : PuleError ptr
) void;
```
### puleNetClientCreate
```c
puleNetClientCreate(
  ci : PuleNetClientCreateInfo,
  error : PuleError ptr
) PuleNetClient;
```
### puleNetClientDestroy
```c
puleNetClientDestroy(
  client : PuleNetClient
) void;
```
### puleNetClientConnected
```c
puleNetClientConnected(
  client : PuleNetClient
) bool;
```
### puleNetClientPoll
```c
puleNetClientPoll(
  client : PuleNetClient,
  maxPacketsToReceive : uint32_t,
  packets : PuleNetPacketReceive ptr,
  error : PuleError ptr
) uint32_t;
```
### puleNetClientSendPacket
```c
puleNetClientSendPacket(
  client : PuleNetClient,
  channel : PuleNetChannelType,
  lane : uint32_t,
  packet : PuleBufferView,
  error : PuleError ptr
) void;
```
### puleNetHostUploadStream

You must not send packets on the specified lane until the stream is finished
Only one transfer per-stream can be done at a time
TODO right now this blocks, needs progress callback I suppose

```c
puleNetHostUploadStream(
  host : PuleNetHost,
  clientUuid : uint64_t,
  stream : PuleStreamRead,
  uploadStreamLane : uint32_t
) PuleNetStreamTransferUpload;
```
### puleNetClientDownloadStreamCheck

to support this on client-side, you need to pass in all packets received, and
the stream transfer will communicate on the specified dedicated lane

```c
puleNetClientDownloadStreamCheck(
  client : PuleNetClient,
  packetsLength : uint32_t,
  packets : PuleNetPacketReceive ptr,
  uploadStreamLane : uint32_t,
  streamWriter : PuleStreamWrite
) PuleNetStreamTransferDownload;
```
### puleNetClientDownloadStreamExists
 returns true if the stream exists 
```c
puleNetClientDownloadStreamExists(
  client : PuleNetClient,
  transfer : PuleNetStreamTransferDownload
) bool;
```
### puleNetClientDownloadStreamContinue

  returns true if more data to consume

```c
puleNetClientDownloadStreamContinue(
  client : PuleNetClient,
  transfer : PuleNetStreamTransferDownload,
  packetsLength : uint32_t,
  packets : PuleNetPacketReceive ptr
) bool;
```
