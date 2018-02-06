/*
 * @File doublyLinkedList.c
 *
 * @Created on: 02-Feb-2018
 * @Author: Gunj Manseta
 */

#include "doublyLinkedList.h"

void LIST_HEAD_ALLOCATE(LIST_NODE_T **list_node)
{
	*list_node = (LIST_NODE_T*)malloc(sizeof(LIST_NODE_T));
}

/**
 * @brief Function to initialize the head node which takes head list pointer
 * @param [in] LIST_NODE_T* list head
 * @return void
 **/
void LIST_HEAD_INIT(LIST_NODE_T *list_node)
{
	if(!list_node)
	{
		LIST_HEAD_ALLOCATE(&list_node);
	}

	list_node->prev = NULL;
	list_node->next = NULL;

}

/**
 * @brief Inserts the node at the beginning and the new node becomes the head
 * @param [in] LIST_NODE_T* list head
 * @param [in] LIST_NODE_T* new node
 * @return LIST_NODE_T* new head
 */
LIST_NODE_T* insert_at_beginning(LIST_NODE_T *list_head, LIST_NODE_T *new_node)
{
	if(list_head)
	{
		new_node->prev 	= NULL;
		new_node->next 	= list_head;
		list_head->prev = new_node;
	}
	else
	{
		LIST_HEAD_INIT(new_node);
	}

	return new_node;
}

/**
 * @brief Inserts the node at the end of the list
 * @param [in] LIST_NODE_T* list head
 * @param [in] LIST_NODE_T* new node
 * @return LIST_NODE_T* head
 */
LIST_NODE_T* insert_at_end(LIST_NODE_T *list_head, LIST_NODE_T *new_node)
{
	if(list_head)
	{
		LIST_NODE_T *list_itr = list_head;

		//while there no element in the list. i.e. traversing to the end of the list
		while(list_itr->next)
			list_itr = list_itr->next;

		new_node->prev = list_itr;
		new_node->next = list_itr->next;
		list_itr->next = new_node;

		return list_head;
	}
	else
	{
		LIST_HEAD_INIT(new_node);
		return new_node;
	}

}

/**
 * @brief Inserts the node at the position given from the base_node i.e. @{base + position}
 * @param [in] LIST_NODE_T* base node
 * @param [in] LIST_NODE_T* new node
 * @param [in] int position
 * @return LIST_NODE_T* head
 */
LIST_NODE_T* insert_at_position(LIST_NODE_T *base_node, LIST_NODE_T *new_node, int pos)
{
	if(base_node)
	{
		LIST_NODE_T *list_itr = base_node;
		while(((--pos) > 0) && list_itr->next)
		{
			list_itr = list_itr->next;
		}

		//the list_itr points to the node after which new node should be entered
		new_node->prev 			= list_itr;
		new_node->next 			= list_itr->next;
		list_itr->next ? list_itr->next->prev = new_node : 0;
		list_itr->next 			= new_node;

		//traversing to the head
		list_itr = base_node;
		while(list_itr->prev)
			list_itr = list_itr->prev;

		return list_itr;
	}
	else
	{
		LIST_HEAD_INIT(new_node);
		return new_node;
	}

}

/**
 * @brief Delete the node at the beginning of the list so the head gets updated
 * The deleted node is head, the deleted node pointer is returned as it is required 
 * to free the containing structure using the GET_LIST_CONTAINER macro.
 * We are taking the pointer to the head pointer, so the list_head gets updated
 * @param [in][out] LIST_NODE_T** address of head node pointer
 * @return LIST_NODE_T* deleted node
 */
LIST_NODE_T* delete_from_beginning(LIST_NODE_T **list_head)
{
	if(*list_head)
	{

                (*list_head)->next->prev = NULL;
                LIST_NODE_T *deletedNode = *list_head;
                *list_head = (*list_head)->next;
                return deletedNode;
	}
	else
		return NULL;
}


/**
 * @brief Delete the node at the end of the list so the tail gets updated
 * The deleted node pointer is returned as it will be required to free the entire list node as well
 * as the containing structure using the GET_LIST_CONTAINER macro.
 * @param [in] LIST_NODE_T* head node
 * @return LIST_NODE_T* The node that was deleted
 */
LIST_NODE_T* delete_from_end(LIST_NODE_T *list_head)
{
	if(list_head)
	{
		//as we are deleting the tail itself, we need to update the next of tail->prev as null

		LIST_NODE_T *list_itr = list_head;
		//while there no element in the list. i.e. traversing to the end of the list
		while(list_itr->next)
			list_itr = list_itr->next;

		list_itr->prev->next = NULL;

		//returning the tail as it will be required by the callee to free list_node as well the containing strucutre
		return list_itr;
	}
	else
		return NULL;
}

/**
 * @brief Delete the node at the specified pos from the base_node
 * The deleted node pointer is returned as it will be required to free the entire list node as well
 * as the containing structure using the GET_LIST_CONTAINER macro.
 * @param [in] LIST_NODE_T* base node
 * @param [in] int position from base node
 * @return LIST_NODE_T* The node that was deleted
 */
LIST_NODE_T* delete_from_position(LIST_NODE_T *base_node, int pos)
{
	if(base_node)
	{
		LIST_NODE_T *list_itr = base_node->next;
		while(((--pos) > 0 ) && list_itr)
		{
			list_itr = list_itr->next;
		}
		//the list_itr points to the node before the node should be deleted
		list_itr->prev->next = list_itr->next;
		list_itr->next->prev = list_itr->prev;

		//returning the tail as it will be required by the callee to free list_node as well the containing strucutre
		return list_itr;
	}
	else
		return NULL;
}

/**
 * @brief Gives the size of the list from the given node
 * @param [in] LIST_NODE_T* hnode
 * @return size_t The size of list from the given node
 */
size_t size(LIST_NODE_T *node)
{
	size_t list_size = 0;
	while(node)
	{
		list_size++;
		node = node->next;
	}

	return list_size;

}



