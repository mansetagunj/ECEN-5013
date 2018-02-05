/**
* @file - test.c
* @brief - Main file containing function calls for all unit test cases
* 
* @author Gunj/Ashish University of Colorado Boulder
* @date 27/10/2017
**/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include "test_list.h"

int main() {
	const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_memmove_nullPtr),
		
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
