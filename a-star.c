#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "a-star.h"

#define numNeighbors 4

typedef struct Map {
    dl_list *open_head, *open_tail;
    int size, start, goal, *came_from, *g_score, *f_score;
    char *grid, *closedset, *openset;
} map;

static int init_map(map *m, char filepath_in[]);
static int alloc_map(map *m);
static void free_map(map *m);
static int lowest_f_score(map *m);
static int h_cost(map *m, int current);
static void neighbor_nodes(int neighbor[], int current, int n);
static int reconstruct_path(map *m);
static int write_map(map *m, char filepath_out[], int exit_status);
static dl_list* dl_push_by_fscore(dl_list **head, dl_list **tail, node_data new_data, int f_score[]);


int a_star(char filepath_in[], char filepath_out[]) {
    int i, tentative_g_score, current, neighbor[numNeighbors];
    map m;

    i = init_map(&m, filepath_in);
    if(i < 0)
        return i;

    m.g_score[m.start] = 0;
    m.f_score[m.start] = m.g_score[m.start] + h_cost(&m, m.start);

    while(m.open_head!=NULL) { // while open_head isn't empty
        current = lowest_f_score(&m); // current=lowest_f and open_head[current]=CLOSED
        if(current < 0) {
            free_map(&m);
            return LOW_F_ERR;
        }

        if(current == m.goal)
            return write_map(&m, filepath_out, GOAL_FOUND); // end program, successful return.

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
                    if(dl_push_by_fscore(&(m.open_head), &(m.open_tail), neighbor[i], m.f_score) == NULL) {
                        free_map(&m);
                        return PUSH_BY_F_ERR;
                    }
                    m.openset[neighbor[i]] = IN_OPENSET; // add neighbor[i] to openset
                }
            }
        }
    }
    return write_map(&m, filepath_out, GOAL_NOT_FOUND);
}


/** Loads the input from filepath into the map.
 * @return Returns i<0 if it errors. */
static int init_map(map *m, char filepath_in[]) {
    ///printf("load()");
    FILE *in = fopen(filepath_in, "r");
    if(in == NULL)
        return FILE_R_ERR;

    fscanf(in, "%d", &(m->size));
    if(m->size < 4 || m->size > 46340) {
        fclose(in);
        return INPUT_ERR;
    }

    if(alloc_map(m) == ALLOC_ERR) {
        fclose(in);
        return ALLOC_ERR;
    }

    m->open_head = NULL;
    m->open_tail = NULL;
    m->start = NOT_SET;
    m->goal = NOT_SET;

    int i=0;
    while(!feof(in)) {
        if(i > m->size*m->size) {
            fclose(in);
            free_map(m);
            return INPUT_ERR;
        }
        fscanf(in, "%c", &m->grid[i]);

        if(m->grid[i] == INVALID_INPUT)
            continue;

        //m->closedset[i] = OPEN; // redundant if using calloc
        //m->openset[i] = NOT_IN_OPENSET; // redundant if using calloc
        m->came_from[i] = NOT_SET;
        m->g_score[i] = INF;
        m->f_score[i] = INF;

        switch(m->grid[i]) {
            case GOAL:
                m->goal = i;
                break;
            case START:
                m->start = i;
                if(dl_push_front(&(m->open_head), &(m->open_tail), i) == NULL) {
                    fclose(in);
                    free_map(m);
                    return PUSH_FRONT_ERR;
                }
                m->openset[i] = IN_OPENSET; // marks the start position as open.
        }
        i++;
    }
    fclose(in);
    if(i < m->size*m->size || m->start == NOT_SET || m->goal == NOT_SET || m->start == m->goal) {
        free_map(m);
        return INPUT_ERR;
    }
    ///printf(" ended successfully.\n");
    return 0;
}


