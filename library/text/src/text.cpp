#include <pulchritude-text/text.h>

#include <pulchritude-asset/shader-module.h>
#include <pulchritude-error/error.h>
#include <pulchritude-gpu/module.h>
#include <pulchritude-log/log.h>

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

namespace pint {

struct TextRenderer {
  PuleAssetFont font;
  PuleTextType type;
  std::unordered_map<size_t, PuleGpuImage> glyphImages;
};

bool initialized = false;
std::array<PuleAssetShaderModule, PuleTextTypeSize> shaderModules;
std::array<PuleGpuPipeline, PuleTextTypeSize> pipelines;

pule::ResourceContainer<pint::TextRenderer> textRenderers;

void initialize() {
  if (!pint::initialized) {
    pint::initialized = true;
  }
  pint::shaderModules[PuleTextType_bitmap] = (
    puleAssetShaderModuleCreateFromPaths(
      puleCStr("pule-text-module"),
      puleCStr("assets/text-bitmap.vert.spv"),
      puleCStr("assets/text-bitmap.frag.spv")
    )
  );
  auto const bitmapLayoutDescriptorSet = PuleGpuPipelineLayoutDescriptorSet {
    .bufferUniformBindings = {},
    .bufferStorageBindings = {},
    .attributeBindings = {},
    .attributeBufferBindings = {},
    .textureBindings = {
      PuleGpuDescriptorStage_fragment,
    },
  };
  PuleGpuPipelineLayoutPushConstant  bitmapPushConstants[] = {
    PuleGpuPipelineLayoutPushConstant {
      .stage = PuleGpuDescriptorStage_vertex,
      .byteLength = sizeof(PuleF32m44),
      .byteOffset = 0u,
    },
    PuleGpuPipelineLayoutPushConstant {
      .stage = PuleGpuDescriptorStage_fragment,
      .byteLength = sizeof(PuleF32v4),
      .byteOffset = 0u,
    },
  };
  auto const bitmapPipeline = (
    PuleGpuPipelineCreateInfo {
      .shaderModule = (
        puleAssetShaderModuleHandle(pint::shaderModules[PuleTextType_bitmap])
      ),
      .layoutDescriptorSet = &bitmapLayoutDescriptorSet,
      .layoutPushConstants = &bitmapPushConstants[0],
      .layoutPushConstantsCount = 2u,
      .config = {
        .depthTestEnabled = true,
        .blendEnabled = true,
        .scissorTestEnabled = false,
        .viewportMin = {0, 0},
        .viewportMax = {800, 600},
        .scissorMin = {0, 0},
        .scissorMax = {800, 600},
      },
    }
  );
  PuleError err;
  pint::pipelines[PuleTextType_bitmap] = (
    puleGpuPipelineCreate(&bitmapPipeline, &err)
  );
  if (puleErrorConsume(&err)) {
    puleLogError("Failed to create bitmap pipeline");
  }
}

void createGlyphImage(
  pint::TextRenderer & textRenderer,
  size_t const fontScale
) {
  char const glyphArray[] =
    "1234567890!@#$%^&*()-+=_\\/\"';:[]{}()<>|~`., "
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  ;
  uint32_t const glyphCount = PULE_arraySize(glyphArray)-1; // remove \0
  std::vector<uint8_t> glyphsetData;
  uint32_t const fontWidth = static_cast<uint32_t>(fontScale * 0.5f);
  uint32_t const fontHeight = static_cast<uint32_t>(fontScale);
  uint32_t const rows = 16u;
  uint32_t const cols = glyphCount / rows + (glyphCount % rows != 0);
  uint32_t const imageWidth = cols * fontWidth;
  uint32_t const imageHeight = rows * fontHeight;
  glyphsetData.resize(imageWidth * imageHeight);
  for (size_t glyphIt = 0; glyphArray[glyphIt] != '\0'; ++ glyphIt) {
    PuleError err;
    // render to temporary buffer
    std::vector<uint8_t> tempBuffer;
    tempBuffer.resize(fontWidth * fontScale);
    puleAssetFontRenderToU8Buffer(
      PuleAssetFontRenderInfo {
        .font = textRenderer.font,
        .fontScale = PuleF32v2 {
          .x = (float)(fontWidth), .y = (float)fontHeight,
        },
        .fontOffset = {0.f, 0.f},
        .renderFlippedY = false,
        .glyphCodepoint = static_cast<uint32_t>(glyphArray[glyphIt]),
        .destinationBuffer = {
          .data = tempBuffer.data(),
          .byteLength = tempBuffer.size(),
        },
        .destinationBufferDim = { // TODO deal with the integer artefacts prior
          .x = fontWidth,
          .y = fontHeight,
        },
      },
      &err
    );
    // copy to glyphset
    for (size_t itx = 0; itx < fontWidth; ++ itx)
    for (size_t ity = 0; ity < fontHeight; ++ ity) {
      uint32_t pixelRow = (glyphIt%rows)*fontWidth + itx;
      uint32_t pixelCol = ity*imageWidth;
      glyphsetData[pixelRow + pixelCol] = tempBuffer[itx + ity*fontWidth];
    }
  }
  //puleAssetFontRenderToU8Buffer(
  //  PuleAssetFontRenderInfo {
  //    .font = textRenderer.font,
  //    .fontScale = PuleF32v2 { .x = fontScale*0.5f, .y = fontScale*1.0f, },
  //    .fontOffset = {0.f, 0.f},
  //    .renderFlippedY = false,
  //    .glyphCodepoint = 0,
  //    .destinationBuffer = {},
  //    .destinationBufferDim = {},
  //  },
  //  &err
  //);
}


} // namespace pint

PuleTextRenderer puleTextRendererCreate(
  PuleAssetFont const font,
  PuleTextType const type
) {
  pint::initialize();
  auto const textRenderer = pint::TextRenderer{.font = font, .type = type,};
  return { .id = pint::textRenderers.create(textRenderer), };
}

void puleTextRendererDestroy(PuleTextRenderer const renderer) {
  if (renderer.id == 0) { return; }
  pint::textRenderers.destroy(renderer.id);
}

void puleTextRender(
  PuleTextRenderer const textRenderer,
  PuleGpuCommandListRecorder const commandListRecorder,
  PuleTextRenderInfo const * const textInfo,
  size_t const textInfoCount
) {
  auto const trend = pint::textRenderers.at(textRenderer.id);
  switch (trend->type) {
    default: PULE_assert(false && "unhandled text type");
    case PuleTextType_bitmap:
      puleGpuCommandListAppendAction(
        commandListRecorder,
        PuleGpuCommand {
          .bindPipeline = {
            .action = PuleGpuAction_bindPipeline,
            .pipeline = pint::pipelines[PuleTextType_bitmap],
          }
        }
      );
    break;
  }
  for (size_t it = 0; it < textInfoCount; ++ it) {
    auto const & info = textInfo[it];
    puleLog("Rendering text '%s'");
    PuleF32v4 scale = {
      .x = info.fontScale,
    };
  }
}
