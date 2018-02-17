#ifndef __LETTER_PARSER_H__
#define __LETTER_PARSER_H__

#include "doublyLinkedList.h"

#ifdef _UNICODE
typedef letterType uint16_t
#else
typedef letterType char
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

int parser_insert(LIST_NODE_T listHead, letterType letter);

letterType* parser_getMaxThree(LIST_NODE_T listHead);







#endif
