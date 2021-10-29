// Copyright 2021 @Profeanu Ioana, 313CA
// source file containing the server's hashtable implementation

#include <stdlib.h>
#include <string.h>

#include "server.h"

// hash function used for hashing the key values
unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

// function which initialises the server memory, which is a hashtable,
// and returns the newly created server
server_memory* init_server_memory() {
	// allocate memory for the hashtable
	server_memory *server = malloc(sizeof(server_memory));
	DIE(server == NULL, "Error");

	// initialise hashtable metadata
	server->hmax = HMAX;
	server->size = 0;

	// allocate memory for the array of cdlls
	server->buckets = calloc(HMAX, sizeof(cdll_list*));
	DIE(server->buckets == NULL, "Error");

	// create each cdll for each of the array's elements
	for (int i = 0; i < (int)server->hmax; i++)
		server->buckets[i] = create_list(sizeof(key_value_pair));

	return server;
}

// function which stores a key-value pair in the server memory
void server_store(server_memory* server, char* key, char* value) {
	// calculate the hash value of the key modulo bucket size
	unsigned int hash_value = hash_function_key(key) % server->hmax;

	int key_size = strlen(key) + 1;
	int value_size = strlen(value) + 1;

	// iterate through the bucket list linked to the hash value of the key
	// check if the key already exists; if so, renew its value
	cdll_node* current = server->buckets[hash_value]->head;

	for (int i = 0; i < (int)server->buckets[hash_value]->size; i++) {
		// compare the given key with the key in the bucket
		if (strncmp(((key_value_pair*)(current->data))->key,
			key, strlen(key)) == 0) {
			memcpy(((key_value_pair*)(current->data))->
					value, value, value_size);
			return;
		}
		current = current->next;
	}

	// if the key doesn't exist, create a new key_value_pair element
	// and initialise its data with the given key and value
	key_value_pair* new_entry = malloc(sizeof(key_value_pair));
	DIE(new_entry == NULL, "Error");

	new_entry->key = malloc(key_size);
	DIE(new_entry->key == NULL, "Error");
	if(new_entry->key) {
		memcpy(new_entry->key, key, key_size);
	}

	new_entry->value = malloc(value_size);
	DIE(new_entry->value == NULL, "Error");
	if (new_entry->value) {
		memcpy(new_entry->value, value, value_size);
	}

	// add the newly created element to the bucket list
	// linked to the hash value of the key
	add_node(server->buckets[hash_value], server->
			buckets[hash_value]->size, new_entry);
	server->size++;
	free(new_entry);
}

// function which removes a key-value pair from the server, being given
// only the key of the entry
void server_remove(server_memory* server, char* key) {
	// calculate the hash value of the key modulo bucket size
	unsigned int hash_value = hash_function_key(key) % server->hmax;

	// iterate through the bucket list linked to the hash value of the key
	// in order to find the wanted key, get its position and remove the key
	int position = 0;
	cdll_node* current = server->buckets[hash_value]->head;

	for (int i = 0; i < (int)server->buckets[hash_value]->size; i++) {
		// compare the given key with the key in the bucket
		if (strncmp(((key_value_pair*)(current->data))->key,
			key, strlen(key)) == 0) {
			cdll_node* removed = remove_node(server->buckets[hash_value],
								 position);
			// free the memory of the removed node
			free(((key_value_pair*)(removed->data))->key);
			free(((key_value_pair*)(removed->data))->value);
			free(removed->data);
			free(removed);
			server->size--;
			return;
		}
		position++;
		current = current->next;
	}
}

// function which looks for the value stored at a given key
// and if found, returns it
char* server_retrieve(server_memory* server, char* key) {
	// calculate the hash value of the key modulo bucket size
	unsigned int hash_value = hash_function_key(key) % server->hmax;

	// iterate through the bucket list linked to the hash value of the key
	// in order to find the key
	// if found, the value stored is returned
	cdll_node* current = server->buckets[hash_value]->head;

	for (int i = 0; i < (int)server->buckets[hash_value]->size; i++) {
		// compare the given key with the key in the bucket
		if (strncmp(((key_value_pair*)(current->data))->key,
			key, strlen(key)) == 0) {
			return ((key_value_pair*)(current->data))->value;
		}
		current = current->next;
	}
	// if the key doesn't exist, return NULL
	return NULL;
}

// function which frees the memory of the serve
void free_server_memory(server_memory* server) {
	// iterate through each element of the cdll array
	for (int i = 0; i < (int)server->hmax; i++) {
		// iterate through the bucket list of the current element in the array
		// and free the data of each node
		cdll_node* current = server->buckets[i]->head;
		for (int j = 0; j < (int)server->buckets[i]->size; j++) {
			current = server->buckets[i]->head;
			if (current != server->buckets[i]->tail) {
				server->buckets[i]->head = server->buckets[i]->head->next;
			}
			free(((key_value_pair*)(current->data))->key);
			free(((key_value_pair*)(current->data))->value);
			free(current->data);
			free(current);
		}
		// free the list
		free(server->buckets[i]);
	}

	// free the array of buckets and the server
	free(server->buckets);
	free(server);
}
