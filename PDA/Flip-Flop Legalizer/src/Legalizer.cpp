#include "Legalizer.h"

Legalizer :: Legalizer(){
    all_blocks.clear();
    PlacementRow.clear();
    cost = 0;
    total_success = 0;
    total_fail = 0;
    total_greedy_success = 0;
    window_search_counter = 0;
}

void Legalizer::parser_info(string filename){
    ifstream file(filename);

    if(!file.is_open()){
        cout << "Error: File not found" << endl;
        return;
    }

    string ffname;
    double number;

    file >> ffname; // "ALPHA"
    file >> alpha;

    file >> ffname; // "BETA"
    file >> beta;

    file >> ffname; // "DieSize" 
    file >> die_x >> die_y >> die_width >> die_height;

    Block new_block;
    string garbage;
    int placementrow_count = 0;

    // read until file ends
    while(file >> ffname){
        if(ffname.substr(0,2) == "FF" || ffname.substr(0,1) == "C"){
            new_block.name = ffname;
            file >> new_block.BL.x >> new_block.BL.y >> new_block.width >> new_block.height;
            file >> garbage;

            new_block.original.x = new_block.BL.x;
            new_block.original.y = new_block.BL.y;

            if(ffname.substr(0,1) == "C"){
                new_block.type = GATE;
            }
            else{
                new_block.type = FF;
            }

            if(new_block.width > max_width){
                max_width = new_block.width;
            }
            if(new_block.height > max_height){
                max_height = new_block.height;
            }

            all_blocks[ffname] = new_block;
        }  
        else { // "PlacementRows"
            if(placementrow_count == 0){
                file >> Placementrow_startX; // Startx
                file >> Placementrow_startY; // Starty
                file >> Placementrow_siteWidth; // SiteWidth
                file >> Placementrow_siteHeight; // SiteHeight
                file >> Placementrow_totalNumOfSites; // TotalNumSites
                Placementrow_rightmost = Placementrow_startX + Placementrow_siteWidth * (Placementrow_totalNumOfSites-1);
            }
            else {
                double row_info;
                file >> row_info; // Startx
                file >> row_info; // Starty
                file >> row_info; // SiteWidth
                file >> row_info; // SiteHeight
                file >> row_info; // TotalNumSites
            }
            placementrow_count++; 
        }
    }
    
    PlacementRow.resize(placementrow_count); //0-> Placementrow_startY 1-> Placementrow_startY+Placementrow_siteHeight 2-> Placementrow_startY+2*Placementrow_siteHeight
    Block *pseudoblock = new Block();
    pseudoblock->name = "PseudoBlock";
    pseudoblock->type = PSEUDO;
    pseudoblock->BL.x = Placementrow_startX - 1;
    pseudoblock->BL.y = Placementrow_startY;
    pseudoblock->width = 1;
    pseudoblock->height = Placementrow_siteHeight;
    for(int i=0;i<PlacementRow.size();i++){
        PlacementRow[i].clear();
        PlacementRow[i][Placementrow_startX-1] = pseudoblock;
        PlacementRow[i][Placementrow_rightmost+1] = pseudoblock;
    }

    
    //initialize all_rows occupied_sites
    for(auto it = all_blocks.begin(); it != all_blocks.end(); it++){
        int this_row_height = ceil(it->second.height / Placementrow_siteHeight);
        //find index of the row in PlacementRow
        int row_index = (it->second.BL.y - Placementrow_startY) / Placementrow_siteHeight;
        for(int i=0;i<this_row_height && row_index < PlacementRow.size() ;i++,row_index++){
            PlacementRow[row_index][it->second.BL.x] = (&it->second);
        }
    }
    file.close();
}

void Legalizer::check_input(){
    cout << "ALPHA: " << alpha << endl;
    cout << "BETA: " << beta << endl;
    cout << "DieSize: " << die_x << " " << die_y << " " << die_width << " " << die_height << endl;

    cout << "Blocks: " << all_blocks.size() << endl;

    cout << "PlacementRows: " << PlacementRow.size() << endl;
    for(int i=0;i<PlacementRow.size();i++){
        cout << "Row " << i << ": " << PlacementRow[i].size() << endl;
    }

}

