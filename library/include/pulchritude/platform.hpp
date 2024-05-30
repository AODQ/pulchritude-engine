/* auto generated file platform */
#pragma once
#include "core.h"

#include "platform.h"
#include "error.hpp"
#include "math.hpp"
#include "string.hpp"

namespace pule {
using PlatformCreateInfo = PulePlatformCreateInfo;
}
namespace pule {
struct Platform {
  PulePlatform _handle;
  inline operator PulePlatform() const {
    return _handle;
  }
  inline void destroy() {
    return pulePlatformDestroy(this->_handle);
  }
  inline bool shouldExit() {
    return pulePlatformShouldExit(this->_handle);
  }
  inline void pollEvents() {
    return pulePlatformPollEvents(this->_handle);
  }
  inline void swapFramebuffer() {
    return pulePlatformSwapFramebuffer(this->_handle);
  }
  inline PuleI32v2 windowSize() {
    return pulePlatformWindowSize(this->_handle);
  }
  inline PuleI32v2 framebufferSize() {
    return pulePlatformFramebufferSize(this->_handle);
  }
  inline bool null() {
    return pulePlatformNull(this->_handle);
  }
  inline bool focused() {
    return pulePlatformFocused(this->_handle);
  }
  inline void mouseOriginSet(PuleI32v2 origin) {
    return pulePlatformMouseOriginSet(this->_handle, origin);
  }
  inline PuleI32v2 mouseOrigin() {
    return pulePlatformMouseOrigin(this->_handle);
  }
  inline bool cursorEnabled() {
    return pulePlatformCursorEnabled(this->_handle);
  }
  inline void cursorHide() {
    return pulePlatformCursorHide(this->_handle);
  }
  inline void cursorShow() {
    return pulePlatformCursorShow(this->_handle);
  }
  static inline Platform create(PulePlatformCreateInfo info, struct PuleError * error) {
    return { ._handle = pulePlatformCreate(info, error),};
  }
};
}
  inline void destroy(pule::Platform self) {
    return pulePlatformDestroy(self._handle);
  }
  inline bool shouldExit(pule::Platform self) {
    return pulePlatformShouldExit(self._handle);
  }
  inline void pollEvents(pule::Platform self) {
    return pulePlatformPollEvents(self._handle);
  }
  inline void swapFramebuffer(pule::Platform self) {
    return pulePlatformSwapFramebuffer(self._handle);
  }
  inline PuleI32v2 windowSize(pule::Platform self) {
    return pulePlatformWindowSize(self._handle);
  }
  inline PuleI32v2 framebufferSize(pule::Platform self) {
    return pulePlatformFramebufferSize(self._handle);
  }
  inline bool null(pule::Platform self) {
    return pulePlatformNull(self._handle);
  }
  inline bool focused(pule::Platform self) {
    return pulePlatformFocused(self._handle);
  }
  inline void mouseOriginSet(pule::Platform self, PuleI32v2 origin) {
    return pulePlatformMouseOriginSet(self._handle, origin);
  }
  inline PuleI32v2 mouseOrigin(pule::Platform self) {
    return pulePlatformMouseOrigin(self._handle);
  }
  inline bool cursorEnabled(pule::Platform self) {
    return pulePlatformCursorEnabled(self._handle);
  }
  inline void cursorHide(pule::Platform self) {
    return pulePlatformCursorHide(self._handle);
  }
  inline void cursorShow(pule::Platform self) {
    return pulePlatformCursorShow(self._handle);
  }
