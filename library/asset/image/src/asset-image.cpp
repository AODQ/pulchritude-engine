#include <pulchritude/asset-image.h>

#include <pulchritude/file.h>
#include <pulchritude/log.h>
#include <pulchritude/string.hpp>

#include <spng.h>

#include <unordered_map>

namespace {
int32_t imageFormatToSpng(PuleAssetImageFormat const fmt) {
  switch (fmt) {
    default: PULE_assert(false); return 0;
    case PuleAssetImageFormat_rgbaU8:  return SPNG_FMT_RGBA8;
    case PuleAssetImageFormat_rgbaU16: return SPNG_FMT_RGBA16;
    case PuleAssetImageFormat_rgbU8:   return SPNG_FMT_RGB8;
  }
}

struct ImageInfo {
  PuleAllocator allocator;
  PuleAssetImageFormat format;
  void * data;
  size_t dataLength;
  uint32_t width, height;
};

std::unordered_map<uint64_t, ImageInfo> images;
uint64_t imagesIt = 1;

} // namespace

extern "C" {

static int32_t readFromStream(
  spng_ctx *,
  void * user,
  void * destination,
  size_t length
) {
  PuleStreamRead & stream = *reinterpret_cast<PuleStreamRead *>(user);
  size_t it = 0;
  uint8_t * destinationAsU8 = reinterpret_cast<uint8_t *>(destination);
  for (it = 0; it < length; ++ it) {
    if (puleStreamReadIsDone(stream)) {
      destinationAsU8[it] = 0;
      return SPNG_IO_EOF;
    }
    destinationAsU8[it] = puleStreamReadByte(stream);
  }
  return 0;
}

PuleAssetImage puleAssetImageLoadFromStream(
  PuleAllocator const allocator,
  PuleStreamRead const imageSource,
  PuleStringView const imageExtension,
  PuleAssetImageFormat const requestedFormat,
  PuleError * const error
) {

  if (!puleStringViewEq(imageExtension, "png"_psv)) {
    PULE_error(PuleErrorAssetImage_decode, "unsupported image format",);
    return { 0 };
  }

  // TODO use new2 mapping spng_alloc to PuleAllocator
  auto context = spng_ctx_new(SPNG_CTX_IGNORE_ADLER32);
  auto const format = imageFormatToSpng(requestedFormat);

  spng_set_png_stream(
    context,
    readFromStream,
    const_cast<void *>(reinterpret_cast<void const *>(&imageSource))
  );

  size_t imageLength;
  if (
    spng_decoded_image_size(context, format, &imageLength) != SPNG_OK
    || imageLength == 0
  ) {
    PULE_error(PuleErrorAssetImage_decode, "failed to get image size",);
    spng_ctx_free(context);
    return { 0 };
  }
  spng_ihdr ihdr;
  if (spng_get_ihdr(context, &ihdr) != SPNG_OK) {
    PULE_error(PuleErrorAssetImage_decode, "failed to get image ihdr info",);
    spng_ctx_free(context);
    return { 0 };
  }

  void * data = (
    puleAllocate(
      allocator,
      PuleAllocateInfo { .numBytes = imageLength, }
    )
  );

  if (spng_decode_image(context, data, imageLength, format, 0) != SPNG_OK) {
    PULE_error(PuleErrorAssetImage_decode, "failed to decode image",);
    spng_ctx_free(context);
    return { 0 };
  }

  ::images[::imagesIt] = {
    .allocator = allocator,
    .format = requestedFormat,
    .data = data,
    .dataLength = imageLength,
    .width = ihdr.width,
    .height = ihdr.height,
  };
  spng_ctx_free(context);
  return { ::imagesIt ++ };
}

PuleAssetImage puleAssetImage(
  uint32_t width, uint32_t height, PuleAssetImageFormat format
) {
  size_t dataLength = width * height;
  switch (format) {
    case PuleAssetImageFormat_rgbU8: dataLength *= 3*sizeof(uint8_t); break;
    case PuleAssetImageFormat_rgbaU8: dataLength *= 4*sizeof(uint8_t); break;
    default: PULE_assert(false);
  }
  void * data = (
    puleAllocate(
      puleAllocateDefault(),
      PuleAllocateInfo {
        .numBytes = dataLength, .alignment = 0, .zeroOut = true,
      }
    )
  );
  ::images[::imagesIt] = {
    .allocator = puleAllocateDefault(),
    .format = format,
    .data = data,
    .dataLength = dataLength,
    .width = width,
    .height = height,
  };
  return { ::imagesIt ++ };
}

void puleAssetImageWriteToStream(
  PuleAssetImage const image,
  PuleStreamWrite const imageDst,
  PuleStringView const imageExtension,
  PuleError * error
) {
  if (!puleStringViewEq(imageExtension, "ppm"_psv)) {
    PULE_error(PuleErrorAssetImage_encode, "unsupported image format",);
    return;
  }

  auto & dataView = ::images.at(image.id);
  PULE_assert(dataView.format == PuleAssetImageFormat_rgbU8);

  // write PPM header
  puleStreamWriteStrFormat(
    imageDst, "P6\n%u %u\n255\n", dataView.width, dataView.height
  );
  puleStreamWriteBytes(imageDst, (uint8_t *)dataView.data, dataView.dataLength);
  puleStreamWriteFlush(imageDst);
}

void puleAssetImageDestroy(PuleAssetImage const image) {
  if (image.id == 0) {
    return;
  }
  auto & info = ::images.at(image.id);
  puleDeallocate(info.allocator, info.data);
  ::images.erase(image.id);
}

PuleBufferView puleAssetImageData(PuleAssetImage const image) {
  auto & info = ::images.at(image.id);
  return { .data = (uint8_t *)info.data, .byteLength = info.dataLength, };
}

void * puleAssetImageDecodedData(PuleAssetImage const image) {
  auto & info = ::images.at(image.id);
  return info.data;
}

size_t puleAssetImageDecodedDataLength(PuleAssetImage const image) {
  auto & info = ::images.at(image.id);
  return info.dataLength;
}

uint32_t puleAssetImageWidth(PuleAssetImage const image) {
  auto & info = ::images.at(image.id);
  return info.width;
}

uint32_t puleAssetImageHeight(PuleAssetImage const image) {
  auto & info = ::images.at(image.id);
  return info.height;
}

PuleF32v4 puleAssetImageTexel(
  PuleAssetImage image,
  uint32_t x,
  uint32_t y
) {
  auto & info = ::images.at(image.id);
  PuleF32v4 result;
  result.w = 1.0f;
  switch (info.format) {
    case PuleAssetImageFormat_rgbU8: {
      uint8_t const * const data = (uint8_t const *)info.data;
      result.x = data[3*(y*info.width + x) + 0] / 255.0f;
      result.y = data[3*(y*info.width + x) + 1] / 255.0f;
      result.z = data[3*(y*info.width + x) + 2] / 255.0f;
    } break;
    case PuleAssetImageFormat_rgbaU8: {
      uint8_t const * const data = (uint8_t const *)info.data;
      result.x = data[4*(y*info.width + x) + 0] / 255.0f;
      result.y = data[4*(y*info.width + x) + 1] / 255.0f;
      result.z = data[4*(y*info.width + x) + 2] / 255.0f;
      result.w = data[4*(y*info.width + x) + 3] / 255.0f;
    } break;
    default: PULE_assert(false);
  }
  return result;
}

void puleAssetImageTexelSet(
  PuleAssetImage image,
  uint32_t x,
  uint32_t y,
  PuleF32v4 const texel
) {
  auto & info = ::images.at(image.id);
  switch (info.format) {
    case PuleAssetImageFormat_rgbU8: {
      uint8_t * const data = (uint8_t *)info.data;
      data[3*(y*info.width + x) + 0] = texel.x * 255.0f;
      data[3*(y*info.width + x) + 1] = texel.y * 255.0f;
      data[3*(y*info.width + x) + 2] = texel.z * 255.0f;
    } break;
    case PuleAssetImageFormat_rgbaU8: {
      uint8_t * const data = (uint8_t *)info.data;
      data[4*(y*info.width + x) + 0] = texel.x * 255.0f;
      data[4*(y*info.width + x) + 1] = texel.y * 255.0f;
      data[4*(y*info.width + x) + 2] = texel.z * 255.0f;
      data[4*(y*info.width + x) + 3] = texel.w * 255.0f;
    } break;
    default: PULE_assert(false);
  }
}

} // C

extern "C" {
PuleAssetImageSupportFlag puleAssetImageExtensionSupported(
  char const * const extension
) {
  if (strcmp(extension, "png") == 0) {
    return PuleAssetImageSupportFlag_read;
  }
  if (strcmp(extension, "ppm") == 0) {
    return PuleAssetImageSupportFlag_write;
  }
  return PuleAssetImageSupportFlag_none;
}
}
