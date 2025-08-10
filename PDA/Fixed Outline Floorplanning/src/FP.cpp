#include "FP.h"

std::default_random_engine rng(std::random_device{}());

void FP::parser_block(string filename){
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening block file" << endl;
        return;
    }

    area_sort_block.clear();

    string line;

    file >> line; //skip the first word "Outline"

    file >> outline_width;
    file >> outline_height;

    file >> line; //skip the first word "NumBlocks"
    file >> num_blocks;

    file >> line; //skip the first word "NumTerminals"
    file >> num_terminals;

    for (int i = 0; i < num_blocks; i++){
        string name;
        int width, height;
        file >> name >> width >> height;
        blocks[name] = new BLOCK(0, 0, width, height);
        match[name] = make_pair(-1, -1);
        area_sort_block.insert(make_pair(width * height, name));
        block_sort.push_back(name);
    }

    for (int i = 0; i < num_terminals; i++){
        string name, garbage;
        int x, y;
        file >> name >> garbage >> x >> y;
        terminals[name] = new COOR(x, y);
    }

    for (auto it = area_sort_block.rbegin(); it != area_sort_block.rend(); ++it) {
        total_blocks.push_back(it->second);
    }

    best_cost = numeric_limits<int>::max();
    // 1. Initialize the variables
    current_X = total_blocks;
    current_Y = total_blocks;

    best_X = current_X;
    best_Y = current_Y;

    in_flag = false;
}

void FP::test_parser_block(string filename){
    ofstream file(filename, ios::out);
    if (!file.is_open()) {
        cerr << "Error opening block file" << endl;
        return;
    }
    file << "Outline: " << outline_width << " " << outline_height << endl;
    file << "NumBlocks: " << num_blocks << endl;
    file << "NumTerminals: " << num_terminals << endl;
    for (auto block : blocks){
        file << "Block: ";
        file << block.first << endl; 
        file << "   " << block.second->width << " " << block.second->height << endl;
        file << "   Nets: " << endl;
        for (auto net : block.second->nets){
            file << "   " << net << endl;
        }
    }
    file << "Terminals: " << endl;
    file << "size: " << terminals.size() << endl;
    for (auto terminal : terminals){
        file << terminal.first << " " << terminal.second->x << " " << terminal.second->y << endl;
    }
}

void FP::parser_net(string filename){
    int net_number = 0;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening net file" << endl;
        return;
    }

    string line;
    file >> line; //skip the first word NumNets:
    file >> num_nets;

    for (int i = 0; i < num_nets; i++){
        string name;
        name = "net" + to_string(net_number++);
        int num_pins;
        file >> line; //skip the first word NetDegree:
        file >> num_pins;
        NET* net = new NET();
        for (int j = 0; j < num_pins; j++){
            string pin;
            file >> pin;
            if (blocks.find(pin) != blocks.end()){
                net->net_blocks.insert(pin);
                blocks[pin]->nets.insert(name);
            } else {
                net->net_terminals.insert(pin);
            }
        }
        nets[name] = net;
    }
}

void FP::test_parser_net(string filename){
    ofstream file(filename , ios::out);
    if (!file.is_open()) {
        cerr << "Error opening net file" << endl;
        return;
    }

    file << "NumNets: " << num_nets << endl;
    for (auto net : nets){
        file << "Net: ";
        file << net.first << endl;
        file << "   Blocks: " << endl;
        for (auto block : net.second->net_blocks){
            file << "   " << block << endl;
        }
        file << "   Terminals: " << endl;
        for (auto terminal : net.second->net_terminals){
            file << "   " << terminal << endl;
        }
    }
}



