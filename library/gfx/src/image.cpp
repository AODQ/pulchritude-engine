#include <pulchritude-gfx/image.h>

#include <pulchritude-log/log.h>

#include <glad/glad.h>

#include <unordered_map>

namespace { // sampler
  std::unordered_map<uint64_t, PuleGfxSamplerCreateInfo> samplers;
  uint64_t samplersIt = 1;

  GLenum imageMagnificationToGl(PuleGfxImageMagnification const magnify) {
    switch (magnify) {
      default:
        puleLogError("unknown image magnification: %d", magnify);
        return GL_NEAREST;
      case PuleGfxImageMagnification_nearest:
        return GL_NEAREST;
    }
  }

  GLenum imageWrapToGl(PuleGfxImageWrap const wrap) {
    switch (wrap) {
      default:
        puleLogError("unknown image wrap: %d", wrap);
        return GL_CLAMP_TO_EDGE;
      case PuleGfxImageWrap_clampToEdge:
        return GL_CLAMP_TO_EDGE;
    }
  }

  void applySampler(GLuint const textureHandle, PuleGfxSampler const sampler) {
    PuleGfxSamplerCreateInfo const & samplerInfo = samplers.at(sampler.id);
    glTextureParameteri(
      textureHandle,
      GL_TEXTURE_MAG_FILTER,
      imageMagnificationToGl(samplerInfo.magnify)
    );
    glTextureParameteri(
      textureHandle,
      GL_TEXTURE_MIN_FILTER,
      imageMagnificationToGl(samplerInfo.minify)
    );
    glTextureParameteri(
      textureHandle,
      GL_TEXTURE_WRAP_S,
      imageWrapToGl(samplerInfo.wrapU)
    );
    glTextureParameteri(
      textureHandle,
      GL_TEXTURE_WRAP_T,
      imageWrapToGl(samplerInfo.wrapV)
    );
  }
}

extern "C" { // sampler

PuleGfxSampler puleGfxSamplerCreate(
  PuleGfxSamplerCreateInfo const createInfo
) {
  auto const prevSamplerIt = samplersIt;
  samplersIt += 1;

  ::samplers.emplace(prevSamplerIt, createInfo);
  return { prevSamplerIt };
}

void puleGfxSamplerDestroy(
  PuleGfxSampler const sampler
) {
  if (sampler.id == 0) { return; }
  ::samplers.erase(sampler.id);
}

} // C

namespace { // image
  GLenum byteFormatToGlInternalFormat(PuleGfxImageByteFormat const byteFormat) {
    switch (byteFormat) {
      default:
        puleLogError("unknown byte format: %d", byteFormat);
        return GL_RGBA8;
      case PuleGfxImageByteFormat_rgba8U:
        return GL_RGBA8;
      case PuleGfxImageByteFormat_rgb8U:
        return GL_RGB8;
      case PuleGfxImageByteFormat_r8U:
        return GL_R8;
    }
  }

  GLenum byteFormatToGlFormat(PuleGfxImageByteFormat const byteFormat) {
    switch (byteFormat) {
      default:
        puleLogError("unknown byte format: %d", byteFormat);
        return GL_RGBA;
      case PuleGfxImageByteFormat_rgba8U:
        return GL_RGBA;
      case PuleGfxImageByteFormat_rgb8U:
        return GL_RGB;
      case PuleGfxImageByteFormat_r8U:
        return GL_RED;
    }
  }

  GLenum byteFormatToGlType(PuleGfxImageByteFormat const byteFormat) {
    switch (byteFormat) {
      default:
        puleLogError("unknown byte format: %d", byteFormat);
        return GL_UNSIGNED_BYTE;
      case PuleGfxImageByteFormat_rgb8U:
      case PuleGfxImageByteFormat_rgba8U:
      case PuleGfxImageByteFormat_r8U:
        return GL_UNSIGNED_BYTE;
    }
  }

  GLenum imageTargetToGl(PuleGfxImageTarget const imageTarget) {
    switch (imageTarget) {
      default:
        puleLogError("unknown image target: %d", imageTarget);
        return GL_TEXTURE_2D;
      case PuleGfxImageTarget_i2D:
        return GL_TEXTURE_2D;
    }
  }
}

