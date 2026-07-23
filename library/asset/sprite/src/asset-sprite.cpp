#include <pulchritude/asset-sprite.h>

#include <pulchritude/core.hpp>

#include <pulchritude/array.h>
#include <pulchritude/asset-image.h>
#include <pulchritude/data-serializer.h>
#include <pulchritude/gpu.h>
#include <pulchritude/math.h>
#include <pulchritude/string.hpp>
#include <pulchritude/time.hpp>

#include <string>
#include <vector>

namespace pint {

struct Spritesheet {
  PuleAssetImage image;
  PuleGpuImage imageGpu;
};

struct Sprite {
  std::string name;
  PuleAssetSpritesheet spritesheet;
  PuleI32v2 texcoordUpperLeft;
  PuleI32v2 texelDims;
};

struct SpriteAnimationKeyframe {
  PuleAssetSprite sprite;
  PuleMillisecond timestamp;
  PuleI32v2 originOffset;
};

struct SpriteAnimation {

  // -- const data created from user
  std::string name;
  std::vector<SpriteAnimationKeyframe> keyframes;
  bool loop;

  // -- runtime data
  PuleMillisecond overallDuration; // calculated at load
};

struct SpriteAnimationLibrary {
  std::vector<SpriteAnimation> animations;
};

struct SpriteSkeletonBone {
  std::string name;
  size_t parentIndex;
  PuleI32v2 originOffset;
  SpriteAnimationLibrary animations;
};

struct SpriteSkeleton {
  std::vector<SpriteSkeletonBone> bones;
};

// this acts as a union of data
struct SpriteLibraryData {
  PuleAssetSprite singleSprite;
  SpriteAnimationLibrary animationLibrary;
  SpriteSkeleton skeleton;
};

struct SpriteLibrary {
  PuleAssetSpritesheet spritesheet;
  SpriteLibraryData data;
  PuleAssetSpriteLibraryDataType dataType;
};

struct SpriteInstance {
  std::string animationLabel;
  PuleMillisecond timestamp;
  PuleAssetSpriteLibrary library;
};

struct SpriteTile {
  int64_t index = -1;
  bool flipX = false;
  bool flipY = false;
};

struct SpriteTilemap {
  std::string label;
  PuleAssetSpritesheet spritesheet;
  PuleI32v2 tileTexelDimensions;
  PuleI32v2 tileCount;
  std::vector<SpriteTile> tiles;
};

pule::ResourceContainer<Spritesheet, PuleAssetSpritesheet> spritesheets;
pule::ResourceContainer<Sprite, PuleAssetSprite> sprites;
pule::ResourceContainer<
  SpriteInstance, PuleAssetSpriteInstance
> spriteInstances;
pule::ResourceContainer<SpriteLibrary, PuleAssetSpriteLibrary> spriteLibraries;
pule::ResourceContainer<SpriteTilemap, PuleAssetSpriteTilemap> tilemaps;

} // namespace pint

// -----------------------------------------------------------------------------
// --- resource fetching utilities ---------------------------------------------
// -----------------------------------------------------------------------------

namespace pint {

pint::SpriteAnimation const * fetchAnimation(
  pint::SpriteInstance const instance,
  pint::SpriteLibrary const & library
) {
  PULE_assert(
    library.dataType == PuleAssetSpriteLibraryDataType_animationLibrary
  );
  for (auto const & animation : library.data.animationLibrary.animations) {
    // fetch the correct animation
    if (animation.name != instance.animationLabel) {
      continue;
    }
    return &animation;
  }
  puleLogError("animation not found: %s", instance.animationLabel.c_str());
  return nullptr;
}

} // namespace pint

