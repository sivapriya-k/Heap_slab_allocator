#ifndef MAIN_H
#define MAIN_H

#include "sivapriya_503_5_dll.h"
#include "sivapriya_503_5_slab_allocator.h"

/*Pointer to slab allocator structure to be used by user to call
 * slab allocator functions*/
struct slab_allocator *alloc = NULL;

/*Pointer used to free the entire memory pool before exit*/
mem_pool* free_ptr = NULL;

/*Pointers to first and last nodes of the dll created for testing*/
test_list* first_test_node = NULL;
test_list* last_test_node = NULL;

extern void* alloc_mem(struct slab_allocator* alloc, int num_of_bytes);
extern int free_mem(struct slab_allocator* alloc, void* ptr_to_free);
extern void init_slab_classes(struct slab_allocator *alloc, int mem_pool_size);


void create_list(int *num_of_nodes);
void print_list();
void delete_list();
void add_node(test_list* new_node);
void delete_node(test_list* new_node);
void free_memory();

#endif
