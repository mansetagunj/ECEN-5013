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
