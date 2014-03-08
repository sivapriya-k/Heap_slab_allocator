#include "sivapriya_503_5_slab_allocator.h"

/*--------------------------------------------------------------------
* Function : allocate_memory_pool
*
* Arguments : int *mem_pool_size - Size of the memory pool to be
*             allocated
*
* Return value : None
*
* Notes:  This function allocates memory specified by mem_pool_size
*         in slabs of 1MB and adds the slab to a double linked list.
* ------------------------------------------------------------------*/
void allocate_memory_pool(int *mem_pool_size)
{
   int i =0;
   void* mem_allocated = NULL;
   mem_pool *new_node = NULL;
   int size_to_allocate = *mem_pool_size;

   if(size_to_allocate <= NUM_OF_CLASSES)
   {
       perror("Mem pool allocation failure, create more slabs!\n");
       free_memory_pool();
       exit(-1);
   }

   /*Loop until required size is allocated*/
   while(i < size_to_allocate)
   {
     mem_allocated = malloc(1024 * 1024);
     if(mem_allocated == NULL)
     {
        perror("Mem pool allocation failure\n");
        free_memory_pool();
        exit(-1);
     }

     new_node = (mem_pool*)mem_allocated;

     /*Add the new slab to pool*/
     add_node_topool(new_node);
     i++;
   }

   //print_pool_info(); 

    /*Update pointer to free, to point to first node allocated during program exit*/
    free_ptr = first_node_in_pool;

}

/*--------------------------------------------------------------------
* Function : add_node_topool
*
* Arguments : mem_pool* new_node - Pointer to new slab allocated
*
* Return value : None
*
* Notes:  This function adds a new slab to linked list of slabs
*         in the memory pool
* ------------------------------------------------------------------*/
void add_node_topool(mem_pool* new_node)
{

   /* When this is the first slab, assign first node to first slab*/
   if(first_node_in_pool == NULL)
     first_node_in_pool = new_node;
   
   /*add the node at the end of the memory pool linked list*/
   if(last_node_in_pool != NULL)
      last_node_in_pool->next = new_node;

   /*Update next, prev pointers*/
   new_node->prev = last_node_in_pool;
   new_node->next = NULL;
   last_node_in_pool = new_node;

}

/*--------------------------------------------------------------------
* Function : print_pool_info
*
* Arguments : None
*
* Return value : None
*
* Notes:  This function prints memory pool linked list values
* ------------------------------------------------------------------*/
void print_pool_info()
{
   mem_pool* temp = first_node_in_pool;
   int i =0;
   
   while(temp != NULL)
   {
      printf("Node %d 's prev node %d\n", i, temp->prev);
      printf("Node %d is %d\n", i, temp);
      printf("Node %d 's next node %d\n", i, temp->next);
    
      i++;
      temp = temp->next;
   }
}  

