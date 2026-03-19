#version 460



uniform sampler2D renderedScene;
uniform int renderedSceneWidth;
uniform int renderedSceneHeight;
uniform float kernel[9];
uniform bool applyKernel;

in vec2 fragUvTex;




out vec4 FragColor;
void main() {

    if (!applyKernel){
        FragColor = vec4(vec3(texture(renderedScene, fragUvTex)), 1.0f);
        return;
    }

    const float offsetU = 1.0f / float(renderedSceneWidth);
    const float offsetV = 1.0f / float(renderedSceneHeight);
    const vec2 kernelOffsets[9] = {
    vec2(-offsetU, offsetV), vec2(0.0f, offsetV), vec2(offsetU, offsetV),
    vec2(-offsetU, 0.0f), vec2(0.0f, 0.0f), vec2(offsetU, 0.0f),
    vec2(-offsetU, -offsetV), vec2(0.0f, -offsetV), vec2(offsetU, -offsetV)

    };


    vec3 finalFragColor = vec3(0.0f);
    for (int i = 0; i < 9; i++){
        vec2 uv = kernelOffsets[i] + fragUvTex;
        finalFragColor += vec3(texture(renderedScene, uv)) * kernel[i];
    }

    FragColor = vec4(finalFragColor, 1.0f);


}
