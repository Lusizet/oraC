#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

void* my_malloc(size_t size);
void my_free(void* ptr);
void* my_calloc(size_t NumElements, size_t element_size);
void* my_realloc(void* mem, size_t new_size);

#endif
