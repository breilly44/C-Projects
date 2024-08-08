#include <stdlib.h>
#include <sys/time.h>

// include guard to prevent multiple include
#ifndef _MYMALLOC_H
#define _MYMALLOC_H

// redefine malloc as macro mymalloc
// redefine free as macro myfree
#define malloc(s)   mymalloc(s, __FILE__, __LINE__)
#define free(p)     myfree(p, __FILE__, __LINE__)

// declaration of functions to global scope for all files with header
void *mymalloc(size_t size, char *file, int line);
void myfree(void *ptr, char *file, int line);

#endif