void Legalizer::parser_opt(string filename, string filename2){
    ifstream file(filename);

    if(!file.is_open()){
        cout << "Error: File1 not found" << endl;
        return;
    }

    ofstream file2(filename2, ios::out);

    if(!file2.is_open()){
        cout << "Error: File2 not found" << endl;
        return;
    }

    string line;

    int i = 0;


    while (std::getline(file, line)) {
        istringstream lineStream(line);
        string part;

        // Part 1: Read "Banking_Cell:"
        lineStream >> part;

        // Part 2: Collect FFs to be removed until " --> "
        while (lineStream >> part) {
            if (part == "-->") {
                break;
            }
            remove_block(part);
        }
        

        // Part 3: Read the merged FF name and coordinates after " --> "
        Block new_block;
        lineStream >> new_block.name >> new_block.original.x >> new_block.original.y >> new_block.width >> new_block.height;
        new_block.BL.x = new_block.original.x;
        new_block.BL.y = new_block.original.y;
        Make_onsite(&new_block);


        Block greedy_block0 = new_block;
        Block greedy_block1 = new_block;
        Block greedy_block2 = new_block;
        Block greedy_block3 = new_block;
        Block greedy_block4 = new_block;
        Block greedy_block5 = new_block;
        Block greedy_block6 = new_block;

        int range = PlacementRow.size()/7;

        auto future_insert = std::async(std::launch::async, [&]() {
            return window_search(&new_block);
        });

        auto future_greedy0 = std::async(std::launch::async, [this, &greedy_block0, range]() mutable {
            return greedy_search(&greedy_block0, 0, range);
        });

        auto future_greedy1 = std::async(std::launch::async, [this, &greedy_block1, range]() mutable {
            return greedy_search(&greedy_block1, range, range);
        });

        auto future_greedy2 = std::async(std::launch::async, [this, &greedy_block2, range]() mutable {
            return greedy_search(&greedy_block2, 2 * range, range);
        });

        auto future_greedy3 = std::async(std::launch::async, [this, &greedy_block3, range]() mutable {
            return greedy_search(&greedy_block3, 3 * range, range);
        });

        auto future_greedy4 = std::async(std::launch::async, [this, &greedy_block4, range]() mutable {
            return greedy_search(&greedy_block4, 4 * range, range);
        });

        auto future_greedy5 = std::async(std::launch::async, [this, &greedy_block5, range]() mutable {
            return greedy_search(&greedy_block5, 5 * range, range);
        });

        size_t adjusted_range = std::min(PlacementRow.size() - 6 * range, static_cast<size_t>(range));
        auto future_greedy6 = std::async(std::launch::async, [this, &greedy_block6, range, adjusted_range]() mutable {
            return greedy_search(&greedy_block6, 6 * range, adjusted_range);
        });


        bool find_insert = future_insert.get(); 
        bool find_greedy0 = future_greedy0.get();
        bool find_greedy1 = future_greedy1.get();
        bool find_greedy2 = future_greedy2.get();
        bool find_greedy3 = future_greedy3.get();
        bool find_greedy4 = future_greedy4.get();
        bool find_greedy5 = future_greedy5.get();
        bool find_greedy6 = future_greedy6.get();

        vector<Block> candidate = {new_block, greedy_block0, greedy_block1, greedy_block2, greedy_block3, greedy_block4, greedy_block5, greedy_block6};
        vector<bool> find_candidate = {find_insert, find_greedy0, find_greedy1, find_greedy2, find_greedy3, find_greedy4, find_greedy5, find_greedy6};

        bool first = false;
        for(int i=0; i<8; i++){
            if(find_candidate[i]){
                if(!first){
                    first = true;
                    new_block = candidate[i];
                }
                else{
                    if(candidate[i].calculate_distance() < new_block.calculate_distance()){
                        new_block = candidate[i];
                    }
                }
            }
        }

        if(first){
            total_success ++;
            new_block.type = INSERTFF;
            insert_block(&new_block);
        }
        else{
            total_fail ++;
            new_block.type = ILLEGAL_FF;
            insert_block(&new_block);
        }

        // output to file
        file2 << new_block.BL.x << " " << new_block.BL.y << endl;
        file2 << "0" << endl;

        i++;
    }
    file.close();
    file2.close();
}

