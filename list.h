#ifndef LIST_H
#define LIST_H

/** node_data should be comparable. */
typedef int node_data;

typedef struct dl_List {
    node_data data;
    struct dl_List *prev, *next;
} dl_list;

typedef struct sl_List {
    node_data data;
    struct sl_List *next;
} sl_list;

dl_list* dl_push_front(dl_list **head, node_data new_data);
dl_list* dl_pop_front(dl_list **head);
dl_list* dl_findNode(dl_list **head, node_data data);
dl_list* push_back(dl_list **head, node_data new_data);
dl_list* pop_back(dl_list **head);
int removeNode(dl_list **head, node_data data);
sl_list* sl_push_front(sl_list **head, node_data new_data);
sl_list* sl_pop_front(sl_list **head);
sl_list* sl_findNode(sl_list **head, node_data data);

#endif // LIST_H