// Copyright 2021 @Profeanu Ioana, 313CA
// header linked to the source file containing
// the server's hashtable implementation

#ifndef SERVER_H_
#define SERVER_H_

#include "circular_doubly_linked_list.h"

#define HMAX 1000
#define MAX_HASH 100000

// key-value data structure which will represent the data of a node within
// each list of the cdlls array
typedef struct key_value_pair key_value_pair;
struct key_value_pair {
	void *key;
	void *value;
};

// hashtable data structure which stores the data of a server
typedef struct server_memory server_memory;
struct server_memory {
	// array of cdlls; buckets[i] represents the bucket for the i hash and
	// stores a list of key_value_pair nodes
	cdll_list **buckets;
	// total number of nodes stored in all buckets
	unsigned int size;
	// number of buckets
	unsigned int hmax;
};

// hashs function for keys
unsigned int hash_function_key(void *a);

// function which initialises and returns a server_memory element
server_memory* init_server_memory();


// server_store() - Stores a key-value pair to the server.
// @arg1: Server which performs the task.
// @arg2: Key represented as a string.
// @arg3: Value represented as a string.
void server_store(server_memory* server, char* key, char* value);

// server_remove() - Removes a key-pair value from the server.
// @arg1: Server which performs the task.
// @arg2: Key represented as a string.
void server_remove(server_memory* server, char* key);

// server_remove() - Gets the value associated with the key.
// @arg1: Server which performs the task.
// @arg2: Key represented as a string.
//
// Return: String value associated with the key
//         or NULL (in case the key does not exist).
char* server_retrieve(server_memory* server, char* key);

// function which frees the memory of the server
void free_server_memory(server_memory* server);

#endif  // SERVER_H_
