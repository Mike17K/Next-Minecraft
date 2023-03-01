#include "./chunk.h"
#include "./tree.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <glfw3.h>

#include <iostream>
#include <cstdlib>

#include <thread>
#include <mutex>

#include <vector>

#include "./GlobalVariables.h"
#include "./fastNoiseLite.h"
#include "./PerlinNoice.h"

using namespace std;
using namespace glm;


Chunk* Chunk::world_chunk_map[WORLD_CHUNKS_SIZE][WORLD_CHUNKS_SIZE] = { NULL };
vector<Block*> Chunk::world_chunk_extra_blocks[WORLD_CHUNKS_SIZE][WORLD_CHUNKS_SIZE] = {};

int Chunk::chunk_counter = 0;

// fix set Point with new arguments
void setPoint(int* vertex_data_low, int* vertex_data_high, int x, int y, int z, int index_x, int index_z) {
    if (x == CHUNK_SIZE) {
        index_x++;
        x = 0;
    }
    if (z == CHUNK_SIZE) {
        index_z++;
        z = 0;
    }
    //int x_ = ((x & 0xF)) << 27;
    //int y_ = ((y & 0xFF)) << 23;
    //int z_ = ((z & 0xF)) << 23;

    //int index_x_ = (index_x & 0x807FFFFF);
    //int index_z_ = (index_z & 0x807FFFFF);


    //cout <<"have to be zero: " << (x_ & y_ & z_ & index_x_ & index_z_) << endl;
    //*vertex_data_low = (int)(y_ | index_x_);
    //*vertex_data_high = (int)(x_ | z_ | index_z_);
    * vertex_data_low = (y<<23 | (index_x & 0x807FFFFF));
    *vertex_data_high = (x<<27 | z<<23 | (index_z & 0x807FFFFF));

}



// it will contain the poiners to all world chunks for x , z dimentions.
// the chunk of (x,z) is world_chunk_map[WORLD_CHUNKS_SIZE/2+x][WORLD_CHUNKS_SIZE/2+z];  


Chunk::Chunk(int index_x, int index_z) {

    this->index_x = index_x;
    this->index_z = index_z;

    this->genArray();

    Chunk::chunk_counter++;
    Chunk::world_chunk_map[WORLD_CHUNKS_SIZE / 2 + this->index_x][WORLD_CHUNKS_SIZE / 2 + this->index_z] = this;
};

void Chunk::brakeBlock(glm::vec3 worldTargetBlock) {};
void Chunk::addBlock(glm::vec3 worldTargetBlock) {};

void Chunk::updateMeshLocaly(glm::vec3 updatedBlock) {}; //additional use when i need update from a naibur chunk
void Chunk::deleteChunk() {};

void Chunk::loadChunk() {};
void Chunk::saveChunk() {};

void Chunk::genArray() {

    int chunk_x = this->index_x;
    int chunk_z = this->index_z;

    NoiseGenerator max_hight_noise= NoiseGenerator(10);
    NoiseGenerator dirt_hight_noise = NoiseGenerator(14);
    NoiseGenerator snow_hight_noise = NoiseGenerator(15);
    snow_hight_noise.m_noiseParameters.amplitude = SNOWLEVEL;

    //srand(); // seed to random
    

    //FastNoiseLite* noiceLight = new FastNoiseLite();
    
    //auto t1= glfwGetTime() - start;

    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            int h_max = (int)max_hight_noise.getHeight(x, z, chunk_x+1000, chunk_z + 1000);
            int h_max_dirt = (int)dirt_hight_noise.getHeight(x, z, chunk_x + 1000, chunk_z + 1000);
            int h_min_snow = (int)snow_hight_noise.getHeight(x, z, chunk_x + 1000, chunk_z + 1000);
            
            
            //int h_max = (int) 50+20*noiceLight->GetNoise((float) x + CHUNK_SIZE*chunk_x, (float) z + CHUNK_SIZE * chunk_z); //(float)temp_noiseGen.getHeight(x, z,chunk_x, chunk_z);
            for (int h = 0; h < glm::max(glm::min(h_max, CHUNK_HEIGHT),WATERLEVEL); h++) {

                // make content for blocks here//////////////////////////////////////////////////more blocks

                bool addTree = false;

                if (h==0) this->chunkArray[z][x][h] = bedrock;
                else if (h<=h_max*0.74 + (h_max_dirt-50)/3) this->chunkArray[z][x][h] = stone;
                else if (h < h_max-1 && h > h_min_snow) this->chunkArray[z][x][h] = snow; //add some noise to the height
                else if (h < h_max -1) this->chunkArray[z][x][h] = dirt;
                else {
                    this->chunkArray[z][x][h] = grass_block;    
                    if (h > h_min_snow) this->chunkArray[z][x][h] = snow;
                    addTree = true;                    
                }

                if (h <= WATERLEVEL && h>h_max) {
                    this->chunkArray[z][x][h] = water_overlay;
                }


                if (addTree) {
                // add trees at the surface
                if ((float)rand() / RAND_MAX > 0.998 && (h > WATERLEVEL)) {
                    Tree t = Tree(x, h + 1, z, this->index_x, this->index_z, 0, 0);
                    t.addToChunkArray(this); //if its in the edges of chunks update for nabour chunks
                }
                }


                

            }
        }
    }

    //cout << this->index_x << endl; //<< " " << WORLD_CHUNKS_SIZE / 2 + this->index_z<<endl;

    vector<Block*> tmp = Chunk::world_chunk_extra_blocks[WORLD_CHUNKS_SIZE / 2 + this->index_x][WORLD_CHUNKS_SIZE / 2 + this->index_z];
    for (int i = 0; i < tmp.size(); i++) {
        this->chunkArray[tmp[i]->z][tmp[i]->x][tmp[i]->y] = tmp[i]->m;
          //cout << this->index_x <<" " << this->index_z << endl;
    };

    //cout << glfwGetTime() - t1-start <<" "<<t1 << endl;
};

