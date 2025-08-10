#include "globalRouting.h"

int main(int argc, char *argv[]){
    GlobalRouting globalRouting;
    globalRouting.parser(argv[1], argv[2], argv[3]);
    globalRouting.L_shape(argv[4]);
    return 0;
}
