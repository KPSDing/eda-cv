#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <cmath>
using namespace std;

struct SIZE{
  double w;
  double h;
};

struct MODULE{
    double area;
    int choose;
    vector<SIZE> possible_size; //ratio w/h: 0.5,1.0,1.5,2.0
};

struct INDEX{
    int L;
    int R;
};

struct SIZE_info{
  SIZE Size;
  INDEX Index;
};

struct Node{
    string op;
    Node* left;
    Node* right;
    vector<SIZE> all_possible_size;
    vector<INDEX> child_index;
    Node(string c) : op(c), left(NULL), right(NULL){}
};

class SlicingTree{
private:
    vector<SIZE> Merge_2_size(string op, vector<SIZE> left, vector<SIZE> right, vector<INDEX>&child_index);
public:
    Node* build_slicing_tree(vector<string> expression,map<int,MODULE> module_information);
    void print_slicing_tree(Node* root, int depth = 0);
    void postorder_slicing_tree(Node* root, int depth=0);
    void bottom_up(Node* node);
    int find_min_area(vector<SIZE>all_size);
    
};

class Floorplan{
private:
    //============================= DATA MEMBER ================================
    int module_number;
    int min_index;
    vector<string> polish_expression; //polish_expression to optimize
    vector<string> polish_expression_out; //output polish_expression 
    map<int,MODULE> module_information; //all module information -> string=module name, key=module information
    
    SlicingTree slicingtree;
    Node* floorplanroot;//slicingtree root
    
    map<int,vector<SIZE>> map_area2size; //use area find possible size
    
    //====================== PRIVATE MEMBER FUNCTION ===========================
    vector<SIZE> Find_all_possible_size(double area);
    void topdown(Node* root, int index);
    

public:
    clock_t original_time, current_time;
    void Parser(char*argv);
    void Construction();
    void Outfile(char*argv);

};

int main(int argc,char*argv[]) {
    Floorplan pa3;
    pa3.original_time = clock(); 
    pa3.Parser(argv[1]);
    cout<<"=============================="<<endl;
    pa3.Construction();
    pa3.Outfile(argv[2]);
    return 0;
}

void Floorplan::Parser(char*argv){
    ifstream infile(argv,ios::in);
    if (!infile) {
        cerr << "Failed to open file" << endl;
        return;
    }
    
    infile >> module_number;
    
    for(int i=0;i<module_number;i++){
        //================= Initial module information =================
        MODULE newmodule;
        string name;
        infile >> name;
        //cout<<"name: "<<name;
        string area;
        infile >> area;
        newmodule.area=stod(area);
        //cout<<"area: "<<newmodule.area<<endl;
        newmodule.possible_size = Find_all_possible_size(newmodule.area);
        newmodule.choose=-1;
        module_information[stoi(name)] = newmodule;
        
        //================= Initial solution =================(12V3V4V5V...)
        polish_expression.push_back(name);
        if(i>0)
          polish_expression.push_back("V");
    }

}

vector<SIZE> Floorplan::Find_all_possible_size(double area){
    //曾經算過
    if (map_area2size.count(area)) {
        return map_area2size[area];
    }
    //沒算過
    vector<SIZE> res;
    SIZE tmp;
    if(module_number<20){
        for (double ratio = 0.501; ratio < 2; ratio=ratio+0.001) { //rato=w/h -> h=sqrt(area/ratio) w=sqrt(area*ratio)
            //cout<<"ratio: "<<ratio<<endl;
            tmp.h = sqrt(area)*sqrt(ratio);
            tmp.w = sqrt(area)/sqrt(ratio);
            //cout<<"\t tmp.w: "<< tmp.w <<"\t tmp.h: "<< tmp.h <<endl;
            //cout<<"\t ratio: "<< tmp.w/tmp.h <<endl;
            res.push_back(tmp);
        }
    }else {
        for (double ratio = 0.51; ratio < 2; ratio=ratio+0.1) { //rato=w/h -> h=sqrt(area/ratio) w=sqrt(area*ratio)
            //cout<<"ratio: "<<ratio<<endl;
            tmp.h = sqrt(area)*sqrt(ratio);
            tmp.w = sqrt(area)/sqrt(ratio);
            //cout<<"\t tmp.w: "<< tmp.w <<"\t tmp.h: "<< tmp.h <<endl;
            //cout<<"\t ratio: "<< tmp.w/tmp.h <<endl;
            res.push_back(tmp);
        }
    }
    map_area2size[area] = res;
    return res;
}

