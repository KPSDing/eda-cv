#ifndef LEGALIZER_H
#define LEGALIZER_H

using namespace std;

#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <iomanip>
#include <omp.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>

#define PSEUDO -1
#define FF 0
#define GATE 1
#define INSERTFF 2
#define ILLEGAL_FF 3

#define Y_SEARCH_RANGE 5

struct Point{
    double x, y;
};

class Block{
    public:
        string name;
        Point BL;
        Point original;
        double width, height;
        int type; // 0 for FFs, 1 for Gates, 2 for Inserted FFs
        double calculate_distance(){
            return sqrt(pow(BL.x - original.x, 2) + pow(BL.y - original.y, 2));
        };
    private:    
};

class CompareByDifference {
    double target_x;
    double target_y;

public:
    explicit CompareByDifference(double x, double y) : target_x(x), target_y(y) {}

    bool operator()(const pair<double, double>& a, const pair<double, double>& b) const {
        double distance_a = sqrt(pow(a.first - target_x, 2) + pow(a.second - target_y, 2));
        double distance_b = sqrt(pow(b.first - target_x, 2) + pow(b.second - target_y, 2));

        if (distance_a == distance_b) {
            if (a.first == b.first) {
                return a.second < b.second;
            }
            return a.first < b.first; 
        }

        return distance_a < distance_b;
    }
};

using Map_x_block = map<double, Block*>;

class Legalizer {
    public:
        // member functions
        Legalizer();
        void parser_info(string filename);
        void parser_opt(string filename, string filename2);
        void output_draw(string filename);

        void check_input();

        //data member
        unordered_map<string, Block> all_blocks;
        vector<Map_x_block> PlacementRow;

    private:
        bool window_search(Block* tempblock);
        bool greedy_search(Block* greedyblock, int initial_index, int range);
        
        mutex placement_mutex;

        // for block
        bool check_overlap(Block block);
        bool check_inside_boundary(Block block);
        bool check_legal(Block block);
        void Make_onsite(Block* block);
        int Get_PLR_index(Block block);
        int Get_y_range(int row_index, int set_range);
        bool find_overlap_block(Block block, unordered_set<string> &overlap_blocks);


        double alpha, beta;
        double die_x, die_y, die_width, die_height;
        double cost;
        double max_width, max_height;

        // for placementrow
        double Placementrow_startX, Placementrow_startY, Placementrow_siteWidth, Placementrow_siteHeight, Placementrow_totalNumOfSites, Placementrow_rightmost;
        void insert_block(Block* new_block);
        void remove_block(string blockname);
        void insert_block_into_row(Block& block);
        void remove_block_from_row(Block block);
        
        // for test legal
        double total_success;
        double total_fail;
        double total_greedy_success;
        double window_search_counter;
};

#endif