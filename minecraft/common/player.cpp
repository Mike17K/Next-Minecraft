#include "player.h"
#include "./chunk.h"
#include "GlobalVariables.h"
#include "chunk.h"
#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>/
#include <iostream>


using namespace std;
using namespace glm;



float jump_speed = 5;

Player::Player(float x, float y, float z, GLFWwindow* window) {
	this->placing_block_timer=0;
	this->breaking_block_timer = 0;

	this->position = vec3(x,y,z);
	boxColider = vec3(0.8f,1.8f,0.8f);
	
	int player_chunk_x = (int)(this->position.x / BLOCK_SIZE_X / 16);
	int player_chunk_z = (int)(this->position.z / BLOCK_SIZE_Y / 16);

	this->chunkOn = Chunk::world_chunk_map[WORLD_CHUNKS_SIZE / 2 + player_chunk_x][WORLD_CHUNKS_SIZE / 2 + player_chunk_z];

	this->maxSpeed = 3.0f;
	this->speed = 6.0f;

	this->height = 1.8;

	camera = new Camera(window);
    this->window = window;
	
	this->enableGravity = true;
	this->update_buffers = false;

	this->inventory[0]=stone;
	this->inventory[1]=jungle_wood;
	this->inventory[2]=diamond_ore;
	this->inventory[3]=iron_ore;
	this->inventory[4]=dirt;
	this->inventory[5]=air;
	this->inventory[6]=air;
	this->inventory[7]=air;

	this->holdingMaterial = this->inventory[0];


	this->createMesh();
};

void Player::update(float dt) {
	this->handleInputs();
	this->rayCasting();

	
	this->handleColitions(dt);
	

	this->velocity.x += this->acceleration.x * dt ;
	this->velocity.y += this->acceleration.y * dt + ((this->enableGravity)? -g * dt:0);
	this->velocity.z += this->acceleration.z * dt ;

	

	this->position.x += this->velocity.x * dt + this->added_velocity.x*dt;
	this->position.y += this->velocity.y * dt;
	this->position.z += this->velocity.z * dt + this->added_velocity.z * dt;
	
	this->camera->position = this->position+vec3(0.5f,this->height, 0.5f);
	this->camera->update();
	
	mat4 projectionMatrix = camera->projectionMatrix;
	mat4 viewMatrix = camera->viewMatrix;
	mat4 modelMatrix = glm::mat4(1.0);

	this->MVP = projectionMatrix * viewMatrix * modelMatrix;

	int player_chunk_x = (int)(this->position.x / BLOCK_SIZE_X / 16 + (this->position.x <0? -1:0 ));
	int player_chunk_z = (int)(this->position.z / BLOCK_SIZE_Y / 16 + (this->position.z <0? -1:0));

	this->chunkOn = Chunk::world_chunk_map[WORLD_CHUNKS_SIZE / 2 + player_chunk_x][WORLD_CHUNKS_SIZE / 2 + player_chunk_z];

};

void Player::createMesh() {
	verticies_mesh_data_low.clear();
	verticies_mesh_data_high.clear();
	verticiesInfo.clear();

	// make the triangles========================== fix

};

void Player::drawCursor(GLuint programshader) {
	glUseProgram(programshader);

	GLfloat vertices[] =
	{
		//position and color
		-0.01f, -0.01f, 0.0f,     0.0f, 0.0f, 0.0f,0.5f, //bottom left
		0.01f, -0.01f,  0.0f,      0.0f, 0.0f, 0.0f,0.5f, //bottom right
		0.01f, 0.01f,   0.0f,       0.0f, 0.0f, 0.0f,0.5f, //middle top

		0.01f, 0.01f, 0.0f,     0.0f, 0.0f, 0.0f,0.5f, //bottom left
		-0.01f, 0.01f,0.0f,      0.0f, 0.0f, 0.0f,0.5f, //bottom right
		-0.01f,-0.01f,0.0f,       0.0f, 0.0f, 0.0f,0.5f, //middle top

	};

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7/*position & color total*/ * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	
	glDrawArrays(GL_TRIANGLES, 0, 7*2);

	glBindVertexArray(0);

};

