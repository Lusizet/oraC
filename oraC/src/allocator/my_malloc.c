#include "Headers/allocator.h"
#include "Headers/allocator_internal.h"

ScPageRecord g_sc_pages[MAX_SC_PAGE];
size_t g_sc_page_count = 0;
MetaBlock* g_mb_head = nullptr;
ScHeads g_heads = {0};

int get_sc_index(size_t size) {
    if (size <= 8) return 0;
    if (size <= 16) return 1;
    if (size <= 32) return 2;
    if (size <= 64) return 3;
    if (size <= 128) return 4;
    if (size <= 256) return 5;
    return -1;
}

size_t index_to_size(int index) {
    return 1<<(index+3);
}

static void extend_sc_heap(size_t size, int index) {
    void* page = VirtualAlloc(nullptr, PAGE_ALLOC_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!page) return;
    if (g_sc_page_count < MAX_SC_PAGE) {
        g_sc_pages[g_sc_page_count].start_addr = page;
        g_sc_pages[g_sc_page_count].end_addr = page + PAGE_ALLOC_SIZE;
        g_sc_pages[g_sc_page_count].sc_index = index;
        g_sc_page_count++;
    }
    for (size_t offset = 0; offset+size <= PAGE_ALLOC_SIZE; offset += size) {
        FreeNode* node = page+offset;
        node->next = g_heads.head[index];
        g_heads.head[index] = node;
    }
}

static void extend_heap() {
    void* page = VirtualAlloc(nullptr, PAGE_ALLOC_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!page) return;
    MetaBlock* new_block = page;
    new_block->next = g_mb_head;
    new_block->prev = nullptr;
    new_block->free = true;
    new_block->size = PAGE_ALLOC_SIZE - METABLOCK_SIZE;
    if (g_mb_head) g_mb_head->prev = new_block;
    g_mb_head = new_block;
}

static void split_space(MetaBlock* block, size_t size) {
    MetaBlock* new_block = (MetaBlock*)(block->data + size);
    new_block->free = true;
    new_block->size = block->size - size - METABLOCK_SIZE;
    new_block->next = block->next;
    new_block->prev = block;
    block->next = new_block;
    block->size = size;
    if (new_block->next) new_block->next->prev = new_block;
}

static void* find_block(size_t size) {
    if (!g_mb_head) extend_heap();
    MetaBlock* block = g_mb_head;
    while (block) {
        if (block->free && block->size>=size) {
            if (block->size>size+METABLOCK_SIZE+DATA_MIN_SIZE) split_space(block, size);
            return block;
        }
        block = block->next;
    }
    return nullptr;
}

void* my_malloc(size_t size) {
    if (size == 0) return nullptr;
    int index = get_sc_index(size);
    if (index != -1) {
        size = index_to_size(index);
        if (!g_heads.head[index]) {
            extend_sc_heap(size, index);
            if (!g_heads.head[index]) return nullptr;
        }
        FreeNode* node = g_heads.head[index];
        g_heads.head[index] = node->next;
        return node;
    }
    size_t aligned_size = ALIGN16(size);
    MetaBlock* block = find_block(aligned_size);
    if (!block) {
        extend_heap();
        block = find_block(aligned_size);
        if (!block) return nullptr;
    }
    block->free = false;
    return block->data;
}