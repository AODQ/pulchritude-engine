#include <pulchritude-platform/platform.h>

#include <pulchritude-error/error.h>
#include <pulchritude-log/log.h>

#include <GLFW/glfw3.h>

#include <cstring>
#include <vector>

extern "C" {

static void inputKeyCallback(
  GLFWwindow * window,
  int32_t keycode, int32_t scancode, int32_t action, int32_t modifiers
);
static void inputMouseButtonCallback(
  GLFWwindow * window, int32_t button, int32_t action, int32_t modifiers
);
static void inputRawTextCallback(GLFWwindow * window, uint32_t rawCharacter);
static void framebufferResizeCallback(GLFWwindow *, int32_t, int32_t);
static void windowResizeCallback(GLFWwindow * w, int32_t, int32_t);

static void scrollCallback(
  GLFWwindow * window, double xoffset, double yoffset
) {
  (void)window; (void)xoffset; (void) yoffset;
  // need to find a way to reset to 0 at end/begin of frame
  /* mouseWheel += static_cast<int32_t>(yoffset); */
}

void pulePlatformInitialize(PuleError * const error) {
  PULE_errorAssert(glfwInit(), PuleErrorWindow_windowCreationFailed, );
}

void pulePlatformShutdown() {
  glfwTerminate();
}

PulePlatform pulePlatformCreate(
  PulePlatformCreateInfo const info,
  PuleError * const error
) {

  // TODO glfwSetErrorCallback

  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
  glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
  glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);
  glfwWindowHint(GLFW_FLOATING, GLFW_FALSE);
  glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
  glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_FALSE);
  glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_FALSE);
  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_SAMPLES, 0);

  glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);

  int32_t windowWidth  = static_cast<int32_t>(info.width);
  int32_t windowHeight = static_cast<int32_t>(info.height);

  // max 8K resolution for now
  PULE_errorAssert(
    windowWidth <= 7680 && windowHeight <= 4320,
    PuleErrorWindow_invalidConfiguration,
    {}
  );

  PULE_errorAssert(
    (windowWidth > 0) && (windowHeight > 0),
    PuleErrorWindow_invalidConfiguration,
    {}
  );

  if (windowWidth == 0 || windowHeight == 0) {
    int32_t xpos, ypos;
    glfwGetMonitorWorkarea(
      glfwGetPrimaryMonitor(), &xpos, &ypos, &windowWidth, &windowHeight
    );
  }

  puleLogDebug("creating window '%s'", info.name.contents);

  GLFWwindow * window = (
    glfwCreateWindow(
      windowWidth, windowHeight, info.name.contents, nullptr, nullptr
    )
  );

  PULE_errorAssert(window, PuleErrorWindow_windowCreationFailed, {});

  PulePlatform platform;
  platform.data = reinterpret_cast<void *>(window);
  glfwMakeContextCurrent(window);

  switch (info.vsyncMode) {
    case PulePlatformVsyncMode_triple:
      // I don't think triple (3) will work as intended in GLFW, nor is it an
      //   option in OpenGL so set it to 1
    case PulePlatformVsyncMode_double:
      glfwSwapInterval(1);
    break;
    default:
      glfwSwapInterval(0);
    break;
  }

  glfwSetScrollCallback(window, ::scrollCallback);
  glfwSetKeyCallback(window, ::inputKeyCallback);
  glfwSetMouseButtonCallback(window, ::inputMouseButtonCallback);
  glfwSetCharCallback(window, ::inputRawTextCallback);
  glfwSetFramebufferSizeCallback(window, ::framebufferResizeCallback);
  glfwSetWindowSizeCallback(window, ::windowResizeCallback);

  return platform;
}

void pulePlatformDestroy(PulePlatform const platform) {
  glfwDestroyWindow(reinterpret_cast<GLFWwindow *>(platform.data));
}

bool pulePlatformShouldExit(PulePlatform const platform) {
  return glfwWindowShouldClose(reinterpret_cast<GLFWwindow *>(platform.data));
}

