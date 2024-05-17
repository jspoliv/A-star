#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "a-star.h"

#define file1 "entrada100.txt"
#define file2 "saida.txt"

int main(int argc, char** argv) {
    clock_t begin, end;
    int result;

    begin = clock();
    if(argc == 3) {
        result = a_star(argv[1], argv[2]);
    } else {
        //for(int i=0; i<1000 && a_star(file1, file2) > 0; i++);
        result = a_star(file1, file2);
    }
    end = clock();

    if(result > 0)
        printf("Pathfinding was successful.\n");
    else if(result == GOAL_NOT_FOUND)
        printf("Pathfinding failed.\n");
    printf("Path cost: %d\nTime elapsed: %fs\n\n", result, ((double)(end - begin) / CLOCKS_PER_SEC));
    
    if(result < 0)
        printerr(result);

    return 0;
}