//fix gen Mesh to give uvs and normals////////////////////////////////////////////////////////////////////////////////
void Chunk::genGreedyMesh() {

}

//fix gen Mesh to give uvs and normals////////////////////////////////////////////////////////////////////////////////
void Chunk::genMesh(bool bindflag) {
    //auto start = glfwGetTime();

    this->verticies_mesh_data_low.clear();
    this->verticies_mesh_data_high.clear();
    int chunk_x = this->index_x;
    int chunk_z = this->index_z;

    for (int h = 0; h < CHUNK_HEIGHT + 2; h++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                // create the triangles of those blocks at their fixed coords

                if (h > 1) {
                    if (this->chunkArray[z][x][h - 2] != air && this->chunkArray[z][x][h - 2] != water_overlay) {

                        int vertex_data_low, vertex_data_high;

                        int u_right_side, v_right_side;
                        int u_left_side, v_left_side;
                        int u_front_side, v_front_side;
                        int u_back_side, v_back_side;
                        int u_top_side, v_top_side;
                        int u_bottom_side, v_bottom_side;

                        material m = this->chunkArray[z][x][h - 2];

                        materialToUVS(m,&u_right_side, &v_right_side,&u_left_side, &v_left_side,&u_front_side, &v_front_side,&u_back_side, &v_back_side,&u_top_side, &v_top_side,&u_bottom_side, &v_bottom_side);


                        
                        

                        // check down h-3
                        bool addTriangles = false;
                        if (h - 2 == 0) {
                            addTriangles = true;
                        }
                        else if (this->chunkArray[z][x][h - 3] == air || this->chunkArray[z][x][h - 3] == water_overlay) {
                            addTriangles = true;
                        }
                        if (addTriangles) {
                            //add bottom 2 triangles to trianglesToAir

                            // triangle 1
                            // p1 = {x_start,h,z_start}
                            // p2 = {x_end,h,z_start}
                            // p3 = {x_end,h,z_end}


                            /*
                            this->verticiesMesh.push_back(Triangle(x_start, (h - 2) * BLOCK_SIZE_Y, z_start, chunk_x, chunk_z));
                            this->verticiesMesh.push_back(vec3(x_end, (h - 2) * BLOCK_SIZE_Y, z_start));
                            this->verticiesMesh.push_back(vec3(x_end, (h - 2) * BLOCK_SIZE_Y, z_end));
                            */
                            // triangle 2
                            // p3 = {x_end,h,z_end}
                            // p2 = {x_start,h,z_end}
                            // p1 = {x_start,h,z_start}
                            /*
                            this->verticiesMesh.push_back(vec3(x_end, (h - 2) * BLOCK_SIZE_Y, z_end));
                            this->verticiesMesh.push_back(vec3(x_start, (h - 2) * BLOCK_SIZE_Y, z_end));
                            this->verticiesMesh.push_back(vec3(x_start, (h - 2) * BLOCK_SIZE_Y, z_start));
                            */

                            this->verticiesInfo.push_back(setInfo(u_bottom_side + 1, v_bottom_side + 1, 0,m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_bottom_side + 1, v_bottom_side, 0, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_bottom_side, v_bottom_side, 0, m)); // add 
                            this->verticiesInfo.push_back(setInfo(u_bottom_side, v_bottom_side, 0, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_bottom_side, v_bottom_side+1, 0, m)); // add uvs  this is ok position middle
                            this->verticiesInfo.push_back(setInfo(u_bottom_side + 1, v_bottom_side +1, 0, m)); // add uvs

                                setPoint(&vertex_data_low, &vertex_data_high, x, h - 2, z, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 2, z, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 2, z + 1, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);

                                setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 2, z + 1, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x, h - 2, z + 1, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x, h - 2, z, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);

                        }

                        // check up h-1
                        addTriangles = false;
                        if (h - 1 == CHUNK_HEIGHT) {
                            addTriangles = true;
                        }
                        else if (this->chunkArray[z][x][h - 1] == air || this->chunkArray[z][x][h - 1] == water_overlay) {
                            addTriangles = true;
                        }

                        if (addTriangles) {

                            //add top 2 triangles to trianglesToAir

                            // triangle 1
                            // p1 = {x_start,h+1,z_start}
                            // p3 = {x_end,h+1,z_end}
                            // p2 = {x_end,h+1,z_start}

                            /*
                            this->verticiesMesh.push_back(vec3(x_start, (h - 1) * BLOCK_SIZE_Y, z_start));
                            this->verticiesMesh.push_back(vec3(x_end, (h - 1) * BLOCK_SIZE_Y, z_end));
                            this->verticiesMesh.push_back(vec3(x_end, (h - 1) * BLOCK_SIZE_Y, z_start));
                            */
                            // triangle 2
                            // p1 = {x_start,h+1,z_start}
                            // p2 = {x_start,h+1,z_end}
                            // p3 = {x_end,h+1,z_end}
                            /*
                            this->verticiesMesh.push_back(vec3(x_start, (h - 1) * BLOCK_SIZE_Y, z_start));
                            this->verticiesMesh.push_back(vec3(x_start, (h - 1) * BLOCK_SIZE_Y, z_end));
                            this->verticiesMesh.push_back(vec3(x_end, (h - 1) * BLOCK_SIZE_Y, z_end));
                            */
                            this->verticiesInfo.push_back(setInfo(u_top_side,     v_top_side, 1, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_top_side + 1, v_top_side+1, 1, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_top_side + 1, v_top_side, 1, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_top_side,     v_top_side, 1, m)); // add uvs  this is ok position middle
                            this->verticiesInfo.push_back(setInfo(u_top_side,     v_top_side + 1, 1, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_top_side+1,   v_top_side+1, 1, m)); // add uvs

                            

                                setPoint(&vertex_data_low, &vertex_data_high, x, h - 1, z, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 1, z + 1, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 1, z, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);

                                setPoint(&vertex_data_low, &vertex_data_high, x, h - 1, z, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x, h - 1, z + 1, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 1, z + 1, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                            
                                //  add texture uvs
                                //int tmp_info = 0xF;
                                //verticiesInfo.push_back();


                        }


                        // check x z-1 h-2
                        addTriangles = false;
                        if (z == 0) {
                            addTriangles = true;
                            // check the chunk(index_x,index_z-1) at Chunkblockarray[CHUNK_SIZE][x][h-2]
                            Chunk *target_chunk = world_chunk_map[WORLD_CHUNKS_SIZE / 2 + index_x][WORLD_CHUNKS_SIZE / 2 + index_z - 1];
                            if (target_chunk != NULL) {

                                if (target_chunk->index_z == index_z - 1) {
                                addTriangles = target_chunk->chunkArray[CHUNK_SIZE - 1][x][h - 2] == air; //if i wana add water i put here alst the id of water 0:air
                                addTriangles = addTriangles || target_chunk->chunkArray[CHUNK_SIZE - 1][x][h - 2] == water_overlay;
                                }
                            }
                            else {
                                addTriangles = true;
                            }
                            

                            /*
                            for (int i = 0; i < Chunk::instances.size(); i++) { //posible i can do this operation with dictionary
                                if (Chunk::instances[i]->index_x == chunk_x && Chunk::instances[i]->index_z == chunk_z - 1) {
                                    if (Chunk::instances[i]->chunkArray[CHUNK_SIZE - 1][x][h - 2] == 1) {
                                        addTriangles = false;
                                    }
                                    break;
                                }
                            }
                            */
                        }
                        else if (this->chunkArray[z - 1][x][h - 2] == air || this->chunkArray[z-1][x][h - 2] == water_overlay) {
                            addTriangles = true;
                        }
                        if (addTriangles) {

                            //add 2 triangles to trianglesToAir

                                // triangle 1
                            // p1 = {x_end,h,z_start}
                            // p2 = {x_start,h,z_start}
                            // p3 = {x_start,h+1,z_start}
/*
                            this->verticiesMesh.push_back(vec3(x_end, (h - 2) * BLOCK_SIZE_Y, z_start));
                            this->verticiesMesh.push_back(vec3(x_start, (h - 2) * BLOCK_SIZE_Y, z_start));
                            this->verticiesMesh.push_back(vec3(x_start, (h - 1) * BLOCK_SIZE_Y, z_start));
*/
// triangle 2
// p3 = {x_start,h+1,z_start}
// p2 = {x_end,h+1,z_start}
// p1 = {x_end,h,z_start}
/*
this->verticiesMesh.push_back(vec3(x_start, (h - 1) * BLOCK_SIZE_Y, z_start));
this->verticiesMesh.push_back(vec3(x_end, (h - 1) * BLOCK_SIZE_Y, z_start));
this->verticiesMesh.push_back(vec3(x_end, (h - 2) * BLOCK_SIZE_Y, z_start));
*/
                            this->verticiesInfo.push_back(setInfo(u_right_side+1, v_right_side+1, 2, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_right_side+1,v_right_side, 2, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_right_side, v_right_side, 2, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_right_side, v_right_side, 2, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_right_side, v_right_side+1, 2, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_right_side+1, v_right_side+1, 2, m)); // add uvs

                                setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 2, z, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x, h - 2, z, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x, h - 1, z, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);

                                setPoint(&vertex_data_low, &vertex_data_high, x, h - 1, z, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 1, z, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 2, z, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                            

                        }

                        // check x z+1 h-2
                        addTriangles = false;
                        if (z == CHUNK_SIZE - 1) {
                            addTriangles = true;

                            Chunk* target_chunk = world_chunk_map[WORLD_CHUNKS_SIZE / 2 + index_x][WORLD_CHUNKS_SIZE / 2 + index_z + 1];
                            
                            if (target_chunk != NULL) {

                            if (target_chunk->index_z == index_z+1) {
                                addTriangles = target_chunk->chunkArray[0][x][h - 2] == air; //if i wana add water i put here alst the id of water 0:air
                                addTriangles = addTriangles || target_chunk->chunkArray[0][x][h - 2] == water_overlay;
                            }
                            }
                            else {
                                addTriangles = true;
                            }
                            /*
                            for (int i = 0; i < Chunk::instances.size(); i++) {
                                if (Chunk::instances[i]->index_x == chunk_x && Chunk::instances[i]->index_z == chunk_z + 1) {
                                    if (Chunk::instances[i]->chunkArray[0][x][h - 2] == 1) {
                                        addTriangles = false;
                                    }
                                    break;
                                }
                            }
                            */
                        }
                        else if (this->chunkArray[z + 1][x][h - 2] == air || this->chunkArray[z+1][x][h - 2] == water_overlay) {
                            addTriangles = true;
                        }
                        if (addTriangles) {
                            //add 2 triangles to

                            // triangle 1
                            // p3 = {x_start,h+1,z_end}
                            // p2 = {x_start,h,z_end}
                            // p1 = {x_end,h,z_end}
                            /*
                            this->verticiesMesh.push_back(vec3(x_start, (h - 1) * BLOCK_SIZE_Y, z_end));
                            this->verticiesMesh.push_back(vec3(x_start, (h - 2) * BLOCK_SIZE_Y, z_end));
                            this->verticiesMesh.push_back(vec3(x_end, (h - 2) * BLOCK_SIZE_Y, z_end));
                            */
                            // triangle 2
                            // p1 = {x_end,h,z_end}
                            // p2 = {x_end,h+1,z_end}
                            // p3 = {x_start,h+1,z_end}
                            /*
                            this->verticiesMesh.push_back(vec3(x_end, (h - 2) * BLOCK_SIZE_Y, z_end));
                            this->verticiesMesh.push_back(vec3(x_end, (h - 1) * BLOCK_SIZE_Y, z_end));
                            this->verticiesMesh.push_back(vec3(x_start, (h - 1) * BLOCK_SIZE_Y, z_end));
                            */

                            this->verticiesInfo.push_back(setInfo(u_left_side, v_left_side, 3, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_left_side+1,v_left_side, 3, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_left_side+1, v_left_side+1, 3, m)); // add uvs             
                            this->verticiesInfo.push_back(setInfo(u_left_side+1, v_left_side+1, 3, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_left_side, v_left_side+1, 3, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_left_side, v_left_side, 3, m)); // add uvs


                                setPoint(&vertex_data_low, &vertex_data_high, x, h - 1, z + 1, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x, h - 2, z + 1, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 2, z + 1, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);

                                setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 2, z + 1, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 1, z + 1, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x, h - 1, z + 1, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                            

                        }


                        // check x-1 z h-2
                        addTriangles = false;
                        if (x == 0) {
                            addTriangles = true;

                            Chunk* target_chunk = world_chunk_map[WORLD_CHUNKS_SIZE / 2 + index_x-1][WORLD_CHUNKS_SIZE / 2 + index_z];
                            if (target_chunk != NULL) {

                                if (target_chunk->index_x == index_x - 1) {
                                addTriangles = target_chunk->chunkArray[z][CHUNK_SIZE - 1][h - 2] == air; //if i wana add water i put here alst the id of water 0:air
                                addTriangles = addTriangles || target_chunk->chunkArray[z][CHUNK_SIZE - 1][h - 2] == water_overlay;

                                }
                            }
                            else {
                                addTriangles = true;
                            }

                            /*
                            for (int i = 0; i < Chunk::instances.size(); i++) {
                                if (Chunk::instances[i]->index_x == chunk_x - 1 && Chunk::instances[i]->index_z == chunk_z) {
                                    if (Chunk::instances[i]->chunkArray[z][CHUNK_SIZE - 1][h - 2] == 1) {
                                        addTriangles = false;
                                    }
                                    break;
                                }
                            }
                            */
                        }
                        else if (this->chunkArray[z][x - 1][h - 2] == air || this->chunkArray[z][x-1][h - 2] == water_overlay) {
                            addTriangles = true;
                        }
                        if (addTriangles) {
                            //add 2 triangles to trianglesToAir

                            // triangle 1
                            // p3 = {x_start,h,z_start}
                            // p2 = {x_start,h,z_end}
                            // p1 = {x_start,h+1,z_end}
                            /*
                            this->verticiesMesh.push_back(vec3(x_start, (h - 2) * BLOCK_SIZE_Y, z_start));
                            this->verticiesMesh.push_back(vec3(x_start, (h - 2) * BLOCK_SIZE_Y, z_end));
                            this->verticiesMesh.push_back(vec3(x_start, (h - 1) * BLOCK_SIZE_Y, z_end));
                            */
                            // triangle 2
                            // p1 = {x_start,h+1,z_end}
                            // p2 = {x_start,h+1,z_start}
                            // p3 = {x_start,h,z_start}
                            /*
                            this->verticiesMesh.push_back(vec3(x_start, (h - 1) * BLOCK_SIZE_Y, z_end));
                            this->verticiesMesh.push_back(vec3(x_start, (h - 1) * BLOCK_SIZE_Y, z_start));
                            this->verticiesMesh.push_back(vec3(x_start, (h - 2) * BLOCK_SIZE_Y, z_start));
                            */

                            this->verticiesInfo.push_back(setInfo(u_front_side+1, v_front_side+1, 4, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_front_side+1,v_front_side, 4, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_front_side, v_front_side, 4, m)); // add uvs        
                            this->verticiesInfo.push_back(setInfo(u_front_side, v_front_side, 4, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_front_side, v_front_side+1, 4, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_front_side+1, v_front_side+1, 4, m)); // add uvs
                           
                                setPoint(&vertex_data_low, &vertex_data_high, x, h - 2, z, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x, h - 2, z + 1, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x, h - 1, z + 1, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);

                                setPoint(&vertex_data_low, &vertex_data_high, x, h - 1, z + 1, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x, h - 1, z, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                setPoint(&vertex_data_low, &vertex_data_high, x, h - 2, z, chunk_x, chunk_z);
                                this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                           


                        }

                        // check x+1 z h-2
                        addTriangles = false;
                        if (x == CHUNK_SIZE - 1) {
                            addTriangles = true;
                            
                            Chunk* target_chunk = world_chunk_map[WORLD_CHUNKS_SIZE / 2 + index_x+1][WORLD_CHUNKS_SIZE / 2 + index_z];
                            if (target_chunk != NULL) {

                                if (target_chunk->index_x == index_x + 1) {
                                    addTriangles = target_chunk->chunkArray[z][0][h - 2] == air; //if i wana add water i put here alst the id of water 0:air
                                    addTriangles = addTriangles || target_chunk->chunkArray[z][0][h - 2] == water_overlay;

                                }
                            }
                            else {
                                addTriangles = true;
                            }
                            /*
                            for (int i = 0; i < Chunk::instances.size(); i++) {
                                if (Chunk::instances[i]->index_x == chunk_x + 1 && Chunk::instances[i]->index_z == chunk_z) {
                                    if (Chunk::instances[i]->chunkArray[z][0][h - 2] == 1) {
                                        addTriangles = false;
                                    }
                                    break;
                                }
                            }
                            //*/
                        }
                        else if (this->chunkArray[z][x + 1][h - 2] == air || this->chunkArray[z][x+1][h - 2] == water_overlay) {
                            addTriangles = true;
                        }
                        if (addTriangles) {
                            //add 2 triangles to trianglesToAir

                            // triangle 1
                            // p1 = {x_end,h+1,z_end}
                            // p2 = {x_end,h,z_end}
                            // p3 = {x_end,h,z_start}
                            /*
                            this->verticiesMesh.push_back(vec3(x_end, (h - 1) * BLOCK_SIZE_Y, z_end));
                            this->verticiesMesh.push_back(vec3(x_end, (h - 2) * BLOCK_SIZE_Y, z_end));
                            this->verticiesMesh.push_back(vec3(x_end, (h - 2) * BLOCK_SIZE_Y, z_start));
                            */
                            // triangle 2
                            // p3 = {x_end,h,z_start}
                            // p2 = {x_end,h+1,z_start}
                            // p1 = {x_end,h+1,z_end}
                            /*
                            this->verticiesMesh.push_back(vec3(x_end, (h - 2) * BLOCK_SIZE_Y, z_start));
                            this->verticiesMesh.push_back(vec3(x_end, (h - 1) * BLOCK_SIZE_Y, z_start));
                            this->verticiesMesh.push_back(vec3(x_end, (h - 1) * BLOCK_SIZE_Y, z_end));
                            */


                            this->verticiesInfo.push_back(setInfo(u_back_side, v_back_side+1, 5, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_back_side+1, v_back_side + 1, 5, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_back_side+1, v_back_side, 5, m)); // add uvs

                            this->verticiesInfo.push_back(setInfo(u_back_side + 1, v_back_side, 5, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_back_side, v_back_side, 5, m)); // add uvs
                            this->verticiesInfo.push_back(setInfo(u_back_side, v_back_side + 1, 5, m)); // add uvs
                                                                  

                                        setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 1, z + 1, chunk_x, chunk_z);
                                        this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                        setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 2, z + 1, chunk_x, chunk_z);
                                        this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                        setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 2, z, chunk_x, chunk_z);
                                        this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);

                                        setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 2, z, chunk_x, chunk_z);
                                        this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                        setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 1, z, chunk_x, chunk_z);
                                        this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);
                                        setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 1, z + 1, chunk_x, chunk_z);
                                        this->verticies_mesh_data_low.push_back(vertex_data_low); this->verticies_mesh_data_high.push_back(vertex_data_high);


                        }

                    }
                    else if (this->chunkArray[z][x][h - 2] == water_overlay) {

                    int vertex_data_low, vertex_data_high;

                    int u_right_side, v_right_side;
                    int u_left_side, v_left_side;
                    int u_front_side, v_front_side;
                    int u_back_side, v_back_side;
                    int u_top_side, v_top_side;
                    int u_bottom_side, v_bottom_side;

                    material m = this->chunkArray[z][x][h - 2];

                    materialToUVS(m, &u_right_side, &v_right_side, &u_left_side, &v_left_side, &u_front_side, &v_front_side, &u_back_side, &v_back_side, &u_top_side, &v_top_side, &u_bottom_side, &v_bottom_side);

                    bool addTriangles = false;
                    if (h - 1 == CHUNK_HEIGHT) {
                        addTriangles = true;
                    }
                    else if (this->chunkArray[z][x][h - 1] == air) {
                        addTriangles = true;
                    }

                    if (addTriangles) {

                        /*
                        this->verticiesInfo_water.push_back(setInfo(u_top_side, v_top_side, 1)); // add uvs
                        this->verticiesInfo_water.push_back(setInfo(u_top_side + 1, v_top_side + 1, 1)); // add uvs
                        this->verticiesInfo_water.push_back(setInfo(u_top_side + 1, v_top_side, 1)); // add uvs
                        this->verticiesInfo_water.push_back(setInfo(u_top_side, v_top_side, 1)); // add uvs  this is ok position middle
                        this->verticiesInfo_water.push_back(setInfo(u_top_side, v_top_side + 1, 1)); // add uvs
                        this->verticiesInfo_water.push_back(setInfo(u_top_side + 1, v_top_side + 1, 1)); // add uvs



                        setPoint(&vertex_data_low, &vertex_data_high, x, h - 1, z, chunk_x, chunk_z);
                        this->verticies_mesh_data_low_water.push_back(vertex_data_low); this->verticies_mesh_data_high_water.push_back(vertex_data_high);
                        setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 1, z + 1, chunk_x, chunk_z);
                        this->verticies_mesh_data_low_water.push_back(vertex_data_low); this->verticies_mesh_data_high_water.push_back(vertex_data_high);
                        setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 1, z, chunk_x, chunk_z);
                        this->verticies_mesh_data_low_water.push_back(vertex_data_low); this->verticies_mesh_data_high_water.push_back(vertex_data_high);

                        setPoint(&vertex_data_low, &vertex_data_high, x, h - 1, z, chunk_x, chunk_z);
                        this->verticies_mesh_data_low_water.push_back(vertex_data_low); this->verticies_mesh_data_high_water.push_back(vertex_data_high);
                        setPoint(&vertex_data_low, &vertex_data_high, x, h - 1, z + 1, chunk_x, chunk_z);
                        this->verticies_mesh_data_low_water.push_back(vertex_data_low); this->verticies_mesh_data_high_water.push_back(vertex_data_high);
                        setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 1, z + 1, chunk_x, chunk_z);
                        this->verticies_mesh_data_low_water.push_back(vertex_data_low); this->verticies_mesh_data_high_water.push_back(vertex_data_high);
                        */
                        this->verticiesInfo_water.push_back(setInfo(u_top_side, v_top_side, 1, m)); // add uvs
                        this->verticiesInfo_water.push_back(setInfo(u_top_side + 1, v_top_side + 1, 1, m)); // add uvs
                        this->verticiesInfo_water.push_back(setInfo(u_top_side + 1, v_top_side, 1, m)); // add uvs
                        this->verticiesInfo_water.push_back(setInfo(u_top_side, v_top_side, 1, m)); // add uvs  this is ok position middle
                        this->verticiesInfo_water.push_back(setInfo(u_top_side, v_top_side + 1, 1, m)); // add uvs
                        this->verticiesInfo_water.push_back(setInfo(u_top_side + 1, v_top_side + 1, 1, m)); // add uvs



                        setPoint(&vertex_data_low, &vertex_data_high, x, h - 1, z, chunk_x, chunk_z);
                        this->verticies_mesh_data_low_water.push_back(vertex_data_low); this->verticies_mesh_data_high_water.push_back(vertex_data_high);
                        setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 1, z + 1, chunk_x, chunk_z);
                        this->verticies_mesh_data_low_water.push_back(vertex_data_low); this->verticies_mesh_data_high_water.push_back(vertex_data_high);
                        setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 1, z, chunk_x, chunk_z);
                        this->verticies_mesh_data_low_water.push_back(vertex_data_low); this->verticies_mesh_data_high_water.push_back(vertex_data_high);

                        setPoint(&vertex_data_low, &vertex_data_high, x, h - 1, z, chunk_x, chunk_z);
                        this->verticies_mesh_data_low_water.push_back(vertex_data_low); this->verticies_mesh_data_high_water.push_back(vertex_data_high);
                        setPoint(&vertex_data_low, &vertex_data_high, x, h - 1, z + 1, chunk_x, chunk_z);
                        this->verticies_mesh_data_low_water.push_back(vertex_data_low); this->verticies_mesh_data_high_water.push_back(vertex_data_high);
                        setPoint(&vertex_data_low, &vertex_data_high, x + 1, h - 1, z + 1, chunk_x, chunk_z);
                        this->verticies_mesh_data_low_water.push_back(vertex_data_low); this->verticies_mesh_data_high_water.push_back(vertex_data_high);

                    }
                    }
                }
            }

        }
    }
    // std::cout << "ddddddddddddddddd: " << this->verticiesMesh.size() << std::endl;

    //cout << glfwGetTime() -start << endl;
    if (bindflag) {
        //cout << "...." << endl;
    this->bind();
    this->bindWater();
    }
};

