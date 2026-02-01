#version 460 core

layout (location = 0) in vec3 vertexPos;

uniform mat4 projectionTransform;
uniform mat4 viewTransform;
uniform mat4 modelTransform;



void main() {
    gl_Position = projectionTransform * viewTransform * modelTransform * vec4(vertexPos, 1);

}