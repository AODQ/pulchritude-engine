#include <pulchritude/plugin.h>

#include <pulchritude/file.h>
#include <pulchritude/log.h>
#include <pulchritude/string.h>

#include <unordered_map>

namespace {
struct InternalPluginPayload {
  std::unordered_map<size_t, void *> hashToData;
};

std::unordered_map<uint64_t, InternalPluginPayload> internalPayloads;
uint64_t internalPayloadCount = 0;
} // namespace

extern "C" {

PulePluginPayload pulePluginPayloadCreate(PuleAllocator const allocator) {
  (void)allocator; // TODO
  ::internalPayloadCount += 1;
  ::internalPayloads.emplace(::internalPayloadCount, InternalPluginPayload{});
  return {.id = ::internalPayloadCount,};
}

void pulePluginPayloadDestroy(PulePluginPayload const payload) {
  if (payload.id == 0) {
    return;
  }
  ::internalPayloads.erase(payload.id);
}

void * pulePluginPayloadFetch(
  PulePluginPayload const payload,
  PuleStringView const handle
) {
  auto const payloadPtr = ::internalPayloads.find(payload.id);
  if (payloadPtr == ::internalPayloads.end()) {
    puleLogError("PulePluginPayload value '%zu' does not exist", payload.id);
    return nullptr;
  }

  if (handle.contents == nullptr || handle.len == 0) {
    puleLogError("Using null/empty handle to fetch from payload");
    return nullptr;
  }

  uint64_t const hash = puleStringViewHash(handle);
  auto const dataPtr = payloadPtr->second.hashToData.find(hash);
  if (dataPtr == payloadPtr->second.hashToData.end()) {
    puleLogError(
      "handle '%s' does not exist in payload %d",
      handle.contents, payload.id
    );
    return nullptr;
  }
  return dataPtr->second;
}
uint64_t pulePluginPayloadFetchU64(
  PulePluginPayload const payload,
  PuleStringView const handle
) {
  return reinterpret_cast<uint64_t>(pulePluginPayloadFetch(payload, handle));
}

void pulePluginPayloadStore(
  PulePluginPayload const payload,
  PuleStringView const handle,
  void * const data
) {
  auto const payloadPtr = ::internalPayloads.find(payload.id);
  if (payloadPtr == ::internalPayloads.end()) {
    puleLogError("PulePluginPayload value '%zu' does not exist", payload.id);
    return;
  }

  uint64_t const hash = puleStringViewHash(handle);
  payloadPtr->second.hashToData.emplace(hash, data);
}

void pulePluginPayloadStoreU64(
  PulePluginPayload const payload,
  PuleStringView const handle,
  uint64_t const data
) {
  pulePluginPayloadStore(payload, handle, reinterpret_cast<void *>(data));
}

void pulePluginPayloadRemove(
  PulePluginPayload const payload,
  PuleStringView const handle
) {
  auto const payloadPtr = ::internalPayloads.find(payload.id);
  if (payloadPtr == ::internalPayloads.end()) {
    puleLogError("PulePluginPayload value '%zu' does not exist", payload.id);
    return;
  }

  uint64_t const hash = puleStringViewHash(handle);
  payloadPtr->second.hashToData.erase(hash);
}

} // extern C