void Chunk::getMesh(std::vector<int>& vertices_data_low, std::vector<int>& vertices_data_high, std::vector<int>& verticiesInfo) {
    
    /*
    for (int j = 0; j < this->verticies_mesh_data_low.size(); j++) {
        vertices_data_low.push_back(this->verticies_mesh_data_low[j]);
        vertices_data_high.push_back(this->verticies_mesh_data_high[j]);
        verticiesInfo.push_back(this->verticiesInfo[j]);
    }
    */
    vertices_data_low.insert(vertices_data_low.end(), this->verticies_mesh_data_low.begin(), this->verticies_mesh_data_low.end());
    vertices_data_high.insert(vertices_data_high.end(), this->verticies_mesh_data_high.begin(), this->verticies_mesh_data_high.end());
    verticiesInfo.insert(verticiesInfo.end(), this->verticiesInfo.begin(), this->verticiesInfo.end());
};

void Chunk::getMeshWater(std::vector<int>& vertices_data_low, std::vector<int>& vertices_data_high, std::vector<int>& verticiesInfo) {

    /*
    for (int j = 0; j < this->verticies_mesh_data_low.size(); j++) {
        vertices_data_low.push_back(this->verticies_mesh_data_low[j]);
        vertices_data_high.push_back(this->verticies_mesh_data_high[j]);
        verticiesInfo.push_back(this->verticiesInfo[j]);
    }
    */
    vertices_data_low.insert(vertices_data_low.end(), this->verticies_mesh_data_low_water.begin(), this->verticies_mesh_data_low_water.end());
    vertices_data_high.insert(vertices_data_high.end(), this->verticies_mesh_data_high_water.begin(), this->verticies_mesh_data_high_water.end());
    verticiesInfo.insert(verticiesInfo.end(), this->verticiesInfo_water.begin(), this->verticiesInfo_water.end());
};


