#ifndef MEMORY_ALLOC
#define MEMORY_ALLOC
#include <vector>
#include <string>


struct Allocation {
    std::size_t size;
    void* space;
};

// function to allocate memory, changes depending on best/first
void* alloc(std::size_t chunk_size);

void dealloc(void* chunk);

extern std::list<Allocation*> allocatedChunks;
extern std::list<Allocation*> freeChunks;

#endif // MEMORY_ALLOC