void pulePlatformPollEvents([[maybe_unused]] PulePlatform const platform) {
  glfwPollEvents();
}

void pulePlatformSwapFramebuffer(PulePlatform const platform) {
  auto const glfwWindow = reinterpret_cast<GLFWwindow *>(platform.data);
  glfwSwapBuffers(glfwWindow);
}

void * pulePlatformGetProcessAddress() {
  return reinterpret_cast<void *>(&glfwGetProcAddress);
}

PuleI32v2 pulePlatformWindowSize(PulePlatform const platform) {
  auto const glfwWindow = reinterpret_cast<GLFWwindow *>(platform.data);
  PuleI32v2 dim;
  glfwGetWindowSize(glfwWindow, &dim.x, &dim.y);
  return dim;
}

PuleI32v2 pulePlatformFramebufferSize(PulePlatform const platform) {
  auto const glfwWindow = reinterpret_cast<GLFWwindow *>(platform.data);
  PuleI32v2 dim;
  glfwGetFramebufferSize(glfwWindow, &dim.x, &dim.y);
  return dim;
}

double pulePlatformGetTime() {
  return glfwGetTime();
}

bool pulePlatformNull(PulePlatform const platform) {
  return platform.data != nullptr;
}

bool pulePlatformFocused(PulePlatform const platform) {
  auto const glfwWindow = reinterpret_cast<GLFWwindow *>(platform.data);
  return glfwGetWindowAttrib(glfwWindow, GLFW_FOCUSED) != 0;
}

void pulePlatformMouseOriginSet(
  PulePlatform const platform,
  PuleI32v2 const origin
) {
  auto const glfwWindow = reinterpret_cast<GLFWwindow *>(platform.data);
  glfwSetCursorPos(
    glfwWindow,
    static_cast<double>(origin.x), static_cast<double>(origin.y)
  );
}

PuleI32v2 pulePlatformMouseOrigin(PulePlatform const platform) {
  auto const glfwWindow = reinterpret_cast<GLFWwindow *>(platform.data);
  double mousex, mousey;
  glfwGetCursorPos(glfwWindow, &mousex, &mousey);
  return (
    PuleI32v2 { static_cast<int32_t>(mousex), static_cast<int32_t>(mousey) }
  );
}

bool pulePlatformCursorEnabled(PulePlatform const platform) {
  auto const glfwWindow = reinterpret_cast<GLFWwindow *>(platform.data);
  return glfwGetInputMode(glfwWindow, GLFW_CURSOR) != GLFW_CURSOR_DISABLED;
}

