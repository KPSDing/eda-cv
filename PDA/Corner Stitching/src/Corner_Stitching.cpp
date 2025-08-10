#include "Corner_Stitching.h"

void Corner_Stitching::parse_input(string input_file){
    ifstream get_input_file(input_file , ios::in);
    if(!get_input_file) cout << "cannot get input file";

    get_input_file >> outline_width >> outline_height;

    //initialize the outline tile
    coordinate outline_bottom_left = {0, 0};
    tile[-1] = Tile(VACANT, outline_bottom_left, outline_width, outline_height, OUTOFBOUNDARY, OUTOFBOUNDARY, OUTOFBOUNDARY, OUTOFBOUNDARY);

    cout << "bottom_right_tile: " << bottom_right_tile << endl;
    string command;
    while (get_input_file >> command) {
        if (command == "P") {
            coordinate target;
            get_input_file >> target.x >> target.y;
            auto result = point_finding(bottom_right_tile, target); // Assuming 0 is the starting tile
            point_finding_result.push_back(tile[result.second].bottom_left);
        } else {            
            int block_index, x, y, width, height;
            block_index = stoi(command);
            block_tile_sort.insert(block_index);
            get_input_file >> x >> y >> width >> height;
            tile_creation(block_index, x, y, width, height);
        }
        cout << "bottom_right_tile: " << bottom_right_tile << endl;
    }
    return ;
}

void Corner_Stitching::parser_output(string output_file){
    ofstream output(output_file, ios::out);
    if(!output) cout << "cannot output file";

    //total tile number
    output << tile.size() << endl;

    //block tile neighbor
    for(auto it : block_tile_sort){
        auto result = neighbor_finding(it);
        output << it << " " << result.first << " " << result.second << endl;
    }

    for(auto it : point_finding_result){
        output << it.x << " " << it.y << endl;
    }
    return ;
}

void Corner_Stitching::print_all_tile(string test_file){
    cout << "Total tile number: " << tile.size() << endl;
    cout << "Print all tile information: " << endl;
    for(auto it : tile){
        cout << "tile name: " << it.first << " \n";
        cout << "   x:" << it.second.bottom_left.x << " , y:" << it.second.bottom_left.y << " , w:" << it.second.w << " , h:" << it.second.h << " \n";
        cout << "   left:" << it.second.left << " , right:" << it.second.right << " , top:" << it.second.top << " , down:" << it.second.down << endl;
    }

    ofstream output(test_file, ios::out);

    output << tile.size() << endl;
    output << outline_width << " " << outline_height << endl;
    for(auto it : tile){
        output << it.first << " "<< it.second.bottom_left.x << " " << it.second.bottom_left.y << " " << it.second.w << " " << it.second.h << " \n";
    }
    return;
}

pair<bool, int> Corner_Stitching::point_finding(int tile_name, coordinate target){
    //cout << "Finding the target in tile " << tile_name << endl;
    pair<bool, int> result = {false, -1};
    //found the tile
    if(tile[tile_name].point_inside_tile(target) == true){
        //cout << "   1. The target is in tile " << tile_name << " at coordinate (" << target.x << ", " << target.y << ")" << endl;
        result = {true, tile_name};
        return result;
    }

    //not found the tile
    if(tile[tile_name].bottom_left.y > target.y){
        return result;
    }

    if(tile[tile_name].top != 0){
        //cout << "   2. Find the target in tile " << tile[tile_name].top << endl;
        int top_tile = tile[tile_name].top;
        result = point_finding(top_tile, target);
        if(result.first == true){
            return result;
        }
    }

    if(tile[tile_name].left != 0){
        //cout << "   3. Find the target in tile " << tile[tile_name].left << endl;
        int left_tile = tile[tile_name].left;
        result = point_finding(left_tile, target);
        if(result.first == true){
            return result;
        }
    }
    return result;
}

