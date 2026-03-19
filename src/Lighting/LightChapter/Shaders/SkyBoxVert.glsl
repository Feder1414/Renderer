#version 460

layout (location = 0) in vec3 position;

out vec3 texCoord;

uniform mat4 viewTransform;
uniform mat4 projectionTransform;

void main() {
    vec4 projPos = projectionTransform*viewTransform * vec4(position, 1.0f);
    gl_Position = projPos.xyww;
    texCoord = position;

}
