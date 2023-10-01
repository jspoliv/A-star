#include <stdio.h>
#include <stdlib.h>
#include "list.h"

#define infinity 10000
#define numNeighbors 4
#define file1 "entrada100.txt"
#define file2 "saida.txt"

int reconstruct_path(int came_from[], int current_node, char map_in[], char map_out[]);
int dist_between(char map_weight);
void neighbor_nodes(int neighbor[], int current, int n);
int lowest_f_score(node **openset, int f_score[]);
int h_cost(int current, int goal, int n);
void load(FILE *in, char map_in[], char map_out[], node **openset, int closedset[], int came_from[], int g_score[], int f_score[], int *goal, int* start);

int main(){
    FILE *in = fopen(file1, "r"), *out;
    int n, n2, i, start, goal, current, tentative_g_score, neighbor[numNeighbors];
    fscanf(in, "%d", &n);
    n2 = n*n;
    printf("n:%i\nn*n:%i\n\n", n, n2);
    int *closedset = (int*)calloc(n2, sizeof(int)), *came_from = (int*)malloc(n2*sizeof(int));
    int *g_score = (int*)malloc(n2*sizeof(int)), *f_score = (int*)malloc(n2*sizeof(int));
    char *map_in = (char*)malloc(n2*sizeof(char)), *map_out = (char*)malloc(n2*sizeof(char));
    node *openset = NULL;
    printf("Mem alloc\n\n");

    /* Variables
    // numNeighbors: number of neighbors, currently a 4-neighborhood.
    // infinity: acts as a pseudo-wall in pathfinding by being magnitudes more expensive than other options.
    // file1 & in: input file.
    // file2 & out: output file.
    // n: X axis value of the map, as the X axis and Y axis have the same value, you can get it only once.
    // n2: n squared
    // start: start position coordinate.
    // goal: goal position coordinate.
    // current: position of the lowest_f_score()
    // openset: the set of tentative nodes to be evaluated, initially containing the start node.
    // closedset: the set of nodes already evaluated.
    // came_from: the map of navigated nodes.
    // g_score: cost from start along best known path.
    // tentative_g_score: g_score[current] + dist_between(map_in[neighbor[i]])
    // f_score: g_score + heuristic cost
    // neighbor: array with the current neighbors' positions.
    // map_in: original map.
    // map_out: final map containing the cost and the way to the exit.
    */

    load(in, map_in, map_out, &openset, closedset, came_from, g_score, f_score, &goal, &start);

    g_score[start] = 0;
    f_score[start] = g_score[start] + h_cost(start, goal, n);

    printf("Main while() start...\n\n");
    while(openset!=NULL){
        current = lowest_f_score(&openset, f_score);
        if(current == goal){ // goal found, execute and exit code
            out = fopen(file2, "w");
            printf("Reconstruct start... ");
            fprintf(out, "%d", reconstruct_path(came_from, goal, map_in, map_out));
            printf("ended sucessfully.\n\n");
            for(i=0; i<n2; i++){
                if(((i%n)==0)){
                    fprintf(out, "\n");
                }
                fprintf(out, "%c", map_out[i]);
            }
            fclose(out);
            printf("Pathfinding was successful.\n");
            // system("pause");
            break; // end program, return successful.
        }

        removeNode(&openset, current);
        closedset[current]=1;

        neighbor_nodes(neighbor, current, n); // fetches position of current neighbors.
        for(i=0; i<numNeighbors; i++){
            if(neighbor[i]!=-1){ // neighbor == -1 is outside the map_in[] scope
                tentative_g_score = g_score[current] + dist_between(map_in[neighbor[i]]);
                if(closedset[neighbor[i]]==1 && tentative_g_score>=g_score[neighbor[i]]){
                    continue;
                }
                if(findNode(&openset, neighbor[i])==NULL || tentative_g_score<g_score[neighbor[i]]){
                    came_from[neighbor[i]] = current;
                    g_score[neighbor[i]] = tentative_g_score;
                    f_score[neighbor[i]] = g_score[neighbor[i]] + h_cost(neighbor[i], goal, n);
                    if(findNode(&openset, neighbor[i])==NULL) {
                        addLast(&openset, neighbor[i]);
                    }
                }
            }
        }
    }
    return 0; // end program
}


