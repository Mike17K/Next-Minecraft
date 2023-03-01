#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>
#include "./GlobalVariables.h"
#include "./fastNoiseLite.h"
#include "./PerlinNoice.h"


using namespace std;
using namespace glm;


// fix set Point with new arguments
void setPoint(int* vertex_data_low, int* vertex_data_high, int x, int y, int z, int index_x, int index_z);
int getAxis(int data_low, int data_high, int choice);
void dims(int t, int* x, int* z);
void invDim(int x, int z, int* t);
float getInfo(int UV, int choice);
int setInfo(int u, int v,int sideNormal,material m=air);

void init(void);

class Chunk
{
public:
	static int chunk_counter;
	static Chunk* world_chunk_map[WORLD_CHUNKS_SIZE][WORLD_CHUNKS_SIZE];
	static vector<Block*> world_chunk_extra_blocks[WORLD_CHUNKS_SIZE][WORLD_CHUNKS_SIZE];

	static void genTotalMesh(std::vector <Chunk*>& instances);
    static void getTotalMesh(std::vector<Chunk*> &instances,std::vector<int>& vertices_data_low, std::vector<int>& vertices_data_high, std::vector<int>& verticiesInfo);
	static void getTotalMeshWater(std::vector<Chunk*>& instances, std::vector<int>& vertices_data_low, std::vector<int>& vertices_data_high, std::vector<int>& verticiesInfo);

	GLuint meshVAO;
	GLuint meshVerticiesVBO, meshInfoVBO;

	GLuint meshVAO_water;
	GLuint meshVerticiesVBO_water, meshInfoVBO_water;

	int index_x, index_z;

	void bind(void);
	void bindWater(void);
	void draw(void);
	void drawWater(void);

	Chunk(int index_x,int index_y);
	void brakeBlock(glm::vec3 worldTargetBlock);
	void addBlock(glm::vec3 worldTargetBlock);
	
	void updateMeshLocaly(glm::vec3 updatedBlock); //additional use when i need update from a naibur chunk
	void deleteChunk();

	void getMesh(std::vector<int>& vertices_data_low, std::vector<int>& vertices_data_high, std::vector<int> &verticiesInfo);
	void getMeshWater(std::vector<int>& vertices_data_low, std::vector<int>& vertices_data_high, std::vector<int>& verticiesInfo);

    material chunkArray[CHUNK_SIZE][CHUNK_SIZE][CHUNK_HEIGHT] = { air };

	//vector<Triangle> triangleMesh; //go to private
	//vector<vec3> verticiesMesh; //go to private
    //vector<uint_fast64_t> verticiesMesh; //go to private
    vector<int> verticies_mesh_data_low, verticies_mesh_data_high,verticiesInfo;
	vector<int> verticies_mesh_data_low_water, verticies_mesh_data_high_water, verticiesInfo_water;

	void loadChunk();
	void saveChunk();
	void genArray(void);
	void genMesh(bool bindflag= false);
    void genGreedyMesh();
private:

};

#endif

