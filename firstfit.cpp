#include <iostream>
#include <list>
#include <cstddef>

struct Allocation {
    std::size_t size;
    void* space;
};

// Global variables to maintain lists of allocated and free chunks
std::list<Allocation> allocatedChunks;
std::list<Allocation> freeChunks;

void* alloc(std::size_t chunk_size) {
    // Search for a chunk in the free list that can satisfy the allocation request
    for (auto it = freeChunks.begin(); it != freeChunks.end(); ++it) {
        if (it->size >= chunk_size) {
            // Found a chunk large enough
            Allocation allocatedChunk = { chunk_size, it->space };
            
            // Add the allocated chunk to the allocated list
            allocatedChunks.push_back(allocatedChunk);

            // Adjust the free chunk's size and space
            it->size -= chunk_size;
            it->space = static_cast<char*>(it->space) + chunk_size;

            return allocatedChunk.space;
        }
    }

    // If no suitable chunk was found, request more memory from the OS using sbrk
    void* newSpace = sbrk(chunk_size);
    if (newSpace == reinterpret_cast<void*>(-1)) {
        // sbrk failed to allocate memory
        std::cerr << "Allocation failed." << std::endl;
        return nullptr;
    }

    // Create an allocation for the new space and add it to the allocated list
    Allocation allocatedChunk = { chunk_size, newSpace };
    allocatedChunks.push_back(allocatedChunk);

    return allocatedChunk.space;
}

int main() {
    // Example usage
    void* allocatedMemory = alloc(64);
    if (allocatedMemory != nullptr) {
        std::cout << "Memory allocated at address: " << allocatedMemory << std::endl;
    }

    return 0;
}
