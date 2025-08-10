#ifndef CORNER_STITCHING_H
#define CORNER_STITCHING_H

#include "Tile.h"
#include <vector>
#include <unordered_map>
#include <map>
#include <stack>
#include <fstream>
#include <set>
#include <utility>

using LEFT_TILE = int;
using RIGHT_TILE = int;
using UP_TILE = int;
using DOWN_TILE = int;
using TILE_NAME = int;
using MOVEMENT = int;

class Corner_Stitching{
public: 

    //data member
    unordered_map <int, Tile> tile;
    set <int> block_tile_sort;
    vector <coordinate> point_finding_result;

    //member function
    void parse_input(string input_file);
    void parser_output(string output_file);
    pair<bool, int> point_finding(int tile_name, coordinate target);
    pair<int, int>  neighbor_finding(int tile_name);
    void tile_creation(int block_index, int x, int y, int width, int height);

    void print_all_tile(string test_file);

    // void find_overlapping_tiles(int tile_name, int x, int y, int width, int height);
    // void find_overlapping_tiles_recursive(int tile_name, int next_tile_name, int x, int y, int width, int height);


private:
    //data member
    int outline_width;
    int outline_height;
    int vacant_tile_counter = -2;
    int bottom_right_tile = -1;

    int insert_top = 0;
    int insert_down = 0;
    int insert_left = 0;
    int insert_right = 0;

    //member function
    pair<int, int> find_top_neighbor(int tile_name);
    pair<int, int> find_down_neighbor(int tile_name);
    pair<int, int> find_left_neighbor(int tile_name);
    pair<int, int> find_right_neighbor(int tile_name);

    map<int, int> find_tile_contain_horizontal_edge(int block_index, int tile_name, int x_min, int x_max, int target_y, bool is_top);
    vector <map<int, int>> find_tile_contain_vertical_edge(int tile_name, int y_min, int y_max, int target_xmin, int target_xmax);

    int horizontal_split(int block_name, int tile_name, int target_y, bool is_top, int x_min, int x_max);
    int vertical_split(int tile_name, int target_x);
    void vertical_merge(int block_index, map<int, int> tiles, bool is_left, int y_min, int y_max);
    void horizontal_merge(map<int, int> tiles);

};


#endif