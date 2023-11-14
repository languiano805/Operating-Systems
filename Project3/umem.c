// umem.c by Leonardo Anguiano
// Submitted for CSC139

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "umem.h" 

// struct for the free list
typedef struct block_header
{
    size_t size;
    int free;
    struct block_header *next;
} block_header;

block_header *head = NULL;

// variable to hold the start of the memory region
char *mem_region;
size_t size_region;

// variable for the allocation algorithm
int allo_algo;

int umeminit(size_t sizeOfRegion, int allocationAlgo)
{
    static int is_initialized = 0;

    if (is_initialized)
    {
        return -1;
    }

    // check if the size of the region is valid
    if (sizeOfRegion <= 0 || head != NULL)
    {
        return -1;
    }

    // get the page size
    int pageSize = getpagesize();

    // calculate the number of pages needed
    size_t numPages = (sizeOfRegion + pageSize - 1) / pageSize;

    // allo mem for the region
    mem_region = mmap(NULL, numPages * pageSize, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (mem_region == MAP_FAILED)
    {
        return -1;
    }

    head = (block_header *)mem_region;
    head->size = numPages * pageSize - sizeof(block_header);
    head->free = 1;
    head->next = NULL;

    allo_algo = allocationAlgo;

    is_initialized = 1;

    return 0;
}

// Allocation alogrithms
//

// BEST_FIT
// searches the list for the smallest chunk of free space that is large
// enough to accommodate the requested amount of memory, then returns
// the requested amount to the user starting from the beginning of the chunk.
// If there are multiple chunks of the same size, the BESTFIT allocator uses
// the first one in the list to satisfy the request.
block_header *best_fit(size_t size)
{
    block_header *current = head;
    block_header *best_fit_block = NULL;

    while (current != NULL)
    {
        if (current->free && current->size >= size)
        {
            if (best_fit_block == NULL || current->size < best_fit_block->size)
            {
                best_fit_block = current;
            }
        }
        current = current->next;
    }
    return best_fit_block;
}

// WORST_FIT
// searches the list for the largest chunk of free space
block_header *worst_fit(size_t size)
{
    block_header *current = head;
    block_header *worst_fit_block = NULL;

    while (current != NULL)
    {
        if (current->free && current->size >= size)
        {
            if (worst_fit_block == NULL || current->size > worst_fit_block->size)
            {
                worst_fit_block = current;
            }
        }
        current = current->next;
    }
    if (worst_fit_block != NULL)
    {
        worst_fit_block->free = 0;
        return worst_fit_block;
    }
    else
    {
        return NULL;
    }
    return worst_fit_block;
}

// FIRST_FIT
// find the first block that is large enough to split this block
block_header *first_fit(size_t size)
{
    block_header *current = head;
    while (current != NULL)
    {
        if (current->free && current->size >= size)
        {
            current->free = 0;
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// NEXT_FIT
// if FIRST_FIT except that the search continues from where it left off at
// at the last request
block_header *next_fit(size_t size)
{
    static block_header *last_alloc = NULL;
    if (last_alloc == NULL)
    {
        last_alloc = head;
    }
    block_header *current = last_alloc;
    do
    {
        /* code */
        if (current->free && current->size >= size)
        {
            current->free = 0;
            last_alloc = current;
            return current;
        }
        current = current->next;
        if (current == NULL)
        {
            current = head;
        }
    } while (current != last_alloc);

    return NULL;
}

void *umalloc(size_t size)
{

    if (size == 0)
    {
        return NULL;
    }

    // Round up size to nearest multiple of 8
    size = (size + 7) & ~7;

    // Add space for block header
    size += sizeof(block_header);

    // check if the request size is greater than any contiguous free space
    size_t max_free_size = 0;
    block_header *current = head;
    while (current != NULL)
    {
        if (current->free && current->size > max_free_size)
        {
            max_free_size = current->size;
        }
        current = current->next;
    }

    if (size > max_free_size)
    {
        return NULL;
    }

    // if trying to allocate 0 bytes, return NULL

    block_header *block;
    if (allo_algo == 1)
    {
        block = best_fit(size);
    }
    else if (allo_algo == 2)
    {
        block = worst_fit(size);
    }
    else if (allo_algo == 3)
    {
        block = first_fit(size);
    }
    else if (allo_algo == 4)
    {
        block = next_fit(size);
    }
    else
    {
        // Default to FIRST_FIT
        block = first_fit(size);
    }

    if (block == NULL)
    {
        return NULL;
    }

    // Split block if it's larger than requested size plus one header
    if (block->size >= size + sizeof(block_header))
    {
        block_header *new_block = (block_header *)((char *)block + size);
        new_block->size = block->size - size;
        new_block->free = 1;
        new_block->next = block->next;
        block->size = size;
        block->next = new_block;
    }

    block->free = 0;

    // Return pointer to memory after block header
    return (void *)(block + 1);
}

int ufree(void *ptr)
{
    if (ptr == NULL)
    {
        return -1;
    }

    // Get block header
    block_header *block = (block_header *)ptr - 1;

    // check if the block is in the list of allocated blocks
    block_header *current = head;
    while (current != NULL)
    {
        if (current == block)
        {
            break;
        }
        current = current->next;
    }

    // if the block is not in the list, reuturn an error or do nothing
    if (current == NULL)
    {
        return -1;
    }

    // Mark block as free
    block->free = 1;

    // Merge with next block if it's free
    if (block->next != NULL && block->next->free)
    {
        block->size += block->next->size;
        block->next = block->next->next;
    }

    // Merge with previous block if it's free
    block_header *prev = head;
    while (prev != NULL && prev->next != block)
    {
        prev = prev->next;
    }
    if (prev != NULL && prev->free)
    {
        prev->size += block->size;
        prev->next = block->next;
    }

    return 0;
}

// print the regions of free memeory to the screen
void umemdump()
{
    block_header *current = head;
    while (current != NULL)
    {
        printf("Block at address %p, size %lu, free %d\n", current, current->size, current->free);
        current = current->next;
    }
}
