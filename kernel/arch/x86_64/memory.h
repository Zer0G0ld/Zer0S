#ifndef _MEMORY_H
#define _MEMORY_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096

// Memory regions from multiboot
typedef struct {
    uint32_t size;
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;  // 1 = RAM, 2 = reserved, etc
} memory_region_t;

// Frame allocator functions
void memory_init(uint64_t mmap_addr, uint64_t mmap_length);
void* alloc_frame(void);
void free_frame(void* addr);
uint64_t get_free_memory(void);
void memory_status(void);

// Heap allocator (kmalloc/kfree)
void* kmalloc(size_t size);
void* kcalloc(size_t num, size_t size);
void* krealloc(void* ptr, size_t size);
void kfree(void* ptr);
void heap_status(void);

#endif
