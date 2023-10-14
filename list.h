#ifndef LIST_H
#define LIST_H

typedef unsigned int node_data;

typedef struct Node {
    node_data data;
    struct Node *prev, *next;
} node;

typedef struct NodeSL{
    node_data data;
    struct NodeSL *next;
} nodeSL_t;

node* addHead(node **head, node_data new_data);
node* addLast(node **head, node_data new_data);
node* findNode(node **head, node_data data);
int removeNode(node **head, node_data data);
int freeHead(node **head);
int freeLast(node **head);

#endif // LIST_H