void Legalizer::output_draw(string filename){
    ofstream file(filename, ios::out);
    file << std::fixed << "die " << die_x << " " << die_y << " " << die_width << " " << die_height <<endl;
 
    for(auto it = all_blocks.begin(); it != all_blocks.end(); it++){
        if(it->second.type == FF){
            file << std::fixed << "2 " << it->second.BL.x << " " << it->second.BL.y << " " << it->second.width << " " << it->second.height << endl;
        }
        else if(it->second.type == GATE){
            file << std::fixed << "1 " << it->second.BL.x << " " << it->second.BL.y << " " << it->second.width << " " << it->second.height << endl;
        }
        else if(it->second.type == ILLEGAL_FF){
            file << std::fixed << "0 " << it->second.BL.x << " " << it->second.BL.y << " " << it->second.width << " " << it->second.height << endl;
        }
        else {
            file << std::fixed << "3 " << it->second.BL.x << " " << it->second.BL.y << " " << it->second.width << " " << it->second.height << endl;
        } 
    }
    file.close();
}

void Legalizer::Make_onsite(Block* block) {
    int row_index = Get_PLR_index(*block);
    double site_y = Placementrow_startY + row_index * Placementrow_siteHeight;
    double site_y_upper = site_y + Placementrow_siteHeight;
    if (abs(block->BL.y - site_y) < abs(block->BL.y - site_y_upper)) {
        block->BL.y = site_y;
    }
    else {
        block->BL.y = site_y_upper;
    }
}

int Legalizer::Get_PLR_index(Block block){
    int row_index = (block.BL.y - Placementrow_startY) / Placementrow_siteHeight;
    if (row_index < 0 ) {
        row_index = 0;
    }
    else if (row_index >= PlacementRow.size()) {
        row_index = PlacementRow.size() - 1;
    }
    return row_index;
}

int Legalizer::Get_y_range(int row_index, int set_range){
    if(row_index - set_range < 0){
        return 0;
    }
    else if(row_index + set_range >= PlacementRow.size()){
        return PlacementRow.size() - (2*set_range) -1;
    }
    else{
        return row_index - set_range;
    }
}

bool Legalizer::check_overlap(Block block) {
    int this_row_height = ceil(block.height / Placementrow_siteHeight);
    int row_index = Get_PLR_index(block);
    bool overlap = false;
    
    for(int i=0;i<this_row_height;i++, row_index++){
        if(row_index == PlacementRow.size()){
            // cout << "Error: Block is out of top boundary" << endl;
            break;
        }
        // cout <<" i: " << i << " row_index: " << row_index << endl;
        auto& current_row = PlacementRow[row_index];
        if(current_row.empty()) continue;
        if(block.BL.x >= PlacementRow[row_index].rbegin()->first){
            // cout << "Error: Block is out of right boundary" << endl;
            overlap = true;
            break;
        }
        else if(block.BL.x <= PlacementRow[row_index].begin()->first){
            // cout << "Error: Block is out of left boundary" << endl;
            overlap = true;
            break;
        }
        else{
            auto block_it = current_row.lower_bound(block.BL.x);
            if(block_it->second->type != PSEUDO){
                // cout << " Block x: " << block.BL.x << " Block width: " << block.width << " block_it->first: " << block_it->first << endl;
                if(block.BL.x+block.width > block_it->first){
                    // cout << "Overlap 1" << endl;
                    overlap = true;
                    break;
                }
            }            
            
            if(block_it != PlacementRow[i].begin()){
                block_it = prev(block_it);
                if(block_it->second->type == PSEUDO){
                    continue;
                }
                if(block_it->second->BL.x + block_it->second->width > block.BL.x){
                    // cout << "Overlap 2" << endl;
                    overlap = true;
                    break;
                }
            }
        }
    }
    return overlap;
}

bool Legalizer::check_inside_boundary(Block block) {
    return (block.BL.x >= die_x) && (block.BL.x <= Placementrow_rightmost) && (block.BL.x >= Placementrow_startX) && (block.BL.y >= die_y) && ((block.BL.x + block.width) <= (die_x + die_width)) && ((block.BL.y + block.height) <= (die_y + die_height));
}
                    
bool Legalizer::check_legal(Block block) {
    if(!check_inside_boundary(block)){
        //cout << "Error: Block is out of boundary" << endl;
        return false;
    }
    if(check_overlap(block)){
        //cout << "Error: Block is overlapping" << endl;
        return false;
    }
    return true;
}

