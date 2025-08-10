#include "globalRouting.h"

using namespace std;

void GlobalRouting::parser(string gmpfile, string gclfile, string cstfile){
    parse_gmpfile(gmpfile);
    parse_gclfile(gclfile);
    parse_cstfile(cstfile);
}

void GlobalRouting::parse_gmpfile(string gmpfile){
    ifstream file(gmpfile);
    string line;
    file >> line; //skip the first line .ra
    file >> routingArea.lowerLeftX >> routingArea.lowerLeftY >> routingArea.width >> routingArea.height;
    // cout << "Routing Area: " << routingArea.lowerLeftX << " " << routingArea.lowerLeftY << " " << routingArea.width << " " << routingArea.height << endl;
    file >> line; //skip the next line .g
    file >> gridInfo.gridWidth >> gridInfo.gridHeight;
    // cout << "Grid Info: " << gridInfo.gridWidth << " " << gridInfo.gridHeight << endl;
    grid.resize((routingArea.height/gridInfo.gridHeight), vector<GCell>((routingArea.width/gridInfo.gridWidth)));
    file >> line; //skip the next line .c
    file >> chip1.lowerLeftX >> chip1.lowerLeftY >> chip1.width >> chip1.height;
    // cout << "Chip1: " << chip1.lowerLeftX << " " << chip1.lowerLeftY << " " << chip1.width << " " << chip1.height << endl;
    file >> line; //skip the next line .b
    getline(file, line);

    //eat until empty line
    while (getline(file, line)){
        if (line.empty()) break;
        istringstream ss(line);
        Point p;
        string name;
        ss >> name >> p.x >> p.y;
        p.x += chip1.lowerLeftX + routingArea.lowerLeftX;
        p.y += chip1.lowerLeftY + routingArea.lowerLeftY;
        // cout << name << " " << p.x << " " << p.y << endl;
        bump_chip1[name] = p;
        Bump b = Bump(name, p, make_pair((p.x - routingArea.lowerLeftX) / gridInfo.gridWidth,(p.y - routingArea.lowerLeftY) / gridInfo.gridHeight));
        bumps_info[name] = b;
    }

    file >> line; //skip the next line .c
    file >> chip2.lowerLeftX >> chip2.lowerLeftY >> chip2.width >> chip2.height;
    file >> line; //skip the next line .b
    getline(file, line);
    // cout << "Chip2: " << chip2.lowerLeftX << " " << chip2.lowerLeftY << " " << chip2.width << " " << chip2.height << endl;

    //eat until empty line
    while (getline(file, line)){
        if (line.empty()) break;
        istringstream ss(line);
        Point p;
        string name;
        ss >> name >> p.x >> p.y;
        p.x += chip2.lowerLeftX + routingArea.lowerLeftX;
        p.y += chip2.lowerLeftY + routingArea.lowerLeftY;
        // cout << name << " " << p.x << " " << p.y << endl;
        bump_chip2[name] = p;
        bumps_info[name].end = p;
        bumps_info[name].endCell = make_pair((p.x - routingArea.lowerLeftX) / gridInfo.gridWidth,(p.y - routingArea.lowerLeftY) / gridInfo.gridHeight);
    }

    file.close();
}

void GlobalRouting::parse_gclfile(string gclfile) {
    ifstream file(gclfile);
    if (!file.is_open()) {
        cerr << "Error: Cannot open " << gclfile << endl;
        return;
    }

    string line;
    file >> line; // skip .ec


    int leftCap, bottomCap;

    for (int i = 0; i < (routingArea.height / gridInfo.gridHeight); i++) {
        for (int j = 0; j < (routingArea.width / gridInfo.gridWidth); j++) {
            file >> grid[i][j].leftCapacity >> grid[i][j].bottomCapacity;
        }
    }

    file.close();
}

