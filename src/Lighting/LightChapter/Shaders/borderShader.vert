#version 460 core

layout (location = 0) in vec3 position;



uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;
uniform mat3 normalMatrix;

void main() {
    vec4 worldPos = modelTransform * vec4(position, 1.0);

    gl_Position = projectionTransform * viewTransform * worldPos;

}