pair<int, int> Corner_Stitching::find_top_neighbor(int tile_name) {
    int block_neighbor = 0;
    int vacant_neighbor = 0;
    bool flag = false;
    int top_tile = 0;

    if (tile[tile_name].top != 0) {
        top_tile = tile[tile_name].top;

        do {
            if (tile[top_tile].bottom_left.x < tile[tile_name].bottom_left.x) {
                if (!flag) {
                    if (tile[top_tile].type == BLOCK) {
                        block_neighbor++;
                    } else {
                        vacant_neighbor++;
                    }
                } else if (tile[top_tile].bottom_left.x + tile[top_tile].w > tile[tile_name].bottom_left.x) {
                    if (tile[top_tile].type == BLOCK) {
                        block_neighbor++;
                    } else {
                        vacant_neighbor++;
                    }
                }
                break;
            }

            if (tile[top_tile].type == BLOCK) {
                block_neighbor++;
            } else {
                vacant_neighbor++;
            }

            if (tile[top_tile].bottom_left.x == tile[tile_name].bottom_left.x) {
                break;
            }

            top_tile = tile[top_tile].left;
            flag = true;
        } while (top_tile != 0);
    }

    return make_pair(block_neighbor, vacant_neighbor);
}

pair<int, int> Corner_Stitching::find_down_neighbor(int tile_name) {
    int block_neighbor = 0;
    int vacant_neighbor = 0;
    bool flag = false;
    int down_tile = 0;

    if (tile[tile_name].down != 0) {
        down_tile = tile[tile_name].down;

        do {
            if (tile[down_tile].bottom_left.x >= tile[tile_name].bottom_left.x + tile[tile_name].w) {
                if (!flag) {
                    if (tile[down_tile].type == BLOCK) {
                        block_neighbor++;
                    } else {
                        vacant_neighbor++;
                    }
                }
                break;
            }

            if (tile[down_tile].type == BLOCK) {
                block_neighbor++;
            } else {
                vacant_neighbor++;
            }

            down_tile = tile[down_tile].right;
            flag = true;
        } while (down_tile != 0);
    }

    return make_pair(block_neighbor, vacant_neighbor);
}

pair<int, int> Corner_Stitching::find_left_neighbor(int tile_name) {
    int block_neighbor = 0;
    int vacant_neighbor = 0;
    bool flag = false;
    int left_tile = 0;

    if (tile[tile_name].left != 0) {
        left_tile = tile[tile_name].left;

        do {
            if (tile[left_tile].bottom_left.y >= tile[tile_name].bottom_left.y + tile[tile_name].h) {
                if (!flag) {
                    if (tile[left_tile].type == BLOCK) {
                        block_neighbor++;
                    } else {
                        vacant_neighbor++;
                    }
                }
                break;
            }

            if (tile[left_tile].type == BLOCK) {
                block_neighbor++;
            } else {
                vacant_neighbor++;
            }

            left_tile = tile[left_tile].top;
            flag = true;
        } while (left_tile != 0);
    }

    return make_pair(block_neighbor, vacant_neighbor);
}

pair<int, int> Corner_Stitching::find_right_neighbor(int tile_name) {
    int block_neighbor = 0;
    int vacant_neighbor = 0;
    bool flag = false;
    int right_tile = 0;

    if (tile[tile_name].right != 0) {
        right_tile = tile[tile_name].right;

        do {
            if (tile[right_tile].bottom_left.y < tile[tile_name].bottom_left.y) {
                if (!flag) {
                    if (tile[right_tile].type == BLOCK) {
                        block_neighbor++;
                    } else {
                        vacant_neighbor++;
                    }
                }
                break;
            }

            if (tile[right_tile].type == BLOCK) {
                block_neighbor++;
            } else {
                vacant_neighbor++;
            }

            if (tile[right_tile].bottom_left.y == tile[tile_name].bottom_left.y) {
                break;
            }

            right_tile = tile[right_tile].down;
            flag = true;
        } while (right_tile != 0);
    }

    return make_pair(block_neighbor, vacant_neighbor);
}

