#version 460 core
in layout(location = 0) vec3 inPosition;
in layout(location = 1) vec2 inUv;
in layout(location = 2) vec3 inNormal;

out layout(location = 0) vec2 outUv;
out layout(location = 1) vec3 outNormal;

out gl_PerVertex {
  vec4 gl_Position;
};

void main() {
  outUv = inUv;
  outNormal = inNormal;
  gl_Position = vec4(inPosition, 1.0f);
}
