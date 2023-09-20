#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define infinity 10000
#define numNeighbors 4
#define file1 "entrada300.txt"
#define file2 "saida.txt"

int reconstruct_path(int came_from[], int current_node, char map_in[], char map_out[]);
int dist_between(int neighbor, char map_in[]);
void neighbor_nodes(int neighbor[], int current, int n);
int notEmpty(int chosenset[], int n2);
int lowest_f_score(int chosenset[], int f_score[], int n2);
int h_cost(int current, int goal, int n);
void Load(FILE *in, char map_in[], char map_out[], int openset[], int closedset[], int came_from[], int g_score[], int f_score[], int *goal, int* start);

int main(){
    clock_t begin, end, begin_r, end_r, begin_m, end_m, begin_f, end_f, begin_low, end_low;
    begin = clock();
    FILE *in = fopen(file1, "r"), *out = fopen(file2, "w");
    int n, n2, i, start, goal, current, tentative_g_score, neighbor[numNeighbors];
    fscanf(in, "%d", &n);
    n2 = n*n;
    printf("n:%i\nn*n:%i\n\n", n, n2);
    int *openset = (int*)malloc(n2*sizeof(int)), *closedset = (int*)malloc(n2*sizeof(int)),
    *came_from = (int*)malloc(n2*sizeof(int)), *g_score = (int*)malloc(n2*sizeof(int)), *f_score = (int*)malloc(n2*sizeof(int));
    char *map_in = (char*)malloc(n2*sizeof(char)), *map_out = (char*)malloc(n2*sizeof(char));
    double sumf = 0, sumlowest = 0;
    end = clock();
    printf("Mem alloc: %fs\n\n",(double)(end - begin) / CLOCKS_PER_SEC);

    /* Variables
    // numNeighbors: number of neighbors, currently a 4-neighborhood.
    // infinity: acts as a pseudo-wall in pathfinding by being magnitudes more expensive than other options.
    // file1 & in: input file.
    // file2 & out: output file.
    // n: X axis value of the map, as the X axis and Y axis have the same value, you can get it only once.
    // n2: n squared
    // start: start position's location.
    // goal: goal position's location.
    // current: position of the lowest_f_score()
    // openset: the set of tentative nodes to be evaluated, initially containing the start node.
    // closedset: the set of nodes already evaluated.
    // came_from: the map of navigated nodes.
    // g_score: cost from start along best known path.
    // tentative_g_score: g_score[current] + dist_between(neighbor[i], map_in)
    // f_score: estimated total cost from start to goal through y.
    // neighbor: array with the current neighbors' positions.
    // map_in: original map.
    // map_out: final map containing the cost and the way to the exit.
    */

    Load(in, map_in, map_out, openset, closedset, came_from, g_score, f_score, &goal, &start);

    g_score[start] = 0;
    f_score[start] = g_score[start] + h_cost(start, goal, n);

    printf("Main while() start...\n\n");
    begin_m = clock();
    //int n_open = n2; // alternative to notEmpty()
    //while(n_open>0){ // alternative to notEmpty()
    while(notEmpty(openset, n2)){
        begin_low = clock();
        current = lowest_f_score(openset, f_score, n2);
        end_low = clock();
        sumlowest += (double)(end_low - begin_low) / CLOCKS_PER_SEC;
        if(current == goal){ // execute and end program
            printf("Reconstruct start... ");
            begin_r = clock();
            fprintf(out, "%d", reconstruct_path(came_from, goal, map_in, map_out));
            end_r = clock();
            printf("ended sucessfully in %lfs.\n\n", (double)(end_r - begin_r) / CLOCKS_PER_SEC);
            for(i=0; i<n2; i++){
                if(((i%n)==0)){
                    fprintf(out, "\n");
                }
                fprintf(out, "%c", map_out[i]);
            }
            fclose(out);
            printf("Pathfinding was successful, look at the output file(.txt) for the map.\n");
            // system("pause");
            break; // end program, return successful.
        }
        openset[current]=0;
        closedset[current]=1;
        //n_open--; // alternative to notEmpty()

        neighbor_nodes(neighbor, current, n); // fetches position of current neighbors.
        for(i=0; i<numNeighbors; i++){
            begin_f = clock();
            if(neighbor[i]!=-1){ // neighbor == -1 is outside the map_in[] scope
                tentative_g_score = g_score[current] + dist_between(neighbor[i], map_in);
                if(closedset[neighbor[i]]==1 && tentative_g_score>=g_score[neighbor[i]]){
                    end_f = clock();
                    sumf += (double)(end_f - begin_f) / CLOCKS_PER_SEC;
                    continue;
                }
                if(openset[neighbor[i]]==0 || tentative_g_score<g_score[neighbor[i]]){
                    came_from[neighbor[i]] = current;
                    g_score[neighbor[i]] = tentative_g_score;
                    f_score[neighbor[i]] = g_score[neighbor[i]] + h_cost(neighbor[i], goal, n);
                    if(openset[neighbor[i]]==0)
                        openset[neighbor[i]]=1;
                }
            }
            end_f = clock();
            sumf += (double)(end_f - begin_f) / CLOCKS_PER_SEC;
        }
    }
    end_m = clock();
    printf("\nExecution time:\n while(): %lfs.\n", (double)(end_m - begin_m) / CLOCKS_PER_SEC);
    printf(" lowest(): %lfs\n", sumlowest);
    printf(" for(): %lfs\n", sumf);
    return 0; // end program
}