pair<int, int> Corner_Stitching::neighbor_finding(int tile_name){
    int vacant_neighbor = 0;
    int block_neighbor = 0;
    
    auto top_neighbor = find_top_neighbor(tile_name);
    block_neighbor += top_neighbor.first;
    vacant_neighbor += top_neighbor.second;

    auto down_neighbor = find_down_neighbor(tile_name);
    block_neighbor += down_neighbor.first;
    vacant_neighbor += down_neighbor.second;

    auto left_neighbor = find_left_neighbor(tile_name);
    block_neighbor += left_neighbor.first;
    vacant_neighbor += left_neighbor.second;

    auto right_neighbor = find_right_neighbor(tile_name);
    block_neighbor += right_neighbor.first;
    vacant_neighbor += right_neighbor.second;

    return make_pair(block_neighbor, vacant_neighbor);
}

void Corner_Stitching::tile_creation(int block_index, int x, int y, int width, int height) {
    insert_top = 0;
    insert_down = 0;
    insert_left = 0;
    insert_right = 0;

    cout << "=================================================\n";
    cout << "tile_creation ========================= block index: "<< block_index << endl;

    // 左上角點
    coordinate left_top = {x, y + height};
    auto top_left_point_result = point_finding(bottom_right_tile, left_top);
    int top_left_tile_name = top_left_point_result.second;
    cout << "top_left_tile_name: " << top_left_tile_name << endl;
    map<int, int> top_edge_tile = find_tile_contain_horizontal_edge(block_index, top_left_tile_name, x, x + width, y + height, true);
    for(auto it : top_edge_tile){
        cout << "top_edge_tile: " << it.second << " , x: " << it.first << endl;
    }

    cout << "=================================================\n";
    cout << "after top_edge_tile spilt" << endl;
    for(auto it : tile){
        cout << "tile_name: " << it.first << " , x: " << it.second.bottom_left.x << " , y: " << it.second.bottom_left.y << " , w: " << it.second.w << " , h: " << it.second.h << endl;
        cout << "   left: " << it.second.left << " , right: " << it.second.right << " , top: " << it.second.top << " , down: " << it.second.down << endl;
    }

    // 左下角點
    coordinate left_bottom = {x, y};
    auto bottom_left_point_result = point_finding(bottom_right_tile, left_bottom);
    int bottom_left_tile_name = bottom_left_point_result.second;
    cout << "bottom_left_tile_name: " << bottom_left_tile_name << endl;
    map<int, int> bottom_edge_tile = find_tile_contain_horizontal_edge(block_index, bottom_left_tile_name, x, x + width, y, false);
    for(auto it : bottom_edge_tile){
        cout << "bottom_edge_tile: " << it.second << " , x: " << it.first << endl;
    }

    cout << "=================================================\n";
    cout << "after bottom_edge_tile spilt" << endl;
    for(auto it : tile){
        cout << "tile_name: " << it.first << " , x: " << it.second.bottom_left.x << " , y: " << it.second.bottom_left.y << " , w: " << it.second.w << " , h: " << it.second.h << endl;
        cout << "   left: " << it.second.left << " , right: " << it.second.right << " , top: " << it.second.top << " , down: " << it.second.down << endl;
    }
    
    cout << "=================================================\n";

    vector <map<int, int>> middle_edge_tile = find_tile_contain_vertical_edge(top_left_tile_name, y, y + height, x, x + width);

    cout << "=================================================\n";
    cout << "after vertical spilt" << endl;
    for(auto it : tile){
        cout << "tile_name: " << it.first << " , x: " << it.second.bottom_left.x << " , y: " << it.second.bottom_left.y << " , w: " << it.second.w << " , h: " << it.second.h << endl;
        cout << "   left: " << it.second.left << " , right: " << it.second.right << " , top: " << it.second.top << " , down: " << it.second.down << endl;
    }

    horizontal_merge(top_edge_tile);
    horizontal_merge(bottom_edge_tile);
    vertical_merge(block_index, middle_edge_tile[0], true, y, y + height);
    vertical_merge(block_index, middle_edge_tile[2], false, y, y + height);

    cout << "=================================================\n";
    tile[block_index] = Tile(1, {x, y}, width, height, insert_top, insert_down, insert_left, insert_right);
    for( auto it: middle_edge_tile[1]){
        tile.erase(it.second);
    }
    return;
}

