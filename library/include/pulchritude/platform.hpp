/* auto generated file platform */
#pragma once
#include "core.hpp"

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
  static inline Platform create(PulePlatformCreateInfo info, PuleError * error) {
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
#include "string.hpp"
#include <string>
namespace pule { //tostr 
inline pule::str toStr(PulePlatformVsyncMode const e) {
  switch (e) {
    case PulePlatformVsyncMode_none: return puleString("none");
    case PulePlatformVsyncMode_double: return puleString("double");
    case PulePlatformVsyncMode_triple: return puleString("triple");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleErrorWindow const e) {
  switch (e) {
    case PuleErrorWindow_none: return puleString("none");
    case PuleErrorWindow_windowCreationFailed: return puleString("windowCreationFailed");
    case PuleErrorWindow_invalidConfiguration: return puleString("invalidConfiguration");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleInputKey const e) {
  switch (e) {
    case PuleInputKey_tab: return puleString("tab");
    case PuleInputKey_left: return puleString("left");
    case PuleInputKey_right: return puleString("right");
    case PuleInputKey_up: return puleString("up");
    case PuleInputKey_down: return puleString("down");
    case PuleInputKey_pageUp: return puleString("pageUp");
    case PuleInputKey_pageDown: return puleString("pageDown");
    case PuleInputKey_home: return puleString("home");
    case PuleInputKey_end: return puleString("end");
    case PuleInputKey_insert: return puleString("insert");
    case PuleInputKey_delete: return puleString("delete");
    case PuleInputKey_backspace: return puleString("backspace");
    case PuleInputKey_space: return puleString("space");
    case PuleInputKey_enter: return puleString("enter");
    case PuleInputKey_escape: return puleString("escape");
    case PuleInputKey_apostrophe: return puleString("apostrophe");
    case PuleInputKey_comma: return puleString("comma");
    case PuleInputKey_minus: return puleString("minus");
    case PuleInputKey_period: return puleString("period");
    case PuleInputKey_slash: return puleString("slash");
    case PuleInputKey_semicolon: return puleString("semicolon");
    case PuleInputKey_equal: return puleString("equal");
    case PuleInputKey_leftBracket: return puleString("leftBracket");
    case PuleInputKey_backslash: return puleString("backslash");
    case PuleInputKey_rightBracket: return puleString("rightBracket");
    case PuleInputKey_graveAccent: return puleString("graveAccent");
    case PuleInputKey_capsLock: return puleString("capsLock");
    case PuleInputKey_scrollLock: return puleString("scrollLock");
    case PuleInputKey_numLock: return puleString("numLock");
    case PuleInputKey_printScreen: return puleString("printScreen");
    case PuleInputKey_pause: return puleString("pause");
    case PuleInputKey_kp0: return puleString("kp0");
    case PuleInputKey_kp1: return puleString("kp1");
    case PuleInputKey_kp2: return puleString("kp2");
    case PuleInputKey_kp3: return puleString("kp3");
    case PuleInputKey_kp4: return puleString("kp4");
    case PuleInputKey_kp5: return puleString("kp5");
    case PuleInputKey_kp6: return puleString("kp6");
    case PuleInputKey_kp7: return puleString("kp7");
    case PuleInputKey_kp8: return puleString("kp8");
    case PuleInputKey_kp9: return puleString("kp9");
    case PuleInputKey_kpDecimal: return puleString("kpDecimal");
    case PuleInputKey_kpDivide: return puleString("kpDivide");
    case PuleInputKey_kpMultiply: return puleString("kpMultiply");
    case PuleInputKey_kpSubtract: return puleString("kpSubtract");
    case PuleInputKey_kpAdd: return puleString("kpAdd");
    case PuleInputKey_kpEnter: return puleString("kpEnter");
    case PuleInputKey_kpEqual: return puleString("kpEqual");
    case PuleInputKey_leftShift: return puleString("leftShift");
    case PuleInputKey_leftControl: return puleString("leftControl");
    case PuleInputKey_leftAlt: return puleString("leftAlt");
    case PuleInputKey_leftSuper: return puleString("leftSuper");
    case PuleInputKey_rightShift: return puleString("rightShift");
    case PuleInputKey_rightControl: return puleString("rightControl");
    case PuleInputKey_rightAlt: return puleString("rightAlt");
    case PuleInputKey_rightSuper: return puleString("rightSuper");
    case PuleInputKey_menu: return puleString("menu");
    case PuleInputKey_i0: return puleString("i0");
    case PuleInputKey_i1: return puleString("i1");
    case PuleInputKey_i2: return puleString("i2");
    case PuleInputKey_i3: return puleString("i3");
    case PuleInputKey_i4: return puleString("i4");
    case PuleInputKey_i5: return puleString("i5");
    case PuleInputKey_i6: return puleString("i6");
    case PuleInputKey_i7: return puleString("i7");
    case PuleInputKey_i8: return puleString("i8");
    case PuleInputKey_i9: return puleString("i9");
    case PuleInputKey_a: return puleString("a");
    case PuleInputKey_b: return puleString("b");
    case PuleInputKey_c: return puleString("c");
    case PuleInputKey_d: return puleString("d");
    case PuleInputKey_e: return puleString("e");
    case PuleInputKey_f: return puleString("f");
    case PuleInputKey_g: return puleString("g");
    case PuleInputKey_h: return puleString("h");
    case PuleInputKey_i: return puleString("i");
    case PuleInputKey_j: return puleString("j");
    case PuleInputKey_k: return puleString("k");
    case PuleInputKey_l: return puleString("l");
    case PuleInputKey_m: return puleString("m");
    case PuleInputKey_n: return puleString("n");
    case PuleInputKey_o: return puleString("o");
    case PuleInputKey_p: return puleString("p");
    case PuleInputKey_q: return puleString("q");
    case PuleInputKey_r: return puleString("r");
    case PuleInputKey_s: return puleString("s");
    case PuleInputKey_t: return puleString("t");
    case PuleInputKey_u: return puleString("u");
    case PuleInputKey_v: return puleString("v");
    case PuleInputKey_w: return puleString("w");
    case PuleInputKey_x: return puleString("x");
    case PuleInputKey_y: return puleString("y");
    case PuleInputKey_z: return puleString("z");
    case PuleInputKey_f1: return puleString("f1");
    case PuleInputKey_f2: return puleString("f2");
    case PuleInputKey_f3: return puleString("f3");
    case PuleInputKey_f4: return puleString("f4");
    case PuleInputKey_f5: return puleString("f5");
    case PuleInputKey_f6: return puleString("f6");
    case PuleInputKey_f7: return puleString("f7");
    case PuleInputKey_f8: return puleString("f8");
    case PuleInputKey_f9: return puleString("f9");
    case PuleInputKey_f10: return puleString("f10");
    case PuleInputKey_f11: return puleString("f11");
    case PuleInputKey_f12: return puleString("f12");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleInputKeyModifier const e) {
  std::string str = "( ";
  if (e & PuleInputKeyModifier_ctrl) {
    str += "ctrl | ";
  }
  if (e & PuleInputKeyModifier_shift) {
    str += "shift | ";
  }
  if (e & PuleInputKeyModifier_alt) {
    str += "alt | ";
  }
  if (e & PuleInputKeyModifier_super) {
    str += "super | ";
  }
  str += ")";
  PuleString strCp = puleString(str.c_str());
  return strCp;
}
inline pule::str toStr(PuleInputMouse const e) {
  switch (e) {
    case PuleInputMouse_left: return puleString("left");
    case PuleInputMouse_right: return puleString("right");
    case PuleInputMouse_middle: return puleString("middle");
    case PuleInputMouse_side1: return puleString("side1");
    case PuleInputMouse_side2: return puleString("side2");
    case PuleInputMouse_End: return puleString("End");
    default: return puleString("N/A");
  }
}
}
