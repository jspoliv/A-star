#ifndef LIST_H
#define LIST_H

/** node_data should be comparable. */
typedef int node_data;

typedef struct dl_list {
    node_data data;
    struct dl_list *prev, *next;
} dl_list;

typedef struct sl_list {
    node_data data;
    struct sl_list *next;
} sl_list;

dl_list* dl_push_by_data(dl_list **head, dl_list **tail, node_data new_data);
dl_list* dl_push_front(dl_list **head, dl_list **tail, node_data new_data);
dl_list* dl_pop_front(dl_list **head, dl_list **tail);
dl_list* dl_push_back(dl_list **head, node_data new_data);
dl_list* dl_pop_back(dl_list **head);
dl_list* dl_findNode(dl_list **head, node_data data);
int dl_removeNode(dl_list **head, node_data data);
sl_list* sl_push_front(sl_list **head, node_data new_data);
sl_list* sl_pop_front(sl_list **head);
sl_list* sl_findNode(sl_list **head, node_data data);

#endif // LIST_H