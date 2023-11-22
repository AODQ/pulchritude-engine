#version 460 core
in layout (location = 0) vec2 inUv;
in layout (location = 1) vec3 inNormal;

//uniform layout(set = 0, binding = 0) sampler2D boundSampler;

out layout (location = 0) vec4 outColor;

void main() {
  outColor = vec4(inUv, 0.5f, 1.0f) * inColor;
}
