#version 460 core

// -- in
// inOrigin is simply instanced rectangle in normalized [-0.5, +0.5] space
layout (location = 0) in vec2 inOrigin;
layout (location = 1) in vec2 inLocalOffset;
layout (location = 2) in vec2 inUvOffset;
layout (location = 3) in vec2 inUvScale;
layout (location = 4) in vec4 inColorMultiply;
layout (location = 5) in vec2 inFlipXY;

// -- out
out gl_PerVertex {
  vec4 gl_Position;
};
layout (location = 0) out vec2 outOrigin;
layout (location = 1) out vec2 outUv;
layout (location = 2) out vec4 outColorMultiply;

// -- push constant
layout(push_constant) uniform PushConstants {
  vec4 viewport;
};

void main() {
  // -- out gl position
  gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
  gl_Position.xy = inOrigin * inUvScale + inLocalOffset;

  // to viewport space
  gl_Position.xy = gl_Position.xy / viewport.zw + viewport.xy;

  // then to clip space
  gl_Position.xy = gl_Position.xy * 2.0f - vec2(1.0f);

  // -- out uv
  vec2 uvOrigin = inOrigin + 0.5f;
  if (inFlipXY.x > 0.0f) {
    uvOrigin.x = 1.0f - uvOrigin.x;
  }
  if (inFlipXY.y > 0.0f) {
    uvOrigin.y = 1.0f - uvOrigin.y;
  }
  outUv = uvOrigin * vec2(32.0f) + inUvOffset;

  // -- out color
  outColorMultiply = inColorMultiply;

  // -- out origin
  outOrigin = inOrigin;
}