void Chunk::genTotalMesh(std::vector <Chunk*>& instances) {
    for (int i = 0; i < instances.size(); i++) {
        instances[i]->genMesh();
    }
};

void Chunk::getTotalMeshWater(std::vector <Chunk*>& instances, std::vector<int>& vertices_data_low, std::vector<int>& vertices_data_high, std::vector<int>& verticiesInfo)
{
    int sum = 0;
    for (int i = 0; i < instances.size(); i++) {
        sum += (int)instances[i]->verticies_mesh_data_high_water.size();
    }
    //vertices_data_low.reserve(sum);
    //vertices_data_high.reserve(sum);
    //verticiesInfo.reserve(sum);

    for (int i = 0; i < instances.size(); i++) {
        instances[i]->getMeshWater(vertices_data_low, vertices_data_high, verticiesInfo);
    }

};


void Chunk::getTotalMesh(std::vector <Chunk*>& instances, std::vector<int>& vertices_data_low, std::vector<int>& vertices_data_high, std::vector<int>& verticiesInfo)
{
    int sum = 0;
    for (int i = 0; i < instances.size(); i++) {
        sum += (int)instances[i]->verticies_mesh_data_high.size();
    }
    //vertices_data_low.reserve(sum);
    //vertices_data_high.reserve(sum);
    //verticiesInfo.reserve(sum);
    
    for (int i = 0; i < instances.size(); i++) {
        instances[i]->getMesh(vertices_data_low, vertices_data_high, verticiesInfo);
    }

};

