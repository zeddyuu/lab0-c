#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    if (!q)
        return NULL;
    INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *c, *n;
    list_for_each_entry_safe (c, n, l, list) {
        list_del(&c->list);
        q_release_element(c);
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
    char *s_new = malloc((strlen(s) + 1) * sizeof(char));
    if (!s_new) {
        free(element);
        return false;
    }
    memcpy(s_new, s, strlen(s) + 1);
    element->value = s_new;
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
    char *s_new = malloc((strlen(s) + 1) * sizeof(char));
    if (!s_new) {
        free(element);
        return false;
    }
    memcpy(s_new, s, strlen(s) + 1);
    element->value = s_new;
    list_add_tail(&element->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *element = list_first_entry(head, element_t, list);
    if (sp && bufsize != 0) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(&element->list);
    return element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *element = list_last_entry(head, element_t, list);
    if (sp && bufsize != 0) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(&element->list);
    return element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    int size = 0;
    struct list_head *c;
    list_for_each (c, head) {
        size++;
    }
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *fast = head->next, *slow = fast;
    while (fast->next != head && fast != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    list_del(slow);
    q_release_element(list_entry(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    element_t *c, *n;
    bool flag = false;
    list_for_each_entry_safe (c, n, head, list) {
        if (c->list.next != head && strcmp(c->value, n->value) == 0) {
            list_del(&c->list);
            q_release_element(c);
            flag = true;
        } else if (flag) {
            list_del(&c->list);
            q_release_element(c);
            flag = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;
    for (struct list_head *c = head->next; c->next != head && c != head;
         c = c->next) {
        list_move(c, c->next);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *c, *n;
    list_for_each_safe (c, n, head) {
        list_del(c);
        list_add(c, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || k <= 1)
        return;
    struct list_head *c, *n, *tmp_head = head;
    LIST_HEAD(dummy);
    int i = 0;
    list_for_each_safe (c, n, head) {
        i++;
        if (i == k) {
            list_cut_position(&dummy, tmp_head, c);
            q_reverse(&dummy);
            list_splice_init(&dummy, tmp_head);
            i = 0;
            tmp_head = n->prev;
        }
    }
}
struct list_head *merge_two_list(struct list_head *l1, struct list_head *l2)
{
    struct list_head *head, *tmp;
    if (strcmp(list_entry(l1, element_t, list)->value,
               list_entry(l2, element_t, list)->value) <= 0) {
        head = l1;
        tmp = head;
        l1 = l1->next;
    } else {
        head = l2;
        tmp = head;
        l2 = l2->next;
    }
    while (l1 && l2) {
        if (strcmp(list_entry(l1, element_t, list)->value,
                   list_entry(l2, element_t, list)->value) <= 0) {
            tmp->next = l1;
            l1->prev = tmp;
            tmp = tmp->next;
            l1 = l1->next;
        } else {
            tmp->next = l2;
            l2->prev = tmp;
            tmp = tmp->next;
            l2 = l2->next;
        }
    }

    while (l2) {
        tmp->next = l2;
        l2->prev = tmp;
        tmp = tmp->next;
        l2 = l2->next;
    }
    while (l1) {
        tmp->next = l1;
        l1->prev = tmp;
        tmp = tmp->next;
        l1 = l1->next;
    }
    head->prev = tmp;
    return head;
}


struct list_head *mergesort(struct list_head *head)
{
    if (!head->next)
        return head;
    struct list_head *fast = head, *slow = fast, *nl, *nr;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    // slow->prev是左邊的結尾 slow為右邊的開頭
    slow->prev->next = NULL;
    nl = mergesort(head);
    nr = mergesort(slow);
    return merge_two_list(nl, nr);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    head->prev->next = NULL;
    head->next = mergesort(head->next);
    head->next->prev->next = head;
    head->prev = head->next->prev;
    head->next->prev = head;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    int len = 0;
    for (struct list_head *cur = head->prev; cur != head && cur->prev != head;
         len++) {
        element_t *c = container_of(cur, element_t, list);
        element_t *p = container_of(cur->prev, element_t, list);
        if (strcmp(c->value, p->value) > 0) {
            list_del(&p->list);
            q_release_element(p);
            len--;
        } else {
            cur = cur->prev;
        }
    }
    return len;
}
/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    queue_contex_t *q1 = container_of(head->next, queue_contex_t, chain);
    if (list_is_singular(head))
        return q1->size;
    for (struct list_head *cur = head->next->next; cur != head;
         cur = cur->next) {
        queue_contex_t *q = container_of(cur, queue_contex_t, chain);
        list_splice_init(q->q, q1->q);
        q->size = 0;
    }
    q_sort(q1->q);
    q1->size = q_size(q1->q);
    return q1->size;
}
