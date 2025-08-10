#include <iostream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <cmath>

using namespace std;

// Node struct that represents each node in the graph
struct Node{
    string pin;              
    vector<string> successors;   // Indices of successor nodes
    int indegree=0;            // Number of nodes pointing to this node as a successor
    int begin=-1;
    int end=-1;
    bool ready=false;
    int t_index=0;
};
struct H_line{
    string pin;
    int begin;
    int end;
};

class Routing{
private:
    vector<string> topTerminals;
    vector<string> bottomTerminals;
    map<string, Node> vertical_constraint_graph;
    vector<string> left_edge_order;
    vector<H_line> track[50];
    int track_number=1;
    
    void print_vertical_constraint_graph();
public:
    void Parser(char*argv); 
    void Construction();
    void Channel_routing();
    void Outfile(char*argv); 
};

int main(int argc,char*argv[]) {
    Routing pa4;
    pa4.Parser(argv[1]);
    pa4.Construction();
    pa4.Channel_routing();
    pa4.Outfile(argv[2]);
    return 0;
}

void Routing::Parser(char*argv){
    ifstream infile(argv,ios::in);
    if (!infile) {
        cerr << "Failed to open file" << endl;
        return;
    }
    
    string Terminals;
    string pin;
    
    //==============topTerminals=====================
    getline(infile,Terminals);
    stringstream st(Terminals);
    
    // 使用 stringstream 以空格分割字串並存入 topTerminals
    while (st >> pin) {
        topTerminals.push_back(pin);
    }

    
    //==============bottomTerminals=====================
    Terminals.clear();
    getline(infile,Terminals);
    stringstream sb(Terminals);
    
    // 使用 stringstream 以空格分割字串並存入 topTerminals
    while (sb >> pin) {
        bottomTerminals.push_back(pin);
    }
    //cout<<"==========finish Parser============"<<endl;
}

void Routing::Construction(){
    Node pin_node1,pin_node2;
    for (int i=0;i<topTerminals.size();i++) {
        string target1 = topTerminals[i];
        string target2 = bottomTerminals[i];
        
        bool exist1=false,exist2=false;
        for(int j=0;j<left_edge_order.size();j++){
            if(left_edge_order[j]==target1){
              exist1=true;
            }
            if(left_edge_order[j]==target2){
              exist2=true;
            }
            if(exist1==true&&exist2==true)
              break;
        }
        
        //此pin_node1還未擺進left_edge_order
        if(target1 != "0"&& exist1==false){
            left_edge_order.push_back(target1);
        }
        /*cout<<"=========top===========\n";
        cout<<"left_edge_order.size"<<left_edge_order.size()<<endl;
        for (const auto& w : left_edge_order) {
            cout << w << " ";
        }
        cout<<endl;*/
        
        //此pin_node2還未擺進left_edge_order
        if(target2 != "0"&& exist2==false && target1!=target2){
            left_edge_order.push_back(target2);
        }
        /*cout<<"=========bottom===========\n";
        cout<<"left_edge_order.size"<<left_edge_order.size()<<endl;
        for (const auto& w : left_edge_order) {
            cout << w << " ";
        }
        cout<<endl;*/
        
        //construct graph
        bool exist=false;
        if (vertical_constraint_graph.count(target1) > 0){
            //cout << "yes target1!" << endl;
            if(target2!="0"){
                for(int j=0;j<(vertical_constraint_graph[target1].successors).size();j++){
                    if(vertical_constraint_graph[target1].successors[j]==target2){
                        exist=true;
                        break;
                    }    
                }
                //此pin_node2還未是pin_node1的子孫
                if(exist==false){
                    //cout<<"successors -> target2:"<<target2<<endl;
                    (vertical_constraint_graph[target1].successors).push_back(target2);
                }
            }
            vertical_constraint_graph[target1].end=i;
        } else if(target1!="0"){
            //cout << "no target1" << endl;
            //create new graph node
            pin_node1.pin=target1;
            pin_node1.successors.clear();
            if(target2!="0"){
              //cout<<"successors -> target2:"<<target2<<endl;
              pin_node1.successors.push_back(target2);
            }
            pin_node1.begin=i;
            pin_node1.end=i;
            vertical_constraint_graph[target1]=pin_node1;
        }
        
        if (vertical_constraint_graph.count(target2) > 0){
            if(exist==false&&target1!="0"){
                vertical_constraint_graph[target2].indegree++;
            }
            vertical_constraint_graph[target2].end=i;
        } else if(target2!="0"){
            //create new graph node
            pin_node2.pin=target2;
            pin_node2.successors.clear();
            if(target1!="0")
                pin_node2.indegree=1;
            pin_node2.begin=i;
            pin_node2.end=i;
            vertical_constraint_graph[target2]=pin_node2;
        }
        
    }
    /*cout<<"left_edge_order.size"<<left_edge_order.size()<<endl;
    for (const auto& w : left_edge_order) {
        cout << w << " ";
    }
    cout<<endl;
    print_vertical_constraint_graph();
    cout<<"==========finish Construction============"<<endl;*/
}