void Player::draw() {
	// VAO
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	// vertex VBO
	glGenBuffers(1, &this->meshVerticiesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->meshVerticiesVBO);
	glBufferData(GL_ARRAY_BUFFER, this->verticies_mesh_data_low.size() * sizeof(int), &this->verticies_mesh_data_low[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(int), NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &this->meshVerticiesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->meshVerticiesVBO);
	glBufferData(GL_ARRAY_BUFFER, this->verticies_mesh_data_high.size() * sizeof(int), &this->verticies_mesh_data_high[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(int), NULL);
	glEnableVertexAttribArray(1);


	// uvs VBO
	glGenBuffers(1, &this->meshInfoVBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->meshInfoVBO);
	glBufferData(GL_ARRAY_BUFFER, this->verticiesInfo.size() * sizeof(int), &this->verticiesInfo[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)this->verticiesInfo.size());

};

void Player::handleInputs() {
	// gravity 		
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		this->sprint_on = true;
	}

	float final_speed = (this->sprint_on ? 2 : 1) * this->speed;
	// Task 5.5: update camera position using the direction/right vectors
	// Move forward
	this->verticalAngle = this->camera->verticalAngle;
	this->horizontalAngle = this->camera->horizontalAngle;
	vec3 direction(
		cos(this->verticalAngle) * sin(this->horizontalAngle),
		sin(this->verticalAngle),
		cos(this->verticalAngle) * cos(this->horizontalAngle)
	);

	// Right, up, front vector
	vec3 right(-cos(this->horizontalAngle), 0, sin(this->horizontalAngle));
	vec3 up(0.0f, 1.0f, 0.0);
	vec3 front = cross(up, right);
	this->added_velocity = vec3(0.0f, 0.0f, 0.0f);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		this->added_velocity += front * final_speed;
	}
	else {
		this->sprint_on = false;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		this->added_velocity -= front * final_speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		this->added_velocity += right * final_speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		this->added_velocity += -right * final_speed;
	}
	// move up
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && this->colision.y == -1) {
		
		this->velocity.y = jump_speed;
		this->enableGravity = true;
	}

	/*
	// move down
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		this->velocity.y = -jump_speed;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS) {
		this->velocity.y = 0;
	}
	//*/

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		this->holdingMaterial = this->inventory[0];
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		this->holdingMaterial = this->inventory[1];
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		this->holdingMaterial = this->inventory[2];
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
		this->holdingMaterial = this->inventory[3];
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
		this->holdingMaterial = this->inventory[4];
	}
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
		this->holdingMaterial = this->inventory[5];
	}
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
		this->holdingMaterial = this->inventory[6];
	}
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
		this->holdingMaterial = this->inventory[7];
	}

};

