#version 330 core

#define NUM_MATERIALS 255 // Can be reduced to 5 for now to save memory
// Do not forget to increase if the type of the enum Voxel is changed.


#define FLOW_SPEED 2.0
#define FLOW_AMPLITUDE 0.01
#define FLOW_SPREAD 3.0
#define FLOW_LAYERS 5

flat in int material; // Receive the color from the vertex shader
flat in vec3 normalRaw;
in vec3 FragPos;

out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    bool flow;
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
uniform float time;

void main()
{   

    Material currentMaterial = materials[material];
    // If the material is a flow, modify the normal to make it flow in time
    vec3 normal = normalRaw;
    if (currentMaterial.flow) {
        float flowTime = time * FLOW_SPEED;
        float flowAmplitude = FLOW_AMPLITUDE;
        float flowSpread = FLOW_SPREAD;
        float flowLayers = FLOW_LAYERS;
        float flow = 0.0;
        float side = -1;
        float side2 = 1;
        for (int i = 0; i < FLOW_LAYERS; i++) {
            flow += sin(flowTime + FragPos.x * flowSpread * side2 + FragPos.y * flowSpread + FragPos.z * flowSpread * side) * flowAmplitude;
            flowTime *= 1.2;
            flowAmplitude *= 0.65;
            flowSpread *= 0.2;
            side *= -1.1;
            side2 *= -1.2;
        }
        normal += vec3(flow,0.0, flow);
        normal = normalize(normal);
    }


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
    //FragColor = vec4(normal.xyz, 1.0);

}