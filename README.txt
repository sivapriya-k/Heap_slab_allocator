**************************************************************************************************
                       SLAB ALLOCATOR 
*************************************************************************************************


*PROGRAM FILES:
   Source code : sivapriya_503_5_main.c sivapriya_503_5_main.h
                 sivapriya_503_5_slab_allocator.h
                 sivapriya_503_5_slab_allocator.c
                 sivapriya_503_5_dll.h
   Makefile : Makefile

*Compilation Instructions:
   Execute the following:
     make -f Makefile

*Run exe
   ./slab_alloc [mem_pool_size]

*BRIEF DESCRIPTION OF THE PROGRAM
   * The program starts with the allocation of the memory pool size
     mentioned by user(in MB).
   * The allocated memory pool is implemented as double linked list
     of slabs,storing the info in the allocated memory itself(i.e
     in the slabs itself)
   * The next step is to form (array of)slab classes and allocate a slab(block of 1MB
      memory) for each slab class and populate with header along with splitting
     the remaining slab space (excluding the space for mem_pool header info
     and slab class header info), into equal sized chunks, except the last
     chunk. This chunk size is rounded to the nearest multiple of 8, so that
     a pointer returned to user from alloc_mem is a multiple of 8
   * An overview of 1 slab memory layout is shown below

  
   mem_pool H   slab_class header                 chunk_header             **
   -----------------------------------------------------first_chunk---------------------second_chunk-------------------
   |next|prev|index|chunk_size|next|start_chunk|slab_class|index|size|next|  |
   |         |                                 |                             |
   |         |                                 |                             |

   ** - Actual portion of memory that will be sent to and used User

   * A layout of array of slab classes is given below
   
     [slab_class_0]->[next_slab_of_class_0]->[next_slab_of-class_0]->so on
     [slab_class_1]->[next_slab_of_class_1]->so on
     [slab_class_2]
         .
         .
         .
         .
         .
     [slab_class_42]

   * When alloc_mem is called, the requested number of bytes by user is
     checked, and a chunk size closest to this is chosen(accounting for the
     header information of the chunk)
   * Based on this chunk size,appropriate slab class is chosen
   * In the array of slab classes, the corresponding chosen class index is
     checked
   * If the first slab in this index has a free chunk, the address is returned
     Else, the next slab of this slab class(index) is checked for free chunk
   * If no chunk is free, a new slab is allcoated to this class(as before) and 
     a free chunk address is returned to user, If there is no free memory in
     pool, NULL is returned
   * When free_mem is called, the appropriate chunk header is found with the 
     passed pointer, and this chunk is added back to the slab class at the
     corresponding index(using the index fields in the header of slab_class 
     and chunk) 
   

OTHER INFO:
   * Number of classes is pre-defined as 42(until reaching 1MB chunk size), based on the formula
     c(i) = 1.25 * c(i-1), i>=0, and rounding to nearest multiple of 8

   * To use only the slab_allocator.h and slab_allocator.c files, the 
     following must be included in main.h,to compile and run

/*Pointer to slab allocator structure to be used by user to call
* slab allocator functions*/
struct slab_allocator *alloc = NULL;

/*Pointer used to free the entire memory pool before exit*/
mem_pool* free_ptr = NULL;

extern void* alloc_mem(struct slab_allocator* alloc, int num_of_bytes);
extern int free_mem(struct slab_allocator* alloc, void* ptr_to_free);
extern void init_slab_classes(struct slab_allocator *alloc, int
mem_pool_size);

   
    *  Various testcases were tested with varying list sizes and order of
       alloc_mem and free_mem. The sivapriya_503_5_main.c, has
       implementation of one of these testcases
