#ifndef DLL_H
#define DLL_H

#include <stdio.h>
#include <stdlib.h>

/*Double linked list structure to test the slab allocator*/
typedef struct test_list{
   int num[1000];
   char *str;
   struct test_list *next, *prev;
}test_list;

#endif
