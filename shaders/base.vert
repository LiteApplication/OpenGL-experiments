#version 330 core
layout (location = 0) in vec3 aPos;   // Vertex position
layout (location = 1) in int aColor; // Vertex color index

flat out int colorIndex; // Output a color index to the fragment shader

uniform mat4 model;
uniform mat4 inv_model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    colorIndex = aColor;
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}