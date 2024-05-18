#version 460 core

layout (location = 0) in vec3 inOrigin;
layout (location = 1) in vec3 inUv;
layout (location = 2) in vec3 inColor;

layout(push_constant) uniform PushConstants {
  mat4 transform;
  mat4 cameraViewProj;
};

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec3 outUv;
layout (location = 2) out vec3 outNormal;
layout (location = 3) out vec3 outOrigin;

out gl_PerVertex {
  vec4 gl_Position;
};

void main() {
  gl_Position = transform * vec4(inOrigin, 1.0);
  gl_Position = cameraViewProj * gl_Position;

  outOrigin = inOrigin;
  outNormal = normalize(inOrigin);
  outColor = vec4(inColor, 1.0);
  outUv = inUv;
}
