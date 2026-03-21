#version 460 core

layout (location = 0) in vec3 position;



uniform mat4 modelTransform;


layout(std140, binding = 0) uniform CameraData{
    mat4 viewTransform;
    mat4 projectionTransform;
};

uniform mat3 normalMatrix;

void main() {
    vec4 worldPos = modelTransform * vec4(position, 1.0);

    gl_Position = projectionTransform * viewTransform * worldPos;

}
