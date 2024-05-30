#include <pulchritude/asset-font.h>

#include <pulchritude/core.hpp>
#include <pulchritude/file.h>

#include <schrift.h>

#include <vector>
#include <unordered_map>

namespace pint {
  pule::ResourceContainer<std::vector<uint8_t>> fontMemory;
  std::unordered_map<uint64_t, uint64_t> sftFontToFontMemory;
}

PuleAssetFont puleAssetFontLoad(
  PuleBufferView const fontSource,
  PuleError * const error
) {
  // retain font memory
  uint64_t fontMemoryId = pint::fontMemory.create(
    std::vector<uint8_t>(fontSource.data, fontSource.data+fontSource.byteLength)
  );
  uint8_t * const fontMemoryPtr = pint::fontMemory.at(fontMemoryId)->data();
  SFT_Font * const font = sft_loadmem(fontMemoryPtr, fontSource.byteLength);
  if (!font) {
    puleLogError("fuckers failed to load font");
    PULE_error(PuleErrorAssetFont_decode, "failed to decode font");
    pint::fontMemory.destroy(fontMemoryId);
    return {};
  }
  pint::sftFontToFontMemory.emplace(
    reinterpret_cast<uint64_t>(font), fontMemoryId
  );
  return PuleAssetFont { .id = reinterpret_cast<uint64_t>(font), };
}

PuleAssetFont puleAssetFontLoadFromPath(
  PuleStringView const path,
  PuleError * const error
) {
  PuleError err = puleError();
  PuleFile const file = (
    puleFileOpen(path, PuleFileDataMode_binary, PuleFileOpenMode_read, &err)
  );
  if (puleErrorConsume(&err)) { return {.id = 0,}; }
  uint64_t const filesize = puleFileSize(file);
  std::vector<uint8_t> filedata;
  filedata.resize(filesize);
  puleFileReadBytes(
    file,
    PuleBufferViewMutable {
      .data = filedata.data(),
      .byteLength = filesize,
    }
  );
  puleFileClose(file);
  return (
    puleAssetFontLoad(
      PuleBufferView {
        .data = filedata.data(),
        .byteLength = filesize,
      },
      error
    )
  );
}

void puleAssetFontDestroy(PuleAssetFont const font) {
  pint::fontMemory.destroy(pint::sftFontToFontMemory.at(font.id));
  sft_freefont(reinterpret_cast<SFT_Font *>(font.id));
}

PULE_exportFn void puleAssetFontRenderToU8Buffer(
  PuleAssetFontRenderInfo const info,
  PuleError * const error
) {
  SFT_Font * const sftFont = reinterpret_cast<SFT_Font *>(info.font.id);
  SFT const sftRequest = {
    .font = sftFont,
    .xScale = (double)info.fontScale.x,
    .yScale = (double)info.fontScale.y,
    .xOffset = (double)info.fontOffset.x,
    .yOffset = (double)info.fontOffset.y,
    .flags = info.renderFlippedY ? SFT_DOWNWARD_Y : 0,
  };

  SFT_Glyph glyph;
  if (sft_lookup(&sftRequest, info.glyphCodepoint, &glyph) < 0) {
    PULE_error(
      PuleErrorAssetFont_decode,
      "failed to lookup glyph for codepoint %u", info.glyphCodepoint
    );
    return;
  }

  SFT_Image const imageDestination = {
    .pixels = info.destinationBuffer.data,
    .width = int(info.destinationBufferDim.x),
    .height = int(info.destinationBufferDim.y),
  };
  puleLog(
    "rendering glyph %c width %d height %d",
    info.glyphCodepoint,
    imageDestination.width,
    imageDestination.height
  );
  if (sft_render(&sftRequest, glyph, imageDestination) < 0) {
    PULE_error(
      PuleErrorAssetFont_decode,
      "failed to render glyph %u", info.glyphCodepoint
    );
    return;
  }
}
