#include <pulchritude-asset/image.h>

#include <pulchritude-log/log.h>

#include <spng.h>

#include <unordered_map>

namespace {
int32_t imageFormatToSpng(PuleAssetImageFormat const fmt) {
  switch (fmt) {
    default: PULE_assert(false);
    case PuleAssetImageFormat_rgbaU8:  return SPNG_FMT_RGBA8;
    case PuleAssetImageFormat_rgbaU16: return SPNG_FMT_RGBA16;
    case PuleAssetImageFormat_rgbU8:   return SPNG_FMT_RGB8;
  }
}

struct ImageInfo {
  PuleAllocator allocator;
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
  puleLogDebug("reading length: %zu", length);
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
  PuleAssetImageFormat const requestedFormat,
  PuleError * const error
) {
  // TODO use new2 mapping spng_alloc to PuleAllocator
  spng_ctx * context = spng_ctx_new(SPNG_CTX_IGNORE_ADLER32);
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
    PULE_error(PuleErrorAssetImage_decode, "failed to get image size");
    spng_ctx_free(context);
    return { 0 };
  }
  spng_ihdr ihdr;
  if (spng_get_ihdr(context, &ihdr) != SPNG_OK) {
    PULE_error(PuleErrorAssetImage_decode, "failed to get image ihdr info");
    spng_ctx_free(context);
    return { 0 };
  }

  puleLogDebug(
    "loading image with length: %zu, with resolution: %ux%u",
    imageLength, ihdr.width, ihdr.height
  );

  void * data = (
    puleAllocate(
      allocator,
      PuleAllocateInfo {
        .zeroOut = false,
        .numBytes = imageLength,
        .alignment = 0,
      }
    )
  );

  if (spng_decode_image(context, data, imageLength, format, 0) != SPNG_OK) {
    PULE_error(PuleErrorAssetImage_decode, "failed to decode image");
    spng_ctx_free(context);
    return { 0 };
  }

  spng_ctx_free(context);

  ::images[::imagesIt] = {
    .allocator = allocator,
    .data = data,
    .dataLength = imageLength,
    .width = ihdr.width,
    .height = ihdr.height,
  };
  return { ::imagesIt ++ };
}

void puleAssetImageDestroy(PuleAssetImage const image) {
  if (image.id == 0) {
    return;
  }
  auto & info = ::images.at(image.id);
  puleDeallocate(info.allocator, info.data);
  ::images.erase(image.id);
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

} // C
