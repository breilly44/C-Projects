#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>


// define MEMLENGTH as 512 to represent number of elements in array
// also simulate the heap for 8-byte alignment
#define MEMLENGTH 512
static double memory[MEMLENGTH];  


// define the alias chunk_header for the typedef struct chunk_header
// has size_t (unsigned integer) to be variable for chunk sizes
// int free to determine status of blocks (1 if free and 0 if allocated)
// and a pointer that points to the next chunk
typedef struct chunk_header {
    size_t size;
    int free;
    struct chunk_header* next;
} chunk_header;

// freeList points to the chunk_header structure
// set initial value as NULL (points to no chunks)
static chunk_header* freeList = NULL;

// ALIGN8 alias i use to rounds down nearest multiple of 8 in binary operations
// if 7 is 111 then ~7 is 1111 1000
// the & ~7 is the AND operation takes 0000 as smallest, then 1000, 10000, and so on (all multiples of 8)
// sizeof() is in bytes... defines size of metadata chunk_header for each chunk
#define ALIGN8(x) (((x) + 7) & ~7)
#define CHUNK_SIZE (sizeof(chunk_header))

// this function to initialize freeList
// ran into errors here. defered the initialization until run time for fix
void initialize_memory_system();
printf("Size of chunk_header: %zu bytes\n", sizeof(chunk_header));

// this is the globally defined function mymalloc
void *mymalloc(size_t requested_size, char *file, int line) {

    // ensure the initialization of freeList
    if (freeList == NULL) {
        initialize_memory_system();
    }

    // total_size calculates the size of memory to allocate
    // requested size + metadata size
    size_t total_size = ALIGN8(requested_size) + CHUNK_SIZE;

    // pointers to traverse list of chunk_headers
    chunk_header *prev = NULL;
    chunk_header *current = freeList;

    // while loop to 
    while (current != NULL) {

        // this condition checks if the current chunk is free and if the current chunk size is >= total_size for new chunk
        if (current->free && current->size >= total_size) {

            // This condition checks if there is space for a new chunk
            if (current->size >= total_size + CHUNK_SIZE + 8) {
                
                // new_chunk set to point location in memory that is 'total_size' bytes ahead of the current
                // sets to new_chunk size by addressing the memory used of current chunk size against allocated chunk
                // sets the new chunk status as free
                // next pointer of new_chunk to what current_next is pointing to
                // current size is set to total size
                // next pointer of current_next points to new chunk

                chunk_header *new_chunk = (chunk_header *)((char *)current + total_size);
                new_chunk->size = current->size - total_size;
                new_chunk->free = 1;
                new_chunk->next = current->next;
                current->size = total_size;
                current->next = new_chunk;

            }
            // status of current chunk is now allocated
            // this returns the pointer to the payload
            current->free = 0;
            return (char *)current + CHUNK_SIZE;
        }
        // adjust pointers for new chunks
        prev = current;
        current = current->next;
    }

    printf("mymalloc: Unable to allocate %zu bytes (file: %s, line: %d)\n", requested_size, file, line);
    return NULL;
}

// global function to free space
void myfree(void *ptr, char *file, int line) {
    if (ptr == NULL) {
        return;
    }

    // Convert to chunk_header and adjust for header offset
    chunk_header *chunk_to_free = (chunk_header *)((char *)ptr - CHUNK_SIZE);
    chunk_to_free->free = 1;

    // Attempt to coalesce with next chunk if it's free
    if (chunk_to_free->next && chunk_to_free->next->free) {
        chunk_to_free->size += chunk_to_free->next->size;
        chunk_to_free->next = chunk_to_free->next->next;
    }

}

void initialize_memory_system() {
    // initialization for linked list freeList
    freeList = (chunk_header *)memory;
    freeList->size = MEMLENGTH * sizeof(double);
    // initally memory is free so status set to 1
    freeList->free = 1;
    // next to null
    freeList->next = NULL;
}