void Legalizer::insert_block_into_row(Block& block) {
    int this_row_height = ceil(block.height / Placementrow_siteHeight);
    int row_index = Get_PLR_index(block);
    for (int i = 0; i < this_row_height && row_index < PlacementRow.size(); i++, row_index++) {
        PlacementRow[row_index][block.BL.x] = &block;
    }
}


void Legalizer::insert_block(Block* new_block){
    lock_guard<mutex> lock(placement_mutex);

    string blockname = new_block->name;
    all_blocks[blockname] = *new_block;
    Block* stored_block = &all_blocks[blockname];

    insert_block_into_row(*stored_block);
}

void Legalizer::remove_block_from_row(Block block){
    // remove the block from the PlacementRow
    int this_row_height = ceil(block.height / Placementrow_siteHeight);
    int row_index = Get_PLR_index(block);
    for(int i=0;i<this_row_height && row_index < PlacementRow.size();i++, row_index++){
        PlacementRow[row_index].erase(block.BL.x);
    }
}       

void Legalizer::remove_block(string blockname){
    auto it = all_blocks.find(blockname);
    if(it == all_blocks.end()){
        cout << "Error: Block not found" << endl;
        return;
    }
    Block& block = it->second;

    remove_block_from_row(block);
    all_blocks.erase(block.name);
}

bool Legalizer::window_search(Block* tempblock) {
    Block* testblock = new Block();
    testblock->BL = tempblock->BL;
    testblock->width = tempblock->width;
    testblock->height = tempblock->height;

    int row_index = Get_PLR_index(*tempblock);
    int y_index = Get_y_range(row_index, Y_SEARCH_RANGE); // Y_SEARCH_RANGE is the range of rows to search
    int this_row_height = ceil(tempblock->height / Placementrow_siteHeight);

    // Initialize set with custom comparator
    set<pair<double, double>, CompareByDifference> need_to_judge(CompareByDifference(tempblock->BL.x, tempblock->BL.y));

    // Initialize iterators for current rows
    vector<decltype(PlacementRow[0].begin())> current_row_r(this_row_height);
    vector<decltype(PlacementRow[0].begin())> current_row_l(this_row_height);

    for (int i = 0; i < this_row_height && row_index < PlacementRow.size(); i++, row_index++) {
        if(PlacementRow[row_index].empty()) continue;
        current_row_r[i] = PlacementRow[row_index].upper_bound(tempblock->BL.x);

        if (current_row_r[i] != PlacementRow[row_index].begin()) {
            current_row_l[i] = prev(current_row_r[i]);
        } else {
            current_row_l[i] = current_row_r[i];
        }

        if (current_row_r[i] != PlacementRow[row_index].end() && current_row_r[i]->first <= Placementrow_rightmost) {
            for(int j = y_index; j < y_index + 2*Y_SEARCH_RANGE ; j++){
                need_to_judge.insert(make_pair(current_row_r[i]->first - tempblock->width, Placementrow_startY + j*Placementrow_siteHeight));
            }
            }
        if (current_row_l[i] != PlacementRow[row_index].end() && current_row_l[i]->first >= Placementrow_startX) {
            for(int j = y_index; j < y_index + 2*Y_SEARCH_RANGE ; j++){
                need_to_judge.insert(make_pair(current_row_l[i]->first + current_row_l[i]->second->width, Placementrow_startY + j*Placementrow_siteHeight));
            }
        }
    }
    
    for (int i = 0; i < this_row_height && row_index < PlacementRow.size(); i++, row_index++) {

        if(PlacementRow[row_index].empty()) continue;
        while (current_row_r[i]->second->type != PSEUDO) {
            for(int j = y_index; j < y_index + 2*Y_SEARCH_RANGE ; j++){
                need_to_judge.insert(make_pair(current_row_r[i]->first - tempblock->width, Placementrow_startY + j*Placementrow_siteHeight));
                need_to_judge.insert(make_pair(current_row_r[i]->first + current_row_r[i]->second->width, Placementrow_startY + j*Placementrow_siteHeight));
            }
            current_row_r[i] = next(current_row_r[i]);
        }
        
        while (current_row_l[i]->second->type != PSEUDO) {
            for(int j = y_index; j < y_index + 2*Y_SEARCH_RANGE ; j++){
                need_to_judge.insert(make_pair(current_row_l[i]->first - tempblock->width, Placementrow_startY + j*Placementrow_siteHeight));
                need_to_judge.insert(make_pair(current_row_l[i]->first + current_row_l[i]->second->width, Placementrow_startY + j*Placementrow_siteHeight));
            }
            current_row_l[i] = prev(current_row_l[i]);
        }
    }

    // Start window search
    while (!need_to_judge.empty()) {
        window_search_counter ++;
        double judge_x = (*need_to_judge.begin()).first;
        double judge_y = (*need_to_judge.begin()).second;
        
        need_to_judge.erase(need_to_judge.begin());
        testblock->BL.x = judge_x;
        testblock->BL.y = judge_y;
        
        if (check_legal(*testblock)) {
            tempblock->BL.x = judge_x;
            tempblock->BL.y = judge_y;
            delete testblock;
            return true;
        }
    }
    
    delete testblock;
    return false;
}