void Player::handleColitions(float dt) {
	
	vec3 prevPos = this->position - this->velocity * dt;
	Chunk* targetChunk;

	if (this->chunkOn != NULL && (int)this->position.y + 1 < CHUNK_HEIGHT) {

		// colision down
		int x = this->position.x / BLOCK_SIZE_X;
		int y = this->position.y / BLOCK_SIZE_Y - 0.1;
		int z = this->position.z / BLOCK_SIZE_Z;

		x %= 16; x += 16; x %= 16;
		z %= 16; z += 16; z %= 16;

		if (this->chunkOn->chunkArray[z][x][y] != 0) {
			//cout << "Colizion DOWN " << endl;
			if (this->velocity.y < 0) {
				this->velocity.y = 0;
				this->enableGravity = false;
				this->colision.y = -1;
			}
		} 
		if (this->chunkOn->chunkArray[z][x][y] == 0) {
			this->colision.y = 0;
			this->enableGravity = true;
		}
		
		//*
		// colision up
		if ((int)this->position.y + 2 < CHUNK_HEIGHT - 1)
		if (this->chunkOn->chunkArray[z][x][y + 2] != 0) {
			cout << "Colizion Up" << endl;
		}
		//*/
	
			
		/*
		// colision front
			
		x = this->position.x / BLOCK_SIZE_X + 0.1;
		y = this->position.y / BLOCK_SIZE_Y;
		z = this->position.z / BLOCK_SIZE_Z;

		targetChunk = Chunk::world_chunk_map[WORLD_CHUNKS_SIZE / 2 + x / CHUNK_SIZE / BLOCK_SIZE_X + (x < 0 ? -1 : 0)][WORLD_CHUNKS_SIZE / 2 + z / CHUNK_SIZE / BLOCK_SIZE_Z + (z < 0 ? -1 : 0)];
		
		x %= 16; x += 16; x %= 16;
		z %= 16; z += 16; z %= 16;
		
		if (targetChunk->chunkArray[z][x][y] != air || targetChunk->chunkArray[z][x][y + 1] != air) {
			//cout << "Colizion front" << endl;
			if (this->velocity.x > 0) this->velocity.x = 0;
			
			if (this->added_velocity.x >0)this->added_velocity.x = 0;
		}

		// colision back
		x = this->position.x / BLOCK_SIZE_X - 0.1;
		y = this->position.y / BLOCK_SIZE_Y;
		z = this->position.z / BLOCK_SIZE_Z;

		targetChunk = Chunk::world_chunk_map[WORLD_CHUNKS_SIZE / 2 + x / CHUNK_SIZE / BLOCK_SIZE_X + (x < 0 ? -1 : 0)][WORLD_CHUNKS_SIZE / 2 + z / CHUNK_SIZE / BLOCK_SIZE_Z + (z < 0 ? -1 : 0)];

		x %= 16; x += 16; x %= 16;
		z %= 16; z += 16; z %= 16;

		if (targetChunk->chunkArray[z][x][y] != air || targetChunk->chunkArray[z][x][y + 1] != air) {
			//cout << "Colizion front" << endl;
			if (this->velocity.x < 0) this->velocity.x = 0;

			if (this->added_velocity.x < 0)this->added_velocity.x = 0;
		}

		// colision left
		x = this->position.x / BLOCK_SIZE_X ;
		y = this->position.y / BLOCK_SIZE_Y;
		z = this->position.z / BLOCK_SIZE_Z - 0.1;

		targetChunk = Chunk::world_chunk_map[WORLD_CHUNKS_SIZE / 2 + x / CHUNK_SIZE / BLOCK_SIZE_X + (x < 0 ? -1 : 0)][WORLD_CHUNKS_SIZE / 2 + z / CHUNK_SIZE / BLOCK_SIZE_Z + (z < 0 ? -1 : 0)];

		x %= 16; x += 16; x %= 16;
		z %= 16; z += 16; z %= 16;

		if (targetChunk->chunkArray[z][x][y] != air || targetChunk->chunkArray[z][x][y + 1] != air) {
			//cout << "Colizion front" << endl;
			if (this->velocity.z < 0) this->velocity.z = 0;

			if (this->added_velocity.z < 0)this->added_velocity.z= 0;
		}

		// colision right
		x = this->position.x / BLOCK_SIZE_X;
		y = this->position.y / BLOCK_SIZE_Y;
		z = this->position.z / BLOCK_SIZE_Z + 0.1;

		targetChunk = Chunk::world_chunk_map[WORLD_CHUNKS_SIZE / 2 + x / CHUNK_SIZE / BLOCK_SIZE_X + (x < 0 ? -1 : 0)][WORLD_CHUNKS_SIZE / 2 + z / CHUNK_SIZE / BLOCK_SIZE_Z + (z < 0 ? -1 : 0)];

		x %= 16; x += 16; x %= 16;
		z %= 16; z += 16; z %= 16;

		if (targetChunk->chunkArray[z][x][y] != air || targetChunk->chunkArray[z][x][y + 1] != air) {
			//cout << "Colizion front" << endl;
			if (this->velocity.z > 0) this->velocity.z = 0;

			if (this->added_velocity.z > 0)this->added_velocity.z = 0;
		}
		*/
		
	}
	else {
		cout << "chunk null" << endl;
	}
	/*

	// colision front and back
	if (this->chunkOn != NULL) {
		if (this->chunkOn->chunkArray[z][x+1][y] != 0 || this->chunkOn->chunkArray[z][x + 1][y+1] != 0
		|| this->chunkOn->chunkArray[z][x - 1][y] != 0 || this->chunkOn->chunkArray[z][x - 1][y + 1]) {
			this->position.x = prevPos.x;
			this->colision.x = 1;
			//this->velocity.y = 0;
		}
		else {
			this->colision.x = 0;
		}
	}
	*/


	

};

