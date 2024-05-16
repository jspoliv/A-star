#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "a-star.h"

#define numNeighbors 4

enum STATUS {
    INF = ~0U >> 1,
    WALL = 100000,
    NOT_SET = INF,
    INVALID_INPUT = INF,
    OUT_OF_BOUNDS = INF,
    OPEN = '0',
    CLOSED = '1',
    IN_OPENSET = 0,
    NOT_IN_OPENSET = 1,
    GOAL_FOUND = 0,
    GOAL_NOT_FOUND = 1,
};

typedef struct Map {
    node *openset;
    int size, *g_score, *f_score;
    int start, goal, *came_from;
    char *closedset, *grid;
} map;

static int alloc_map(map *m);
static int load(FILE *in, map *m);
static int edge_weight(char input);
static int lowest_f_score(node **openset);
static int h_cost(int current, int goal, int n);
static void neighbor_nodes(int neighbor[], int current, int n);
static int reconstruct_path(int came_from[], int current_node, char grid[]);
static int write_map(char out_path[], map *m, int exit_status);
static node* push_by_fscore(node **head, node_data new_data, int f_score[]);



int a_star(char in_path[], char out_path[]) {
    int i, is_found, tentative_g_score;
    int current, neighbor[numNeighbors];
    FILE *in_file;
    map m;
    err_e err_no;

    in_file = fopen(in_path, "r");
    if(in_file == NULL)
        return FILE_R_ERR;
    
    fscanf(in_file, "%d", &m.size);
    if(m.size < 4 || m.size > 46340)
        return INPUT_ERR;

    err_no = alloc_map(&m);
    if(err_no < 0)
        return err_no;

    err_no = load(in_file, &m);
    if(err_no < 0)
        return err_no;
    
    m.g_score[m.start] = 0;
    m.f_score[m.start] = m.g_score[m.start] + h_cost(m.start, m.goal, m.size);

    // printf("while(m.openset!=NULL)\n\n");
    while(m.openset!=NULL) { // while openset isn't empty
        current = lowest_f_score(&(m.openset)); // current=lowest_f and openset[current]=CLOSED
        if(current < 0)
            return current;
        
        if(current == m.goal)
            return write_map(out_path, &m, GOAL_FOUND); // end program, successful return.

        m.closedset[current] = CLOSED;

        neighbor_nodes(neighbor, current, m.size); // fetches position of current neighbors.
        for(i=0; i<numNeighbors; i++) {
            if(neighbor[i] == OUT_OF_BOUNDS || m.grid[neighbor[i]] == '#') // ignores OUT_OF_BOUNDS positions and '#'
                continue;

            tentative_g_score = m.g_score[current] + edge_weight(m.grid[neighbor[i]]);
            if(m.closedset[neighbor[i]] == CLOSED && tentative_g_score>=m.g_score[neighbor[i]])
                continue;

            is_found = findNode(&(m.openset), neighbor[i])==NULL ? NOT_IN_OPENSET : IN_OPENSET;
            if(is_found == NOT_IN_OPENSET || tentative_g_score<m.g_score[neighbor[i]]) {
                m.came_from[neighbor[i]] = current;
                m.g_score[neighbor[i]] = tentative_g_score;
                m.f_score[neighbor[i]] = m.g_score[neighbor[i]] + h_cost(neighbor[i], m.goal, m.size);
                if(is_found == NOT_IN_OPENSET && push_by_fscore(&(m.openset), neighbor[i], m.f_score) == NULL) // if NOT_IN_OPENSET add neighbor[i] to openset
                    return ALLOC_ERR; // push_by_fscore failed to set openset[position] to OPEN
            }
        }
    }
    return write_map(out_path, &m, GOAL_NOT_FOUND);
}


/** Allocates the pointers in map. */
static int alloc_map(map *m) {
    printf("\nn:%i n*n:%i\nalloc_map()", m->size, m->size*m->size);
    m->openset = NULL;
    m->start = NOT_SET;
    m->goal = NOT_SET;

    m->came_from = (int*)malloc(m->size*m->size*sizeof(int));
    if(m->came_from == NULL)
        return ALLOC_ERR;

    m->g_score = (int*)malloc(m->size*m->size*sizeof(int));
    if(m->g_score == NULL)
        return ALLOC_ERR;

    m->f_score = (int*)malloc(m->size*m->size*sizeof(int));
    if(m->f_score == NULL)
        return ALLOC_ERR;

    m->closedset = (char*)malloc(m->size*m->size*sizeof(char));
    if(m->closedset == NULL)
        return ALLOC_ERR;

    m->grid = (char*)malloc(m->size*m->size*sizeof(char));
    if(m->grid == NULL)
        return ALLOC_ERR;

    printf(" ended successfully.\n");
    return 0;
}


/** Loads the input from *in into the map. */
static int load(FILE *in, map *m) {
    printf("load()");
    int i=0;
    while(!feof(in)) { // Load map.
        if(i > m->size*m->size)
            return INPUT_ERR;
        fscanf(in, "%c", &m->grid[i]);
        if(edge_weight(m->grid[i]) != INVALID_INPUT) { // Skips invalid inputs
            m->closedset[i] = OPEN;
            m->came_from[i] = NOT_SET;
            m->g_score[i] = INF;
            m->f_score[i] = INF;

            switch(m->grid[i]) {
                case 'X':
                    m->goal = i;
                    break;
                case 'O':
                    m->start = i;
                    if(push_front(&(m->openset), m->start) == NULL) // push_front marks the start position as open.
                        return ALLOC_ERR;
            }
            i++;
        }
    } // End Load Map.
    if(i < m->size*m->size || m->start == NOT_SET || m->goal == NOT_SET || m->start == m->goal)
        return INPUT_ERR;
    fclose(in);
    printf(" ended successfully.\n");
    return 0;
} // Function end.


