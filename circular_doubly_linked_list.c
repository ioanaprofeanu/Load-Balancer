// Copyright 2021 @Profeanu Ioana, 313CA
// source file containing generic functions for list processing

#include "circular_doubly_linked_list.h"

// function which returns a pointer to a newly created
// list structure element
cdll_list* create_list(unsigned int data_size)
{
	// allocate memory for the list and its metadata
	cdll_list* list = malloc(sizeof(cdll_list));
	DIE(list == NULL, "Error\n");

	// initialize the list's metadata
	list->data_size = data_size;
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;

	return list;
}

// function which returns a pointer to the nth node of the list
cdll_node* get_node(cdll_list* list, unsigned int n)
{
	// in order to have a complexity of maximum O(n),
	// we iterate through the list until the n % list->size element,
	// because the list is circular and the values repeat themselves

	// get node at the beginning of the list
	cdll_node *current = list->head;
	int position = n % list->size;

	if ((int)n < 0) {
		return NULL;
	// if the node is the first or the last, return it without
	// iterating through the entire list
	} else {
		if (position == 0) {
			return list->head;
		}
		if (position == (int)list->size - 1) {
			return list->tail;
		}
		// otherwise, iterate until reaching the wanted node
		for (int i = 0; i < position; i++) {
			current = current->next;
		}
	}

	// return the node
	return current;
}

// function which adds a new node at the nth position
// with the given data
void add_node(cdll_list* list, unsigned int n, const void* data)
{
	if (list == NULL) {
		return;
	}

	// get node at the beginning of the list
	cdll_node *current = list->head;
	// allocate memory for the to be added node
	cdll_node *auxiliary = malloc(sizeof(cdll_node));
	DIE(auxiliary == NULL, "Error\n");

	// initialize the node and copy the input data to the data
	// component of the node
	auxiliary->next = NULL;
	auxiliary->prev = NULL;
	auxiliary->data = malloc(list->data_size);
	DIE(auxiliary->data == NULL, "Error\n");
	memcpy(auxiliary->data, data, list->data_size);

	if ((int)n < 0) {
		return;

	} else if ((int)n == 0) {
		// if n == 0, we can have two cases:
		if (list->head != NULL) {
			// case one: the list is not empty and we add
			// the node at the beginning
			auxiliary->next = list->head;
			auxiliary->prev = list->tail;
			list->head->prev = auxiliary;
			list->tail->next = auxiliary;
			list->head = auxiliary;

		} else {
			// case two: the list is empty and we only need to change
			// the head and tail; both the head and the tail are the
			// new added element
			list->head = auxiliary;
			list->tail = auxiliary;
			auxiliary->next = list->head;
			auxiliary->prev = list->head;
		}

	} else if ((int)n == (int)list->size) {
		// add node at the end of list
		list->tail->next = auxiliary;
		auxiliary->prev = list->tail;
		auxiliary->next = list->head;
		list->head->prev = auxiliary;
		list->tail = auxiliary;

	} else {
		// add node at an ordinary position
		// get to the node on the (n-1)th position
		// and add the new node using this node's links
		current = get_node(list, n-1);
		auxiliary->next = current->next;
		current->next->prev = auxiliary;
		auxiliary->prev = current;
		current->next = auxiliary;
	}

	// increase list size
	list->size++;
}

// function which removes the links of a node on a given position,
// returning a pointer this node, in order for the memory to be freed
// by the function caller
cdll_node* remove_node(cdll_list* list, unsigned int n)
{
	// get node at the beginning of the list
	cdll_node *current = list->head;
	// auxiliary node in which we will store the to be removed node
	cdll_node *auxiliary;

	// verify conditions for the removal to be done
	if (list == NULL) {
		return NULL;
	}

	if ((int)n < 0) {
		return NULL;

	} else if (list->head == NULL) {
		return NULL;

	} else if ((int)n == 0) {
		// delete the node at the beginning
		auxiliary = list->head;
		list->head = auxiliary->next;
		list->head->prev = list->tail;
		list->tail->next = list->head;
		list->size--;
		if (list->size == 0) {
			list->head = NULL;
			list->tail = NULL;
		}
		return auxiliary;

	} else if ((int)n == (int)list->size - 1) {
		// delete the node at the end
		auxiliary = list->tail;
		list->tail = auxiliary->prev;
		list->tail->next = list->head;
		list->head->prev = list->tail;
		list->size--;
		return auxiliary;

	} else {
		// add node at an ordinary position
		// get to the node on the nth position
		// and remove its links
		current = get_node(list, n);
		current->prev->next = current->next;
		current->next->prev = current->prev;
		list->size--;
		return current;
	}
}

// function which frees the memory of a given list
void
cdll_free(cdll_list** pp_list)
{
	if (pp_list == NULL || *pp_list == NULL) {
		return;
	}

    // iterate through the list and free the data of the node and the node
    // for each step, modify the head of the list
	cdll_node *current = (*pp_list)->head;

    for (int i = 0; i < (int)(*pp_list)->size; i++) {
        current = (*pp_list)->head;
        if (current != (*pp_list)->tail) {
            (*pp_list)->head = (*pp_list)->head->next;
        }
        free(current->data);
        free(current);
    }

    free(*pp_list);
}
