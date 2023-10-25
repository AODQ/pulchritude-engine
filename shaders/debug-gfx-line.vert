#version 460 core

layout(push_constant) uniform PushConstants {
  vec4 line;
};

out gl_PerVertex {
  vec4 gl_Position;
};

void main() {
  if (gl_VertexIndex == 0) {
    gl_Position = vec4(line.xy, 0.0f, 1.0f);
  } else {
    gl_Position = vec4(line.zw, 0.0f, 1.0f);
  }
}
