//#include <iostream>
#include "tree.h"
#include "GlobalVariables.h"
#include "./chunk.h"
#include <iostream>

using namespace std;

Tree::Tree(int x, int y, int z, int chunk_x, int chunk_z, int type,int seed=16) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->chunk_x = chunk_x;
	this->chunk_z = chunk_z;
	this->type = type;
	this->seed = seed;

	this->blocks.clear();

	this->genArray(this->seed);
};


void Tree::genArray(int seed) {
	// parameters baced on random
	this->height = 6;
	material type_wood = jungle_wood;
	material type_leaves = jungle_leaves;
	//
	
	for (int h = 0; h < this->height; h++) {
		Block tmp_block = Block(0,h,0, type_wood);
		this->blocks.push_back(tmp_block);
	}	

	for (int h =  - 2; h <  3; h++) {
		for (int x = -2; x < 3; x++) {
			for (int z = -2; z < 3; z++) {
				if (h < 0 && z == 0 && x == 0) continue;
				Block tmp_block = Block(x, this->height+ h, z, type_leaves);
				this->blocks.push_back(tmp_block);
		
			}
		}
	}

};


void Tree::addToChunkArray(Chunk *chunk) {
	for (int b = 0; b < this->blocks.size(); b++) {
		int dx = ((this->x + this->blocks[b].x < 0) ? -1 : 0) + (this->x + this->blocks[b].x) / CHUNK_SIZE;
		int dz = ((this->z + this->blocks[b].z < 0) ? -1 : 0) + (this->z + this->blocks[b].z) / CHUNK_SIZE;

		int newCooodr_x = -dx * CHUNK_SIZE + this->x + this->blocks[b].x;
		int newCooodr_z = -dz * CHUNK_SIZE + this->z + this->blocks[b].z;

		Chunk* target_chunk = Chunk::world_chunk_map[WORLD_CHUNKS_SIZE / 2 + chunk->index_x + dx][WORLD_CHUNKS_SIZE / 2 + chunk->index_z + dz];
		vector<Block*> *target_extra_blocks = &Chunk::world_chunk_extra_blocks[WORLD_CHUNKS_SIZE / 2 + chunk->index_x + dx][WORLD_CHUNKS_SIZE / 2 + chunk->index_z + dz];
			
		if (target_chunk == NULL) {
			Block* tmp = new Block(newCooodr_x, this->y + this->blocks[b].y, newCooodr_z, this->blocks[b].m);
			// CAUSE OF THIS PUSH PROBABLY THERE WILL BE MANY ITEMS IN SO IT WILL BE NEED TO HANDLED LATER MUCH LATER	
			target_extra_blocks->push_back(tmp);
			//cout << "Added bock to null chunk at "<< chunk->index_x + dx <<" "<< chunk->index_z + dz<<" - "<< chunk->index_x<< " " << chunk->index_z << " - " << dx<< " " << dz<<endl;
		}
		else{			
			target_chunk->chunkArray[newCooodr_z][newCooodr_x][this->y + this->blocks[b].y] = this->blocks[b].m;
		}
		
		
	}
};