extern "C" {

PuleAssetSpritesheet puleAssetSpritesheetCreate(
  PuleAssetSpritesheetCreateInfo info
) {
  auto imageGpu = info.imageGpu;
  static auto defaultSampler = PuleGpuSampler {.id = 0};
  // create gpu image if not provided
  if (imageGpu.id == 0) {
    if (defaultSampler.id == 0) {
      defaultSampler = (
        puleGpuSamplerCreate({
          .minify = PuleGpuImageMagnification_nearest,
          .magnify = PuleGpuImageMagnification_nearest,
          .wrapU = PuleGpuImageWrap_clampToEdge,
          .wrapV = PuleGpuImageWrap_clampToEdge,
          .wrapW = PuleGpuImageWrap_clampToEdge,
        })
      );
    }
    imageGpu = (
      puleGpuImageCreate({
        .width = puleAssetImageWidth(info.image),
        .height = puleAssetImageHeight(info.image),
        .target = PuleGpuImageTarget_i2D,
        .byteFormat = PuleGpuImageByteFormat_rgba8U,
        .sampler = defaultSampler,
        .label = "spritesheet-autogen"_psv,
        .optionalInitialData = (
          puleAssetImageDecodedData(info.image)
        )
      })
    );
  }
  PULE_assert(
    imageGpu.id != 0
    && "spritesheet image gpu not initialized"
  );
  return pint::spritesheets.create({.image = info.image, .imageGpu = imageGpu});
}

PuleAssetSprite puleAssetSpritesheetFetchSprite(
  PuleAssetSpritesheet const puSpritesheet,
  PuleStringView const name
) {
  for (auto & sprite : pint::sprites) {
    if (
         sprite.second.name == std::string(name.contents, name.len)
      && sprite.second.spritesheet.id == puSpritesheet.id
    ) {
      return { sprite.first };
    }
  }
  puleLogError("sprite not found: %s", name.contents);
  PULE_assert(false && "sprite not found");
  return { .id = 0, };
}

void puleAssetSpritesheetDestroy(
  PuleAssetSpritesheet spritesheet
) {
  pint::spritesheets.destroy(spritesheet);
}

PuleAssetSprite puleAssetSpriteCreate(
  PuleAssetSpriteCreateInfo info
) {
  return pint::sprites.create(pint::Sprite {
    .name = std::string(info.label.contents, info.label.len),
    .spritesheet = info.spritesheet,
    .texcoordUpperLeft = info.texcoordUpperLeft,
    .texelDims = info.texelDims,
  });
}

void puleAssetSpriteDestroy(
  PuleAssetSprite sprite
) {
  pint::sprites.destroy(sprite);
}

PuleAssetSpriteLibrary puleAssetSpriteLibraryCreate(
  PuleAssetSpriteLibraryCreateInfo info
) {
  pint::SpriteLibrary library = {
    .spritesheet = info.spritesheet,
    .data = {},
    .dataType = info.dataType,
  };
  switch (info.dataType) {
    case PuleAssetSpriteLibraryDataType_singleSprite:
      // assert it actually exists
      library.data.singleSprite = info.data.singleSprite;
    break;
    case PuleAssetSpriteLibraryDataType_animationLibrary:
      for (
        size_t it = 0; it < info.data.animationLibrary.animationCount; ++ it
      ) {
        auto & infoAnimation = info.data.animationLibrary.animations[it];
        pint::SpriteAnimation animation = {
          .name = std::string(infoAnimation.label.contents),
          .keyframes = {},
          .loop = infoAnimation.loop,
          .overallDuration = {0},
        };
        for (
          size_t keyframeIt = 0;
          keyframeIt < infoAnimation.keyframeCount;
          ++ keyframeIt
        ) {
          auto & infoKeyframe = infoAnimation.keyframes[keyframeIt];
          animation.keyframes.push_back({
            .sprite = infoKeyframe.sprite,
            .timestamp = infoKeyframe.timestamp,
            .originOffset = infoKeyframe.originOffset,
          });
          animation.overallDuration.valueMilli += (
            infoKeyframe.timestamp.valueMilli
          );
        }
        library.data.animationLibrary.animations.push_back(animation);
      }
    break;
    case PuleAssetSpriteLibraryDataType_skeleton:
      library.data.skeleton.bones.resize(
        info.data.skeleton.boneCount
      );
      for (size_t it = 0; it < info.data.skeleton.boneCount; ++ it) {
        auto & bone = info.data.skeleton.bones[it];
        auto & libraryBone = library.data.skeleton.bones[it];
        libraryBone = {
          .name = std::string(bone.name.contents, bone.name.len),
          .parentIndex = bone.parentIndex,
          .originOffset = bone.originOffset,
          .animations = {},
        };
        libraryBone.animations.animations.resize(
          bone.animationLibrary.animationCount
        );
        memcpy(
          libraryBone.animations.animations.data(),
          bone.animationLibrary.animations,
          (
              bone.animationLibrary.animationCount
            * sizeof(pint::SpriteAnimation)
          )
        );
      }
    break;
  }
  return pint::spriteLibraries.create(library);
}

void puleAssetSpriteLibraryDestroy(
  PuleAssetSpriteLibrary spriteLibrary
) {
  pint::spriteLibraries.destroy(spriteLibrary);
}

PuleAssetSpriteInstance puleAssetSpriteInstanceCreate(
  PuleAssetSpriteInstanceCreateInfo info
) {
  return pint::spriteInstances.create(pint::SpriteInstance {
    .animationLabel = (
      std::string(info.defaultAnimation.contents, info.defaultAnimation.len)
    ),
    .timestamp = { .valueMilli = 0, },
    .library = info.spriteLibrary,
  });
}

void puleAssetSpriteInstanceSetAnimation(
  PuleAssetSpriteInstance const puSpriteInstance,
  PuleStringView const animationLabel
) {
  pint::SpriteInstance & instance = *pint::spriteInstances.at(puSpriteInstance);
  instance.animationLabel = std::string(
    animationLabel.contents, animationLabel.len
  );
  instance.timestamp = { .valueMilli = 0, };
}

void puleAssetSpriteInstanceAnimationUpdate(
  PuleAssetSpriteInstance const puSpriteInstance,
  PuleMillisecond const deltaTime
) {
  pint::SpriteInstance & instance = *pint::spriteInstances.at(puSpriteInstance);
  auto & library = *pint::spriteLibraries.at(instance.library);
  if (library.dataType != PuleAssetSpriteLibraryDataType_animationLibrary) {
    return;
  }
  pint::SpriteAnimation const * animationPtr = (
    fetchAnimation(instance, library)
  );
  auto & animation = *animationPtr;
  instance.timestamp.valueMilli += deltaTime.valueMilli;
  if (animation.loop) {
    instance.timestamp.valueMilli %= animation.overallDuration.valueMilli;
  }
}

} // extern "C"

