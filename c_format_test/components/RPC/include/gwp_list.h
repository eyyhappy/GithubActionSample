

#ifndef _GWP_LIST_H_
#define _GWP_LIST_H_

#include <stdint.h>



typedef struct
{
    struct gwp_list_t *    next;
    struct gwp_list_t *    last;
    /** Amount of items in the list */
    uint32_t              size;
    uint32_t              ReadPos;
    uint32_t              WritePos;
} gwp_list_head_t;

typedef struct gwp_list_t
{
    struct gwp_list_t *    next;
} gwp_list_t;

void gwp_list_init(gwp_list_head_t * list_head);

#define gwp_list_empty(l) ((l)->next == NULL)
//#define gwp_list_set_empty(l) (l)->next = NULL

/* returns an iterator to first element */
//#define gwp_list_begin(l) ((gwp_list_t *)(l)->next)
#define gwp_list_begin(l) gwp_list_next((gwp_list_t *) l)

/* return an iterator to one past last element */
#define gwp_list_end(l) ((gwp_list_t *) NULL)

#define gwp_list_front(l) ((gwp_list_t *)(l)->next)

/* increases iterator by one */
#define gwp_list_ITER_NEXT(iter) { (iter) = (((gwp_list_t *)(iter))->next); }

/**
 * Returns next element in the list.
 *
 * Example
 * \code
 * gwp_list_t * item = gwp_list_begin(list);
 * while (item != gwp_list_end(list))
 * {
 *     // ...
 *     item = gwp_list_next(item);
 * }
 * \endcode
 */
gwp_list_t * gwp_list_next(const gwp_list_t * iter);

void gwp_list_push_before(gwp_list_head_t * list_head,
                          const gwp_list_t * iter,
                          gwp_list_t * element);

void gwp_list_push_back(gwp_list_head_t * list_head, gwp_list_t * element);

void gwp_list_push_front(gwp_list_head_t * list_head, gwp_list_t * element);

gwp_list_t * gwp_list_pop_front(gwp_list_head_t * list_head);

gwp_list_t * gwp_list_pop_back(gwp_list_head_t * list_head);

/**
 * Removes named element from the list.
 */
gwp_list_t * gwp_list_pop(gwp_list_head_t * list_head, gwp_list_t * element);

/**
 * Removes named element form the list and returns the previous element,
 */
gwp_list_t * gwp_list_remove(gwp_list_head_t * list_head, gwp_list_t * element);

/* Returns element at index. Does not perform boundary checking. */
gwp_list_t * gwp_list_at(const gwp_list_head_t * list_head, int idx);

#define gwp_list_size(l) ((l)->size)


/**
 * Returns 1 if the named element is in the list.
 */
unsigned char gwp_list_contains(const gwp_list_head_t * list_head,
                                const gwp_list_t * element);

gwp_list_t * gwp_list_search(const gwp_list_t * start,
                             int (*match)(const gwp_list_t *, const void *),
                             const void * match_param);

/**
 * Swaps the contents of two lists.
 */
void gwp_list_swap(gwp_list_head_t * list1, gwp_list_head_t * list2);

#endif

