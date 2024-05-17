#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "a-star.h"

#define numNeighbors 4

typedef struct Map {
    dl_list *openset;
    int size, *g_score, *f_score;
    int start, goal, *came_from;
    char *closedset, *grid;
} map;

static int new_map(map *m, char filepath_in[]);
static int alloc_map(map *m);
static int lowest_f_score(dl_list **openset);
static int h_cost(int current, int goal, int n);
static void neighbor_nodes(int neighbor[], int current, int n);
static int reconstruct_path(map *m);
static int write_map(char filepath_out[], map *m, int exit_status);
static dl_list* dl_push_by_fscore(dl_list **head, node_data new_data, int f_score[]);
static sl_list* sl_push_by_fscore(sl_list **head, node_data new_data, int f_score[]);


int a_star(char filepath_in[], char filepath_out[]) {
    int i, is_found, tentative_g_score;
    int current, neighbor[numNeighbors];
    map m;

    i = new_map(&m, filepath_in);
    if(i < 0)
        return i;
    
    m.g_score[m.start] = 0;
    m.f_score[m.start] = m.g_score[m.start] + h_cost(m.start, m.goal, m.size);

    while(m.openset!=NULL) { // while openset isn't empty
        current = lowest_f_score(&(m.openset)); // current=lowest_f and openset[current]=CLOSED
        if(current < 0)
            return current;
        
        if(current == m.goal)
            return write_map(filepath_out, &m, GOAL_FOUND); // end program, successful return.

        m.closedset[current] = CLOSED;

        neighbor_nodes(neighbor, current, m.size); // fetches position of current neighbors.
        for(i=0; i<numNeighbors; i++) {
            if(neighbor[i] == OUT_OF_BOUNDS || m.grid[neighbor[i]] == WALL) // ignores OUT_OF_BOUNDS positions and walls
                continue;

            tentative_g_score = m.g_score[current] + m.grid[neighbor[i]];
            if(m.closedset[neighbor[i]] == CLOSED && tentative_g_score>=m.g_score[neighbor[i]])
                continue;

            is_found = dl_findNode(&(m.openset), neighbor[i])==NULL ? NOT_IN_OPENSET : IN_OPENSET;
            if(is_found == NOT_IN_OPENSET || tentative_g_score<m.g_score[neighbor[i]]) {
                m.came_from[neighbor[i]] = current;
                m.g_score[neighbor[i]] = tentative_g_score;
                m.f_score[neighbor[i]] = m.g_score[neighbor[i]] + h_cost(neighbor[i], m.goal, m.size);
                if(is_found == NOT_IN_OPENSET && dl_push_by_fscore(&(m.openset), neighbor[i], m.f_score) == NULL) // if NOT_IN_OPENSET add neighbor[i] to openset
                    return ALLOC_ERR; // push_by_fscore failed to set openset[position] to OPEN
            }
        }
    }
    return write_map(filepath_out, &m, GOAL_NOT_FOUND);
}


/** Loads the input from file into the map. */
static int new_map(map *m, char filepath_in[]) {
    ///printf("load()");
    FILE *in = fopen(filepath_in, "r");
    if(in == NULL)
        return FILE_R_ERR;

    fscanf(in, "%d", &(m->size));
    if(m->size < 4 || m->size > 46340)
        return INPUT_ERR;

    if(alloc_map(m) == ALLOC_ERR)
        return ALLOC_ERR;

    m->openset = NULL;
    m->start = NOT_SET;
    m->goal = NOT_SET;

    int i=0;
    while(!feof(in)) { // Load map.
        if(i > m->size*m->size)
            return INPUT_ERR;
        fscanf(in, "%c", &m->grid[i]);

        if(m->grid[i] == INVALID_INPUT) // Skips invalid inputs
            continue;

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
                if(dl_push_front(&(m->openset), m->start) == NULL) // marks the start position as open.
                    return ALLOC_ERR;
        }
        i++;
    } // End Load Map.

    if(i < m->size*m->size || m->start == NOT_SET || m->goal == NOT_SET || m->start == m->goal)
        return INPUT_ERR;
    fclose(in);
    ///printf(" ended successfully.\n");
    return 0;
} // Function end.


/** Allocates the pointers in map. */
static int alloc_map(map *m) {
    ///printf("\nn:%i n*n:%i\nalloc_map()", m->size, m->size*m->size);

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

    ///printf(" ended successfully.\n");
    return 0;
}


/** Frees the pointers in map. */
static int free_map(map *m) {
    free(m->came_from);
    free(m->g_score);
    free(m->f_score);
    free(m->closedset);
    free(m->grid);
    if(m->openset != NULL)
        return ALLOC_ERR;
    return 0;
}


/** Returns the cost from getting from the start to the exit. */
static int reconstruct_path(map *m) {
    ///printf("reconstruct_path()");
    int sum = 0, current_node = m->goal;
    sum += m->grid[current_node];
    current_node = m->came_from[current_node]; // } skips 'X'
    while(m->came_from[current_node] != NOT_SET) {
        sum += m->grid[current_node];//edge_weight(m->grid[current_node]);
        m->grid[current_node]='*';
        current_node = m->came_from[current_node];
    }
    ///printf(" ended sucessfully.\n");
    return sum;
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
static int lowest_f_score(dl_list **openset) {
    dl_list *aux = dl_pop_front(openset);
    if(aux == NULL)
        return LOW_F_ERR;

    int lowest = aux->data;
    free(aux);
    aux = NULL;
    return lowest;
}


/** Writes the pathfinding cost and m->out into a ".txt" file. */
static int write_map(char filepath_out[], map *m, int exit_status) {
    int i, cost = exit_status;
    FILE *out_file = fopen(filepath_out, "w");
    if(out_file == NULL)
        return FILE_W_ERR;

    if(exit_status == GOAL_FOUND)
        cost = reconstruct_path(m);

    fprintf(out_file, "%d", cost);
    for(i=0; i<m->size*m->size; i++) {
        if((i%m->size)==0) {
            fprintf(out_file, "\n");
        }
        fprintf(out_file, "%c", m->grid[i]);
    }
    fclose(out_file);
    i = free_map(m);
    if(i < 0)
        return i;
    return cost;
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


/** Pushes new_data based on it's f_score; the *head always ends with the lowest f_score.
 * Doubly linked list */
static dl_list* dl_push_by_fscore(dl_list **head, node_data new_data, int f_score[]) {
    dl_list *new_node = (dl_list*)malloc(sizeof(dl_list));
    if(new_node==NULL)
        return NULL;
    new_node->prev = NULL;
    new_node->data = new_data;
    new_node->next = NULL;

    if(*head == NULL) {
        *head = new_node;
    }
    else {
        dl_list *aux = *head;
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

/** Pushes new_data based on it's f_score; the *head always ends with the lowest f_score.
 * Singly linked list. */
static sl_list* sl_push_by_fscore(sl_list **head, node_data new_data, int f_score[]) {
    sl_list *new_node = (sl_list*)malloc(sizeof(sl_list));
    if(new_node==NULL)
        return NULL;
    new_node->data = new_data;
    new_node->next = NULL;

    if(*head == NULL) {
        *head = new_node;
        return new_node;
    }

    sl_list *aux = *head;
    if(f_score[(*head)->data] >= f_score[new_data]) {
        new_node->next = *head;
        *head = new_node;
        return new_node;
    }

    while (aux->next != NULL && f_score[aux->next->data] < f_score[new_data])
            aux = aux->next;
    if(aux->next == NULL) {
        aux->next = new_node;
    } else {
        new_node->next = aux->next;
        aux->next = new_node;
    }
    return new_node;
}