void Player::rayCasting(void) {

	vec3 startPosition = this->camera->position;
	float verticalAngle = this->camera->verticalAngle;
	float horizontalAngle = this->camera->horizontalAngle;

	// make the line function
	int maxDepth = 4; // 5 blocks 
	float step = 0.1;
	
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		this->breaking_block_timer ++;
	}
	else {
		this->breaking_block_timer = 0;
	}

	this->placing_block_timer++;

	//cout << this->breaking_block_timer << " " << this->placing_block_timer << endl;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		for (int i = 0; i < maxDepth / step; i++) {
			float tmp_y = startPosition.y + step * i * glm::sin(verticalAngle);
			float tmp_z = startPosition.z + step * i * glm::cos(verticalAngle) * glm::cos(horizontalAngle);
			float tmp_x = startPosition.x + step * i * glm::cos(verticalAngle) * glm::sin(horizontalAngle);

			int y = tmp_y;
			int z = tmp_z;
			int x = tmp_x;
			
			if (tmp_x < 0) {
				x -= 1;
			}
			if (tmp_z < 0) {
				z -= 1;
			}
			
			Chunk* chunkLookingAt,*nextChunks[4];
			chunkLookingAt = Chunk::world_chunk_map[WORLD_CHUNKS_SIZE / 2 + x / CHUNK_SIZE / BLOCK_SIZE_X + (x < 0 ? -1 : 0)][WORLD_CHUNKS_SIZE / 2 + z / CHUNK_SIZE / BLOCK_SIZE_Z + (z < 0 ? -1 : 0)];
			//x + 1
			nextChunks[0] = Chunk::world_chunk_map[WORLD_CHUNKS_SIZE / 2 + x / CHUNK_SIZE / BLOCK_SIZE_X + (x < 0 ? -1 : 0)+1][WORLD_CHUNKS_SIZE / 2 + z / CHUNK_SIZE / BLOCK_SIZE_Z + (z < 0 ? -1 : 0)];
			//x - 1 
			nextChunks[1] = Chunk::world_chunk_map[WORLD_CHUNKS_SIZE / 2 + x / CHUNK_SIZE / BLOCK_SIZE_X + (x < 0 ? -1 : 0)-1][WORLD_CHUNKS_SIZE / 2 + z / CHUNK_SIZE / BLOCK_SIZE_Z + (z < 0 ? -1 : 0)];
			//z + 1
			nextChunks[2] = Chunk::world_chunk_map[WORLD_CHUNKS_SIZE / 2 + x / CHUNK_SIZE / BLOCK_SIZE_X + (x < 0 ? -1 : 0)][WORLD_CHUNKS_SIZE / 2 + z / CHUNK_SIZE / BLOCK_SIZE_Z + (z < 0 ? -1 : 0)+1];
			//z - 1
			nextChunks[3] = Chunk::world_chunk_map[WORLD_CHUNKS_SIZE / 2 + x / CHUNK_SIZE / BLOCK_SIZE_X + (x < 0 ? -1 : 0)][WORLD_CHUNKS_SIZE / 2 + z / CHUNK_SIZE / BLOCK_SIZE_Z + (z < 0 ? -1 : 0)-1];

			x %= 16; x += 16; x %= 16;
			z %= 16; z += 16; z %= 16;

			
			


			if (chunkLookingAt != NULL) {
				material targetMaterial = chunkLookingAt->chunkArray[z][x][y];
				if (targetMaterial != air && targetMaterial != water_overlay) {

					this->targetingBlock = vec3(x,y,z); // &chunkLookingAt->chunkArray[z][x][y];

					if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
						// get the face of where we looking at
						float tmp_y = startPosition.y + step * (i-1) * glm::sin(verticalAngle);
						float tmp_z = startPosition.z + step * (i-1) * glm::cos(verticalAngle) * glm::cos(horizontalAngle);
						float tmp_x = startPosition.x + step * (i-1) * glm::cos(verticalAngle) * glm::sin(horizontalAngle);

						int prev_y = tmp_y;
						int prev_z = tmp_z;
						int prev_x = tmp_x;

						if (tmp_x < 0) {
							prev_x -= 1;
						}
						if (tmp_z < 0) {
							prev_z -= 1;
						}

						Chunk* prev_chunkLookingAt, * prev_nextChunks[4];
						prev_chunkLookingAt = Chunk::world_chunk_map[WORLD_CHUNKS_SIZE / 2 + prev_x / CHUNK_SIZE / BLOCK_SIZE_X + (prev_x < 0 ? -1 : 0)][WORLD_CHUNKS_SIZE / 2 + prev_z / CHUNK_SIZE / BLOCK_SIZE_Z + (prev_z < 0 ? -1 : 0)];
						//x + 1
						prev_nextChunks[0] = Chunk::world_chunk_map[WORLD_CHUNKS_SIZE / 2 + prev_x / CHUNK_SIZE / BLOCK_SIZE_X + (prev_x < 0 ? -1 : 0) + 1][WORLD_CHUNKS_SIZE / 2 + prev_z / CHUNK_SIZE / BLOCK_SIZE_Z + (prev_z < 0 ? -1 : 0)];
						//x - 1 
						prev_nextChunks[1] = Chunk::world_chunk_map[WORLD_CHUNKS_SIZE / 2 + prev_x / CHUNK_SIZE / BLOCK_SIZE_X + (prev_x < 0 ? -1 : 0) - 1][WORLD_CHUNKS_SIZE / 2 + prev_z / CHUNK_SIZE / BLOCK_SIZE_Z + (prev_z < 0 ? -1 : 0)];
						//z + 1
						prev_nextChunks[2] = Chunk::world_chunk_map[WORLD_CHUNKS_SIZE / 2 + prev_x / CHUNK_SIZE / BLOCK_SIZE_X + (prev_x < 0 ? -1 : 0)][WORLD_CHUNKS_SIZE / 2 + prev_z / CHUNK_SIZE / BLOCK_SIZE_Z + (prev_z < 0 ? -1 : 0) + 1];
						//z - 1
						prev_nextChunks[3] = Chunk::world_chunk_map[WORLD_CHUNKS_SIZE / 2 + prev_x / CHUNK_SIZE / BLOCK_SIZE_X + (prev_x < 0 ? -1 : 0)][WORLD_CHUNKS_SIZE / 2 + prev_z / CHUNK_SIZE / BLOCK_SIZE_Z + (prev_z < 0 ? -1 : 0) - 1];

						prev_x %= 16; prev_x += 16; prev_x %= 16;
						prev_z %= 16; prev_z += 16; prev_z %= 16;

						if (prev_chunkLookingAt != NULL) {
							if (prev_chunkLookingAt->chunkArray[prev_z][prev_x][prev_y]!=water_overlay && prev_chunkLookingAt->chunkArray[prev_z][prev_x][prev_y] != air) return;
							
							if (this->placing_block_timer < 10) return;
							this->placing_block_timer = 0;

							prev_chunkLookingAt->chunkArray[prev_z][prev_x][prev_y] = this->holdingMaterial;

							cout << prev_x << " " << prev_y << " " << prev_z << "  Material: " << targetMaterial << endl;
							chunkLookingAt = prev_chunkLookingAt;
							

						}

					}

					
					if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
						// delete the block
						if (this->targetingBlock != this->lastTargetingBlock) {
							this->breaking_block_timer = 0;
							this->lastTargetingBlock = this->targetingBlock;
						}

						int targetTimer = 20; //modify based of block /////////////////////////////////////
						if (this->breaking_block_timer < targetTimer) return;

						this->breaking_block_timer = 0;

						chunkLookingAt->chunkArray[z][x][y] = air;

						cout << x << " " << y << " " << z << "  Material: " << targetMaterial << endl;

					}
					

					chunkLookingAt->verticiesInfo.clear();
					chunkLookingAt->verticies_mesh_data_high.clear();
					chunkLookingAt->verticies_mesh_data_low.clear();
					chunkLookingAt->genMesh(); // genMesh not hole chunck

					this->update_buffers = true;

					// update naibur chunks
					if (x == 0) {
						Chunk* sideChunk = nextChunks[1];
						sideChunk->verticiesInfo.clear();
						sideChunk->verticies_mesh_data_high.clear();
						sideChunk->verticies_mesh_data_low.clear();
						sideChunk->genMesh();
					}
					if (x == 15) {
						Chunk* sideChunk = nextChunks[0];
						sideChunk->verticiesInfo.clear();
						sideChunk->verticies_mesh_data_high.clear();
						sideChunk->verticies_mesh_data_low.clear();
						sideChunk->genMesh();
					}
					if (z == 0) {
						Chunk* sideChunk = nextChunks[3]; 
						sideChunk->verticiesInfo.clear();
						sideChunk->verticies_mesh_data_high.clear();
						sideChunk->verticies_mesh_data_low.clear();
						sideChunk->genMesh();
					}
					if (z == 15) {
						Chunk* sideChunk = nextChunks[2]; 
						sideChunk->verticiesInfo.clear();
						sideChunk->verticies_mesh_data_high.clear();
						sideChunk->verticies_mesh_data_low.clear();
						sideChunk->genMesh();
					}

					return;
				}

			}

		}
	}




};
