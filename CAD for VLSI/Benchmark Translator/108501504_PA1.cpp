#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

class v2bench{
private:
    vector<string> pattern={"and","nand","or","nor","xor","xnor","buf","not"}; //present the gate type
    string module_name;
    vector<int> gate_counts={0,0,0,0,0,0,0,0};//0->and,1->nand,2->or,3->nor,4->xor,5->xnor,6->buff,7->not
    vector<string> input_ports; //input ports
    vector<string> output_ports; //output ports
    vector<string> gate_expression;
    int total_gate = 0;
public:
    void parser_in(char*argv);
    void transfer_v2bench();
    void parser_out(char*argv);

};



int main(int argc, char *argv[])
{
    v2bench pa1;
    pa1.parser_in(argv[1]);
    pa1.transfer_v2bench();
    pa1.parser_out(argv[2]);
    return 0;
}

void v2bench::parser_in(char*argv){

    string garbage;
    string all_input={};
    string all_output={};
    string str;
    string gate;
    string token;

    ifstream openv(argv,ios::in);

    if(!openv)
    {
        cerr << "v could not be opened." << endl;
        exit(1);
    }
    openv >> garbage; //eat module
    openv >> module_name; //get module name
    do{
        getline(openv, garbage);
    }while(garbage.find(";")==string::npos);

    //===================== Deal with input ports =====================
    openv >> garbage; //eat "input"
    do{
        getline(openv, str);
        all_input+=str;
    }while(all_input.find(";")==string::npos);

    stringstream ssi(all_input);
    //put each input port into the private vector input ports
    while (getline(ssi, token, ',')) {
        int a = token.find("N");
        if(a!=0){
            token=token.substr(a);
        }
        input_ports.push_back(token);
    }
    //deal with the last ;
    string final_input = input_ports[input_ports.size()-1];
    input_ports.pop_back();
    remove(final_input.begin(), final_input.end(), ';');
    input_ports.push_back(final_input);
    transform(input_ports.begin(), input_ports.end(), input_ports.begin(),[](string& s) {
                   s.erase(remove(s.begin(), s.end(), 'N'), s.end());
                   return s;
                   });
                   
    //for(int i=0;i<input_ports.size();i++)
    //  cout<<input_ports[i]<<endl;
                   
    //===================== Deal with output ports =====================
    openv >> garbage; //eat "output"
    do{
        getline(openv, str);
        all_output+=str;
    }while(all_output.find(";")==string::npos);
    stringstream sso(all_output);
    ////put each input port into the private vector output ports
    while (getline(sso, token, ',')) {
        int a = token.find("N");
        if(a!=0){
            token=token.substr(a);
        }
        output_ports.push_back(token);
    }
    //deal with the last ;
    string final_output = output_ports[output_ports.size()-1];
    output_ports.pop_back();
    remove(final_output.begin(), final_output.end(), ';');
    output_ports.push_back(final_output);

    transform(output_ports.begin(), output_ports.end(), output_ports.begin(),[](string& s) {
                   s.erase(remove(s.begin(), s.end(), 'N'), s.end());
                   return s;
                   });
    //for(int i=0;i<output_ports.size();i++)
    //  cout<<output_ports[i]<<endl;

    //===================== Deal with wires =====================
    do{getline(openv, garbage);}while(garbage.find(";")==string::npos);
    getline(openv, garbage);
    
    //cout<<"wires"<<endl;

    //===================== Deal with gates =====================
    //Calculate the number of each logic gate
    while (getline(openv, gate)) {
        if (gate.empty()) {
            break;
        }
        gate_expression.push_back(gate);
        //cout << gate << endl;
        string GATE = gate.substr(0, gate.find(" "));
        //cout << GATE << endl;
        for (int i=0;i<8;i++) {
          //cout << GATE << "    " << pattern[i] << endl;
            if (GATE==pattern[i]) {
                gate_counts[i]++;
            }
        }
    }
    
    for(int i=0;i<gate_counts.size();i++)
      total_gate+=gate_counts[i];
    cout<<"total_gate : "<< total_gate<<endl;

}

void v2bench::transfer_v2bench(){
    for(int i=0;i<total_gate;i++){
        if (gate_expression[i].empty()) {
            break;
        }
        string Gateline = gate_expression[i];

        //=========== get gate name ===========
        stringstream gate(Gateline);
        string Gate,garbage,wireline;
        gate >> Gate;
        gate >> garbage;
        wireline = gate.str().substr(gate.tellg());
        
        //============ get all wire name ============
        vector<string> nums;
        // 在字串中尋找匹配的字串
        size_t pos = wireline.find("(", 0);
        while (pos != string::npos) {
            // 尋找 N 的位置
            pos = wireline.find('N', pos);
            if (pos != string::npos) {
                // 找到了 N，接著找數字的結束位置
                size_t num_start = pos + 1;
                size_t num_end = wireline.find_first_not_of("0123456789", num_start);
                
                if (num_end == string::npos) {
                    num_end = wireline.size();
                }
                // 提取數字並存入 vector 中
                string NUM = wireline.substr(num_start, num_end - num_start);
                nums.push_back(NUM);
        
                // 繼續尋找下一個 N 的位置
                pos = wireline.find('N', num_end);
            }
        }

        string expression = nums[0] + " = ";

        if(Gate == "nand")
            expression += "NAND(";
        else if(Gate == "and")
            expression += "AND(";
        else if(Gate == "xnor")
            expression += "XNOR(";
        else if(Gate == "nor")
            expression += "NOR(";
        else if(Gate == "xor")
            expression += "XOR(";
        else if(Gate == "or")
            expression += "OR(";
        else if(Gate == "buf")
            expression += "BUFF(";
        else if(Gate == "not")
            expression += "NOT(";
        else
            expression += "FAULT(";


        // 印出所有存進vector中的數字
        for (int i=1;i<nums.size();i++) {
            expression += nums[i];
            //cout<<expression<<endl;
            if(i!=nums.size()-1)
                expression +=", ";
            else
                expression +=")";

        }
        gate_expression[i] = expression;
    }
}

