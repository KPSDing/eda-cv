#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
using namespace std;

struct Tile {
    int x1, y1, x2, y2; 
    bool is_block;
    int block_index; 
};

struct Command {
    string type; 
    int x, y;
    int block_index, w, h;
    int bx, by; 
};

struct PointQueryResult {
    int x1, y1; 
};

int string_to_int(const string& s) {
    istringstream iss(s);
    int value;
    iss >> value;
    return value;
}

bool compare_tiles(const Tile& a, const Tile& b) {
    if (a.x1 != b.x1)
        return a.x1 < b.x1;
    if (a.x2 != b.x2)
        return a.x2 < b.x2;
    return a.y1 < b.y1;
}

int main(int argc, char* argv[]) {
    ifstream fin(argv[1]);
    ofstream fout(argv[2]);

    int outline_w, outline_h;
    fin >> outline_w >> outline_h;

    vector<Tile> tiles;
    Tile outline_tile = {0, 0, outline_w, outline_h, false, 0};
    tiles.push_back(outline_tile);

    vector<Command> commands;
    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        string token;
        iss >> token;
        if (token == "P") {
            int x, y;
            iss >> x >> y;
            Command cmd;
            cmd.type = "P";
            cmd.x = x;
            cmd.y = y;
            commands.push_back(cmd);
        } else {
            int block_index = string_to_int(token);
            int bx, by, w, h;
            iss >> bx >> by >> w >> h;
            Command cmd;
            cmd.type = "B";
            cmd.block_index = block_index;
            cmd.bx = bx;
            cmd.by = by;
            cmd.w = w;
            cmd.h = h;
            commands.push_back(cmd);
        }
    }

    vector<PointQueryResult> point_query_results;

    for (size_t idx = 0; idx < commands.size(); ++idx) {
        Command cmd = commands[idx];
        if (cmd.type == "P") {
            int x = cmd.x;
            int y = cmd.y;
            bool found = false;
            for (size_t i = 0; i < tiles.size(); ++i) {
                Tile tile = tiles[i];
                if (tile.x1 <= x && x < tile.x2 && tile.y1 <= y && y < tile.y2) {
                    PointQueryResult result;
                    result.x1 = tile.x1;
                    result.y1 = tile.y1;
                    point_query_results.push_back(result);
                    found = true;
                    break;
                }
            }
            if (!found) {
                PointQueryResult result;
                result.x1 = -1;
                result.y1 = -1;
                point_query_results.push_back(result);
            }
        } else if (cmd.type == "B") {
            int block_index = cmd.block_index;
            int bx1 = cmd.bx;
            int by1 = cmd.by;
            int bx2 = bx1 + cmd.w;
            int by2 = by1 + cmd.h;

            vector<Tile> new_tiles;

			for (size_t i = 0; i < tiles.size(); ++i) {
			    Tile tile = tiles[i];
			    if (tile.x2 <= bx1 || tile.x1 >= bx2 || tile.y2 <= by1 || tile.y1 >= by2) {
			        new_tiles.push_back(tile);
			    } else {
			    	if (tile.x1 < bx1) {
			            Tile left_tile = {tile.x1, max(by1,tile.y1), bx1, min(by2,tile.y2), tile.is_block, tile.block_index};
			            new_tiles.push_back(left_tile);
			        }
			        if (bx2 < tile.x2) {
			            Tile right_tile = {bx2, max(by1,tile.y1), tile.x2, min(by2,tile.y2), tile.is_block, tile.block_index};
			            new_tiles.push_back(right_tile);
			        }
			        if (tile.y1 < by1) {
			            Tile bottom_tile = {min(tile.x1, bx1), tile.y1, max(tile.x2, bx2), by1, tile.is_block, tile.block_index};
			            new_tiles.push_back(bottom_tile);
			        }
			        if (by2 < tile.y2) {
			            Tile top_tile = {min(tile.x1, bx1), by2, max(tile.x2, bx2), tile.y2, tile.is_block, tile.block_index};
			            new_tiles.push_back(top_tile);
			        }
			    }
			}
			
			Tile block_tile = {bx1, by1, bx2, by2, true, block_index};
			new_tiles.push_back(block_tile);
			tiles = new_tiles;

            vector<Tile> space_tiles;
            vector<Tile> block_tiles;
            for (size_t i = 0; i < tiles.size(); ++i) {
                Tile tile = tiles[i];
                if (!tile.is_block) {
                    space_tiles.push_back(tile);
                } else {
                    block_tiles.push_back(tile);
                }
            }

            sort(space_tiles.begin(), space_tiles.end(), compare_tiles);

            vector<Tile> merged_space_tiles;
            size_t i = 0;
            while (i < space_tiles.size()) {
                Tile current_tile = space_tiles[i];
                size_t j = i + 1;
                while (j < space_tiles.size()) {
                    Tile next_tile = space_tiles[j];
                    if (current_tile.x1 == next_tile.x1 && current_tile.x2 == next_tile.x2 && current_tile.y2 == next_tile.y1) {
                        current_tile.y2 = next_tile.y2;
                        j++;
                    } else {
                        break;
                    }
                }
                merged_space_tiles.push_back(current_tile);
                i = j;
            }

            tiles.clear();
            for (size_t k = 0; k < merged_space_tiles.size(); ++k) {
                tiles.push_back(merged_space_tiles[k]);
            }
            for (size_t k = 0; k < block_tiles.size(); ++k) {
                tiles.push_back(block_tiles[k]);
            }
            
        }
    }

    vector<int> block_indices;
    for (size_t i = 0; i < tiles.size(); ++i) {
        Tile tile = tiles[i];
        if (tile.is_block) {
            if (find(block_indices.begin(), block_indices.end(), tile.block_index) == block_indices.end()) {
                block_indices.push_back(tile.block_index);
            }
        }
    }
    sort(block_indices.begin(), block_indices.end());

    vector<int> block_adjacent_counts(block_indices.size(), 0);
    vector<int> space_adjacent_counts(block_indices.size(), 0);

    for (size_t i = 0; i < tiles.size(); ++i) {
        Tile tile_i = tiles[i];
        for (size_t j = i + 1; j < tiles.size(); ++j) {
            Tile tile_j = tiles[j];
            bool adjacent = false;
            if (tile_i.x2 == tile_j.x1 || tile_i.x1 == tile_j.x2) {
                if (!(tile_i.y2 <= tile_j.y1 || tile_i.y1 >= tile_j.y2)) {
                    adjacent = true;
                }
            }
            else if (tile_i.y2 == tile_j.y1 || tile_i.y1 == tile_j.y2) {
                if (!(tile_i.x2 <= tile_j.x1 || tile_i.x1 >= tile_j.x2)) {
                    adjacent = true;
                }
            }
            if (adjacent) {
                if (tile_i.is_block && !tile_j.is_block) {
                    int bi = find(block_indices.begin(), block_indices.end(), tile_i.block_index) - block_indices.begin();
                    space_adjacent_counts[bi]++;
                } else if (!tile_i.is_block && tile_j.is_block) {
                    int bi = find(block_indices.begin(), block_indices.end(), tile_j.block_index) - block_indices.begin();
                    space_adjacent_counts[bi]++;
                }
            }
        }
    }
	
    for (size_t i = 0; i < tiles.size(); ++i) {
        Tile tile_i = tiles[i];
        for (size_t j = 0; j < tiles.size(); ++j) {
            Tile tile_j = tiles[j];
            bool adjacent = false;
            if (tile_i.x2 == tile_j.x1 || tile_i.x1 == tile_j.x2) {
                if (!(tile_i.y2 <= tile_j.y1 || tile_i.y1 >= tile_j.y2)) {
                    adjacent = true;
                }
            }
            else if (tile_i.y2 == tile_j.y1 || tile_i.y1 == tile_j.y2) {
                if (!(tile_i.x2 <= tile_j.x1 || tile_i.x1 >= tile_j.x2)) {
                    adjacent = true;
                }
            }

            if (adjacent) {
                if (tile_i.is_block && tile_j.is_block) {
                    if (tile_i.block_index != tile_j.block_index) {
                        int bi = find(block_indices.begin(), block_indices.end(), tile_i.block_index) - block_indices.begin();
                        block_adjacent_counts[bi]++;
                    }
                } 
            }
        }
    }
    fout << tiles.size() << endl;

    for (size_t k = 0; k < block_indices.size(); ++k) {
        int block_index = block_indices[k];
        fout << block_index << " " << block_adjacent_counts[k] << " " << space_adjacent_counts[k] << endl;
    }

    size_t point_query_index = 0;
    for (size_t idx = 0; idx < commands.size(); ++idx) {
        if (commands[idx].type == "P") {
            PointQueryResult result = point_query_results[point_query_index++];
            fout << result.x1 << " " << result.y1 << endl;
        }
    }

    fin.close();
    fout.close();

    return 0;
}