/** Allocates the pointers in map. */
static int alloc_map(map *m) {
    ///printf("\nn:%i n*n:%i\nalloc_map()", m->size, m->size*m->size);

    m->came_from = (int*)malloc(m->size*m->size*sizeof(int));
    if(m->came_from == NULL)
        return ALLOC_ERR;

    m->g_score = (int*)malloc(m->size*m->size*sizeof(int));
    if(m->g_score == NULL) {
        free(m->came_from);
        return ALLOC_ERR;
    }

    m->f_score = (int*)malloc(m->size*m->size*sizeof(int));
    if(m->f_score == NULL) {
        free(m->came_from);
        free(m->g_score);
        return ALLOC_ERR;
    }

    m->grid = (char*)malloc(m->size*m->size*sizeof(char));
    if(m->grid == NULL) {
        free(m->came_from);
        free(m->g_score);
        free(m->f_score);
        return ALLOC_ERR;
    }

    m->closedset = (char*)calloc(m->size*m->size, sizeof(char));
    if(m->closedset == NULL) {
        free(m->came_from);
        free(m->g_score);
        free(m->f_score);
        free(m->grid);
        return ALLOC_ERR;
    }

    m->openset = (char*)calloc(m->size*m->size, sizeof(char));
    if(m->openset == NULL) {
        free(m->came_from);
        free(m->g_score);
        free(m->f_score);
        free(m->grid);
        free(m->closedset);
        return ALLOC_ERR;
    }

    ///printf(" ended successfully.\n");
    return 0;
}


/** Frees the pointers in map. */
static void free_map(map *m) {
    free(m->came_from);
    free(m->g_score);
    free(m->f_score);
    free(m->grid);
    free(m->closedset);
    free(m->openset);
    while(m->open_head != NULL)
        free(dl_pop_front(&(m->open_head), &(m->open_tail)));
}


/** Returns the cost from getting from the start to the goal while marking the path. */
static int reconstruct_path(map *m) {
    ///printf("reconstruct_path()");
    int sum = 0, current_node;
    sum += m->grid[m->goal];
    current_node = m->came_from[m->goal];
    while(m->came_from[current_node] != NOT_SET) {
        sum += m->grid[current_node];
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


/** Returns the position with the lowest f_score in open_head; removes that position from open_head */
static int lowest_f_score(map *m) {
    dl_list *aux = dl_pop_front(&(m->open_head), &(m->open_tail));
    if(aux == NULL)
        return LOW_F_ERR;
    int lowest = aux->data;
    m->openset[lowest] = CLOSED;
    free(aux);
    return lowest;
}


/** Writes the pathfinding cost and m->grid into a ".txt" file. */
static int write_map(map *m, char filepath_out[], int exit_status) {
    int i, cost = exit_status;
    FILE *out_file = fopen(filepath_out, "w");
    if(out_file == NULL) {
        free_map(m);
        return FILE_W_ERR;
    }

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
static dl_list* dl_push_by_fscore(dl_list **head, dl_list **tail, node_data new_data, int f_score[]) {
    dl_list *new_node = (dl_list*)malloc(sizeof(dl_list)), *aux;
    if(new_node == NULL)
        return NULL;

    new_node->data = new_data;

    if(*head == NULL) {
        *head = new_node;
        *tail = new_node;
        new_node->prev = NULL;
        new_node->next = NULL;
        return new_node;
    }

    // finds which of head->data or tail->data is closer to new_data
    if(abs(f_score[(*head)->data]-f_score[new_data]) > abs(f_score[(*tail)->data]-f_score[new_data])) {
        aux = *tail;
        while(aux->prev != NULL && f_score[aux->data] > f_score[new_data])
            aux = aux->prev;
    }
    else {
        aux = *head;
        while(aux->next != NULL && f_score[aux->data] < f_score[new_data])
            aux = aux->next;
    }

    if(f_score[aux->data] >= f_score[new_data]) { // adds new_node as aux->prev
        new_node->prev = aux->prev;
        if(aux->prev != NULL)
            aux->prev->next = new_node;
        aux->prev = new_node;
        new_node->next = aux;
        if(aux == *head) {
            *head = new_node;
            new_node->prev = NULL;
        }
    }
    else { // adds new_node as aux->next
        new_node->next = aux->next;
        if(aux->next != NULL)
            aux->next->prev = new_node;
        aux->next = new_node;
        new_node->prev = aux;
        if(aux == *tail) {
            *tail = new_node;
            new_node->next = NULL;
        }
    }
    return new_node;
}