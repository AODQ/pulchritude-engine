#version 460 core

layout(push_constant) uniform PushConstants {
  layout (offset=80) vec4 textureOffset;
};

in layout(location = 0) vec2 inUv;

uniform layout(set = 0, binding = 0) sampler2D fontTexture;

out layout(location = 0) vec4 outColor;

void main() {
  float alpha = (
    texture(fontTexture, textureOffset.xy + textureOffset.zw*inUv).r
  );
  if (alpha < 0.9f) {
    discard;
  }
  outColor = vec4(alpha);
}
