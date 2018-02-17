#ifndef __LETTER_PARSER_H__
#define __LETTER_PARSER_H__

#include <stdint.h>

#include "doublyLinkedList.h"

#ifdef _UNICODE
typedef int16_t letterType;
#else
typedef char letterType;
#endif


typedef struct parser_letter{

	letterType letterElement;
	uint32_t letterCount;
	LIST_NODE_T selfNode;
	
}PARSER_LETTER_T;

#define DELETE_LETTER_STRUCT(p_list_node) \
    free(GET_LIST_CONTAINER(p_list_node,struct info, node))

#define DESTROY_LETTER_LIST(p_list_head)    \
    DESTROY_LIST(p_list_head,struct info,node)


int parser_parse(const char *filename);

PARSER_LETTER_T* parser_insert(PARSER_LETTER_T *parsedListHead, letterType letter);

void resetGlobalMax();

void updateMax_onFly(PARSER_LETTER_T *parsedListNode);

letterType* parser_getMaxThreeGlobalElements();

uint32_t* parser_getMaxThreeGlobalElementsCount();

letterType* parser_getMaxThreeElements(PARSER_LETTER_T *parsedListHead);







#endif
