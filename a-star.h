#ifndef ASTAR_H
#define ASTAR_H

#define infinity 10000
#define numNeighbors 4

enum STATUS {
    OPEN = 0,
    CLOSED = 1,
    NOT_FOUND = 0,
    FOUND = 1,
    OUT_OF_BOUNDS = -1,
    NOT_SET = -1,
    INVALID_INPUT = -1,
};

typedef enum ASTAR_ERR {
    FILE_R_ERR = -10,
    FILE_W_ERR = -11,
    INPUT_ERR = -12,
    ALLOC_ERR = -20,
} err_e;

int a_star(char inputfile[], char outputfile[]);
void printerr(err_e err_no);

#endif // ASTAR_H
