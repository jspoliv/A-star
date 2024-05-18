#ifndef ASTAR_H
#define ASTAR_H

enum STATUS {
    INF = ~0U >> 1,
    WALL = '#',
    INVALID_INPUT = '\n',
    OUT_OF_BOUNDS = INF,
    NOT_SET = INF,
    OPEN = '0',
    CLOSED = '1',
    IN_OPENSET = 0,
    NOT_IN_OPENSET = 1,
    GOAL_FOUND = 1,
    GOAL_NOT_FOUND = 0,
};

typedef enum ASTAR_ERR {
    FILE_R_ERR = -10,
    FILE_W_ERR = -11,
    INPUT_ERR = -12,
    ALLOC_ERR = -20,
    PUSH_FRONT_ERR = -21,
    LOW_F_ERR = -22,
    PUSH_BY_F_ERR = -23,
} err_e;

int a_star(char inputfile[], char outputfile[]);
void printerr(err_e err_no);

#endif // ASTAR_H
