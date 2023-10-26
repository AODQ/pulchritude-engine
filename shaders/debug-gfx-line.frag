#version 460 core

in layout(location = 0) vec4 inColor;

out layout(location = 0) vec4 outColor;

void main() {
  outColor = inColor;
}
