#ifndef ALLOCATOR_INTERNAL_H
#define ALLOCATOR_INTERNAL_H

#include <stdint.h>
#include <windows.h>

#define ALIGN16(x) (((x) + 15) & ~15)

#define METABLOCK_SIZE ALIGN16(sizeof(MetaBlock))
#define DATA_MIN_SIZE 8
#define MAX_SC_PAGE 128
#define NUM_SIZE_CLASS 6
#define PAGE_ALLOC_SIZE 4096
#define POISON_BYTE 0xDF

typedef struct {
    uint8_t* start_addr;
    uint8_t* end_addr;
    int sc_index;
} ScPageRecord;

typedef struct MetaBlock {
    struct MetaBlock* next;
    struct MetaBlock* prev;
    bool free;
    size_t size;
    char data[];
} MetaBlock;

typedef struct FreeNode {
    struct FreeNode* next;
} FreeNode;

typedef struct {
    FreeNode* head[NUM_SIZE_CLASS];
} ScHeads;

extern ScPageRecord g_sc_pages[MAX_SC_PAGE];
extern size_t g_sc_page_count;
extern MetaBlock* g_mb_head;
extern ScHeads g_heads;

size_t index_to_size(int index);
int ptr_to_index(void* ptr);
int get_sc_index(size_t size);

#endif