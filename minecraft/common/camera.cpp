#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"
#include <iostream>

using namespace std;
using namespace glm;

float maxTiltAngle = 10.0f;
float currnettiltAngle = 0.0f;
float tiltSpeed = 40.0f;

float FoV_max = 45.8f;
float FoV_min = 44.2f;

Camera::Camera(GLFWwindow* window) : window(window)
{
    position = glm::vec3(0, 150, 0);
    horizontalAngle = 3.14f;
    verticalAngle = 0.0f;
    FoV = 70.0f;
    speed = 10.0f;
    mouseSpeed = 0.001f;
    fovSpeed = 1.0001f;
}

void Camera::update()
{
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Get mouse position
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Reset mouse position for next frame
    glfwSetCursorPos(window, width / 2, height / 2);

    // Task 5.3: Compute new horizontal and vertical angles, given windows size
    //*/
    // and cursor position
    horizontalAngle += mouseSpeed * (width / 2 - xPos);

    verticalAngle = glm::max( -3.141592 / 2,glm::min(verticalAngle + mouseSpeed * (height / 2 - yPos),3.141592 / 2));
    

    // Task 5.4: right and up vectors of the camera coordinate system
    // use spherical coordinates
    vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    // Right, up, front vector
    vec3 right(-cos(horizontalAngle), 0, sin(horizontalAngle));
    vec3 up(0.0f, 1.0f, 0.0);
    vec3 front = cross(up, right);

    // add :
    // open menu with E, 
    // with escape stop the game and pop settings
    vec3 tiltPosition(0.0f, 0.0f, 0.0f);
    mat4 tiltRotation = mat4(1.0f);
    // tilt left
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) { //////////////////////////////////////
        currnettiltAngle = glm::max(currnettiltAngle - tiltSpeed * deltaTime, -maxTiltAngle);
    }

    // tilt right
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {/////////////////////////////////////
        currnettiltAngle = glm::min(currnettiltAngle + tiltSpeed * deltaTime, maxTiltAngle);
    }

    if (glfwGetKey(window, GLFW_KEY_R) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_T) != GLFW_PRESS) {
        if (abs(currnettiltAngle) < 0.2) {
            currnettiltAngle = 0.0f;
        }
        else {
            currnettiltAngle -= sign(currnettiltAngle) * tiltSpeed * deltaTime;
        }
    }

    float m = currnettiltAngle / maxTiltAngle * 0.5;
    tiltPosition = vec3(m * right.x, m * right.y, m * right.z);
    tiltRotation = glm::rotate(glm::mat4(1.0f), glm::radians(currnettiltAngle), glm::vec3(0.0f, 0.0f, 1.0f));

    // Task 5.6: handle zoom in/out effects
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        FoV /= fovSpeed;
    }
    else
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            FoV *= fovSpeed;
        }
        else {
            FoV = 45.0f;
        }

    if (FoV > FoV_max) FoV = FoV_max;
    if (FoV < FoV_min) FoV = FoV_min;

    // change mode LINE of FILL
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }



    // Task 5.7: construct projection and view matrices
    vec4 tmp_rot = vec4(up, 1.0f) * tiltRotation;

    projectionMatrix = perspective(FoV, 4.0f / 3.0f, 0.1f, 10000.0f);
    viewMatrix = lookAt(
        position + tiltPosition, // Camera position, in World Space
        position + tiltPosition + direction, // and looks at the origin
        vec3(tmp_rot.x, tmp_rot.y, tmp_rot.z) // Head is up (set to 0, -1, 0 to look upside-down)
    );

    

    
    //*/

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}