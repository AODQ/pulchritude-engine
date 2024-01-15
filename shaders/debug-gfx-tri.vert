#version 460 core

layout(push_constant) uniform PushConstants {
  vec4 inOrigins[4];
  vec4 inUv[4];
  vec4 inColor;
};

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outUv;

out gl_PerVertex {
  vec4 gl_Position;
};

void main() {
  gl_Position = inOrigins[gl_VertexIndex];
  gl_Position.xy /= vec2(800.0f, 600.0f)*0.5f;
  outColor = inColor;
  outUv = inUv[gl_VertexIndex];
}
