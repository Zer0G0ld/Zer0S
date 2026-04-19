#include "memory.h"
#include <stdint.h>
#include <stddef.h>

// External VGA functions
extern void terminal_writestring(const char* str);
extern void print_hex(uint64_t value);

// Memory variables
static uint8_t* frame_bitmap = NULL;
static uint64_t total_pages = 0;
static uint64_t used_pages = 0;
static uint64_t free_pages = 0;
static uint64_t memory_end = 64 * 1024 * 1024; // Assume 64MB por enquanto

// Simple bitmap operations
static void bitmap_set_bit(uint8_t* bitmap, uint64_t bit) {
    bitmap[bit / 8] |= (1 << (bit % 8));
}

static void bitmap_clear_bit(uint8_t* bitmap, uint64_t bit) {
    bitmap[bit / 8] &= ~(1 << (bit % 8));
}

static int bitmap_test_bit(uint8_t* bitmap, uint64_t bit) {
    return (bitmap[bit / 8] & (1 << (bit % 8))) != 0;
}

// Find first free frame
static uint64_t find_free_frame(void) {
    for (uint64_t i = 0; i < total_pages; i++) {
        if (!bitmap_test_bit(frame_bitmap, i)) {
            return i;
        }
    }
    return (uint64_t)-1;
}

// Initialize memory manager (versão simplificada)
void memory_init(uint64_t mmap_addr, uint64_t mmap_length) {
    terminal_writestring("\n[Memory] Initializing frame allocator...\n");
    
    // Usar valor fixo de 64MB por enquanto
    memory_end = 64 * 1024 * 1024; // 64MB
    total_pages = memory_end / PAGE_SIZE;
    
    terminal_writestring("[Memory] Total memory: 64 MB\n");
    terminal_writestring("[Memory] Total pages: ");
    print_hex(total_pages);
    terminal_writestring("\n");
    
    // Calcular tamanho do bitmap (1 bit por página)
    uint64_t bitmap_size = (total_pages + 7) / 8;
    
    // Colocar o bitmap após o kernel (em 16MB)
    uint64_t bitmap_addr = 16 * 1024 * 1024; // 16MB
    frame_bitmap = (uint8_t*)bitmap_addr;
    
    // Limpar bitmap
    for (uint64_t i = 0; i < bitmap_size; i++) {
        frame_bitmap[i] = 0;
    }
    
    // Marcar páginas do kernel como usadas
    // Kernel está entre 1MB e 16MB
    uint64_t kernel_start_page = 1 * 1024 * 1024 / PAGE_SIZE;
    uint64_t kernel_end_page = (bitmap_addr + bitmap_size + PAGE_SIZE - 1) / PAGE_SIZE;
    
    for (uint64_t i = kernel_start_page; i < kernel_end_page; i++) {
        bitmap_set_bit(frame_bitmap, i);
        used_pages++;
    }
    
    free_pages = total_pages - used_pages;
    
    terminal_writestring("[Memory] Free pages: ");
    print_hex(free_pages);
    terminal_writestring("\n");
    terminal_writestring("[Memory] Used pages: ");
    print_hex(used_pages);
    terminal_writestring("\n");
    terminal_writestring("[Memory] Frame allocator ready!\n");
}

// Allocate a single frame (4KB)
void* alloc_frame(void) {
    uint64_t frame = find_free_frame();
    if (frame == (uint64_t)-1) {
        terminal_writestring("[ERROR] Out of memory!\n");
        return NULL;
    }
    
    bitmap_set_bit(frame_bitmap, frame);
    used_pages++;
    free_pages--;
    
    return (void*)(frame * PAGE_SIZE);
}

// Free a frame
void free_frame(void* addr) {
    uint64_t frame = (uint64_t)addr / PAGE_SIZE;
    if (frame >= total_pages) return;
    
    if (bitmap_test_bit(frame_bitmap, frame)) {
        bitmap_clear_bit(frame_bitmap, frame);
        used_pages--;
        free_pages++;
    }
}

// Get free memory in bytes
uint64_t get_free_memory(void) {
    return free_pages * PAGE_SIZE;
}

// Show memory status
void memory_status(void) {
    terminal_writestring("\n=== Memory Status ===\n");
    terminal_writestring("Total: 64 MB\n");
    terminal_writestring("Free: ");
    print_hex(get_free_memory() / 1024 / 1024);
    terminal_writestring(" MB\n");
    terminal_writestring("Used: ");
    print_hex((used_pages * PAGE_SIZE) / 1024 / 1024);
    terminal_writestring(" MB\n");
    terminal_writestring("Pages: ");
    print_hex(total_pages);
    terminal_writestring("\n");
    terminal_writestring("====================\n");
}

// Simple heap allocator
typedef struct heap_block {
    size_t size;
    int used;
    struct heap_block* next;
} heap_block_t;

#define HEAP_START 0x20000000  // 512MB
#define HEAP_MIN_BLOCK 32

static heap_block_t* heap_head = NULL;

void* kmalloc(size_t size) {
    if (size == 0) return NULL;
    
    // Align size to 8 bytes
    size = (size + 7) & ~7;
    
    // First call - initialize heap
    if (heap_head == NULL) {
        heap_head = (heap_block_t*)HEAP_START;
        heap_head->size = 8 * 1024 * 1024; // 8MB heap
        heap_head->used = 0;
        heap_head->next = NULL;
    }
    
    // Find a free block
    heap_block_t* current = heap_head;
    while (current) {
        if (!current->used && current->size >= size) {
            // Found a block
            if (current->size >= size + sizeof(heap_block_t) + HEAP_MIN_BLOCK) {
                // Split the block
                heap_block_t* new_block = (heap_block_t*)((uint64_t)current + sizeof(heap_block_t) + size);
                new_block->size = current->size - size - sizeof(heap_block_t);
                new_block->used = 0;
                new_block->next = current->next;
                
                current->size = size;
                current->next = new_block;
            }
            current->used = 1;
            return (void*)((uint64_t)current + sizeof(heap_block_t));
        }
        current = current->next;
    }
    
    terminal_writestring("[ERROR] kmalloc: Out of heap memory!\n");
    return NULL;
}

void kfree(void* ptr) {
    if (ptr == NULL) return;
    
    heap_block_t* block = (heap_block_t*)((uint64_t)ptr - sizeof(heap_block_t));
    block->used = 0;
    
    // Coalesce with next block if free
    if (block->next && !block->next->used) {
        block->size += sizeof(heap_block_t) + block->next->size;
        block->next = block->next->next;
    }
}

void* kcalloc(size_t num, size_t size) {
    void* ptr = kmalloc(num * size);
    if (ptr) {
        for (size_t i = 0; i < num * size; i++) {
            ((char*)ptr)[i] = 0;
        }
    }
    return ptr;
}

void heap_status(void) {
    terminal_writestring("\n=== Heap Status ===\n");
    heap_block_t* current = heap_head;
    int blocks = 0;
    size_t total = 0;
    size_t free = 0;
    
    while (current) {
        blocks++;
        total += current->size;
        if (!current->used) free += current->size;
        current = current->next;
    }
    
    terminal_writestring("Blocks: ");
    print_hex(blocks);
    terminal_writestring("\nTotal: ");
    print_hex(total / 1024);
    terminal_writestring(" KB\nFree: ");
    print_hex(free / 1024);
    terminal_writestring(" KB\n");
    terminal_writestring("==================\n");
}
