#include "../include/debug.hpp"

#include <pulchritude-allocator/allocator.h>
#include <pulchritude-string/string.h>

#include <cassert>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace {

std::thread::id const mainThreadId = std::this_thread::get_id();
bool isMainThread() {
  return mainThreadId == std::this_thread::get_id();
}

struct DebugLayerSliceInternal {
  std::string label;
};

struct DebugLayer {
  std::vector<DebugLayerSliceInternal> functions;
  std::string layerTab;
};

DebugLayer & debugLayer() {
  static std::mutex mutex;
  std::lock_guard<std::mutex> lock(mutex);
  static std::unordered_map<std::thread::id, DebugLayer> layer;
  return layer[std::this_thread::get_id()];
}
} // namespace


DebugLayerSlice::DebugLayerSlice() {
}

DebugLayerSlice::DebugLayerSlice(DebugLayerSlice &&) {
}

DebugLayerSlice::~DebugLayerSlice() {
  auto & layer = ::debugLayer();
  assert(layer.functions.size() > 0);
  layer.functions.pop_back();
  assert(layer.layerTab.size() >= 3);
  layer.layerTab.resize(layer.layerTab.size()-3);
}

void debugLayerFnEntryImpl(std::string const & label) {
  auto & layer = ::debugLayer();
  DebugLayerSliceInternal slice;
  slice.label = label;
  layer.functions.emplace_back(label);

  /* puleLogRaw("%s| %s:\n", layer.layerTab.c_str(), label.c_str()); */
  layer.layerTab += "|  ";
}

void debugLayerError(char const * const formatCStr, ...) {
  // TODO write to file instead of not writing anything when not main thread
  if (!isMainThread()) { return; }
  va_list args;
  va_start(args, formatCStr);
  auto str = puleStringFormatVargs(puleAllocateDefault(), formatCStr, args);
  puleLogRaw("[ERR] %s%s\n", debugLayer().layerTab.c_str(), str);
  puleStringDestroy(str);
}

void debugLayerWarn(char const * const formatCStr, ...) {
  if (!isMainThread()) { return; }
  va_list args;
  va_start(args, formatCStr);
  auto str = puleStringFormatVargs(puleAllocateDefault(), formatCStr, args);
  puleLogRaw("[WRN] %s%s\n", debugLayer().layerTab.c_str(), str);
  puleStringDestroy(str);
}

void debugLayerPrint(char const * const formatCStr, ...) {
  if (!isMainThread()) { return; }
  va_list args;
  va_start(args, formatCStr);
  auto str = puleStringFormatVargs(puleAllocateDefault(), formatCStr, args);
  puleLogRaw("[NOR] %s%s\n", debugLayer().layerTab.c_str(), str);
  puleStringDestroy(str);
}