// -- render -------------------------------------------------------------------

// this is capable of batching sprites into a single draw call

namespace pint {

constexpr size_t maxSpriteInstances = 1024;

struct SpriteAttribute {
  PuleF32v2 origin;
};

struct SpriteAttributeInstance {
  PuleF32v2 origin;
  PuleF32v2 uvOffset;
  PuleF32v2 uvScale;
  PuleF32v4 colorMultiply;
  PuleF32v2 flipXY;
};

struct GpuData {
  PuleGpuPipeline spritePipeline;
  PuleGpuShaderModule spriteShaderModule;

  PuleGpuBuffer bufferAttribute;
  PuleGpuBuffer bufferAttributeInstance;
  SpriteAttributeInstance * mappedBufferAttributeInstance;
};

static GpuData gpuData; // global gpu data

void initializeGpuData() {
  #include "autogen-asset-sprite.vert.spv"
  #include "autogen-asset-sprite.frag.spv"

  PuleError err = puleError();

  pint::gpuData.spriteShaderModule = (
    puleGpuShaderModuleCreate(
      PuleBufferView {
        .data = assetSpriteVert,
        .byteLength = sizeof(assetSpriteVert),
      },
      PuleBufferView {
        .data = assetSpriteFrag,
        .byteLength = sizeof(assetSpriteFrag),
      },
      &err
    )
  );
  if (puleErrorConsume(&err)) {
    return;
  }

  // -- layout descriptor sets
  auto layoutDescriptorSet = puleGpuPipelineDescriptorSetLayout();
  layoutDescriptorSet.attributeBindings[0] = {
    .dataType = PuleGpuAttributeDataType_f32,
    .bufferIndex = 0,
    .numComponents = 2,
    .convertFixedDataTypeToNormalizedFloating = false,
    .relativeOffset = offsetof(SpriteAttribute, origin),
  };
  layoutDescriptorSet.attributeBindings[1] = {
    .dataType = PuleGpuAttributeDataType_f32,
    .bufferIndex = 1,
    .numComponents = 2,
    .convertFixedDataTypeToNormalizedFloating = false,
    .relativeOffset = offsetof(SpriteAttributeInstance, origin),
  };
  layoutDescriptorSet.attributeBindings[2] = {
    .dataType = PuleGpuAttributeDataType_f32,
    .bufferIndex = 1,
    .numComponents = 2,
    .convertFixedDataTypeToNormalizedFloating = false,
    .relativeOffset = offsetof(SpriteAttributeInstance, uvOffset),
  };
  layoutDescriptorSet.attributeBindings[3] = {
    .dataType = PuleGpuAttributeDataType_f32,
    .bufferIndex = 1,
    .numComponents = 2,
    .convertFixedDataTypeToNormalizedFloating = false,
    .relativeOffset = offsetof(SpriteAttributeInstance, uvScale),
  };
  layoutDescriptorSet.attributeBindings[4] = {
    .dataType = PuleGpuAttributeDataType_f32,
    .bufferIndex = 1,
    .numComponents = 4,
    .convertFixedDataTypeToNormalizedFloating = false,
    .relativeOffset = offsetof(SpriteAttributeInstance, colorMultiply),
  };
  layoutDescriptorSet.attributeBindings[5] = {
    .dataType = PuleGpuAttributeDataType_f32,
    .bufferIndex = 1,
    .numComponents = 2,
    .convertFixedDataTypeToNormalizedFloating = false,
    .relativeOffset = offsetof(SpriteAttributeInstance, flipXY),
  };
  layoutDescriptorSet.attributeBufferBindings[0] = {
    .stridePerElement = sizeof(SpriteAttribute),
    .inputRate = PuleGpuPipelineAttributeInputRate_perVertex,
  };
  layoutDescriptorSet.attributeBufferBindings[1] = {
    .stridePerElement = sizeof(SpriteAttributeInstance),
    .inputRate = PuleGpuPipelineAttributeInputRate_perInstance,
  };
  layoutDescriptorSet.textureBindings[0] = PuleGpuDescriptorStage_fragment;

  constexpr PuleGpuPipelineLayoutPushConstant pipelineLayoutPushConstants = {
    .stage = PuleGpuDescriptorStage_vertex,
    .byteLength = sizeof(PuleF32v4),
    .byteOffset = 0,
  };

  // -- pipeline creation
  auto pipelineInfo = (
    PuleGpuPipelineCreateInfo {
      .shaderModule = pint::gpuData.spriteShaderModule,
      .layoutDescriptorSet = layoutDescriptorSet,
      .layoutPushConstants = pipelineLayoutPushConstants,
      .config = {
        .depthTestEnabled = false,
        .blendEnabled = true,
        .scissorTestEnabled = false,
        .viewportMin = { 0, 0 },
        .viewportMax = { 800, 600 }, // TODO FIX
        .scissorMin = { 0, 0 },
        .scissorMax = { 0, 0 },
        .drawPrimitive = PuleGpuDrawPrimitive_triangleStrip,
        .colorAttachmentCount = 1,
        .colorAttachmentFormats = {
          PuleGpuImageByteFormat_rgba8U
        },
        .depthAttachmentFormat = PuleGpuImageByteFormat_depth16,
      },
    }
  );

  pint::gpuData.spritePipeline = puleGpuPipelineCreate(pipelineInfo, &err);
  if (puleErrorConsume(&err)) { return; }

  // -- buffer creation
  { // attribute buffer
    constexpr size_t maxAttributeByteSize = (
      pint::maxSpriteInstances * sizeof(SpriteAttribute)
    );
    pint::gpuData.bufferAttribute = (
      puleGpuBufferCreate(
        puleCStr("sprite-attribute"),
        maxAttributeByteSize,
        PuleGpuBufferUsage_attribute,
        PuleGpuBufferVisibilityFlag_deviceOnly
      )
    );
    std::vector<SpriteAttribute> attributes {
      { .origin = { -0.5f, -0.5f }, },
      { .origin = {  0.5f, -0.5f }, },
      { .origin = { -0.5f,  0.5f }, },
      { .origin = {  0.5f,  0.5f }, },
    };
    puleGpuBufferMemcpy(
      PuleGpuBufferMappedFlushRange {
        .buffer = pint::gpuData.bufferAttribute,
        .byteOffset = 0,
        .byteLength = maxAttributeByteSize,
      },
      attributes.data()
    );
  }

  constexpr size_t maxAttributeInstanceByteSize = (
    pint::maxSpriteInstances * sizeof(SpriteAttributeInstance)
  );

  { // attribute instance buffer
    pint::gpuData.bufferAttributeInstance = (
      puleGpuBufferCreate(
        puleCStr("sprite-attribute-instance"),
        maxAttributeInstanceByteSize,
        PuleGpuBufferUsage_attribute,
        PuleGpuBufferVisibilityFlag_hostWritable
      )
    );
  }

  // map buffer
  pint::gpuData.mappedBufferAttributeInstance = (
    static_cast<SpriteAttributeInstance *>(
      puleGpuBufferMap({
        .buffer = pint::gpuData.bufferAttributeInstance,
        .access = PuleGpuBufferMapAccess_hostWritable,
        .byteOffset = 0,
        .byteLength = maxAttributeInstanceByteSize,
      })
    )
  );
}

struct Recorder {
  PuleGpuCommandListRecorder commandListRecorder;
  size_t numSpriteInstances;
  std::vector<PuleGpuImage> spriteInstanceTextures;
  PuleI32v2 viewportMin;
  PuleI32v2 viewportMax;
};

pule::ResourceContainer<Recorder, PuleAssetSpriteInstanceDrawRecorder>
  recorders
;

void renderSpriteInternal(
  Recorder & recorder,
  PuleAssetSpritesheet const puSpritesheet,
  SpriteAttributeInstance const & attributeInstance
) {
  pint::Spritesheet const & spritesheet = (
    *pint::spritesheets.at(puSpritesheet)
  );

  memcpy(
    (
        pint::gpuData.mappedBufferAttributeInstance
      + recorder.numSpriteInstances
    ),
    &attributeInstance,
    sizeof(SpriteAttributeInstance)
  );

  // -- record texture and increment sprite instance count
  recorder.spriteInstanceTextures.push_back(spritesheet.imageGpu);
  ++ recorder.numSpriteInstances;
}

void renderSprite(
  Recorder & recorder,
  PuleAssetSprite const sprite,
  PuleF32v2 const position
) {
  pint::Sprite const & spriteData = *pint::sprites.at(sprite);

  // -- update attribute instance buffer
  SpriteAttributeInstance attributeInstance = {
    .origin = position,
    .uvOffset = {
      (float)spriteData.texcoordUpperLeft.x,
      (float)spriteData.texcoordUpperLeft.y
    },
    .uvScale = {
      (float)spriteData.texelDims.x,
      (float)spriteData.texelDims.y,
    },
    .colorMultiply = { 1.0f, 1.0f, 1.0f, 1.0f, },
    .flipXY = { (float)false, (float)false, },
  };

  renderSpriteInternal(recorder, spriteData.spritesheet, attributeInstance);
}

void commandListRecorderFinish(
  pint::Recorder const & recorder
) {
  if (recorder.numSpriteInstances == 0) {
    puleLogError("no sprite instances to draw");
    return;
  }
  // -- bind pipeline
  puleGpuCommandListAppendAction(
    recorder.commandListRecorder,
    {.bindPipeline = { .pipeline = pint::gpuData.spritePipeline, }}
  );
  // -- bind push constants
  PuleF32v4 viewport = {
    (float)recorder.viewportMin.x,
    (float)recorder.viewportMin.y,
    (float)recorder.viewportMax.x,
    (float)recorder.viewportMax.y,
  };
  puleGpuCommandListAppendAction(
    recorder.commandListRecorder,
    {.pushConstants = {
      .stage = PuleGpuDescriptorStage_vertex,
      .byteLength = sizeof(PuleF32v4),
      .byteOffset = 0,
      .data = &viewport,
    }}
  );
  // -- bind buffers
  puleGpuCommandListAppendAction(
    recorder.commandListRecorder,
    {.bindAttributeBuffer = {
      .bindingIndex = 0,
      .buffer = pint::gpuData.bufferAttribute,
      .offset = 0,
      .stride = sizeof(SpriteAttribute),
    }}
  );
  puleGpuCommandListAppendAction(
    recorder.commandListRecorder,
    {.bindAttributeBuffer = {
      .bindingIndex = 1,
      .buffer = pint::gpuData.bufferAttributeInstance,
      .offset = 0,
      .stride = sizeof(SpriteAttributeInstance),
    }}
  );
  // iterate and draw sprites for each texture
  // TODO i can obviously batch sprites in many different ways
  for (size_t it = 0; it < recorder.numSpriteInstances; ++ it) {
    PULE_assert(
      recorder.spriteInstanceTextures[it].id != 0
      && "sprite instance texture not initialized"
    );
    // -- bind texture
    puleGpuCommandListAppendAction(
      recorder.commandListRecorder,
      {.bindTexture = {
        .bindingIndex = 0,
        .imageView = {
          .image = recorder.spriteInstanceTextures[it],
          .mipmapLevelStart = 0,
          .mipmapLevelCount = 1,
          .arrayLayerStart = 0,
          .arrayLayerCount = 1,
          .byteFormat = PuleGpuImageByteFormat_rgba8U,
        },
        .imageLayout = PuleGpuImageLayout_storage,
      }}
    );
    puleGpuCommandListAppendAction(
      recorder.commandListRecorder,
      {.dispatchRender = {
        .vertexOffset = 0,
        .numVertices = 4,
        .instanceOffset = it,
        .numInstances = 1,
      }}
    );
  }
}

} // namespace pint

