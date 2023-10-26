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

struct GlyphImageInfo {
  PuleGpuImage image;
  int32_t fontWidth;
  int32_t fontHeight;
  int32_t cols;
  int32_t rows;
  int32_t imageWidth;
  int32_t imageHeight;
};

struct TextRenderer {
  PuleAssetFont font;
  PuleTextType type;
  std::unordered_map<size_t, GlyphImageInfo> glyphImages;
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
  pint::GlyphImageInfo info = {
    .image = {},
    .fontWidth = static_cast<int32_t>(fontScale*0.5f),
    .fontHeight = static_cast<int32_t>(fontScale),
    .cols = 16,
    .rows = pint::glyphCount / 16 + (pint::glyphCount % 16 != 0),
  };
  info.imageWidth = info.cols * info.fontWidth;
  info.imageHeight = info.rows * info.fontHeight;
  puleLog("creating the glyph data");
  std::vector<uint8_t> glyphsetData;
  glyphsetData.resize(info.imageWidth * info.imageHeight);
  for (size_t glyphIt = 0; pint::glyphArray[glyphIt] != '\0'; ++ glyphIt) {
    PuleError err = puleError();
    // render to temporary buffer
    std::vector<uint8_t> tempBuffer;
    tempBuffer.resize(info.fontWidth * fontScale);
    puleAssetFontRenderToU8Buffer(
      PuleAssetFontRenderInfo {
        .font = textRenderer.font,
        .fontScale = PuleF32v2 {
          .x = (float)(info.fontWidth), .y = (float)info.fontHeight,
        },
        .fontOffset = {0.f, 0.f},
        .renderFlippedY = true,
        .glyphCodepoint = static_cast<uint32_t>(pint::glyphArray[glyphIt]),
        .destinationBuffer = {
          .data = tempBuffer.data(),
          .byteLength = tempBuffer.size(),
        },
        .destinationBufferDim = { // TODO deal with the integer artefacts prior
          .x = (uint32_t)info.fontWidth,
          .y = (uint32_t)info.fontHeight,
        },
      },
      &err
    );
    if (puleErrorConsume(&err)) {
      continue;
    }
    // print out the glyph
    puleLog(
      "---- glyph for letter '%c' [%dx%d]\n",
      pint::glyphArray[glyphIt],
      info.fontWidth, info.fontHeight
    );
    for (int32_t itx = 0; itx < info.fontWidth; ++ itx) {
      for (int32_t ity = 0; ity < info.fontHeight; ++ ity) {
        printf("%c", tempBuffer[itx*info.fontHeight + ity] == 0 ? ' ' : '#');
      }
      printf("\n");
    }
    // copy to glyphset
    for (int32_t itx = 0; itx < info.fontWidth; ++ itx)
    for (int32_t ity = 0; ity < info.fontHeight; ++ ity) {
      uint32_t const glyphCol = glyphIt%info.cols;
      uint32_t const glyphRow = glyphIt/info.cols;
      uint32_t const pixelIt = (
        glyphCol*info.fontWidth
        + glyphRow*info.fontHeight*info.imageWidth
        + itx
        + ity*info.imageWidth
      );
      glyphsetData[pixelIt] = (tempBuffer[ity*info.fontWidth + itx]);
    }
  }
  // create the image
  puleLog("creating the glyph image");
  // TODO use font name in the label
  std::string imageLabel = "pule-text-glyphset-" + std::to_string(fontScale);
  info.image = (
    puleGpuImageCreate(
      PuleGpuImageCreateInfo {
        .width = (uint32_t)info.imageWidth,
        .height = (uint32_t)info.imageHeight,
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
  textRenderer.glyphImages.emplace(fontScale, info);
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
    auto const & fontInfo = trend.glyphImages.at(info.fontScale);
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
        .x = (float)(textCharIt*fontInfo.fontWidth),
        .y = 0.f,
      };
      PuleF32v4 glyphVertices = {
        .x = (float)(glyphIt%fontInfo.cols)*fontInfo.fontWidth,
        .y = std::floor(glyphIt/(float)fontInfo.cols)*fontInfo.fontHeight,
        .z = (float)fontInfo.fontWidth,
        .w = (float)fontInfo.fontHeight,
      };
      puleLog("glyph vertices: %f %f %f %f", glyphVertices.x, glyphVertices.y, glyphVertices.z, glyphVertices.w);
      // normalize
      glyphVertices.x /= (float)fontInfo.imageWidth;
      glyphVertices.y /= (float)fontInfo.imageHeight;
      glyphVertices.z /= (float)fontInfo.imageWidth;
      glyphVertices.w /= (float)fontInfo.imageHeight;
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
                .image = trend.glyphImages.at(info.fontScale).image,
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

void puleTextRender2D(
  PuleTextRenderer const textRenderer,
  PuleGpuCommandListRecorder const commandListRecorder,
  PuleTextRender2DInfo const * const textInfo,
  size_t const textInfoCount
) {
  // TODO get viewport dimensions from renderpass bound to command list recorder
  PuleF32v2 const viewport = { .x = 4800.f, .y = 4600.f, };
  std::vector<PuleTextRenderInfo> textRenderInfos;
  for (size_t it = 0; it < textInfoCount; ++ it) {
    auto const & info = textInfo[it];
    PuleF32m44 transform = puleF32m44Viewport(viewport.x, viewport.y);
    //transform = (
    //  puleF32m44Mul(
    //    transform,
    //    puleF32m44Translate(PuleF32v3 {
    //      .x = (float)info.position.x,
    //      .y = (float)info.position.y,
    //      .z = 0.f,
    //    })
    //  )
    //);
    textRenderInfos.emplace_back(PuleTextRenderInfo {
      .fontScale = info.fontScale,
      .transform = transform,
      .color = info.color,
      .text = info.text,
    });
  }
  puleTextRender(
    textRenderer, commandListRecorder,
    textRenderInfos.data(), textRenderInfos.size()
  );
}

#endif