void FP::SA_initial(double initial_temperature, double cooling_rate, int max_iterations){
    cout << "SA_initial" << endl;
    vector<string> XR;

    clock_t temp_start = clock();
    // if the time is over 295s, break
    if (static_cast<double>(temp_start - start) / CLOCKS_PER_SEC > 295){
        //cout << "time > 295s ==> break" <<endl;
        return;
    }
    
    // 1. Initialize the variables
    current_X = best_X;
    current_Y = best_Y;

    temp_X = current_X;
    temp_Y = current_Y;
    
    current_cost = best_cost;
    temp_cost = current_cost;
    
    temp_start = clock();
    srand(time(NULL));
    double temperature = initial_temperature;

    int index, x1, x2;
    auto outblock_it = outblock.begin();

    for (int i = 0; i < max_iterations; ++i){
        //cout << "Round " << i << endl;
        temp_start = clock();
        // if the time is over 250s, break
        if (static_cast<double>(temp_start - start)/ CLOCKS_PER_SEC > 295){
            //cout << "time > 295s ==> break" <<endl;
            break;
        }

        // choose a shuffle function
        switch(rand()%5){
                case 0:
                // 選擇 X 中的兩個塊並交換
                if (temp_X.size() > 1) {
                    int x1 = rand() % temp_X.size();
                    int x2 = rand() % temp_X.size();
                    std::swap(temp_X[x1], temp_X[x2]);
                }
                break;
            case 1:
                // 選擇 Y 中的兩個塊並交換
                if (temp_Y.size() > 1) {
                    int y1 = rand() % temp_Y.size();
                    int y2 = rand() % temp_Y.size();
                    std::swap(temp_Y[y1], temp_Y[y2]);
                }
                break;
            case 2:
                // 選擇一個塊並交換它的寬度和高度
                if (!temp_X.empty()) {
                    int block_index = rand() % temp_X.size();
                    int width = blocks[temp_X[block_index]]->width;
                    int height = blocks[temp_X[block_index]]->height;
                    blocks[temp_X[block_index]]->width = height;
                    blocks[temp_X[block_index]]->height = width;
                }
                break;
            case 3:
                if(outblock.size() == 0) break;
                index = rand() % outblock.size();
                outblock_it = outblock.begin();
                advance(outblock_it, index); // 獲取 outblock 中的元素
                for(x2 = 0; x2 < temp_X.size(); x2++){
                    if(temp_X[x2] == *outblock_it) break;
                }
                x1 = rand() % temp_X.size();
                swap(temp_X[x1], temp_X[x2]);
                break;
            case 4:
                if(outblock.size() == 0) break;
                index = rand() % outblock.size();
                outblock_it = outblock.begin();
                advance(outblock_it, index); // 獲取 outblock 中的元素
                for(x2 = 0; x2 < temp_Y.size(); x2++){
                    if(temp_Y[x2] == *outblock_it) break;
                }
                x1 = rand() % temp_Y.size();
                swap(temp_Y[x1], temp_Y[x2]);
                break;
        }

        XR = temp_X;
        reverse(XR.begin(), XR.end());

        outblock.clear();

        // 2. Calculate the area width and height
        int temp_area_width = get_LCS(temp_X, temp_Y, true); // width
        int temp_area_height = get_LCS(XR, temp_Y, false); // height

        
        // 3. Calculate the total outblock area
        int temp_outblock_area = 0;
        for (auto block : outblock){
            temp_outblock_area += (blocks[block]->width * blocks[block]->height);
        }

        if(temp_outblock_area == 0){
            //cout << "================================================================================="<<endl;
            in_flag = true;
        }
        // 4. Calculate the total HPWL
        int temp_HPWL = HPWL();

        // 5. Calculate the total area
        int temp_area = temp_area_width * temp_area_height;

        // 6. Calculate the total cost
        double temp_cost = Cost1(alpha, temp_area, temp_HPWL, temp_outblock_area);
        // cout << temp_cost << endl;
        // cout << temp_area <<" " << temp_HPWL << " " << temp_outblock_area << " "  <<endl;

        // if(temp_cost < 0 ) break;
        // cout << "best cost: " << best_cost << ", current cost: "<< current_cost << ", temp cost: "<< temp_cost << endl;
        
        double acceptance_prob ;
        if (temperature > 1e-10) { // 確保 temperature 足夠大
            acceptance_prob = exp((current_cost - temp_cost) / temperature);
        }
        else {
            acceptance_prob = exp((current_cost - temp_cost) / (10*temperature));
        }

        // 7. Update the current best solution
        if ((temp_cost < current_cost) || (acceptance_prob > (rand() % 100) / 100.0)) {
            current_X = temp_X;
            current_Y = temp_Y;
            current_cost = temp_cost;
            current_area = temp_area;
            current_HPWL = temp_HPWL;
            current_width = temp_area_width;
            current_height = temp_area_height;
            current_blocks.clear();
            for (auto block : blocks){
                current_blocks[block.first] = new BLOCK(block.second->x, block.second->y, block.second->width, block.second->height);
            }

            if (temp_cost < best_cost) {
                best_X = temp_X;
                best_Y = temp_Y;
                best_cost = temp_cost;
                best_area = temp_area;
                best_HPWL = temp_HPWL;
                best_width = temp_area_width;
                best_height = temp_area_height;
                best_outblock_area = temp_outblock_area;
                best_blocks.clear();
                for (auto block : blocks){
                    best_blocks[block.first] = new BLOCK(block.second->x, block.second->y, block.second->width, block.second->height);
                }
            }
        }else {
            // 恢復臨時解為當前解
            temp_X = current_X;
            temp_Y = current_Y;
            temp_cost = current_cost;
            temp_area = current_area;
            temp_HPWL = current_HPWL;
            temp_width = current_width;
            temp_height = current_height;
            for (auto block : blocks){
                blocks[block.first]->x = current_blocks[block.first]->x;
                blocks[block.first]->y = current_blocks[block.first]->y;
                blocks[block.first]->width = current_blocks[block.first]->width;
                blocks[block.first]->height = current_blocks[block.first]->height;
            }
        }

        // 8. cooling down
        temperature *= cooling_rate;

        // 9. if the temperature is lower than 0.1, break
        if (temperature < 1e-10) {
            //cout << "temperature < 1e-10 ==> break" <<endl;
            break;
        }

        if(in_flag) break;
    }
}