extern "C" {

PuleAssetSpriteInstanceDrawRecorder puleAssetSpriteInstanceDrawRecorder(
  PuleGpuCommandListRecorder const commandListRecorder,
  PuleI32v2 const viewportMin,
  PuleI32v2 const viewportMax
) {
  if (pint::gpuData.spritePipeline.id == 0) {
    pint::initializeGpuData();
  }
  return (
    pint::recorders.create(
      {
        .commandListRecorder = commandListRecorder,
        .numSpriteInstances = 0,
        .spriteInstanceTextures = {},
        .viewportMin = viewportMin,
        .viewportMax = viewportMax,
      }
    )
  );
}

void puleAssetSpriteInstanceDrawRecorderFinish(
  PuleAssetSpriteInstanceDrawRecorder const puRecorder
) {
  auto const & recorder = *pint::recorders.at(puRecorder);
  pint::commandListRecorderFinish(recorder);
  pint::recorders.destroy(puRecorder);
}

void puleAssetSpriteInstanceDraw(
  PuleAssetSpriteInstanceDrawRecorder const puRecorder,
  PuleAssetSpriteInstance const spriteInstance,
  PuleF32v2 const position
) {
  pint::Recorder & recorder = *pint::recorders.at(puRecorder);
  pint::SpriteInstance const & instance = (
    *pint::spriteInstances.at(spriteInstance)
  );
  pint::SpriteLibrary const & library = (
    *pint::spriteLibraries.at(instance.library)
  );
  switch (library.dataType) {
    case PuleAssetSpriteLibraryDataType_singleSprite:
      pint::renderSprite(recorder, library.data.singleSprite, position);
    break;
    case PuleAssetSpriteLibraryDataType_animationLibrary: {
      // -- find the animation
      pint::SpriteAnimation const * animationPtr = (
        fetchAnimation(instance, library)
      );

      // -- prepare to render animation
      auto & animation = *animationPtr;
      puleLogDev(
        "rendering animation: %s, total size: %zu",
        animation.name.c_str(),
        animation.keyframes.size()
      );
      if (animation.keyframes.empty()) {
        puleLogError("animation has no keyframes");
        break;
      }
      // -- fetch correct keyframe
      int64_t remainingTime = instance.timestamp.valueMilli;
      PuleAssetSprite spriteToRender = animation.keyframes.back().sprite;
      for (auto const & keyframe : animation.keyframes) {
        if (remainingTime < keyframe.timestamp.valueMilli) {
          puleLogDev("rendering keyframe: %p", keyframe.sprite.id);
          spriteToRender = keyframe.sprite;
          break;
        }
        remainingTime -= keyframe.timestamp.valueMilli;
      }
      pint::renderSprite(recorder, spriteToRender, position);
    } break;
    case PuleAssetSpriteLibraryDataType_skeleton:
    break;
  }
}

// -- deserialize / serialize --------------------------------------------------

PuleAssetSpriteLibrary puleAssetSpriteLibraryDeserialize(
  PuleDsValue const dsValue
) {
  PuleError err = puleError();

  PuleAssetSpriteLibraryCreateInfo spriteLibraryCi;

  // -- load up spritesheet --
  {
    PuleFileStream assetImage = (
      puleFileStreamReadOpen("assets/sprite.png"_psv, PuleFileDataMode_binary)
    );
    puleScopeExit { puleFileStreamClose(assetImage); };

    auto const dsSpritesheet = puleDsObjectMember(dsValue, "spritesheet");
    spriteLibraryCi.spritesheet = (
      puleAssetSpritesheetCreate({
        .label = puleDsMemberAsString(dsSpritesheet, "label"),
        .image = (
          puleAssetImageLoadFromStream(
            puleAllocateDefault(),
            puleFileStreamReader(assetImage),
            "png"_psv,
            PuleAssetImageFormat_rgbaU8,
            &err
          )
        ),
        .imageGpu = { .id = 0 },
      })
    );
  }

  // -- load up sprite library data --
  // auto const dsValueType = puleDsMemberAsI64(dsValue, "type");
  // spriteLibraryCi.dataType = (
  //   static_cast<PuleAssetSpriteLibraryDataType>(dsValueType)
  // );
  // std::vector<PuleAssetSpriteAnimation> animations;
  // switch (dsValueType) {
  //   case PuleAssetSpriteLibraryDataType_skeleton:
  //   break;
  //   case PuleAssetSpriteLibraryDataType_singleSprite:
  //   break;
  //   case PuleAssetSpriteLibraryDataType_animationLibrary:
  //     auto const dsValueAnimations = puleDsMemberAsArray(dsValue, "animations");
  //     spriteLibraryCi.data.animationLibrary.animationCount = (
  //       dsValueAnimations.length
  //     );
  //     animations.resize(dsValueAnimations.length);
  //     for (size_t it = 0; it < dsValueAnimations.length; ++ it) {
  //       // auto & dsValueAnimation = dsValueAnimations[it];
  //       animations[it].label = (
  //         puleDsMemberAsString(dsValueAnimation, "label")
  //       );
  //       animations[it].loop = (
  //         puleDsMemberAsBool(dsValueAnimation, "loop")
  //       );
  //     }
  //   break;
  //   default:
  //     PULE_assert("invalid sprite library type");
  //   break;
  // }
}

// -----------------------------------------------------------------------------
// -- sprite tilemap -----------------------------------------------------------
// -----------------------------------------------------------------------------

PuleAssetSpriteTilemap puleAssetSpriteTilemapCreate(
  PuleAssetSpriteTilemapCreateInfo ci
) {
  auto puTilemap = (
    pint::tilemaps.create({
      .label = std::string(ci.label.contents, ci.label.len),
      .spritesheet = ci.spritesheet,
      .tileTexelDimensions = ci.tileTexelDimensions,
      .tileCount = ci.tileCount,
      .tiles = {},
    })
  );
  auto & tilemap = *pint::tilemaps.at(puTilemap);
  tilemap.tiles.resize(ci.tileCount.x * ci.tileCount.y);
  return puTilemap;
}

void puleAssetSpriteTilemapDestroy(
  PuleAssetSpriteTilemap tm
) {
  pint::tilemaps.destroy(tm);
}

void puleAssetSpriteTilemapSetTile(
  PuleAssetSpriteTilemap const puTilemap,
  PuleI32v2 const origin,
  int64_t const tileIndex,
  bool const flipX,
  bool const flipY
) {
  auto & tilemap = *pint::tilemaps.at(puTilemap);
  PULE_assert(
       origin.x >= 0 && origin.x < tilemap.tileCount.x
    && origin.y >= 0 && origin.y < tilemap.tileCount.y
    && "tilemap origin out of bounds"
  );
  tilemap.tiles[origin.y*tilemap.tileCount.x + origin.x] = {
    .index = tileIndex,
    .flipX = flipX,
    .flipY = flipY,
  };
}

PULE_exportFn void puleAssetSpriteTilemapRender(
  PuleAssetSpriteInstanceDrawRecorder drawRecorder,
  PuleAssetSpriteTilemap puTilemap,
  PuleF32v2 cameraOffset,
  PuleF32v4 colorMultiply
) {
  pint::SpriteTilemap const & tilemap = *pint::tilemaps.at(puTilemap);
  auto & recorder = *pint::recorders.at(drawRecorder);
  for (size_t it = 0; it < tilemap.tiles.size(); ++ it) {
    if (tilemap.tiles[it].index == -1) { continue; }
    size_t const itx = it % tilemap.tileCount.x;
    size_t const ity = it / tilemap.tileCount.x;
    int64_t tileId = tilemap.tiles[it].index;

    size_t const texcoordX = (
      (tileId % tilemap.tileCount.x) * tilemap.tileTexelDimensions.x
    );
    size_t const texcoordY = (
      (tileId / tilemap.tileCount.x) * tilemap.tileTexelDimensions.y
    );

    pint::SpriteAttributeInstance attributeInstance = {
      .origin = {
        cameraOffset.x + (float)itx * tilemap.tileTexelDimensions.x,
        cameraOffset.y + (float)ity * tilemap.tileTexelDimensions.y,
      },
      .uvOffset = { (float)texcoordX, (float)texcoordY },
      .uvScale = {
        (float)tilemap.tileTexelDimensions.x,
        (float)tilemap.tileTexelDimensions.y,
      },
      .colorMultiply = colorMultiply,
      .flipXY = {
        (float)tilemap.tiles[it].flipX, (float)tilemap.tiles[it].flipY
      },
    };

    pint::renderSpriteInternal(
      recorder,
      tilemap.spritesheet,
      attributeInstance
    );
  }
}

} // extern "C"