void pulePlatformCursorHide(PulePlatform const platform) {
  auto const glfwWindow = reinterpret_cast<GLFWwindow *>(platform.data);
  glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void pulePlatformCursorShow(PulePlatform const platform) {
  auto const glfwWindow = reinterpret_cast<GLFWwindow *>(platform.data);
  glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

} // C

// -- INPUT --

namespace {

int32_t keyToGlfw(PuleInputKey const key) {
  switch (key) {
    default:
      puleLogError("unknown key mapping: %d", key);
      return 0;
    break;
    case PuleInputKey_tab: return GLFW_KEY_TAB;
    case PuleInputKey_left: return GLFW_KEY_LEFT;
    case PuleInputKey_right: return GLFW_KEY_RIGHT;
    case PuleInputKey_up: return GLFW_KEY_UP;
    case PuleInputKey_down: return GLFW_KEY_DOWN;
    case PuleInputKey_pageUp: return GLFW_KEY_PAGE_UP;
    case PuleInputKey_pageDown: return GLFW_KEY_PAGE_DOWN;
    case PuleInputKey_home: return GLFW_KEY_HOME;
    case PuleInputKey_end: return GLFW_KEY_END;
    case PuleInputKey_insert: return GLFW_KEY_INSERT;
    case PuleInputKey_delete: return GLFW_KEY_DELETE;
    case PuleInputKey_backspace: return GLFW_KEY_BACKSPACE;
    case PuleInputKey_space: return GLFW_KEY_SPACE;
    case PuleInputKey_enter: return GLFW_KEY_ENTER;
    case PuleInputKey_escape: return GLFW_KEY_ESCAPE;
    case PuleInputKey_apostrophe: return GLFW_KEY_APOSTROPHE;
    case PuleInputKey_comma: return GLFW_KEY_COMMA;
    case PuleInputKey_minus: return GLFW_KEY_MINUS;
    case PuleInputKey_period: return GLFW_KEY_PERIOD;
    case PuleInputKey_slash: return GLFW_KEY_SLASH;
    case PuleInputKey_semicolon: return GLFW_KEY_SEMICOLON;
    case PuleInputKey_equal: return GLFW_KEY_EQUAL;
    case PuleInputKey_leftBracket: return GLFW_KEY_LEFT_BRACKET;
    case PuleInputKey_backslash: return GLFW_KEY_BACKSLASH;
    case PuleInputKey_rightBracket: return GLFW_KEY_RIGHT_BRACKET;
    case PuleInputKey_graveAccent: return GLFW_KEY_GRAVE_ACCENT;
    case PuleInputKey_capsLock: return GLFW_KEY_CAPS_LOCK;
    case PuleInputKey_scrollLock: return GLFW_KEY_SCROLL_LOCK;
    case PuleInputKey_numLock: return GLFW_KEY_NUM_LOCK;
    case PuleInputKey_printScreen: return GLFW_KEY_PRINT_SCREEN;
    case PuleInputKey_pause: return GLFW_KEY_PAUSE;
    case PuleInputKey_kp0: return GLFW_KEY_KP_0;
    case PuleInputKey_kp1: return GLFW_KEY_KP_1;
    case PuleInputKey_kp2: return GLFW_KEY_KP_2;
    case PuleInputKey_kp3: return GLFW_KEY_KP_3;
    case PuleInputKey_kp4: return GLFW_KEY_KP_4;
    case PuleInputKey_kp5: return GLFW_KEY_KP_5;
    case PuleInputKey_kp6: return GLFW_KEY_KP_6;
    case PuleInputKey_kp7: return GLFW_KEY_KP_7;
    case PuleInputKey_kp8: return GLFW_KEY_KP_8;
    case PuleInputKey_kp9: return GLFW_KEY_KP_9;
    case PuleInputKey_kpDecimal: return GLFW_KEY_KP_DECIMAL;
    case PuleInputKey_kpDivide: return GLFW_KEY_KP_DIVIDE;
    case PuleInputKey_kpMultiply: return GLFW_KEY_KP_MULTIPLY;
    case PuleInputKey_kpSubtract: return GLFW_KEY_KP_SUBTRACT;
    case PuleInputKey_kpAdd: return GLFW_KEY_KP_ADD;
    case PuleInputKey_kpEnter: return GLFW_KEY_KP_ENTER;
    case PuleInputKey_kpEqual: return GLFW_KEY_KP_EQUAL;
    case PuleInputKey_leftShift: return GLFW_KEY_LEFT_SHIFT;
    case PuleInputKey_leftControl: return GLFW_KEY_LEFT_CONTROL;
    case PuleInputKey_leftAlt: return GLFW_KEY_LEFT_ALT;
    case PuleInputKey_leftSuper: return GLFW_KEY_LEFT_SUPER;
    case PuleInputKey_rightShift: return GLFW_KEY_RIGHT_SHIFT;
    case PuleInputKey_rightControl: return GLFW_KEY_RIGHT_CONTROL;
    case PuleInputKey_rightAlt: return GLFW_KEY_RIGHT_ALT;
    case PuleInputKey_rightSuper: return GLFW_KEY_RIGHT_SUPER;
    case PuleInputKey_menu: return GLFW_KEY_MENU;
    case PuleInputKey_i0: return GLFW_KEY_0;
    case PuleInputKey_i1: return GLFW_KEY_1;
    case PuleInputKey_i2: return GLFW_KEY_2;
    case PuleInputKey_i3: return GLFW_KEY_3;
    case PuleInputKey_i4: return GLFW_KEY_4;
    case PuleInputKey_i5: return GLFW_KEY_5;
    case PuleInputKey_i6: return GLFW_KEY_6;
    case PuleInputKey_i7: return GLFW_KEY_7;
    case PuleInputKey_i8: return GLFW_KEY_8;
    case PuleInputKey_i9: return GLFW_KEY_9;
    case PuleInputKey_a: return GLFW_KEY_A;
    case PuleInputKey_b: return GLFW_KEY_B;
    case PuleInputKey_c: return GLFW_KEY_C;
    case PuleInputKey_d: return GLFW_KEY_D;
    case PuleInputKey_e: return GLFW_KEY_E;
    case PuleInputKey_f: return GLFW_KEY_F;
    case PuleInputKey_g: return GLFW_KEY_G;
    case PuleInputKey_h: return GLFW_KEY_H;
    case PuleInputKey_i: return GLFW_KEY_I;
    case PuleInputKey_j: return GLFW_KEY_J;
    case PuleInputKey_k: return GLFW_KEY_K;
    case PuleInputKey_l: return GLFW_KEY_L;
    case PuleInputKey_m: return GLFW_KEY_M;
    case PuleInputKey_n: return GLFW_KEY_N;
    case PuleInputKey_o: return GLFW_KEY_O;
    case PuleInputKey_p: return GLFW_KEY_P;
    case PuleInputKey_q: return GLFW_KEY_Q;
    case PuleInputKey_r: return GLFW_KEY_R;
    case PuleInputKey_s: return GLFW_KEY_S;
    case PuleInputKey_t: return GLFW_KEY_T;
    case PuleInputKey_u: return GLFW_KEY_U;
    case PuleInputKey_v: return GLFW_KEY_V;
    case PuleInputKey_w: return GLFW_KEY_W;
    case PuleInputKey_x: return GLFW_KEY_X;
    case PuleInputKey_y: return GLFW_KEY_Y;
    case PuleInputKey_z: return GLFW_KEY_Z;
    case PuleInputKey_f1: return GLFW_KEY_F1;
    case PuleInputKey_f2: return GLFW_KEY_F2;
    case PuleInputKey_f3: return GLFW_KEY_F3;
    case PuleInputKey_f4: return GLFW_KEY_F4;
    case PuleInputKey_f5: return GLFW_KEY_F5;
    case PuleInputKey_f6: return GLFW_KEY_F6;
    case PuleInputKey_f7: return GLFW_KEY_F7;
    case PuleInputKey_f8: return GLFW_KEY_F8;
    case PuleInputKey_f9: return GLFW_KEY_F9;
    case PuleInputKey_f10: return GLFW_KEY_F10;
    case PuleInputKey_f11: return GLFW_KEY_F11;
    case PuleInputKey_f12: return GLFW_KEY_F12;
  }
}

PuleInputKey keyToPule(int32_t const key) {
  switch (key) {
    default:
      puleLogError("unknown key mapping: %d", key);
      return PuleInputKey_space;
    break;
    case GLFW_KEY_TAB: return PuleInputKey_tab;
    case GLFW_KEY_LEFT: return PuleInputKey_left;
    case GLFW_KEY_RIGHT: return PuleInputKey_right;
    case GLFW_KEY_UP: return PuleInputKey_up;
    case GLFW_KEY_DOWN: return PuleInputKey_down;
    case GLFW_KEY_PAGE_UP: return PuleInputKey_pageUp;
    case GLFW_KEY_PAGE_DOWN: return PuleInputKey_pageDown;
    case GLFW_KEY_HOME: return PuleInputKey_home;
    case GLFW_KEY_END: return PuleInputKey_end;
    case GLFW_KEY_INSERT: return PuleInputKey_insert;
    case GLFW_KEY_DELETE: return PuleInputKey_delete;
    case GLFW_KEY_BACKSPACE: return PuleInputKey_backspace;
    case GLFW_KEY_SPACE: return PuleInputKey_space;
    case GLFW_KEY_ENTER: return PuleInputKey_enter;
    case GLFW_KEY_ESCAPE: return PuleInputKey_escape;
    case GLFW_KEY_APOSTROPHE: return PuleInputKey_apostrophe;
    case GLFW_KEY_COMMA: return PuleInputKey_comma;
    case GLFW_KEY_MINUS: return PuleInputKey_minus;
    case GLFW_KEY_PERIOD: return PuleInputKey_period;
    case GLFW_KEY_SLASH: return PuleInputKey_slash;
    case GLFW_KEY_SEMICOLON: return PuleInputKey_semicolon;
    case GLFW_KEY_EQUAL: return PuleInputKey_equal;
    case GLFW_KEY_LEFT_BRACKET: return PuleInputKey_leftBracket;
    case GLFW_KEY_BACKSLASH: return PuleInputKey_backslash;
    case GLFW_KEY_RIGHT_BRACKET: return PuleInputKey_rightBracket;
    case GLFW_KEY_GRAVE_ACCENT: return PuleInputKey_graveAccent;
    case GLFW_KEY_CAPS_LOCK: return PuleInputKey_capsLock;
    case GLFW_KEY_SCROLL_LOCK: return PuleInputKey_scrollLock;
    case GLFW_KEY_NUM_LOCK: return PuleInputKey_numLock;
    case GLFW_KEY_PRINT_SCREEN: return PuleInputKey_printScreen;
    case GLFW_KEY_PAUSE: return PuleInputKey_pause;
    case GLFW_KEY_KP_0: return PuleInputKey_kp0;
    case GLFW_KEY_KP_1: return PuleInputKey_kp1;
    case GLFW_KEY_KP_2: return PuleInputKey_kp2;
    case GLFW_KEY_KP_3: return PuleInputKey_kp3;
    case GLFW_KEY_KP_4: return PuleInputKey_kp4;
    case GLFW_KEY_KP_5: return PuleInputKey_kp5;
    case GLFW_KEY_KP_6: return PuleInputKey_kp6;
    case GLFW_KEY_KP_7: return PuleInputKey_kp7;
    case GLFW_KEY_KP_8: return PuleInputKey_kp8;
    case GLFW_KEY_KP_9: return PuleInputKey_kp9;
    case GLFW_KEY_KP_DECIMAL: return PuleInputKey_kpDecimal;
    case GLFW_KEY_KP_DIVIDE: return PuleInputKey_kpDivide;
    case GLFW_KEY_KP_MULTIPLY: return PuleInputKey_kpMultiply;
    case GLFW_KEY_KP_SUBTRACT: return PuleInputKey_kpSubtract;
    case GLFW_KEY_KP_ADD: return PuleInputKey_kpAdd;
    case GLFW_KEY_KP_ENTER: return PuleInputKey_kpEnter;
    case GLFW_KEY_KP_EQUAL: return PuleInputKey_kpEqual;
    case GLFW_KEY_LEFT_SHIFT: return PuleInputKey_leftShift;
    case GLFW_KEY_LEFT_CONTROL: return PuleInputKey_leftControl;
    case GLFW_KEY_LEFT_ALT: return PuleInputKey_leftAlt;
    case GLFW_KEY_LEFT_SUPER: return PuleInputKey_leftSuper;
    case GLFW_KEY_RIGHT_SHIFT: return PuleInputKey_rightShift;
    case GLFW_KEY_RIGHT_CONTROL: return PuleInputKey_rightControl;
    case GLFW_KEY_RIGHT_ALT: return PuleInputKey_rightAlt;
    case GLFW_KEY_RIGHT_SUPER: return PuleInputKey_rightSuper;
    case GLFW_KEY_MENU: return PuleInputKey_menu;
    case GLFW_KEY_0: return PuleInputKey_i0;
    case GLFW_KEY_1: return PuleInputKey_i1;
    case GLFW_KEY_2: return PuleInputKey_i2;
    case GLFW_KEY_3: return PuleInputKey_i3;
    case GLFW_KEY_4: return PuleInputKey_i4;
    case GLFW_KEY_5: return PuleInputKey_i5;
    case GLFW_KEY_6: return PuleInputKey_i6;
    case GLFW_KEY_7: return PuleInputKey_i7;
    case GLFW_KEY_8: return PuleInputKey_i8;
    case GLFW_KEY_9: return PuleInputKey_i9;
    case GLFW_KEY_A: return PuleInputKey_a;
    case GLFW_KEY_B: return PuleInputKey_b;
    case GLFW_KEY_C: return PuleInputKey_c;
    case GLFW_KEY_D: return PuleInputKey_d;
    case GLFW_KEY_E: return PuleInputKey_e;
    case GLFW_KEY_F: return PuleInputKey_f;
    case GLFW_KEY_G: return PuleInputKey_g;
    case GLFW_KEY_H: return PuleInputKey_h;
    case GLFW_KEY_I: return PuleInputKey_i;
    case GLFW_KEY_J: return PuleInputKey_j;
    case GLFW_KEY_K: return PuleInputKey_k;
    case GLFW_KEY_L: return PuleInputKey_l;
    case GLFW_KEY_M: return PuleInputKey_m;
    case GLFW_KEY_N: return PuleInputKey_n;
    case GLFW_KEY_O: return PuleInputKey_o;
    case GLFW_KEY_P: return PuleInputKey_p;
    case GLFW_KEY_Q: return PuleInputKey_q;
    case GLFW_KEY_R: return PuleInputKey_r;
    case GLFW_KEY_S: return PuleInputKey_s;
    case GLFW_KEY_T: return PuleInputKey_t;
    case GLFW_KEY_U: return PuleInputKey_u;
    case GLFW_KEY_V: return PuleInputKey_v;
    case GLFW_KEY_W: return PuleInputKey_w;
    case GLFW_KEY_X: return PuleInputKey_x;
    case GLFW_KEY_Y: return PuleInputKey_y;
    case GLFW_KEY_Z: return PuleInputKey_z;
    case GLFW_KEY_F1: return PuleInputKey_f1;
    case GLFW_KEY_F2: return PuleInputKey_f2;
    case GLFW_KEY_F3: return PuleInputKey_f3;
    case GLFW_KEY_F4: return PuleInputKey_f4;
    case GLFW_KEY_F5: return PuleInputKey_f5;
    case GLFW_KEY_F6: return PuleInputKey_f6;
    case GLFW_KEY_F7: return PuleInputKey_f7;
    case GLFW_KEY_F8: return PuleInputKey_f8;
    case GLFW_KEY_F9: return PuleInputKey_f9;
    case GLFW_KEY_F10: return PuleInputKey_f10;
    case GLFW_KEY_F11: return PuleInputKey_f11;
    case GLFW_KEY_F12: return PuleInputKey_f12;
  }
}

PuleInputKeyModifier keyModifierToPule(int32_t const key) {
  int32_t modifier = 0;
  if (key & GLFW_MOD_CONTROL) {
    modifier |= PuleInputKeyModifier_ctrl;
  }
  if (key & GLFW_MOD_SHIFT) {
    modifier |= PuleInputKeyModifier_shift;
  }
  if (key & GLFW_MOD_ALT) {
    modifier |= PuleInputKeyModifier_alt;
  }
  if (key & GLFW_MOD_SUPER) {
    modifier |= PuleInputKeyModifier_super;
  }
  return static_cast<PuleInputKeyModifier>(modifier);
}

int32_t mouseKeyToGlfw(PuleInputMouse const key) {
  switch (key) {
    default:
      puleLogError("unknown mouse mapping: %d", key);
      return 0;
    break;
    case PuleInputMouse_left: return GLFW_MOUSE_BUTTON_LEFT;
    case PuleInputMouse_right: return GLFW_MOUSE_BUTTON_RIGHT;
    case PuleInputMouse_middle: return GLFW_MOUSE_BUTTON_MIDDLE;
    case PuleInputMouse_side1: return GLFW_MOUSE_BUTTON_4;
    case PuleInputMouse_side2: return GLFW_MOUSE_BUTTON_5;
  }
}

PuleInputMouse mouseKeyToPule(int32_t const key) {
  switch (key) {
    default:
      puleLogError("unknown mouse mapping: %d", key);
      return PuleInputMouse_left;
    break;
    case GLFW_MOUSE_BUTTON_LEFT: return PuleInputMouse_left;
    case GLFW_MOUSE_BUTTON_RIGHT: return PuleInputMouse_right;
    case GLFW_MOUSE_BUTTON_MIDDLE: return PuleInputMouse_middle;
    case GLFW_MOUSE_BUTTON_4: return PuleInputMouse_side1;
    case GLFW_MOUSE_BUTTON_5: return PuleInputMouse_side2;
  }
}

// TODO assign this to window
static int32_t mouseWheel;

} // namespace

extern "C" {

bool puleInputKey(PulePlatform const platform, PuleInputKey const key) {
  auto const glfwWindow = reinterpret_cast<GLFWwindow *>(platform.data);
  return glfwGetKey(glfwWindow, keyToGlfw(key)) == GLFW_PRESS;
}

PuleInputKeyModifier puleInputKeyModifiers(PulePlatform const platform) {
  return (
    static_cast<PuleInputKeyModifier>(
      PuleInputKeyModifier_ctrl * (
        puleInputKey(platform, PuleInputKey_leftControl)
        || puleInputKey(platform, PuleInputKey_rightControl)
      )
      | PuleInputKeyModifier_shift * (
        puleInputKey(platform, PuleInputKey_leftShift)
        || puleInputKey(platform, PuleInputKey_rightShift)
      )
      | PuleInputKeyModifier_alt * (
        puleInputKey(platform, PuleInputKey_leftAlt)
        || puleInputKey(platform, PuleInputKey_rightAlt)
      )
      | PuleInputKeyModifier_super * (
        puleInputKey(platform, PuleInputKey_leftSuper)
        || puleInputKey(platform, PuleInputKey_rightSuper)
      )
    )
  );
}

PULE_exportFn PuleInputKeyModifier puleInputKeyModifiers(
  PulePlatform const platform
);

bool puleInputMouse(PulePlatform const platform, PuleInputMouse const key) {
  auto const glfwWindow = reinterpret_cast<GLFWwindow *>(platform.data);
  return glfwGetMouseButton(glfwWindow, mouseKeyToGlfw(key)) == GLFW_PRESS;
}

int32_t puleInputScroll(PulePlatform const platform) {
  (void)platform;
  return ::mouseWheel;
}

} // extern C


// --- callback handling ---

namespace {

// TODO make this an array
std::vector<
  void (*)(PuleInputKey const, PuleInputKeyModifier const, bool const)
> userInputKeyCallbacks = {};
std::vector<
  void (*)(PuleInputMouse const, PuleInputKeyModifier const, bool const)
> userInputMouseButtonCallbacks = {};
std::vector<void (*)(PuleI32v2 const)> userFramebufferResizeCallbacks = {};
std::vector<void (*)(PuleI32v2 const)> userWindowResizeCallbacks = {};
std::vector<void (*)(uint32_t const)> userInputRawTextCallbacks = {};

// copied from glfw's imgui, untranslates GLFW's keys
int32_t translateKeyScancode(int32_t const key, int32_t const scancode) {
  int32_t fixedKey = key;
  if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_EQUAL) {
    return fixedKey;
  }
  char const * const keyName = glfwGetKeyName(key, scancode);
  if (keyName && keyName[0] != 0 && keyName[1] == 0) {
    char const charNames[] = "`-=[]\\,;\'./";
    int32_t const charKeys[] = {
       GLFW_KEY_GRAVE_ACCENT, GLFW_KEY_MINUS, GLFW_KEY_EQUAL,
       GLFW_KEY_LEFT_BRACKET, GLFW_KEY_RIGHT_BRACKET, GLFW_KEY_BACKSLASH,
       GLFW_KEY_COMMA, GLFW_KEY_SEMICOLON, GLFW_KEY_APOSTROPHE,
       GLFW_KEY_PERIOD, GLFW_KEY_SLASH, 0,
    };
    PULE_assert(PULE_arraySize(charNames) == PULE_arraySize(charKeys));
    if (keyName[0] >= '0' && keyName[0] <= '9') {
      fixedKey = GLFW_KEY_0 + (keyName[0] - '0');
    } else if (keyName[0] >= 'A' && keyName[0] <= 'Z') {
      fixedKey = GLFW_KEY_A + (keyName[0] - 'A');
    } else if (char const * const p = strchr(charNames, keyName[0])) {
      fixedKey = charKeys[p - charNames];
    }
  }
  return fixedKey;
}

} // namespace

