#ifndef LIST_H
#define LIST_H

typedef unsigned int node_data;

typedef struct Node {
    node_data data;
    struct Node *prev, *next;
} node;

node* addHead(node **head, node_data new_data);
node* addTail(node **head, node_data new_data);
node* findNode(node **head, node_data data);
int removeNode(node **head, node_data data);
int freeHead(node **head);
int freeTail(node **head);

#endif // LIST_H
