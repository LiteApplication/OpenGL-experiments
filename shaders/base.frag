#version 330 core
flat in int colorIndex; // Receive the color from the vertex shader

out vec4 FragColor;

// Color list
vec3 colors[] = vec3[](
    vec3(1.0, 0.0,1.0), // ERROR
    vec3(0.5, 0.2, 0.0), // Brown
    vec3(0.0, 0.5, 0.0), // Green
    vec3(0.0, 0.0, 0.5), // Blue
    vec3(1.0, 0.0, 1.0) // ERROR 
);

void main()
{   
    int index = clamp(colorIndex, 0, colors.length() - 1); // Make sure the color index is between 0 and 3  
    FragColor = vec4(colors[index], gl_FragCoord.z);
}