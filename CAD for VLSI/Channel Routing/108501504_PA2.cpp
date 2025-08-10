#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <queue>

using namespace std;

// Node struct that represents each node in the graph
struct Node {
    char symbol;              // Node type: i, o, +, *
    vector<int> successors;   // Indices of successor nodes
    vector<int> predecessors; // Indices of predecessor nodes
    int in_degree;            // Number of nodes pointing to this node as a successor
    int out_degree;           // Number of nodes that this node pointing to as a successor
    int delay;                // +:1,*:3
    int execute=0;              // 0:not execute, 1:on executing, 2:already executed
};

class List_Schedule{
private:
  //================ Private Variables =========================
  int Latency;                // max latency constraint
  int Nodes;                  // total number of nodes
  map<int, Node> graph;       // map to store the relationship for each node
  vector<int> sorted_nodes;   // inverse topological sort for nodes
  map<int, int> alap;         // map to store ALAP for each node
  int resource[2]={1,1};      // resource[0]=adder , resource[1]=multiplixer
  int canbeschedule = 1;      // alap>= 0:canbeschedule = 1,alap< 0:canbeschedule = 0
  map<int,vector<int>> scheduled; // time cycles -> scheduled nodes (for output)

  map<int,vector<int>> ready_node_add; //alap -> ready node to execute add
  map<int,vector<int>> ready_node_mul; //alap -> ready node to execute mul
  queue <int> on_executing_add;   //nodes on executing add
  queue <int> on_executing_mul;   //nodes on executing mul
  //================ Private Function Prototyping =========================
  void predecessor_ready(int index);
public:
  //================ Public Function Prototyping =========================
  void parser_in(char*);      // read input file      
  void constructGraph();      // initialize all nodes informations in the graph
  void topologicalSort();     // calculate all nodes ALAP
  bool MR_LCS();              // List-scheduling for MR_LCS
  void parser_out(char*);     // write output file
};


int main(int argc,char*argv[]) {
    List_Schedule pa2;
    pa2.parser_in(argv[1]);
    pa2.constructGraph();
    pa2.topologicalSort();
    pa2.MR_LCS();
    pa2.parser_out(argv[2]);
    return 0;
}

// Function that reads the input file and constructs the graph
void List_Schedule::parser_in(char*argv) {
    ifstream file(argv,ios::in);
    if (!file) {
        cerr << "Failed to open file" << endl;
        return;
    }

    string garbage;
    getline(file, garbage);    // Skip the comment line
    file >> garbage;           // Skip "Latency"
    file >> garbage;           // Skip "constrain:"     
    file >> Latency;
    //cout<<"Latency: "<<Latency<<endl;
    
    getline(file, garbage);           // Skip " "
    getline(file, garbage);           // Skip " "
    getline(file, garbage);           // Skip "#Test Data"   
    
    //=================== Deal with Nodes =========================
    string node_expression;
    while (getline(file, node_expression)) {
        //cout<<"=========================================="<<endl;
        //cout<<"node_expression: "<<node_expression<<endl;
        
        Node node;
        stringstream ss(node_expression);
        
        //=========== index ============
        int index;
        ss >> index;
        //cout<< "index: "<<index<<endl;
        
        //=========== symbol ============
        ss >> node.symbol;
        //cout<< "symbol: "<<node.symbol<<endl;
        
        //=========== number of the node's predecessors ============
        node.in_degree = 0;
        
        //=========== successors ============
        int successor;
        while (ss >> successor) {
            node.successors.push_back(successor);
            //cout<< "successor: "<<successor<<endl;
        }
        
        //=========== Save node into class parameter: graph ============
        graph[index] = node;
    }

    file.close();
}

// Function that constructs the graph by updating predecessors & successors of nodes
void List_Schedule::constructGraph() {
    // Update the information of nodes
    Nodes = graph.size();
    for (auto& it : graph) {
        int index = it.first;
        Node& node = it.second;
        // Initialize the out-degree of the node
        node.out_degree = node.successors.size(); 
        // Initialize the in-degree of the node
        for (int successor : node.successors) {
            graph[successor].in_degree++;
            graph[successor].predecessors.push_back(index);
        }
        // Set the delay time for each node
        if (node.symbol == '+') {
            node.delay = 1;
        } else if (node.symbol == '*') {
            node.delay = 3;
        }else {
            node.delay = 0;
            // input node execute time cycle = 0
            if(node.symbol == 'i'){
              scheduled[0].push_back(index);
            }
        }
    }
}

