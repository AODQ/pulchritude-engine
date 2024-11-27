#version 460 core

// -- in
in layout(location = 0) vec2 inOrigin;
in layout(location = 1) vec2 inUv;
in layout(location = 2) vec4 inColorMultiply;

// -- out
out layout(location = 0) vec4 outColor;

// -- samplers
uniform layout(set = 0, binding = 0) sampler2D boundSampler;

// -- main
void main() {
  // -- out color
  ivec2 uv = ivec2(inUv.x, inUv.y);
  outColor = texelFetch(boundSampler, uv, 0);
  outColor *= inColorMultiply;
  // outColor.rgb = outColor.rgb * outColor.a;
}
