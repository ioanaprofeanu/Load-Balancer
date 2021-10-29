// Copyright 2021 @Profeanu Ioana, 313CA
// header linked to the source file containing
// generic functions for list processing

#ifndef CIRCULAR_DOUBLY_LINKED_LIST_H_
#define CIRCULAR_DOUBLY_LINKED_LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "utils.h"

// we create two generic data structures, which
// can be used for any data type

// generic data structure which contains the components of a list
// node, with its generic data, and two pointers which point to
// its list neighbours
typedef struct cdll_node cdll_node;
struct cdll_node
{
	void* data;
	cdll_node* next;
	cdll_node* prev;
};

// generic data structure which contains the metadata of the list
typedef struct cdll_list cdll_list;
struct cdll_list
{
	cdll_node* head;
	cdll_node* tail;
	unsigned int size;
	unsigned int data_size;
};


cdll_list* create_list(unsigned int data_size);

cdll_node* get_node(cdll_list* list, unsigned int n);

void add_node(cdll_list* list, unsigned int n, const void* data);

cdll_node* remove_node(cdll_list* list, unsigned int n);

void cdll_free(cdll_list** pp_list);

#endif  // CIRCULAR_DOUBLY_LINKED_LIST_H_
