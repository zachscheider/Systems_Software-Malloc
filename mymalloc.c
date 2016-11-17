/*
Zach Scheider
CS0449
Allocates memory using worst fit, frees memory that it allocates
*/

#include <stdio.h>
#include "mymalloc.h"

typedef struct Node{

	int size;
	int used;
	struct Node *next;
	struct Node *prev;

} Node;

Node* first_node = NULL;
Node* last_node = NULL;

// allocates memory in the heap using worst fit
void *my_worstfit_malloc(int size){

    // if no memory has been allocated, create the first page
	if(!first_node){

		first_node = sbrk(sizeof(Node) + size);
		first_node->size = size;
		first_node->used = 1;
		first_node->next = NULL;
		first_node->prev = NULL;

		return (void *)(first_node) + sizeof(Node);
	}else{

		Node* temp_node = first_node;
		Node* largest_node = NULL;
		last_node = first_node;

		// finds the largest free space
		while(temp_node){
			if(!temp_node->used && temp_node->size >= size){
				if(!largest_node || temp_node->size > largest_node->size){
					largest_node = temp_node;
				}
			}
			temp_node = temp_node->next;
		}
		
		// if the there is enough free space it splits it
		if(largest_node && largest_node->size > size){

			Node* split_node = (void*)largest_node + sizeof(Node) + size;
			split_node->size = largest_node->size - size - sizeof(Node);
			split_node->used = 0;
			split_node->prev = largest_node;
			split_node->next = largest_node->next;
			
			largest_node->used = 1;
			largest_node->size = size;
			largest_node->next = split_node;
			
			if(split_node->next) split_node->next->prev = split_node;

			return (void *)(largest_node) + sizeof(Node);

		// if there isn't enough space it allocates more
		}else{

			Node* new_node = sbrk(size + sizeof(Node));
			new_node->size = size;
			new_node->used = 1;
			new_node->prev = last_node;
			new_node->next = NULL;
			last_node->next = new_node;
			last_node = new_node;
			
			return (void *)(new_node) + sizeof(Node);
		}
	}
}

// frees memory allocated by my_worstfit_malloc
void my_free(void *ptr){

	int combined_prev = 0;
	Node* freed_node = (Node*)(ptr - sizeof(Node));
	freed_node->used = 0;

	// if the previous node is also free it combines the space
	if(freed_node->prev && !freed_node->prev->used){

		combined_prev = 1;
		freed_node->prev->next = freed_node->next;
		freed_node->prev->size += freed_node->size + sizeof(Node);
		if(freed_node->next) freed_node->next->prev = freed_node->prev;
	}
	
	// if the next node is also free it combines the space
	if(freed_node->next && !freed_node->next->used){

		freed_node->next = freed_node->next->next;
		freed_node->size += freed_node->next->size + sizeof(Node);
		freed_node->next->prev = freed_node;
	}

	// shrinks the heap if it needs to
	Node* resize_node;
	if(combined_prev) resize_node = freed_node->prev;
	else resize_node = freed_node;

	if(!resize_node->next && !resize_node->used){

		int size_change;
		if(resize_node == first_node) first_node = NULL;
		else resize_node->prev->next = NULL;
		size_change = resize_node->size + sizeof(Node);
		sbrk(-1 * size_change);
	}
}