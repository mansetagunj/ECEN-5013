/**
* @file - test_file.h
* @brief - Header file for declaration of the test case functions on doubly linked list
* 
* @author Gunj, University of Colorado Boulder
* @date 4/2/2018
**/



#include <stdlib.h>
#include <stdio.h>
#include <cmocka.h>
#include <stdint.h>
#include <stdarg.h>
#include "doublyLinkedList.h"

struct info{

	int32_t data;
	LIST_NODE_T node;

};

#define DELETE_INFO_STRUCT(p_list_node) \
    free(GET_LIST_CONTAINER(p_list_node,struct info, node))

#define DESTROY_INFO_STRUCT(p_list_head)	DESTROY_LIST(p_list_head,struct info,node)
    
static inline void print_INFO_LIST_data(LIST_NODE_T *data_list_head)
{
    LIST_NODE_T *list_itr =data_list_head;
    printf("-----------------\n");
    for(int i = 0 ; list_itr ; i++, list_itr = list_itr->next)
        printf("Node %d data: %d \n",i, GET_LIST_CONTAINER(list_itr,struct info,node)->data);
    printf("-----------------\n");
}


/**
* @brief - 
* @return void
**/
void test_list_insert_begin();

void test_list_insert_end();

void test_list_insert_pos();

void test_list_delete_begin();

void test_list_delete_end();

void test_list_delete_pos();

void test_list_size();

void test_list_destroy();

void test_list_getcontainerof_macro();

