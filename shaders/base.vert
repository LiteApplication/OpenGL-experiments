#version 330 core
layout (location = 0) in vec3 aPos;   // Vertex position
layout (location = 1) in int aMaterial; // Vertex color index
layout (location = 2) in vec3 aNormal; // Vertex normal

flat out int material; // Output a color index to the fragment shader
flat out vec3 normalRaw; // Output a normal to the fragment shader
out vec3 FragPos; // Output a position to the fragment shader

uniform mat4 model;
uniform mat4 inv_model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    material = aMaterial;
    FragPos = vec3(model * vec4(aPos, 1.0));
    normalRaw = aNormal;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}