map<int, int> Corner_Stitching::find_tile_contain_horizontal_edge(int block_index, int tile_name, int x_min, int x_max, int target_y, bool is_top){
    cout << "find_tile_contain_horizontal_edge" << endl;
    Tile target_tile = tile[tile_name];
    map <int, int> result;
    
    while(target_tile.bottom_left.x < x_max){
        if(is_top && target_tile.bottom_left.x <= x_max && target_tile.bottom_left.x + target_tile.w > x_max){
            insert_top = tile_name;
        }
        if(!is_top && target_tile.bottom_left.x < x_min && target_tile.bottom_left.x + target_tile.w >= x_min){
            insert_down = tile_name;
        }

        if(target_tile.bottom_left.x + target_tile.w > x_min && target_tile.bottom_left.y <= target_y && target_tile.bottom_left.y + target_tile.h > target_y){
            
            if(is_top){
                result[target_tile.bottom_left.x] = tile_name;
            }
            int new_tile_name = horizontal_split(block_index, tile_name, target_y, is_top, x_min, x_max);
            if(tile_name != new_tile_name && !is_top){
                result[tile[new_tile_name].bottom_left.x] = new_tile_name;
            }
            tile_name = tile[tile_name].right;
            if(tile_name == 0){
                break;
            }
            target_tile = tile[tile_name];
        }
        else{
            tile_name = tile[tile_name].down;
            if(tile_name == 0){
                break;
            }
            target_tile = tile[tile_name];
        }
    }

    return result;
}

vector <map<int,int>> Corner_Stitching::find_tile_contain_vertical_edge(int tile_name, int y_min, int y_max, int target_xmin, int target_xmax){

    map<int, int> left_result;
    map<int, int> right_result;
    map<int, int> insert_result;

    Tile target_tile = tile[tile_name];

    if(target_tile.bottom_left.y == y_max){
        tile_name = target_tile.down;
        target_tile = tile[target_tile.down];
    }
    
    while(target_tile.bottom_left.y >= y_min){

        if(target_tile.bottom_left.x <= target_xmin && target_tile.bottom_left.x + target_tile.w >= target_xmax){
            // left | insert | right
            if(target_tile.bottom_left.x < target_xmin){ 
                left_result[tile[tile_name].bottom_left.y] = tile_name;
                int new_tile_name = vertical_split(tile_name, target_xmin);
                if(tile[new_tile_name].bottom_left.x + tile[new_tile_name].w > target_xmax){
                    // left | insert | right
                    int new_tile_name_1 = vertical_split(new_tile_name, target_xmax);
                    insert_result[tile[new_tile_name].bottom_left.y] = new_tile_name;
                    right_result[tile[new_tile_name_1].bottom_left.y] = new_tile_name_1;
                }else{
                    // left | insert
                    insert_result[tile[new_tile_name].bottom_left.y] = new_tile_name;
                }
            }else if(target_tile.bottom_left.x + target_tile.w > target_xmax){
                // insert | right
                int new_tile_name = vertical_split(tile_name, target_xmax);
                insert_result[tile[tile_name].bottom_left.y] = tile_name;
                right_result[tile[new_tile_name].bottom_left.y] = new_tile_name;
            }else{
                // insert
                insert_result[tile[tile_name].bottom_left.y] = tile_name;
            }

            tile_name = tile[tile_name].down;
            if(tile_name == 0){
                break;
            }
            target_tile = tile[tile_name];
        }
        else{
            tile_name = tile[tile_name].right;
            if(tile_name == 0){
                break;
            }
            target_tile = tile[tile_name];
        }
    }

    vector <map<int, int>> result;
    result.push_back(left_result);
    result.push_back(insert_result);
    result.push_back(right_result);

    return result;
}

