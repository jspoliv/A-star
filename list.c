#include <stdio.h>
#include <stdlib.h>
#include "list.h"


node* push_front(node **head, node_data new_data) {
    node *new_node = (node*)malloc(sizeof(node));
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


node* push_back(node **head, node_data new_data) {
    node *new_node = (node*)malloc(sizeof(node));
    if(new_node==NULL)
        return NULL;
    node *aux = *head;
    new_node->data = new_data;
    new_node->next = NULL;

    if (*head == NULL) {
        new_node->prev = NULL;
        *head = new_node;
    }
    else {
        while (aux->next != NULL)
            aux = aux->next;
        aux->next = new_node;
        new_node->prev = aux;
    }
    return new_node;
}


node* pop_front(node **head) {
    node *aux = *head;
    if(*head != NULL) {
        if((*head)->next == NULL) { // if *head is the only node
            *head = NULL;
        }
        else { // if *head isn't the only node
            *head = (*head)->next;
            (*head)->prev = NULL;
        }
    }
    return aux; // returns the popped head
}


node* pop_back(node **head) {
    node *aux = *head;
    if(*head != NULL) {
        if((*head)->next == NULL) { // if *head is the only node
            *head = NULL;
        }
        else { // finds and frees the last node
            while (aux->next != NULL)
                aux = aux->next;
            aux->prev->next = NULL;
        }
    }
    return aux;
}


node* findNode(node **head, node_data data) {
    node *aux = *head;
    while(aux != NULL && aux->data != data)
        aux = aux->next;
    return aux; // returns NULL if data is not found
}


int removeNode(node **head, node_data data) {
    node *aux = *head;
    while(aux != NULL && aux->data != data)
        aux = aux->next;
    if(aux != NULL) { // data is found
        if(aux->prev == NULL) { // aux == head
            *head = (*head)->next;
            if(*head != NULL)
                (*head)->prev = NULL;
        }
        else if(aux->next != NULL) { // aux == body
            aux->prev->next = aux->next;
            aux->next->prev = aux->prev;
        }
        else { // aux == tail end
            aux->prev->next = NULL;
        }
        free(aux);
        aux = NULL;
        return 1; // data removed successfully
    }
    return 0; // data not found
}
