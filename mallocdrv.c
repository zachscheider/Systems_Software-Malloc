#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

//include your code

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



//replace malloc here with the appropriate version of mymalloc
#define MALLOC my_worstfit_malloc
//replace free here with the appropriate version of myfree
#define FREE my_free

//You can adjust how many things are allocated
#define TIMES 100

//If you want to make times bigger than 100, remove the call to qsort and do something else.
//Then remove this check.
#if TIMES >= 1000
	#error "TIMES is too big, qsort will switch to mergesort which requires a temporary malloc()ed array"
#endif

//Do not modify below here
struct tree {
	int data;
	struct tree *left;
	struct tree *right;
};

void freetree(struct tree *root)
{
	if(root->left != NULL)
		freetree(root->left);
	if(root->right != NULL)
		freetree(root->right);
	FREE(root);
}

void randominsert(struct tree *head, struct tree *new)
{
	int way;
	struct tree *curr, *prev;
	prev = NULL;
	curr = head;

	while(curr != NULL)
	{
		prev = curr;
		way = rand()%2;
		if(way == 0)
		{
			curr = curr->left;
		}
		else
		{
			curr = curr->right;
		}
	}
	if(way == 0)
		prev->left = new;
	else
		prev->right = new;
}

void printtree(struct tree *head)
{
	struct tree *curr = head;
	if(head == NULL)
		return;

	printtree(curr->left);	
	printf("%d\n", curr->data);
	printtree(curr->right);
}			 

void test1()
{
	int i;
 	
	struct tree *head = (struct tree *)MALLOC(sizeof(struct tree));
	head->data = 0;
	head->left = NULL;
	head->right = NULL;

	for(i=1;i<TIMES;i++)
	{
		struct tree *new = (struct tree *)MALLOC(sizeof(struct tree));
		new->data = i;
		new->left = NULL;
		new->right = NULL;
		randominsert(head, new);
	}

	printtree(head);
	freetree(head);
}

int comp(const void *a, const void *b)
{
	return *((int *)a) - *((int *)b);
}

void test2()
{
	int *a;
	int i;

	a = (int *)MALLOC(TIMES * sizeof(int));

	for(i=0;i<TIMES;i++)
	{
		a[i] = rand()%TIMES + 1;
	}

	qsort(a, TIMES, sizeof(int), comp);

	for(i=0;i<TIMES;i++)
	{
		printf("%d\n", a[i]);
	}

	FREE(a);
	
}

int main() 
{
	srand((unsigned int)time(NULL));
	printf("original val brk: %p\n", sbrk(0));
	test1();
	printf("brk after  test1: %p\n", sbrk(0));
	test2();
	printf("brk after  test2: %p\n", sbrk(0));
	

	return 0;
}