int Corner_Stitching::horizontal_split(int block_name, int tile_name, int target_y, bool is_top, int x_min, int x_max){
    cout << "horizontal_split" << endl;
    if(tile[tile_name].bottom_left.y == target_y || tile[tile_name].bottom_left.y + tile[tile_name].h == target_y){
        cout << "horizontal_split return tile_name :" << tile_name << endl;
        return tile_name;
    }
    // tile_name is the split top tile, new_tile_name is the split bottom tile
    // return the new_tile_name
    cout << " horizontal_split tile_name :" << tile_name << endl;
    Tile target_tile = tile[tile_name];

    int new_tile_name = vacant_tile_counter--;
    cout << "horizontal_split new_tile_name: " << new_tile_name << endl;
    tile[new_tile_name] = target_tile;

    // update the origin tile's bottom tile
    cout << "update the origin tile's bottom tile" << endl;
    int down_tile = 0;
    if (target_tile.down != 0) {
        down_tile = target_tile.down;
        cout << "   down_tile: " << down_tile << endl;
        do {
            if (tile[down_tile].top == tile_name) {
                tile[down_tile].top = new_tile_name;
                cout << "       adjust down_tile: " << down_tile << endl;
            }
            if (tile[down_tile].bottom_left.x >= target_tile.bottom_left.x + target_tile.w) {
                break;
            }
            down_tile = tile[down_tile].right;
        } while (down_tile != 0);
    }
    // update the origin tile's right tile
    cout << "update the origin tile's right tile" << endl;
    int right_tile = 0;
    int tile_right_modify = 0;
    if (target_tile.right != 0) {
        right_tile = target_tile.right;
        cout << "   right_tile: " << right_tile << endl;
        do {
            if (tile[right_tile].bottom_left.y <= target_y && tile[right_tile].bottom_left.y + tile[right_tile].h > target_y){
                tile_right_modify = right_tile;
            }

            if (tile[right_tile].left == tile_name && tile[right_tile].bottom_left.y < target_y) {
                cout << "       adjust right_tile: " << right_tile << endl;
                tile[right_tile].left = new_tile_name;
            }
            if (tile[right_tile].bottom_left.y <= target_tile.bottom_left.y) {
                break;
            }
            right_tile = tile[right_tile].down;
        } while (right_tile != 0);
    }

    // update the origin tile's left tile
    cout << "update the origin tile's left tile" << endl;
    int left_tile = 0;
    int tile_left_modify = 0;
    if (target_tile.left != 0) {
        left_tile = target_tile.left;
        cout << "   left_tile: " << left_tile << endl;
        do {
            if (tile[left_tile].right == tile_name && (tile[left_tile].bottom_left.y + tile[left_tile].h) <= target_y) {
                tile[left_tile].right = new_tile_name;
                cout << "       adjust left_tile: " << left_tile << endl;
            }
            
            if (tile[left_tile].bottom_left.y <= target_y && tile[left_tile].bottom_left.y + tile[left_tile].h > target_y){
                tile_left_modify = left_tile;
            }

            if (tile[left_tile].bottom_left.y >= target_tile.bottom_left.y + target_tile.h) {
                break;
            }
            left_tile = tile[left_tile].top;
        } while (left_tile != 0);
    }

    // update the new tile
    if(is_top==0 && target_tile.bottom_left.x + target_tile.w >= x_min && target_tile.bottom_left.x + target_tile.w < x_max){
        tile[new_tile_name].down = block_name;
    }else{
        tile[new_tile_name].top = tile_name;
    }
    tile[new_tile_name].h = target_y - target_tile.bottom_left.y;
    tile[new_tile_name].right = tile_right_modify;

    // update the tile
    if(is_top && target_tile.bottom_left.x >= x_min && target_tile.bottom_left.x < x_max){
        tile[tile_name].down = block_name;
    }else{
        tile[tile_name].down = new_tile_name;
    }
    tile[tile_name].left = tile_left_modify;
    tile[tile_name].bottom_left.y = target_y;
    tile[tile_name].h = tile[tile_name].h - tile[new_tile_name].h;

    if(tile[new_tile_name].right == 0 && tile[new_tile_name].down == 0){
        bottom_right_tile = new_tile_name;
    }

    return new_tile_name;
}

