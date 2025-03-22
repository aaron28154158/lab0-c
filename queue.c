#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

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

    element_t *pos = NULL, *next = NULL;
    list_for_each_entry_safe(pos, next, head, list) {
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

    element_t *ele = list_first_entry(head, element_t, list);
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

    element_t *ele = list_last_entry(head, element_t, list);
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

    list_for_each(li, head)
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
    q_release_element(list_entry(slow, element_t, list));

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
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *temp = NULL, *safe = NULL;

    list_for_each_safe(temp, safe, head)
        list_move(temp, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || k <= 1)
        return;

    int len = 0;
    struct list_head *node;
    list_for_each(node, head)
        len++;

    // 'pre' points to the tail of the processed segment (initially the dummy
    // head).
    struct list_head *pre = head;

    for (int i = 0; i < len / k; i++) {
        // 'start' is the first node of the current k-group.
        struct list_head *start = pre->next;
        struct list_head *end = start;

        // Advance 'end' to the k-th node in the group.
        for (int j = 0; j < k - 1; j++) {
            end = end->next;
        }

        // Use a stack-allocated dummy head for the segment.
        struct list_head tmp_head;
        INIT_LIST_HEAD(&tmp_head);

        // Cut the segment [pre->next, end] from the original list into
        // tmp_head.
        list_cut_position(&tmp_head, pre, end);

        // Reverse the cut segment.
        q_reverse(&tmp_head);

        // Splice the reversed segment back after 'pre'.
        list_splice(&tmp_head, pre);

        // Update 'pre' to the original first node (now the tail of the reversed
        // segment).
        pre = start;
    }
}

struct list_head *q_split(struct list_head *head)
{
    struct list_head *slow = head->next, *fast = head->next;

    while (fast->next != head && fast->next->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;
}

void q_mergeLeftRight(struct list_head *head,
                      struct list_head *left,
                      struct list_head *right,
                      bool descend)
{
    while (!list_empty(left) && !list_empty(right)) {
        element_t *ele_l = list_first_entry(left, element_t, list);
        element_t *ele_r = list_first_entry(right, element_t, list);

        int cmp_result = strcmp(ele_l->value, ele_r->value);

        if ((descend && cmp_result >= 0) || (!descend && cmp_result > 0)) {
            list_move_tail(&ele_r->list, head);
        } else {
            list_move_tail(&ele_l->list, head);
        }
    }

    list_splice_tail_init(left, head);
    list_splice_tail_init(right, head);
}

void q_merge_sort(struct list_head *head, bool descend)
{
    if (list_empty(head) || list_is_singular(head))
        return;

    struct list_head *mid = q_split(head);

    struct list_head left, right;
    INIT_LIST_HEAD(&left);
    INIT_LIST_HEAD(&right);

    list_cut_position(&left, head, mid);
    list_splice_init(head, &right);
    INIT_LIST_HEAD(head);

    q_merge_sort(&left, descend);
    q_merge_sort(&right, descend);

    q_mergeLeftRight(head, &left, &right, descend);
}

void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    q_merge_sort(head, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    int len = q_size(head);

    if (!head || list_empty(head) || list_is_singular(head))
        return len;

    struct list_head *right = head->prev, *left = right->prev;

    while (left != head) {
        const element_t *ele_l = list_entry(left, element_t, list);
        const element_t *ele_r = list_entry(right, element_t, list);

        if (strcmp(ele_l->value, ele_r->value) <= 0) {
            right = right->prev;
            left = left->prev;

        } else {
            list_del(left);
            element_t *tmp = list_entry(left, element_t, list);
            q_release_element(tmp);

            left = right->prev;
            len--;
        }
    }

    return len;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    int len = q_size(head);

    if (!head || list_empty(head) || list_is_singular(head))
        return len;

    struct list_head *right = head->prev, *left = right->prev;

    while (left != head) {
        const element_t *ele_l = list_entry(left, element_t, list);
        const element_t *ele_r = list_entry(right, element_t, list);

        if (strcmp(ele_l->value, ele_r->value) >= 0) {
            right = right->prev;
            left = left->prev;

        } else {
            list_del(left);
            element_t *tmp = list_entry(left, element_t, list);
            q_release_element(tmp);

            left = right->prev;
            len--;
        }
    }

    return len;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *chain_head, bool descend)
{
    // If the chain is empty or has only one queue, return its size.
    if (!chain_head || list_empty(chain_head) || list_is_singular(chain_head)) {
        if (!chain_head || list_empty(chain_head))
            return 0;
        queue_contex_t *ctx =
            list_entry(chain_head->next, queue_contex_t, chain);
        return ctx->size;
    }

    // Use the first queue context as the target for merging.
    queue_contex_t *first_ctx =
        list_entry(chain_head->next, queue_contex_t, chain);

    // Merge all other queues into the first.
    struct list_head *pos, *n;
    list_for_each_safe(pos, n, chain_head) {
        queue_contex_t *ctx = list_entry(pos, queue_contex_t, chain);
        if (ctx == first_ctx)
            continue;  // Skip the target queue.
        if (ctx->q && !list_empty(ctx->q)) {
            // Append ctx->q to the tail of first_ctx->q.
            list_splice_tail_init(ctx->q, first_ctx->q);
            first_ctx->size += ctx->size;
            ctx->size = 0;
        }
    }

    // Sort the merged queue.
    q_sort(first_ctx->q, descend);

    return first_ctx->size;
}
