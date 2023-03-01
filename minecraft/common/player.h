#ifndef PLAYER_HPP
#define PLAYER_HPP


#include <GL/glew.h>
#include <glm/glm.hpp>

#include <glfw3.h>
#include "./chunk.h"
#include "./camera.h"

#include <iostream>
#include <cstdlib>

#include <vector>


using namespace std;
using namespace glm;


class Player
{
public:
	vec3 position, velocity,acceleration,added_velocity;	// bottom center part of the player
	vec3 boxColider;
	vec3 colision;
	Chunk* chunkOn;

	material inventory[8];
	
	float placing_block_timer;
	float breaking_block_timer;

	material holdingMaterial;
	vec3 targetingBlock;
	vec3 lastTargetingBlock;

	float height;
	float speed;
	float maxSpeed;
	
	float mouseSpeed = 0.001f;

	float maxTiltAngle = 10.0f;
	float currnettiltAngle = 0.0f;
	float tiltSpeed = 40.0f;

	float FoV_max = 45.8f;
	float FoV_min = 44.2f;
	float fovSpeed = 1.0001f;

	bool sprint_on = false;
	bool enableGravity;

	float horizontalAngle;
	float verticalAngle;
	float tiltRotation;
	float FoV;

	bool update_buffers;

	GLuint VAO, meshVerticiesVBO,meshInfoVBO;
	vector<int> verticies_mesh_data_low;
	vector<int> verticies_mesh_data_high;
	vector<int> verticiesInfo;

	GLFWwindow* window;
	Camera* camera;
	mat4 MVP;

	Player(float x, float y, float z, GLFWwindow* window);

	void drawCursor(GLuint programshader);
	void update(float dt);
	void handleInputs();
	void handleColitions(float dt);
	void rayCasting(void);
	void createMesh();
	void draw();

private:
};

#endif