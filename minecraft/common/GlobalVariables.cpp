#include "./GlobalVariables.h"

#include <iostream>
using namespace std;

void materialToUVS(
	int m,
	int* u_rigth_side, int* v_rigth_side,
	int* u_left_side, int* v_left_side,
	int* u_front_side, int* v_front_side,
	int* u_back_side, int* v_back_side,
	int* u_top_side, int* v_top_side,
	int* u_bottom_side, int* v_bottom_side

) {
	switch (m) {
		case destroy_stage_0: *u_rigth_side=0; *v_rigth_side=0; *u_left_side=0; *v_left_side=0; *u_front_side=0; *v_front_side=0; *u_back_side=0; *v_back_side=0; *u_top_side=0; *v_top_side=0; *u_bottom_side=0; *v_bottom_side=0; break;
		case destroy_stage_1: *u_rigth_side = 0; *v_rigth_side = 1; *u_left_side = 0; *v_left_side = 1; *u_front_side = 0; *v_front_side = 1; *u_back_side = 0; *v_back_side = 1; *u_top_side = 0; *v_top_side = 1; *u_bottom_side = 0; *v_bottom_side = 1; break;
		case destroy_stage_2: *u_rigth_side = 0; *v_rigth_side = 2; *u_left_side = 0; *v_left_side = 2; *u_front_side = 0; *v_front_side = 2; *u_back_side = 0; *v_back_side = 2; *u_top_side = 0; *v_top_side = 2; *u_bottom_side = 0; *v_bottom_side = 2; break;
		case destroy_stage_3: *u_rigth_side = 0; *v_rigth_side = 3; *u_left_side = 0; *v_left_side = 3; *u_front_side = 0; *v_front_side = 3; *u_back_side = 0; *v_back_side = 3; *u_top_side = 0; *v_top_side = 3; *u_bottom_side = 0; *v_bottom_side = 3; break;
		case destroy_stage_4: *u_rigth_side = 0; *v_rigth_side = 4; *u_left_side = 0; *v_left_side = 4; *u_front_side = 0; *v_front_side = 4; *u_back_side = 0; *v_back_side = 4; *u_top_side = 0; *v_top_side = 4; *u_bottom_side = 0; *v_bottom_side = 4; break;
		case destroy_stage_5: *u_rigth_side = 0; *v_rigth_side = 5; *u_left_side = 0; *v_left_side = 5; *u_front_side = 0; *v_front_side = 5; *u_back_side = 0; *v_back_side = 5; *u_top_side = 0; *v_top_side = 5; *u_bottom_side = 0; *v_bottom_side = 5; break;
		case destroy_stage_6: *u_rigth_side = 0; *v_rigth_side = 6; *u_left_side = 0; *v_left_side = 6; *u_front_side = 0; *v_front_side = 6; *u_back_side = 0; *v_back_side = 6; *u_top_side = 0; *v_top_side = 6; *u_bottom_side = 0; *v_bottom_side = 6; break;
		case destroy_stage_7: *u_rigth_side = 0; *v_rigth_side = 7; *u_left_side = 0; *v_left_side = 7; *u_front_side = 0; *v_front_side = 7; *u_back_side = 0; *v_back_side = 7; *u_top_side = 0; *v_top_side = 7; *u_bottom_side = 0; *v_bottom_side = 7; break;
		case destroy_stage_8: *u_rigth_side = 0; *v_rigth_side = 8; *u_left_side = 0; *v_left_side = 8; *u_front_side = 0; *v_front_side = 8; *u_back_side = 0; *v_back_side = 8; *u_top_side = 0; *v_top_side = 8; *u_bottom_side = 0; *v_bottom_side = 8; break;
		case destroy_stage_9: *u_rigth_side = 0; *v_rigth_side = 9; *u_left_side = 0; *v_left_side = 9; *u_front_side = 0; *v_front_side = 9; *u_back_side = 0; *v_back_side = 9; *u_top_side = 0; *v_top_side = 9; *u_bottom_side = 0; *v_bottom_side = 9; break;
		case bedrock: *u_rigth_side = 0; *v_rigth_side = 10; *u_left_side = 0; *v_left_side = 10; *u_front_side = 0; *v_front_side = 10; *u_back_side = 0; *v_back_side = 10; *u_top_side = 0; *v_top_side = 10; *u_bottom_side = 0; *v_bottom_side = 10; break;
		case dirt:  *u_rigth_side = 0; *v_rigth_side = 11; *u_left_side = 0; *v_left_side = 11; *u_front_side = 0; *v_front_side = 11; *u_back_side = 0; *v_back_side = 11; *u_top_side = 0; *v_top_side = 11; *u_bottom_side = 0; *v_bottom_side = 11; break;
		case grass_block:  *u_rigth_side = 0; *v_rigth_side = 12; *u_left_side = 0; *v_left_side = 12; *u_front_side = 0; *v_front_side = 12; *u_back_side = 0; *v_back_side = 12; *u_top_side = 0; *v_top_side = 13; *u_bottom_side = 0; *v_bottom_side = 11; break;
		case stone:  *u_rigth_side = 0; *v_rigth_side = 14; *u_left_side = 0; *v_left_side = 14; *u_front_side = 0; *v_front_side = 14; *u_back_side = 0; *v_back_side = 14; *u_top_side = 0; *v_top_side = 14; *u_bottom_side = 0; *v_bottom_side = 14; break;
		case water_overlay:  *u_rigth_side = 0; *v_rigth_side = 15; *u_left_side = 0; *v_left_side = 15; *u_front_side = 0; *v_front_side = 15; *u_back_side = 0; *v_back_side = 15; *u_top_side = 0; *v_top_side = 15; *u_bottom_side = 0; *v_bottom_side = 15; break;
		case coal_ore: *u_rigth_side = 1; *v_rigth_side = 0; *u_left_side = 1; *v_left_side = 0; *u_front_side = 1; *v_front_side = 0; *u_back_side = 1; *v_back_side = 0; *u_top_side = 1; *v_top_side = 0; *u_bottom_side = 1; *v_bottom_side = 0; break;
		case diamond_ore: *u_rigth_side = 1; *v_rigth_side = 1; *u_left_side = 1; *v_left_side = 1; *u_front_side = 1; *v_front_side = 1; *u_back_side = 1; *v_back_side = 1; *u_top_side = 1; *v_top_side = 1; *u_bottom_side = 1; *v_bottom_side = 1; break;
		case emerald_ore: *u_rigth_side = 1; *v_rigth_side = 2; *u_left_side = 1; *v_left_side = 2; *u_front_side = 1; *v_front_side = 2; *u_back_side = 1; *v_back_side = 2; *u_top_side = 1; *v_top_side = 2; *u_bottom_side = 1; *v_bottom_side = 2; break;
		case iron_ore: *u_rigth_side = 1; *v_rigth_side = 3; *u_left_side = 1; *v_left_side = 3; *u_front_side = 1; *v_front_side = 3; *u_back_side = 1; *v_back_side = 3; *u_top_side = 1; *v_top_side = 3; *u_bottom_side = 1; *v_bottom_side = 3; break;
		case lapis_ore: *u_rigth_side = 1; *v_rigth_side = 4; *u_left_side = 1; *v_left_side = 4; *u_front_side = 1; *v_front_side = 4; *u_back_side = 1; *v_back_side = 4; *u_top_side = 1; *v_top_side = 4; *u_bottom_side = 1; *v_bottom_side = 4; break;
		case gold_ore: *u_rigth_side = 1; *v_rigth_side = 5; *u_left_side = 1; *v_left_side = 5; *u_front_side = 1; *v_front_side = 5; *u_back_side = 1; *v_back_side = 5; *u_top_side = 1; *v_top_side = 5; *u_bottom_side = 1; *v_bottom_side = 5; break;
		case redstone_ore: *u_rigth_side = 1; *v_rigth_side = 6; *u_left_side = 1; *v_left_side = 6; *u_front_side = 1; *v_front_side = 6; *u_back_side = 1; *v_back_side = 6; *u_top_side = 1; *v_top_side = 6; *u_bottom_side = 1; *v_bottom_side = 6; break;
		case acacia_wood: *u_rigth_side = 1; *v_rigth_side = 7; *u_left_side = 1; *v_left_side = 7; *u_front_side = 1; *v_front_side = 7; *u_back_side = 1; *v_back_side = 7; *u_top_side = 1; *v_top_side = 8; *u_bottom_side = 1; *v_bottom_side = 8; break;
		case acacia_planks: *u_rigth_side = 1; *v_rigth_side = 9; *u_left_side = 1; *v_left_side = 9; *u_front_side = 1; *v_front_side =9; *u_back_side = 1; *v_back_side = 9; *u_top_side = 1; *v_top_side = 9; *u_bottom_side = 1; *v_bottom_side = 9; break;
		case birch_wood: *u_rigth_side = 1; *v_rigth_side = 10; *u_left_side = 1; *v_left_side = 10; *u_front_side = 1; *v_front_side = 10; *u_back_side = 1; *v_back_side = 10; *u_top_side = 1; *v_top_side = 11; *u_bottom_side = 1; *v_bottom_side = 11; break;
		case birch_planks: *u_rigth_side = 1; *v_rigth_side = 12; *u_left_side = 1; *v_left_side = 12; *u_front_side = 1; *v_front_side = 12; *u_back_side = 1; *v_back_side = 12; *u_top_side = 1; *v_top_side = 12; *u_bottom_side = 1; *v_bottom_side = 12; break;
		case jungle_wood: *u_rigth_side = 1; *v_rigth_side = 13; *u_left_side = 1; *v_left_side = 13; *u_front_side = 1; *v_front_side = 13; *u_back_side = 1; *v_back_side = 13; *u_top_side = 1; *v_top_side = 14; *u_bottom_side = 1; *v_bottom_side = 14; break;
		case jungle_planks: *u_rigth_side = 1; *v_rigth_side = 15; *u_left_side = 1; *v_left_side = 15; *u_front_side = 1; *v_front_side = 15; *u_back_side = 1; *v_back_side = 15; *u_top_side = 1; *v_top_side = 15; *u_bottom_side = 1; *v_bottom_side = 15; break;
		case spruce_wood: *u_rigth_side = 2; *v_rigth_side = 0; *u_left_side = 2; *v_left_side = 0; *u_front_side = 2; *v_front_side = 0; *u_back_side = 2; *v_back_side = 0; *u_top_side = 2; *v_top_side = 1; *u_bottom_side = 2; *v_bottom_side = 1; break;
		case spruce_planks:*u_rigth_side = 2; *v_rigth_side = 2; *u_left_side = 2; *v_left_side = 2; *u_front_side = 2; *v_front_side = 2; *u_back_side = 2; *v_back_side = 2; *u_top_side = 2; *v_top_side = 2; *u_bottom_side = 2; *v_bottom_side = 2; break;
		case stripped_birch_wood: *u_rigth_side = 2; *v_rigth_side = 3; *u_left_side = 2; *v_left_side = 3; *u_front_side = 2; *v_front_side = 3; *u_back_side = 2; *v_back_side = 3; *u_top_side = 2; *v_top_side = 4; *u_bottom_side = 2; *v_bottom_side = 4;		 break;
		case stripped_acacia_wood: *u_rigth_side = 2; *v_rigth_side = 5; *u_left_side = 2; *v_left_side = 5; *u_front_side = 2; *v_front_side = 5; *u_back_side = 2; *v_back_side = 5; *u_top_side = 2; *v_top_side = 6; *u_bottom_side = 2; *v_bottom_side = 6;		 break;
		case stripped_jungle_wood: *u_rigth_side = 2; *v_rigth_side = 7; *u_left_side = 2; *v_left_side = 7; *u_front_side = 2; *v_front_side = 7; *u_back_side = 2; *v_back_side = 7; *u_top_side = 2; *v_top_side = 8; *u_bottom_side = 2; *v_bottom_side = 8; break;
		case stripped_mangrove_wood: *u_rigth_side = 2; *v_rigth_side = 9; *u_left_side = 2; *v_left_side = 9; *u_front_side = 2; *v_front_side = 9; *u_back_side = 2; *v_back_side = 9; *u_top_side = 2; *v_top_side = 10; *u_bottom_side = 2; *v_bottom_side = 10; break;
		case stripped_dark_oak_wood:*u_rigth_side = 2; *v_rigth_side = 11; *u_left_side = 2; *v_left_side = 11; *u_front_side = 2; *v_front_side = 11; *u_back_side = 2; *v_back_side = 11; *u_top_side = 2; *v_top_side = 12; *u_bottom_side = 2; *v_bottom_side = 12; break;
		case stripped_spruce_wood: *u_rigth_side = 2; *v_rigth_side = 13; *u_left_side = 2; *v_left_side = 13; *u_front_side = 2; *v_front_side = 13; *u_back_side = 2; *v_back_side = 13; *u_top_side = 2; *v_top_side = 14; *u_bottom_side = 2; *v_bottom_side = 14; break;
		case azalea_leaves: *u_rigth_side = 2; *v_rigth_side = 15; *u_left_side = 2; *v_left_side = 15; *u_front_side = 2; *v_front_side = 15; *u_back_side = 2; *v_back_side = 15; *u_top_side = 2; *v_top_side = 15; *u_bottom_side = 2; *v_bottom_side = 15; break;
		case bamboo_large_leaves: *u_rigth_side = 3; *v_rigth_side = 0; *u_left_side = 3; *v_left_side = 0; *u_front_side = 3; *v_front_side = 0; *u_back_side = 3; *v_back_side = 0; *u_top_side = 3; *v_top_side = 0; *u_bottom_side = 3; *v_bottom_side = 0; break;
		case jungle_leaves: *u_rigth_side = 3; *v_rigth_side = 1; *u_left_side = 3; *v_left_side = 1; *u_front_side = 3; *v_front_side = 1; *u_back_side = 3; *v_back_side = 1; *u_top_side = 3; *v_top_side = 1; *u_bottom_side = 3; *v_bottom_side = 1; break;
		case mangrove_leaves: *u_rigth_side = 3; *v_rigth_side = 2; *u_left_side = 3; *v_left_side = 2; *u_front_side = 3; *v_front_side = 2; *u_back_side = 3; *v_back_side = 2; *u_top_side = 3; *v_top_side = 2; *u_bottom_side = 3; *v_bottom_side = 2; break;
		case spruce_leaves: *u_rigth_side = 3; *v_rigth_side = 3; *u_left_side = 3; *v_left_side = 3; *u_front_side = 3; *v_front_side = 3; *u_back_side = 3; *v_back_side = 3; *u_top_side = 3; *v_top_side = 3; *u_bottom_side = 3; *v_bottom_side = 3; break;
		case snow: *u_rigth_side = 3; *v_rigth_side = 4; *u_left_side = 3; *v_left_side = 4; *u_front_side = 3; *v_front_side = 4; *u_back_side = 3; *v_back_side = 4; *u_top_side = 3; *v_top_side = 4; *u_bottom_side = 3; *v_bottom_side = 4; break;
		
		default: u_rigth_side = 0; v_rigth_side = 0; u_left_side = 0; v_left_side = 0; u_front_side = 0; v_front_side = 0; u_back_side = 0; v_back_side = 0; u_top_side = 0; v_top_side = 0; u_bottom_side = 0; v_bottom_side = 0; break;

	}
};


