#ifndef ASTAR_H
#define ASTAR_H

typedef enum ASTAR_ERR {
    FILE_R_ERR = -10,
    FILE_W_ERR = -11,
    INPUT_ERR = -12,
    ALLOC_ERR = -20,
    LOW_F_ERR = -21,
} err_e;

int a_star(char inputfile[], char outputfile[]);
void printerr(err_e err_no);

#endif // ASTAR_H
