typedef int node_data;

typedef struct Node {
    node_data data;
    struct Node *prev, *next;
} node;

node* findNode(node **head, node_data data) {
    //printf("FIND[%i]", data);
    node *tmp = *head;
    if(*head != NULL) {
        while(tmp != NULL && tmp->data != data) {
            tmp = tmp->next;
        }
        //tmp == NULL ? printf("NOT ") : printf("FOUND ");
        return tmp;
    }
    //printf("HEADNULL ");
    return NULL;
}

void addLast(node **head, node_data new_data) {
    node *new_node = (node*)calloc(1, sizeof(node)); // calloc initializes node->prev and node->next as NULL
    node *last = *head;
    new_node->data = new_data;
    new_node->next = NULL;

    //printf("ADD[%i]", new_data);
    //if(findNode(&(*head), new_data) != NULL)
    //    return;

    if (*head == NULL) {
        new_node->prev = NULL;
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
        if((*head)->next == NULL) {
            free(head);
            *head = NULL;
        }
        else {
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
    if(*head != NULL) {
        while(tmp != NULL && tmp->data != data)
            tmp = tmp->next;
        if(tmp != NULL) { // data is found
            if(tmp->prev == NULL) { // if tmp == head
                *head = tmp->next;
                free(tmp);
                //printf("REMOVE[%i]HEAD", data);
            }
            else if(tmp->next != NULL){ // tmp == body
                tmp->prev->next = tmp->next;
                tmp->next->prev = tmp->prev;
                free(tmp);
                //printf("REMOVE[%i]BODY", data);
            }
            else { // tmp == tail end
                tmp = tmp->prev;
                free(tmp->next);
                tmp->next = NULL;
                //printf("REMOVE[%i]TAIL", data);
            }
            return 1;
        }
    }
    return 0;
}