void GlobalRouting::parse_cstfile(string cstfile) {
    ifstream file(cstfile);
    if (!file.is_open()) {
        cerr << "Error: Cannot open " << cstfile << endl;
        return;
    }

    string line, section;

    file >> line; // skip .alpha
    file >> alpha;
    file >> line; // skip .beta
    file >> beta;
    file >> line; // skip .gamma
    file >> gamma;
    file >> line; // skip .delta
    file >> delta;
    file >> line; // skip .v
    file >> viaCost;
    file >> line; // skip .l
    
    for (int i = 0; i < (routingArea.height / gridInfo.gridHeight); i++) {
        for (int j = 0; j < (routingArea.width / gridInfo.gridWidth); j++) {
            file >> grid[i][j].verticalCost;
        }
    }

    file >> line; // skip .l

    for (int i = 0; i < (routingArea.height / gridInfo.gridHeight); i++) {
        for (int j = 0; j < (routingArea.width / gridInfo.gridWidth); j++) {
            file >> grid[i][j].horizontalCost;
        }
    }
    file.close();
}

void GlobalRouting::print_grid() {
    cout << "Grid Information (" << (routingArea.width / gridInfo.gridWidth) << " x " << (routingArea.height / gridInfo.gridHeight) << "):\n";
    for (int r = 0; r < (routingArea.height / gridInfo.gridHeight); ++r) {
        for (int c = 0; c < (routingArea.width / gridInfo.gridWidth); ++c) {
            const GCell& cell = grid[r][c];
            cout << "GCell[" << r << "][" << c << "] -> "
                 << "LeftCap: " << cell.leftCapacity << ", "
                 << "BottomCap: " << cell.bottomCapacity << ", "
                 << "LeftUsed: " << cell.leftUsed << ", "
                 << "BottomUsed: " << cell.bottomUsed << ", "
                 << "VerticalCost: " << fixed << setprecision(2) << cell.verticalCost << ", "
                 << "HorizontalCost: " << fixed << setprecision(2) << cell.horizontalCost
                 << endl;
        }
        cout << "-------------------------------------------\n";
    }
}

void GlobalRouting::print_bumps() {
    cout << "Bumps Information:\n";
    for (const auto& entry : bumps_info) {
        const string& key = entry.first;  // Bump 的 key 名稱
        const Bump& bump = entry.second;  // Bump 結構

        cout << "Bump Name: " << bump.name << "\n";
        cout << "  Start Position: (" << bump.start.x << ", " << bump.start.y << ")\n";
        cout << "  End Position: (" << bump.end.x << ", " << bump.end.y << ")\n";
        // startGCell
        const GCell& startGCell = grid[bump.startCell.first][bump.startCell.second];
            cout << "  Start GCell: (Row: " << bump.startCell.first 
                 << ", Col: " << bump.startCell.second << ")\n";
            cout << "    Left Capacity: " << startGCell.leftCapacity
                 << ", Bottom Capacity: " << startGCell.bottomCapacity
                 << ", Vertical Cost: " << startGCell.verticalCost
                 << ", Horizontal Cost: " << startGCell.horizontalCost << "\n";
        // endGCell
        const GCell& endGCell = grid[bump.endCell.first][bump.endCell.second];
            cout << "  End GCell: (Row: " << bump.endCell.first 
                 << ", Col: " << bump.endCell.second << ")\n";
            cout << "    Left Capacity: " << endGCell.leftCapacity
                 << ", Bottom Capacity: " << endGCell.bottomCapacity
                 << ", Vertical Cost: " << endGCell.verticalCost
                 << ", Horizontal Cost: " << endGCell.horizontalCost << "\n";
                
        cout << "  Cost: " << bump.cost << "\n";

        if (!bump.path.empty()) {
            cout << "  Path: ";
            for (const auto& p : bump.path) {
                cout << "(" << p.x << ", " << p.y << ") ";
            }
            cout << "\n";
        }
        cout << "-----------------------------------\n";
    }
}