/* function load()
// parameters: input file, map_in, map_out, openset, closedset, came_from, g_score, f_score, goal, start, n
//
// The function starts all used variables.
*/
void load(FILE *in, char map_in[], char map_out[], node **openset, int closedset[], int came_from[], int g_score[], int f_score[], int *goal, int* start){
    printf("load start... ");
    int i=0;
    while(!feof(in)){// Load map.
        fscanf(in, "%c", &map_in[i]);
        if(map_in[i]=='X' || map_in[i]=='O' || map_in[i]=='V' || map_in[i]=='W' || map_in[i]=='#'){ // Accepted char set
            // Set other maps.
            // closedset[i]=0; // initialized by calloc
            came_from[i]=-1;
            g_score[i]=10000000; // arbitrarily high number that will be swapped.
            f_score[i]=10000000; // arbitrarily high number that will be swapped.
            map_out[i]=map_in[i];

            switch(map_in[i]){
                case 'X':
                    *goal = i;
                    break;
                case 'O':
                    *start = i;
                    addLast(&(*openset), i); // Mark the start position as open.
                    break;
            }
            i++;
        }
    }// End Load Map.
    fclose(in);
    printf("ended successfully.\n\n");
    return;
} // Function end.


/* function recontruct_path()
// parameters: came_from vector, current position, map, final map.
//
// The function gets the best path from the start to the exit of the map and print it in the final map.
// Calculates recursively the  cost from getting from the start to the exit.
// Returns the cost from getting from the start to the exit.
*/
int reconstruct_path(int came_from[], int current_node, char map_in[], char map_out[]){
    int sum;
    if(came_from[current_node]>-1){
        if(map_in[current_node]!='X'){
            map_out[current_node]='*';
        }
        sum = reconstruct_path(came_from, came_from[current_node], map_in, map_out);
        return(sum + dist_between(map_in[current_node]));
    } else {
        return 0;
    }
}

/* function dist_between()
// parameters: map_in[position].
//
// Maps a char input to return an int.
*/
int dist_between(char map_weight){ // map_in[neighbor]
    switch(map_weight){
        case 'X':
        case 'V':
            return 1;
        case 'W':
            return 2;
        default: // case: '#' || 'O'
            return infinity;
    }
}

void neighbor_nodes(int neighbor[], int current, int n){
    if(current-n>=n){
        neighbor[0]=current-n; // Same column, one line above
    } else {
        neighbor[0]=-1; // Out of bounds
    }
    if(current-1>=n){
        neighbor[1]=current-1; // Same line, one column to the left
    } else {
        neighbor[1]=-1;
    }
    if(current+1<=(n*n)){
        neighbor[2]=current+1; // Same line, one column to the right
    } else {
        neighbor[2]=-1;
    }
    if(current+n<=(n*n)){
        neighbor[3]=current+n; // Same column, one line bellow
    } else {
        neighbor[3]=-1;
    }
}


/* function h_cost()
// parameters: current position, goal position, size of the X axis of the map.
//
// Returns the heuristic cost from position "current" to "goal".
*/
int h_cost(int current, int goal, int n){ // Manhattan distance from [current] to [goal]
    return(abs((goal/n)-(current/n))+abs((goal%n)-(current%n)));
    /* extended version of the return() above.
    int cx, cy, gx, gy; // transforms a position in a N^2 sized array into a [x,y] pair in a NxN matrix
    cx=(current/n); // current's x
    cy=(current%n); // current's y
    gx=(goal/n); // goal's x
    gy=(goal%n); // goal's y
    return(abs(gx-cx)+abs(gy-cy)); */
}


int lowest_f_score(node **openset, int f_score[]){ // position of the lowest value in f_score[]
    int lowest;
    node *tmp = *openset;
    if(tmp!=NULL)
        lowest = tmp->data;
    while(tmp!=NULL){ // starts from the above skip
        if(f_score[tmp->data]<f_score[lowest])
            lowest = tmp->data;
        tmp = tmp->next;
    }
    // printf(" %i", f_score[lowest]);
    return lowest;
}
