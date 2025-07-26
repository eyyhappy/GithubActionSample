
#include <stdint.h>
#include <string.h> // memcpy

#include "gwp_list.h"

void gwp_list_init(gwp_list_head_t * list_head)
{
    list_head->next = NULL;
    list_head->last = NULL;
    list_head->size = 0;
    list_head->ReadPos = 0;
    list_head->WritePos = 0;
}

gwp_list_t * gwp_list_next(const gwp_list_t * iter)
{
    return iter->next;
}

/* puts one element to the end of the list */
void gwp_list_push_back(gwp_list_head_t * list_head, gwp_list_t * element)
{
    // even if (element->next==0) we still might be adding
    // the last element again to the list.
    // Waits further testing: commented out
    if (list_head->next == NULL)
    {
        list_head->next = (gwp_list_t *) element;
    }
    else
    {
        list_head->last->next = (gwp_list_t *) element;
    }
    list_head->last = (gwp_list_t *) element;
    ((gwp_list_t *) element)->next = NULL;
    list_head->size++;
}

/* puts one element to the start of the list (just after list_head)*/
void gwp_list_push_front(gwp_list_head_t * list_head, gwp_list_t * element)
{
    ((gwp_list_t *) element)->next = list_head->next;
    list_head->next = (gwp_list_t *) element;
    if (list_head->last == NULL)
    {
        list_head->last = (gwp_list_t *) element;
    }
    list_head->size++;
}

/* puts element to queue just before element 'iter' */
void gwp_list_push_before(gwp_list_head_t * list_head,
                          const gwp_list_t * iter,
                          gwp_list_t * element)
{
    gwp_list_t * h = (gwp_list_t *) list_head;
    // search item h that precedes iter
    while (h->next != iter)
    {
        h = h->next;
    }
    element->next = h->next;
    h->next = element;
    list_head->size++;
}

gwp_list_t * gwp_list_pop_back(gwp_list_head_t * list_head)
{
    gwp_list_t * h = (gwp_list_t *) list_head;
    gwp_list_t * prev = NULL;
    if (h->next == NULL)
        return NULL;
    // Search item that precedes the last item. Because h->next != NULL, this loop
    // is executed at least once --> prev = valid.
    while (h->next != NULL)
    {
        prev = h;
        h = prev->next;
    }
    // now:
    // - h is the last item
    // - prev is the second last item (might be list_head)
    prev->next = NULL; // stored in h
    list_head->size--;
    if (prev == (gwp_list_t *) list_head)
    {
        list_head->last = NULL;
    }
    else
    {
        list_head->last = prev;
    }
    h->next = NULL;
    return h;
}

gwp_list_t * gwp_list_pop_front(gwp_list_head_t * list_head)
{
    gwp_list_t * ret;
    ret = list_head->next;
    if (ret == NULL)
    {
        // empty list
        return NULL;
    }
    list_head->size--;
    list_head->next = ret->next;
    if (list_head->next == NULL)
    {
        list_head->last = NULL;
    }
    ret->next = NULL;
    return ret;
}

gwp_list_t * gwp_list_pop(gwp_list_head_t * list_head, gwp_list_t * element)
{
    element = gwp_list_remove(list_head, element);
    // clear next pointer to avoid unintentional bugs
    element->next = NULL;
    return element;
}

gwp_list_t * gwp_list_remove(gwp_list_head_t * list_head, gwp_list_t * element)
{
    gwp_list_t * h = (gwp_list_t *) list_head;
    // search element position in list
    while (h->next != NULL)
    {
        if (h->next == element)
        {
            // fix next linkage
            h->next = element->next;
            list_head->size--;
            // fix list head last element
            if (list_head->last == element)
            {
                if (h != (gwp_list_t *) list_head)
                {
                    list_head->last = h;
                }
                else
                {
                    list_head->last = NULL;
                }
            }
            // do not clear the next pointer
            return element;
        }
        // continue search
        h = h->next;
    }
    // not in list
    return element;
}

gwp_list_t * gwp_list_at(const gwp_list_head_t * list_head, int idx)
{
    gwp_list_t * h = gwp_list_next((gwp_list_t *) list_head);
    while ((h != NULL) && (idx != 0))
    {
        h = gwp_list_next(h);
        idx--;
    }
    return h;
}

unsigned char gwp_list_contains(const gwp_list_head_t * list_head,
                                const gwp_list_t * element)
{
    gwp_list_t * x = gwp_list_next((gwp_list_t *) list_head);
    while (x != NULL)
    {
        if (x == element)
            return 1;
        x = gwp_list_next(x);
    }
    return 0;
}

gwp_list_t * gwp_list_search(const gwp_list_t * start,
                             int (*match)(const gwp_list_t *, const void *),
                             const void * match_param)
{
    gwp_list_t * x = gwp_list_next(start);
    while (x != NULL)
    {
        if (match((const gwp_list_t *) x, match_param))
            return x;
        x = gwp_list_next(x);
    }
    return NULL;
}

void gwp_list_swap(gwp_list_head_t * list1, gwp_list_head_t * list2)
{
    gwp_list_head_t t;
    memcpy((void *) &t, (void *) list1, sizeof(gwp_list_head_t));
    memcpy((void *) list1, (void *) list2, sizeof(gwp_list_head_t));
    memcpy((void *) list2, (void *) &t, sizeof(gwp_list_head_t));
}
