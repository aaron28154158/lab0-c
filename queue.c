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
    struct list_head *lh =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (!lh)
        return NULL;

    INIT_LIST_HEAD(lh);
    return lh;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *pos, *next;
    list_for_each_entry_safe (pos, next, head, list) {
        list_del(&pos->list);
        q_release_element(pos);
    }

    free(head);
}


/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *new_ele = (element_t *) malloc(sizeof(element_t));
    if (!new_ele)
        return false;

    new_ele->value = strdup(s);
    if (!new_ele->value) {
        q_release_element(new_ele);
        return false;
    }

    list_add(&new_ele->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *new_ele = (element_t *) malloc(sizeof(element_t));
    if (!new_ele)
        return false;

    new_ele->value = strdup(s);
    if (!new_ele->value) {
        q_release_element(new_ele);
        return false;
    }

    list_add_tail(&new_ele->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *ele = list_entry(head->next, element_t, list);
    list_del_init(head->next);

    if (sp && bufsize > 0)
        strlcpy(sp, ele->value, bufsize);

    return ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *ele = list_entry(head->prev, element_t, list);
    list_del_init(head->prev);

    if (sp && bufsize > 0)
        strlcpy(sp, ele->value, bufsize);

    return ele;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
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
    if (!head || list_empty(head))
        return false;

    struct list_head *slow = head->next, *fast = head->next;

    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 'slow' points to the middle node that should be deleted
    list_del(slow);

    // Retrieve the 'element_t' structure containing the middle node
    element_t *ele = list_entry(slow, element_t, list);
    q_release_element(ele);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // Return false if queue is NULL or empty
    if (!head || list_empty(head))
        return false;

    // If there's only one element, there are no duplicates.
    if (list_is_singular(head))
        return true;

    // Traverse through the list starting from the first real element
    struct list_head *cur = head->next;
    while (cur != head) {
        // Declare as pointer to const to indicate no modification to cur_ele
        const element_t *cur_ele = list_entry(cur, element_t, list);

        // Check if the next node exists and has the same value as the current
        // node
        if (strcmp(cur_ele->value,
                   list_entry(cur->next, element_t, list)->value) == 0) {
            // Duplicate group detected: remove all nodes with this value

            // Delete all duplicates following the current node
            while (cur->next != head &&
                   strcmp(cur_ele->value,
                          list_entry(cur->next, element_t, list)->value) == 0) {
                struct list_head *dup = cur->next;
                list_del(dup);
                q_release_element(list_entry(dup, element_t, list));
            }
            // Delete the first node of the duplicate group as well
            struct list_head *tmp = cur;
            cur = cur->next;  // Advance pointer before deletion
            list_del(tmp);
            q_release_element(list_entry(tmp, element_t, list));
        } else {
            // No duplicate: move to the next node
            cur = cur->next;
        }
    }
    return true;
}


/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *cur = head->next, *next = cur->next;

    while (cur->next != head && cur != head) {
        list_move(next, cur->prev);
        cur = cur->next;
        next = cur->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
