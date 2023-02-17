#include <string>

#include <pulchritude-plugin/engine.h>
#include <pulchritude-plugin/plugin.h>

struct DebugLayerSlice {
  DebugLayerSlice();
  DebugLayerSlice(DebugLayerSlice &&);
  DebugLayerSlice(DebugLayerSlice const &) = delete;
  DebugLayerSlice operator =(DebugLayerSlice const &) = delete;
  ~DebugLayerSlice();
};

void debugLayerFnEntryImpl(std::string const & label);
#define debugLayerFnEntry() \
  debugLayerFnEntryImpl(__PRETTY_FUNCTION__); \
  auto slice__ = DebugLayerSlice();

void debugLayerError(char const * const formatCStr, ...);
void debugLayerPrint(char const * const formatCStr, ...);
void debugLayerWarn(char const * const formatCStr, ...);
