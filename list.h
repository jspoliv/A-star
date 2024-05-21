#ifndef LIST_H
#define LIST_H

typedef struct node_data {
    int value, priority;
} node_data;

typedef struct dl_node {
    node_data data;
    struct dl_node *prev, *next;
} dl_node;

typedef struct dl_list {
    dl_node *head, *tail;
} dl_list;

dl_node* dl_push_priority(dl_list *list, node_data new_data);
dl_node* dl_push_front(dl_list *list, node_data new_data);
dl_node* dl_pop_front(dl_list *list);
dl_node* dl_push_back(dl_list *list, node_data new_data);
dl_node* dl_pop_back(dl_list *list);
dl_node* dl_find_value(dl_list *list, node_data data);
int dl_remove_value(dl_list *list, node_data data);



typedef struct sl_list {
    node_data data;
    struct sl_list *next;
} sl_list;

sl_list* sl_push_front(sl_list **head, node_data new_data);
sl_list* sl_pop_front(sl_list **head);
sl_list* sl_find_value(sl_list **head, node_data data);

#endif // LIST_H