/*--------------------------------------------------------------------
* Function : init_slab_classes
*
* Arguments : struct slab_allocator* alloc - Pointer to slab allocator
*             structure
*             int mem_pool_size - Size of the memory pool to be 
*             created
*
* Return value : None
*
* Notes:  This function, allocates the given memory pool and forms
*         slabs of 1MB. It then caluclates chunk sizes and assigns
*         a slab to each slab class (also dividing the slab into
*         equal sized chunks, except last one). The remaining set
*         of slabs are maintained, so that they can be used later
* ------------------------------------------------------------------*/     
void init_slab_classes(struct slab_allocator* alloc, int mem_pool_size)
{
    int i =0;
    int chunk_size = 80;
    int space_avl_for_chunk = 0;
    void *temp = NULL,*temp1 = NULL;
    int chunk_count = 0;
    slab_chunk_nodes* cur_chunk = NULL;
    mem_pool *cur_pool_ptr = NULL;

    /*Allocate required memory first*/
    allocate_memory_pool(&mem_pool_size);

    /*Current pool pointer points to first node in the 
     * available memory pool*/
    cur_pool_ptr = first_node_in_pool;

    /*Allocate memory for pointer to array of slab classes*/
    alloc->slab_class_array = (slab_class**)malloc(sizeof(slab_class*) * (NUM_OF_CLASSES));

    /*Error check*/
    if(alloc->slab_class_array == NULL)
    {
        perror("Memory Allocation failure while initializing slab allocator\n");
        free_memory_pool();
        exit(-1);
    }

    /*Loop until initial slab is assigned to each class*/
    while(i < NUM_OF_CLASSES && cur_pool_ptr != NULL)
    {
      /* Move the pointer after the size of the mem pool
         since first few bytes of a slab hold mem pool
         node info */
      temp = ((void*)cur_pool_ptr + sizeof(mem_pool));

      /*populate slab class header values */
      alloc->slab_class_array[i] = (slab_class*)temp;
      alloc->slab_class_array[i]->index = 0;
      alloc->slab_class_array[i]->chunk_size = chunk_size;
      alloc->slab_class_array[i]->next = NULL;

      /* First chunk of slab starts after the size of slab class*/
      first_chunk = (slab_chunk_nodes*)(temp + sizeof(slab_class));


      /*Update the start chunk value to this first chunk*/
      alloc->slab_class_array[i]->start_chunk = first_chunk;
      //printf("First chunk in slab %lu is %lu\n", alloc->slab_class_array[i], alloc->slab_class_array[i]->start_chunk);

      /*current chunk now points to first chunk */ 
      cur_chunk = first_chunk;

      /*Update total space available for chunks*/
      space_avl_for_chunk = TOTAL_SLAB_SPACE - sizeof(mem_pool) - sizeof(slab_class);

       //printf("Slab start address %d\n", alloc->slab_class_array[i]);

      /*Split the remaining space into chunks
        and update chunk header values */
      while(space_avl_for_chunk >= chunk_size)
      {
         cur_chunk->size = chunk_size - sizeof(slab_chunk_nodes);
         cur_chunk->slab_class = i;
         cur_chunk->index = alloc->slab_class_array[i]->index;
 
         add_chunk_to_list(cur_chunk); 
         temp1 = (void*)cur_chunk + chunk_size;
         cur_chunk = (slab_chunk_nodes*)temp1;
         //printf("Current chunk %d with size %d is at %d\n", chunk_count,chunk_size, cur_chunk);
         space_avl_for_chunk = space_avl_for_chunk - chunk_size;
         //printf("Space for chunk left %d in slab %d\n", space_avl_for_chunk, i);
         //printf("Slab start address %d\n", alloc->slab_class_array[i]);
         chunk_count ++;
      }

      /*Assign last chunk of the slab*/
      if(space_avl_for_chunk >= sizeof(slab_chunk_nodes))
      {
         cur_chunk->size = space_avl_for_chunk - sizeof(slab_chunk_nodes);
         cur_chunk->slab_class = i;
         cur_chunk->index = alloc->slab_class_array[i]->index;

         add_chunk_to_list(cur_chunk);
         space_avl_for_chunk = space_avl_for_chunk - sizeof(slab_chunk_nodes) - cur_chunk->size;
      }

      /*Chunk size  for the next slab class*/
      chunk_size = 1.25 * chunk_size;

      /*Assign chunk size such that its a multiple of 8
        so that pointer returned to user from alloc_mem
        is a multiple of 8*/
      chunk_size = (chunk_size - (chunk_size%8) + 8);

         
      /*If its the last chunk(after this size, size exceeds 1 MB), update the size to (1MB - slab_class_header - chunk_header)*/
      if(chunk_size == MAX_CALCULATED_CHUNK_SIZE)
        chunk_size = MAX_CHUNK_SIZE;

      i++;

      /*Move current pointer to next available slab in memory pool*/
      cur_pool_ptr = cur_pool_ptr->next;
      first_node_in_pool = cur_pool_ptr;

      /*Update first_chunk and last_chunk, so that they can be used for next slab*/
      first_chunk = NULL;
      last_chunk = NULL;
    }

    //print_slab_info();   
}

/*--------------------------------------------------------------------
* Function : add_chunk_to_list
*
* Arguments : slab_chunk_nodes* cur_chunk - Pointer to chunk to be 
*             added to the linked lsit
*
* Return value : None
*
* Notes:  This function adds a chunk to the linked list of chunks
* ------------------------------------------------------------------*/
void add_chunk_to_list(slab_chunk_nodes* cur_chunk)
{

   /*add the chunk at the end of the existing chunks*/
   if(last_chunk != NULL)
      last_chunk->next = cur_chunk;

   /*Update next, prev pointers*/
   cur_chunk->next = NULL;
   last_chunk = cur_chunk;
}

/*--------------------------------------------------------------------
* Function : print_slab_info
*
* Arguments : None
*
* Return value : None
*
* Notes:  This function prints slab class and chunk size info
* ------------------------------------------------------------------*/
void print_slab_info()
{
    int i =0;

    while(i < NUM_OF_CLASSES &&  alloc->slab_class_array[i]!= NULL)
    {
      printf("Slab class %d\n", i);
      printf("chunk_size : %d\n", alloc->slab_class_array[i]->chunk_size);
      i++;
    }
}

