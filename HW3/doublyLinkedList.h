/*
 * @File doublyLinkedList.h
 *
 * @Created on: 02-Feb-2018
 * @Author: Gunj Manseta
 */

#ifndef DOUBLYLINKEDLIST_H_
#define DOUBLYLINKEDLIST_H_

#include <stdlib.h>
#include <stddef.h>
#include <malloc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct list_node{

	struct list_node *next;
	struct list_node *prev;

}LIST_NODE_T;

//This macro gives the container pointer of the list_node structure.
#define __GET_LIST_CONTAINER(member_addr,container_type,container_member) \
	((container_type*)(((char*)member_addr) - ((size_t)(&((container_type*)0)->container_member))))

#define GET_LIST_CONTAINER(member_addr,container_type,container_member) \
		__GET_LIST_CONTAINER(member_addr,container_type,container_member)

//Creates a List head structure locally and initializes it. This is used to keep the head of the list
//#define LIST_HEAD_PTR(p_name)	(LIST_NODE_T p_name = NULL;)

#define LIST_HEAD(name)			LIST_NODE_T name = {NULL, NULL}

/**
 * @brief Expands to a for loop used to traverse through the list nodes
 * @param itr [pointer]The iterater starting from head to the end of list nodes. Can be used to act on each list node
 * @param list_head the list head to start with
 */	
#define LIST_FOR_EACH(itr,list_head) \
	for(itr = list_head; itr != NULL; itr = itr->next)
	
	
/**
 * @brief Expands to a for loop used to traverse through the container of the list nodes
 * @param itr [pointer] The container iterater starting from head to the end of list nodes.
 * @param list_head the list head to start with
 * @param container_listmember The name of the LIST_NODE_T member in the container 	
 */	
#define LIST_FOR_EACH_ENTRY(itr, list_head, container_listmember) \
	for(itr = GET_LIST_CONTAINER(list_head, typeof(*itr), container_listmember); \
	itr->container_listmember != NULL; \
	itr = GET_LIST_CONTAINER(itr->container_listmember->next, typeof(*itr), container_listmember);	\
	)



static inline void UPDATE_LIST_HEAD(LIST_NODE_T *head, LIST_NODE_T *new_head)
{
	head = new_head;
}

//macro to initialize the head node which takes a local copy created statically
#define INIT_LIST_HEAD()	{NULL, NULL}


#define DESTROY_LIST(list_head_addr,container_type, container_member) 	\
{	typeof(list_head_addr) list_itr = list_head_addr;	\
	while(list_itr)										\
	{container_type *container = GET_LIST_CONTAINER(list_itr,container_type,container_member);	\
        list_itr = list_itr->next;free(container);}                          \
        list_head_addr = NULL;\
}

void LIST_HEAD_ALLOCATE(LIST_NODE_T **list_node);

/**
 * @brief Function to initialize the head node which takes head list pointer
 * @param [in] LIST_NODE_T* list head
 * @return void
 **/
void LIST_HEAD_INIT(LIST_NODE_T *list_node);


/**
 * @brief Inserts the node at the beginning and the new node becomes the head
 * @param [in] LIST_NODE_T* list head
 * @param [in] LIST_NODE_T* new node
 * @return LIST_NODE_T* new head
 */
LIST_NODE_T* insert_at_beginning(LIST_NODE_T *list_head, LIST_NODE_T *new_node);

/**
 * @brief Inserts the node at the end of the list
 * @param [in] LIST_NODE_T* list head
 * @param [in] LIST_NODE_T* new node
 * @return LIST_NODE_T* head
 */
LIST_NODE_T* insert_at_end(LIST_NODE_T *list_head, LIST_NODE_T *new_node);

/**
 * @brief Inserts the node at the position given from the base_node i.e. @{base + position}
 * @param [in] LIST_NODE_T* base node
 * @param [in] LIST_NODE_T* new node
 * @param [in] int position
 * @return LIST_NODE_T* head
 */
LIST_NODE_T* insert_at_position(LIST_NODE_T *base_node, LIST_NODE_T *new_node, int pos);

/**
 * @brief Delete the node at the beginning of the list so the head gets updated
 * The deleted node is head, the deleted node pointer is returned as it is required 
 * to free the containing structure using the GET_LIST_CONTAINER macro.
 * We are taking the pointer to the head pointer, so the list_head gets updated
 * @param [in][out] LIST_NODE_T** address of head node pointer
 * @return LIST_NODE_T* deleted node
 */
LIST_NODE_T* delete_from_beginning(LIST_NODE_T **list_head);


/**
 * @brief Delete the node at the end of the list so the tail gets updated
 * The deleted node pointer is returned as it will be required to free the entire list node as well
 * as the containing structure using the GET_LIST_CONTAINER macro.
 * @param [in] LIST_NODE_T* head node
 * @return LIST_NODE_T* The node that was deleted
 */
LIST_NODE_T* delete_from_end(LIST_NODE_T *list_head);

/**
 * @brief Delete the node at the specified pos from the base_node
 * The deleted node pointer is returned as it will be required to free the entire list node as well
 * as the containing structure using the GET_LIST_CONTAINER macro.
 * @param [in] LIST_NODE_T* head node
 * @param [in] int position
 * @return LIST_NODE_T* The node that was deleted
 */
LIST_NODE_T* delete_from_position(LIST_NODE_T *base_node, int pos);

/**
 * @brief Gives the size of the list from the given node
 * @param [in] LIST_NODE_T* hnode
 * @return size_t The size of list from the given node
 */
size_t size(LIST_NODE_T *node);

/**
 * @brief Sorts the list from the head node
 * @param [in] LIST_NODE_T* node
 * @param [in] int8_t reverse If true, sorts ascending
 * @return LIST_NODE_T* The new head of the node as the list got modified due to sorting
 */
LIST_NODE_T* sort(LIST_NODE_T *node, int8_t reverse){}

#ifdef __cplusplus
}
#endif


#endif /* DOUBLYLINKEDLIST_H_ */
