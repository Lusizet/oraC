#include "Headers/allocator.h"
#include "Headers/allocator_internal.h"

int ptr_to_index(void* ptr) {
    uint8_t* p = ptr;
    for (int i = 0; i < g_sc_page_count; i++) {
        if (p >= g_sc_pages[i].start_addr && p < g_sc_pages[i].end_addr) {
            return g_sc_pages[i].sc_index;
        }
    }
    return -1;
}

static void coalesce(MetaBlock* block) {
    if (!block) return;
    if (block->next && block->next->free) {
        block->size += block->next->size + METABLOCK_SIZE;
        block->next = block->next->next;
        if (block->next) block->next->prev = block;
    }
    if (block->prev && block->prev->free) {
        block->prev->size += block->size + METABLOCK_SIZE;
        block->prev->next = block->next;
        if (block->next) block->next->prev = block->prev;
        block = block->prev;
    }
}

void my_free(void* ptr) {
    if (!ptr) return;
    int index = ptr_to_index(ptr);
    if (index != -1) {
        memset(ptr, POISON_BYTE, index_to_size(index));
        FreeNode* node = ptr;
        node->next = g_heads.head[index];
        g_heads.head[index] = node;
        return;
    }
    MetaBlock* block = (MetaBlock*)((uint8_t*)ptr - offsetof(MetaBlock, data));
    memset(block->data, POISON_BYTE, block->size);
    block->free = true;
    coalesce(block);
}