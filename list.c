#include <stdio.h>
#include <stdlib.h>
#include "list.h"


node* addHead(node **head, node_data new_data) {
    node *new_node = (node*)malloc(sizeof(node)); // calloc could be used to initialize node->prev and node->next as NULL
    if(new_node==NULL)
        return NULL;
    new_node->prev = NULL;
    new_node->data = new_data;
    new_node->next = *head;
    if(*head != NULL)
        (*head)->prev = new_node;
    *head = new_node;
    return new_node;
}

node* addTail(node **head, node_data new_data) {
    node *new_node = (node*)malloc(sizeof(node)); // calloc could be used to initialize node->prev and node->next as NULL
    if(new_node==NULL)
        return NULL;
    node *last = *head;
    new_node->data = new_data;
    new_node->next = NULL;

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
    return new_node;
}

node* findNode(node **head, node_data data) {
    node *tmp = *head;
    while(tmp != NULL && tmp->data != data)
        tmp = tmp->next;
    return tmp; // returns NULL if *head is NULL or if data is not found
}

int freeTail(node **head) { // returns 1 when removing the last node or when the list is already empty
    node *last = *head;
    if(*head != NULL) {
        if((*head)->next == NULL) { // frees the *head
            free(*head);
            *head = NULL;
            return 1;
        }
        else { // finds and frees the last node
            while (last->next != NULL)
                last = last->next;
            last = last->prev;
            free(last->next);
            last->next = NULL;
            return 0;
        }
    }
    return 1;
}

int freeHead(node **head) { // returns 1 when removing the last node or when the list is already empty
    node *tmp = *head;
    if(*head != NULL) {
        if((*head)->next == NULL) { // if *head is the only node
            free(*head);
            *head = NULL;
            return 1;
        }
        else { // if *head isn't the only node
            *head = (*head)->next;
            (*head)->prev = NULL;
            free(tmp);
            tmp = NULL;
            return 0;
        }
    }
    return 1;
}

int removeNode(node **head, node_data data) {
    node *tmp = *head;
    while(tmp != NULL && tmp->data != data)
        tmp = tmp->next;
    if(tmp != NULL) { // data is found
        if(tmp->prev == NULL) { // tmp == head
            *head = tmp->next;
            if(*head != NULL)
                (*head)->prev = NULL;
            free(tmp);
            tmp = NULL;
        }
        else if(tmp->next != NULL){ // tmp == body
            tmp->prev->next = tmp->next;
            tmp->next->prev = tmp->prev;
            free(tmp);
            tmp = NULL;
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
