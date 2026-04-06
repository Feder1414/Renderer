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




uniform sampler2D happyFace;

//Material
uniform sampler2D materialDiffuse;
uniform sampler2D materialSpecular;
uniform int materialShininess;
uniform int transparency;

//Light
//uniform Light lights[MAX_LIGHTS];
uniform int amountLights;


//Eye position
uniform vec3 eyePosition;

uniform float nearPlane;

uniform float farPlane;

uniform float fogDensity;
uniform bool fogEffect;
uniform vec3 fogColor;

const float kPi = 3.14159265;
const float kShininess = 16.0;

uniform bool usingBlin;

uniform sampler2D shadowMap;

vec3 GetSpecularComponent(vec3 normLightDirection, vec3 fragToEye, vec3 normWorldNormal, vec3 lightSpecular, float diff){
    float spec;

    if (diff == 0.0){
        return vec3(0.0f);
    }
    if (usingBlin){
        const float kEnergyConservation = (8.0 + materialShininess) / (8.0 * kPi);
        vec3 halfwayDir = normalize(normLightDirection + fragToEye);
        float spec = kEnergyConservation* pow(max(0, dot(halfwayDir, normWorldNormal)), materialShininess);

    }
    else {
        const float kEnergyConservation = (2.0 + materialShininess) / (2.0 * kPi);
        vec3 reflectVector = reflect(normLightDirection, normWorldNormal);
        float spec =  kEnergyConservation* pow(max(0, dot(fragToEye, reflectVector)), materialShininess);
    }
    return lightSpecular*spec* vec3(texture(materialSpecular, uvFrag));


}


float IsInShadow(vec3 normal, vec3 lightDir){

    vec3 projCoords = fragLightClip.xyz / fragLightClip.w;
    float depth = projCoords.z;
    projCoords = (projCoords*0.5) + 0.5;
    float currShadowMapFragDepth = texture(shadowMap, projCoords.xy).r;
    float bias = max(0.05*(1.0-dot(normal, lightDir)), 0.005f);
    return currShadowMapFragDepth < depth - bias ? 1.0f: 0.0f;


}

vec3 CalculatePointLightContribution(Light light, float attenuation, vec3 normWorldNormal, vec3 fragDiffuse, vec3 fragToEye) {

    vec3 lightAmbient = light.color * light.ambientFactor * attenuation;
    vec3 ambientComponent = lightAmbient*fragDiffuse;

    vec3 lightDiff = light.color * light.diffuseFactor *attenuation;
    vec3 lightSpecular = light.color * light.specularFactor * attenuation;

    vec3 normLightDirection = normalize(light.position - worldFragPos);
    float diff = max(dot(normWorldNormal, normLightDirection), 0);

    vec3 diffuseComponent = diff*lightDiff*fragDiffuse;




    vec3 specularComponent = GetSpecularComponent(-normLightDirection, fragToEye, normWorldNormal, lightSpecular, diff);

    return ambientComponent + specularComponent + diffuseComponent;


}

vec3 CalculateDirLightContribution(Light light, float attenuation, vec3 normWorldNormal, vec3 fragDiffuse, vec3 fragToEye) {
    vec3 normLightDirection = normalize(light.direction);
    vec3 lightAmbient = light.color * light.ambientFactor;
    vec3 ambientComponent = lightAmbient*fragDiffuse;
    if (IsInShadow(normWorldNormal, normLightDirection) == 1.0){
        return ambientComponent;
    }

    vec3 lightDiff = light.color * light.diffuseFactor;

    vec3 lightSpecular = light.color * light.specularFactor;



    float diff = max(dot(normWorldNormal, -normLightDirection), 0);

    vec3 diffuseComponent = diff*lightDiff*fragDiffuse;



    vec3 specularComponent = GetSpecularComponent(-normLightDirection, fragToEye, normWorldNormal, lightSpecular, diff);

    return ambientComponent + specularComponent + diffuseComponent;

}

vec3 CalculateSpotLightContribution(Light light, float attenuation, vec3 normWorldNormal, vec3 fragDiffuse, vec3 fragToEye){

    vec3 normFragToLight = normalize(light.position - worldFragPos);
    vec3 normLightDirection = normalize(light.direction);
    float theta = dot(normFragToLight, -normLightDirection);
    float epsilon = light.innerConeAngle - light.outerConeAngle;
    float spotIntensity = clamp((theta - light.outerConeAngle)/epsilon, 0, 1);

    vec3 lightDiff = light.color * light.diffuseFactor * spotIntensity * attenuation;
    vec3 lightAmbient = light.color * light.ambientFactor * spotIntensity * attenuation;
    vec3 lightSpecular = light.color * light.specularFactor * attenuation;


    float diff = max(dot(normWorldNormal, normFragToLight), 0);

    vec3 diffuseComponent = diff*lightDiff*fragDiffuse;
    vec3 ambientComponent = lightAmbient*fragDiffuse;

    vec3 halfwayDir = normalize(normLightDirection + fragToEye);
    float spec = pow(max(0, dot(halfwayDir, normWorldNormal)), materialShininess);
    vec3 specularComponent = GetSpecularComponent(-normLightDirection, fragToEye, normWorldNormal, lightSpecular, diff);

    return ambientComponent + specularComponent + diffuseComponent;

}

float LinearizeDepth(float depth){
    float z = depth * 2.0f - 1.0f;
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane -z * (farPlane-nearPlane));
}



void main() {
    //Enums for lightType
    vec4 firstTextureColor = texture(materialDiffuse, uvFrag);


    vec4 secondTextureColor = texture(happyFace, uvFrag);

    //Light Calculation
    vec4 fragDiffuse = mix(firstTextureColor, secondTextureColor, 0.2);


    if (transparency == OPAQUE){
        fragDiffuse.a = 1.0f;
    }


    vec4 finalFragColor = vec4(0.0f, 0.0f, 0.0f, fragDiffuse.a);

    //    if (transparency == TRANSPARENT && fragDiffuse.a < 0.1){
    //        discard;
    //    }



    vec3 normWorldNormal = normalize(worldNormal);





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


    if (fogEffect){
        float depthLinear = LinearizeDepth(gl_FragCoord.z) / farPlane;
        float depthFactor = exp(-pow(depthLinear * fogDensity, 2.0f));
        vec3 finalFragColorRgb = mix(fogColor, finalFragColor.rgb, depthFactor);
        finalFragColor = vec4(finalFragColorRgb, finalFragColor.a);
    }

    //Output
    FragColor =   finalFragColor;

}