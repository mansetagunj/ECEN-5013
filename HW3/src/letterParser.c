#include <stdlib.h>

#include "letterParser.h"


static inline int isUpperAlpha(letterType letter)
{
	return ((letter > 64 && letter < 91) ?  1 :  0);
}

static inline letterType toLowerAlpha(letterType letter)
{
	return ((isUpperAlpha(letter)) ?  (letter + 32) : letter);
}

int parser_parse(const char *filename, PARSER_LETTER_T  **toBeParsedListHead)
{
	FILE *fp = NULL;
	if( (fp = fopen(filename,"r")) == NULL )
	{
		/* LOG OPEN ERROR */
		printf("File open Error\n");
		return 1;
	}
	
	PARSER_LETTER_T *parsedListHead = *toBeParsedListHead;
	letterType parsedChar;
	int ret = fread(&parsedChar,sizeof(letterType),1,fp);
	while(ret == 1)
	{
		
		parsedListHead = parser_insert(parsedListHead, toLowerAlpha(parsedChar));
	
		ret = fread(&parsedChar,sizeof(letterType),1,fp);
	}
	if(feof(fp))
	{
		/*LOG EOF */
		printf("END OF FILE\n");
	}
	else if(ferror(fp))
	{
		/* LOF ERROR */
		printf("FILE ERROR\n");
		return 1;
	}	
	
	*toBeParsedListHead = parsedListHead;
	return 0;
}


PARSER_LETTER_T* parser_insert(PARSER_LETTER_T *parsedListHead, letterType letter)
{
	/*We go the a list head having no elements. So we initialized the  head with the new letter */
	if(NULL == parsedListHead)
	{
		/* Creating the linked ist */
		parsedListHead = (PARSER_LETTER_T *)malloc(sizeof(PARSER_LETTER_T));
		parsedListHead->letterElement = letter;
		parsedListHead->letterCount = 1;
		LIST_HEAD_INIT(&parsedListHead->selfNode);
		
		return parsedListHead;
	}
	else
	{
		/* 
		* We traverse the list and find the occurence of the letter. 
		* If found, we increment the letterCount of that node
		* else we add a new node at the end of list 
		*/
		PARSER_LETTER_T *list_itr = parsedListHead;
		uint8_t found = 0;
		LIST_FOR_EACH_ENTRY(list_itr, &list_itr->selfNode, selfNode)
		{
			if(list_itr->letterElement == letter)
			{
				list_itr->letterCount++;
				found = 1;
				break;
			}
			
		}
		/*	Improve - If not found, we are already at the end of the list, so we can just append the new node after
		*	list_itr used above. Expand the scope of that iterator and we are good to go to add the new node
		*	But can improve after wards. Going with the first intuition.	
		*/
		if(!found)
		{
			PARSER_LETTER_T *newListNode = (PARSER_LETTER_T*)malloc(sizeof(PARSER_LETTER_T));
			
			newListNode->letterElement = letter;
			newListNode->letterCount = 1;
			newListNode->selfNode.next = NULL;
			newListNode->selfNode.prev = NULL;

			
			/* Using insert at beginning as to avoid traversing to the end */
			return GET_LIST_CONTAINER(insert_at_beginning(&parsedListHead->selfNode,&newListNode->selfNode),PARSER_LETTER_T,selfNode);		
		}
		else
			return parsedListHead;
	}

}

letterType* parser_getMaxThreeElements(PARSER_LETTER_T *parsedListHead)
{
	PARSER_LETTER_T *list_itr = parsedListHead; //GET_LIST_CONTAINER(parsedListHead->selfNode.next, PARSER_LETTER_T , selfNode);
	
	static letterType max_arr[3] = {0};
	
	letterType max1_E = parsedListHead->letterElement;
	letterType max2_E = 0;
	letterType max3_E = 0;
	
	uint32_t max1_C = parsedListHead->letterCount;
	uint32_t max2_C = 0;
	uint32_t max3_C = 0;
	
	max_arr[0] = 0;
	max_arr[1] = 0;
	max_arr[2] = 0;
	
	LIST_FOR_EACH_ENTRY(list_itr, &list_itr->selfNode, selfNode)
	{
		if(list_itr->letterCount > max1_C)
		{
			max1_C = list_itr->letterCount;
			max2_C = max1_C;
			max3_C = max2_C;
			
			max1_E = list_itr->letterElement;
			max2_E = max1_E;
			max3_E = max2_E;
			
		}
		else if(list_itr->letterCount > max2_C)
		{
			max2_C = list_itr->letterCount;
			max3_C = max2_C;
			
			max2_E = list_itr->letterElement;
			max3_E = max2_E;		
		}
		else if(list_itr->letterCount > max3_C)
		{
			max3_C = list_itr->letterCount;
			
			max3_E = list_itr->letterElement;
		}
		
	}

	max_arr[0] = max1_E;
	max_arr[1] = max2_E;
	max_arr[2] = max3_E;
	
	return max_arr;

}

size_t get_occurenceN_letters(PARSER_LETTER_T *parsedListHead, letterType **inout_elemArray, uint32_t occurenceN)
{
	PARSER_LETTER_T *list_itr = parsedListHead;
	size_t i = 0;
	if(NULL == *inout_elemArray)
	{
		*inout_elemArray = (letterType*)malloc(sizeof(letterType)*10);
		if(NULL == *inout_elemArray)
		{
			/*LOG ERROR*/
			printf("MALLOC ERROR\n");
			return 0;
		}
	}
	
	LIST_FOR_EACH_ENTRY(list_itr, &list_itr->selfNode, selfNode)
	{
		if(list_itr->letterCount == occurenceN)	
		{
			*(*inout_elemArray+i) = list_itr->letterElement;
			i++;
		}
	}
	
	return i;
}

void cleanup_parser(PARSER_LETTER_T *parsedListHead)
{
	LIST_FOR_EACH_ENTRY(parsedListHead, &parsedListHead->selfNode, selfNode)
	{
		free(parsedListHead);
	}	
}


