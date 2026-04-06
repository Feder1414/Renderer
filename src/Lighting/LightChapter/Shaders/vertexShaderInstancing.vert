#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 vertexCol;
layout (location = 2) in vec2 uv0;
layout (location = 3) in vec3 normal;
layout (location =4) in mat4 worldTransform;


out vec4 fragCol;
out vec2 uvFrag;
out vec3 worldNormal;
out vec3 worldFragPos;
out vec4 fragLightClip;

layout(std140, binding = 0) uniform CameraData{
    mat4 viewTransform;
    mat4 projectionTransform;
};

layout(std140, binding = 2) uniform LightData{
    mat4 lightViewTransform;
    mat4 lightProjectionTransform;
};
//uniform mat3 normalMatrix;

void main() {
    vec4 worldPos = worldTransform * vec4(position, 1.0);

    gl_Position = projectionTransform * viewTransform * worldPos;

    fragLightClip = lightProjectionTransform*lightViewTransform*worldPos;

    mat3 normalMatrix = transpose(inverse(mat3(worldTransform)));
    fragCol = vertexCol;
    worldFragPos = worldPos.xyz;
    worldNormal = normalize(normalMatrix * normal);
    uvFrag = uv0;
}
