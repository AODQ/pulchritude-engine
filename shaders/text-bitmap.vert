#version 460 core

layout(push_constant) uniform PushConstants {
  mat4 transform;
  vec2 relativeOffset;
};

out layout(location = 0) vec2 outUv;

out gl_PerVertex {
  vec4 gl_Position;
};

const vec2 triangleVert[6] = {
  vec2(0.0f, 0.0f),
  vec2(1.0f, 0.0f),
  vec2(1.0f, 1.0f),

  vec2(0.0f, 0.0f),
  vec2(1.0f, 1.0f),
  vec2(0.0f, 1.0f),
};

void main() {
  vec4 origin = (
      vec4(triangleVert[gl_VertexIndex%6], 0.0f, 1.0f)
    + vec4(relativeOffset, 0.0f, 0.0f)
  );
  origin = transform * origin;
  outUv = triangleVert[gl_VertexIndex%6];
}
