/**
* @file - test.c
* @brief - Main file containing function calls for all unit test cases
* 
* @author Gunj University of Colorado Boulder
* @date 27/10/2017
**/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "test_list.h"
/*
static void test_list_insert_begin(void **state) {
	
	struct info * data, *data_1, *data_2;
    
    LIST_NODE_T *data_list_head = NULL;
    
    
    data = (struct info*)malloc(sizeof(struct info));
  	data->data = 10;
	LIST_HEAD_INIT(&data->node);
    
    data_1 = (struct info*)malloc(sizeof(struct info));
	data_1->data = 20;
        
    data_2 = (struct info*)malloc(sizeof(struct info));
	data_2->data = 30;

	data_list_head = insert_at_beginning(data_list_head,&data->node);
	data_list_head = insert_at_beginning(data_list_head,&data_1->node);
	data_list_head = insert_at_beginning(data_list_head,&data_2->node);
	
	assert_ptr_equal(GET_LIST_CONTAINER(data_list_head,struct info,node),GET_LIST_CONTAINER(&data_2->node,struct info,node));
	
	//DESTROY_LIST(data_list_head, struct info, node);
	
}

static void test_list_insert_end(void **state) {

	struct info * data, *data_1, *data_2;
	
    LIST_NODE_T *data_list_head = NULL;
    
    data = (struct info*)malloc(sizeof(struct info));
  	data->data = 10;
	LIST_HEAD_INIT(&data->node);
    
    data_1 = (struct info*)malloc(sizeof(struct info));
	data_1->data = 20;
        
    data_2 = (struct info*)malloc(sizeof(struct info));
	data_2->data = 30;

	data_list_head = insert_at_end(data_list_head,&data->node);
	data_list_head = insert_at_end(data_list_head,&data_1->node);
	data_list_head = insert_at_end(data_list_head,&data_2->node);
	
	LIST_NODE_T *list_itr_end = data_list_head;
	//while there no element in the list. i.e. traversing to the end of the list
	while(list_itr_end->next)
		list_itr_end = list_itr_end->next;
	
	assert_ptr_equal(GET_LIST_CONTAINER(list_itr_end,struct info,node),GET_LIST_CONTAINER(&data_2->node,struct info,node));
	
	//DESTROY_LIST(data_list_head, struct info, node);
	
}

static void test_list_insert_pos(void **state) {

	struct info * data, *data_1, *data_2;
	
    LIST_NODE_T *data_list_head = NULL;
    
    data = (struct info*)malloc(sizeof(struct info));
  	data->data = 10;
	LIST_HEAD_INIT(&data->node);
    
    data_1 = (struct info*)malloc(sizeof(struct info));
	data_1->data = 20;
        
    data_2 = (struct info*)malloc(sizeof(struct info));
	data_2->data = 30;

	data_list_head = insert_at_end(data_list_head,&data->node);
	data_list_head = insert_at_end(data_list_head,&data_1->node);
	data_list_head = insert_at_position(data_list_head,&data_2->node,1);
	
	LIST_NODE_T *list_itr_end = data_list_head->next;
	
	assert_ptr_equal(GET_LIST_CONTAINER(data_list_head->next,struct info,node),GET_LIST_CONTAINER(&data_2->node,struct info,node));
	
	//DESTROY_LIST(data_list_head, struct info, node);
	
}

static void test_list_delete_begin(void **state) {
	
	struct info * data, *data_1, *data_2;
	
    LIST_NODE_T *data_list_head = NULL;
    
    data = (struct info*)malloc(sizeof(struct info));
  	data->data = 10;
	LIST_HEAD_INIT(&data->node);
    
    data_1 = (struct info*)malloc(sizeof(struct info));
	data_1->data = 20;
        
    data_2 = (struct info*)malloc(sizeof(struct info));
	data_2->data = 30;

	data_list_head = insert_at_beginning(data_list_head,&data->node);
	data_list_head = insert_at_beginning(data_list_head,&data_1->node);
	data_list_head = insert_at_beginning(data_list_head,&data_2->node);
	LIST_NODE_T *deletedNode = delete_from_beginning(&data_list_head);
   	DELETE_INFO_STRUCT(deletedNode);
	
	LIST_NODE_T *list_itr_end = data_list_head->next;
	
	assert_ptr_equal(deletedNode,NULL);
	
	assert_ptr_not_equal(GET_LIST_CONTAINER(data_list_head,struct info,node),GET_LIST_CONTAINER(&data->node,struct info,node));
	
	//DESTROY_LIST(data_list_head, struct info, node);
}

static void test_list_delete_end(void **state) {

	struct info * data, *data_1, *data_2;
	
    LIST_NODE_T *data_list_head = NULL;
    
    data = (struct info*)malloc(sizeof(struct info));
  	data->data = 10;
	LIST_HEAD_INIT(&data->node);
    
    data_1 = (struct info*)malloc(sizeof(struct info));
	data_1->data = 20;
        
    data_2 = (struct info*)malloc(sizeof(struct info));
	data_2->data = 30;

	data_list_head = insert_at_beginning(data_list_head,&data->node);
	data_list_head = insert_at_beginning(data_list_head,&data_1->node);
	data_list_head = insert_at_beginning(data_list_head,&data_2->node);
	LIST_NODE_T *deletedNode = delete_from_end(data_list_head);
   	DELETE_INFO_STRUCT(deletedNode);
	
	LIST_NODE_T *list_itr_end = data_list_head->next;
	
	assert_ptr_equal(deletedNode,NULL);
	
	assert_ptr_not_equal(GET_LIST_CONTAINER(data_list_head,struct info,node),GET_LIST_CONTAINER(&data->node,struct info,node));
	
	//DESTROY_LIST(data_list_head, struct info, node);
}

static void test_list_delete_pos(void **state) {
	
	struct info * data, *data_1, *data_2;
	
    LIST_NODE_T *data_list_head = NULL;
    
    data = (struct info*)malloc(sizeof(struct info));
  	data->data = 10;
	LIST_HEAD_INIT(&data->node);
    
    data_1 = (struct info*)malloc(sizeof(struct info));
	data_1->data = 20;
        
    data_2 = (struct info*)malloc(sizeof(struct info));
	data_2->data = 30;

	data_list_head = insert_at_end(data_list_head,&data->node);
	data_list_head = insert_at_end(data_list_head,&data_1->node);
	data_list_head = insert_at_end(data_list_head,&data_2->node);
	LIST_NODE_T *deletedNode = delete_from_position(data_list_head,1);
   	DELETE_INFO_STRUCT(deletedNode);
	
	LIST_NODE_T *list_itr_end = data_list_head->next;
	
	assert_ptr_equal(deletedNode,NULL);
	
	assert_ptr_not_equal(GET_LIST_CONTAINER(data_list_head,struct info,node),GET_LIST_CONTAINER(&data_1->node,struct info,node));
	
	//DESTROY_LIST(data_list_head, struct info, node);	
}

static void test_list_size(void **state) {
	
	struct info * data, *data_1, *data_2;
	
    LIST_NODE_T *data_list_head = NULL;
    
    data = (struct info*)malloc(sizeof(struct info));
  	data->data = 10;
	LIST_HEAD_INIT(&data->node);
    
    data_1 = (struct info*)malloc(sizeof(struct info));
	data_1->data = 20;
        
    data_2 = (struct info*)malloc(sizeof(struct info));
	data_2->data = 30;

	data_list_head = insert_at_end(data_list_head,&data->node);
	data_list_head = insert_at_end(data_list_head,&data_1->node);
	data_list_head = insert_at_end(data_list_head,&data_2->node);
	
	int size_list = size(data_list_head);
	assert_int_equal(size_list,3);
	
	
	//DESTROY_LIST(data_list_head, struct info, node);
		
}


static void test_list_destroy(void **state) {
	
	assert_int_equal(1,1);			
}


static void test_list_getcontainerof_macro(void **state) {
	
	struct info * data, *data_1;
	
    LIST_NODE_T *data_list_head = NULL;
    
    data = (struct info*)malloc(sizeof(struct info));
  	data->data = 10;
	LIST_HEAD_INIT(&data->node);
    
    data_1 = (struct info*)malloc(sizeof(struct info));
	data_1->data = 20;
     
	data_list_head = insert_at_end(data_list_head,&data->node);
	data_list_head = insert_at_end(data_list_head,&data_1->node);
	
	assert_ptr_equal(GET_LIST_CONTAINER(data_list_head,struct info,node),data);
	assert_ptr_equal(GET_LIST_CONTAINER(data_list_head->next,struct info,node),data_1);
	
	//DESTROY_LIST(data_list_head, struct info, node);			
			
}
*/
int main() {
	
	const struct CMUnitTest tests[] = {
	
	cmocka_unit_test(test_list_insert_begin),
	cmocka_unit_test(test_list_insert_end),
	cmocka_unit_test(test_list_insert_pos),
	cmocka_unit_test(test_list_delete_begin),
	cmocka_unit_test(test_list_delete_end),
	cmocka_unit_test(test_list_delete_pos),
	cmocka_unit_test(test_list_size),
	cmocka_unit_test(test_list_destroy),
	cmocka_unit_test(test_list_getcontainerof_macro)		
	
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
