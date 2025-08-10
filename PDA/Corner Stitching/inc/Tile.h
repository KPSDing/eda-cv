#ifndef TILE_H
#define TILE_H

#include <iostream>
#include <string>

enum TileType { VACANT = 0, BLOCK = 1 };
#define OUTOFBOUNDARY 0

using namespace std;

struct coordinate {
    int x;
    int y;

    // Overloading the == operator
    bool operator==(const coordinate& other) const {
        return x == other.x && y == other.y;
    }
};


class Tile{
public:
    //Constructor
    Tile();
    Tile(bool in_type, coordinate in_coor, int in_w, int in_h, int in_top, int in_down, int in_left, int in_right);
    //data member
    bool type; //0:vacant, 1:block
    coordinate bottom_left;
    int w, h;
    int top, down, left, right;

    //member function
    bool point_inside_tile(coordinate);
    coordinate bottom_right();
    coordinate top_left();
    coordinate top_right();


};


#endif