vector<Point> GlobalRouting::a_star(Point start, Point end){
    temp_cost = 0;
    map <double, Node> openSet;
    unordered_set<Point> closedSet;
    
    unordered_map<Point, double> g_score;  // g(n)
    unordered_map<Point, double> f_score;  // f(n)

    g_score[start] = 0;
    f_score[start] = heuristic(start, end);
    openSet[f_score[start]] = {start, g_score[start], f_score[start], 0, 0, 0, 0};

    came_from.clear();

    vector<pair<Point, int>> directions = {
        {{0, 1}, 2}, {{1, 0}, 1}, {{0, -1}, 2}, {{-1, 0}, 1}
    };

    while (!openSet.empty()) {
        Node current = openSet.begin()->second;
        

        if (current.pos == end) {
            temp_cost = f_score[current.pos];
            return reconstruct_path(current.pos);
        }
        
        openSet.erase(openSet.begin());
        closedSet.insert(current.pos);

        for (auto& dir : directions) {
            Point neighbor = {current.pos.x + dir.first.x, current.pos.y + dir.first.y};
            int newDir = dir.second;

            if (neighbor.x < 0 || neighbor.y < 0 || 
                neighbor.x >= (routingArea.width / gridInfo.gridWidth) || 
                neighbor.y >= (routingArea.height / gridInfo.gridHeight))
                continue;

            if (closedSet.count(neighbor)) continue;

            GCell& cell = grid[neighbor.y][neighbor.x];
            double wireCost = (newDir == 1) ? alpha*gridInfo.gridWidth : alpha*gridInfo.gridHeight;
            double edgeCost = (newDir == 1) ? gamma *cell.horizontalCost : gamma *cell.verticalCost;
            double Viacost = (current.prevDir != 0 && current.prevDir != newDir) ? delta*viaCost : 0;
            double overflowCost = 0;
            // if (newDir == 1) {
            //     if (cell.leftUsed > cell.leftCapacity) overflowCost = beta ;
            //     else if (cell.leftUsed + 1 > cell.leftCapacity) overflowCost = beta * cell.leftCapacity;
            // } else {
            //     if (cell.bottomUsed > cell.bottomCapacity) overflowCost = beta;
            //     else if (cell.bottomUsed + 1 > cell.bottomCapacity) overflowCost = beta * cell.bottomCapacity;
            // }
            if (newDir == 1) {
                int overflow = max(0, (cell.leftUsed + 1) - cell.leftCapacity);
                overflowCost = beta * overflow;
            } else {
                int overflow = max(0, (cell.bottomUsed + 1) - cell.bottomCapacity);
                overflowCost = beta * overflow;
            }

            double tentative_g_score = g_score[current.pos] + wireCost + overflowCost + gamma * edgeCost + Viacost;

            if (!g_score.count(neighbor) || tentative_g_score < g_score[neighbor]) {
                came_from[neighbor] = current.pos;
                g_score[neighbor] = tentative_g_score;
                f_score[neighbor] = g_score[neighbor] + heuristic(neighbor, end);
                openSet[f_score[neighbor]] = {neighbor, g_score[neighbor], f_score[neighbor], newDir, wireCost, overflowCost, Viacost};
            }
        }
    }

    return {};
}

double GlobalRouting::heuristic(Point start, Point end){
    return (abs(start.x - end.x)*gridInfo.gridWidth) + (abs(start.y - end.y)*gridInfo.gridHeight);
}

vector<Point> GlobalRouting::reconstruct_path(Point current) {
    vector<Point> path;
    Point previous = current;
    int prevMoveX = 0, prevMoveY = 0;

    while (came_from.find(current) != came_from.end()) {
        Point next = came_from[current];

        int moveX = current.x - next.x;
        int moveY = current.y - next.y;

        if (moveX != prevMoveX || moveY != prevMoveY) {
            int realX = routingArea.lowerLeftX + current.x * gridInfo.gridWidth;
            int realY = routingArea.lowerLeftY + current.y * gridInfo.gridHeight;
            path.push_back({realX, realY});
            prevMoveX = moveX; 
            prevMoveY = moveY;
        }

        if (moveX == 1 || moveX == -1) {
            int col = min(current.x, next.x);
            int row = current.y;
            grid[row][col].leftUsed++;
        }
        else if (moveY == 1 || moveY == -1) {
            int row = min(current.y, next.y);
            int col = current.x;
            grid[row][col].bottomUsed++;
        }

        current = next;
    }

    int realStartX = routingArea.lowerLeftX + current.x * gridInfo.gridWidth;
    int realStartY = routingArea.lowerLeftY + current.y * gridInfo.gridHeight;
    path.push_back({realStartX, realStartY});
    reverse(path.begin(), path.end());
    return path;
}

