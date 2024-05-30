/* auto generated file platform */
#pragma once
#include "core.h"

#include "error.h"
#include "math.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PulePlatformVsyncMode_none = 0,
  PulePlatformVsyncMode_double = 1,
  PulePlatformVsyncMode_triple = 2,
} PulePlatformVsyncMode;
const uint32_t PulePlatformVsyncModeSize = 3;
typedef enum {
  PuleErrorWindow_none = 0,
  PuleErrorWindow_windowCreationFailed = 1,
  PuleErrorWindow_invalidConfiguration = 2,
} PuleErrorWindow;
const uint32_t PuleErrorWindowSize = 3;
typedef enum {
  PuleInputKey_tab = 0,
  PuleInputKey_left = 1,
  PuleInputKey_right = 2,
  PuleInputKey_up = 3,
  PuleInputKey_down = 4,
  PuleInputKey_pageUp = 5,
  PuleInputKey_pageDown = 6,
  PuleInputKey_home = 7,
  PuleInputKey_end = 8,
  PuleInputKey_insert = 9,
  PuleInputKey_delete = 10,
  PuleInputKey_backspace = 11,
  PuleInputKey_space = 12,
  PuleInputKey_enter = 13,
  PuleInputKey_escape = 14,
  PuleInputKey_apostrophe = 15,
  PuleInputKey_comma = 16,
  PuleInputKey_minus = 17,
  PuleInputKey_period = 18,
  PuleInputKey_slash = 19,
  PuleInputKey_semicolon = 20,
  PuleInputKey_equal = 21,
  PuleInputKey_leftBracket = 22,
  PuleInputKey_backslash = 23,
  PuleInputKey_rightBracket = 24,
  PuleInputKey_graveAccent = 25,
  PuleInputKey_capsLock = 26,
  PuleInputKey_scrollLock = 27,
  PuleInputKey_numLock = 28,
  PuleInputKey_printScreen = 29,
  PuleInputKey_pause = 30,
  PuleInputKey_kp0 = 31,
  PuleInputKey_kp1 = 32,
  PuleInputKey_kp2 = 33,
  PuleInputKey_kp3 = 34,
  PuleInputKey_kp4 = 35,
  PuleInputKey_kp5 = 36,
  PuleInputKey_kp6 = 37,
  PuleInputKey_kp7 = 38,
  PuleInputKey_kp8 = 39,
  PuleInputKey_kp9 = 40,
  PuleInputKey_kpDecimal = 41,
  PuleInputKey_kpDivide = 42,
  PuleInputKey_kpMultiply = 43,
  PuleInputKey_kpSubtract = 44,
  PuleInputKey_kpAdd = 45,
  PuleInputKey_kpEnter = 46,
  PuleInputKey_kpEqual = 47,
  PuleInputKey_leftShift = 48,
  PuleInputKey_leftControl = 49,
  PuleInputKey_leftAlt = 50,
  PuleInputKey_leftSuper = 51,
  PuleInputKey_rightShift = 52,
  PuleInputKey_rightControl = 53,
  PuleInputKey_rightAlt = 54,
  PuleInputKey_rightSuper = 55,
  PuleInputKey_menu = 56,
  PuleInputKey_i0 = 57,
  PuleInputKey_i1 = 58,
  PuleInputKey_i2 = 59,
  PuleInputKey_i3 = 60,
  PuleInputKey_i4 = 61,
  PuleInputKey_i5 = 62,
  PuleInputKey_i6 = 63,
  PuleInputKey_i7 = 64,
  PuleInputKey_i8 = 65,
  PuleInputKey_i9 = 66,
  PuleInputKey_a = 67,
  PuleInputKey_b = 68,
  PuleInputKey_c = 69,
  PuleInputKey_d = 70,
  PuleInputKey_e = 71,
  PuleInputKey_f = 72,
  PuleInputKey_g = 73,
  PuleInputKey_h = 74,
  PuleInputKey_i = 75,
  PuleInputKey_j = 76,
  PuleInputKey_k = 77,
  PuleInputKey_l = 78,
  PuleInputKey_m = 79,
  PuleInputKey_n = 80,
  PuleInputKey_o = 81,
  PuleInputKey_p = 82,
  PuleInputKey_q = 83,
  PuleInputKey_r = 84,
  PuleInputKey_s = 85,
  PuleInputKey_t = 86,
  PuleInputKey_u = 87,
  PuleInputKey_v = 88,
  PuleInputKey_w = 89,
  PuleInputKey_x = 90,
  PuleInputKey_y = 91,
  PuleInputKey_z = 92,
  PuleInputKey_f1 = 93,
  PuleInputKey_f2 = 94,
  PuleInputKey_f3 = 95,
  PuleInputKey_f4 = 96,
  PuleInputKey_f5 = 97,
  PuleInputKey_f6 = 98,
  PuleInputKey_f7 = 99,
  PuleInputKey_f8 = 100,
  PuleInputKey_f9 = 101,
  PuleInputKey_f10 = 102,
  PuleInputKey_f11 = 103,
  PuleInputKey_f12 = 104,
} PuleInputKey;
const uint32_t PuleInputKeySize = 105;
typedef enum {
  PuleInputKeyModifier_ctrl = 1,
  PuleInputKeyModifier_shift = 2,
  PuleInputKeyModifier_alt = 4,
  PuleInputKeyModifier_super = 8,
} PuleInputKeyModifier;
const uint32_t PuleInputKeyModifierSize = 4;
typedef enum {
  PuleInputMouse_left = 0,
  PuleInputMouse_right = 1,
  PuleInputMouse_middle = 2,
  PuleInputMouse_side1 = 3,
  PuleInputMouse_side2 = 4,
  PuleInputMouse_End = 5,
} PuleInputMouse;
const uint32_t PuleInputMouseSize = 6;

