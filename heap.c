#include <stdio.h>
#include <stdlib.h>
#include "heap.h"


int push(heap_t *h, int priority, int data) {
    /* if (h->len + 1 >= h->size) {             // fails for len > m->size*1.5
        h->size = h->size ? h->size*1.1 : 100;
        h->nodes = (node_t*)realloc(h->nodes, h->size*sizeof(node_t));
        if(h->nodes == NULL)
            return -1;
    } */
    int i = h->len + 1;
    int j = i / 2;
    while (i > 1 && h->nodes[j].priority > priority) {
        h->nodes[i] = h->nodes[j];
        i = j;
        j = j / 2;
    }
    h->nodes[i].priority = priority;
    h->nodes[i].data = data;
    h->len++;
    return 0;
}


int pop(heap_t *h) {
    int i, j, k;
    /* if (!h->len) {  // impossible due to only being called right after while(m.queue->len > 0)
        return -1;
    } */
    int data = h->nodes[1].data;
    
    h->nodes[1] = h->nodes[h->len];
    
    h->len--;
    
    i = 1;
    while (i!=h->len+1) {
        k = h->len+1;
        j = 2 * i;
        if (j <= h->len && h->nodes[j].priority < h->nodes[k].priority) {
            k = j;
        }
        if (j + 1 <= h->len && h->nodes[j + 1].priority < h->nodes[k].priority) {
            k = j + 1;
        }
        h->nodes[i] = h->nodes[k];
        i = k;
    }
    return data;
}