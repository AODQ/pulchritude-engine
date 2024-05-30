# asset-font

## structs
### PuleAssetFontRenderInfo
```c
struct {
  font : PuleAssetFont;
  fontScale : PuleF32v2;
  fontOffset : PuleF32v2;
  renderFlippedY : bool;
  glyphCodepoint : uint32_t;
  destinationBuffer : PuleBufferViewMutable;
  destinationBufferDim : PuleU32v2;
};
```

## enums
### PuleErrorAssetFont
```c
enum {
  none,
  decode,
}
```

## entities
### PuleAssetFont

## functions
### puleAssetFontLoad
```c
puleAssetFontLoad(
  fontSource : PuleBufferView,
  error : PuleError ptr
) PuleAssetFont;
```
### puleAssetFontLoadFromPath
```c
puleAssetFontLoadFromPath(
  path : PuleStringView,
  error : PuleError ptr
) PuleAssetFont;
```
### puleAssetFontDestroy
```c
puleAssetFontDestroy(
  font : PuleAssetFont
) void;
```
### puleAssetFontRenderToU8Buffer

    the buffer must be packed as {
      color[0].a, color[1].a, .., color[width].a, color[1+width].a, ...
    }

```c
puleAssetFontRenderToU8Buffer(
  info : PuleAssetFontRenderInfo,
  error : PuleError ptr
) void;
```
