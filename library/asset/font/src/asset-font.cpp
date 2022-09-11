#include <pulchritude-asset/font.h>

#include <schrift.h>

#include <vector>
#include <unordered_map>

PuleAssetFont puleAssetFontLoad(
  PuleBufferView const fontSource,
  PuleError * const error
) {
  SFT_Font * const font = sft_loadmem(fontSource.data, fontSource.byteLength);
  if (!font) {
    PULE_error(PuleErrorAssetFont_decode, "failed to decode font");
    return {};
  }
  return PuleAssetFont { .id = reinterpret_cast<uint64_t>(font), };
}

void puleAssetFontDestroy(PuleAssetFont const font) {
  sft_freefont(reinterpret_cast<SFT_Font *>(font.id));
}

PULE_exportFn void puleAssetFontRenderToU8Buffer(
  PuleAssetFontRenderInfo const info,
  PuleError * const error
) {
  SFT_Font * const sftFont = reinterpret_cast<SFT_Font *>(info.font.id);
  SFT const sftRequest = {
    .font = sftFont,
    .xScale = info.fontScale.x,
    .yScale = info.fontScale.y,
    .xOffset = info.fontOffset.x,
    .yOffset = info.fontOffset.y,
    .flags = info.renderFlippedY ? SFT_DOWNWARD_Y : 0,
  };

  SFT_Glyph glyph;
  if (sft_lookup(&sftRequest, info.glyphCodepoint, &glyph)) {
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
  if (sft_render(&sftRequest, glyph, imageDestination)) {
    PULE_error(
      PuleErrorAssetFont_decode,
      "failed to render glyph %u", info.glyphCodepoint
    );
    return;
  }
}
