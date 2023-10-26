#version 460 core

layout(push_constant) uniform PushConstants {
  vec4 line;
  vec4 color;
};

layout (location = 0) out vec4 outColor;

out gl_PerVertex {
  vec4 gl_Position;
};

void main() {
  if (gl_VertexIndex == 0) {
    gl_Position = vec4(line.xy, 0.0f, 1.0f);
  } else {
    gl_Position = vec4(line.zw, 0.0f, 1.0f);
  }
  gl_Position.xy /= vec2(800.0f, 600.0f)*0.5f;
  outColor = color;
}
