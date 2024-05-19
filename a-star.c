#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "a-star.h"

#define numNeighbors 4

typedef struct Map {
    dl_list *open_list;
    int size, start, goal, *came_from, *g_score, *f_score;
    char *closedset, *openset, *grid;
} map;

static int new_map(map *m, char filepath_in[]);
static int alloc_map(map *m);
static int lowest_f_score(dl_list **open_list, char openset[]);
static int h_cost(map *m, int current);
static void neighbor_nodes(int neighbor[], int current, int n);
static int reconstruct_path(map *m);
static int write_map(char filepath_out[], map *m, int exit_status);
static dl_list* dl_push_by_fscore(dl_list **head, node_data new_data, int f_score[]);
static sl_list* sl_push_by_fscore(sl_list **head, node_data new_data, int f_score[]);


int a_star(char filepath_in[], char filepath_out[]) {
    int i, tentative_g_score, current, neighbor[numNeighbors];
    map m;

    i = new_map(&m, filepath_in);
    if(i < 0)
        return i;
    
    m.g_score[m.start] = 0;
    m.f_score[m.start] = m.g_score[m.start] + h_cost(&m, m.start);

    while(m.open_list!=NULL) { // while open_list isn't empty
        current = lowest_f_score(&(m.open_list), m.openset); // current=lowest_f and open_list[current]=CLOSED
        if(current < 0)
            return current;
        
        if(current == m.goal)
            return write_map(filepath_out, &m, GOAL_FOUND); // end program, successful return.

        m.closedset[current] = CLOSED;

        neighbor_nodes(neighbor, current, m.size);
        for(i=0; i<numNeighbors; i++) {
            if(neighbor[i] == OUT_OF_BOUNDS || m.grid[neighbor[i]] == WALL) // ignores OUT_OF_BOUNDS positions and walls
                continue;

            tentative_g_score = m.g_score[current] + m.grid[neighbor[i]];
            if(m.closedset[neighbor[i]] == CLOSED && tentative_g_score>=m.g_score[neighbor[i]])
                continue;

            if(m.openset[neighbor[i]] == NOT_IN_OPENSET || tentative_g_score<m.g_score[neighbor[i]]) {
                m.came_from[neighbor[i]] = current;
                m.g_score[neighbor[i]] = tentative_g_score;
                m.f_score[neighbor[i]] = m.g_score[neighbor[i]] + h_cost(&m, neighbor[i]);
                if(m.openset[neighbor[i]] == NOT_IN_OPENSET) {
                    if(dl_push_by_fscore(&(m.open_list), neighbor[i], m.f_score) == NULL) // add neighbor[i] to openset
                        return PUSH_BY_F_ERR;
                    m.openset[neighbor[i]] = IN_OPENSET; // add neighbor[i] to openset
                }
            }
        }
    }
    return write_map(filepath_out, &m, GOAL_NOT_FOUND);
}


/** Loads the input from filepath into the map.
 * @return Returns i<0 if it errors. */
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

    m->open_list = NULL;
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
        m->openset[i] = NOT_IN_OPENSET;
        m->came_from[i] = NOT_SET;
        m->g_score[i] = INF;
        m->f_score[i] = INF;

        switch(m->grid[i]) {
            case 'X':
                m->goal = i;
                break;
            case 'O':
                m->start = i;
                if(dl_push_front(&(m->open_list), i) == NULL) // marks the start position as open.
                    return PUSH_FRONT_ERR;
                m->openset[i] = IN_OPENSET;
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

    m->openset = (char*)malloc(m->size*m->size*sizeof(char));
    if(m->closedset == NULL)
        return ALLOC_ERR;

    m->grid = (char*)malloc(m->size*m->size*sizeof(char));
    if(m->grid == NULL)
        return ALLOC_ERR;

    ///printf(" ended successfully.\n");
    return 0;
}


/** Frees the pointers in map. */
static void free_map(map *m) {
    free(m->came_from);
    free(m->g_score);
    free(m->f_score);
    free(m->closedset);
    free(m->openset);
    free(m->grid);
    while(m->open_list != NULL)
        free(dl_pop_front(&(m->open_list)));
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
    neighbor[0] = current-n>=0 ? current-n : OUT_OF_BOUNDS; // Same column, one line above
    neighbor[1] = current-1>=0 && (current-1)/n == current/n ? current-1 : OUT_OF_BOUNDS; // Same line, one column to the left
    neighbor[2] = current+1<n*n && (current+1)/n == current/n ? current+1 : OUT_OF_BOUNDS; // Same line, one column to the right
    neighbor[3] = current+n<n*n ? current+n : OUT_OF_BOUNDS; // Same column, one line bellow
}


/** Manhattan distance from [current] to [goal]. */
static int h_cost(map *m, int current) { 
    return(abs((m->goal/m->size)-(current/m->size))+abs((m->goal%m->size)-(current%m->size)));
    /* extended version of the return() above.
    int cx, cy, gx, gy; // transforms a position in a N^2 sized array into a [x,y] pair in a NxN matrix
    cx=(current/n); // current's x
    cy=(current%n); // current's y
    gx=(goal/n); // goal's x
    gy=(goal%n); // goal's y
    return(abs(gx-cx)+abs(gy-cy)); */
}


/** Returns the position with the lowest f_score in open_list; removes that position from open_list */
static int lowest_f_score(dl_list **open_list, char openset[]) {
    dl_list *aux = dl_pop_front(open_list);
    if(aux == NULL)
        return LOW_F_ERR;
    int lowest = aux->data;
    openset[lowest] = CLOSED;
    free(aux);
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
    for(i=0; i < m->size*m->size; i++) {
        if((i%m->size)==0) {
            fprintf(out_file, "\n");
        }
        fprintf(out_file, "%c", m->grid[i]);
    }
    fclose(out_file);
    free_map(m);
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
        case PUSH_FRONT_ERR:
            printf("\nError: push_front() failed.\n\n");
            break;
        case PUSH_BY_F_ERR:
            printf("\nError: push_by_f_score() failed.\n\n");
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