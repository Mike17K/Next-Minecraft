#ifndef TREE_CONSTANT
#define TREE_CONSTANT

#include <vector>
#include "./GlobalVariables.h"
#include "./chunk.h"

using namespace std;

class Tree
{
public:
	int x, y, z;
	int chunk_x, chunk_z;
	int type, seed;
	int height;
	vector<Block> blocks;

	Tree(int x, int y, int z, int chunk_x, int chunk_z, int type, int seed);

	void Tree::addToChunkArray(Chunk* chunk);
	void genArray(int seed);
private:

};

#endif