bool Legalizer::greedy_search(Block* greedyblock, int initial_index, int range) {
    Block* testblock = new Block();
    testblock->BL = greedyblock->BL;
    testblock->width = greedyblock->width;
    testblock->height = greedyblock->height;

    int row_index = Get_PLR_index(*greedyblock);
    int bias = 0;
    bool flag = true;

    while (flag) {
        testblock->BL.x = greedyblock->BL.x;

        int this_row_height = ceil(greedyblock->height / Placementrow_siteHeight);
        flag = false;

        // Initialize set with custom comparator
        set<pair<double, double>, CompareByDifference> need_to_judge(CompareByDifference(greedyblock->BL.x, greedyblock->BL.y));

        for(int i = initial_index; i < initial_index+range; i++){
            if(PlacementRow[i].empty()) continue;
            for(auto it = PlacementRow[i].begin(); it != PlacementRow[i].end(); it++){
                if(it->second->type != PSEUDO){
                    if(it->first - greedyblock->width >= Placementrow_startX){
                        need_to_judge.insert(make_pair(it->first - greedyblock->width, Placementrow_startY + i*Placementrow_siteHeight));
                    }
                    if(it->first + it->second->width <= Placementrow_rightmost){
                        need_to_judge.insert(make_pair(it->first + it->second->width, Placementrow_startY + i*Placementrow_siteHeight));
                    }
                }
            }
        }

        while (!need_to_judge.empty()) {
            double judge_x = (*need_to_judge.begin()).first;
            double judge_y = (*need_to_judge.begin()).second;
            need_to_judge.erase(need_to_judge.begin());
            testblock->BL.x = judge_x;
            testblock->BL.y = judge_y;
            if (check_legal(*testblock)) {
                greedyblock->BL.x = testblock->BL.x;
                greedyblock->BL.y = testblock->BL.y;
                delete testblock;
                return true;
            }
        }
    }
    return false;
}

bool Legalizer::find_overlap_block(Block block, unordered_set<string> &overlap_blocks) {
    int this_row_height = ceil(block.height / Placementrow_siteHeight);
    int row_index = Get_PLR_index(block);
    bool overlap = false;
    
    for(int i=0;i<this_row_height;i++, row_index++){
        if(row_index == PlacementRow.size()){
            break;
        }

        auto& current_row = PlacementRow[row_index];
        if(current_row.empty()) continue;
        if(block.BL.x >= PlacementRow[row_index].rbegin()->first){
            overlap_blocks.insert(current_row.rbegin()->second->name);
            overlap = true;
            continue;
        }
        else if(block.BL.x <= PlacementRow[row_index].begin()->first){
            overlap_blocks.insert(current_row.begin()->second->name);
            overlap = true;
            continue;
        }
        else{
            auto block_it = current_row.lower_bound(block.BL.x);
            if(block_it->second->type != PSEUDO){
                if(block.BL.x+block.width > block_it->first){
                    overlap_blocks.insert(block_it->second->name);
                    overlap = true;
                    continue;
                }
            }            
            
            if(block_it != PlacementRow[i].begin()){
                block_it = prev(block_it);
                if(block_it->second->type == PSEUDO){
                    continue;
                }
                if(block_it->second->BL.x + block_it->second->width > block.BL.x){
                    overlap_blocks.insert(block_it->second->name);
                    overlap = true;
                    continue;
                }
            }
        }
    }
    return overlap;
}