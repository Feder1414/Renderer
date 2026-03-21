#version 460 core

layout (location = 0) in vec3 vertexPos;


layout(std140, binding = 0) uniform CameraData{
    mat4 viewTransform;
    mat4 projectionTransform;
};
uniform mat4 modelTransform;



void main() {
    gl_Position = projectionTransform * viewTransform * modelTransform * vec4(vertexPos, 1);

}