void Floorplan::Construction(){
    floorplanroot = slicingtree.build_slicing_tree(polish_expression,module_information);
    //slicingtree.print_slicing_tree(floorplanroot);
    slicingtree.bottom_up(floorplanroot);
    min_index = slicingtree.find_min_area(floorplanroot->all_possible_size);
    topdown(floorplanroot,min_index);
}

void Floorplan::Outfile(char*argv){
    ofstream file(argv,ios::out);
    if (!file) {
        cerr << "Failed to open file" << endl;
        return;
    }
    
    
    //cout<<"packing : "<<floorplanroot->all_possible_size[min_index].w;
    file<<floorplanroot->all_possible_size[min_index].w<<" ";
    //cout<<"\t"<<floorplanroot->all_possible_size[min_index].h;
    file<<floorplanroot->all_possible_size[min_index].h<<" ";
    double area = floorplanroot->all_possible_size[min_index].w*floorplanroot->all_possible_size[min_index].h;
    //cout<<"\t"<<area<<endl;
    file<<floorplanroot->all_possible_size[min_index].w*floorplanroot->all_possible_size[min_index].h<<endl;
    for (auto it = module_information.begin(); it != module_information.end(); ++it) {
        file << it->second.possible_size[it->second.choose].w <<" "<<it->second.possible_size[it->second.choose].h<<endl;
        //cout << "Module name: " << it->first << endl;
        //cout << "Module information:\n\tModule area: " << it->second.area <<endl;
        //cout << "\tModule size: \n";
        //cout<<"\t\tw: "<<it->second.possible_size[it->second.choose].w<<" h: "<<it->second.possible_size[it->second.choose].h<<endl;
    }
    for (int i=0;i<polish_expression.size();i++){
        //cout << polish_expression[i] << " ";
        file << polish_expression[i] ;
        if(i != polish_expression.size()-1)
          file << " ";
    }
}

Node* SlicingTree::build_slicing_tree(vector<string> expression,map<int,MODULE> module_information){
    stack<Node*> st;
    for (int i = 0; i<expression.size(); i++) {
        string c = expression[i];
        if (c!="V"&&c!="H") {
            Node* node = new Node(c);
            node->all_possible_size = module_information[stoi(c)].possible_size;
            st.push(node);
        } else {
            Node* left = st.top(); st.pop();
            Node* right = st.top(); st.pop();
            Node* node = new Node(c);
            node->left = left;
            node->right = right;
            st.push(node);
        }
    }
    return st.top();
}

void SlicingTree::print_slicing_tree(Node* root, int depth){
    if (root != NULL) {
        cout << depth <<":"<< root->op << endl;
        if(root->op!="V"&&root->op!="H"){
            for(int i=0;i<root->all_possible_size.size();i++){
                cout<<"\t\tw: "<<root->all_possible_size[i].w<<" h: "<<root->all_possible_size[i].h<<endl;
            }
        }
        print_slicing_tree(root->left, depth + 1);
        print_slicing_tree(root->right, depth + 1);
    }
}

void Floorplan::topdown(Node* root, int index){
    if (root != NULL) {
        //cout<<"root->op: "<<root->op<<endl;
        if(root->op!="V"&&root->op!="H"){
            module_information[stoi(root->op)].choose = index; 
        }
        if(root->op=="V"||root->op=="H"){
            topdown(root->left, root->child_index[index].L);
            topdown(root->right, root->child_index[index].R);
        }
    }
}

void SlicingTree::postorder_slicing_tree(Node* root, int depth){
    if (root == NULL) {
        return;
    }
    print_slicing_tree(root->left, depth + 1);
    for (int i = 0; i < depth; i++) {
        cout << "  ";
    }
    cout << root->op << endl;
    print_slicing_tree(root->right, depth + 1);
}


