#version 460 core
in vec4 fragCol;
in vec2 uvFrag;
out vec4 FragColor;

uniform float opacity;

uniform sampler2D texture0;
uniform sampler2D texture1;


void main() {
    vec4 fragVertexColor = vec4(fragCol.xyz, opacity);
    vec4 firstTextureColor = texture(texture0, uvFrag);
    vec4 secondTextureColor = texture(texture1, uvFrag);

    FragColor = mix(firstTextureColor, secondTextureColor, 0.2) * fragVertexColor;

}