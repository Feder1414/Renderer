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

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;
//uniform mat3 normalMatrix;

void main() {
    vec4 worldPos = worldTransform * vec4(position, 1.0);

    gl_Position = projectionTransform * viewTransform * worldPos;

    mat3 normalMatrix = transpose(inverse(mat3(modelTransform)));
    fragCol = vertexCol;
    worldFragPos = worldPos.xyz;
    worldNormal = normalize(normalMatrix * normal);
    uvFrag = uv0;
}
