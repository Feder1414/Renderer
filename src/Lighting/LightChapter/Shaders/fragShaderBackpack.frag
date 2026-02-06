#version 460 core

#define MAX_LIGHTS 10

//Enum LightType
#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1
#define SPOT_LIGHT 2

in vec4 fragCol;
in vec2 uvFrag;
in vec3 worldNormal;
in vec3 worldFragPos;

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





//Material
uniform sampler2D materialDiffuse;
uniform sampler2D materialSpecular;
uniform int materialShininess;

//Light
uniform Light lights[MAX_LIGHTS];
uniform int amountLights;


//Eye position
uniform vec3 eyePosition;


vec3 CalculatePointLightContribution(Light light, float attenuation, vec3 normWorldNormal, vec3 fragDiffuse, vec3 fragToEye) {
    vec3 lightDiff = light.color * light.diffuseFactor *attenuation;
    vec3 lightAmbient = light.color * light.ambientFactor * attenuation;
    vec3 lightSpecular = light.color * light.specularFactor * attenuation;

    vec3 normLightDirection = normalize(light.position - worldFragPos);
    float diff = max(dot(normWorldNormal, normLightDirection), 0);

    vec3 diffuseComponent = diff*lightDiff*fragDiffuse;
    vec3 ambientComponent = lightAmbient*fragDiffuse;


    vec3 reflectDirLight = reflect(-normLightDirection, normWorldNormal);
    float spec = pow(max(0, dot(reflectDirLight, fragToEye)), materialShininess);

    vec3 specularComponent = lightSpecular*spec*vec3(texture(materialSpecular, uvFrag));

    return ambientComponent + specularComponent + diffuseComponent;


}

vec3 CalculateDirLightContribution(Light light, float attenuation, vec3 normWorldNormal, vec3 fragDiffuse, vec3 fragToEye) {
    vec3 lightDiff = light.color * light.diffuseFactor;
    vec3 lightAmbient = light.color * light.ambientFactor;
    vec3 lightSpecular = light.color * light.specularFactor;

    vec3 normLightDirection = normalize(light.direction);

    float diff = max(dot(normWorldNormal, normLightDirection), 0);

    vec3 diffuseComponent = diff*lightDiff*fragDiffuse;
    vec3 ambientComponent = lightAmbient*fragDiffuse;


    vec3 reflectDirLight = reflect(-normLightDirection, normWorldNormal);
    float spec = pow(max(0, dot(reflectDirLight, fragToEye)), materialShininess);
    vec3 specularComponent = lightSpecular*spec*vec3(texture(materialSpecular, uvFrag));

    return lightSpecular + specularComponent + diffuseComponent;

}

vec3 CalculateSpotLightContribution(Light light, float attenuation, vec3 normWorldNormal, vec3 fragDiffuse, vec3 fragToEye){

    vec3 normFragToLight = normalize(light.position - worldFragPos);
    vec3 normLightDirection = normalize(light.direction);
    float theta = dot(normFragToLight, -normLightDirection);
    float epsilon = light.innerConeAngle - light.outerConeAngle;
    float spotIntensity = clamp((theta - light.outerConeAngle)/epsilon, 0, 1);

    vec3 lightDiff = light.color * light.diffuseFactor * spotIntensity;
    vec3 lightAmbient = light.color * light.ambientFactor * spotIntensity;
    vec3 lightSpecular = light.color * light.specularFactor;


    float diff = max(dot(normWorldNormal, -normLightDirection), 0);

    vec3 diffuseComponent = diff*lightDiff*fragDiffuse;
    vec3 ambientComponent = lightAmbient*fragDiffuse;

    vec3 reflectDirLight = reflect(-normLightDirection, normWorldNormal);
    float spec = pow(max(0, dot(reflectDirLight, fragToEye)), materialShininess);
    vec3 specularComponent = lightSpecular*spec* vec3(texture(materialSpecular, uvFrag));

    return lightSpecular + specularComponent + diffuseComponent;

}




void main() {
    //Enums for lightType
    vec3 fragDiffuse = vec3(texture(materialDiffuse, uvFrag));

    vec3 normWorldNormal = normalize(worldNormal);


    vec3 finalFragColor = vec3(0.0);

    vec3 fragToEye = normalize(eyePosition - worldFragPos);

    int maxDirLights = 1;
    int amountDirLights = 0;

    for (int i = 0; i < amountLights; i++){
        float distance = length(lights[i].position - worldFragPos);
        float attenuation = 1.0/(lights[i].attConstant + lights[i].attLinear*distance + lights[i].attQuadratic * distance*distance);
        if (lights[i].type == POINT_LIGHT){
            finalFragColor += CalculatePointLightContribution(lights[i], attenuation, normWorldNormal, fragDiffuse, fragToEye);
        }
        else if (lights[i].type == DIRECTIONAL_LIGHT && amountDirLights < maxDirLights){
            finalFragColor += CalculateDirLightContribution(lights[i], attenuation, normWorldNormal, fragDiffuse, fragToEye);
            amountDirLights += 1;
        }
        else if (lights[i].type == SPOT_LIGHT){
            finalFragColor+= CalculateSpotLightContribution(lights[i], attenuation, normWorldNormal, fragDiffuse, fragToEye);

        }

    }

    //Output
    FragColor = vec4(finalFragColor,1.0);

}