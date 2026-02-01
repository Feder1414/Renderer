#version 460 core
in vec4 fragCol;
out vec4 FragColor;

uniform float opacity;

void main() {
    FragColor = vec4(fragCol.xyz, opacity);

}