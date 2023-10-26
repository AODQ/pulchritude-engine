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
  vec2(-0.5f, -0.5f),
  vec2(+0.5f, -0.5f),
  vec2(+0.5f, +0.5f),

  vec2(-0.5f, -0.5f),
  vec2(+0.5f, +0.5f),
  vec2(-0.5f, +0.5f),
};

void main() {
  vec4 origin = vec4(triangleVert[gl_VertexIndex%6], 0.0f, 1.0f);
  origin.x *= 12.0f;
  origin.y *= 24.0f;
  origin.x += relativeOffset.x;
  origin.y += relativeOffset.y;
  origin.x /= 800.0f*0.5f;
  origin.y /= 600.0f*0.5f;
  //origin.xy = origin.xy*2.0f-vec2(1.0f);
  //gl_Position = transform * origin;
  gl_Position = origin;
  outUv = triangleVert[gl_VertexIndex%6]+vec2(0.5f);
}
