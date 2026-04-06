#version 460 core

#define MAX_LIGHTS 1024

//Enum LightType
#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1
#define SPOT_LIGHT 2


//Enum transparency
#define OPAQUE 0
#define SEMITRANSPARENT 1
#define TRANSPARENT 2



in vec4 fragCol;
in vec2 uvFrag;
in vec3 worldNormal;
in vec3 worldFragPos;
in vec4 fragLightClip;

out vec4 FragColor;

struct Light {
    vec3 color;
    vec3 direction;
    vec3 position;

    float diffuseFactor;
    float ambientFactor;
    float specularFactor;

    float innerConeAngle;
    float outerConeAngle;

    float attConstant;
    float attLinear;
    float attQuadratic;

    int type;
};

layout(std430, binding = 1) buffer LightsData{
    Light lights[1024];
};


//Material
uniform sampler2D materialDiffuse;
uniform sampler2D materialSpecular;
uniform int materialShininess;
uniform int transparency;
uniform bool hasSpecularTexture;
uniform  vec3 matSpecSolid;

//Light
//uniform Light lights[MAX_LIGHTS];
uniform int amountLights;

uniform bool usingBlin;


//Eye position
uniform vec3 eyePosition;

const float kPi = 3.14159265;
const float kShininess = 16.0;


//Shadows

uniform sampler2D shadowMap;



float IsInShadow(vec3 normal, vec3 lightDir){

    vec3 projCoords = fragLightClip.xyz / fragLightClip.w;
    projCoords = (projCoords*0.5) + 0.5;

    float depth = projCoords.z;
    if (depth > 1.0f){
        return 0.0f;
    }
    float currShadowMapFragDepth = texture(shadowMap, projCoords.xy).r;
    float bias = max(0.05*(1.0-dot(normal, -lightDir)), 0.005f);
    vec2 texSize = 1.0f / textureSize(shadowMap, 0);
    float pcfAvg = 0.0f;
    for (int x = -1; x <= 1; x++){
        for (int y = -1; y <= 1; y++){

            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texSize).r;
            pcfAvg += pcfDepth < depth - bias ? 1.0f: 0.0;
        }
    }
    pcfAvg/= 9;
    return pcfAvg;


}


vec3 GetSpecularComponent(vec3 normLightDirection, vec3 fragToEye, vec3 normWorldNormal, vec3 lightSpecular, float diff){
    float spec;

    if (diff <= 0.0){
        return vec3(0.0f);
    }
    if (usingBlin){
        const float kEnergyConservation = (8.0 + materialShininess) / (8.0 * kPi);
        vec3 halfwayDir = normalize(normLightDirection + fragToEye);
        spec = kEnergyConservation* pow(max(0, dot(halfwayDir, normWorldNormal)), materialShininess);

    }
    else {
        const float kEnergyConservation = (2.0 + materialShininess) / (2.0 * kPi);
        vec3 reflectVector = reflect(normLightDirection, normWorldNormal);
        spec =  kEnergyConservation* pow(max(0, dot(fragToEye, reflectVector)), materialShininess);
    }
    vec3 specularColor = hasSpecularTexture ? vec3(texture(materialSpecular, uvFrag)) : matSpecSolid;
    return lightSpecular*spec*  specularColor;
    //return lightSpecular*spec* vec3(0.3);


}

vec3 CalculatePointLightContribution(Light light, float attenuation, vec3 normWorldNormal, vec3 fragDiffuse, vec3 fragToEye) {
    vec3 lightDiff = light.color * light.diffuseFactor *attenuation;
    vec3 lightAmbient = light.color * light.ambientFactor * attenuation;
    vec3 lightSpecular = light.color * light.specularFactor * attenuation;

    vec3 normLightDirection = normalize(light.position - worldFragPos);
    float diff = max(dot(normWorldNormal, normLightDirection), 0);

    vec3 diffuseComponent = diff*lightDiff*fragDiffuse;
    vec3 ambientComponent = lightAmbient*fragDiffuse;



    vec3 specularComponent = GetSpecularComponent(normLightDirection, fragToEye, normWorldNormal, lightSpecular, diff);

    return ambientComponent + specularComponent + diffuseComponent;


}

