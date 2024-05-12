#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "a-star.h"

#define file1 "entrada100.txt"
#define file2 "saida.txt"

int main(int argc, char** argv) {
    clock_t begin, end;
    err_e err = 0;

    begin = clock();
    if(argc == 3) {
        err = a_star(argv[1], argv[2]);
    } else {
        err = a_star(file1, file2);
    }
    end = clock();
    if(err < 0)
        printerr(err);

    printf("Time elapsed: %fs\n\n", (double)(end - begin) / CLOCKS_PER_SEC);

    return err;
}
