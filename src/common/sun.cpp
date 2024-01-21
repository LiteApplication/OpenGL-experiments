#include "testgl/sun.hpp"
#include "testgl/constants.hpp"
#include <glm/gtc/matrix_transform.hpp>

#define SUN_HEIGHT 256.0f

Sun::Sun(Shader *shader)
{
    this->shader = shader;

    position = glm::vec3(0.0f, 0.0f, 0.0f);
    color = glm::vec3(1.0f, 1.0f, 1.0f);
    ambient = 0.5f;
    diffuse = 0.6f;
    specular = 0.8f;

    view = glm::mat4(1.0f);
    projection = glm::mat4(1.0f);

    currentTime = 0.0f;

    shader->use();
    //  vec3 position;
    // vec3 ambient;
    // vec3 diffuse;
    // vec3 specular;
    shader->setVec3("light.position", position);
    shader->setVec3("light.ambient", color * ambient);
    shader->setVec3("light.diffuse", color * diffuse);
    shader->setVec3("light.specular", color * specular);
}

Sun::~Sun()
{
}

void Sun::update(float delta_time, glm::vec3 player_position)
{
    currentTime += delta_time;
    // Update the position of the sun
    position = player_position + glm::vec3(cos(currentTime / DAY_LENGTH / 10.0f * TWO_PI) * 25.0f, cos(currentTime / DAY_LENGTH * TWO_PI) * SUN_HEIGHT, sin(currentTime / DAY_LENGTH * TWO_PI) * SUN_HEIGHT);
    // Update the shader
    shader->use();
    shader->setVec3("light.position", position);
}