// Function that performs a topological sort on the graph and returns the sorted node indices
void List_Schedule::topologicalSort(){
    // Initialize a queue with all nodes with out-degree 0
    queue<int> q;
    for (auto& it : graph) {
        int index = it.first;
        Node& node = it.second;
        if (node.out_degree == 0) {
            q.push(index);
            alap[index] = Latency+1;
        }
    }
    // Perform the topological sort & calculate ALAP
    while (!q.empty()) {
        int node_index = q.front();
        q.pop();
        sorted_nodes.push_back(node_index);
        
        // Calculate ALAP for this node
        for (int pred : graph[node_index].predecessors) {
            if(graph[pred].symbol=='i')
              alap[pred]=0;
            else{
              int alap_time=Latency+1;
              for (int succ : graph[pred].successors)
                 alap_time = min(alap_time,alap[succ]);
              alap[pred] = alap_time-graph[pred].delay;
              if(alap[pred]==0)
                canbeschedule=0;
            }
        }

        // Decrement the out-degrees of all predecessors and add them to the queue if they have out-degree 0
        for (int pred : graph[node_index].predecessors) {
            graph[pred].out_degree--;
            if (graph[pred].out_degree == 0) {
                q.push(pred);
            }
        }
    }
}
// Function that performs a minimum resource and latency constraint scheduling
bool List_Schedule::MR_LCS(){
    int t=1;int schedule_ready=0;
    //=========== Can be schedule or not =========== 
    if(canbeschedule==0)
        return false;
      
    //cout<<"============== TIME 0 ================"<<endl;
    vector<int> time0 = scheduled[0];
    for (int nodes : time0) {
        predecessor_ready(nodes);
    }
    
    //====================== Scheduling ====================== 
    for(t=1;t<Latency+1;t++){
        int use_add=0, use_mul=0;
        //================= On_executing ==================
        int NumberinQueue = on_executing_add.size();
        for(int i=0;i<NumberinQueue;i++){
            if(on_executing_add.empty()==1)
                break;
            int node_index = on_executing_add.front();
            on_executing_add.pop();
            graph[node_index].delay--;
            if(graph[node_index].delay==0){
                graph[node_index].execute=2;
                predecessor_ready(node_index);
            }
            else{
                on_executing_add.push(node_index);
                scheduled[t].push_back(node_index);
                use_add++;
            }
        }
        NumberinQueue = on_executing_mul.size();
        for(int i=0;i<NumberinQueue;i++){
            if(on_executing_mul.empty()==1)
                break;
            int node_index = on_executing_mul.front();
            on_executing_mul.pop();
            graph[node_index].delay--;
            if(graph[node_index].delay==0){
                graph[node_index].execute=2;
                predecessor_ready(node_index);
            }
            else{
                on_executing_mul.push(node_index);
                scheduled[t].push_back(node_index);
                use_mul++;
            }
        }
        //================== Must to do ===================
        for(int must_to_do_node : ready_node_add[t]){
            graph[must_to_do_node].execute=1;
            scheduled[t].push_back(must_to_do_node);
            on_executing_add.push(must_to_do_node);
            use_add++;
        }
        //cout<<"\n============== Must to do MUL================"<<endl;
        for(int must_to_do_node : ready_node_mul[t]){
            graph[must_to_do_node].execute=1;
            scheduled[t].push_back(must_to_do_node);
            on_executing_mul.push(must_to_do_node);
            use_mul++;
        }
        //======= Resources are not fully utilized ========
        if(resource[0]<use_add){
            resource[0]=use_add;
        }
        if(resource[1]<use_mul){
            resource[1]=use_mul;
        }
    }
    
}
void List_Schedule::predecessor_ready(int index){
  //in_degree--
  for(int succ :graph[index].successors){
    //one predecessor already
    graph[succ].in_degree=graph[succ].in_degree-1;
    if(graph[succ].in_degree==0){//ready to execute
        if(graph[succ].symbol=='+')
            ready_node_add[alap[succ]].push_back(succ);
        else if(graph[succ].symbol=='*')
            ready_node_mul[alap[succ]].push_back(succ);
    }  
  }
}

void List_Schedule::parser_out(char*argv){
    ofstream file(argv,ios::out);
    if (!file) {
        cerr << "Failed to open file" << endl;
        return;
    }
    file << resource[0]<<endl; //number of adders
    file << resource[1]<<endl; //number of muls
    for (auto const& it : scheduled) {
        int key = it.first;
        if(key==0)continue;
        vector<int> vec = it.second;
        for (int i=0;i<vec.size();i++) {
            file << vec[i] ;
            if(i!=vec.size()-1){
                file << " ";
            }
            
        }
        if(key!=Latency)
        {
            file << endl;
        }
        
    }
}

