#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 vertexCol;
layout (location = 2) in vec2 uv0;
layout (location = 3) in vec3 normal;

out vec4 fragCol;
out vec2 uvFrag;
out vec3 worldNormal;
out vec3 worldFragPos;
layout(std140, binding = 0) uniform CameraData{
    mat4 viewTransform;
    mat4 projectionTransform;
};

uniform mat4 modelTransform;


uniform mat3 normalMatrix;

void main() {
    vec4 worldPos = modelTransform * vec4(position, 1.0);

    gl_Position = projectionTransform * viewTransform * worldPos;


    fragCol = vertexCol;
    worldFragPos = worldPos.xyz;
    worldNormal = normalize(normalMatrix * normal);
    uvFrag = uv0;
}
