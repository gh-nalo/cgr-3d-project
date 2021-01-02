#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

struct Light {
    vec3 position;  
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

in vec3 vertexPosition;  
in vec3 normalPosition;  
in vec2 textureCoordinates;
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform float iTime;
uniform bool cheatMode;

void main()
{
    // Ambient light
    vec3 ambient = light.ambient * texture(material.diffuse, textureCoordinates).rgb;
    
    // Diffuse light
    vec3 norm = normalize(normalPosition);
    vec3 lightDir = normalize(light.position - vertexPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, textureCoordinates).rgb;  

    // Specular
    vec3 viewDir = normalize(viewPos - vertexPosition);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, textureCoordinates).rgb;  
    
    // Spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= 2.5 * intensity;
    specular *= 2.5 * intensity;
    ambient *= intensity;
    
    // Attenuation
    float distance    = length(light.position - vertexPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;   
        
    vec3 result = ambient + diffuse + specular;

    if(cheatMode) {
        FragColor = vec4(texture(material.diffuse, textureCoordinates).rgb, 1.0); // For map building
    } else {
        FragColor = vec4(result, 1.0); // Flashlight
	}
    
    // FragColor = 0.5 + 0.5 * cos(iTime + vec4(1.0, 1.0, 0.0, 1.0)); // Time testing
    
} 