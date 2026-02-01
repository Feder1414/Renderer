#version 460 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec4 vertexCol;
layout (location = 2) in vec2 uv;
out vec4 fragCol;
out vec2 uvFrag;


uniform float opacity;

void main() {
    gl_Position = vec4(vertexPos, 1);
    fragCol = vertexCol;
    uvFrag = uv;


}