int Corner_Stitching::vertical_split(int tile_name, int target_x){
    cout << "vertical_split" << endl;
    if(tile[tile_name].bottom_left.x == target_x || tile[tile_name].bottom_left.x + tile[tile_name].w == target_x){
        return tile_name;
    }
    // tile_name is the split left tile, new_tile_name is the split right tile
    // return the new_tile_name
    cout << "tile_name :" << tile_name << endl;
    Tile target_tile = tile[tile_name];

    int new_tile_name = vacant_tile_counter--;
    tile[new_tile_name] = target_tile;

    // update the origin tile's right tile
    cout << "update the origin tile's right tile" << endl;
    int right_tile = 0;
    if (target_tile.right != 0) {
        right_tile = target_tile.right;
        cout << "   right_tile: " << right_tile << endl;
        do {
            if (tile[right_tile].left == tile_name && tile[right_tile].bottom_left.y < target_tile.bottom_left.y) {
                cout << "       adjust right_tile: " << right_tile << endl;
                tile[right_tile].left = new_tile_name;
            }
            if (tile[right_tile].bottom_left.y <= target_tile.bottom_left.y) {
                break;
            }
            right_tile = tile[right_tile].down;
        } while (right_tile != 0);
    }

    // update the origin tile's top tile
    cout << "update the origin tile's top tile" << endl;
    int top_tile = 0;
    int tile_top_modify = 0;
    if (target_tile.top != 0) {
        top_tile = target_tile.top;
        cout << "   top_tile: " << top_tile << endl;
        do {
            if (tile[top_tile].bottom_left.x < target_x && tile[top_tile].bottom_left.x + tile[top_tile].w >= target_x) {
                tile_top_modify = top_tile;
            }
            if (tile[top_tile].down == tile_name && tile[top_tile].bottom_left.x >= target_x) {
                tile[top_tile].down = new_tile_name;
                cout << "       adjust top_tile: " << top_tile << endl;
            }
            if (tile[top_tile].bottom_left.x < target_tile.bottom_left.x) {
                break;
            }
            top_tile = tile[top_tile].left;
        } while (top_tile != 0);
    }

    // update the origin tile's bottom tile
    cout << "update the origin tile's bottom tile" << endl;
    int down_tile = 0;
    int tile_down_modify = 0;
    if (target_tile.down != 0) {
        down_tile = target_tile.down;
        cout << "   down_tile: " << down_tile << endl;
        do {
            if (tile[down_tile].bottom_left.x <= target_x && tile[down_tile].bottom_left.x + tile[down_tile].w > target_x) {
                tile_down_modify = down_tile;
            }
            if (tile[down_tile].top == tile_name && tile[down_tile].bottom_left.x + tile[down_tile].w > target_x) {
                tile[down_tile].top = new_tile_name;
                cout << "       adjust down_tile: " << down_tile << endl;
            }
            if (tile[down_tile].bottom_left.x >= target_tile.bottom_left.x + target_tile.w) {
                break;
            }
            down_tile = tile[down_tile].right;
        } while (down_tile != 0);
    }

    // update the new tile
    tile[new_tile_name].down = tile_down_modify;    
    tile[new_tile_name].w =  target_tile.bottom_left.x +  target_tile.w - target_x;
    tile[new_tile_name].left = tile_name;
    tile[new_tile_name].bottom_left.x = target_x;

    // update the tile
    tile[tile_name].top = tile_top_modify;
    tile[tile_name].right = new_tile_name;
    tile[tile_name].w = target_x - tile[tile_name].bottom_left.x;

    cout << "=========================== Finish vertical split ====================" << endl;
    cout << "tile[tile_name]: " << tile_name << endl;
    cout << "tile[tile_name].bottom_left.x: " << tile[tile_name].bottom_left.x << endl;
    cout << "tile[tile_name].bottom_left.y: " << tile[tile_name].bottom_left.y << endl;
    cout << "tile[tile_name].w: " << tile[tile_name].w << endl;
    cout << "tile[tile_name].h: " << tile[tile_name].h << endl;
    cout << "tile[tile_name].left: " << tile[tile_name].left << endl;
    cout << "tile[tile_name].right: " << tile[tile_name].right << endl;
    cout << "tile[tile_name].top: " << tile[tile_name].top << endl;
    cout << "tile[tile_name].down: " << tile[tile_name].down << endl;

    cout << "tile[new_tile_name]: " << new_tile_name << endl;
    cout << "tile[new_tile_name].bottom_left.x: " << tile[new_tile_name].bottom_left.x << endl;
    cout << "tile[new_tile_name].bottom_left.y: " << tile[new_tile_name].bottom_left.y << endl;
    cout << "tile[new_tile_name].w: " << tile[new_tile_name].w << endl;
    cout << "tile[new_tile_name].h: " << tile[new_tile_name].h << endl;
    cout << "tile[new_tile_name].left: " << tile[new_tile_name].left << endl;
    cout << "tile[new_tile_name].right: " << tile[new_tile_name].right << endl;
    cout << "tile[new_tile_name].top: " << tile[new_tile_name].top << endl;
    cout << "tile[new_tile_name].down: " << tile[new_tile_name].down << endl;

    if(tile[new_tile_name].right == 0 && tile[new_tile_name].down == 0){
        bottom_right_tile = new_tile_name;
    }

    return new_tile_name;
}