/** Returns the cost from getting from the start to the exit. */
static int reconstruct_path(int came_from[], int current_node, char grid[]) {
    // the starting current_node should be 'X'
    int sum = 0;
    // if(came_from[current_node] != NOT_SET) { // should be true by definition
    sum += edge_weight(grid[current_node]);
    current_node = came_from[current_node]; // } skips 'X'
    while(came_from[current_node] != NOT_SET) {
        sum += edge_weight(grid[current_node]);
        grid[current_node]='*';
        current_node = came_from[current_node];
    }
    return sum;
}


/** Checks the weight for the input char.
 * @param input char to evaluate, grid[position].
 * @return the corresponding weight for the input char or INVALID_INPUT. */
static int edge_weight(char input) {
    switch (input) {
    case 'X':
        return 1;
    case '#':
    case 'O':
        return WALL;
    case '\n':
    case '*':
        return INVALID_INPUT;
    default:
        return input-47;
    }
}


/** Receives the position of the current node's neighbors.
 * @param neighbor array that receives the positions around current.
 * @param current position of the current node.
 * @param n N value of the NxN map. */
static void neighbor_nodes(int neighbor[], int current, int n) {
    neighbor[0] = (current-n>=n) ? current-n : OUT_OF_BOUNDS; // Same column, one line above

    neighbor[1] = (current-1>=n) ? current-1 : OUT_OF_BOUNDS; // Same line, one column to the left

    neighbor[2] = (current+1<=(n*n)) ? current+1 : OUT_OF_BOUNDS; // Same line, one column to the right

    neighbor[3] = (current+n<=(n*n)) ? current+n : OUT_OF_BOUNDS; // Same column, one line bellow
}


/** Heuristic cost for f_score
 * @param current current position.
 * @param goal goal position.
 * @param n N value of the NxN map.
 * @return the Manhattan distance from [current] to [goal]. */
static int h_cost(int current, int goal, int n) { 
    return(abs((goal/n)-(current/n))+abs((goal%n)-(current%n)));
    /* extended version of the return() above.
    int cx, cy, gx, gy; // transforms a position in a N^2 sized array into a [x,y] pair in a NxN matrix
    cx=(current/n); // current's x
    cy=(current%n); // current's y
    gx=(goal/n); // goal's x
    gy=(goal%n); // goal's y
    return(abs(gx-cx)+abs(gy-cy)); */
}


/** Returns the position with the lowest f_score in openset; removes that position from openset */
static int lowest_f_score(node **openset) {
    node *aux = pop_front(openset);
    if(aux == NULL)
        return LOW_F_ERR;

    int lowest = aux->data;
    free(aux);
    aux = NULL;
    return lowest;
}


/** Writes the pathfinding cost and m->out into a ".txt" file. */
static int write_map(char out_path[], map *m, int exit_status) {
    FILE *out_file = fopen(out_path, "w");
    int cost = INF, i;
    if(out_file == NULL)
        return FILE_W_ERR;
    if(exit_status == GOAL_FOUND) {
        printf("reconstruct_path()");
        cost = reconstruct_path(m->came_from, m->goal, m->grid);
        printf(" ended sucessfully.\n\nPathfinding was successful.\n");
    }
    else {
        printf("\nPathfinding failed.\n");
    }
    fprintf(out_file, "%d", cost);
    for(i=0; i<m->size*m->size; i++) {
        if((i%m->size)==0) {
            fprintf(out_file, "\n");
        }
        fprintf(out_file, "%c", m->grid[i]);
    }
    fclose(out_file);
    printf("Path cost: %d\n", cost);
    // system("pause");
    return 0;
}


void printerr(err_e err_no) {
    switch(err_no) {
        case FILE_R_ERR:
            printf("\nError: Input file not found.\n\n");
            break;
        case FILE_W_ERR:
            printf("\nError: Output file could not be written.\n\n");
            break;
        case INPUT_ERR:
            printf("\nError: Invalid input.\n\n");
            break;
        case ALLOC_ERR:
            printf("\nError: Memory allocation failed.\n\n");
            break;
        case LOW_F_ERR:
            printf("\nError: lowest_fscore() failed.\n\n");
    }
}

/** Pushes new_data based on it's f_score; the *head always ends with the lowest f_score. */
static node* push_by_fscore(node **head, node_data new_data, int f_score[]) {
    node *new_node = (node*)malloc(sizeof(node));
    if(new_node==NULL)
        return NULL;
    new_node->prev = NULL;
    new_node->data = new_data;
    new_node->next = NULL;

    if(*head == NULL) {
        *head = new_node;
    }
    else {
        node *aux = *head;
        while(aux->next != NULL && f_score[aux->data] < f_score[new_data])
            aux = aux->next;
        if(f_score[aux->data] >= f_score[new_data]) { // adds new_node as aux->prev
            new_node->prev = aux->prev;
            if(aux->prev != NULL)
                aux->prev->next = new_node;
            aux->prev = new_node;
            new_node->next = aux;
            if(aux == *head)
                *head = new_node;
        }
        else { // adds new_node as aux->next
            new_node->next = aux->next;
            if(aux->next != NULL)
                aux->next->prev = new_node;
            aux->next = new_node;
            new_node->prev = aux;
        }
    }
    return new_node;
}