#version 460

layout (location = 0) in vec3 position;


layout(location = 5) in mat4 instance;


layout(std140, binding = 0) uniform CameraData{
    mat4 viewTransform;
    mat4 projectionTransform;
};

layout(std140, binding = 2) uniform LightData{
    mat4 lightView;
    mat4 lightProj;
};

uniform mat4 modelTransform;
void main() {
    gl_Position = lightProj* lightView* modelTransform * vec4(position, 1.0f);

}
