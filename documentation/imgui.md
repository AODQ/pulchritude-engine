# imgui

## structs
### PuleImguiCallback
```c
struct {
  callback : @fnptr(void);
};
```

## entities
### PuleImguiCallbackRegistry

usually this isn't necessary, but since everything is so heavily
module-based, it's easier to register callbacks to display information
from many different callbacks


## functions
### puleImguiInitialize
```c
puleImguiInitialize(
  window : PulePlatform
) void;
```
### puleImguiShutdown
```c
puleImguiShutdown() void;
```
### puleImguiNewFrame
```c
puleImguiNewFrame() void;
```
### puleImguiRender
```c
puleImguiRender(
  renderGraph : PuleRenderGraph
) void;
```
### puleImguiJoinNext
```c
puleImguiJoinNext() void;
```
### puleImguiSliderF32
```c
puleImguiSliderF32(
  label : char const ptr,
  data : float ptr,
  min : float,
  max : float
) bool;
```
### puleImguiSliderZu
```c
puleImguiSliderZu(
  label : char const ptr,
  data : size_t ptr,
  min : size_t,
  max : size_t
) bool;
```
### puleImguiWindowBegin
```c
puleImguiWindowBegin(
  labelFormat : char const ptr,
  openNullable : bool ptr
) void;
```
### puleImguiSectionBegin
```c
puleImguiSectionBegin(
  label : char const ptr
) bool;
```
### puleImguiSectionEnd
```c
puleImguiSectionEnd() void;
```
### puleImguiText
```c
puleImguiText(
  format : char const ptr,
  args : __VA_ARGS__
) void;
```
### puleImguiImage
```c
puleImguiImage(
  image : PuleGpuImage,
  size : PuleF32v2,
  uvMin : PuleF32v2,
  uvMax : PuleF32v2,
  borderColor : PuleF32v4
) void;
```
### puleImguiLastItemHovered
```c
puleImguiLastItemHovered() bool;
```
### puleImguiCurrentOrigin
```c
puleImguiCurrentOrigin() PuleI32v2;
```
### puleImguiToggle
```c
puleImguiToggle(
  label : char const ptr,
  data : bool ptr
) bool;
```
### puleImguiButton
```c
puleImguiButton(
  label : char const ptr
) bool;
```
### puleImguiCallbackRegister
```c
puleImguiCallbackRegister(
  callback : PuleImguiCallback
) PuleImguiCallbackRegistry;
```
### puleImguiCallbackUnregister
```c
puleImguiCallbackUnregister(
  registry : PuleImguiCallbackRegistry
) void;
```
### puleImguiCallbackShowAll
```c
puleImguiCallbackShowAll() void;
```
