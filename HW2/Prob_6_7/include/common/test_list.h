/**
* @file - test_file.h
* @brief - Header file for declaration of the test case functions on doubly linked list
* 
* @author Gunj, University of Colorado Boulder
* @date 4/2/2018
**/

#ifndef __TEST_LIST_H__
#define __TEST_LIST_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cmocka.h>
#include <stdarg.h>
#include <setjmp.h>
#include "doublyLinkedList.h"
#include <string.h>
#include <malloc.h>


/**
* @brief - 
* @return void
**/
	
	
struct info{

	int32_t data;
	LIST_NODE_T node;

};

#define DELETE_INFO_STRUCT(p_list_node) \
    free(GET_LIST_CONTAINER(p_list_node,struct info, node))

#define DESTROY_INFO_STRUCT(p_list_head)    \
    DESTROY_LIST(p_list_head,struct info,node)

void test_list_insert_begin();

void test_list_insert_end();

void test_list_insert_pos();

void test_list_delete_begin();

void test_list_delete_end();

void test_list_delete_pos();

void test_list_size();

void test_list_destroy();

void test_list_getcontainerof_macro();

#endif
