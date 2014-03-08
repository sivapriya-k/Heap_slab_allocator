slab_alloc : main.o slab.o
	gcc -o slab_alloc sivapriya_503_5_main.o sivapriya_503_5_slab_allocator.o 

main.o : sivapriya_503_5_main.h sivapriya_503_5_dll.h
	gcc -c -g sivapriya_503_5_main.c

slab.o : sivapriya_503_5_slab_allocator.h
	gcc -c -g sivapriya_503_5_slab_allocator.c

clean:
	rm -f *.o
	rm -f *.out