void Routing::Channel_routing(){
    int ready_routing=0;
    int track_index=0;
    for(int i=0;i<left_edge_order.size();i++){
        if(ready_routing==left_edge_order.size())
            break;
            
        //cout<<"i: "<<i<<endl;
        if(vertical_constraint_graph[left_edge_order[i]].indegree==0&&vertical_constraint_graph[left_edge_order[i]].ready==false){
            track_index=0;
            H_line line;
            line.pin = vertical_constraint_graph[left_edge_order[i]].pin;
            line.begin = vertical_constraint_graph[left_edge_order[i]].begin;
            line.end = vertical_constraint_graph[left_edge_order[i]].end;
            while(true){
                if(track[track_index].empty()){
                    //cout<<"track["<<track_index<<"] is empty"<<endl;
                    //cout<<"track_pin:"<<line.pin<<endl;
                    //cout<<"track_index:"<<track_index<<endl;                   
                    track[track_index].push_back(line);
                    vertical_constraint_graph[left_edge_order[i]].ready=true;
                    vertical_constraint_graph[left_edge_order[i]].t_index=track_index;
                    ready_routing++;
                    for(int k=0;k<vertical_constraint_graph[left_edge_order[i]].successors.size();k++){
                        vertical_constraint_graph[vertical_constraint_graph[left_edge_order[i]].successors[k]].indegree--;
                        if(vertical_constraint_graph[vertical_constraint_graph[left_edge_order[i]].successors[k]].ready==false)
                            vertical_constraint_graph[vertical_constraint_graph[left_edge_order[i]].successors[k]].t_index=track_index+1;
                    }
                    //back to the begin
                    i=-1;
                    break;
                }
                else{
                    bool can_routing=true;
                    for(int j=0;j<track[track_index].size();j++){
                        if(!(line.begin>track[track_index][j].end||line.end<track[track_index][j].begin)){
                            can_routing=false;
                            break;
                        }
                    }
                    if(can_routing&&track_index>vertical_constraint_graph[left_edge_order[i]].t_index){
                        //cout<<"track_pin:"<<line.pin<<endl;
                        //cout<<"track_index:"<<track_index<<endl;
                        track[track_index].push_back(line);
                        vertical_constraint_graph[left_edge_order[i]].ready=true;
                        vertical_constraint_graph[left_edge_order[i]].t_index=track_index;
                        ready_routing++;
                        for(int k=0;k<vertical_constraint_graph[left_edge_order[i]].successors.size();k++){
                            vertical_constraint_graph[vertical_constraint_graph[left_edge_order[i]].successors[k]].indegree--;
                            if(vertical_constraint_graph[vertical_constraint_graph[left_edge_order[i]].successors[k]].ready==false)
                                vertical_constraint_graph[vertical_constraint_graph[left_edge_order[i]].successors[k]].t_index=track_index+1;
                        }
                        //back to the begin
                        i=-1;
                        break;
                    }
                }
                track_index++;
                if(track_index+1>track_number)
                    track_number=track_index+1;
            }
          
        }
        
    }
    
   // cout<<"==========finish Channel_routing============"<<endl;
}

void Routing::Outfile(char*argv){

    ofstream file(argv,ios::out);
    if (!file) {
        cerr << "Failed to open file" << endl;
        return;
    }
    
    //print_vertical_constraint_graph();
    //cout<<"track_number"<<track_number<<endl;
    for(map<string, Node>::iterator it = vertical_constraint_graph.begin(); it != vertical_constraint_graph.end(); ++it){
        //cout <<".begin "<< it->first << endl;
        //cout <<".H "<< it->second.begin <<" "<<track_number-it->second.t_index <<" "<<it->second.end<<endl;
        
        file <<".begin "<< it->first << endl;
        file <<".H "<< it->second.begin <<" "<<track_number-it->second.t_index <<" "<<it->second.end<<endl;
        
        for(int j=it->second.begin;j<=it->second.end;j++){
            if(topTerminals[j]==it->second.pin){
                //cout <<".V "<< j <<" "<<track_number-it->second.t_index <<" "<<track_number+1<<endl;
                file <<".V "<< j <<" "<<track_number-it->second.t_index <<" "<<track_number+1<<endl;
            }
            if(bottomTerminals[j]==it->second.pin){
                //cout <<".V "<< j <<" "<<"0"<<" "<<track_number-it->second.t_index <<endl;
                file <<".V "<< j <<" "<<"0"<<" "<<track_number-it->second.t_index <<endl;
            }
        }
        //cout <<".end\n";
        file <<".end\n";
    }
    
    
    
    //cout<<"==========finish outfile============"<<endl;
}


void Routing::print_vertical_constraint_graph(){
    cout<<"=======================print vertical constraint graph==============================\n";
    for (map<string, Node>::iterator it = vertical_constraint_graph.begin(); it != vertical_constraint_graph.end(); ++it) {
        cout << "Key: " << it->first << endl;
        cout << "Pin: " << it->second.pin << endl;
        cout << "Successors: ";
        for (const std::string& successor : it->second.successors) {
            std::cout << successor << " ";
        }
        cout << std::endl;
        cout << "In-degree: " << it->second.indegree << endl;
        cout << "Begin: " << it->second.begin << endl;
        cout << "End: " << it->second.end << endl;
        cout << "Ready: " << boolalpha << it->second.ready << endl;
        cout << "Track index: " << it->second.t_index << endl;
        cout << "==========================" << endl;
    }
}