double FP::Cost1(double alpha, int area, int HPWL, int outblock_area){
    return int(alpha * area + (1 - alpha) * HPWL + outblock_area);
}

double FP::Cost2(double alpha, int area, int HPWL, int outblock_area){
    return int(alpha * area + (1 - alpha) * HPWL + 100*outblock_area);
}

void FP::SA_detail(double initial_temperature, double cooling_rate, int max_iterations){
    cout << "SA_detail" << endl;
    clock_t temp_start = clock();
    // if the time is over 295s, break
    if (static_cast<double>(temp_start - start)/ CLOCKS_PER_SEC > 295){
        //cout << "time > 295s ==> break" <<endl;
        return;
    }

    vector<string> XR;
    current_X = best_X;
    current_Y = best_Y;
    
    temp_X = current_X;
    temp_Y = current_Y;

    best_cost = Cost2(alpha, best_area, best_HPWL, best_outblock_area);
    current_cost = best_cost;
    temp_cost = current_cost;

    srand(time(NULL));
    double temperature = initial_temperature;

    int index, x1, x2, i=0;
    auto outblock_it = outblock.begin();

    do{
        //cout << "Round " << i++ << endl;
        temp_start = clock();
        // if the time is over 295s, break
        if (static_cast<double>(temp_start - start)/ CLOCKS_PER_SEC > 295){
            //cout << "time > 295s ==> break" <<endl;
            break;
        }

        //cout << "1111111111111111111111111111111111111111111111111"<<endl;

        switch(rand()%3){
            case 0:
                // 選擇 X 中的兩個塊並交換
                if (temp_X.size() > 1) {
                    int x1 = rand() % temp_X.size();
                    int x2 = rand() % temp_X.size();
                    std::swap(temp_X[x1], temp_X[x2]);
                }
                break;
            case 1:
                // 選擇 Y 中的兩個塊並交換
                if (temp_Y.size() > 1) {
                    int y1 = rand() % temp_Y.size();
                    int y2 = rand() % temp_Y.size();
                    std::swap(temp_Y[y1], temp_Y[y2]);
                }
                break;
            case 2:
                // 選擇一個塊並交換它的寬度和高度
                if (!temp_X.empty()) {
                    int block_index = rand() % temp_X.size();
                    int width = blocks[temp_X[block_index]]->width;
                    int height = blocks[temp_X[block_index]]->height;
                    blocks[temp_X[block_index]]->width = height;
                    blocks[temp_X[block_index]]->height = width;
                }
                break;
        }

        //cout << "22222222222222222222222222222222222222222222222"<<endl;
        
        XR = temp_X;
        reverse(XR.begin(), XR.end());

        outblock.clear();

        // 2. Calculate the area width and height
        int temp_area_width = get_LCS(temp_X, temp_Y, true); // width
        if(temp_area_width > outline_width) {
            continue;
        }

        //cout << "3333333333333333333333333333333333333333333333333"<<endl;
        int temp_area_height = get_LCS(XR, temp_Y, false); // height
        if(temp_area_height > outline_height) {
            continue;
        }
        
        //cout << "44444444444444444444444444444444444444444444444444"<<endl;
        // 3. Calculate the total outblock area
        int temp_outblock_area = 0;
        for (auto block : outblock){
            temp_outblock_area += (blocks[block]->width * blocks[block]->height);
        }

        //cout << "555555555555555555555555555555555555555555555555555"<<endl;
        // 4. Calculate the total HPWL
        int temp_HPWL = HPWL();

        // 5. Calculate the total area
        int temp_area = temp_area_width * temp_area_height;

        // 6. Calculate the total cost
        double temp_cost = Cost2(alpha, temp_area, temp_HPWL, temp_outblock_area);

        double acceptance_prob ;
        if (temperature > 1e-10) { // 確保 temperature 足夠大
            acceptance_prob = exp((current_cost - temp_cost) / temperature);
        }
        else {
            acceptance_prob = exp((current_cost - temp_cost) / (10*temperature));
        }

        //cout << "6666666666666666666666666666666666666666666666666666"<<endl;

        // 7. Update the current best solution
        if ((temp_cost < current_cost) || (acceptance_prob > (rand() % 100) / 100.0)) {
            current_X = temp_X;
            current_Y = temp_Y;
            current_cost = temp_cost;
            current_area = temp_area;
            current_HPWL = temp_HPWL;
            current_width = temp_area_width;
            current_height = temp_area_height;
            current_blocks.clear();
            for (auto block : blocks){
                current_blocks[block.first] = new BLOCK(block.second->x, block.second->y, block.second->width, block.second->height);
            }

            if (temp_cost < best_cost) {
                best_X = temp_X;
                best_Y = temp_Y;
                best_cost = temp_cost;
                best_area = temp_area;
                best_HPWL = temp_HPWL;
                best_width = temp_area_width;
                best_height = temp_area_height;
                best_outblock_area = temp_outblock_area;
                best_blocks.clear();
                for (auto block : blocks){
                    best_blocks[block.first] = new BLOCK(block.second->x, block.second->y, block.second->width, block.second->height);
                }
            }
        }else {
            // 恢復臨時解為當前解
            temp_X = current_X;
            temp_Y = current_Y;
            temp_cost = current_cost;
            temp_area = current_area;
            temp_HPWL = current_HPWL;
            temp_width = current_width;
            temp_height = current_height;
            for (auto block : blocks){
                blocks[block.first]->x = current_blocks[block.first]->x;
                blocks[block.first]->y = current_blocks[block.first]->y;
                blocks[block.first]->width = current_blocks[block.first]->width;
                blocks[block.first]->height = current_blocks[block.first]->height;
            }
        }

        // 8. cooling down
        temperature *= cooling_rate;

    }while(temperature > 1e-12);
}