void v2bench::parser_out(char*argv){
    ofstream openbench(argv,ios::out);

    if(!openbench)
    {
        cerr << "bench could not be opened." << endl;
        exit(1);
    }

    //================== Module name ==================
    openbench<<"# "<<module_name<<endl;

    //================== Input ports ==================
    openbench<<"# "<<input_ports.size();
    if(input_ports.size()==0 | input_ports.size()==1)
        openbench<<" input"<<endl;
    else
        openbench<<" inputs"<<endl;

    //================== Output ports ==================
    openbench<<"# "<<output_ports.size();
    if(output_ports.size()==0 | output_ports.size()==1)
        openbench<<" output"<<endl;
    else
        openbench<<" outputs"<<endl;

    //================== Inverter ==================
    if (gate_counts[7] != 0) {//gate_counts[7]->inverter
        openbench << "# " << gate_counts[7];
        if (gate_counts[7] == 1)
            openbench << " inverter" << endl;
        else
            openbench << " inverters" << endl;
    } else {
        openbench << "# 0 inverter" << endl;
    }

    //================== Gate ========================
    openbench << "# " << total_gate - gate_counts[7];
    if (total_gate - gate_counts[7] == 0 || total_gate - gate_counts[7] == 1)
        openbench << " gate ( ";
    else
        openbench << " gates ( ";

    //0->and,1->nand,2->or,3->nor,4->xor,5->xnor,6->buff

    if(gate_counts[0]!=0){
        openbench << gate_counts[0] ;
        if(gate_counts[0]!=1)
            openbench <<" ANDs ";
        else
            openbench <<" AND ";
    }
    if(gate_counts[1]!=0){
        if(gate_counts[0]!=0)
            openbench <<"+ ";
        openbench << gate_counts[1];
        if(gate_counts[1]!=1)
            openbench <<" NANDs ";
        else
            openbench <<" NAND ";
    }
    if(gate_counts[2]!=0){
        if((gate_counts[0]+gate_counts[1])!=0)
            openbench <<"+ ";
        openbench << gate_counts[2];
        if(gate_counts[2]!=1)
            openbench <<" ORs ";
        else
            openbench <<" OR ";
    }
    if(gate_counts[3]!=0){
        if((gate_counts[0]+gate_counts[1]+gate_counts[2])!=0)
            openbench <<"+ ";
        openbench << gate_counts[3] ;
        if(gate_counts[3]!=1)
            openbench <<" NORs ";
        else
            openbench <<" NOR ";
    }
    if(gate_counts[4]!=0){
        if((gate_counts[0]+gate_counts[1]+gate_counts[2]+gate_counts[3])!=0)
            openbench <<"+ ";
        openbench << gate_counts[4] ;
        if(gate_counts[4]!=1)
            openbench <<" XORs ";
        else
            openbench <<" XOR ";
    }
    if(gate_counts[5]!=0){
        if((gate_counts[0]+gate_counts[1]+gate_counts[2]+gate_counts[3]+gate_counts[4])!=0)
            openbench <<"+ ";
        openbench << gate_counts[5] ;
        if(gate_counts[5]!=1)
            openbench <<" XORs ";
        else
            openbench <<" XOR ";
    }
    if(gate_counts[6]!=0){
        if((gate_counts[0]+gate_counts[1]+gate_counts[2]+gate_counts[3]+gate_counts[4]+gate_counts[5])!=0)
            openbench <<"+ ";
        openbench << gate_counts[6] ;
        if(gate_counts[6]!=1)
            openbench <<" XORs ";
        else
            openbench <<" XOR ";
    }
    openbench << ")"<<endl<<endl;
    
    for(int i=0;i<input_ports.size();i++){
        cout << input_ports[i]<<endl;
    }

    
    //================== Input ========================
    size_t last_non_control_char = input_ports[input_ports.size()-1].find_last_not_of("\n\r");
    if (last_non_control_char != string::npos) {
        input_ports[input_ports.size()-1].erase(last_non_control_char + 1);
    }
    for(int i=0;i<input_ports.size();i++){
        openbench << "INPUT("<<input_ports[i]<<")"<<endl;
    }
    openbench <<endl;

    last_non_control_char = output_ports[output_ports.size()-1].find_last_not_of("\n\r");
    if (last_non_control_char != string::npos) {
        output_ports[output_ports.size()-1].erase(last_non_control_char + 1);
    }
    //================== Output ========================
    for(int i=0;i<output_ports.size();i++){
        openbench << "OUTPUT("<<output_ports[i]<<")"<<endl;
    }
    openbench <<endl;
    cout <<endl;
    //================== Gate ========================
    for(int i=0;i<total_gate;i++){
        openbench << gate_expression[i] <<endl;
    }
    openbench.close();
}