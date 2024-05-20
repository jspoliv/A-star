#include <stdio.h>
#include <stdlib.h>
#include "list.h"


dl_list* dl_push_by_data(dl_list **head, dl_list **tail, node_data new_data) {
    dl_list *new_node = (dl_list*)malloc(sizeof(dl_list)), *aux;
    if(new_node==NULL)
        return NULL;
    new_node->prev = NULL;
    new_node->data = new_data;
    new_node->next = NULL;

    if(*head == NULL) {
        *head = new_node;
        *tail = new_node;
        return new_node;
    }

    if(abs((*head)->data-new_data) > abs((*tail)->data-new_data)) {
        aux = *tail;
        while(aux->prev != NULL && aux->data > new_data)
            aux = aux->prev;
    }
    else {
        aux = *head;
        while(aux->next != NULL && aux->data < new_data)
            aux = aux->next;
    }

    if(aux->data >= new_data) { // adds new_node as aux->prev
        new_node->prev = aux->prev;
        if(aux->prev != NULL)
            aux->prev->next = new_node;
        aux->prev = new_node;
        new_node->next = aux;
        if(aux == *head)
            *head = new_node;
    }
    else { // adds new_node as aux->next
        new_node->next = aux->next;
        if(aux->next != NULL)
            aux->next->prev = new_node;
        aux->next = new_node;
        new_node->prev = aux;
        if(aux == *tail)
            *tail = new_node;
    }
    return new_node;
}


dl_list* dl_push_front(dl_list **head, dl_list **tail, node_data new_data) {
    dl_list *new_node = (dl_list*)malloc(sizeof(dl_list));
    if(new_node==NULL)
        return NULL;
    new_node->prev = NULL;
    new_node->data = new_data;
    new_node->next = *head;

    if(*head != NULL)
        (*head)->prev = new_node;
    else
        *tail = new_node;

    *head = new_node;
    return new_node;
}


dl_list* dl_pop_front(dl_list **head, dl_list **tail) {
    dl_list *aux = *head;
    if(*head == NULL)
        return NULL;

    if((*head)->next == NULL) { // if *head is the only node
        *head = NULL;
        *tail = NULL;
    }
    else { // if *head isn't the only node
        *head = (*head)->next;
        (*head)->prev = NULL;
    }
    return aux; // returns the popped head
}


dl_list* dl_findNode(dl_list **head, node_data data) {
    dl_list *aux = *head;
    while(aux != NULL && aux->data != data)
        aux = aux->next;
    return aux; // returns NULL if data is not found
}


dl_list* dl_push_back(dl_list **head, node_data new_data) {
    dl_list *new_node = (dl_list*)malloc(sizeof(dl_list));
    if(new_node==NULL)
        return NULL;
    dl_list *aux = *head;
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


dl_list* dl_pop_back(dl_list **head) {
    dl_list *aux = *head;
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


int dl_removeNode(dl_list **head, node_data data) {
    dl_list *aux = *head;
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


sl_list* sl_push_front(sl_list **head, node_data new_data) {
    sl_list *new_node = (sl_list*)malloc(sizeof(sl_list));
    if(new_node==NULL)
        return NULL;
    new_node->data = new_data;
    new_node->next = *head;
    *head = new_node;
    return new_node;
}


sl_list* sl_pop_front(sl_list **head) {
    sl_list *aux = *head;
    if(*head != NULL) {
        if((*head)->next == NULL) // if *head is the only dl_list
            *head = NULL;
        else // if *head isn't the only node
            *head = (*head)->next;
    }
    return aux; // returns the popped head
}


sl_list* sl_findNode(sl_list **head, node_data data) {
    sl_list *aux = *head;
    while(aux != NULL && aux->data != data)
        aux = aux->next;
    return aux; // returns NULL if data is not found
}