#ifndef ASTAR_H
#define ASTAR_H

#define numNeighbors 4

enum STATUS {
    MAX_INT = ~0U >> 1,
    WALL = 100000,
    OPEN = 0,
    CLOSED = 1,
    NOT_FOUND = 0,
    FOUND = 1,
    OUT_OF_BOUNDS = -1,
    NOT_SET = -1,
    INVALID_INPUT = -1,
    LIMITED = 0,
};

typedef enum ASTAR_ERR {
    PATH_ERR = -1,
    FILE_R_ERR = -10,
    FILE_W_ERR = -11,
    INPUT_ERR = -12,
    ALLOC_ERR = -20,
    LOW_F_ERR = -30,
} err_e;

int a_star(char inputfile[], char outputfile[]);
void printerr(err_e err_no);

#endif // ASTAR_H