extern "C" { // image

PuleGfxGpuImage puleGfxGpuImageCreate(PuleGfxImageCreateInfo const createInfo) {
  GLuint textureHandle;
  glCreateTextures(::imageTargetToGl(createInfo.target), 1, &textureHandle);
  ::applySampler(textureHandle, createInfo.sampler);

  switch (createInfo.target) {
    default:
      puleLogError("unknown image target: %d", createInfo.target);
    [[fallthrough]];
    case PuleGfxImageTarget_i2D:
      glTextureStorage2D(
        textureHandle,
        1, // texture levels
        byteFormatToGlInternalFormat(createInfo.byteFormat),
        createInfo.width,
        createInfo.height
      );
      if (createInfo.optionalInitialData) {
        glTextureSubImage2D(
          textureHandle,
          0, 0, 0,
          createInfo.width,
          createInfo.height,
          ::byteFormatToGlFormat(createInfo.byteFormat),
          ::byteFormatToGlType(createInfo.byteFormat),
          createInfo.optionalInitialData
        );
      }
    break;
  }

  return { textureHandle };
}

void puleGfxGpuImageDestroy(PuleGfxGpuImage const image) {
  if (image.id == 0) { return; }
  GLuint handle = static_cast<GLuint>(image.id);
  if (handle != 0) {
    glDeleteTextures(1, &handle);
  }
}

} // C

// ------------------------------------------------------------------------------
// -- FRAMEBUFFER ---------------------------------------------------------------
// ------------------------------------------------------------------------------

namespace {
  GLuint framebufferAttachmenToGl(PuleGfxFramebufferAttachment attachment) {
    switch (attachment) {
      default:
        puleLogError("unknown attachment: %d", attachment);
      [[fallthrough]];
      case PuleGfxFramebufferAttachment_color0:
        return GL_COLOR_ATTACHMENT0;
      case PuleGfxFramebufferAttachment_color1:
        return GL_COLOR_ATTACHMENT1;
      case PuleGfxFramebufferAttachment_color3:
        return GL_COLOR_ATTACHMENT2;
      case PuleGfxFramebufferAttachment_color4:
        return GL_COLOR_ATTACHMENT3;
      case PuleGfxFramebufferAttachment_depth:
        return GL_DEPTH_ATTACHMENT;
      case PuleGfxFramebufferAttachment_stencil:
        return GL_STENCIL_ATTACHMENT;
      case PuleGfxFramebufferAttachment_depthStencil:
        return GL_DEPTH_STENCIL_ATTACHMENT;
    }
  }
} // namespace

extern "C" {
PuleGfxFramebufferCreateInfo puleGfxFramebufferCreateInfo() {
  PuleGfxFramebufferCreateInfo info;
  memset(
    &info.attachment,
    0,
    sizeof(PuleGfxFramebufferAttachments)
  );
  return info;
}

PuleGfxFramebuffer puleGfxFramebufferCreate(
  PuleGfxFramebufferCreateInfo const info,
  PuleError * const error
) {
  GLuint framebuffer;
  glCreateFramebuffers(1, &framebuffer);

  for (size_t it = 0; it < PuleGfxFramebufferAttachment_End; ++ it) {
    if (info.attachment.images[it].image.id == 0) {
      continue;
    }

    glNamedFramebufferTexture(
      framebuffer,
      framebufferAttachmenToGl(static_cast<PuleGfxFramebufferAttachment>(it)),
      info.attachment.images[it].image.id,
      info.attachment.images[it].mipmapLevel
    );
  }

  PULE_errorAssert(
    (
      glCheckNamedFramebufferStatus(
        framebuffer, GL_FRAMEBUFFER
      ) == GL_FRAMEBUFFER_COMPLETE
    ),
    PuleErrorGfx_invalidFramebuffer,
    {0}
  )

  return { framebuffer };
}

void puleGfxFramebufferDestroy(PuleGfxFramebuffer const framebuffer) {
  if (framebuffer.id == 0) { return; }
  auto const glFramebuffer = static_cast<GLuint>(framebuffer.id);
  if (glFramebuffer != 0) {
    glDeleteFramebuffers(1, &glFramebuffer);
  }
}

PuleGfxFramebuffer puleGfxFramebufferWindow() {
  return { 0 };
}

} // C
