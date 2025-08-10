#include "Legalizer.h"

int main(int argc, char* argv[]){
    clock_t start, end;
    start = clock();
    Legalizer legalizer;
    legalizer.parser_info(argv[1]);
    legalizer.parser_opt(argv[2], argv[3]);
    end = clock();
    cout << "Time: " << (double)(end - start) / CLOCKS_PER_SEC << "s" << endl;
    return 0;
}