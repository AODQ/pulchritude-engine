//#include <bulkher-window/window.h>

extern "C" {

PuleWindow puleWindowCreate(
  char const * const windowName,
  size_t const windowWidth,
  size_t const windowHeight
) {
  PuleWindow window;

  GLFWWindow * window = (
    glfwCreateWindow(
      windowWidth, windowHeight, windowName, nullptr, nullptr
    )
  );

  if (!window) {
  }
}

} // extern C