vec3 CalculateDirLightContribution(Light light, float attenuation, vec3 normWorldNormal, vec3 fragDiffuse, vec3 fragToEye) {
    vec3 normLightDirection = normalize(light.direction);
    vec3 lightAmbient = light.color * light.ambientFactor;
    vec3 ambientComponent = lightAmbient*fragDiffuse;

    float isInShadow = IsInShadow(normWorldNormal, normLightDirection);
    //    if (isInShadow == 1.0){
    //        return ambientComponent;
    //    }


    vec3 lightDiff = light.color * light.diffuseFactor;

    vec3 lightSpecular = light.color * light.specularFactor;



    float diff = max(dot(normWorldNormal, -normLightDirection), 0);

    vec3 diffuseComponent = diff*lightDiff*fragDiffuse;



    vec3 specularComponent = GetSpecularComponent(-normLightDirection, fragToEye, normWorldNormal, lightSpecular, diff);

    return ambientComponent + (1-isInShadow)*(specularComponent + diffuseComponent);

}

vec3 CalculateSpotLightContribution(Light light, float attenuation, vec3 normWorldNormal, vec3 fragDiffuse, vec3 fragToEye){

    vec3 normFragToLight = normalize(light.position - worldFragPos);
    vec3 normLightDirection = normalize(light.direction);
    float theta = dot(normFragToLight, -normLightDirection);
    float epsilon = light.innerConeAngle - light.outerConeAngle;
    float spotIntensity = clamp((theta - light.outerConeAngle)/epsilon, 0, 1);

    vec3 lightDiff = light.color * light.diffuseFactor * spotIntensity * attenuation;
    vec3 lightAmbient = light.color * light.ambientFactor * spotIntensity * attenuation;
    vec3 lightSpecular = light.color * light.specularFactor * attenuation * spotIntensity;


    float diff = max(dot(normWorldNormal, normFragToLight), 0);

    vec3 diffuseComponent = diff*lightDiff*fragDiffuse;
    vec3 ambientComponent = lightAmbient*fragDiffuse;

    vec3 specularComponent = GetSpecularComponent(normFragToLight, fragToEye, normWorldNormal, lightSpecular, diff);

    return ambientComponent + specularComponent + diffuseComponent;

}

//Param normLightDirection must point from frag to light.





void main() {
    //Enums for lightType
    vec4 fragDiffuse = texture(materialDiffuse, uvFrag);



    vec3 normWorldNormal = normalize(worldNormal);


    if (transparency == OPAQUE){
        fragDiffuse.a = 1.0f;
    }
    vec4 finalFragColor = vec4(0.0f, 0.0f, 0.0f, fragDiffuse.a);

    vec3 fragToEye = normalize(eyePosition - worldFragPos);

    int maxDirLights = 1;
    int amountDirLights = 0;

    for (int i = 0; i < amountLights; i++){
        float distance = length(lights[i].position - worldFragPos);
        float attenuation = 1.0/(lights[i].attConstant + lights[i].attLinear*distance + lights[i].attQuadratic * distance*distance);
        if (lights[i].type == POINT_LIGHT){
            finalFragColor += vec4(CalculatePointLightContribution(lights[i], attenuation, normWorldNormal, vec3(fragDiffuse), fragToEye), 0.0f);
        }
        else if (lights[i].type == DIRECTIONAL_LIGHT && amountDirLights < maxDirLights){
            finalFragColor += vec4(CalculateDirLightContribution(lights[i], attenuation, normWorldNormal, vec3(fragDiffuse), fragToEye), 0.0f);
            amountDirLights += 1;
        }
        else if (lights[i].type == SPOT_LIGHT){
            finalFragColor+= vec4(CalculateSpotLightContribution(lights[i], attenuation, normWorldNormal, vec3(fragDiffuse), fragToEye), 0.0f);

        }

    }

    //Output
    FragColor = finalFragColor;

}