// Copyright 2021 @Profeanu Ioana, 313CA
// source file used for implementing the load balancer's
// functionality and commands

#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"

struct load_balancer {
	// array of servers hashtables
	// servers_ht[i] represents the server of the i server
	server_memory** servers_ht;
	// hashring represented as a cdll
    cdll_list* hashring;
};

// hash function used for hashing the server values
unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

// function which initialises the main load balancer
// and returns it
load_balancer* init_load_balancer() {
	// allocate memory for the load balancer
	load_balancer* main_server = malloc(sizeof(load_balancer));
    DIE(main_server == NULL, "Error");

	// allocate memory for the aray of servers hashtables
    main_server->servers_ht = calloc(MAX_HASH, sizeof(server_memory*));
    DIE(main_server->servers_ht == NULL, "Error");

	// create the hashring cdll
	main_server->hashring = create_list(sizeof(unsigned int));

    return main_server;
}

// auxiliary function which returns the position on which should
// a given server label be located in the hashring (the cdll hashring is
// sorted in ascending order by hash value)
unsigned int hashring_position(cdll_list* hashring, unsigned int server_label)
{
	// iterate through the cdll and compare the given server label's hash
	// with the current node's hash
	cdll_node* current = hashring->head;

	for (int i = 0; i < (int)hashring->size; i++) {
		// when finding a node with the hash greater than the
		// server label's hash, return the position
		// (the server label should be located before this node)
		if (hash_function_servers(&server_label) <=
			hash_function_servers(current->data)) {
			return i;
		}
		current = current->next;
	}

	// if there isn't any node with a hash greater than
	// the server label's hash, return the size of the cdll
	// (the server label should be located at the end of the list)
	return hashring->size;
}

// auxiliary function hich returns the position of the server
// on which should an object with a given key value be stored in
// the hashring cdll (the list is sorted in ascending order
// by hash value)
unsigned int key_hashring_position(cdll_list* hashring, char* key_value)
{
	// iterate through the cdll and compare the given key value's hash
	// with the current node's hash
	cdll_node* current = hashring->head;

	for (int i = 0; i < (int)hashring->size; i++) {
		// when finding a node with the hash greater than the
		// key value's hash, return the position
		// (the object with the given key should be stored on this server)
		if (hash_function_key(key_value) <=
			hash_function_servers(current->data)) {
			return i;
		}
		current = current->next;
	}

	// if there isn't any node with a hash greater than
	// the key value's hash, the object with the given key should be
	// stored on the first server of the hashring
	return 0;
}

// function which stores an object given by its key and value
// on the specific server it belongs to
void loader_store(load_balancer* main_server, char* key,
				  char* value, int* server_id)
{
	// get the position of the server label on which the key should be stored
	unsigned int position = key_hashring_position(main_server->hashring, key);

	// get the server label from the found position
	cdll_node* server_label = get_node(main_server->hashring, position);

	// calculate the server id and store the object on the server's hashtable
	*server_id = *(unsigned int*)server_label->data % MAX_HASH;
	server_store(main_server->servers_ht[*server_id], key, value);
}

// function which retrieves the value stored at a given key
char* loader_retrieve(load_balancer* main_server, char* key, int* server_id) {
	// get the position of the server label on which the key shoukd be found
	unsigned int position = key_hashring_position(main_server->hashring, key);

	// get the server label on the found position
	cdll_node* server = get_node(main_server->hashring, position);

	// calculate the server id and retrieve the value stored on the hashtable
	// at the given key
	*server_id = *(unsigned int*) server->data % MAX_HASH;
	return server_retrieve(main_server->servers_ht[*server_id], key);
}

