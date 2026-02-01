#version 460 core



layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec4 vertexCol;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 normal;
out vec4 fragCol;
out vec2 uvFrag;
out vec3 worldNormal;
out vec3 worldFragPos;




uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;
uniform mat3 normalMatrix;






void main() {
    gl_Position = projectionTransform * viewTransform * modelTransform * vec4(vertexPos, 1);
    fragCol = vertexCol;
    worldFragPos = vec3(modelTransform * vec4(vertexPos, 1.0f));
    worldNormal = normalMatrix * normal;
    uvFrag = uv;


}