void SlicingTree::bottom_up(Node* node) {
    if (node->left == NULL && node->right == NULL) { //leaf
        return;
    }
    bottom_up(node->left);  
    bottom_up(node->right);  
    
    node->all_possible_size = Merge_2_size(node->op, node->left->all_possible_size, node->right->all_possible_size, node->child_index);
    
    /*cout<<"=============  out: =============="<<endl;
    cout<<"res.size"<<node->all_possible_size.size()<<endl;
    cout<<"child_index.size"<<node->child_index.size()<<endl;
    for (int i=0;i<node->all_possible_size.size();i++) {
        cout<<"\t"<<i<<"\t w: "<<node->all_possible_size[i].w<<"\t ";
        cout<<"h: "<<node->all_possible_size[i].h<<"\t";
        cout<<"L"<<node->child_index[i].L<<"\t";
        cout<<"R"<<node->child_index[i].R<<endl;
    }*/
    
    /*vector<SIZE> res,A,B;
    vector<INDEX> child_index;
    
    A.push_back({4,6});
    A.push_back({5,5});
    A.push_back({6,4});
    B.push_back({2,7});
    B.push_back({3,4});
    B.push_back({4,2});
    
    
    res = Merge_2_size("V", A, B, child_index);
    
    cout<<"=============  out: =============="<<endl;
    cout<<"res.size"<<res.size()<<endl;
    cout<<"child_index.size"<<child_index.size()<<endl;
    for (int i=0;i<res.size();i++) {
        cout<<"\t"<<i<<"\t w: "<<res[i].w<<"\t ";
        cout<<"h: "<<res[i].h<<"\t";
        cout<<"L"<<child_index[i].L<<"\t";
        cout<<"R"<<child_index[i].R<<endl;
    }*/
    
}

vector<SIZE> SlicingTree::Merge_2_size(string op, vector<SIZE> left, vector<SIZE> right, vector<INDEX>&child_index){
    
    map <double,SIZE_info> same_width; //key->width,value->(key->height,value->size)
    map <double,SIZE_info> same_height; //key->height,value->(key->width,value->size)
    vector<SIZE> res;
    
    SIZE_info tmp,original;
    if(op=="V"){
        for(int i=0;i<left.size();i++){
            for(int j=0;j<right.size();j++){
                tmp.Size.w = left[i].w+right[j].w;
                tmp.Size.h = max(left[i].h,right[j].h);
                tmp.Index.L = i;
                tmp.Index.R = j;
                if(same_width.count(tmp.Size.w)){
                    original=same_width[tmp.Size.w];
                    if(original.Size.h>tmp.Size.h)
                        same_width[tmp.Size.w]=tmp;
                }
                else{
                    same_width[tmp.Size.w]=tmp;
                }
            }
        }
    }else{
        for(int i=0;i<left.size();i++){
            for(int j=0;j<right.size();j++){
                tmp.Size.h = left[i].h+right[j].h;
                tmp.Size.w = max(left[i].w,right[j].w);
                tmp.Index.L = i;
                tmp.Index.R = j;
                if(same_width.count(tmp.Size.w)){
                    original=same_width[tmp.Size.w];
                    if(original.Size.h>tmp.Size.h)
                        same_width[tmp.Size.w]=tmp;
                }
                else{
                    same_width[tmp.Size.w]=tmp;
                }
            }
        }
    }
    for (auto it = same_width.begin(); it != same_width.end(); ++it) {
        tmp.Size.w = it->first;
        tmp.Size.h = it->second.Size.h;
        tmp.Index.L = it->second.Index.L;
        tmp.Index.R = it->second.Index.R;
        
        if(same_height.count(tmp.Size.h)){
            original=same_height[tmp.Size.h];
            if(original.Size.h>tmp.Size.h)
                same_height[tmp.Size.h]=tmp;
        }
        else{
            same_height[tmp.Size.h]=tmp;
        }
    }
    for (auto it = same_height.begin(); it != same_height.end(); ++it) {
        tmp = it->second;
        res.push_back(tmp.Size);
        child_index.push_back(tmp.Index);
    }
    
    /*cout<<"================================================================"<<endl;
    cout<<"res.size"<<res.size()<<endl;
    cout<<"child_index.size"<<child_index.size()<<endl;
    for (int i=0;i<res.size();i++) {
        cout<<"\t"<<i<<"\t w: "<<res[i].w<<"\t ";
        cout<<"h: "<<res[i].h<<"\t";
        cout<<"L"<<child_index[i].L<<"\t";
        cout<<"R"<<child_index[i].R<<endl;
    }*/
    
    return res;
}

int SlicingTree::find_min_area(vector<SIZE>all_size){
    int mini=0;
    double area = all_size[0].w * all_size[0].h;
    for(int i=1;i<all_size.size();i++){
        if(all_size[i].w * all_size[i].h<area){
            mini = i;
            area = all_size[i].w * all_size[i].h;
        }
    }
    return mini;
}



