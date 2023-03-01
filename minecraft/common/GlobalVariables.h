#ifndef GLOBAL_CHH 
#define GLOBAL_CHH

#define CHUNK_SIZE 16 //dont change this cause the variables are fixed in memory size for this to work optimal
#define CHUNK_HEIGHT 256

// have to be same with vertex shader
#define BLOCK_SIZE_X 1
#define BLOCK_SIZE_Y 1
#define BLOCK_SIZE_Z 1


#define WORLD_CHUNKS_SIZE 1000 //16777216 //2^24
#define WATERLEVEL 70
#define SNOWLEVEL 100


#include <vector>

using namespace std;

const int g = 9.81;

enum material {
	air,
	destroy_stage_0,
	destroy_stage_1,
	destroy_stage_2,
	destroy_stage_3,
	destroy_stage_4,
	destroy_stage_5,
	destroy_stage_6,
	destroy_stage_7,
	destroy_stage_8,
	destroy_stage_9,
	bedrock,
	dirt,
	grass_block,
	stone,
	water_overlay,
	coal_ore,
	diamond_ore,
	emerald_ore,
	iron_ore,
	lapis_ore,
	gold_ore,
	redstone_ore,
	acacia_wood,
	acacia_planks,
	birch_wood,
	birch_planks,
	jungle_wood,
	jungle_planks,
	spruce_wood,
	spruce_planks,
	stripped_birch_wood,
	stripped_acacia_wood,
	stripped_dark_oak_wood,
	stripped_jungle_wood,
	stripped_mangrove_wood,
	stripped_spruce_wood,
	azalea_leaves,
	bamboo_large_leaves,
	jungle_leaves,
	mangrove_leaves,
	spruce_leaves,
	snow,
	mouseCursor

};


void materialToUVS(
	int m,
	int* u_rigth_side, int* v_rigth_side,
	int* u_left_side, int* v_left_side,
	int* u_front_side, int* v_front_side,
	int* u_back_side, int* v_back_side,
	int* u_top_side, int* v_top_side,
	int* u_bottom_side, int* v_bottom_side
);


struct Block {
	int x;
	int y;
	int z;
	material m;
	Block(int x, int y, int z, material m) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->m = m;
	}
};

#endif