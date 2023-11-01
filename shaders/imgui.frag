#version 460 core
in layout (location = 0) vec2 inUv;
in layout (location = 1) vec4 inColor;

uniform layout(set = 0, binding = 0) sampler2D boundSampler;

out layout (location = 0) vec4 outColor;

void main() {
  // TODO this is a hack until i figure out the alpha compositing
  vec4 t = texture(boundSampler, inUv);
  t.r = t.a;
  if (t.a < 0.2f) {
    discard;
  }
  outColor = inColor * t;
  outColor.a = t.a;
}