void Corner_Stitching::vertical_merge(int block_index, map<int, int> tiles, bool is_left, int y_min, int y_max){
    cout << "vertical_merge" <<endl;
    stack <int> merge_tile;

    int down_not_top_tile = tile[tiles.begin()->second].down;
    if(down_not_top_tile!=0){
        cout << "tiles.begin()->second: "<< tiles.begin()->second<<endl;
        cout << "tile[tiles.begin()->second].down: "<< tile[tiles.begin()->second].down<<endl;
        merge_tile.push(down_not_top_tile);
    }

    for(auto it : tiles){
        int tile_name = it.second;
        Tile target_tile = tile[tile_name];
        if(is_left && target_tile.bottom_left.y <= y_min && target_tile.bottom_left.y + target_tile.h >= y_min){
            insert_left = tile_name;
        }
        if(is_left==false && target_tile.bottom_left.y <= y_max && target_tile.bottom_left.y + target_tile.h >= y_max){
            insert_right = tile_name;
        }
        if(merge_tile.size()==0){
            if(is_left){
                tile[tile_name].right = block_index;
            }else{
                tile[tile_name].left = block_index;
            }
            merge_tile.push(tile_name);
            continue;
        }
        if(tile[merge_tile.top()].bottom_left.x == target_tile.bottom_left.x && tile[merge_tile.top()].w == target_tile.w){
            int merge_tile_top = merge_tile.top();
            Tile target_tile_up = tile[merge_tile_top];

            // update target_tile right_neighbor
            if (target_tile.right != 0 && is_left == false) {
                int right_tile = target_tile.right;
                do {
                    if(tile[right_tile].left == tile_name){
                        tile[right_tile].left = merge_tile_top;
                    }
                    if (tile[right_tile].bottom_left.y <= tile[tile_name].bottom_left.y) {
                        break;
                    }
                    right_tile = tile[right_tile].down;
                } while (right_tile != 0);
            }

            // update target_tile left_neighbor
            if (tile[tile_name].left != 0 && is_left == true) {
                int left_tile = tile[tile_name].left;

                do {
                    if(tile[left_tile].right == tile_name){
                        tile[left_tile].right = merge_tile_top;
                    }
                    if (tile[left_tile].bottom_left.y >= tile[tile_name].bottom_left.y + tile[tile_name].h) {
                        break;
                    }
                    left_tile = tile[left_tile].top;
                } while (left_tile != 0);
            }
            
            // update target_tile top_neighbor
            if (tile[tile_name].top != 0) {
                int top_tile = tile[tile_name].top;

                do {
                    if(tile[top_tile].down == tile_name){
                        tile[top_tile].down = merge_tile_top;
                    }
                    if (tile[top_tile].bottom_left.x <= tile[tile_name].bottom_left.x) {
                        break;
                    }

                    top_tile = tile[top_tile].left;
                } while (top_tile != 0);
            }
            

            tile[merge_tile_top].h += tile[tile_name].h;

            if(is_left){ //left
                tile[merge_tile_top].top = tile[tile_name].top;
                if(tile[merge_tile_top].bottom_left.y + tile[merge_tile_top].h > y_min && tile[merge_tile_top].bottom_left.y + tile[merge_tile_top].h <= y_max){
                    tile[merge_tile_top].right = block_index;
                }
                else{
                    tile[merge_tile_top].right = tile[tile_name].right;
                }
            }else{ //right
                tile[merge_tile_top].top = tile[tile_name].top;
                tile[merge_tile_top].right = tile[tile_name].right;
                if(tile[merge_tile_top].bottom_left.y >= y_min && tile[merge_tile_top].bottom_left.y < y_max){
                    tile[merge_tile_top].left = block_index;
                }
                else{
                    tile[merge_tile_top].left = tile[tile_name].left;
                }
            }
            if(tile[merge_tile_top].down == 0 && tile[merge_tile_top].right == 0){
                bottom_right_tile = merge_tile_top;
            }
            //delete tile[tile_name]
            tile.erase(tile_name);
        }else{
            if(is_left){
                tile[tile_name].right = block_index;
            }else{
                tile[tile_name].left = block_index;
            }
            merge_tile.push(tile_name);
        }
    }

    return;
}

