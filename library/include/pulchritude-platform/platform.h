#pragma once

#include <stdbool.h>

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-math/math.h>
#include <pulchritude-string/string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PulePlatformVsyncMode_none,
  PulePlatformVsyncMode_double,
  PulePlatformVsyncMode_triple,
  PulePlatformVsyncMode_End,
  PulePlatformVsyncMode_MaxEnum = 0x7FFFFFFF,
} PulePlatformVsyncMode;

typedef enum {
  PuleErrorwindow_none,
  PuleErrorWindow_windowCreationFailed,
  PuleErrorWindow_invalidConfiguration,
} PuleErrorWindow;

typedef struct {
  PuleStringView name;
  size_t width; // <= 7680, can leave at 0 for full screen
  size_t height; // <= 4320, can leave at 0 for full screen
  PulePlatformVsyncMode vsyncMode;
} PulePlatformCreateInfo;

typedef struct {
  uint64_t id;
} PulePlatform;

PULE_exportFn void pulePlatformInitialize(PuleError * const error);
PULE_exportFn void pulePlatformShutdown();
PULE_exportFn PulePlatform pulePlatformCreate(
  PulePlatformCreateInfo const info, PuleError * const error
);
PULE_exportFn void pulePlatformDestroy(PulePlatform const window);

PULE_exportFn bool pulePlatformShouldExit(PulePlatform const window);

PULE_exportFn void * pulePlatformGetProcessAddress();

PULE_exportFn void pulePlatformPollEvents(PulePlatform const window);
PULE_exportFn void pulePlatformSwapFramebuffer(PulePlatform const window);

PULE_exportFn PuleI32v2 pulePlatformWindowSize(
  PulePlatform const window
);
PULE_exportFn PuleI32v2 pulePlatformFramebufferSize(PulePlatform const window);

typedef struct {
  void (* callback)(PuleI32v2 const
      dimensions);
} PulePlatformFramebufferResizeCallbackCreateInfo;

PULE_exportFn void pulePlatformFramebufferResizeCallback(
  PulePlatformFramebufferResizeCallbackCreateInfo const info
);

typedef struct {
  void (* callback)(PuleI32v2 const
      dimensions);
} PulePlatformWindowResizeCallbackCreateInfo;

PULE_exportFn void pulePlatformWindowResizeCallback(
  PulePlatformWindowResizeCallbackCreateInfo const info
);

PULE_exportFn double pulePlatformGetTime();

PULE_exportFn bool pulePlatformNull(PulePlatform const window);

PULE_exportFn bool pulePlatformFocused(PulePlatform const window);

PULE_exportFn void pulePlatformMouseOriginSet(
  PulePlatform const window, PuleI32v2 const origin
);
PULE_exportFn PuleI32v2 pulePlatformMouseOrigin(
  PulePlatform const window
);
/* PULE_exportFn PuleI32v2 pulePlatformMouseOriginPrevious( */
/*   PulePlatform const platform, uint32_t const prevFrame */
/* ); */
PULE_exportFn bool pulePlatformCursorEnabled(PulePlatform const window);
PULE_exportFn void pulePlatformCursorHide(PulePlatform const window);
PULE_exportFn void pulePlatformCursorShow(PulePlatform const window);

typedef enum {
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
} PuleInputKey;

typedef enum {
  PuleInputKeyModifier_ctrl = 0x1,
  PuleInputKeyModifier_shift = 0x2,
  PuleInputKeyModifier_alt = 0x4,
  PuleInputKeyModifier_super = 0x8,
} PuleInputKeyModifier;

typedef enum {
  PuleInputMouse_left,
  PuleInputMouse_right,
  PuleInputMouse_middle,
  PuleInputMouse_side1,
  PuleInputMouse_side2,
  PuleInputMouse_End,
} PuleInputMouse;

PULE_exportFn bool puleInputKey(
  PulePlatform const window,
  PuleInputKey const key
);
PULE_exportFn PuleInputKeyModifier puleInputKeyModifiers(
  PulePlatform const window
);
PULE_exportFn bool puleInputMouse(
  PulePlatform const window,
  PuleInputMouse const mouseKey
);
PULE_exportFn int32_t puleInputScroll(PulePlatform const window);

// TODO fix the json exporter to allow function pointers in params
typedef struct {
  // TODO fix this to allow multiline in parser
  void (* const callback)(
    PuleInputKey const inputKey,
    PuleInputKeyModifier const keyModifier,
    bool const wasPressed
  );
} PuleInputKeyCallbackCreateInfo;

// callback for keyboard input, for event-driven applications such as text
//   editors. It's recommended to use `puleInputKey` if you need to poll the
//   input every frame, like for player movement input.
PULE_exportFn void puleInputKeyCallback(
  PuleInputKeyCallbackCreateInfo const info
);

typedef struct {
  void (* const callback)(
    PuleInputMouse const mouse,
    PuleInputKeyModifier const keyModifier,
    bool const wasPressed
  );
} PuleInputMouseButtonCallbackCreateInfo;

// callback for mouse input, for event-driven applications. It's recommended to
//   use `puleInputMouse` if you need to poll input every frame, like for
//   player direction input.
PULE_exportFn void puleInputMouseButtonCallback(
  PuleInputMouseButtonCallbackCreateInfo const info
);

typedef struct {
  void (* const callback)(uint32_t const character);
} PuleInputRawTextCallbackCreateInfo;

// callback for text input, with UTF8 support, for text editors
PULE_exportFn void puleInputRawTextCallback(
  PuleInputRawTextCallbackCreateInfo const info
);

#ifdef __cplusplus
} // extern C
#endif
