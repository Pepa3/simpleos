#ifndef MEMORY_H_INC
#define MEMORY_H_INC

#include <stdint.h>
#include <stddef.h>

typedef struct {
	uint8_t status;
	uint32_t size;
} alloc_t;

void mm_init(uint32_t kernel_end);
void mm_print_out();

void paging_init();
void paging_map_virtual_to_phys(uint32_t virt, uint32_t phys);

uint8_t* pmalloc(size_t size); /* page aligned alloc */
uint8_t* malloc(size_t size);
void free(void *mem);


//void memcpy(uint8_t *destination, uint8_t *source, int bytes);
//void memset(uint8_t *destination, uint8_t value, uint32_t len);
//void* memcpy(const void* dest, const void* src, size_t num );
//void* memset(void * ptr, int value, size_t num );
void memcpy(uint8_t* dest, const uint8_t* src, size_t num);
void memset(void * ptr, int value, size_t num);

#endif