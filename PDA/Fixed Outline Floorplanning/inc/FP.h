#ifndef _FP_H_
#define _FP_H_

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <ctime>
#include <cstdlib>
#include <random>
#include <utility>
#include <map>
#include <set>
#include <list>
#include <queue>
#include <stack>
#include <deque>
#include <unordered_map>
#include <unordered_set>

using namespace std;

// Define the data structure for the graph
class BLOCK{
    public:
        int x, y;
        int width, height;
        unordered_set<string> nets;
        BLOCK(int x, int y, int width, int height): x(x), y(y), width(width), height(height) {};
};

class NET{
    public:
        unordered_set<string> net_blocks;
        unordered_set<string> net_terminals;
    private:
};

class COOR{
    public:
        int x, y;
        COOR(int x, int y): x(x), y(y) {};
    private:
};

struct Node {
    int index;
    int length;
    Node* left;
    Node* right;
};


class BST{
    // public:
    //     BST() : root(nullptr) {}
    //     ~BST() { clear(root); }
    //     void insertBST(int index, int length);
    //     void deleteNodeBST(int p, int t);
    //     int findBST(int p);
    //     void printBST();

        
    // private:
    //     Node* root;
    //     Node* createNode(int index, int length);
    //     Node* insert(Node* insertroot, int index, int length);
    //     Node* deleteNode(Node* deleteroot, int p, int t);
    //     Node* deletetheNode(Node* deleteroot, int index);
    //     Node* findMin(Node* root);
    //     void print(Node* root);
    //     // 清除 BST
    //     void clear(Node* root);
    public:
        map<int, int> bstMap;
        void insertBST(int index, int length);
        void deleteNodeBST(int p, int t);
        int findBST(int p);
        void printBST();
};

class FP{
    public:
        unordered_map<string, BLOCK*> blocks;
        unordered_map<string, NET*> nets;
        unordered_map<string, COOR*> terminals;

        void parser_block(string filename);
        void parser_net(string filename);
        void output_file(string filename);
        void output_draw(string filename);
        
        int get_LCS(vector<string> X, vector<string> Y, bool widthorheight);
        void update_match(vector<string> X, vector<string> Y);
        void test_parser_block(string filename);
        void test_parser_net(string filename);

        double alpha;

        void SA_initial(double initial_temperature, double cooling_rate, int max_iterations);
        void SA_detail(double initial_temperature, double cooling_rate, int max_iterations);

        clock_t start;
        bool in_flag;
    private:
        vector<string> block_sort;
        int num_blocks, num_nets, num_terminals;
        int outline_width, outline_height;
        int area_width, area_height;
        int total_HPWL;
        int total_area;
        multimap <int, string> area_sort_block;
        vector<string> total_blocks;
        unordered_map<string, pair<int, int>> match;
        unordered_set<string> outblock;
        int HPWL();
        void print_match();
        double Cost1(double alpha, int area, int HPWL, int outblock_area);
        double Cost2(double alpha, int area, int HPWL, int outblock_area);
        
        
        // best solution
        vector<string> best_X, best_Y;
        int best_cost, best_area, best_width, best_height, best_HPWL, best_outblock_area;
        unordered_map<string, BLOCK*> best_blocks;

        // current solution
        vector<string> current_X, current_Y;
        int current_cost, current_area, current_width, current_height, current_HPWL;
        unordered_map<string, BLOCK*> current_blocks;

        // temp solution
        vector<string> temp_X, temp_Y;
        int temp_cost, temp_area, temp_width, temp_height, temp_HPWL;

};

#endif