#include <pulchritude-text/text.h>

#if defined(PULCHRITUDE_FEATURE_TEXT)

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
std::array<PuleGpuShaderModule, PuleTextTypeSize> shaderModules;
std::array<PuleGpuPipeline, PuleTextTypeSize> pipelines;

pule::ResourceContainer<pint::TextRenderer> textRenderers;

// for now, only support ascii
char const glyphArray[] =
  "1234567890!@#$%^&*()-+=_\\/\"';:[]{}()<>|~`., "
  "abcdefghijklmnopqrstuvwxyz"
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "?"
;
uint32_t const glyphCount = PULE_arraySize(pint::glyphArray)-1; // remove \0

void initialize() {
  if (!pint::initialized) {
    pint::initialized = true;
  }
  #include "autogen-text-bitmap.frag.spv"
  #include "autogen-text-bitmap.vert.spv"
  PuleError err = puleError();
  pint::shaderModules[PuleTextType_bitmap] = (
    puleGpuShaderModuleCreate(
      PuleBufferView {
        .data = textBitmapVert,
        .byteLength = sizeof(textBitmapVert),
      },
      PuleBufferView {
        .data = textBitmapFrag,
        .byteLength = sizeof(textBitmapFrag),
      },
      &err
    )
  );
  if (puleErrorConsume(&err)) {
    puleLogError("Failed to create bitmap shader module");
    return;
  }
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
      .byteLength = sizeof(PuleF32m44) + sizeof(PuleF32v2),
      .byteOffset = 0u,
    },
    PuleGpuPipelineLayoutPushConstant {
      .stage = PuleGpuDescriptorStage_fragment,
      .byteLength = sizeof(PuleF32v4),
      .byteOffset = 80,
    },
  };
  auto const bitmapPipeline = (
    PuleGpuPipelineCreateInfo {
      .shaderModule = pint::shaderModules[PuleTextType_bitmap],
      .layoutDescriptorSet = &bitmapLayoutDescriptorSet,
      .layoutPushConstants = &bitmapPushConstants[0],
      .layoutPushConstantsCount = PULE_arraySize(bitmapPushConstants),
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
  // lazy init
  puleLog("Initializing glyph image for font scale %zu", fontScale);
  if (textRenderer.glyphImages.contains(fontScale)) {
    return;
  }
  puleLog("creating the glyph data");
  std::vector<uint8_t> glyphsetData;
  uint32_t const fontWidth = static_cast<uint32_t>(fontScale * 0.5f);
  uint32_t const fontHeight = static_cast<uint32_t>(fontScale);
  uint32_t const rows = 16u;
  uint32_t const cols = (
    pint::glyphCount / rows + (pint::glyphCount % rows != 0)
  );
  uint32_t const imageWidth = rows * fontWidth;
  uint32_t const imageHeight = cols * fontHeight;
  glyphsetData.resize(imageWidth * imageHeight);
  for (size_t glyphIt = 0; pint::glyphArray[glyphIt] != '\0'; ++ glyphIt) {
    PuleError err = puleError();
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
        .glyphCodepoint = static_cast<uint32_t>(pint::glyphArray[glyphIt]),
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
      uint32_t const pixelRow = (glyphIt%rows)*fontWidth + itx;
      uint32_t const pixelColOffset = ity*imageWidth;
      glyphsetData[pixelRow + pixelColOffset] = tempBuffer[itx + ity*fontWidth];
    }
  }
  // create the image
  puleLog("creating the glyph image");
  // TODO use font name in the label
  std::string imageLabel = "pule-text-glyphset-" + std::to_string(fontScale);
  textRenderer.glyphImages.emplace(
    fontScale,
    puleGpuImageCreate(
      PuleGpuImageCreateInfo {
        .width = imageWidth,
        .height = imageHeight,
        .target = PuleGpuImageTarget_i2D,
        .byteFormat = PuleGpuImageByteFormat_r8U,
        .sampler = puleGpuSamplerCreate({
          .minify = PuleGpuImageMagnification_nearest,
          .magnify = PuleGpuImageMagnification_nearest,
          .wrapU = PuleGpuImageWrap_clampToEdge,
          .wrapV = PuleGpuImageWrap_clampToEdge,
        }),
        .label = puleCStr(imageLabel.c_str()),
        .optionalInitialData = glyphsetData.data(),
      }
    )
  );
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
  auto & trend = *pint::textRenderers.at(textRenderer.id);
  for (size_t it = 0; it < textInfoCount; ++ it) {
    pint::createGlyphImage(trend, textInfo[it].fontScale);
  }
  switch (trend.type) {
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
  // TODO - I shouldn't have to do this
  puleGpuCommandListAppendAction(
    commandListRecorder,
    {
      .setScissor = {
        .action = PuleGpuAction_setScissor,
        .scissorMin = { .x = 0, .y = 0, },
        .scissorMax = { .x = 800, .y = 600, },
      },
    }
  );
  for (size_t it = 0; it < textInfoCount; ++ it) {
    auto const & info = textInfo[it];
    uint32_t const fontWidth = static_cast<uint32_t>(info.fontScale * 0.5f);
    uint32_t const fontHeight = static_cast<uint32_t>(info.fontScale);
    uint32_t const rows = 16u;
    puleLog("Rendering text '%s'", info.text.contents);
    for (size_t textCharIt = 0; textCharIt < info.text.len; ++ textCharIt) {
      // shitty way to get the glyph index
      size_t glyphIt = 0;
      for (; glyphIt < pint::glyphCount; ++ glyphIt) {
        if (pint::glyphArray[glyphIt] == info.text.contents[textCharIt]) {
          break;
        }
      }
      struct vertPc {
        PuleF32m44 transform;
        PuleF32v2 offset;
      } vertPc;
      vertPc.transform = info.transform;
      vertPc.offset = PuleF32v2 {
        .x = (float)(textCharIt*fontWidth),
        .y = 0.f,
      };
      PuleF32v4 glyphVertices = {
        .x = (float)(glyphIt%rows)*fontWidth,
        .y = std::floor(glyphIt/(float)rows)*fontHeight,
        .z = (float)fontWidth,
        .w = (float)fontHeight,
      };
      puleGpuCommandListAppendAction(
        commandListRecorder,
        PuleGpuCommand {
          .pushConstants = {
            .action = PuleGpuAction_pushConstants,
            .stage = PuleGpuDescriptorStage_vertex,
            .byteLength = sizeof(vertPc),
            .byteOffset = 0u,
            .data = &vertPc,
          }
        }
      );
      puleGpuCommandListAppendAction(
        commandListRecorder,
        PuleGpuCommand {
          .pushConstants = {
            .action = PuleGpuAction_pushConstants,
            .stage = PuleGpuDescriptorStage_fragment,
            .byteLength = sizeof(PuleF32v4),
            .byteOffset = 80,
            .data = &glyphVertices,
          }
        }
      );
      puleGpuCommandListAppendAction(
        commandListRecorder,
        PuleGpuCommand {
          .bindTexture = {
            .action = PuleGpuAction_bindTexture,
            .bindingIndex = 0,
            .imageView = (
              PuleGpuImageView {
                .image = trend.glyphImages.at(info.fontScale),
                .mipmapLevelStart = 0, .mipmapLevelCount = 1,
                .arrayLayerStart = 0,  .arrayLayerCount = 1,
                .byteFormat = PuleGpuImageByteFormat_r8U,
              }
            ),
            .imageLayout = PuleGpuImageLayout_storage,
          }
        }
      );

      puleGpuCommandListAppendAction(
        commandListRecorder,
        PuleGpuCommand {
          .dispatchRender = {
            .action = PuleGpuAction_dispatchRender,
            .drawPrimitive = PuleGpuDrawPrimitive_triangle,
            .vertexOffset = 0u,
            .numVertices = 6u,
          },
        }
      );
    }
  }
}

#endif