//////////////////////////////////////////////////////////////

int sign(int x) {
    if (x >= 0) {
        return 1;
    }
    return -1;
}

int dist(int t, int r, int x) {
    return t - 4 * r * r + 4 * r - 2 * r * x;
}

void dims(int t, int* x, int* z) {
    int r = (int)(sqrt(t) + 1) / 2;
    if (r == 0) {
        *x = 0;
        *z = 0;
        return;
    }

    int p = (int)(t - (2 * r - 1) * (2 * r - 1)) / (2 * r);


    if (p == 0) {
        *x = -r + dist(t, r, p);
        *z = r;
    }
    else if (p == 1) {
        *x = r;
        *z = r - dist(t, r, p);
    }
    else if (p == 2) {
        *x = r - dist(t, r, p);
        *z = -r;
    }
    else if (p == 3) {
        *x = -r;
        *z = -r + dist(t, r, p);
    }
}
void invDim(int x, int z, int* t) {
    if (x == 0 && z == 0) {
        *t = 0;
        return;
    }

    *t = abs(4 * x * x - 2 * x + sign(x) * x - sign(x) * z);
    int tmp_x = 0 , tmp_z=0;
    dims(*t, &tmp_x, &tmp_z);
    if (x == tmp_x && z == tmp_z) {
        return;
    }
    else {
        *t = sign(z) * x + 4 * z * z - 2 * z - sign(z) * z;
    }
}