int FP::HPWL(){
    int temp_HPWL = 0;
    int xmin, ymin, xmax, ymax;
    for (auto net : nets){
        xmin = numeric_limits<int>::max();
        ymin = numeric_limits<int>::max();
        xmax = 0;
        ymax = 0;
        for (auto block : net.second->net_blocks){
            xmin = min(xmin, (blocks[block]->x + (blocks[block]->width/2)));
            ymin = min(ymin, (blocks[block]->y + (blocks[block]->height/2)));
            xmax = max(xmax, (blocks[block]->x + (blocks[block]->width/2)));
            ymax = max(ymax, (blocks[block]->y + (blocks[block]->height/2)));
        }
        for (auto terminal : net.second->net_terminals){
            xmin = min(xmin, terminals[terminal]->x);
            ymin = min(ymin, terminals[terminal]->y);
            xmax = max(xmax, terminals[terminal]->x);
            ymax = max(ymax, terminals[terminal]->y);
        }
        temp_HPWL += ((xmax - xmin) + (ymax - ymin));
    }
    return temp_HPWL;
}


int FP::get_LCS(vector<string> X, vector<string> Y, bool widthorheight){
    update_match(X, Y);
    // print_match();
    BST bst;
    bst.insertBST(0, 0);
    string b;
    int p, Pb, t;
    for (int i = 0; i < X.size(); i++){
        // cout << "Round i: " << i <<endl;
        b = X[i];
        // cout << "   b = X[i] = " << b <<endl;
        p = match[b].second;
        // cout << "   p = match[b].first = " << p <<endl;
        if(widthorheight == 1){
            // cout << "       widthorheight = 1 " <<endl;
            blocks[b]->x = bst.findBST(p);

            // judge whether the block is out of outline
            if(blocks[b]->x > outline_width - blocks[b]->width) outblock.insert(b);

            // judge whether the block is at the leftmost
            if(blocks[b]->x == -1) blocks[b]->x = 0;

            // cout << "       blocks[b]->x = bst.findBST(p) = " << blocks[b]->x <<endl;
            t = blocks[b]->x + blocks[b]->width;
            // cout << "       t = blocks[b]->x + blocks[b]->width = " << t <<endl;
            bst.insertBST(p, t);
        }
        else{
            // cout << "       widthorheight = 0 " <<endl;
            blocks[b]->y = bst.findBST(p);

            // judge whether the block is out of outline
            if(blocks[b]->y > outline_height - blocks[b]->height) outblock.insert(b);

            // judge whether the block is at the bottommost
            if(blocks[b]->y == -1) blocks[b]->y =0;

            // cout << "       blocks[b]->y = bst.findBST(p) = " << blocks[b]->y <<endl;
            t = blocks[b]->y + blocks[b]->height;
            // cout << "       t = blocks[b]->y + blocks[b]->height = " << t <<endl;
            bst.insertBST(p, t);
        }
        bst.deleteNodeBST(p, t);

        //bst.printBST();
    }

    int result = bst.bstMap.rbegin()->second;
    return result;
}