extern "C" {

static void inputKeyCallback(
  GLFWwindow * platform,
  int32_t keycode, int32_t scancode, int32_t action, int32_t modifiers
) {
  (void)platform;
  if (action != GLFW_PRESS && action != GLFW_RELEASE) {
    return;
  }

  for (auto const callback : ::userInputKeyCallbacks) {
    PULE_assert(callback);
    callback(
      keyToPule(translateKeyScancode(keycode, scancode)),
      keyModifierToPule(modifiers),
      action == GLFW_PRESS
    );
  }
}

static void inputMouseButtonCallback(
  GLFWwindow * platform, int32_t button, int32_t action, int32_t modifiers
) {
  (void)platform;
  if (action != GLFW_PRESS && action != GLFW_RELEASE) {
    return;
  }
  for (auto const callback : ::userInputMouseButtonCallbacks) {
    callback(
      mouseKeyToPule(button),
      keyModifierToPule(modifiers),
      action == GLFW_PRESS
    );
  }
}

static void inputRawTextCallback(GLFWwindow * platform, uint32_t rawCharacter) {
  (void)platform;
  for (auto const callback : ::userInputRawTextCallbacks) {
    callback(rawCharacter);
  }
}

static void framebufferResizeCallback(
  GLFWwindow * platform, int32_t width, int32_t height
) {
  (void)platform;
  for (auto const callback : ::userFramebufferResizeCallbacks) {
    callback(PuleI32v2 { width, height });
  }
}

static void windowResizeCallback(
  GLFWwindow * platform, int32_t width, int32_t height
) {
  (void)platform;
  for (auto const callback : ::userWindowResizeCallbacks) {
    callback(PuleI32v2 { width, height });
  }
}

} // C