/*--------------------------------------------------------------------
 * Function : alloc_mem
 *
 * Arguments : struct slab_allocator* alloc - Pointer to slab allocator
 *             structure
 *             int num_of_bytes - Number of bytes of memory requested
 *             by user
 *
 * Return value : void* ptr_to_user - Pointer to memory returned 
 *                to user for use of user program
 *
 * Notes:  This function assigns the return pointer based on the
 *         requested number of bytes by user and pointer assigned
 *         is returned to user
 *         If no memory is available, NULL is returned to user
 * ------------------------------------------------------------------*/
void* alloc_mem(struct slab_allocator* alloc,int num_of_bytes)
{
    int total_size_req = 0;
    int class_found = 0, chunk_found = 0;
    int chosen_class = -1;
    int i=0;
    void* ptr_to_user = NULL;
    slab_chunk_nodes* first_chunk_avl = NULL;
    slab_class *current_slab = NULL, *previous_slab = NULL;
    slab_class *new_slab = NULL;

    /*Since chunks contain header, total size of the chosen chunk
      must be sizeof(header) + requested size*/
    total_size_req = num_of_bytes + sizeof(slab_chunk_nodes);
 
    if(total_size_req <= 0)
    {
        perror("Incorrect argument to alloc_mem\n");
        free_memory_pool();
        exit(-1);
    }
   
    /*Find the class with chunk size close to this*/
    for(i=0;i<NUM_OF_CLASSES;i++)
    {
       if(alloc->slab_class_array[i]->chunk_size >= total_size_req)
       {
          class_found = 1;
          chosen_class = i;
       }
       if(class_found == 1)
         break;
    }
  
    /*Choose a slab in the chosen class which has free size*/ 
    current_slab = alloc->slab_class_array[chosen_class]; 
    while(current_slab != NULL && chunk_found == 0 && chosen_class != -1)
    {
       if(current_slab->start_chunk != NULL)
       {
           if(current_slab->start_chunk->size >= num_of_bytes)
           {
               /*If chunk is available with requested size, choose this chunk*/
               //printf("Current slab %lu start chunk %lu of slab class %d\n",  current_slab,current_slab->start_chunk, chosen_class);
               first_chunk_avl = current_slab->start_chunk;   
               current_slab->start_chunk = current_slab->start_chunk->next;
               //printf("stat chunk of slab %lu is %lu\n", current_slab, current_slab->start_chunk);
               chunk_found = 1;  
           }
           else /*If this doesn't satisfy size requirements, move to next slab*/
           {
               previous_slab = current_slab;
               current_slab = current_slab->next;
           }
       }
       else /*if no chunk in this slab, go to next slab*/
       {
           previous_slab = current_slab;
           current_slab = current_slab->next;
       }
    }

    /*If no chunk is found,allocate a new slab for this slab class, from available memory pool 
      If no memory available in pool, then do nothing*/
    if(chunk_found == 0 && chosen_class != -1)
    {
       if(first_node_in_pool != NULL)
       {
          new_slab = add_slab_to_class(previous_slab,chosen_class);
          if(new_slab != NULL)
          {
            if(new_slab->start_chunk->size >= num_of_bytes)
            {
              first_chunk_avl = new_slab->start_chunk;
              new_slab->start_chunk = new_slab->start_chunk->next;
            }
          }
       }
    }

    /*If a chunk is found, advance pointer after the header and return the pointer to user*/
    if(first_chunk_avl != NULL)
    {
        ptr_to_user = (void*)(((void*)first_chunk_avl) + sizeof(slab_chunk_nodes));
    }
 
    if(ptr_to_user == NULL)
      printf("No sufficient memory available in pool\n");
 
    return ptr_to_user;

}

/*--------------------------------------------------------------------
* Function : free_mem
*
* Arguments : struct slab_allocator* alloc - Pointer to slab allocator
*             structure
*             void* ptr_to_free - Pointer to the memory freed by user
*
* Return value : int result - 0 if SUCCESS
*                             -1 if FAILURE
*
* Notes:  This function takes a pointer to a portion of chunk
*         returned by user, finds the corresponding chunk info
*         and puts the chunk back to available list, based on
*         the header values
* ------------------------------------------------------------------*/
int free_mem(struct slab_allocator* alloc, void* ptr_to_free)
{
    slab_chunk_nodes *freed_chunk = NULL;
    int freed_chunk_class = 0;
    int freed_chunk_class_index = 0;
    int result = -1;
    slab_class *slab_to_add = NULL;

    /*Find the slab class and index of which slab of the class, 
      chunk belongs to, with the returned pointer*/
    freed_chunk = ptr_to_free - sizeof(slab_chunk_nodes);
    freed_chunk_class = freed_chunk->slab_class;
    freed_chunk_class_index = freed_chunk->index;

    /*Find the exact slab with the index*/
    slab_to_add = alloc->slab_class_array[freed_chunk_class];
    while(slab_to_add->next != NULL && freed_chunk_class_index != slab_to_add->index)
    {
        slab_to_add = slab_to_add->next;
    }

    if(slab_to_add != NULL)
    {
       /*Add the chunk to the start of free chunk of the slab, the chunk belongs to*/
       freed_chunk->next = slab_to_add->start_chunk;
       slab_to_add->start_chunk = freed_chunk;
       result = 0;
    }

    return result;
    //printf("chosen class %d, slab starting chunk %d\n", freed_chunk_class, alloc->slab_class_array[freed_chunk_class]->start_chunk);
}

