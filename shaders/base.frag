#version 330 core
in vec3 ourColor; // Receive the color from the vertex shader

out vec4 FragColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}