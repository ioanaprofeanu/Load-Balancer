 Load-Balancer
 Copyright 2021 @Profeanu Ioana
 
 README file
-------------------------------------------------------------------------------
* Server implementation *
   ~ Data structures used: 
   	- server_memory structure - it represents a hashtable on which the
   	key-value pairs will be stored by hash on an array of circular doubly 
   	linked lists (representing the buckets);
   	- the array is similar to a frequency array, because the bucket for the
   	i hash is stored on buckets[i]
   ~ Functionality implementation:
   	- initialise the hashtable by allocating memory for its components
   	- store a key and value pair by calculating the key's hash, then
   	checking if it already exists in the hash's bucket; if so, renew the
   	value, otherwise create a new key-value component and add it to the
   	cdll bucket of the hash
   	- remove an key-value entry by calculating the key's hash, finding the
   	key in the hash's bucket and removing the key-value pair from the cdll
   	- retrieve a value stored at a given key by calculating the key's hash,
   	finding it in the hash's bucket and returning the value; if the entry
   	doesn't exist, return NULL
   	- free the hashtable by iterating through the cdll array and freeing
   	each cdll bucket and its components
-------------------------------------------------------------------------------
* Load balancer implementation *
   ~ Data structures used:
	- load_balancer structure which contains:
	- an array of server_memory components, which contains the hashtable 
	for each server stored on the main server; it is similar to a frequency
	array, since the hashtable for the i server id is stored on
	servers_ht[i]
	- a circular doubly linked list which represents the hashring of server
	ids and labels; I opted for cdll because this implementation is more
	suitable for finding the server on which a key should be stored (for
	that, the circular quality of cdll eases the implementation based on
	consistent hashing);
   ~ Functionality implementation:
	- initialise load balancer by allocating memory for its components	
	- store a certain key-value entry on a server using the consistent
	hashing method; find the position of the server on which it should be
	stored, then store it in the server's hashtable
	- retrieve the value stored at a given key by calculating the position
	on which it should be stored and returning the data from the server's
	hashtable
	- add a server in the load balancer by adding it and its labels on the
	hashring and initialising its hashtable; for object redistribution, get
	the label's right neighbour from the hashring and iterate through its
	hashtable buckets; for each key, check if it should be stored on the
	newly added server
	- remove a server from the load balancer by removing it and its labels
	from the hashring, then iterating through its hashtable buckets and
	restoring each key-value pair on a different server; in the end, free
	the server's hashtable memory
	- free the load balancer by iterating through the hashring and freeing
	each server's hashtable (free it only if the label represents a server
	id, not a label); then free the remaining load balancer components
	- auxiliary functions used:
	   - function for finding the position in the hashring cdll on which a
	   server label should be stored; it returns the position of the first
	   server label with the hash greater than the given server label
	   - function for finding the position in the hashring cdll of the
	   first server label which has the hash greater than a given key's
	   hash
-------------------------------------------------------------------------------