extern "C" {

void puleInputKeyCallback(PuleInputKeyCallbackCreateInfo const info) {
  if (info.callback == nullptr) {
    puleLogError("attempting to add null key callback");
    return;
  }
  ::userInputKeyCallbacks.emplace_back(info.callback);
}

void puleInputMouseButtonCallback(
  PuleInputMouseButtonCallbackCreateInfo const info
) {
  if (info.callback == nullptr) {
    puleLogError("attempting to add null mouse button callback");
    return;
  }
  ::userInputMouseButtonCallbacks.emplace_back(info.callback);
}

void puleInputRawTextCallback(PuleInputRawTextCallbackCreateInfo const info) {
  if (info.callback == nullptr) {
    puleLogError("attempting to add null raw text callback");
    return;
  }
  ::userInputRawTextCallbacks.emplace_back(info.callback);
}

void pulePlatformFramebufferResizeCallback(
  PulePlatformFramebufferResizeCallbackCreateInfo const info
) {
  if (info.callback == nullptr) {
    puleLogError("attempting to add null framebuffer resize callback");
    return;
  }
  ::userFramebufferResizeCallbacks.emplace_back(info.callback);
}

void pulePlatformWindowResizeCallback(
  PulePlatformWindowResizeCallbackCreateInfo const info
) {
  if (info.callback == nullptr) {
    puleLogError("attempting to add null framebuffer resize callback");
    return;
  }
  ::userWindowResizeCallbacks.emplace_back(info.callback);
}

} // extern C
