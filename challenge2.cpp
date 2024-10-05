#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#define MEMORY_SIZE 256
#define BAD_BLOCK 'X'

// Simulate the memory space for our allocator
static char memory[MEMORY_SIZE];

// Structure to manage memory blocks
typedef struct Block {
    size_t size;
    bool free;
    struct Block *next;
} Block;

Block *freeList = (Block*)memory;

// Initialize the memory manager
void initializeMemory() {
    freeList->size = MEMORY_SIZE - sizeof(Block);
    freeList->free = true;
    freeList->next = NULL;
}

// Mark random blocks as "bad"
void markBadBlocks(char *memory, size_t size, size_t badBlockCount) {
    srand(time(NULL));
    for (size_t i = 0; i < badBlockCount; i++) {
        size_t randomIndex = rand() % (size - 1);
        memory[randomIndex] = BAD_BLOCK; // Mark as bad block
    }
}

// Allocate memory dynamically, skipping bad blocks
void* myMalloc(size_t size) {
    Block *current = freeList;
    while (current) {
        // Check if the block is free and has enough size
        if (current->free && current->size >= size + sizeof(Block)) {
            // Split the block if it's significantly larger than needed
            if (current->size >= size + sizeof(Block) + 1) {
                Block *newBlock = (Block*)((char*)current + sizeof(Block) + size);
                newBlock->size = current->size - size - sizeof(Block);
                newBlock->free = true;
                newBlock->next = current->next;
                current->next = newBlock;
                current->size = size;
            }
            current->free = false; // Mark the block as used
            return (char*)current + sizeof(Block); // Return pointer to usable memory
        }
        current = current->next;
    }
    return NULL; // No suitable block found
}

// Free the allocated memory
void myFree(void *ptr) {
    if (!ptr) return; // Null check
    Block *block = (Block*)((char*)ptr - sizeof(Block));
    block->free = true; // Mark the block as free
    // Optionally coalesce free blocks (not implemented for simplicity)
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <size_of_allocation>\n", argv[0]);
        return 1;
    }

    // Convert the command-line argument to an integer for allocation size
    size_t allocationSize = atoi(argv[1]);

    // Initialize memory management
    initializeMemory();

    // Mark some blocks as "bad"
    markBadBlocks(memory, MEMORY_SIZE, 10); // Mark 10 bad blocks

    // Simulate memory allocation
    int *array = (int*)myMalloc(allocationSize * sizeof(int));  // Allocate memory for an array of integers
    if (array == NULL) {
        printf("Memory allocation failed.\n");
    } else {
        // Assign values to the array and print them
        for (int i = 0; i < allocationSize; i++) {
            array[i] = i * i;  // Assign square of index
            printf("Array[%d] = %d\n", i, array[i]);
        }

        // Free the allocated memory
        myFree(array);
        printf("Memory successfully freed.\n");
    }

    return 0;
}