void GlobalRouting::route_bumps() {
    double astarcost = 0;
    for (auto& entry : bumps_info) {
        const string& name = entry.first;
        Bump& bump = entry.second;
        Point startCell = {bump.startCell.first, bump.startCell.second};
        Point endCell = {bump.endCell.first, bump.endCell.second};
        vector<Point> result = a_star(startCell, endCell);
        bump.path = result;
        if (result.empty()) {
            cout << "No path found for bump " << name << endl;
            continue;
        }
        bump.cost = temp_cost;
        astarcost += temp_cost;
    }
    cout << "Total A* Cost: " << astarcost << endl;
}

void GlobalRouting::L_shape(string lgfile){
    ofstream file(lgfile);
    if(!file.is_open()){
        cout << "Error opening file" << endl;
        return;
    }
    // cout << "Writing to file " << lgfile << endl;
    // cout << bump_chip1.size() << " bumps in chip1" << endl;
    for(auto bump: bump_chip1){
        string name = bump.first;
        Point p1 = bump.second;
        Point p2 = bump_chip2[name];
        if(p1.x == p2.x){
            file << "n" << name << endl;
            file << "M1 " << p1.x << " " << p1.y << " " << p2.x << " " << p2.y << endl;
            file << ".end" << endl;
        }
        else if(p1.y == p2.y){
            file << "n" << name << endl;
            file << "via" << endl;
            file << "M2 " << p1.x << " " << p1.y << " " << p2.x << " " << p2.y << endl;
            file << "via" << endl;
            file << ".end" << endl;
        }
        else {
            srand(time(NULL));
            int r = rand() % 2;
            if(r == 0){ //horizontal first
                file << "n" << name << endl;
                file << "via" << endl;
                file << "M2 " << p1.x << " " << p1.y << " " << p2.x << " " << p1.y << endl;
                file << "via" << endl;
                file << "M1 " << p2.x << " " << p1.y << " " << p2.x << " " << p2.y << endl;
                file << ".end" << endl;
            }
            else { //vertical first
                file << "n" << name << endl;
                file << "M1 " << p1.x << " " << p1.y << " " << p1.x << " " << p2.y << endl;
                file << "via" << endl;
                file << "M2 " << p1.x << " " << p2.y << " " << p2.x << " " << p2.y << endl;
                file << "via" << endl;
                file << ".end" << endl;
            }
        }
    }

}

void GlobalRouting::outputfile(string lgfile){
    ofstream file(lgfile);
    if(!file.is_open()){
        cout << "Error opening file" << endl;
        return;
    }
    for(auto bump: bumps_info){
        string name = bump.first;
        Bump b = bump.second;
        if(b.path.empty()){
            cout << "No path found for bump " << name << endl;
            continue;
        }
        file << "n" << name << endl;
        bool layer = M1LAYER;
        Point prev = b.path[0];
        for(int i=1; i < b.path.size(); i++){
            Point curr = b.path[i];
            bool isHorizontal = (curr.y == prev.y);
            if (isHorizontal != layer) {
                file << "via" << endl;
                layer = !layer;
            }
            file << (layer ? "M2 " : "M1 ") << prev.x << " " << prev.y << " " << curr.x << " " << curr.y << endl;
            prev = curr;
        }
        if (layer) file << "via" << endl;
        file << ".end" << endl;
    }
}