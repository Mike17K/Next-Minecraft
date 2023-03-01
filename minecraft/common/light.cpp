#include <glfw3.h>
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include "light.h"

using namespace glm;

Light::Light(GLFWwindow* window,
    glm::vec4 init_La,
    glm::vec4 init_Ld,
    glm::vec4 init_Ls,
    glm::vec3 init_position,
    glm::vec3 init_dir,
    float init_power) : window(window) {
    La = init_La;
    Ld = init_Ld;
    Ls = init_Ls;
    direction = normalize(init_dir);
    power = init_power;
    lightPosition_worldspace = init_position;


    viewMatrix = glm::lookAt(lightPosition_worldspace, lightPosition_worldspace + direction, glm::vec3(0.0f, 0.0f, -1.0f));

    // Define the projection matrix for the light
    float nearPlane = 0.01f;
    float farPlane = 1000.0f;
    float width = 100.0f;
    float height = 100.0f;

    projectionMatrix = glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f, nearPlane, farPlane);

    
}



void Light::update() {
}


mat4 Light::lightVP() {

    viewMatrix = glm::lookAt(lightPosition_worldspace, lightPosition_worldspace + direction, glm::vec3(0.0f, 0.0f, -1.0f));
    return projectionMatrix* viewMatrix;
}