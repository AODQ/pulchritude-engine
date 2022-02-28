#include <pulchritude-window/window.h>

#include <pulchritude-error/error.h>

#include <GLFW/glfw3.h>

extern "C" {

PuleWindow puleWindowCreate(
  PuleWindowCreateInfo const info,
  PuleError * const error
) {
  PULE_errorAssert(glfwInit(), PuleErrorWindow_windowCreationFailed, {});

  // TODO glfwSetErrorCallback

  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
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

  if (windowWidth > 0 && windowHeight > 0) {
    int32_t xpos, ypos;
    glfwGetMonitorWorkarea(
      glfwGetPrimaryMonitor(), &xpos, &ypos, &windowWidth, &windowHeight
    );
  }

  GLFWwindow * glfwWindow = (
    glfwCreateWindow(
      windowWidth, windowHeight, info.name.contents, nullptr, nullptr
    )
  );

  PULE_errorAssert(glfwWindow, PuleErrorWindow_windowCreationFailed, {});

  PuleWindow windowOut;
  windowOut.data = reinterpret_cast<void *>(glfwWindow);
  glfwMakeContextCurrent(glfwWindow);

  /* PULE_errorAssert( */
  /*   gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)), */
  /*   PuleErrorWindow_windowCreationFailed, */
  /*   nullptr */
  /* ); */

  switch (info.vsyncMode) {
    case PuleWindowVsyncMode_triple:
      // I don't think triple (3) will work as intended in GLFW, nor is it an
      //   option in OpenGL so set it to 1
    case PuleWindowVsyncMode_single:
      glfwSwapInterval(1);
    break;
    default:
      glfwSwapInterval(0);
    break;
  }

  return windowOut;
}

void puleWindowDestroy(PuleWindow const window) {
  glfwDestroyWindow(reinterpret_cast<GLFWwindow *>(window.data));
}

void puleWindowPollEvents([[maybe_unused]] PuleWindow const window) {
  glfwPollEvents();
}

void puleWindowSwapFramebuffer(PuleWindow const window) {
  glfwSwapBuffers(reinterpret_cast<GLFWwindow *>(window.data));
}

} // extern C