void Chunk::bind(void) {
    // VAO
    glGenVertexArrays(1, &this->meshVAO);
    glBindVertexArray(this->meshVAO);

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
    
};


void Chunk::bindWater(void) {
    if (this->verticies_mesh_data_low_water.size() == 0) return;

    // VAO
    glGenVertexArrays(1, &this->meshVAO_water);
    glBindVertexArray(this->meshVAO_water);

    // vertex VBO
    glGenBuffers(1, &this->meshVerticiesVBO_water);
    glBindBuffer(GL_ARRAY_BUFFER, this->meshVerticiesVBO_water);
    glBufferData(GL_ARRAY_BUFFER, this->verticies_mesh_data_low_water.size() * sizeof(int), &this->verticies_mesh_data_low_water[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(int), NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &this->meshVerticiesVBO_water);
    glBindBuffer(GL_ARRAY_BUFFER, this->meshVerticiesVBO_water);
    glBufferData(GL_ARRAY_BUFFER, this->verticies_mesh_data_high_water.size() * sizeof(int), &this->verticies_mesh_data_high_water[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(int), NULL);
    glEnableVertexAttribArray(1);


    // uvs VBO
    glGenBuffers(1, &this->meshInfoVBO_water);
    glBindBuffer(GL_ARRAY_BUFFER, this->meshInfoVBO_water);
    glBufferData(GL_ARRAY_BUFFER, this->verticiesInfo_water.size() * sizeof(int), &this->verticiesInfo_water[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);

};


void Chunk::draw(void) {
    auto start = glfwGetTime();
    //glDrawArrays(GL_TRIANGLES, 0, (GLsizei)this->verticiesInfo.size());
    //glDrawElements(GL_TRIANGLES, (GLsizei)this->verticiesInfo.size(), GL_UNSIGNED_INT, NULL);
    
    glBindVertexArray(this->meshVAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)this->verticiesInfo.size());
   // cout << glfwGetTime() - start << endl;
}

void Chunk::drawWater(void) {
    auto start = glfwGetTime();
    //glDrawArrays(GL_TRIANGLES, 0, (GLsizei)this->verticiesInfo.size());
    //glDrawElements(GL_TRIANGLES, (GLsizei)this->verticiesInfo.size(), GL_UNSIGNED_INT, NULL);

    glBindVertexArray(this->meshVAO_water);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)this->verticiesInfo_water.size());
    // cout << glfwGetTime() - start << endl;
}


// texture encription
// side normals: 0: (0,-1,0), 1: (0,1,0), 2: (0,0,1), 3: (0,0,-1), 4: (-1,0,0), 5: (1,0,0),  
float getInfo(int UV, int choice) { //choice: 0=u , 1=v , 2=side normals witch is int from 0 to 5 , 3: material
    switch (choice) {
    case 0: return (float)(((UV >> 5) & 0x1F) / 16.0);
    case 1: return (float)((UV & 0x1F) / 16.0);
    case 2: return (float)(((UV>>10) & 0xF));
    case 3: return (float)(((UV >> 14) & 0xFF));
    default: return -1;
    }
}

int setInfo(int u, int v,int sideNormal,material m) { // u: int index of column , v: int index of row , sideNormal = {0,1,2,3,4,5} sides of the normals in a cube 6 faces
    return (int)(((u & 0x1F)) << 5) | ((v & 0x1F)) | ((sideNormal & 0xF)<<10) | ((m & 0xFF) << 14);
}

// side normals: 0: (0,-1,0), 1: (0,1,0), 2: (0,0,1), 3: (0,0,-1), 4: (-1,0,0), 5: (1,0,0),  