void Corner_Stitching::horizontal_merge(map<int, int> tiles){
    cout <<"horizontal_merge"<<endl;
    stack <int> merge_tile;

    int left_not_top_tile = tile[tiles.begin()->second].left;
    if(left_not_top_tile!=0){
        cout << "tiles.begin()->second: "<< tiles.begin()->second<<endl;
        cout << "tile[tiles.begin()->second].left: "<< tile[tiles.begin()->second].down<<endl;
        merge_tile.push(left_not_top_tile);
    }

    for(auto it : tiles){
        int tile_name = it.second;
        Tile target_tile = tile[tile_name];
        if(merge_tile.size()==0){
            merge_tile.push(tile_name);
            continue;
        }

        if(tile[merge_tile.top()].bottom_left.y == target_tile.bottom_left.y && tile[merge_tile.top()].h == target_tile.h){
            int merge_tile_top = merge_tile.top();
            Tile target_tile_left = tile[merge_tile_top];

            // update target_tile right_neighbor
            if (target_tile.right != 0) {
                int right_tile = target_tile.right;
                do {
                    if(tile[right_tile].left == tile_name){
                        tile[right_tile].left = merge_tile_top;
                    }
                    if (tile[right_tile].bottom_left.y <= tile[tile_name].bottom_left.y) {
                        break;
                    }
                    right_tile = tile[right_tile].down;
                } while (right_tile != 0);
            }

            // update target_tile down_neighbor
            if (tile[tile_name].down != 0) {
                int down_tile = tile[tile_name].down;
                do {
                    if(tile[down_tile].top == tile_name){
                        tile[down_tile].top = merge_tile_top;
                    }
                    if (tile[down_tile].bottom_left.x >= tile[tile_name].bottom_left.x + tile[tile_name].w) {
                        break;
                    }
                    down_tile = tile[down_tile].right;
                } while (down_tile != 0);
            }
            // update target_tile top_neighbor
            if (tile[tile_name].top != 0) {
                int top_tile = tile[tile_name].top;

                do {
                    if(tile[top_tile].down == tile_name){
                        tile[top_tile].down = merge_tile_top;
                    }
                    if (tile[top_tile].bottom_left.x <= tile[tile_name].bottom_left.x) {
                        break;
                    }

                    top_tile = tile[top_tile].left;
                } while (top_tile != 0);
            }
            
            tile[merge_tile_top].top = tile[tile_name].top;
            tile[merge_tile_top].right = tile[tile_name].right;

            tile[merge_tile_top].w += tile[tile_name].w;

            if(tile[merge_tile_top].down == 0 && tile[merge_tile_top].right == 0){
                bottom_right_tile = merge_tile_top;
            }

            //delete tile[tile_name]
            tile.erase(tile_name);
        }else{
            merge_tile.push(tile_name);
        }
    }

    return;
}