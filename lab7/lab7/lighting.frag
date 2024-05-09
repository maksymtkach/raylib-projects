#version 430

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec3 fragNormal;

// Output fragment color
out vec4 finalColor;

struct Light {
    int enabled;
    vec3 position;
    vec3 target;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float transparency;
};

// Input lighting values
uniform Light light;
uniform Material material;
uniform vec3 viewPos;

void main()
{
    // ambient
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = vec3(0);
    vec3 specular = vec3(0);
    if (light.enabled == 1) {
        // diffuse
        vec3 norm = normalize(fragNormal);
        vec3 lightDir = normalize(light.position - fragPosition);
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse = light.diffuse * (diff * material.diffuse);

        // specular
        vec3 viewDir = normalize(viewPos - fragPosition);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        specular = light.specular * (spec * material.specular);
    }

    vec3 result = ambient + diffuse + specular;
    finalColor = vec4(result, material.transparency);

    float dist = length(viewPos - fragPosition);

    // these could be parameters...
    const vec4 fogColor = vec4(0.5, 0.5, 0.5, 1.0);
    const float fogDensity = 0.01;

    // Exponential fog
    float fogFactor = 1.0/exp((dist*fogDensity)*(dist*fogDensity));

    // Linear fog (less nice)
    //const float fogStart = 2.0;
    //const float fogEnd = 10.0;
    //float fogFactor = (fogEnd - dist)/(fogEnd - fogStart);

    fogFactor = clamp(fogFactor, 0.0, 1.0);
    finalColor = mix(fogColor, finalColor, fogFactor);
}