#ifndef LIST_H
#define LIST_H

/** node_data should be comparable. */
typedef int node_data;

typedef struct Node {
    node_data data;
    struct Node *prev, *next;
} node;

node* push_front(node **head, node_data new_data);
node* push_back(node **head, node_data new_data);
node* pop_front(node **head);
node* pop_back(node **head);
node* findNode(node **head, node_data data);
int removeNode(node **head, node_data data);

#endif // LIST_H
