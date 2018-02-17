#include <stdlib.h>

#include "letterParser.h"


static letterType g_max1_E = 0;
static letterType g_max2_E = 0;
static letterType g_max3_E = 0;
	
static uint32_t g_max1_C = 0;
static uint32_t g_max2_C = 0;
static uint32_t g_max3_C = 0;
	

int parser_parse(const char *filename)
{
	FILE *fp = NULL;
	if( (fp = fopen(filename,"r")) != 0 )
	{
		/* LOG OPEN ERROR */
		return 1;
	}
	
	PARSER_LETTER_T *parsedListHead = NULL;
	letterType parsedChar;
	int ret = fread(&parsedChar,sizeof(letterType),1,fp);
	while(ret == 1)
	{
		
		parsedListHead = parser_insert(parsedListHead, parsedChar);
	
		ret = fread(&parsedChar,sizeof(letterType),1,fp);
	}
	if(feof(fp))
	{
		/*LOG EOF */
	}
	else if(ferror(fp))
	{
		/* LOF ERROR */
		return 1;
	}	
	
	/*Now we have the list ready with all the elements in the list */
	//letterType max[3] = parser_getMaxThreeElements(parsedListHead);
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
		
		/* As this is the first element in the list, we can update the globals max1 directly */ 
		g_max1_E = letter;
		g_max1_C = 1;	
		
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
				updateMax_onFly(list_itr);
				break;
			}
			
		}
		/*	Improve - If not found, we are already at the end of the list, so we can just append the new node after
		*	list_itr used above. Expand the scope of that iterator and we are good to go to add the new node
		*	But can improve after wards. Going with the first intuition.	
		*/
		if(!found)
		{
			PARSER_LETTER_T newListNode = 
			{
				.letterElement = letter,
				.letterCount = 1,
				.selfNode.next = NULL,
				.selfNode.prev = NULL,
			};
			
			updateMax_onFly(&newListNode);
			
			/* Using insert at beginning as to avoid traversing to the end */
			return GET_LIST_CONTAINER(insert_at_beginning(&parsedListHead->selfNode,&newListNode.selfNode),PARSER_LETTER_T,selfNode);		
		}
		else
			return parsedListHead;
	}

}

/* Should call this before starting of the parse for safety as the variables are static.
`Else call the parser_getMaxThreeElements() function
*/
void resetGlobalMax()
{
	g_max1_C = 0;
	g_max2_C = 0;
	g_max3_C = 0;
	
	g_max1_E = 0;
	g_max2_E = 0;
	g_max3_E = 0;

}
/* Using  this in the parser_insert for_each loop to optimize the time */
void updateMax_onFly(PARSER_LETTER_T *parsedListNode)
{
	if(parsedListNode->letterCount > g_max1_C)
	{
		g_max1_C = parsedListNode->letterCount;
		g_max2_C = g_max1_C;
		g_max3_C = g_max2_C;
		
		g_max1_E = parsedListNode->letterElement;
		g_max2_E = g_max1_E;
		g_max3_E = g_max2_E;
		
	}
	else if(parsedListNode->letterCount > g_max2_C)
	{
		g_max2_C = parsedListNode->letterCount;
		g_max3_C = g_max2_C;
		
		g_max2_E = parsedListNode->letterElement;
		g_max3_E = g_max2_E;		
	}
	else if(parsedListNode->letterCount > g_max3_C)
	{
		g_max3_C = parsedListNode->letterCount;
		
		g_max3_E = parsedListNode->letterElement;
	}


}

letterType* parser_getMaxThreeGlobalElements()
{
	static letterType max_arr[3] = {0};
	max_arr[0] = g_max1_E;
	max_arr[1] = g_max2_E;
	max_arr[2] = g_max3_E;
	
	return max_arr;
}

uint32_t* parser_getMaxThreeGlobalElementsCount()
{
	static uint32_t max_arr[3] = {0};
	max_arr[0] = g_max1_C;
	max_arr[1] = g_max2_C;
	max_arr[2] = g_max3_C;
	
	return max_arr;
}

letterType* parser_getMaxThreeElements(PARSER_LETTER_T *parsedListHead)
{
	PARSER_LETTER_T *list_itr = GET_LIST_CONTAINER(parsedListHead->selfNode.next, PARSER_LETTER_T , selfNode);
	
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
