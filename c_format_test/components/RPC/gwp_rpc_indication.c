#include <stdint.h>
#include <string.h>




#include "gwp_common.h"
//#include "app_error.h"
#include "gwp_list.h"
#include "gwp_rpc_indication.h"


#define GWP_ITEMS_MAX                   10



//static gwp_item_t __attribute__((aligned(4))) gwp_item_bank[GWP_ITEMS_MAX];
//static gwp_item_t   gwp_item_bank[GWP_ITEMS_MAX];
gwp_item_header_t   gwp_item_bank[GWP_POLLING_ITEMS_MAX];
uint8_t gwp_polling_buff[GWP_POLLING_BUFF_SIZE];
//STRUCT_ALIGN_1(gwp_item_t);
static gwp_list_head_t           free_items;

void gwp_indication_item_init(void)
{
    uint16_t i;
      gwp_list_init(&free_items);
    for(i = 0; i < GWP_POLLING_ITEMS_MAX; i++)
    {
                        gwp_list_push_front(&free_items, (gwp_list_t *)&gwp_item_bank[i]);
    }
}
uint16_t gwp_size_of_unused_buffer(gwp_list_head_t * list_head)
{
    uint16_t SizeOfUnusedBuf = 0;
    if(list_head->WritePos >= list_head->ReadPos)
        SizeOfUnusedBuf = GWP_POLLING_BUFF_SIZE - list_head->WritePos + list_head->ReadPos;
    else
        SizeOfUnusedBuf = list_head->ReadPos - list_head->WritePos;
    return SizeOfUnusedBuf;
}

gwp_item_header_t *gwp_indication_item_reserve(uint16_t length)
{
    gwp_item_header_t *item = NULL;
    if(length < gwp_size_of_unused_buffer(&free_items))
    {
        if(gwp_list_size(&free_items) > 0)
        {
            item = (gwp_item_header_t *)gwp_list_pop_front(&free_items);
            if(item != NULL)
            {
                item->ReadPos = free_items.WritePos;
                item->Reserve_length = length;
                free_items.WritePos = (free_items.WritePos + length) % GWP_POLLING_BUFF_SIZE;
            }
        }
    }
    return item;
}

uint8_t gwp_indication_item_size()
{
    return GWP_POLLING_ITEMS_MAX - gwp_list_size(&free_items);
}

void gwp_indication_item_free(gwp_item_header_t *item)
{
    free_items.ReadPos = (free_items.ReadPos + item->Reserve_length) % GWP_POLLING_BUFF_SIZE;
    memset(item, 0, sizeof(gwp_item_header_t));
    gwp_list_push_front(&free_items, (gwp_list_t *)item);
}

