#include <pulchritude-gfx-debug/gfx-debug.h>

namespace {

struct Shape {
  PuleGfxCommandList commandList;
  PuleGfxShaderModule shaderModule;
  PuleGfxGpuBuffer mappedBuffer;
  PuleGfxPipeline pipeline;
};

struct Context {
  PuleGfxCommandList lineCommandList;
  PuleGfxShaderModule shaderModule;
};
Context ctx;

} // namespace

extern "C" {

void puleGfxDebugInitialize() {
  
}

} // C