/*--------------------------------------------------------------------
* Function : add_slab_to_class
*
* Arguments : slab_class *previous_slab - Pointer to previous slab
*             after which the new slab is to be inserted
*             int chosen_class - Class to which the new slab belongs
*             and will be inserted to
*
* Return value : slab_class* new_slab - Pointer to the new slab
*                added to the passed/chosen class
*
* Notes:  This function adds a new slab(with chunks divided) to the
*          passed/chosen class
* ------------------------------------------------------------------*/
slab_class* add_slab_to_class(slab_class *previous_slab,int chosen_class)
{
     
     void *temp = NULL,*temp1 = NULL;
     int chunk_count = 0, i =0; 
     mem_pool *cur_pool_ptr = first_node_in_pool;
     slab_class *new_slab = NULL;
     int chunk_size = 0;
     int space_avl_for_chunk = 0;
     slab_chunk_nodes* cur_chunk = NULL;

     /* chunk size is the size already decided for this class
        available in the header*/
     chunk_size = alloc->slab_class_array[chosen_class]->chunk_size;

      /* Move the pointer after the size of the mem pool
      since first few bytes of a slab hold mem pool
      node info and fill new slab header info*/
      temp = ((void*)cur_pool_ptr + sizeof(mem_pool));
      new_slab = (slab_class*)temp;
      new_slab->chunk_size = chunk_size;
      new_slab->next = NULL;

      /*The next slab of the passed previous slab will be this new slab*/
      if(previous_slab != NULL)
      {
        previous_slab->next = new_slab;
        new_slab->index = previous_slab->index + 1;
      }
      else
      {
        perror("Memory corruption\n");
        free_memory_pool();
        exit(-1);
      }

      /*Assign first chunk and populate this value in the new slab header*/
      first_chunk = (slab_chunk_nodes*)(temp + sizeof(slab_class));
      new_slab->start_chunk = first_chunk;

      /*Current chunk points to first chunk, in the beginning*/
      cur_chunk = first_chunk;

      space_avl_for_chunk = TOTAL_SLAB_SPACE - sizeof(mem_pool) - sizeof(slab_class);

      /*Split the remaining space into chunks
       and update chunk header values */
      while(space_avl_for_chunk >= chunk_size)
      {
         cur_chunk->size = chunk_size - sizeof(slab_chunk_nodes);
         cur_chunk->slab_class = i;
         cur_chunk->index = new_slab->index;

         add_chunk_to_list(cur_chunk);
         temp1 = (void*)cur_chunk + chunk_size;
         cur_chunk = (slab_chunk_nodes*)temp1;
         space_avl_for_chunk = space_avl_for_chunk - chunk_size;
         chunk_count ++;
      }

      /*Assign the last chunk */
      if(space_avl_for_chunk >= sizeof(slab_chunk_nodes))
      {
         cur_chunk->size = space_avl_for_chunk - sizeof(slab_chunk_nodes);
         cur_chunk->slab_class = i;
         cur_chunk->index = new_slab->index;

         add_chunk_to_list(cur_chunk);
         space_avl_for_chunk = space_avl_for_chunk - sizeof(slab_chunk_nodes) - cur_chunk->size;
      }

      /*Update the first_node_in_pool pointer to point to the next available slab*/
      cur_pool_ptr = cur_pool_ptr->next;
      first_node_in_pool = cur_pool_ptr;

      /*Reset first and last chunk pointers*/
      first_chunk = NULL;
      last_chunk = NULL;

      return new_slab;
}

/*--------------------------------------------------------------------
* Function : free_memory_pool()
*
* Arguments : None
*
* Return value : None
*
* Notes:  This function uses the pointer to the memory pool and
*         fress the memory allocated initially. This is done to
*         exit the program gracefully
* ------------------------------------------------------------------*/
void free_memory_pool()
{
    mem_pool* temp = free_ptr;
    mem_pool* temp1 = NULL;

    /*Starting from the first slab that was allocated, free all slabs
      that were allocated by this program*/
    while(temp != NULL)
    {
       temp1 = temp->next;
       free(temp);
       temp = temp1;
    }

}