// function used for the redistribution of objects
// when adding a server and its labels
void add_redistribute_objects(load_balancer* main_server, int server_id_label)
{
	// get the positon on which the server label should be stored
	// on the hashring and add it to the cdll
	unsigned int server_label_position = hashring_position(main_server->
								  hashring, server_id_label);
	add_node(main_server->hashring, server_label_position, &server_id_label);

	int server_id = server_id_label % MAX_HASH;

	// if the hashring only has one element, the objects
	// have nowhere to be redistributed
	if (main_server->hashring->size <= 1) {
		return;
	}

	// calculate the position, id and label
	// of the server label's right neighbour
	int right_label_position = server_label_position + 1;

	// verify if the server label is the last in the hashring
	// because the hashring is circular, it's neighbour is the first
	// element of the hashring
	if (right_label_position == (int)main_server->hashring->size) {
		right_label_position = 0;
	}

	cdll_node* right_id = get_node(main_server->hashring,
						  right_label_position);
	int right_id_label = *(unsigned int*)right_id->data;
	int right_server_id = right_id_label % MAX_HASH;

	if (server_id == right_server_id) {
		return;
	}

	// iterate through the right neighbour label's array of buckets
	for (int i = 0; i < (int)main_server->servers_ht[server_id]->hmax; i++) {
		// iterate through each cdll bucket
		cdll_node* current = main_server->servers_ht[right_server_id]
							->buckets[i]->head;
		int size = main_server->servers_ht[right_server_id]->buckets[i]->size;

		for (int j = 0; j < size; j++) {
			// get each key's position in the hashring
			unsigned int position_key = key_hashring_position(main_server->
						hashring, ((key_value_pair *)(current->data))->key);

			// if the position of the key is the same as the label's position,
			// store the key on the newly added server's hashtable
			if (position_key == server_label_position) {
				server_store(main_server->servers_ht[server_id],
							((key_value_pair *)(current->data))->key,
							((key_value_pair *)(current->data))->value);
			}
			current = current->next;
		}
	}
}

// function used for adding a server on the load balancer
void loader_add_server(load_balancer* main_server, int server_id)
{
	// calculate the server's labels values
	int server_label_1 = 1 * MAX_HASH + server_id;
    int server_label_2 = 2 * MAX_HASH + server_id;

	// create the hashtable of the server
    main_server->servers_ht[server_id] = init_server_memory();

	// call the function for object redistribution for the server
	// and its labels
	add_redistribute_objects(main_server, server_id);
	add_redistribute_objects(main_server, server_label_1);
	add_redistribute_objects(main_server, server_label_2);
}

// function used for removing a server label from the hashring cdll
void remove_from_hashring(load_balancer* main_server, int server_id_label)
{
	unsigned int position = hashring_position(main_server->hashring,
							server_id_label);
	cdll_node* removed = remove_node(main_server->hashring, position);
	free(removed->data);
	free(removed);
}

// function used for removing a server from the load balancer
void loader_remove_server(load_balancer* main_server, int server_id)
{
	// calculate the server's labels values
	int server_label_1 = 1 * MAX_HASH + server_id;
    int server_label_2 = 2 * MAX_HASH + server_id;

	// remove the server and its labels from the hashring
	remove_from_hashring(main_server, server_id);
	remove_from_hashring(main_server, server_label_1);
	remove_from_hashring(main_server, server_label_2);

	// get the number of total nodes stored on the server's buckets
	// and stop the iteration when finding all of them
	int no_nodes_in_buckets = (int)main_server->servers_ht[server_id]->size;

	// iterate through the server's array of buckets
    for (int i = 0; i < (int)main_server->servers_ht[server_id]->hmax; i++) {
		// iterate through each cdll bucket
		cdll_node *current = main_server->servers_ht[server_id]->
							 buckets[i]->head;

		for (int j = 0; j < (int)main_server->servers_ht[server_id]
							->buckets[i]->size; j++) {
            int new_server = 0;
			// store the object on a different server
            loader_store(main_server, ((key_value_pair *)(current->data))->key,
					((key_value_pair *)(current->data))->value, &new_server);

			// check if all the nodes were found
			no_nodes_in_buckets--;
			if (no_nodes_in_buckets == 0) {
				// free the server's memory
				free_server_memory(main_server->servers_ht[server_id]);
				return;
			}
			current = current->next;
		}
	}

	// free the server's memory
    free_server_memory(main_server->servers_ht[server_id]);
}

// function used for freeing the main load balancer
void free_load_balancer(load_balancer* main_server)
{
	// iterate through the hashring elements
	cdll_node* current = main_server->hashring->head;
	int hashring_size = main_server->hashring->size;

    for (int i = 0; i < hashring_size; i++) {
		// if the server label represents the server's id
		// (not it's labels), free the server's hashtable
		if (*(unsigned int*)current->data < MAX_HASH) {
			free_server_memory(main_server->servers_ht[*(unsigned int*)
							   current->data]);
		}
		current = current->next;
	}

	// free the hashring cdll, the array of hashtables and the main server
	cdll_free(&main_server->hashring);
	free(main_server->servers_ht);
	free(main_server);
}
