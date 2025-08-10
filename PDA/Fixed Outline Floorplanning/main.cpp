#include "FP.h"

int main(int argc, char *argv[]){
    FP fp;
    fp.start = clock();
    fp.alpha = std::atof(argv[1]);
    fp.parser_block(argv[2]);
    fp.parser_net(argv[3]);

    double initial_temperature = 1000000000000000000.0;  // 初始溫度
    double cooling_rate = 0.999;  // 降溫率
    int max_iterations = 1000000000;  // 最大迭代次數
    do{
        fp.SA_initial(initial_temperature, cooling_rate, max_iterations);
        clock_t temp_start = clock();
        // if the time is over 295s, break
        if (static_cast<double>(temp_start - fp.start)/ CLOCKS_PER_SEC > 295){
            cout << "time > 295s ==> break" <<endl;
            fp.output_file(argv[4]);
            return 0;
        }
    }while(fp.in_flag == 0);


    fp.SA_detail(initial_temperature, cooling_rate, max_iterations);
    
    cout<<"==== finish ====\n";
    fp.output_file(argv[4]);
    return 0;
}