/* function Load
// parameters: input file, map_in, map_out, openset, closedset, came_from, g_score, f_score, goal, start, n
//
// The function starts all used variables.
*/
void Load(FILE *in, char map_in[], char map_out[], int openset[], int closedset[], int came_from[], int g_score[], int f_score[], int *goal, int* start){
    printf("load start... ");
    clock_t begin_l, end_l;
    begin_l = clock();
    int i=0;
    while(!feof(in)){// Load map.
        fscanf(in, "%c", &map_in[i]);
        if(map_in[i]=='X' || map_in[i]=='O' || map_in[i]=='V' || map_in[i]=='W' || map_in[i]=='#'){ // Accepted char set
            // Set other maps.
            openset[i]=0;
            closedset[i]=0;
            came_from[i]=-1;
            g_score[i]=10000000; // arbitrarily high number that will be swapped
            f_score[i]=10000000; // arbitrarily high number that will be swapped
            map_out[i]=map_in[i];

            switch(map_in[i]){
                case 'X':
                    *goal = i;
                    break;
                case 'O':
                    *start = i;
                    openset[i]=1; // Set start position to 1 in openset.
                    break;
            }
            /* same as the switch above
            if(map_in[i]=='X'){
                *goal = i;
            } else if(map_in[i]=='O'){
                *start = i;
                openset[i]=1; // Set start position to 1 in openset.
            }*/
            i++;
        }
    }// End Load Map.
    fclose(in);
    end_l = clock();
    printf("ended successfully in %fs.\n\n", (double)(end_l - begin_l) / CLOCKS_PER_SEC);
    return;
} // Function end.


/* function recontruct_path
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
        return(sum + dist_between(current_node, map_in));
    } else {
        return 0;
    }
}

/* function dist_between
// parameters: position of the neighbor, map of characters.
//
// The function calculates the  value of the neighbor based on what that neighbor is.
// Returns the respective value of the neighbor.
*/
int dist_between(int neighbor, char map_in[]){
    if(map_in[neighbor]=='V' || map_in[neighbor]=='X'){
        return 1;
    } else if(map_in[neighbor]=='W'){
        return 2;
    } else/*if(map_in[neighbor]=='#' || map_in[neighbor]=='O')*/{
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


/* function notEmpty
// parameters: chosen set of integers, size of the chosen set
//
// The function checks if the set is empty or not.
// Returns 1 if not empty, 0 if empty.
*/
int notEmpty(int chosenset[], int n2){
    int i;
    for(i=0; i<n2; i++){
        if(chosenset[i]==1){
            return 1; // not empty
        }
    }
    return 0; // empty
}


/* function h_cost
// parameters: current position, goal position, size of the X axis of the map.
//
// The function calculates the heuristic cost from the current to the goal position.
// Returns the heuristic cost from the current to the goal position.
*/
int h_cost(int current, int goal, int n){ // Manhattan distance frim [current] to [goal]
    return(abs((goal/n)-(current/n))+abs((goal%n)-(current%n)));
    /* extended version of the return() above.
    int cx, cy, gx, gy; // transforms a position in a N^2 sized array into a [x,y] pair in a NxN matrix
    cx=(current/n); // current's x
    cy=(current%n); // current's y
    gx=(goal/n); // goal's x
    gy=(goal%n); // goal's y
    return(abs(gx-cx)+abs(gy-cy)); */
}


int lowest_f_score(int chosenset[], int f_score[], int n2){ // position of the lowest value in f_score[]
    int i, lowest;
    for(i=0; chosenset[i]!=1; i++); // skips set[i]!=1
    lowest=i;
    while(i<n2){ // starts from the above skip
        if(chosenset[i]==1 && f_score[i]<f_score[lowest])
            lowest=i;
        i++;
    }
    //static int k=0;
    //k++;
    printf(" %i", f_score[lowest]);
    return lowest;
}
