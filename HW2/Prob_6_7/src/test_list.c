/**
* @file - test_file.c
* @brief - Implementation file containing function definitions for test cases on doubly linked list operations 
* 
* @author Gunj, University of Colorado Boulder
* @date 4/2/2018
**/

#include "test_list.h"


void test_list_insert_begin(void) {
	
	struct info * data, *data_1, *data_2;
    
    data = (struct info*)malloc(sizeof(struct info));
  	data->data = 10;
	LIST_HEAD_INIT(&data->node);
    LIST_NODE_T *data_list_head = &data->node;
    
    data_1 = (struct info*)malloc(sizeof(struct info));
	data_1->data = 20;
        
    data_2 = (struct info*)malloc(sizeof(struct info));
	data_2->data = 30;

	data_list_head = insert_at_beginning(data_list_head,&data_2->node);
	
	assert_ptr_equal(GET_LIST_CONTAINER(data_list_head,struct info,node),GET_LIST_CONTAINER(&data_2->node,struct info,node));
	
	DESTROY_LIST(data_list_head, struct info, node);
	
}

void test_list_insert_end(void) {

	struct info * data, *data_1, *data_2;
    
    data = (struct info*)malloc(sizeof(struct info));
  	data->data = 10;
	LIST_HEAD_INIT(&data->node);
    LIST_NODE_T *data_list_head = &data->node;
    
    data_1 = (struct info*)malloc(sizeof(struct info));
	data_1->data = 20;
        
    data_2 = (struct info*)malloc(sizeof(struct info));
	data_2->data = 30;

	data_list_head = insert_at_end(data_list_head,&data_2->node);
	
	
	
	assert_ptr_equal(GET_LIST_CONTAINER(data_list_head,struct info,node),GET_LIST_CONTAINER(&data_2->node,struct info,node));
	
	DESTROY_LIST(data_list_head, struct info, node);
	
	assert_ptr_equal(cast_ptr_to_largest_integral_type(result),NULL);	
}

void test_list_insert_pos(void) {
	
	assert_ptr_equal(cast_ptr_to_largest_integral_type(result),NULL);	
}

void test_list_delete_begin(void) {
	
	assert_ptr_equal(cast_ptr_to_largest_integral_type(result),NULL);	
}

void test_list_delete_end(void) {
	
	assert_ptr_equal(cast_ptr_to_largest_integral_type(result),NULL);	
}

void test_list_delete_pos(void) {
	
	assert_ptr_equal(cast_ptr_to_largest_integral_type(result),NULL);	
}

void test_list_insert_end(void) {
	
	assert_ptr_equal(cast_ptr_to_largest_integral_type(result),NULL);	
}

void test_list_size(void) {
	
	assert_ptr_equal(cast_ptr_to_largest_integral_type(result),NULL);	
}

void test_list_insert_end(void) {
	
	assert_ptr_equal(cast_ptr_to_largest_integral_type(result),NULL);	
}

void test_list_destroy(void) {
	
	assert_ptr_equal(cast_ptr_to_largest_integral_type(result),NULL);	
}

void test_list_insert_end(void) {
	
	assert_ptr_equal(cast_ptr_to_largest_integral_type(result),NULL);	
}

void test_list_getcontainerof_macro(void) {
	
	assert_ptr_equal(cast_ptr_to_largest_integral_type(result),NULL);	
}




