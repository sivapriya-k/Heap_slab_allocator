#include "sivapriya_503_5_main.h"

/*********************************MAIN FUNCTION************************************/
int main(int argc, char* argv[])
{
   int mem_pool_size = 0;
   int i=0;
   int num_of_nodes = 0;
  
   /*Get the memory pool size and allocate the memory*/
   if(argv[1] != NULL)
     mem_pool_size = atoi(argv[1]);

   alloc = (struct slab_allocator*)malloc(sizeof(struct slab_allocator));
   init_slab_classes(alloc, mem_pool_size); 

   /*test alloc_mem/free_mem */
   num_of_nodes = 100;
   create_list(&num_of_nodes);
   
   for(i=0;i<100;i++)
   {
      create_list(&num_of_nodes);
   }
  
   delete_list();
}

/*--------------------------------------------------------------------
* Function : create_list
*
* Arguments : int *num_of_nodes-Pointer tonum_of_nodes to be created
*
* Return value : None
*
* Notes:  This function creates a double linked list, of structure 
*         mentioned in sivapriya_503_5_dll.h, with num_of_nodes, nodes
* ------------------------------------------------------------------*/
void create_list(int *num_of_nodes)
{
    test_list* new_node = NULL;
    void* mem_allocated = NULL;
    int i =0, j=0;
 
    for(i=0;i<*num_of_nodes;i++)
    {
       mem_allocated = alloc_mem(alloc,sizeof(test_list));
       new_node = (test_list*)mem_allocated;
       if(new_node == NULL)
       {
           printf("memory alloc failure from slab allocator\n");
           free_memory();
           exit(-1);
       }
      
       for(j=0;j<1000;j++)
         new_node->num[j]= j;
       new_node->str = "hello";
       add_node(new_node);
    }


}

/*--------------------------------------------------------------------
* Function : add_node
*
* Arguments : test_list* new_node - Pointer to the node to be deleted
*
* Return value : None
*
* Notes:  This function,given a node, adds it to the double
*         linked list 
* ------------------------------------------------------------------*/
void add_node(test_list* new_node)
{
   /* When this is the first node, assign first node pointer*/
  if(first_test_node == NULL)
    first_test_node = new_node;

  /*add the new node at the end of the linked list*/
  if(last_test_node != NULL)
    last_test_node->next = new_node;

  new_node->prev = last_test_node;
  new_node->next = NULL;
  last_test_node = new_node;
}

/*--------------------------------------------------------------------
* Function : delete_node
*
* Arguments : test_list* todelete_node - Pointer to the node that has
*             to be deleted
*
* Return value : None
*
* Notes:  This function,given a node, deletes it from the double 
*         linked list
* ------------------------------------------------------------------*/
void delete_node(test_list* todelete_node)
{
   int result = -1;

   /*Update prev/next pointers for prev/next nodes*/
   if(todelete_node->prev != NULL)
     todelete_node->prev->next = todelete_node->next;

   if(todelete_node->next != NULL)
     todelete_node->next->prev = todelete_node->prev;

   /*Call the free_mem function of slab allocator*/
   result = free_mem(alloc, (void*)todelete_node);

   /*Check result*/
   if(result == -1)
     printf("Free failed\n");
   else
     printf("Free success\n");

}

/*--------------------------------------------------------------------
* Function : print_list
*
* Arguments : None
*
* Return value : None
*
* Notes:  This function prints the list values starting from first node
* ------------------------------------------------------------------*/
void print_list()
{
   int i =0;
   test_list* temp = first_test_node;

   while(temp!= NULL)
   {
     for(i=0;i<1000;i++)
       printf("Value at %d: %d\n", i,temp->num[i]);
     temp = temp->next;
   }

}

/*--------------------------------------------------------------------
* Function : delete_list
*
* Arguments : None
*
* Return value : None
*
* Notes:  This function,given a start node deletes the entire linked
*         list 
* ------------------------------------------------------------------*/
void delete_list()
{
    test_list* temp = first_test_node;
    test_list* cur_node = NULL;

    /*Starting from the first node in the list, delete all nodes*/
    while(temp != NULL)
    {
       cur_node = temp;
       temp = temp->next;
       delete_node(cur_node);
    }

    /*Reset first and last node pointers*/
    first_test_node = NULL;
    last_test_node = NULL;
}

/*--------------------------------------------------------------------
* Function : free_memory()
*
* Arguments : None
*
* Return value : None
*
* Notes:  This function uses the pointer to the memory pool and
*         fress the memory allocated initially. This is done to
*         exit the program gracefully
* ------------------------------------------------------------------*/
void free_memory()
{
    mem_pool* temp = free_ptr;
    mem_pool* temp1 = NULL;

    while(temp != NULL)
    {
       temp1 = temp->next;
       free(temp);
       temp = temp1;
    }

}
