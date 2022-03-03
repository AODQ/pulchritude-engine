#include <util.hpp>

#include <pulchritude-log/log.h>

#include <glad/glad.h>

bool processGlErrors(
  char const * const file,
  int const line,
  char const * const function
) {
  bool errExit = false;
  while (auto err = glGetError()) {
    switch (err) {
      case GL_NO_ERROR: return errExit;
      case GL_INVALID_ENUM:
        puleLogError("GL_INVALID_ENUM %s@%d [%s]", file, line, function);
        errExit = true;
      break;
      case GL_INVALID_VALUE:
        puleLogError("GL_INVALID_VALUE %s@%d [%s]", file, line, function);
        errExit = true;
      break;
      case GL_INVALID_OPERATION:
        puleLogError("GL_INVALID_OPERATION %s@%d [%s]", file, line, function);
        errExit = true;
      break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        puleLogError("GL_INVALID_FRAMEBUFFER_OPERATION %s@%d [%s]", file, line, function);
        errExit = true;
      break;
      case GL_OUT_OF_MEMORY:
        puleLogError("GL_OUT_OF_MEMORY %s@%d [%s]", file, line, function);
        errExit = true;
      break;
    }
  }
  return errExit;
}
