#ifndef SLAB_H
#define SLAB_H

#include <stdio.h>
#include <stdlib.h>

/*Pre-defined number, refer README.txt*/
#define NUM_OF_CLASSES 42 

/*Every slab is 1MB ie 1024 * 1024 bytes */
#define TOTAL_SLAB_SPACE 1024*1024

/*Max chunk size : (1MB - slab_class_header - chunk_header) */
#define MAX_CHUNK_SIZE 1048544

/*Max calculated : Calculated based on the given formula*/
#define MAX_CALCULATED_CHUNK_SIZE 955144

/*Memory pool implemented as double linked list*/
typedef struct mem_pool{
  struct mem_pool *next, *prev;
}mem_pool;

/*Chunks of a slab implemented as linked list*/
typedef struct slab_chunk_nodes{
   int slab_class;
   int index;
   int size;
   struct slab_chunk_nodes *next;
}slab_chunk_nodes;

/*Slab class structure, implemented as linked list
 * next pointer points to the next slab in the
 * same class*/
typedef struct slab_class{
  int index;
  int chunk_size;
  struct slab_class *next;
  struct slab_chunk_nodes *start_chunk;
}slab_class;

/*Slab allocator structure, containing pointer to 
 * array of slab classes*/
struct slab_allocator{
  slab_class **slab_class_array;
}slab_allocator;

/*Pointer to slab allocator structure*/
extern struct slab_allocator *alloc;
extern mem_pool *free_ptr ;

/*Pointer to first node of the available memory pool*/
static mem_pool *first_node_in_pool = NULL;

/*Pointer to last node of the available memory pool*/
static mem_pool *last_node_in_pool = NULL;

/*Pointers to first and last chunk of a slab
 * While splitting slab into chunks
 * these pointers are used to link
 * the chunks*/
static slab_chunk_nodes *first_chunk = NULL;
static slab_chunk_nodes *last_chunk = NULL;

/*utility functions*/
slab_class* add_slab_to_class(slab_class *previous_slab,int chosen_class);
void allocate_memory_pool(int *mem_pool_size);
void add_node_topool(mem_pool* new_node);
void add_chunk_to_list(slab_chunk_nodes* cur_chunk);
void print_pool_info();
void print_slab_info();
void free_memory_pool();

/*Main functions*/
void init_slab_classes(struct slab_allocator* alloc, int mem_pool_size);
void* alloc_mem(struct slab_allocator* alloc,int num_of_bytes);
int free_mem(struct slab_allocator* alloc,void* ptr_to_free);   
#endif