void FP::update_match(vector<string> X, vector<string> Y){
    match.clear();
    for (int i = 0; i < X.size(); i++){
        match[X[i]].first = i;
        match[Y[i]].second = i;
    }
}

void FP::print_match(){
    cout <<"=========== Print match ================\n";
    for (auto matches: match){
        cout << matches.first << " " << matches.second.first << " " << matches.second.second <<endl;
    }
}


void FP::output_file(string filename){
    ofstream file(filename, ios::out);
    if (!file.is_open()) {
        cerr << "Error opening output file" << endl;
        return;
    }

    file << best_cost << endl;
    file << best_HPWL << endl;
    file << best_area << endl;
    file << best_width << " " << best_height << endl;
    clock_t end = clock();
    file << (end - start) / CLOCKS_PER_SEC << endl;
    for (auto block : block_sort){
        file << block << " " << best_blocks[block]->x << " " << best_blocks[block]->y << " " << best_blocks[block]->x + best_blocks[block]->width << " " << best_blocks[block]->y + best_blocks[block]->height << endl;
        // file << block.first << " " << block.second->x << " " << block.second->y << " " << block.second->x + block.second->width << " " << block.second->y + block.second->height << endl;
    }
}

void FP::output_draw(string filename){
    ofstream file(filename, ios::out);
    if (!file.is_open()) {
        cerr << "Error opening output file" << endl;
        return;
    }
    // print total blocks number
    file << total_blocks.size() << endl;
    // print outline width and height
    file << outline_width << " " << outline_height << endl;
    // print each block's name, x, y, width, height
    int i=1;
    for (auto block : block_sort){
        file << block << " " << best_blocks[block]->x << " " << best_blocks[block]->y << " " << best_blocks[block]->width << " " << best_blocks[block]->height << endl;
        // file << block.first << " " << block.second->x << " " << block.second->y << " " << block.second->x + block.second->width << " " << block.second->y + block.second->height << endl;
    }
}

//=====================================================================================
// BST
//=====================================================================================

void BST::insertBST(int index, int length) {
    bstMap[index] = length;
}

void BST::deleteNodeBST(int p, int t) {
    auto it = bstMap.begin();
    while (it != bstMap.end()) {
        if (it->first > p && it->second < t) {
            it = bstMap.erase(it);  // 删除满足条件的节点
        } else {
            ++it;
        }
    }
}

int BST::findBST(int p) {
    int maxIndex = -1;
    int length = -1;

    auto it = bstMap.lower_bound(p); // 找到大于等于 p 的第一个元素
    if (it != bstMap.begin()) {
        --it; // 移动到小于 p 的最大元素
        maxIndex = it->first;
        length = it->second;
    }

    return length;
}

void BST::printBST() {
    for (const auto& node : bstMap) {
        cout << "(" << node.first << ", " << node.second << ") ";
    }
    cout << endl;
}