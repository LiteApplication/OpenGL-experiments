#pragma once

#include "learnopengl/Shaders.hpp"

#include <glm/glm.hpp>

#define TWO_PI 6.28318530718f

class Sun
{
private:
    glm::vec3 position;
    glm::vec3 color;
    float ambient;
    float diffuse;
    float specular;

    glm::mat4 view;
    glm::mat4 projection;

    float currentTime;

    Shader *shader;

public:
    Sun(Shader *shader);
    ~Sun();

    void update(float delta_time, glm::vec3 player_position);
};