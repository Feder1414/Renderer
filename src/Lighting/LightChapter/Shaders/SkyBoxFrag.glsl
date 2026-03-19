#version 460

in vec3 texCoord;


out vec4 FragColor;

uniform samplerCube skybox;
void main() {
    FragColor = vec4(texture(skybox, texCoord).rgb, 1.0f);


}
