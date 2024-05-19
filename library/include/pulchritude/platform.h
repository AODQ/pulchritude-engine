/* auto generated file platform */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/error.h>
#include <pulchritude/math.h>
#include <pulchritude/string.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
  PuleStringView name;
  /*  <= 7680, can leave at 0 for full screen  */
  size_t width;
  /*  // <= 4320, can leave at 0 for full screen  */
  size_t height;
  PulePlatformVsyncMode vsyncMode;
} PulePlatformCreateInfo;
typedef struct {
  uint64_t id;
} PulePlatform;
typedef struct {
  void(* callback)(PuleI32v2);
} PulePlatformFramebufferResizeCallbackCreateInfo;
typedef struct {
  void(* callback)(PuleI32v2);
} PulePlatformWindowResizeCallbackCreateInfo;
/*  TODO fix this to allow multiline in parser (bool waspressed) */
typedef struct {
  void(* callback)(PuleInputKey, PuleInputKeyModifier, bool);
} PuleInputKeyCallbackCreateInfo;
typedef struct {
  void(* callback)(PuleInputMouse, PuleInputKeyModifier, bool);
} PuleInputMouseButtonCallbackCreateInfo;
typedef struct {
  void(* callback)(uint32_t);
} PuleInputRawTextCallbackCreateInfo;

// enum
typedef enum {
  PulePlatformVsyncMode_none,
  PulePlatformVsyncMode_double,
  PulePlatformVsyncMode_triple,
} PulePlatformVsyncMode;
typedef enum {
  PuleErrorwindow_none,
  PuleErrorWindow_windowCreationFailed,
  PuleErrorWindow_invalidConfiguration,
} PuleErrorWindow;
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
  PuleInputKeyModifier_ctrl,
  PuleInputKeyModifier_shift,
  PuleInputKeyModifier_alt,
  PuleInputKeyModifier_super,
} PuleInputKeyModifier;
typedef enum {
  PuleInputMouse_left,
  PuleInputMouse_right,
  PuleInputMouse_middle,
  PuleInputMouse_side1,
  PuleInputMouse_side2,
  PuleInputMouse_End,
} PuleInputMouse;

// entities

// functions
PULE_exportFn void pulePlatformInitialize(PuleError * error);
PULE_exportFn void pulePlatformShutdown();
PULE_exportFn PulePlatform pulePlatformCreate(PulePlatformCreateInfo info, PuleError * error);
PULE_exportFn void pulePlatformDestroy(PulePlatform window);
PULE_exportFn bool pulePlatformShouldExit(PulePlatform window);
/* 
  TODO these need to pass in platform (puleGpuInitialize)
 */
PULE_exportFn void * pulePlatformGetProcessAddress();
PULE_exportFn char const * * pulePlatformRequiredExtensions(uint32_t * count);
PULE_exportFn void pulePlatformPollEvents(PulePlatform window);
PULE_exportFn void pulePlatformSwapFramebuffer(PulePlatform window);
PULE_exportFn PuleI32v2 pulePlatformWindowSize(PulePlatform window);
PULE_exportFn PuleI32v2 pulePlatformFramebufferSize(PulePlatform window);
PULE_exportFn void pulePlatformFramebufferResizeCallback(PulePlatformFramebufferResizeCallbackCreateInfo info);
PULE_exportFn void pulePlatformWindowResizeCallback(PulePlatformWindowResizeCallbackCreateInfo info);
PULE_exportFn double pulePlatformGetTime();
PULE_exportFn bool pulePlatformNull(PulePlatform window);
PULE_exportFn bool pulePlatformFocused(PulePlatform window);
PULE_exportFn void pulePlatformMouseOriginSet(PulePlatform window, PuleI32v2 origin);
PULE_exportFn PuleI32v2 pulePlatformMouseOrigin(PulePlatform window);
PULE_exportFn bool pulePlatformCursorEnabled(PulePlatform window);
PULE_exportFn void pulePlatformCursorHide(PulePlatform window);
PULE_exportFn void pulePlatformCursorShow(PulePlatform window);
PULE_exportFn bool puleInputKey(PulePlatform window, PuleInputKey key);
PULE_exportFn PuleInputKeyModifier puleInputKeyModifiers(PulePlatform window);
PULE_exportFn bool puleInputMouse(PulePlatform window, PuleInputMouse mouseKey);
PULE_exportFn int32_t puleInputScroll(PulePlatform window);
/* 
callback for keyboard input, for event-driven applications such as text
editors. It's recommended to use 'puleInputKey' if you need to poll the input
every frame, like for player movement input.
 */
PULE_exportFn void puleInputKeyCallback(PuleInputKeyCallbackCreateInfo info);
/* 
callback for mouse input, for event-driven applications. It's recommended to
use 'puleInputMouse' if you need to poll input every frame, like for player
direction input.
 */
PULE_exportFn void puleInputMouseButtonCallback(PuleInputMouseButtonCallbackCreateInfo info);
/* 
  callback for text input, with UTF8 support, for text editors
 */
PULE_exportFn void puleInputRawTextCallback(PuleInputRawTextCallbackCreateInfo info);

#ifdef __cplusplus
} // extern C
#endif
