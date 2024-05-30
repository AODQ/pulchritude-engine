# text

## structs
### PuleTextRenderer
```c
struct {
  id : uint64_t;
};
```
### PuleTextRenderInfo
```c
struct {
  fontScale : size_t;
  transform : PuleF32m44;
  color : PuleF32v4;
  text : PuleStringView;
};
```
### PuleTextRender2DInfo
```c
struct {
  fontScale : size_t;
  position : PuleI32v2;
  color : PuleF32v4;
  text : PuleStringView;
};
```

## enums
### PuleTextType
```c
enum {
  bitmap,
  sdf,
}
```

## functions
### puleTextRendererCreate
```c
puleTextRendererCreate(
  font : PuleAssetFont,
  type : PuleTextType
) PuleTextRenderer;
```
### puleTextRendererDestroy
```c
puleTextRendererDestroy(
  renderer : PuleTextRenderer
) void;
```
### puleTextRender
```c
puleTextRender(
  textRenderer : PuleTextRenderer,
  commandListRecorder : PuleGpuCommandListRecorder,
  textInfo : PuleTextRenderInfo const ptr,
  textInfoCount : size_t
) void;
```
### puleTextRender2D
```c
puleTextRender2D(
  textRenderer : PuleTextRenderer,
  commandListRecorder : PuleGpuCommandListRecorder,
  textInfo : PuleTextRender2DInfo const ptr,
  textInfoCount : size_t
) void;
```
