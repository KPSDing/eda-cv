#ifndef GLOBALROUTING_H
#define GLOBALROUTING_H

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
#include <random>
#include <functional>
#include <queue>

#define M1LAYER false
#define M2LAYER true

struct Point {
    int x;
    int y;
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
    bool operator<(const Point& other) const { return x < other.x || (x == other.x && y < other.y); }
};

namespace std {
    template <>
    struct hash<Point> {
        size_t operator()(const Point& p) const {
            return hash<int>()(p.x) ^ (hash<int>()(p.y) << 1);
        }
    };
}

struct GCell {
    // Edge capacities
    int leftCapacity;
    int bottomCapacity;
    int leftUsed;
    int bottomUsed;
    // Layer costs
    double verticalCost;    // layer 1
    double horizontalCost;  // layer 2

    GCell() : leftCapacity(0), bottomCapacity(0), leftUsed(0), bottomUsed(0), verticalCost(0), horizontalCost(0) {}
};

struct Bump {
    string name;
    Point start;
    Point end;
    pair<int, int> startCell;
    pair<int, int> endCell;
    vector<Point> path;
    double cost;

    Bump() : name(""), start({0, 0}), end({0, 0}), startCell({-1, -1}), endCell({-1, -1}), cost(0) {}
    Bump(string n, Point s, pair<int, int> sc) : name(n), start(s), startCell(sc), end({0, 0}), endCell({-1, -1}), cost(0) {}
};

struct RoutingArea {
    int lowerLeftX;
    int lowerLeftY;
    int width;
    int height;
};

struct GridInfo {
    int gridWidth;
    int gridHeight;
};

struct Node {
    Point pos;
    double gScore;
    double fScore;
    double wireLength;
    double overflowCost;
    double viaCost;
    int prevDir;

    bool operator>(const Node& other) const {
        if (fScore != other.fScore)
            return fScore > other.fScore;
        return gScore > other.gScore;
    }
};

struct CompareNode {
    bool operator()(const Node& a, const Node& b) const {
        return a.fScore > b.fScore;
    }
};

struct AStarResult {
    vector<Point> path;
    double totalCost;

    AStarResult() : totalCost(0) {}
    AStarResult(const vector<Point>& p, double c) : path(p), totalCost(c) {}
};


class GlobalRouting{
    public:
        void parser(string gmpfile, string gclfile, string cstfile);
        void L_shape(string lgfile);
        void print_grid();
        void print_bumps();
        void route_bumps();
        void outputfile(string lgfile);
    private:
        void parse_gmpfile(string gmpfile);
        void parse_gclfile(string gclfile);
        void parse_cstfile(string cstfile);
        vector<Point> a_star(Point start, Point end);
        double heuristic(Point start, Point end);
        vector<Point> reconstruct_path(Point current);

        RoutingArea routingArea;
        RoutingArea chip1;
        RoutingArea chip2;
        GridInfo gridInfo;
        unordered_map<string, Point> bump_chip1;
        unordered_map<string, Point> bump_chip2;
        unordered_map<string, Bump> bumps_info;

        vector<vector<GCell>> grid; //2D grid
        double alpha, beta, gamma, delta, viaCost;
        double temp_cost;
        unordered_map<Point, Point> came_from;
};


#endif