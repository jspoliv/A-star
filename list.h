typedef unsigned int node_data;

typedef struct Node {
    node_data data;
    struct Node *prev, *next;
} node;

node* findNode(node **head, node_data data) {
    node *tmp = *head;
    while(tmp != NULL && tmp->data != data)
        tmp = tmp->next;
    return tmp; // returns NULL if *head is NULL or if data is not found
}

void addLast(node **head, node_data new_data) {
    node *new_node = (node*)calloc(1, sizeof(node));
    node *last = *head;
    new_node->data = new_data;
    new_node->next = NULL; // calloc initializes node->prev and node->next as NULL

    if (*head == NULL) {
        new_node->prev = NULL; // calloc initializes node->prev and node->next as NULL
        *head = new_node;
    }
    else {
        while (last->next != NULL)
            last = last->next;
        last->next = new_node;
        new_node->prev = last;
    }
    return;
}

void freeLast(node **head) {
    node *last = *head;
    if(*head != NULL) {
        if((*head)->next == NULL) { // frees the *head
            free(head);
            *head = NULL;
        }
        else { // finds and frees the last node
            while (last->next != NULL)
                last = last->next;
            last = last->prev;
            free(last->next);
            last->next = NULL;
        }
    }
    return;
}


int removeNode(node **head, node_data data) {
    node *tmp = *head;
    while(tmp != NULL && tmp->data != data)
        tmp = tmp->next;
    if(tmp != NULL) { // data is found
        if(tmp->prev == NULL) { // tmp == head
            *head = tmp->next;
            free(tmp);
        }
        else if(tmp->next != NULL){ // tmp == body
            tmp->prev->next = tmp->next;
            tmp->next->prev = tmp->prev;
            free(tmp);
        }
        else { // tmp == tail end
            tmp = tmp->prev;
            free(tmp->next);
            tmp->next = NULL;
        }
        return 1; // data removed successfully
    }
    return 0; // data not found
}
