#include "Tile.h"

Tile::Tile(){
    type = VACANT;
    bottom_left.x = 0;
    bottom_left.y = 0;
    w = 0;
    h = 0;
    top = down = left = right = 0;
}

Tile::Tile(bool in_type, coordinate in_coor, int in_w, int in_h, int in_top, int in_down, int in_left, int in_right){
    type = in_type;
    bottom_left = in_coor;
    w = in_w;
    h = in_h;
    top = in_top;
    down = in_down;
    left = in_left;
    right = in_right;    
}

bool Tile::point_inside_tile(coordinate target){
    if(bottom_left.x <= target.x && target.x < bottom_left.x + w && bottom_left.y <= target.y && target.y < bottom_left.y + h){
        return true;
    }else{
        return false;
    }
}

coordinate Tile::bottom_right(){
    return {bottom_left.x + w, bottom_left.y};
}

coordinate Tile::top_left(){
    return {bottom_left.x, bottom_left.y + h};
}

coordinate Tile::top_right(){
    return {bottom_left.x + w, bottom_left.y + h};
}