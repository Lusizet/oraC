#include "Headers/allocator.h"
#include "Headers/allocator_internal.h"

void* my_realloc(void* mem, size_t new_size) {
    if (!mem) return nullptr;
    void* new_mem = malloc(new_size);
    if (!new_mem) return nullptr;
    int index = ptr_to_index(mem);
    if (index != -1) memmove(new_mem, mem, index_to_size(index));
    else memmove(new_mem, mem, new_size);
    my_free(mem);
    return new_mem;
}