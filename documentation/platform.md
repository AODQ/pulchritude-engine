# platform

## structs
### PulePlatformCreateInfo
```c
struct {
  name : PuleStringView;
  /*  <= 7680, can leave at 0 for full screen  */
  width : size_t;
  /*  // <= 4320, can leave at 0 for full screen  */
  height : size_t;
  vsyncMode : PulePlatformVsyncMode;
};
```
### PulePlatform
```c
struct {
  id : uint64_t;
};
```
### PulePlatformFramebufferResizeCallbackCreateInfo
```c
struct {
  callback : @fnptr(void, PuleI32v2);
};
```
### PulePlatformWindowResizeCallbackCreateInfo
```c
struct {
  callback : @fnptr(void, PuleI32v2);
};
```
### PuleInputKeyCallbackCreateInfo
 TODO fix this to allow multiline in parser (bool waspressed)
```c
struct {
  callback : @fnptr(void, PuleInputKey, PuleInputKeyModifier, bool);
};
```
### PuleInputMouseButtonCallbackCreateInfo
```c
struct {
  callback : @fnptr(void, PuleInputMouse, PuleInputKeyModifier, bool);
};
```
### PuleInputRawTextCallbackCreateInfo
```c
struct {
  callback : @fnptr(void, uint32_t);
};
```

## enums
### PulePlatformVsyncMode
```c
enum {
  none,
  double,
  triple,
}
```
### PuleErrorWindow
```c
enum {
  none,
  windowCreationFailed,
  invalidConfiguration,
}
```
### PuleInputKey
```c
enum {
  tab,
  left,
  right,
  up,
  down,
  pageUp,
  pageDown,
  home,
  end,
  insert,
  delete,
  backspace,
  space,
  enter,
  escape,
  apostrophe,
  comma,
  minus,
  period,
  slash,
  semicolon,
  equal,
  leftBracket,
  backslash,
  rightBracket,
  graveAccent,
  capsLock,
  scrollLock,
  numLock,
  printScreen,
  pause,
  kp0,
  kp1,
  kp2,
  kp3,
  kp4,
  kp5,
  kp6,
  kp7,
  kp8,
  kp9,
  kpDecimal,
  kpDivide,
  kpMultiply,
  kpSubtract,
  kpAdd,
  kpEnter,
  kpEqual,
  leftShift,
  leftControl,
  leftAlt,
  leftSuper,
  rightShift,
  rightControl,
  rightAlt,
  rightSuper,
  menu,
  i0,
  i1,
  i2,
  i3,
  i4,
  i5,
  i6,
  i7,
  i8,
  i9,
  a,
  b,
  c,
  d,
  e,
  f,
  g,
  h,
  i,
  j,
  k,
  l,
  m,
  n,
  o,
  p,
  q,
  r,
  s,
  t,
  u,
  v,
  w,
  x,
  y,
  z,
  f1,
  f2,
  f3,
  f4,
  f5,
  f6,
  f7,
  f8,
  f9,
  f10,
  f11,
  f12,
}
```
### PuleInputKeyModifier
```c
enum {
  ctrl,
  shift,
  alt,
  super,
}
```
### PuleInputMouse
```c
enum {
  left,
  right,
  middle,
  side1,
  side2,
  End,
}
```

## functions
### pulePlatformInitialize
```c
pulePlatformInitialize(
  error : PuleError ptr
) void;
```
### pulePlatformShutdown
```c
pulePlatformShutdown() void;
```
### pulePlatformCreate
```c
pulePlatformCreate(
  info : PulePlatformCreateInfo,
  error : PuleError ptr
) PulePlatform;
```
### pulePlatformDestroy
```c
pulePlatformDestroy(
  window : PulePlatform
) void;
```
### pulePlatformShouldExit
```c
pulePlatformShouldExit(
  window : PulePlatform
) bool;
```
### pulePlatformGetProcessAddress

  TODO these need to pass in platform (puleGpuInitialize)

```c
pulePlatformGetProcessAddress() void ptr;
```
### pulePlatformRequiredExtensions
```c
pulePlatformRequiredExtensions(
  count : uint32_t ptr
) char const ptr ptr;
```
### pulePlatformPollEvents
```c
pulePlatformPollEvents(
  window : PulePlatform
) void;
```
### pulePlatformSwapFramebuffer
```c
pulePlatformSwapFramebuffer(
  window : PulePlatform
) void;
```
### pulePlatformWindowSize
```c
pulePlatformWindowSize(
  window : PulePlatform
) PuleI32v2;
```
### pulePlatformFramebufferSize
```c
pulePlatformFramebufferSize(
  window : PulePlatform
) PuleI32v2;
```
### pulePlatformFramebufferResizeCallback
```c
pulePlatformFramebufferResizeCallback(
  info : PulePlatformFramebufferResizeCallbackCreateInfo
) void;
```
### pulePlatformWindowResizeCallback
```c
pulePlatformWindowResizeCallback(
  info : PulePlatformWindowResizeCallbackCreateInfo
) void;
```
### pulePlatformGetTime
```c
pulePlatformGetTime() double;
```
### pulePlatformNull
```c
pulePlatformNull(
  window : PulePlatform
) bool;
```
### pulePlatformFocused
```c
pulePlatformFocused(
  window : PulePlatform
) bool;
```
### pulePlatformMouseOriginSet
```c
pulePlatformMouseOriginSet(
  window : PulePlatform,
  origin : PuleI32v2
) void;
```
### pulePlatformMouseOrigin
```c
pulePlatformMouseOrigin(
  window : PulePlatform
) PuleI32v2;
```
### pulePlatformCursorEnabled
```c
pulePlatformCursorEnabled(
  window : PulePlatform
) bool;
```
### pulePlatformCursorHide
```c
pulePlatformCursorHide(
  window : PulePlatform
) void;
```
### pulePlatformCursorShow
```c
pulePlatformCursorShow(
  window : PulePlatform
) void;
```
### puleInputKey
```c
puleInputKey(
  window : PulePlatform,
  key : PuleInputKey
) bool;
```
### puleInputKeyModifiers
```c
puleInputKeyModifiers(
  window : PulePlatform
) PuleInputKeyModifier;
```
### puleInputMouse
```c
puleInputMouse(
  window : PulePlatform,
  mouseKey : PuleInputMouse
) bool;
```
### puleInputScroll
```c
puleInputScroll(
  window : PulePlatform
) int32_t;
```
### puleInputKeyCallback

callback for keyboard input, for event-driven applications such as text
editors. It's recommended to use 'puleInputKey' if you need to poll the input
every frame, like for player movement input.

```c
puleInputKeyCallback(
  info : PuleInputKeyCallbackCreateInfo
) void;
```
### puleInputMouseButtonCallback

callback for mouse input, for event-driven applications. It's recommended to
use 'puleInputMouse' if you need to poll input every frame, like for player
direction input.

```c
puleInputMouseButtonCallback(
  info : PuleInputMouseButtonCallbackCreateInfo
) void;
```
### puleInputRawTextCallback

  callback for text input, with UTF8 support, for text editors

```c
puleInputRawTextCallback(
  info : PuleInputRawTextCallbackCreateInfo
) void;
```
