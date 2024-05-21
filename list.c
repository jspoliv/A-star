#include <stdio.h>
#include <stdlib.h>
#include "list.h"


dl_node* dl_push_priority(dl_list *list, node_data new_data) {
    dl_node *new_node = (dl_node*)malloc(sizeof(dl_node)), *aux;
    if(new_node == NULL)
        return NULL;

    new_node->data = new_data;

    if(list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
        new_node->prev = NULL;
        new_node->next = NULL;
        return new_node;
    }

    if(abs(list->head->data.priority - new_data.priority) > abs(list->tail->data.priority - new_data.priority)) {
        aux = list->tail;
        while(aux->prev != NULL && aux->data.priority > new_data.priority)
            aux = aux->prev;
    }
    else {
        aux = list->head;
        while(aux->next != NULL && aux->data.priority < new_data.priority)
            aux = aux->next;
    }

    if(aux->data.priority < new_data.priority) { // adds new_node as aux->prev
        new_node->next = aux->next;
        if(aux->next != NULL)
            aux->next->prev = new_node;
        aux->next = new_node;
        new_node->prev = aux;
        if(aux == list->tail)
            list->tail = new_node;
    }
    else { // adds new_node as aux->next
        new_node->prev = aux->prev;
        if(aux->prev != NULL)
            aux->prev->next = new_node;
        aux->prev = new_node;
        new_node->next = aux;
        if(aux == list->head)
            list->head = new_node;
    }
    return new_node;
}


dl_node* dl_push_front(dl_list *list, node_data new_data) {
    dl_node *new_node = (dl_node*)malloc(sizeof(dl_node));
    if(new_node == NULL)
        return NULL;
    new_node->prev = NULL;
    new_node->data = new_data;
    new_node->next = list->head;

    if(list->head != NULL)
        list->head->prev = new_node;
    else
        list->tail = new_node;

    list->head = new_node;
    return new_node;
}


dl_node* dl_pop_front(dl_list *list) {
    dl_node *aux = list->head;
    if(aux == NULL)
        return NULL;

    if(aux->next == NULL) { // if HEAD is the only node
        list->head = NULL;
        list->tail = NULL;
    }
    else { // if HEAD isn't the only node
        list->head = aux->next;
        list->head->prev = NULL;
    }
    return aux; // returns the popped HEAD
}


dl_node* dl_push_back(dl_list *list, node_data new_data) {
    dl_node *new_node = (dl_node*)malloc(sizeof(dl_node));
    if(new_node == NULL)
        return NULL;

    new_node->prev = list->tail;
    new_node->data = new_data;
    new_node->next = NULL;

    if(list->tail != NULL)
        list->tail->next = new_node;
    else
        list->head = new_node;

    list->tail = new_node;
    return new_node;

    return new_node;
}


dl_node* dl_pop_back(dl_list *list) {
    dl_node *aux = list->tail;
    if(aux == NULL)
        return NULL;

    if(aux->prev == NULL) { // if TAIL is the only node
        list->head = NULL;
        list->tail = NULL;
    }
    else { // if TAIL isn't the only node
        list->tail = aux->prev;
        list->tail->next = NULL;
    }
    return aux; // returns the popped TAIL
}


dl_node* dl_find_value(dl_list *list, node_data data) {
    dl_node *aux = list->head;
    if(aux == NULL)
        return NULL;

    if(abs(list->head->data.value - data.value) > abs(list->tail->data.value - data.value)) {
        aux = list->tail;
        while(aux != NULL && aux->data.value != data.value)
            aux = aux->prev;
    }
    else {
        aux = list->head;
        while(aux != NULL && aux->data.value != data.value)
            aux = aux->next;
    }

    return aux; // returns NULL if data is not found
}


int dl_remove_value(dl_list *list, node_data data) {
    dl_node *aux = dl_find_value(list, data);
    if(aux == NULL)
        return 0;

    if(aux == list->head)
        list->head = aux->next;
    if(aux == list->tail)
        list->tail = aux->prev;

    if(aux->prev != NULL)
        aux->prev->next = aux->next;
    if(aux->next != NULL)
        aux->next->prev = aux->prev;

    free(aux);
    return 1;
}


sl_list* sl_push_front(sl_list **head, node_data new_data) {
    sl_list *new_node = (sl_list*)malloc(sizeof(sl_list));
    if(new_node == NULL)
        return NULL;
    new_node->data = new_data;
    new_node->next = *head;
    *head = new_node;
    return new_node;
}


sl_list* sl_pop_front(sl_list **head) {
    sl_list *aux = *head;
    if(*head != NULL) {
        if((*head)->next == NULL) // if *head is the only node
            *head = NULL;
        else // if *head isn't the only node
            *head = (*head)->next;
    }
    return aux; // returns the popped head
}


sl_list* sl_find_value(sl_list **head, node_data data) {
    sl_list *aux = *head;
    while(aux != NULL && aux->data.value != data.value)
        aux = aux->next;
    return aux; // returns NULL if data is not found
}