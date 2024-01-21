#version 330 core

#define NUM_MATERIALS 255 // Can be reduced to 5 for now to save memory
// Do not forget to increase if the type of the enum Voxel is changed.

flat in int material; // Receive the color from the vertex shader
flat in vec3 normal;
in vec3 FragPos;

out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


uniform Material materials[NUM_MATERIALS];
uniform vec3 viewPos;
uniform Light light;

void main()
{   

    Material currentMaterial = materials[material];

    // ambient
    vec3 ambient = light.ambient * currentMaterial.ambient;
  	
    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * currentMaterial.diffuse);
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), currentMaterial.shininess);
    vec3 specular = light.specular * (spec * currentMaterial.specular);  
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(normal, 1.0);

}