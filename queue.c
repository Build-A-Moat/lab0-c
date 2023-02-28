#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *queue = malloc(sizeof(struct list_head));
    if (queue)
        INIT_LIST_HEAD(queue);
    return queue;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *node, *safe;
    list_for_each_entry_safe (node, safe, l, list) {
        q_release_element(node);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *element = malloc(sizeof(element_t));
    if (!element)
        return false;
    element->value = strdup(s);
    if (!element->value) {
        free(element);
        return false;
    }
    list_add(&element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *element = malloc(sizeof(element_t));
    if (!element)
        return false;
    element->value = strdup(s);
    if (!element->value) {
        free(element);
        return false;
    }
    list_add_tail(&element->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *remove = list_first_entry(head, element_t, list);
    list_del(head->next);
    if (sp) {
        strncpy(sp, remove->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return remove;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *remove = list_last_entry(head, element_t, list);
    list_del(head->prev);
    if (sp) {
        strncpy(sp, remove->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return remove;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *slow = head, *fast = head;
    while (fast->next != head && fast->next->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    struct list_head *middle = slow->next;
    list_del(middle);
    q_release_element(list_entry(middle, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return NULL;
    element_t *node, *safe;
    bool remove = false;
    list_for_each_entry_safe (node, safe, head, list) {
        if (&safe->list != head && !strcmp(node->value, safe->value)) {
            remove = true;
            list_del(&node->list);
            q_release_element(node);
            continue;
        }
        if (remove) {
            remove = false;
            list_del(&node->list);
            q_release_element(node);
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *cur = head->next;
    while (cur != head && cur->next != head) {
        struct list_head *next = cur->next;
        list_move(cur, next);
        cur = cur->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *cur = head->prev->prev, *last = head->prev;
    while (cur != head) {
        list_move_tail(cur, head);
        cur = last->prev;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

struct list_head *merge_two_list(struct list_head *L1,
                                 struct list_head *L2,
                                 struct list_head *head)
{
    struct list_head *tmp, *L1_last = L1->prev, *L2_last = L2->prev;
    L1->prev->next = NULL;
    L2->prev->next = NULL;
    INIT_LIST_HEAD(head);
    while (L1 && L2) {
        if (strcmp(list_entry(L1, element_t, list)->value,
                   list_entry(L2, element_t, list)->value) > 0) {
            tmp = L2;
            L2 = L2->next;
            list_add_tail(tmp, head);
        } else {
            tmp = L1;
            L1 = L1->next;
            list_add_tail(tmp, head);
        }
    }
    struct list_head *result = head->next;
    list_del_init(head);
    if (!L1) {
        L2->prev = L2_last;
        L2_last->next = L2;
        list_add_tail(head, L2);
    } else {
        L1->prev = L1_last;
        L1_last->next = L1;
        list_add_tail(head, L1);
    }
    for (struct list_head *node = head->next; !list_empty(head);
         node = head->next) {
        list_move_tail(node, result);
    }
    return result;
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    int size = q_size(head);
    struct list_head **stack =
        (struct list_head **) malloc(size * sizeof(struct list_head *));

    if (stack == NULL) {
        return false;
    }

    for (int i = 0; i < size; i++) {
        stack[i] = (struct list_head *) malloc(sizeof(struct list_head));
        if (stack[i] == NULL) {
            return false;
        }
    }
    struct list_head *tmp = head->next;
    for (int i = 0; tmp != head; tmp = head->next) {
        list_del_init(tmp);
        stack[i++] = tmp;
    }
    while (size > 1) {
        for (int i = 0, j = size - 1; i < j; i++, j--) {
            stack[i] = merge_two_list(stack[i], stack[j], head);
        }
        size = (size + 1) / 2;
    }
    list_add_tail(head, stack[0]);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
