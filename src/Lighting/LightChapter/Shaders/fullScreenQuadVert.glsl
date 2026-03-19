#version 460


layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uvTex;

out vec2 fragUvTex;

void main() {
    gl_Position = vec4(position, 1.0f);
    fragUvTex = uvTex;
}
