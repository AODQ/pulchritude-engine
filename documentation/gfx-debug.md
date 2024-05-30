# gfx-debug

## structs
### PuleGfxDebugRecorder
```c
struct {
  id : uint64_t;
};
```
### PuleGfxDebugSubmitInfo
 Writes out the debug data to the render-pass 
```c
struct {
  recorder : PuleGfxDebugRecorder;
  commandListRecorder : PuleGpuCommandListRecorder;
  /* 
    need this to call the begin/end render-pass; it's a safety measure to
    ensure a render-pass is active when drawing
   */
  renderPassBegin : PuleGpuActionRenderPassBegin;
};
```
### PuleGfxDebugRenderLine
```c
struct {
  type : PuleGfxDebugRenderType = PuleGfxDebugRenderType_line;
  a : PuleF32v2;
  b : PuleF32v2;
  color : PuleF32v3;
};
```
### PuleGfxDebugRenderQuad
```c
struct {
  type : PuleGfxDebugRenderType = PuleGfxDebugRenderType_quad;
  originCenter : PuleF32v2;
  angle : float;
  dimensionsHalf : PuleF32v2;
  color : PuleF32v3;
};
```
### PuleGfxDebugRenderCube
```c
struct {
  type : PuleGfxDebugRenderType = PuleGfxDebugRenderType_cube;
  originCenter : PuleF32v3;
  rotationMatrix : PuleF32m33;
  dimensionsHalf : PuleF32v3;
  color : PuleF32v3;
};
```
### PuleGfxDebugRenderSphere
```c
struct {
  type : PuleGfxDebugRenderType = PuleGfxDebugRenderType_sphere;
  originCenter : PuleF32v3;
  radius : float;
  color : PuleF32v3;
};
```
### PuleGfxDebugRenderPlane
```c
struct {
  type : PuleGfxDebugRenderType = PuleGfxDebugRenderType_plane;
  originCenter : PuleF32v3;
  rotationMatrix : PuleF32m33;
  color : PuleF32v3;
};
```
### PuleGfxDebugRenderParam
```c
union {
  type : PuleGfxDebugRenderType;
  line : PuleGfxDebugRenderLine;
  quad : PuleGfxDebugRenderQuad;
  cube : PuleGfxDebugRenderCube;
  sphere : PuleGfxDebugRenderSphere;
  plane : PuleGfxDebugRenderPlane;
};
```

## enums
### PuleGfxDebugRenderType
```c
enum {
  line,
  quad,
  cube,
  sphere,
  plane,
}
```

## functions
### puleGfxDebugInitialize
```c
puleGfxDebugInitialize(
  platform : PulePlatform
) void;
```
### puleGfxDebugShutdown
```c
puleGfxDebugShutdown() void;
```
### puleGfxDebugStart
```c
puleGfxDebugStart(
  transform : PuleF32m44,
  view : PuleF32m44,
  projection : PuleF32m44
) PuleGfxDebugRecorder;
```
### puleGfxDebugSubmit
```c
puleGfxDebugSubmit(
  submitInfo : PuleGfxDebugSubmitInfo
) void;
```
### puleGfxDebugRender
```c
puleGfxDebugRender(
  debugRecorder : PuleGfxDebugRecorder,
  param : PuleGfxDebugRenderParam
) void;
```
