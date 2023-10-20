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
};

int a_star(char inputfile[], char outputfile[]);

#endif // ASTAR_H
