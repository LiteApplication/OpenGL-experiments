#pragma once

enum Voxel : unsigned char
{
    Air = 0,
    Dirt = 1,
    Grass = 2,
    Sand = 3,
    Water = 4,
};

#include <glm/glm.hpp>

#include "learnopengl/Shaders.hpp"
using namespace glm;

namespace ShaderData
{
    struct Material
    {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;
        bool flow;
    };

    const Material materials[] = {
        // Air
        {.ambient = vec3(0.0f, 0.0f, 0.0f),
         .diffuse = vec3(0.0f, 0.0f, 0.0f),
         .specular = vec3(0.0f, 0.0f, 0.0f),
         .shininess = 0.0f,
         .flow = false},
        // Dirt
        {.ambient = vec3(0.17f, 0.09f, 0.05f),
         .diffuse = vec3(0.17f, 0.09f, 0.05f),
         .specular = vec3(0.0f, 0.00f, 0.00f),
         .shininess = 1.0f,
         .flow = false},
        // Grass
        {.ambient = vec3(0.07f, 0.43f, 0.08f),
         .diffuse = vec3(0.07f, 0.43f, 0.08f),
         .specular = vec3(0.03f, 0.21f, 0.04f),
         .shininess = 1.0f,
         .flow = false},
        // Sand
        {.ambient = vec3(0.43f, 0.43f, 0.00f),
         .diffuse = vec3(0.60f, 0.60f, 0.00f),
         .specular = vec3(0.00f, 0.0f, 0.00f),
         .shininess = 1.0f,
         .flow = false},
        // Water
        {.ambient = vec3(0.0f, 0.0f, 0.43f),
         .diffuse = vec3(0.05f, 0.31f, 0.48f),
         .specular = vec3(0.9f, 0.9f, 1.0f),
         .shininess = 512.0f,
         .flow = true},
    };

    // Setup function to load the materials into the shader
    inline void setupMaterials(Shader *shader)
    {
        shader->use();
        for (int i = 0; i < sizeof(materials) / sizeof(Material); i++)
        {
            shader->setVec3("materials[" + std::to_string(i) + "].ambient", materials[i].ambient);
            shader->setVec3("materials[" + std::to_string(i) + "].diffuse", materials[i].diffuse);
            shader->setVec3("materials[" + std::to_string(i) + "].specular", materials[i].specular);
            shader->setFloat("materials[" + std::to_string(i) + "].shininess", materials[i].shininess);
            shader->setBool("materials[" + std::to_string(i) + "].flow", materials[i].flow);
        }
    }

} // namespace ShaderData