namespace pule {
using PlatformFramebufferResizeCallbackCreateInfo = PulePlatformFramebufferResizeCallbackCreateInfo;
}
namespace pule {
using PlatformWindowResizeCallbackCreateInfo = PulePlatformWindowResizeCallbackCreateInfo;
}
namespace pule {
using InputKeyCallbackCreateInfo = PuleInputKeyCallbackCreateInfo;
}
namespace pule {
using InputMouseButtonCallbackCreateInfo = PuleInputMouseButtonCallbackCreateInfo;
}
namespace pule {
using InputRawTextCallbackCreateInfo = PuleInputRawTextCallbackCreateInfo;
}
namespace pule {
inline char const * toStr(PulePlatformVsyncMode const e) {
  switch (e) {
    case PulePlatformVsyncMode_none: return "none";
    case PulePlatformVsyncMode_double: return "double";
    case PulePlatformVsyncMode_triple: return "triple";
    default: return "N/A";
  }
}
inline char const * toStr(PuleErrorWindow const e) {
  switch (e) {
    case PuleErrorWindow_none: return "none";
    case PuleErrorWindow_windowCreationFailed: return "windowCreationFailed";
    case PuleErrorWindow_invalidConfiguration: return "invalidConfiguration";
    default: return "N/A";
  }
}
inline char const * toStr(PuleInputKey const e) {
  switch (e) {
    case PuleInputKey_tab: return "tab";
    case PuleInputKey_left: return "left";
    case PuleInputKey_right: return "right";
    case PuleInputKey_up: return "up";
    case PuleInputKey_down: return "down";
    case PuleInputKey_pageUp: return "pageUp";
    case PuleInputKey_pageDown: return "pageDown";
    case PuleInputKey_home: return "home";
    case PuleInputKey_end: return "end";
    case PuleInputKey_insert: return "insert";
    case PuleInputKey_delete: return "delete";
    case PuleInputKey_backspace: return "backspace";
    case PuleInputKey_space: return "space";
    case PuleInputKey_enter: return "enter";
    case PuleInputKey_escape: return "escape";
    case PuleInputKey_apostrophe: return "apostrophe";
    case PuleInputKey_comma: return "comma";
    case PuleInputKey_minus: return "minus";
    case PuleInputKey_period: return "period";
    case PuleInputKey_slash: return "slash";
    case PuleInputKey_semicolon: return "semicolon";
    case PuleInputKey_equal: return "equal";
    case PuleInputKey_leftBracket: return "leftBracket";
    case PuleInputKey_backslash: return "backslash";
    case PuleInputKey_rightBracket: return "rightBracket";
    case PuleInputKey_graveAccent: return "graveAccent";
    case PuleInputKey_capsLock: return "capsLock";
    case PuleInputKey_scrollLock: return "scrollLock";
    case PuleInputKey_numLock: return "numLock";
    case PuleInputKey_printScreen: return "printScreen";
    case PuleInputKey_pause: return "pause";
    case PuleInputKey_kp0: return "kp0";
    case PuleInputKey_kp1: return "kp1";
    case PuleInputKey_kp2: return "kp2";
    case PuleInputKey_kp3: return "kp3";
    case PuleInputKey_kp4: return "kp4";
    case PuleInputKey_kp5: return "kp5";
    case PuleInputKey_kp6: return "kp6";
    case PuleInputKey_kp7: return "kp7";
    case PuleInputKey_kp8: return "kp8";
    case PuleInputKey_kp9: return "kp9";
    case PuleInputKey_kpDecimal: return "kpDecimal";
    case PuleInputKey_kpDivide: return "kpDivide";
    case PuleInputKey_kpMultiply: return "kpMultiply";
    case PuleInputKey_kpSubtract: return "kpSubtract";
    case PuleInputKey_kpAdd: return "kpAdd";
    case PuleInputKey_kpEnter: return "kpEnter";
    case PuleInputKey_kpEqual: return "kpEqual";
    case PuleInputKey_leftShift: return "leftShift";
    case PuleInputKey_leftControl: return "leftControl";
    case PuleInputKey_leftAlt: return "leftAlt";
    case PuleInputKey_leftSuper: return "leftSuper";
    case PuleInputKey_rightShift: return "rightShift";
    case PuleInputKey_rightControl: return "rightControl";
    case PuleInputKey_rightAlt: return "rightAlt";
    case PuleInputKey_rightSuper: return "rightSuper";
    case PuleInputKey_menu: return "menu";
    case PuleInputKey_i0: return "i0";
    case PuleInputKey_i1: return "i1";
    case PuleInputKey_i2: return "i2";
    case PuleInputKey_i3: return "i3";
    case PuleInputKey_i4: return "i4";
    case PuleInputKey_i5: return "i5";
    case PuleInputKey_i6: return "i6";
    case PuleInputKey_i7: return "i7";
    case PuleInputKey_i8: return "i8";
    case PuleInputKey_i9: return "i9";
    case PuleInputKey_a: return "a";
    case PuleInputKey_b: return "b";
    case PuleInputKey_c: return "c";
    case PuleInputKey_d: return "d";
    case PuleInputKey_e: return "e";
    case PuleInputKey_f: return "f";
    case PuleInputKey_g: return "g";
    case PuleInputKey_h: return "h";
    case PuleInputKey_i: return "i";
    case PuleInputKey_j: return "j";
    case PuleInputKey_k: return "k";
    case PuleInputKey_l: return "l";
    case PuleInputKey_m: return "m";
    case PuleInputKey_n: return "n";
    case PuleInputKey_o: return "o";
    case PuleInputKey_p: return "p";
    case PuleInputKey_q: return "q";
    case PuleInputKey_r: return "r";
    case PuleInputKey_s: return "s";
    case PuleInputKey_t: return "t";
    case PuleInputKey_u: return "u";
    case PuleInputKey_v: return "v";
    case PuleInputKey_w: return "w";
    case PuleInputKey_x: return "x";
    case PuleInputKey_y: return "y";
    case PuleInputKey_z: return "z";
    case PuleInputKey_f1: return "f1";
    case PuleInputKey_f2: return "f2";
    case PuleInputKey_f3: return "f3";
    case PuleInputKey_f4: return "f4";
    case PuleInputKey_f5: return "f5";
    case PuleInputKey_f6: return "f6";
    case PuleInputKey_f7: return "f7";
    case PuleInputKey_f8: return "f8";
    case PuleInputKey_f9: return "f9";
    case PuleInputKey_f10: return "f10";
    case PuleInputKey_f11: return "f11";
    case PuleInputKey_f12: return "f12";
    default: return "N/A";
  }
}
inline char const * toStr(PuleInputKeyModifier const e) {
  switch (e) {
    case PuleInputKeyModifier_ctrl: return "ctrl";
    case PuleInputKeyModifier_shift: return "shift";
    case PuleInputKeyModifier_alt: return "alt";
    case PuleInputKeyModifier_super: return "super";
    default: return "N/A";
  }
}
inline char const * toStr(PuleInputMouse const e) {
  switch (e) {
    case PuleInputMouse_left: return "left";
    case PuleInputMouse_right: return "right";
    case PuleInputMouse_middle: return "middle";
    case PuleInputMouse_side1: return "side1";
    case PuleInputMouse_side2: return "side2";
    case PuleInputMouse_End: return "End";
    default: return "N/A";
  }
}
}
