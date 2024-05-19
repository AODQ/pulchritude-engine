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
  PulePlatformVsyncMode_none,
  PulePlatformVsyncMode_double,
  PulePlatformVsyncMode_triple,
}
```
### PuleErrorWindow
```c
enum {
  PuleErrorwindow_none,
  PuleErrorWindow_windowCreationFailed,
  PuleErrorWindow_invalidConfiguration,
}
```
### PuleInputKey
```c
enum {
  PuleInputKey_tab,
  PuleInputKey_left,
  PuleInputKey_right,
  PuleInputKey_up,
  PuleInputKey_down,
  PuleInputKey_pageUp,
  PuleInputKey_pageDown,
  PuleInputKey_home,
  PuleInputKey_end,
  PuleInputKey_insert,
  PuleInputKey_delete,
  PuleInputKey_backspace,
  PuleInputKey_space,
  PuleInputKey_enter,
  PuleInputKey_escape,
  PuleInputKey_apostrophe,
  PuleInputKey_comma,
  PuleInputKey_minus,
  PuleInputKey_period,
  PuleInputKey_slash,
  PuleInputKey_semicolon,
  PuleInputKey_equal,
  PuleInputKey_leftBracket,
  PuleInputKey_backslash,
  PuleInputKey_rightBracket,
  PuleInputKey_graveAccent,
  PuleInputKey_capsLock,
  PuleInputKey_scrollLock,
  PuleInputKey_numLock,
  PuleInputKey_printScreen,
  PuleInputKey_pause,
  PuleInputKey_kp0,
  PuleInputKey_kp1,
  PuleInputKey_kp2,
  PuleInputKey_kp3,
  PuleInputKey_kp4,
  PuleInputKey_kp5,
  PuleInputKey_kp6,
  PuleInputKey_kp7,
  PuleInputKey_kp8,
  PuleInputKey_kp9,
  PuleInputKey_kpDecimal,
  PuleInputKey_kpDivide,
  PuleInputKey_kpMultiply,
  PuleInputKey_kpSubtract,
  PuleInputKey_kpAdd,
  PuleInputKey_kpEnter,
  PuleInputKey_kpEqual,
  PuleInputKey_leftShift,
  PuleInputKey_leftControl,
  PuleInputKey_leftAlt,
  PuleInputKey_leftSuper,
  PuleInputKey_rightShift,
  PuleInputKey_rightControl,
  PuleInputKey_rightAlt,
  PuleInputKey_rightSuper,
  PuleInputKey_menu,
  PuleInputKey_i0,
  PuleInputKey_i1,
  PuleInputKey_i2,
  PuleInputKey_i3,
  PuleInputKey_i4,
  PuleInputKey_i5,
  PuleInputKey_i6,
  PuleInputKey_i7,
  PuleInputKey_i8,
  PuleInputKey_i9,
  PuleInputKey_a,
  PuleInputKey_b,
  PuleInputKey_c,
  PuleInputKey_d,
  PuleInputKey_e,
  PuleInputKey_f,
  PuleInputKey_g,
  PuleInputKey_h,
  PuleInputKey_i,
  PuleInputKey_j,
  PuleInputKey_k,
  PuleInputKey_l,
  PuleInputKey_m,
  PuleInputKey_n,
  PuleInputKey_o,
  PuleInputKey_p,
  PuleInputKey_q,
  PuleInputKey_r,
  PuleInputKey_s,
  PuleInputKey_t,
  PuleInputKey_u,
  PuleInputKey_v,
  PuleInputKey_w,
  PuleInputKey_x,
  PuleInputKey_y,
  PuleInputKey_z,
  PuleInputKey_f1,
  PuleInputKey_f2,
  PuleInputKey_f3,
  PuleInputKey_f4,
  PuleInputKey_f5,
  PuleInputKey_f6,
  PuleInputKey_f7,
  PuleInputKey_f8,
  PuleInputKey_f9,
  PuleInputKey_f10,
  PuleInputKey_f11,
  PuleInputKey_f12,
}
```
### PuleInputKeyModifier
```c
enum {
  PuleInputKeyModifier_ctrl,
  PuleInputKeyModifier_shift,
  PuleInputKeyModifier_alt,
  PuleInputKeyModifier_super,
}
```
### PuleInputMouse
```c
enum {
  PuleInputMouse_left,
  PuleInputMouse_right,
  PuleInputMouse_middle,
  PuleInputMouse_side1,
  PuleInputMouse_side2,
  PuleInputMouse_End,
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
