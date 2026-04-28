#include "memory.h"
#include <stdint.h>
#include <stddef.h>

extern void terminal_writestring(const char* str);
extern void print_hex(uint64_t value);

// Usar endereço seguro - 12MB está mapeado pelo boot.asm (64MB mapeados)
#define BITMAP_ADDR  0xC00000    // 12MB
#define HEAP_START   0x2000000   // 32MB

static uint8_t* frame_bitmap = NULL;
static uint64_t total_pages = 0;
static uint64_t used_pages = 0;
static uint64_t free_pages = 0;

static void bitmap_set_bit(uint8_t* bitmap, uint64_t bit) {
    bitmap[bit / 8] |= (1 << (bit % 8));
}

static void bitmap_clear_bit(uint8_t* bitmap, uint64_t bit) {
    bitmap[bit / 8] &= ~(1 << (bit % 8));
}

static int bitmap_test_bit(uint8_t* bitmap, uint64_t bit) {
    return (bitmap[bit / 8] & (1 << (bit % 8))) != 0;
}

static uint64_t find_free_frame(void) {
    for (uint64_t i = 0; i < total_pages; i++) {
        if (!bitmap_test_bit(frame_bitmap, i)) {
            return i;
        }
    }
    return (uint64_t)-1;
}

void memory_init(uint64_t mmap_addr, uint64_t mmap_length) {
    terminal_writestring("\n[Memory] Initializing frame allocator...\n");
    (void)mmap_addr;
    (void)mmap_length;
    
    // Configurar memória
    uint64_t memory_end = 64 * 1024 * 1024;  // 64MB
    total_pages = memory_end / PAGE_SIZE;
    
    terminal_writestring("[Memory] Total memory: 64 MB\n");
    terminal_writestring("[Memory] Page size: ");
    print_hex(PAGE_SIZE);
    terminal_writestring("\n");
    terminal_writestring("[Memory] Total pages: ");
    print_hex(total_pages);
    terminal_writestring("\n");
    
    // Calcular tamanho do bitmap
    uint64_t bitmap_size = (total_pages + 7) / 8;
    
    terminal_writestring("[Memory] Bitmap size: ");
    print_hex(bitmap_size);
    terminal_writestring(" bytes\n");
    
    // Usar endereço fixo para o bitmap (12MB)
    frame_bitmap = (uint8_t*)BITMAP_ADDR;
    
    terminal_writestring("[Memory] Bitmap address: ");
    print_hex((uint64_t)frame_bitmap);
    terminal_writestring("\n");
    
    // Limpar bitmap - CUIDADO: esta é a operação que pode causar page fault
    terminal_writestring("[Memory] Clearing bitmap...\n");
    for (uint64_t i = 0; i < bitmap_size; i++) {
        frame_bitmap[i] = 0;
    }
    terminal_writestring("[Memory] Bitmap cleared\n");
    
    // Marcar páginas do kernel como usadas
    uint64_t kernel_start_page = 1 * 1024 * 1024 / PAGE_SIZE;  // 1MB
    uint64_t kernel_end_page = (BITMAP_ADDR + bitmap_size + PAGE_SIZE - 1) / PAGE_SIZE;
    
    terminal_writestring("[Memory] Kernel pages: ");
    print_hex(kernel_start_page);
    terminal_writestring(" to ");
    print_hex(kernel_end_page);
    terminal_writestring("\n");
    
    for (uint64_t i = kernel_start_page; i < kernel_end_page && i < total_pages; i++) {
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

void* alloc_frame(void) {
    if (!frame_bitmap) return NULL;
    
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

void free_frame(void* addr) {
    if (!frame_bitmap || !addr) return;
    
    uint64_t frame = (uint64_t)addr / PAGE_SIZE;
    if (frame >= total_pages) return;
    
    if (bitmap_test_bit(frame_bitmap, frame)) {
        bitmap_clear_bit(frame_bitmap, frame);
        used_pages--;
        free_pages++;
    }
}

uint64_t get_free_memory(void) {
    return free_pages * PAGE_SIZE;
}

void memory_status(void) {
    terminal_writestring("\n=== Memory Status ===\n");
    terminal_writestring("Total: 64 MB\n");
    terminal_writestring("Free: ");
    print_hex(get_free_memory() / 1024 / 1024);
    terminal_writestring(" MB\n");
    terminal_writestring("Used: ");
    print_hex((used_pages * PAGE_SIZE) / 1024 / 1024);
    terminal_writestring(" MB\n");
    terminal_writestring("Pages total: ");
    print_hex(total_pages);
    terminal_writestring("\n");
    terminal_writestring("Pages used: ");
    print_hex(used_pages);
    terminal_writestring("\n");
    terminal_writestring("Pages free: ");
    print_hex(free_pages);
    terminal_writestring("\n");
    terminal_writestring("====================\n");
}

// Heap allocator
typedef struct heap_block {
    size_t size;
    int used;
    struct heap_block* next;
} heap_block_t;

static heap_block_t* heap_head = NULL;
#define HEAP_MIN_BLOCK 32

void* kmalloc(size_t size) {
    if (size == 0) return NULL;
    size = (size + 7) & ~7;
    
    if (heap_head == NULL) {
        heap_head = (heap_block_t*)HEAP_START;
        heap_head->size = 8 * 1024 * 1024;
        heap_head->used = 0;
        heap_head->next = NULL;
    }
    
    heap_block_t* current = heap_head;
    while (current) {
        if (!current->used && current->size >= size) {
            if (current->size >= size + sizeof(heap_block_t) + HEAP_MIN_BLOCK) {
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
    size_t total = 0, free_mem = 0;
    
    while (current) {
        blocks++;
        total += current->size;
        if (!current->used) free_mem += current->size;
        current = current->next;
    }
    
    terminal_writestring("Blocks: ");
    print_hex(blocks);
    terminal_writestring("\nTotal: ");
    print_hex(total / 1024);
    terminal_writestring(" KB\nFree: ");
    print_hex(free_mem / 1024);
    terminal_writestring(" KB\n");
    terminal_writestring("==================\n");
}
