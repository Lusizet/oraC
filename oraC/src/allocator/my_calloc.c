#include "Headers/allocator.h"
#include "Headers/allocator_internal.h"

void* my_calloc(size_t num_elements, size_t element_size) {
    size_t total_size = num_elements * element_size;
    uint8_t* data = my_malloc(total_size);
    if (!data) return nullptr;
    memset(data, 0, total_size);
    return data;
}