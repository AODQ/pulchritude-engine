#version 460 core
in  layout(location = 0) vec2 inPosition;
in  layout(location = 1) vec2 inUv;
in  layout(location = 2) vec4 inColor;

layout(push_constant) uniform constants {
  vec2 unfScale;
  vec2 unfTranslate;
};

out layout(location = 0) vec2 outUv;
out layout(location = 1) vec4 outColor;

out gl_PerVertex {
  vec4 gl_Position;
};

void main() {
  outUv = inUv;
  outColor = inColor;
  gl_Position = vec4(inPosition * unfScale + unfTranslate, 0.0f, 1.0f);
}
