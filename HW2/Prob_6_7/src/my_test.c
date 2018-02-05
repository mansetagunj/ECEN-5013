/*
 * main.c
 *
 *  Created on: 02-Feb-2018
 *      Author: Gunj Manseta
 */

#include "doublyLinkedList.h"
#include <stdint.h>
#include <stdio.h>

struct info{

	int32_t data;
	LIST_NODE_T node;

};

#define DELETE_INFO_STRUCT(p_list_node) \
    free(GET_LIST_CONTAINER(p_list_node,struct info, node))

#define DESTROY_INFO_STRUCT(p_list_head)    \
    DESTROY_LIST(p_list_head,struct info,node)

static inline void print_INFO_LIST_data(LIST_NODE_T *data_list_head)
{
    LIST_NODE_T *list_itr =data_list_head;
    printf("-----------------\n");
    for(int i = 0 ; list_itr ; i++, list_itr = list_itr->next)
        printf("Node %d data: %d \n",i, GET_LIST_CONTAINER(list_itr,struct info,node)->data);
    printf("-----------------\n");
}

int main()
{
	struct info * data, *data_1, *data_2, *data_3, *data_4;
        data = (struct info*)malloc(sizeof(struct info));
        
	data->data = 10;
	//LIST_NODE_T list_head_data;
	LIST_HEAD_INIT(&data->node);
        //LIST_NODE_T *data_list_head = &data->node;
        LIST_NODE_T *data_list_head = NULL;
        data_1 = (struct info*)malloc(sizeof(struct info));
	data_1->data = 20;
	//LIST_HEAD_INIT(&data_new->node);
        
        data_2 = (struct info*)malloc(sizeof(struct info));
	data_2->data = 30;
	//LIST_HEAD_INIT(&data_new->node);
        
                
        data_3 = (struct info*)malloc(sizeof(struct info));
	data_3->data = 40;
	//LIST_HEAD_INIT(&data_new->node);
        
        data_4 = (struct info*)malloc(sizeof(struct info));
	data_4->data = 5;
	//LIST_HEAD_INIT(&data_new->node);
        
        data_list_head = insert_at_end(data_list_head,&data->node);
	data_list_head = insert_at_end(data_list_head,&data_1->node);
        data_list_head = insert_at_end(data_list_head,&data_2->node);
        data_list_head = insert_at_position(data_list_head,&data_3->node,0);
        data_list_head = insert_at_beginning(data_list_head,&data_4->node);
        
        print_INFO_LIST_data(data_list_head);
        
        size_t list_size = size(data_list_head);
        printf("List size: %d\n",list_size);
  
        LIST_NODE_T *deletedNode = delete_from_beginning(&data_list_head);
        DELETE_INFO_STRUCT(deletedNode);
        
        print_INFO_LIST_data(data_list_head);
        list_size = size(data_list_head);
        printf("List size: %d\n",list_size);
        
        deletedNode = delete_from_position(data_list_head,0);
        DELETE_INFO_STRUCT(deletedNode);

        print_INFO_LIST_data(data_list_head);
        list_size = size(data_list_head);
        printf("List size: %d\n",list_size);
        
        deletedNode = delete_from_end(data_list_head);
        DELETE_INFO_STRUCT(deletedNode);
        

        print_INFO_LIST_data(data_list_head);
        list_size = size(data_list_head);
        printf("List size: %d\n",list_size);
        
        DESTROY_LIST(data_list_head, struct info, node);
        
        list_size = size(data_list_head);
        printf("List size: %d\n",list_size);

	return 0;
}

