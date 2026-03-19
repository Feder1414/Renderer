#version 460

//Enum transparency
#define OPAQUE 0
#define SEMITRANSPARENT 1
#define TRANSPARENT 2


uniform sampler2D materialDiffuse;
uniform int transparency;


in vec4 fragCol;
in vec2 uvFrag;
in vec3 worldNormal;
in vec3 worldFragPos;

out vec4 FragColor;

void main() {
    vec4 fragDiffuse = texture(materialDiffuse, uvFrag);
    if (transparency == TRANSPARENT && fragDiffuse.a < 0.1){
        discard;
    }
    FragColor = fragDiffuse;
}