// entities

// structs
struct PulePlatformCreateInfo;
struct PulePlatform;
struct PulePlatformFramebufferResizeCallbackCreateInfo;
struct PulePlatformWindowResizeCallbackCreateInfo;
struct PuleInputKeyCallbackCreateInfo;
struct PuleInputMouseButtonCallbackCreateInfo;
struct PuleInputRawTextCallbackCreateInfo;

typedef struct PulePlatformCreateInfo {
  PuleStringView name;
  /*  <= 7680, can leave at 0 for full screen  */
  size_t width;
  /*  // <= 4320, can leave at 0 for full screen  */
  size_t height;
  PulePlatformVsyncMode vsyncMode;
} PulePlatformCreateInfo;
typedef struct PulePlatform {
  uint64_t id;
} PulePlatform;
typedef struct PulePlatformFramebufferResizeCallbackCreateInfo {
  void(* callback)(PuleI32v2);
} PulePlatformFramebufferResizeCallbackCreateInfo;
typedef struct PulePlatformWindowResizeCallbackCreateInfo {
  void(* callback)(PuleI32v2);
} PulePlatformWindowResizeCallbackCreateInfo;
/*  TODO fix this to allow multiline in parser (bool waspressed) */
typedef struct PuleInputKeyCallbackCreateInfo {
  void(* callback)(PuleInputKey, PuleInputKeyModifier, bool);
} PuleInputKeyCallbackCreateInfo;
typedef struct PuleInputMouseButtonCallbackCreateInfo {
  void(* callback)(PuleInputMouse, PuleInputKeyModifier, bool);
} PuleInputMouseButtonCallbackCreateInfo;
typedef struct PuleInputRawTextCallbackCreateInfo {
  void(* callback)(uint32_t);
} PuleInputRawTextCallbackCreateInfo;

// functions
PULE_exportFn void pulePlatformInitialize(struct PuleError * error);
PULE_exportFn void pulePlatformShutdown();
PULE_exportFn PulePlatform pulePlatformCreate(PulePlatformCreateInfo info, struct PuleError * error);
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
